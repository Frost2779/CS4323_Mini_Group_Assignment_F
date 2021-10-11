#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "clientThread.h"
#include "encryption.h"
#include "linkedList.h"
#include "socketConnection.h"
#include "util.h"

/////////////////////////////////////////////////////
//
// ClientThread.h
//
/////////////////////////////////////////////////////
struct messagePair_t {
    char* sentence;
    int key;
};

static pthread_t pThreadId = -1;
static pthread_mutex_t dataListLock;
static struct linkedList_t* dataList;

void sendSentence(const char* sentence) {
    struct messagePair_t* message = malloc(sizeof(struct messagePair_t));
    message->key = 13;//rand();
    message->sentence = encrypt(sentence, message->key);

    pthread_mutex_lock(&dataListLock);
    linkedListAppend(dataList, message);
    pthread_mutex_unlock(&dataListLock);

    // Eventual waking of the thread here
}

static _Noreturn void* threadLoop(void* data) {
    struct socket_t* socket = createSocket(ADDRESS, PORT);
    connectSocket(socket, 5, 2);

    dataList = mallocLinkedList();

    while(1) {
        sleep(1);

        pthread_mutex_lock(&dataListLock);
        if(dataList->count == 0) {
            pthread_mutex_unlock(&dataListLock);
            continue;
        }

        for(int i = 0; i < dataList->count; i++) {
            struct messagePair_t* message = linkedListPop(dataList);
            char buffer[MAX_TCP_BUFFER_SIZE];

            sprintf(buffer, "%d|%s\n", message->key, message->sentence);
            writeSocket(socket, buffer);

            bzero(buffer, MAX_TCP_BUFFER_SIZE);
            readSocket(socket, buffer);
            printf("Sentence file name: %s\n", buffer);

            free(message);
        }
        pthread_mutex_unlock(&dataListLock);
        // Sleeping the thread in here somewhere
    }
}

pthread_t createClientThread() {
    if(pThreadId != -1) {
        printf("[createClientThread] duplicate creation of client thread\n");
        exit(1);
    }

    pthread_t threadId;
    if(pthread_create(&threadId, NULL, threadLoop, NULL) != 0) {
        printf("[createClientThread] Error on thread creation, %d\n", errno);
        exit(1);
    }

    if (pthread_mutex_init(&dataListLock, NULL) != 0) {
        printf("[createClientThread] mutex lock init failed\n");
        exit(1);
    }

    pThreadId = threadId;
    return threadId;
}

/////////////////////////////////////////////////////
//
// LinkedList.h
//
/////////////////////////////////////////////////////
struct linkedList_t* mallocLinkedList() {
    struct linkedList_t* list = malloc(sizeof(struct linkedList_t));
    list->count = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

void linkedListAppend(struct linkedList_t* list, void* data) {
    assert(list != NULL);
    assert(data != NULL);

    struct node_t* node = malloc(sizeof(struct node_t));
    node->data = data;
    node->next = NULL;

    if(list->tail == NULL) {
        list->head = node;
        list->tail = node;
    }
    else {
        list->tail->next = node;
        list->tail = node;
    }
    list->count++;
}

void* linkedListPop(struct linkedList_t* list) {
    assert(list != NULL);

    if(list->count == 0) {
        return NULL;
    }

    void* data = list->head->data;
    // Only 1 element in list
    if(list->head->next == NULL) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    }
    else {
        struct node_t* head = list->head;
        list->head = list->head->next;
        free(head);
    }
    list->count--;

    return data;
}

/////////////////////////////////////////////////////
//
// Encryption.h
//
/////////////////////////////////////////////////////
#define LANGUAGE_SIZE 53
#define ERROR -1
#define MAX_MSG_SIZE 8192 - 64

static int translateToInt(char ch) {
    int value = ERROR;

    if(ch >= 'A' && ch <= 'Z') {
        value = ch - 'A';
    }
    else if(ch == ' ') {
        value = 26;
    }
    else if(ch >= 'a' && ch <= 'z') {
        value = ch - 'a' + 27;
    }

    assert(value != ERROR);
    return value;
}

static char translateToChar(int value) {
    char ch = ERROR;

    if(value >= 0 && value <= 25) {
        ch = value + 'A';
    }
    else if(value == 26) {
        ch = ' ';
    }
    else if(value >= 27 && value <= 52) {
        ch = (value - 27) + 'a';
    }

    assert(ch != ERROR);
    return ch;
}

char* encrypt(const char* str, int key) {
    assert(str != NULL);
    assert(key > 0);

    unsigned int length = strlen(str);
    char* encrypted = mallocString(length);

    for(int i = 0; i < length; i++) {
        int intValue = translateToInt(str[i]);
        intValue = (intValue + key) % LANGUAGE_SIZE;
        encrypted[i] = translateToChar(intValue);
    }

    return encrypted;
}

char* decrypt(const char* str, int key) {
    assert(str != NULL);
    assert(key > 0);

    unsigned int length = strlen(str);
    char* decrypted = mallocString(length);

    for(int i = 0; i < length; i++) {
        int intValue = translateToInt(str[i]);
        int eval = intValue - key;

        // Bring the negative value forward by LANGUAGE_SIZE until it is positive.
        // Simulates the underflow from 0 - 1 -> LANGUAGE_SIZE
        while(eval < 0) {
            eval = LANGUAGE_SIZE - abs(eval);
        }

        intValue = eval % LANGUAGE_SIZE;
        decrypted[i] = translateToChar(intValue);
    }

    return decrypted;
}

/////////////////////////////////////////////////////
//
// socketConnection.h
//
/////////////////////////////////////////////////////
/*
 * Creates a socket wrapper about C-style socket int
 */
struct socket_t* createSocket(const char* address, int port) {
    struct socket_t* created = malloc(sizeof(struct socket_t));

    // Memory cast hack to remove the const on the struct field to assign value
    *(int *)&created->socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if(created->socketFD == -1) {
        printf("[createSocket] creation of socket failed\n");
        exit(1);
    }

    strcpy((char*)created->address, address);
    *(int *)&created->port = port;

    return created;
}

void bindSocket(const struct socket_t* socket) {
    assert(socket != NULL);

    struct sockaddr_in serverAddress = addressAndPort(socket->address, socket->port);
    if(bind(socket->socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0) {
        printf("[bindSocket] failed to bind to port\n");
        exit(1);
    }
}

void listenSocket(const struct socket_t* socket, int listenCount) {
    assert(socket != NULL);

    if(listen(socket->socketFD, listenCount) != 0) {
        printf("[listenAcceptSocket] failed to listen on socket\n");
        exit(1);
    }
}

struct socket_t* acceptSocket(const struct socket_t* socket) {
    assert(socket != NULL);

    struct sockaddr_in serverAddress = addressAndPort(socket->address, socket->port);

    int acceptedConnection;
    unsigned int length = sizeof(serverAddress);
    if((acceptedConnection = accept(socket->socketFD, (struct sockaddr*)&serverAddress, &length)) < 0) {
        printf("[listenAcceptSocket] failed to accept incoming client connection\n");
        exit(1);
    }

    struct socket_t* clientSocket = malloc(sizeof(struct socket_t));
    *(int*)&clientSocket->socketFD = acceptedConnection;

    return clientSocket;
}

void connectSocket(const struct socket_t* socket, int maxRetryAttempt, int backoffFactor) {
    assert(socket != NULL);
    assert(maxRetryAttempt >= 0);
    assert(backoffFactor >= 0);

    struct sockaddr_in serverAddress = addressAndPort(socket->address, socket->port);

    for(int i = 0; i < maxRetryAttempt; i++) {
        printf("Connecting to TCP server. Attempt %d...\n", i + 1);
        if (connect(socket->socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0) {
            if(i != maxRetryAttempt - 1) {
                int backoff = backoffFactor * (i + 1);
                printf("Failed to connect, backoff waiting %d seconds\n", backoff);
                sleep(backoff);
                continue;
            }
            else {
                printf("[connectSocket] Failed to connect to server after %d retry attempts. Exiting...", maxRetryAttempt);
                exit(1);
            }
        }
        else {
            printf("connected to server successfully\n");
            break;
        }
    }
}

void readSocket(const struct socket_t* socket, char* buffer) {
    assert(socket != NULL);
    assert(buffer != NULL);

    if(read(socket->socketFD, buffer, MAX_MSG_SIZE) == -1) {
        printf("[readSocket] read failed\n");
        exit(1);
    }
}

void writeSocket(const struct socket_t* socket, char* buffer) {
    assert(socket != NULL);
    assert(buffer != NULL);

    if(write(socket->socketFD, buffer, MAX_MSG_SIZE) == -1) {
        printf("[writeSocket] write failed\n");
        exit(1);
    }
}

void freeSocket(struct socket_t* socket) {
    assert(socket != NULL);

    close(socket->socketFD);
    free(socket);
    socket = NULL;
}

/////////////////////////////////////////////////////
//
// util.h
//
/////////////////////////////////////////////////////
char* mallocString(unsigned int length) {
    assert(length > 0);

    char* str = calloc(length + 1, sizeof(char));
    str[length] = '\0';

    return str;
}

struct sockaddr_in addressAndPort(const char* address, int port) {
    assert(address != NULL);
    assert(port > 0);

    struct sockaddr_in socketAddress;
    bzero(&socketAddress, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = inet_addr(address);
    socketAddress.sin_port = htons(port);

    return socketAddress;
}
