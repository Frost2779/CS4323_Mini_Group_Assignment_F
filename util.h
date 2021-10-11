#include <stdint.h>

#ifndef UNTITLED_UTIL_H
#define UNTITLED_UTIL_H

/*
 * Allocates an empty string with the specified length on the heap, including a null-terminator at length + 1
 *
 * Paramters:
 *     length -> the desired length of the string
 * Returns:
 *     A char pointer to the first element of the string
 * */
char* mallocString(unsigned int length);
struct sockaddr_in addressAndPort(const char* address, int port);

#endif //UNTITLED_UTIL_H
