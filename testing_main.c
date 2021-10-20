#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "clientThread.h"
#include "socketConnection.h"
#include "util.h"

_Noreturn void* serverListen(void* data) {
    struct socket_t* tcpSocket = createSocket(ADDRESS, PORT);
    bindSocket(tcpSocket);
    listenSocket(tcpSocket, 2);

    while(1) {
        struct socket_t* acceptedClient = acceptSocket(tcpSocket);

        while(1) {
            char buffer[MAX_TCP_BUFFER_SIZE];
            readSocket(acceptedClient, buffer);

            if(strstr(buffer, "|exit|") != NULL) {
                printf("Client has closed the socket\n");
                freeSocket(acceptedClient);
                break;
            }

            printf("Received key: %s\n", buffer);

            bzero(buffer, MAX_TCP_BUFFER_SIZE);
            readSocket(acceptedClient, buffer);
            printf("Received sentence: %s\n", buffer);

            sprintf(buffer, "%d", rand());
            writeSocket(acceptedClient, buffer);
        }
    }
}

int main() {
    seedRand();

    pthread_t threadId;
    pthread_create(&threadId, NULL, serverListen, NULL);
    createClientThread();

    sleep(5);

    while(1) {
        char buffer[50];
        printf("Input> ");
        fgets(buffer, 50, stdin);
        // Strip the new line that fgets grabs
        buffer[strcspn(buffer, "\n")] = 0;
        printf("typed: '%s'\n", buffer);
        sendSentence(buffer);
    }
}