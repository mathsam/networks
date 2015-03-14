#ifndef _tcp_sockets_h_
#define _tcp_sockets_h_
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdexcept>

#define MAXBUFSIZE 1000 // maximium size of buffer

class ClientSocket{
public:

    ClientSocket(const char *node, const char *port);

    void make_connection();

    char * const recieve();

    void push(char * data);

    ~ClientSocket();

private:

    /// hide copy constructor
    ClientSocket(const ClientSocket & socket){};

    /// hide copy assignement constructor
    ClientSocket & operator=(const ClientSocket & socket){};

    struct addrinfo *client_info_; ///< a linked list

    struct addrinfo *work_info_; ///< the working node in linked list

    int sockfd_;

    char buf_[MAXBUFSIZE];
};


class ConnectionError: public std::runtime_error{
public:
    ConnectionError(const std::string & what_arg):
        std::runtime_error(what_arg)
    {};

    ConnectionError(char * what_arg):
        std::runtime_error(std::string(what_arg))
    {};
};

#endif // _tcp_sockets_h_
