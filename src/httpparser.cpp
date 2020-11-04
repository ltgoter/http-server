#include "httpparser.hpp"

int http_request::parseHeaderLine(char* input, int n){
    string input_s = string(input,n);
    char* split =find(input,input+n, ':');
    if(split == input+n){
        return -1;
    }
    this->header.push_back(pair<string, string>(string(input,split-input), string(split+2, input+ n-(split+2))));
    return 0;
}

void http_response::addHeader(string title, string value){
    this->header.push_back(pair<string, string>(title, value));
}

void http_response::addBody(const char* filename){

}

int http_response::to_c(char* buffer){
    string tmp("HTTP/1.1 200 OK\r\n");
    
    time_t rawtime;
    struct tm * timeinfo;
    char date [128];

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (date,sizeof(date),"%a, %d %b %Y %T %Z",timeinfo);
    cout<<date<<endl;

    addHeader("Date",date);
    addHeader("Content-Length","7");
    addHeader("Content-Type","text/html");
    addHeader("Connection", "Keep-Alive");
    addHeader("Server","Apache");
    for(auto itr=header.begin();itr!=header.end();itr++){
        tmp+=itr->first+": "+itr->second+"\r\n";
    }
    tmp+="\r\n";
    tmp+="body...";
    memcpy(buffer, tmp.c_str(), tmp.length());
    // cout<<buffer<<endl;
    return tmp.length();
}