#include "main.hpp"

void
errorcb(struct bufferevent *bev, short error, void *ctx);

// do unfinish read
void readcb2(struct bufferevent *bev, void *ctx);

void
readcb(struct bufferevent *bev, void *ctx)
{
    cout<<"do readcb"<<endl;
    struct evbuffer *input, *output;
    char *line;
    size_t n;
    int i;
    input = bufferevent_get_input(bev);
    output = bufferevent_get_output(bev);

    // http_request req;
    http_request* req=(http_request*)ctx;
    http_response res(output);
    //read header
    int temp_pan=1;
    while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF))) {
        for (i = 0; i < n; ++i){
            cout<<line[i];
        }
        cout<<endl;
        temp_pan= req->parseHeaderLine(line,n);
        if(temp_pan==0 || temp_pan == -1){//头读取结束
            break;
        }
        free(line);
    }
    
    if(temp_pan == -1){
        res.sendERR(http_state::BadRequest,*req);
        return;
    }else if(req->method == http_method::GET){
        //不需要body部分
        handle_request(*req, res);
        return;
    }

    // read body
    n = req->parseBodyLine(input);

    // printHeader(req.header);

    cout<<"left :" <<n<<endl;
    if(n != 0){
        bufferevent_setcb(bev, readcb2, NULL, errorcb, ctx);
    }else
        handle_request(*req, res);
    // // write buffer for send
    // int len =res.to_c(result);
    // evbuffer_add(output, result, len);
}


void
readcb2(struct bufferevent *bev, void *ctx)
{
    cout<<"do readcb2"<<endl;
    struct evbuffer *input, *output;
    char *line;
    size_t n;
    int i;
    input = bufferevent_get_input(bev);
    output = bufferevent_get_output(bev);

    http_request* req=(http_request*)ctx;
    http_response res(output);

    
    n = req->parseBodyLine(input);

    // printHeader(req.header);

    cout<<"left :" <<n<<endl;
    if(n != 0){
        bufferevent_setcb(bev, readcb2, NULL, errorcb, ctx);
    }else
        handle_request(*req, res);
}


void
errorcb(struct bufferevent *bev, short error, void *ctx)
{
    http_request* req=(http_request*)ctx;
    cout<<"do errorcb"<<endl;
    if (error & BEV_EVENT_EOF) {
        /* connection has been closed, do any clean up here */
        /* ... */
        cout<<"connection has been closed, do any clean up here"<<endl;
    } else if (error & BEV_EVENT_ERROR) {
        /* check errno to see what error occurred */
        /* ... */
        int err = EVUTIL_SOCKET_ERROR();

        fprintf(stderr, "Got an error %d (%s) on the listener. "
            "Shutting down.\n", err, evutil_socket_error_to_string(err));
        // event_base_loopexit(base, NULL);
        cout<<"check errno to see what error occurred"<<endl;
    } else if (error & BEV_EVENT_TIMEOUT) {
        /* must be a timeout event handle, handle it */
        /* ... */
        cout<<"must be a timeout event handle, handle it"<<endl;
    }
    free(req);
    bufferevent_free(bev);
}


struct accept_arg{
    struct event_base *base;
    SSL_CTX *ctx;
};

void
do_accept(evutil_socket_t listener, short event, void *arg)
{
    cout<<"do accept"<<endl;
    accept_arg * arg_s= (struct accept_arg *)arg;
    struct event_base *base = arg_s->base;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0) {
        perror("accept");
    // } else if (fd > FD_SETSIZE) {
    //     close(fd);
    } else {
        
        struct event_base *evbase;
        struct bufferevent *bev;

        SSL_CTX *server_ctx;
        SSL *client_ctx;

        // cout<<"?0"<<endl;
        server_ctx = (SSL_CTX *)arg_s->ctx;
        // cout<<"?22"<<endl;
        client_ctx = SSL_new(server_ctx);
        // cout<<"?33"<<endl;
        evbase = base;


        http_request* req=new http_request();
        // cout<<"?1"<<endl;
        evutil_make_socket_nonblocking(fd);
        // bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
        // cout<<"?2"<<endl;
        bev = bufferevent_openssl_socket_new(evbase, fd, client_ctx,
                                         BUFFEREVENT_SSL_ACCEPTING,
                                         BEV_OPT_CLOSE_ON_FREE);
        // cout<<"?3"<<endl;
        bufferevent_setcb(bev, readcb, NULL, errorcb, req);
        bufferevent_setwatermark(bev, EV_READ, 0, MAX_BUFFER_SIZE);
        bufferevent_enable(bev, EV_READ|EV_WRITE);
    }
}

static SSL_CTX *
evssl_init(void)
{
    SSL_CTX  *server_ctx;

    /* Initialize the OpenSSL library */
    SSL_load_error_strings();
    SSL_library_init();
    /* We MUST have entropy, or else there's no point to crypto. */
    if (!RAND_poll())
        return NULL;

    server_ctx = SSL_CTX_new(SSLv23_server_method());

    if (! SSL_CTX_use_certificate_chain_file(server_ctx, "localhost+1.pem") ||
        ! SSL_CTX_use_PrivateKey_file(server_ctx, "localhost+1-key.pem", SSL_FILETYPE_PEM)) {
        puts("Couldn't read 'pkey' or 'cert' file.  To generate a key\n"
           "and self-signed certificate, run:\n"
           "  openssl genrsa -out pkey 2048\n"
           "  openssl req -new -key pkey -out cert.req\n"
           "  openssl x509 -req -days 365 -in cert.req -signkey pkey -out cert");
        return NULL;
    }
    SSL_CTX_set_options(server_ctx, SSL_OP_NO_SSLv2);

    return server_ctx;
}

void
run(void)
{
    evutil_socket_t listener;
    struct sockaddr_in sin;
    struct event_base *base;
    struct event *listener_event;

    SSL_CTX *ctx;
    ctx = evssl_init();
    if (ctx == NULL)
        return;

    base = event_base_new();
    if (!base)
        return; /*XXXerr*/

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(8099);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(listener);

#ifndef WIN32
    {
        int one = 1;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    }
#endif

    if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return;
    }

    if (listen(listener, 16)<0) {
        perror("listen");
        return;
    }

    accept_arg arg;
    arg.base=base;
    arg.ctx=ctx;
    listener_event = event_new(base, listener, EV_READ|EV_PERSIST, do_accept, (void*)&arg);
    /*XXX check it */
    event_add(listener_event, NULL);

    event_base_dispatch(base);
}

int
main(int c, char **v)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    run();
    return 0;
}