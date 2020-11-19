#include "httpparser.hpp"

int http_request::parseHeaderLine(char* input, int n){
    // string input_s = string(input,n);
    if(n<=1){
        return 0;
    }

    char* split =find(input,input+n, ':');
    if(split == input+n){
        // cout<<"???:"<<n<<endl;
        // cout<<input_s<<endl;
        // split =find(input,input+n, ' ');
        // if(string(input,split-input) =="POST"){
        //     method = http_method::POST;
        // }else if(string(input,split-input) =="GET"){
        //     method = http_method::GET;
        // }else{
        //     method = http_method::UNSUPPORT;
        //     return method;
        // }
        
        // char* split2 =find(split+1,input+n, ' ');
        // url = string(split+1, split2-split-1);
        url = "/";
        method=http_method::GET;
        cout<<"method:"<<method<<endl;
        return method;
        // return -1;
    }
    this->header.push_back(pair<string, string>(string(input,split-input), string(split+2, input+ n-(split+2))));
    return 0;
}

void http_response::addHeader(string title, string value){
    this->header.push_back(pair<string, string>(title, value));
}

void http_response::addBody(const char* filename){

}

int http_response::sendFile(string file, const http_request& req){
    time_t rawtime;
    struct tm * timeinfo;
    char date [128];

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (date,sizeof(date),"%a, %d %b %Y %T %Z",timeinfo);
    // cout<<date<<endl;

    cout<<date<<":send file"<<endl;
    stringstream ss;
    string temp_status;
    ss<<http_state::OK;
    ss>>temp_status;
    string tmp("HTTP/1.1 "+  temp_status +" OK\r\n");
    


    addHeader("Date",date);
    addHeader("Content-Length","45");
    addHeader("Content-Type","application/octet-stream");
    addHeader("Connection", "Keep-Alive");
    addHeader("Server","Apache");
    addHeader("Content-Disposition","attachment;filename=test.txt");
    for(auto itr=header.begin();itr!=header.end();itr++){
        tmp+=itr->first+": "+itr->second+"\r\n";
    }
    tmp+="\r\n";
    tmp+="Error happended,please contact with admin...";
    // memcpy(buffer, tmp.c_str(), tmp.length());
    // cout<<buffer<<endl;
    evbuffer_add(this->output, tmp.c_str(), tmp.length()+2);
    return http_state::BadRequest;
}

int http_response::sendERR(int err, const http_request& req){
    time_t rawtime;
    struct tm * timeinfo;
    char date [128];

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (date,sizeof(date),"%a, %d %b %Y %T %Z",timeinfo);

    cout<<date<<":send err"<<endl;
    stringstream ss;
    string temp_status;
    ss<<http_state::BadRequest;
    ss>>temp_status;
    string tmp("HTTP/1.1 "+  temp_status +" Bad Request\r\n");
    

    // cout<<date<<endl;

    addHeader("Date",date);
    addHeader("Content-Length","45");
    addHeader("Content-Type","text/html");
    addHeader("Connection", "Keep-Alive");
    addHeader("Server","Apache");
    for(auto itr=header.begin();itr!=header.end();itr++){
        tmp+=itr->first+": "+itr->second+"\r\n";
    }
    tmp+="\r\n";
    tmp+="Error happended,please contact with admin...";
    // memcpy(buffer, tmp.c_str(), tmp.length());
    // cout<<buffer<<endl;
    evbuffer_add(this->output, tmp.c_str(), tmp.length());
    return http_state::BadRequest;
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