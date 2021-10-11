#include <stdio.h>
#include "socketConnection.h"

_Noreturn void* serverListen(void* data) {
    struct socket_t* tcpSocket = createSocket(ADDRESS, PORT);
    bindSocket(tcpSocket);
    listenSocket(tcpSocket, 1);
    struct socket_t* acceptedClient = acceptSocket(tcpSocket);

    while(1) {
        char buffer[MAX_TCP_BUFFER_SIZE];
    }
}

int main() {

}