#include "requestHandler.hpp"

void handle_request(const http_request& req, http_response& res){
    int status;
    // check request
    if((status=checkURL(req))== http_state::OK){
        //send
        res.sendFile(req.url,req);
    }else{
        res.sendERR(status,req);
    }
}


int checkURL(const http_request& req){
    #define lt_find(url, split) find(url.begin(),url.end(), split)!=url.end() 
    if(req.method==http_method::GET || req.method==http_method::POST){
        return http_state::OK;
    }else{
        return http_state::BadRequest;
    }
}