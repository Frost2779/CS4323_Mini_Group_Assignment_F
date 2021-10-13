#include <pthread.h>

#ifndef CS4323_MINI_GROUP_ASSIGNMENT_F_CLIENTTHREAD_H
#define CS4323_MINI_GROUP_ASSIGNMENT_F_CLIENTTHREAD_H

/*
 * Adds a sentence to the queue to be encrypted and sent to the server.
 *
 * Parameters:
 *     The sentence to encrypt and send.
 * Returns:
 *     None
 */
void sendSentence(const char* sentence);

/*
 * Creates a new thread for the encryption and sending of user sentences.
 * An attempt to call this function more than once will result in an assert().
 *
 * Parameters:
 *     None
 * Returns:
 *     A thread handle for the management of the created thread
 */
pthread_t createClientThread();

#endif //CS4323_MINI_GROUP_ASSIGNMENT_F_CLIENTTHREAD_H
