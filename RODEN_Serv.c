#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "socketConnection.h"
//#include "michael_somdecerff.c"


#define maxClients 3
#define maxThreads 3



/*Thread Pool Vars*/
pthread_t poolOfThreads[maxThreads];
pthread_cond_t threadConditional = PTHREAD_COND_INITIALIZER;
pthread_mutex_t queLock = PTHREAD_MUTEX_INITIALIZER;


/********************
Function: writeFile()
Purpose:  write encrypted message into file.
Return Type: void 
********************/

//CODE

/********************
Function: readFile()
Purpose:  reads file into a char array.
Return Type: void 
********************/

//CODE

/********************
Function: addClientToQue()
Purpose:  add client to threadpool to handle.
Return Type: void 
********************/
void addClientToQue() {
    pthread_mutex_lock(&queLock);

}


/********************
Function: threadTask()
Purpose:  function that each thread in the thread pool will be executing.
Return Type: void 
********************/
void * threadTask(void *arg) {
    while(1) {
        //int *clientSock;
        bool isQueEmpty = true;

        //Entering critical section, locking thread 
        pthread_mutex_lock(&queLock);

        /* if(numClients > 0) {
            isQueEmpty = false;
        } */


        //unlocking
        pthread_mutex_unlock(&queLock);
        //EXECUTE HANDLING OF CLIENT
        if(!isQueEmpty) {
            //execute function call
        }
    }
    return NULL;
}

/*
int main() {
    //Server Vars
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    int clientLength, servSock, clientSock;

    //Creating threads in the poolOfThreads array
    for(int i = 0; i < maxThreads; i++) {
        pthread_create(&poolOfThreads[i], NULL, threadTask, NULL);
    }

    //Populate serverAddress
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);  //Setting port

    //Create server socket
    if((servSock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        printf("error creating server socket\n");
    } else {
        printf("server socket created\n");
    }

    //Bind Socket to port
    if((bind(servSock,(struct sockaddr *)&serverAddress, sizeof(serverAddress))) < 0 ) {
        printf("error binding server socket\n");
    } else {
        printf("server socket bound\n");
    }

    //Listen and set max number of clients to accept
    if(listen(servSock, maxClients) < 0 ) {
        printf("error listening\n");
    } else {
        printf("Listening for %d clients on port %d\n", maxClients, PORT);
    }
    clientLength = sizeof(struct sockaddr_in);

     //Handle client traffic
    while (1) {
        if(clientSock = accept(servSock, (struct sockaddr *)&clientAddress,(socklen_t*)&clientLength) < 0 ) {
            printf("!!!error connecting client!!!\n");
        } else {
            printf("...client connected...\n");
        }

        //Once a client connects, pass it to a thread from the threadpool
        pthread_mutex_lock(&queLock); //
        
        //ADD Client to cue

        pthread_cond_signal(&threadConditional);
        pthread_mutex_unlock(&queLock);


    }


    return 0;
}
*/
