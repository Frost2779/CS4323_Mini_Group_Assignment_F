#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
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

#define SOCKET_EXIT "|exit|"
#define ERROR_MSG "ERROR"
#define RETRIEVE_FILE_COMMAND "|retrieve_file|"
#define SAVE_FILE_COMMAND "|save_file|"

/////////////////////////////////////////////////////
//
// HandleClientTraffic.h
//
/////////////////////////////////////////////////////
void handleClientTraffic(struct socket_t* socket) {
    char buffer[MAX_TCP_BUFFER_SIZE];

    bzero(buffer, MAX_TCP_BUFFER_SIZE);
    readSocket(socket, buffer);

    if(strstr(buffer, RETRIEVE_FILE_COMMAND) != NULL) {
        bzero(buffer, MAX_TCP_BUFFER_SIZE);
        readSocket(socket, buffer);

        FILE* encryptedFile = fopen(buffer, "r");
        if(encryptedFile == NULL) {
            writeSocket(socket, ERROR_MSG);
            freeSocket(socket);
            return;
        }

        char key[20];
        char string[MAX_TCP_BUFFER_SIZE];
        char line[MAX_TCP_BUFFER_SIZE];

        fgets(line, 20, encryptedFile);
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(key, line);

        bzero(line, MAX_TCP_BUFFER_SIZE);
        fgets(line, MAX_TCP_BUFFER_SIZE, encryptedFile);
        strcpy(string, line);

        writeSocket(socket, key);
        writeSocket(socket, string);

        fclose(encryptedFile);
    }
    else if(strstr(buffer, SAVE_FILE_COMMAND) != NULL) {
        bzero(buffer, MAX_TCP_BUFFER_SIZE);
        readSocket(socket, buffer);

        char key[20];
        strcpy(key, buffer);

        bzero(buffer, MAX_TCP_BUFFER_SIZE);
        readSocket(socket, buffer);

        char string[MAX_TCP_BUFFER_SIZE];
        strcpy(string, buffer);

        time_t cTime = time(NULL);
        struct tm tm = *localtime(&cTime);

        char fileName[60];
        // Generate a file name
        sprintf(fileName, "%d-%d-%d_%d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, rand());
        while(access(fileName, F_OK) == 0) {
            // If the file already exists, generate a new one until it doesn't
            sprintf(fileName, "%d-%d-%d_%d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, rand());
        }

        FILE* file = fopen(fileName, "w");

        fprintf(file, "%s\n", key);
        fprintf(file, "%s", string);

        fclose(file);
        writeSocket(socket, fileName);
    }

    freeSocket(socket);
}

/////////////////////////////////////////////////////
//
// RetrieveFile.h
//
/////////////////////////////////////////////////////
void retrieveFile(const char* fileName) {
    assert(fileName != NULL);

    struct socket_t* socket = createSocket(ADDRESS, PORT);
    connectSocket(socket, 5, 2);

    char buffer[MAX_TCP_BUFFER_SIZE];

    // Write command
    bzero(buffer, MAX_TCP_BUFFER_SIZE);
    sprintf(buffer, "%s", RETRIEVE_FILE_COMMAND);
    writeSocket(socket, buffer);

    // Write the file name
    bzero(buffer, MAX_TCP_BUFFER_SIZE);
    sprintf(buffer, "%s", fileName);
    writeSocket(socket, buffer);

    // Read the key
    bzero(buffer, MAX_TCP_BUFFER_SIZE);
    readSocket(socket, buffer);

    // Check if there was an error on the server
    if(strstr(buffer, ERROR_MSG) != NULL) {
        printf("Specified file '%s' does not exist\n", fileName);
        return;
    }

    // parse the key to int
    int key = atoi(buffer);

    // Read the sentence
    bzero(buffer, MAX_TCP_BUFFER_SIZE);
    readSocket(socket, buffer);

    // print file contents
    char* decrypted = decrypt(buffer, key);
    printf("Received file contents for name '%s': %s\n", fileName, decrypted);
    free(decrypted);
}


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
static pthread_cond_t threadConditionalSignal = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t dataListLock;
static struct linkedList_t* dataList;

void sendSentence(const char* sentence) {
    struct messagePair_t* message = malloc(sizeof(struct messagePair_t));
    message->key = rand();
    message->sentence = encrypt(sentence, message->key);

    pthread_mutex_lock(&dataListLock);
    linkedListAppend(dataList, message);
    pthread_mutex_unlock(&dataListLock);

    pthread_cond_signal(&threadConditionalSignal);
}

static _Noreturn void* threadLoop(void* data) {
    dataList = mallocLinkedList();

    while(1) {
        pthread_mutex_lock(&dataListLock);
        if(dataList->count == 0) {
            // Sleep the thread until there is work
            pthread_cond_wait(&threadConditionalSignal, &dataListLock);
        }

        struct socket_t* socket = createSocket(ADDRESS, PORT);
        connectSocket(socket, 5, 2);

        char buffer[MAX_TCP_BUFFER_SIZE];

        // Write command
        bzero(buffer, MAX_TCP_BUFFER_SIZE);
        sprintf(buffer, "%s", SAVE_FILE_COMMAND);
        writeSocket(socket, buffer);

        for(int i = 0; i < dataList->count; i++) {
            struct messagePair_t* message = linkedListPop(dataList);

            // Send key
            bzero(buffer, MAX_TCP_BUFFER_SIZE);
            sprintf(buffer, "%d", message->key);
            writeSocket(socket, buffer);

            // Send sentence
            bzero(buffer, MAX_TCP_BUFFER_SIZE);
            sprintf(buffer, "%s", message->sentence);
            writeSocket(socket, buffer);

            // Receive the file name
            bzero(buffer, MAX_TCP_BUFFER_SIZE);
            readSocket(socket, buffer);
            printf("Sentence file name: '%s'\n", buffer);

            free(message->sentence);
            free(message);
        }

        // Send exit message to the server to close the socket
        bzero(buffer, MAX_TCP_BUFFER_SIZE);
        sprintf(buffer, "%s", SOCKET_EXIT);
        writeSocket(socket, buffer);

        freeSocket(socket);
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
        // No idea what standard practice is here. I know returning null is almost always a bad idea
        return NULL;
    }

    void* data = list->head->data;

    // Only 1 element in list
    if(list->count == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    }
    // More than 1 element in the list
    else {
        struct node_t* head = list->head;
        list->head = list->head->next;
        free(head);
    }
    list->count--;

    return data;
}

void* linkedListPeek(const struct linkedList_t* list) {
    assert(list != NULL);

    if(list->head == NULL) {
        return NULL;
    }
    else {
        return list->head->data;
    }
}

void freeLinkedList(struct linkedList_t* list) {
    assert(list != NULL);

    if(list->count > 0) {
        while (list->head != NULL) {
            struct node_t *next = list->head->next;

            free(list->head->data);
            free(list->head);

            list->head = next;
        }
    }

    free(list);
    list = NULL;
}

/////////////////////////////////////////////////////
//
// Encryption.h
//
/////////////////////////////////////////////////////
#define LANGUAGE_SIZE 54
#define ERROR (-1)
#define MAX_MSG_SIZE (8192 - 64)

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
    else if(ch == '.') {
        value = 53;
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
    else if(value == 53) {
        ch = '.';
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
#define MILLISECONDS_PER_SECOND 1000

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

void connectSocket(const struct socket_t* socket, int maxRetryAttempt, float backoffFactor) {
    assert(socket != NULL);
    assert(maxRetryAttempt >= 0);
    assert(backoffFactor >= 0);

    struct sockaddr_in serverAddress = addressAndPort(socket->address, socket->port);

    for(int i = 0; i < maxRetryAttempt + 1; i++) {
        printf("Connecting to TCP server. Attempt %d...\n", i + 1);
        if (connect(socket->socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0) {
            if(i != maxRetryAttempt) {
                float backoff = backoffFactor * (float)(i + 1);
                printf("Failed to connect, backoff waiting %.2f seconds\n", backoff);
                // For some reason usleep doesn't work here?
                sleep(MILLISECONDS_PER_SECOND * backoff);
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

void seedRand() {
    srand(time(NULL));
}