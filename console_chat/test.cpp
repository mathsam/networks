#include <iostream>
#include "tcp_sockets.h"
using namespace std;

int main(){

    ClientSocket myclient("127.0.0.1", "8888");
    int const MAXLEN = 1000;
    char buf[MAXLEN];
    myclient.make_connection();
    while(1){
        cin.getline(buf, MAXLEN);
        myclient.push(buf);
    };
}
