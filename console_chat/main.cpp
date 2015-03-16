#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include "tcp_sockets.h"
#include "string.h"
#include <thread>

#define MAXLEN 1000
using namespace std;


void read_input_client(char *buf, ClientSocket* psocket){
    while(strncmp(buf, "quit", 4) !=0){
        memset(buf, 0, MAXLEN);
        cin.getline(buf, MAXLEN);
        psocket->push(buf);
    }
    return;
}

void read_input_server(char *buf, ServerSocket* psocket){
    while(strncmp(buf, "quit", 4) !=0){
        memset(buf, 0, MAXLEN);
        cin.getline(buf, MAXLEN);
        psocket->push(buf);
    }
    return;
}

int main(int argc, char* argv[]){

    if(argc == 1){
        cout << "Usage: \n\n" 
        << "Client\n\t" << argv[0] << " ip_addr port_num\n\n"
        << "Server\n\t" << argv[0] << " port_num\n";
    }

    char buf[MAXLEN] = {0};

    if(argc == 3){
        ClientSocket myclient(argv[1], argv[2]);
        myclient.make_connection();
     
        thread client_read(read_input_client, buf, &myclient);  

        while(0 != strncmp(buf, "quit", 4)){
           cout << "\t\tThe other says: " << myclient.recieve() << "\n"; 
        }
        exit(0);
        //client_read.join();
        myclient.close_connection();
    }

    if(argc == 2){
        ServerSocket myserver(argv[1]);
        myserver.start_listen();
        myserver.accept_connection();

        thread server_read(read_input_server, buf, &myserver);

        while(0 != strncmp(buf, "quit", 4)){
            cout << "\t\tThe other says: " << myserver.recieve() << "\n";
        } 
        exit(0);
        //server_read.join();
        myserver.close_connection();
    }
        
return 0;
}
