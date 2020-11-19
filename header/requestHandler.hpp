#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>
#include "httpparser.hpp"


using namespace std;

void handle_request(const http_request& req, http_response& res);

int checkURL(const http_request& req);

#endif