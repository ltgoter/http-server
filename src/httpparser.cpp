#include "httpparser.hpp"

string num2str(int num){
    stringstream ss;
    ss<<num;
    return ss.str();
}

int http_request::parseHeaderLine(char* input, int n){
    // string input_s = string(input,n);
    // cout<<input_s<<endl;
    if(n<=1){
        parseStage=http_parse_stage::bodyBoundry;
        return 0;
    }

    char* split =find(input,input+n, ':');
    if(split == input+n){
        // cout<<"???:"<<n<<endl;
        split =find(input,input+n, ' ');
        if(string(input,split-input) =="POST"){
            method = http_method::POST;
        }else if(string(input,split-input) =="GET"){
            method = http_method::GET;
        }else{
            method = http_method::UNSUPPORT;
            parseStage=http_parse_stage::err;
            return method;
        }
        
        char* split2 =find(split+1,input+n, ' ');
        url = string(split+1, split2-split-1);
        // url = "/";
        // method=http_method::GET;
        cout<<"method:"<<method<<endl;
        cout<<"url:"<<url<<endl;
        if(url=="/"){
            url = static_file;
        }else{
            url = url.substr(1, url.length()-1);
        }
        return method;
        // return -1;
    }
    this->header.push_back(pair<string, string>(string(input,split-input), string(split+2, input+ n-(split+2))));
    if(string(input,split-input) == "Content-Length"){
        leftSize=this->bodySize=atoi(string(split+2, input+ n-(split+2)).c_str());
    }else if(string(input,split-input) == "Content-Type"){
        if(n>34 && string(input+14,input+33) == "multipart/form-data"){
            split =find(input,input+n, '=');
            boundary = string(split+ 1, input+n - (split+1));
        }
    }
    return method;
}

int http_request::parseBodyLine(struct evbuffer *input){

    ofstream outfile;
    char* split;
    char *line;
    size_t i,n;
    
    while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF))) {
        // cout<<"stage"<<parseStage<<endl;
        string input_s = string(line,n);
        leftSize-=n+1;//换行也算一个
        for (i = 0; i < n; ++i){
            cout<<line[i];
        }
        cout<<endl;
        if(n<=0)
            continue;
        if(parseStage == http_parse_stage::bodyBoundry){
            if(string(line,n) == "--"+boundary ){
                parseStage = http_parse_stage::bodyHeader;
            }else{
                // cout<<"unmatch"<<endl<<string(line,n)<<endl<<"--"+boundary<<endl;
            }
        }else if(parseStage == http_parse_stage::bodyHeader){
            if(string(line, 30) == "Content-Disposition: form-data"){
                int beg=0;
                if((beg=string(line,n).find("filename="))!=n){
                    filename = string(line+beg+10, n-2 - (beg+10));
                    // outfile.open(static_dir_root+("/"+ filename), ios::out | ios::trunc );
                    cout<<"filename w:"<< static_dir_root+("/"+ filename)<<endl;

                    parseStage = http_parse_stage::fileType;
                }else{
                    return -1;
                }

            }else{
                parseStage == http_parse_stage::bodyBoundry;
            }
        }else if(parseStage == http_parse_stage::fileType){
            parseStage = http_parse_stage::val;
        }else if(parseStage == http_parse_stage::val){
            if(string(line,n) == "--"+boundary ){
                parseStage = http_parse_stage::bodyBoundry;
            }else{
                if(!outfile.is_open())
                    outfile.open(static_dir_root+("/"+ filename), ios::out );
                outfile<<line<<endl;
            }
        }
        free(line);
    }
    if(outfile.is_open()){
        outfile.close();
    }
    return leftSize;
}

void http_response::addHeader(string title, string value){
    this->header.push_back(pair<string, string>(title, value));
}

int http_response::sendFile(const http_request& req){
    time_t rawtime;
    struct tm * timeinfo;
    char date [128];

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (date,sizeof(date),"%a, %d %b %Y %T %Z",timeinfo);
    // cout<<date<<endl;

    cout<<date<<":send file:"<<req.url<<endl;
    string tagFile = req.url;
    stringstream ss;
    string temp_status;
    ss<<http_state::OK;
    ss>>temp_status;

    int fd = open((string(static_dir_root)+"/"+tagFile).c_str(), O_RDONLY);
    if(fd== -1){
        cout<<"ERROR:!!NOT FOUND"<<endl;
        sendERR(http_state::NOTFOUND,req);
        return -1;
    }

    int len = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    string tmp("HTTP/1.1 "+  temp_status +" OK\r\n");
    
    addHeader("Date",date);
    addHeader("Content-Length", num2str(len));
    addHeader("Content-Type","application/octet-stream");
    addHeader("Connection", "Keep-Alive");
    addHeader("Server","Apache");
    addHeader("Content-Disposition","attachment;filename="+tagFile);
    for(auto itr=header.begin();itr!=header.end();itr++){
        tmp+=itr->first+": "+itr->second+"\r\n";
    }
    tmp+="\r\n";
    // tmp+="Error happended,please contact with admin...";
    // memcpy(buffer, tmp.c_str(), tmp.length());
    // cout<<buffer<<endl;
    evbuffer_add(this->output, tmp.c_str(), tmp.length());

    // int evbuffer_add_file(struct evbuffer *output, int fd, ev_off_t offset,size_t length);
    evbuffer_add_file(output, fd, 0, len);
    return http_state::OK;
}

int http_response::sendERR(int err, const http_request& req){
    time_t rawtime;
    struct tm * timeinfo;
    char date [128];

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (date,sizeof(date),"%a, %d %b %Y %T %Z",timeinfo);

    cout<<date<<":send err: "<<err<<endl;
    
    string msg;
    switch (err)
    {
    case http_state::NOTFOUND :
        msg=" NOT FOUND\r\n";;
        break;
    
    case http_state::BadRequest :
        msg=" BadRequest\r\n";
        break;
    
    case http_state::Forbidden :
        msg=" Forbidden\r\n";
        break;

    case http_state::SERVER_ERR :
        msg=" SERVER_ERR\r\n";
        break;

    default:
        msg=" WHATERROR\r\n";
        break;
    }
    string tmp("HTTP/1.1 "+  num2str(err) + msg);

    // cout<<date<<endl;

    addHeader("Date",date);
    addHeader("Content-Length",num2str(msg.length()));
    addHeader("Content-Type","text/html");
    addHeader("Connection", "Keep-Alive");
    addHeader("Server","Apache");
    for(auto itr=header.begin();itr!=header.end();itr++){
        tmp+=itr->first+": "+itr->second+"\r\n";
    }
    tmp+="\r\n";
    tmp+=msg;
    // memcpy(buffer, tmp.c_str(), tmp.length());
    // cout<<buffer<<endl;
    evbuffer_add(this->output, tmp.c_str(), tmp.length());
    return err;
}


int http_response::recvFile(const http_request& req){
    time_t rawtime;
    struct tm * timeinfo;
    char date [128];

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (date,sizeof(date),"%a, %d %b %Y %T %Z",timeinfo);

    cout<<date<<":recv file: "<<req.url<<endl;
    
    string msg;
    msg="recv succeedly\r\n";;

    string tmp("HTTP/1.1 "+  num2str(http_state::OK) + " OK");

    // cout<<date<<endl;

    addHeader("Date",date);
    addHeader("Content-Length",num2str(msg.length()));
    addHeader("Content-Type","text/html");
    addHeader("Connection", "Keep-Alive");
    addHeader("Server","Apache");
    for(auto itr=header.begin();itr!=header.end();itr++){
        tmp+=itr->first+": "+itr->second+"\r\n";
    }
    tmp+="\r\n";
    tmp+=msg;
    // memcpy(buffer, tmp.c_str(), tmp.length());
    // cout<<buffer<<endl;
    evbuffer_add(this->output, tmp.c_str(), tmp.length());
    return http_state::OK;
}
