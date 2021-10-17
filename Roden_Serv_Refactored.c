#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include "socketConnection.h"
#include "linkedList.h"

#define maxClients 3
#define maxThreads 3
#define convertIntToVoidPtr(i) (void*)(uintptr_t)(i)

/*Thread Pool Vars*/
pthread_t poolOfThreads[maxThreads];
pthread_cond_t threadConditional = PTHREAD_COND_INITIALIZER;
pthread_mutex_t queLock = PTHREAD_MUTEX_INITIALIZER;

struct linkedList_t* clientQue;


/********************
Function: threadTask()
Purpose:  function that each thread in the thread pool will be executing.
Return Type: void
********************/
void * threadTask(void *arg) {
    void* clientSock;
    while(1) {
        //What needs to happen:
        pthread_mutex_lock(&queLock); // turn on mutex lock
        // if client que is empty, then wait with conditional //
       if (clientQue->count == 0) {
            pthread_cond_wait(&threadConditional, &queLock);
            clientSock = linkedListPop(clientQue); // get client Socket and remove from que
        } else {
            clientSock = linkedListPop(clientQue); // get client Socket and remove from que
        }
       pthread_mutex_unlock(&queLock); // unlock mutex
       printf("popped socket : %d\n", *(int *)clientSock);

       // Receive from client what thread will be doing //
        // 1. receive an encrypted message, store into a file, then send back the filename and key
        // 2. receive a file name and send back an encrypted message
    }
}

int main() {
    //Server Vars
    struct socket_t *serverSocket, *clientSocket;
    void *cSock;

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
            cSock = convertIntToVoidPtr(clientSocket->socketFD);
            printf("...Client connected...\n");
        }
        //Once a client connects, pass it to a thread from the threadpool
        pthread_mutex_lock(&queLock);              //lock que to prevent race condition
        linkedListAppend(clientQue, cSock);        //Add client's socket to the clientQue
        pthread_cond_signal(&threadConditional);   //signal a thread to work
        pthread_mutex_unlock(&queLock);            //unlock client que
    }
    return 0;
}