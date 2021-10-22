#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "socketConnection.h"
#include "linkedList.h"
#include "handleClientTraffic.h"

#define maxClients 3
#define maxThreads 3

/*Thread Pool Vars*/
pthread_t poolOfThreads[maxThreads];
pthread_cond_t threadConditional = PTHREAD_COND_INITIALIZER;
pthread_mutex_t queLock = PTHREAD_MUTEX_INITIALIZER;
int numOfConnectedClients = 0;
struct linkedList_t* clientQue;


/********************
Function: threadTask()
Purpose:  function that each thread in the thread pool will be executing.
Return Type: void
********************/
void * threadTask(void *arg) {
    struct socket_t* clientSocket;
    while(1) {
        pthread_mutex_lock(&queLock); // turn on mutex lock
        // if client que is empty, then wait with conditional //
        if (clientQue->count == 0) {
            pthread_cond_wait(&threadConditional, &queLock);
            clientSocket = (struct socket_t*)linkedListPop(clientQue); // get client Socket and remove from que
        } else {
            clientSocket = (struct socket_t*)linkedListPop(clientQue); // get client Socket and remove from que
        }
        pthread_mutex_unlock(&queLock); // unlock mutex
        printf("Handling Client on socket : %d\n", clientSocket->socketFD);

        // Receive from client what thread will be doing //
        handleClientTraffic(clientSocket);
        numOfConnectedClients--;
    }
}

int main() {
    //Server Vars
    struct socket_t *serverSocket, *clientSocket;

    //init linked list to store client connections, declared in
    // global since threads share data segment.
    clientQue = mallocLinkedList();

    //Creating threads in the poolOfThreads array
    for(int i = 0; i < maxThreads; i++) {
        pthread_create(&poolOfThreads[i], NULL, threadTask, NULL);
    }

    //Setup server TCP
    printf("Initializing Server...\n");
    serverSocket = createSocket(ADDRESS, PORT);
    bindSocket(serverSocket);
    listenSocket(serverSocket, maxClients);
    printf("Listening for %d clients on port %d\n", maxClients, PORT);

    //Handle client traffic
    while (1) {
        if ((clientSocket = acceptSocket(serverSocket)) != NULL) {
            printf("...Client connected...\n");
        }

        numOfConnectedClients++;

        if(numOfConnectedClients < 4) {                     //Check to see if there is 3 Clients currently being served.
            //Once a client connects, pass it to a thread from the threadpool
            pthread_mutex_lock(&queLock);              //lock que to prevent race condition
            linkedListAppend(clientQue, clientSocket);        //Add client's socket to the clientQue
            pthread_cond_signal(&threadConditional);   //signal a thread to work

            pthread_mutex_unlock(&queLock);            //unlock client que
        } else {                                       //If there is 3 Clients already connected, Send error message and close socket.
            printf("Max connections reached! closing incoming connection.\n");
            writeSocket(clientSocket, "Server is currently Full!");
            freeSocket(clientSocket);
            numOfConnectedClients--;
        }
    }
    return 0;
}

