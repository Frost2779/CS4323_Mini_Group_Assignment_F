#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "socketConnection.h"
#include "encryption.h"
#include "util.h"

#define LANGUAGE_SIZE 53
#define ERROR -1
#define MAX_MSG_SIZE 8192 - 64

/////////////////////////////////////////////////////
//
// Encryption.h
//
/////////////////////////////////////////////////////
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
        // Simulates the underflow from 0 -> LANGUAGE_SIZE
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
static struct sockaddr_in addressAndPort(const char* address, int port) {
    assert(address != NULL);
    assert(port > 0);

    struct sockaddr_in socketAddress;
    bzero(&socketAddress, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = inet_addr(address);
    socketAddress.sin_port = htons(port);

    return socketAddress;
}

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

struct socket_t* listenAcceptSocket(const struct socket_t* socket) {
    assert(socket != NULL);

    struct sockaddr_in serverAddress = addressAndPort(socket->address, socket->port);

    if(listen(socket->socketFD, 1) != 0) {
        printf("[listenAcceptSocket] failed to listen on socket\n");
        exit(1);
    }

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