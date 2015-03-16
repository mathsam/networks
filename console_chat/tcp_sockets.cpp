#include "tcp_sockets.h"

ClientSocket::ClientSocket(const char *node, const char *port):
  sockfd_(-1)
{
    int status;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    status = getaddrinfo(node, port, &hints, &client_info_);
    if (0 != status){
        freeaddrinfo(client_info_);
        throw ConnectionError(gai_strerror(status));
    }
}

void ClientSocket::make_connection(){
    for(work_info_ = client_info_; work_info_ != NULL; 
        work_info_ = work_info_->ai_next){
        if ((sockfd_ = socket(work_info_->ai_family, work_info_->ai_socktype,
                work_info_->ai_protocol)) == -1){
            continue;
        }

        if (connect(sockfd_, work_info_->ai_addr, work_info_->ai_addrlen) == -1){
            close(sockfd_);
            continue;
        }
        else
            break;
    }

    if (work_info_ == NULL){
        throw ConnectionError("failed to connect");
    }
}

const char* ClientSocket::recieve(){
    int numbytes;
    numbytes = recv(sockfd_, buf_, MAXBUFSIZE-1, 0);
    if (numbytes == -1)
        throw ConnectionError("fail to recieve");
    buf_[numbytes] = '\0';
    return buf_;
}

void ClientSocket::push(const char * data){
    if (send(sockfd_, data, strlen(data), 0) == -1)
        throw ConnectionError("fail to send");
}

void ClientSocket::close_connection(){
    int status;
    status = close(sockfd_);
    if (status == -1)
        throw ConnectionError("fail to close connection");
}

ClientSocket::~ClientSocket(){
    freeaddrinfo(client_info_);
    if (sockfd_ != -1)
        close(sockfd_);
}


ServerSocket::ServerSocket(const char *port):
  socklisten_fd_(-1), sockaccept_fd_(-1)
{
    int status;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    status = getaddrinfo(NULL, port, &hints, &server_info_);
    if (0!= status){
        freeaddrinfo(server_info_);
        throw ConnectionError(gai_strerror(status));
    }
}

void ServerSocket::start_listen(){
    struct addrinfo *p;
    int yes = 1;
    for(p = server_info_; p!=NULL; p = p->ai_next){
        if((socklisten_fd_=socket(p->ai_family,
                                  p->ai_socktype,
                                  p->ai_protocol)) == -1)
            continue;

        if(setsockopt(socklisten_fd_, SOL_SOCKET, SO_REUSEADDR, &yes,
                      sizeof(int)) == -1)
            continue;
        if(bind(socklisten_fd_, p->ai_addr, p->ai_addrlen) == -1)
            continue;

        break;
    }

    if(p==NULL)
        throw ConnectionError("fail to bind to port");

    if(listen(socklisten_fd_, BACKLOG) == -1)
        throw ConnectionError("fail to listen");
}

void ServerSocket::stop_listen(){
    int status;
    status = close(socklisten_fd_);
    if(status == -1)
        throw ConnectionError("fail to stop_listen");
    socklisten_fd_ = -1;
}

static void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

const char* ServerSocket::accept_connection(){
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    if((sockaccept_fd_ = accept(socklisten_fd_,
                                (struct sockaddr *)&client_addr,
                                &sin_size)) == -1)
        throw ConnectionError("fail to accept");

    inet_ntop(client_addr.ss_family,
              get_in_addr((struct sockaddr*) &client_addr),
              accept_ip_, sizeof accept_ip_);
    return accept_ip_;
}

void ServerSocket::close_connection(){
    int status;
    status = close(sockaccept_fd_);
    if(status == -1)
        throw ConnectionError("fail to close connection");
    sockaccept_fd_ = -1;
}

const char* ServerSocket::recieve(){
    int numbytes;
    if((numbytes = recv(sockaccept_fd_, buf_, MAXBUFSIZE-1, 0)) ==  -1)
        throw ConnectionError("fail to recieve");

    buf_[numbytes] = '\0';
    return buf_;
}

void ServerSocket::push(const char *data){
    if(send(sockaccept_fd_, data, strlen(data), 0) == -1)
        throw ConnectionError("fail to send");
}

ServerSocket::~ServerSocket(){
    freeaddrinfo(server_info_);
    if(socklisten_fd_ != -1)
        close(socklisten_fd_);
    if(sockaccept_fd_ != -1)
        close(sockaccept_fd_);
}
