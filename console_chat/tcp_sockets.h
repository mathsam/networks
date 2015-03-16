#ifndef _tcp_sockets_h_
#define _tcp_sockets_h_
#include <iostream>
#include <string>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXBUFSIZE 1000 // maximium size of buffer
#define BACKLOG 10 // maximum number of waiting connections in the queue

class ClientSocket{
public:

    ClientSocket(const char *node, const char *port);

    void make_connection();

    const char * recieve();

    void push(const char * data);

    void close_connection();

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


class ServerSocket{
public:

    ServerSocket(const char *port);

    void start_listen();

    void stop_listen();

    ///returns incoming connection's ip addr
    const char* accept_connection();

    void close_connection();

    const char * recieve();

    void push(const char * data);

    ~ServerSocket();

private:

    ///hide copy constructor
    ServerSocket(const ServerSocket & socket){};

    ///hide copy assignment constructor
    ServerSocket & operator=(const ServerSocket & socket){};

    struct addrinfo *server_info_;

    struct addrinfo *work_info_;

    int socklisten_fd_;

    int sockaccept_fd_;

    char buf_[MAXBUFSIZE];

    char accept_ip_[INET6_ADDRSTRLEN];
};


class ConnectionError: public std::runtime_error{
public:
    ConnectionError(const std::string & what_arg):
        std::runtime_error(what_arg)
    {};

    ConnectionError(char const * what_arg):
        std::runtime_error(std::string(what_arg))
    {};
};

#endif // _tcp_sockets_h_
