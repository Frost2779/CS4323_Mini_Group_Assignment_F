#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#define ADDRESS "127.0.0.1"
#define PORT 8080
#define MAX_TCP_BUFFER_SIZE 8192
#define TERMINATION_COMMAND "|exit|"

struct socket_t {
    const int socketFD;
    const char address[20];
    const int port;
};

struct socket_t* createSocket(const char* address, int port);
void bindSocket(const struct socket_t* socket);
struct socket_t* listenAcceptSocket(const struct socket_t* socket);
void connectSocket(const struct socket_t* socket, int maxRetryAttempt, int backoffFactor);
void readSocket(const struct socket_t* socket, char* buffer);
void writeSocket(const struct socket_t* socket, char* buffer);
void freeSocket(struct socket_t* socket);

#endif //SOCKETCONNECTION_H
