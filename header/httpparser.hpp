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
#include <fstream>

#include<sys/types.h>
#include<sys/stat.h>
#include <unistd.h>
#include<fcntl.h> 

#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>

#define static_dir_root "/home/lt/http-server/static"
#define static_file "readme.txt"

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

enum http_parse_stage{
    header=1,
    bodyBoundry,
    bodyHeader,
    fileType,
    val,
    err=-1
};

class http_request{
private:
    string filename;
    int parseStage;
    int bodySize;
    int leftSize;
    string boundary;
public:
    string url;
    http_method method;

    vector< pair<string, string> > header;
    vector<char> body;
    int parseHeaderLine(char* input, int n);
    int parseBodyLine(struct evbuffer *input);

    http_request(){bodySize=0;leftSize=0;url="";method=http_method::UNSUPPORT;parseStage=http_parse_stage::header;}
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
    int sendFile(const http_request& req);
    int sendERR(int err,const http_request& req);
    int recvFile(const http_request& req);
};

#endif