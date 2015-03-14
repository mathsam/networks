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

char* const ClientSocket::recieve(){
    int numbytes;
    numbytes = recv(sockfd_, buf_, MAXBUFSIZE-1, 0);
    if (numbytes == -1)
        throw ConnectionError("fail to recieve");
    buf_[numbytes] = '\0';
    return buf_;
}

void ClientSocket::push(char * data){
    if (send(sockfd_, data, strlen(data), 0) == -1)
        throw ConnectionError("fail to send");
}

ClientSocket::~ClientSocket(){
    freeaddrinfo(client_info_);
    if (sockfd_ != -1)
        close(sockfd_);
}
