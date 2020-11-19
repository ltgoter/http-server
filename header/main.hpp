#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <stdlib.h>

using namespace std;

// int main(){
    

//     cout<<"server start"<<endl;

//     return 0;W
// }
/* For sockaddr_in */
#include <netinet/in.h>
/* For socket functions */
#include <sys/socket.h>
/* For fcntl */
#include <fcntl.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "httpparser.hpp"
#include "requestHandler.hpp"

#define MAX_BUFFER_SIZE 1024*1024*128

#define MAX_RES_BUFFER 1024*1024

void do_read(evutil_socket_t fd, short events, void *arg);
void do_write(evutil_socket_t fd, short events, void *arg);



#endif