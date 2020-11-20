#include "requestHandler.hpp"

void handle_request(const http_request& req, http_response& res){
    int status;
    // check request
    if((status=checkURL(req))== http_state::OK){
        if(req.method == http_method::GET){
            //send
            res.sendFile(req);
        }else if(req.method == http_method::POST){
            res.recvFile(req);
        }else{
            // cout<<"method err~"<<endl;
            res.sendERR(http_state::BadRequest,req);
        }
    }else{
        res.sendERR(status,req);
    }
}


int check_file(string tag) //文件夹地址，文件列表
{
    DIR *dirp;
    struct dirent *dp;
    dirp = opendir(static_dir_root);
    if(dirp == NULL){
        cout<<"static_dir_root not found!!!"<<endl;
        exit(-1);
    }
    while ((dp = readdir(dirp)) != NULL) {
        if(dp->d_name ==tag ){
            closedir(dirp);
            return 1;
        }
    }
    closedir(dirp);
    return 0;
}

int checkURL(const http_request& req){
    #define lt_find(url, split) find(url.begin(),url.end(), split)!=url.end() 
    if(req.method==http_method::GET){
        if(check_file(req.url)==1)
            return http_state::OK;
        else
            return http_state::NOTFOUND;
    } else if(req.method==http_method::POST){
        return http_state::OK;
    }else{
        return http_state::BadRequest;
    }
}