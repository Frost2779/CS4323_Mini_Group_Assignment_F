#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#define ADDRESS "127.0.0.1"
#define PORT 8080
#define MAX_TCP_BUFFER_SIZE 8192

/*
 * A struct representing a wrapper around a C-style int socket
 */
struct socket_t {
    const int socketFD;
    const char address[20];
    const int port;
};

/*
 * Creates a new socket.
 *
 * Parameters:
 *     address -> the address of the socket
 *     port -> the port of the socket
 * Returns:
 *     A pointer to the created socket
 */
struct socket_t* createSocket(const char* address, int port);

/*
 * Binds a socket to the port given at its creation
 *
 * Parameters:
 *     socket -> the socket to bind
 * Returns:
 *     None
 */
void bindSocket(const struct socket_t* socket);

/*
 * Sets a socket to listen on its specified address and port
 *
 * Parameters:
 *     socket -> the socket to set up to listen
 *     listenCount -> the maximum number of pending connections the socket can hold in queue
 * Returns:
 *     None
 */
void listenSocket(const struct socket_t* socket, int listenCount);

/*
 * Have a socket accept an incoming connection.
 * This will block until a connection is established
 *
 * Parameters:
 *     socket -> the server socket that will accept an incoming connection
 * Returns:
 *     A pointer to the socket that holds the incoming connection
 */
struct socket_t* acceptSocket(const struct socket_t* socket);

/*
 * Has a socket attempt to connect.
 *
 * Parameters:
 *     socket -> the socket that will try to connect
 *     maxRetryAttempts -> the maximum amount  of times the socket will try to connect before failing
 *     backoffFactor -> scales how long the socket should wait after each failed attempt.
 *                      A value of 1 will have the socket wait one additional second after each failed attempt.
 * Returns:
 *     None
 */
void connectSocket(const struct socket_t* socket, int maxRetryAttempt, float backoffFactor);

/*
 * Has a socket read data. This will block until data is received.
 *
 * Parameters:
 *     socket -> the socket to read from
 *     buffer -> a buffer to put the received data
 * Returns:
 *     None
 */
void readSocket(const struct socket_t* socket, char* buffer);

/*
 * Has a socket write data.
 *
 * Parameters:
 *     socket -> the socket to write to
 *     buffer -> the data that will be writen to the socket
 * Returns:
 *     None
 */
void writeSocket(const struct socket_t* socket, char* buffer);

/*
 * Frees and closes a socket connection
 *
 * Parameters:
 *     socket -> the socket to free
 * Returns:
 *     None
 */
void freeSocket(struct socket_t* socket);

#endif //SOCKETCONNECTION_H
