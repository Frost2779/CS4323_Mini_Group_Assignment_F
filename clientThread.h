#include <pthread.h>

#ifndef CS4323_MINI_GROUP_ASSIGNMENT_F_CLIENTTHREAD_H
#define CS4323_MINI_GROUP_ASSIGNMENT_F_CLIENTTHREAD_H

void sendSentence(const char* sentence);
pthread_t createClientThread();

#endif //CS4323_MINI_GROUP_ASSIGNMENT_F_CLIENTTHREAD_H
