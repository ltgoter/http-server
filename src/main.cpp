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
        cout<<"check errno to see what error occurred"<<endl;
    } else if (error & BEV_EVENT_TIMEOUT) {
        /* must be a timeout event handle, handle it */
        /* ... */
        cout<<"must be a timeout event handle, handle it"<<endl;
    }
    free(req);
    bufferevent_free(bev);
}

void
do_accept(evutil_socket_t listener, short event, void *arg)
{
    cout<<"do accept"<<endl;
    struct event_base *base = (struct event_base *)arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0) {
        perror("accept");
    } else if (fd > FD_SETSIZE) {
        close(fd);
    } else {
        struct bufferevent *bev;
        http_request* req=new http_request();

        evutil_make_socket_nonblocking(fd);
        bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, readcb, NULL, errorcb, req);
        bufferevent_setwatermark(bev, EV_READ, 0, MAX_BUFFER_SIZE);
        bufferevent_enable(bev, EV_READ|EV_WRITE);
    }
}

void
run(void)
{
    evutil_socket_t listener;
    struct sockaddr_in sin;
    struct event_base *base;
    struct event *listener_event;

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

    listener_event = event_new(base, listener, EV_READ|EV_PERSIST, do_accept, (void*)base);
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