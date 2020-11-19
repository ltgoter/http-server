#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H


#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>


#include <cstring>
#include <string>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>

#define static_dir_root "/root/http-server/static/"

using namespace std;

enum http_state{
    OK=200,
    BadRequest=400,
    Forbidden = 403,
    NOTFOUND=404,
    SERVER_ERR=500
};

enum http_method{
    POST=1,
    GET=2,
    UNSUPPORT=-1
};

static void printHeader(const vector< pair<string, string> >& header){
    for(auto itr=header.begin();itr!= header.end();itr++){
        cout<<itr->first<<" : "<<itr->second<<endl;
    }
}

class http_request{
public:
    string url;
    http_method method;

    vector< pair<string, string> > header;
    vector<char> body;
    int parseHeaderLine(char* input, int n);

    http_request(){}
};

class http_response{
private:
    struct evbuffer * output;
    http_response(){}
public:

    vector< pair<string, string>> header;
    vector<char> body;
    
    http_response(struct evbuffer *buff){
        output = buff;
    }
    
    void addHeader(string title, string value);
    // int evbuffer_add_file(struct evbuffer *output, int fd, ev_off_t offset,size_t length);
    int sendFile(string file,const http_request& req);
    int sendERR(int err,const http_request& req);
    void addBody(const char* filename);
    int to_c(char* buffer);
};

#endif