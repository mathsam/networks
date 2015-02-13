/*
** server for instant chat. Listen to a port and print out what are being
** recieved
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> //addrinfo, gai_strerror
#include <arpa/inet.h> //inet_ntop
#include <sys/wait.h>
#include <signal.h>
#define PORT "8888"
#define BACKLOG 20
#define MAXDATASIZE 128

void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int sock_listen; // listen on this socket
    int sock_client; // serve client on this socket
    struct addrinfo hints, *serverinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    {
        int rv;
        if ((rv = getaddrinfo(NULL, PORT, &hints, &serverinfo)) != 0){
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }
    }

    {
        struct addrinfo *p;
        for(p=serverinfo; p!=NULL; p=p->ai_next){
            if((sock_listen=socket(p->ai_family,
                                   p->ai_socktype,
                                   p->ai_protocol)) == -1){
            perror("server: socket");
            continue;
            }

            int yes = 1;
            if(setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &yes, 
                          sizeof(int)) == -1){
                perror("setsockopt");
                exit(1);
            }
 
            if(bind(sock_listen, p->ai_addr, p->ai_addrlen)==-1){
                close(sock_listen);
                perror("server: bind");
                continue;
            }
 
            break;
        }
    
        if(p == NULL){
            fprintf(stderr, "server: failed to bind\n");
            return 2;
        }
    }

    freeaddrinfo(serverinfo);

    if(listen(sock_listen, BACKLOG) == -1){
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1){
        struct sockaddr_storage client_addr;
        socklen_t sin_size;
        sin_size = sizeof client_addr;
        int client_fd;
        if((client_fd = accept(sock_listen, (struct sockaddr *)&client_addr, 
                             &sin_size)) == -1){
            perror("accept");
            continue;
        }

        char ip_s[INET6_ADDRSTRLEN];
        inet_ntop(client_addr.ss_family,
                  get_in_addr((struct sockaddr *) &client_addr),
                  ip_s, sizeof ip_s);
        printf("server: got connection from %s\n", ip_s);

        char buf[MAXDATASIZE] = "";            

        if(!fork()){
            close(sock_listen);

            while(strncmp(buf, "quit", 4) != 0){ 
                memset(buf, '\0', MAXDATASIZE);

                int numbytes;
                if((numbytes = recv(client_fd, buf, MAXDATASIZE-1, 0)) == -1){
                    perror("recv");
                    exit(1);
                }

                buf[numbytes] = '\0';
                printf("client: recived\n%s\n", buf);
            }

            close(client_fd);
            exit(0);
        }

        close(client_fd);
 
    }

    return 0;
}
