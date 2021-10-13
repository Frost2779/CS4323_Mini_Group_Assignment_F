#include <stdint.h>

#ifndef UNTITLED_UTIL_H
#define UNTITLED_UTIL_H

/*
 * Allocates an empty string with the specified length, including a null-terminator at length + 1
 *
 * Parameters:
 *     length -> the desired length of the string
 * Returns:
 *     A char pointer to the first element of the string
 * */
char* mallocString(unsigned int length);

/*
 * Formats an address and port to a sockaddr_in with default paramaters
 *
 * Parameters:
 *     address -> the desired address
 *     port -> the desired port
 * Returns:
 *     A sockaddr_in struct containing the address and port
 * */
struct sockaddr_in addressAndPort(const char* address, int port);

/* Seeds rand() with the current system time
 *
 * Parameters:
 *   none
 * Returns:
 *   none
 * */
void seedRand();

#endif //UNTITLED_UTIL_H
