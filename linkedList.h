#ifndef CS4323_MINI_GROUP_ASSIGNMENT_F_LINKEDLIST_H
#define CS4323_MINI_GROUP_ASSIGNMENT_F_LINKEDLIST_H

struct node_t {
    void* data;
    struct node_t* next;
};

struct linkedList_t {
    unsigned int count;
    struct node_t* head;
    struct node_t* tail;
};

struct linkedList_t* mallocLinkedList();
void linkedListAppend(struct linkedList_t* list, void* data);
void* linkedListPop(struct linkedList_t* list);

#endif //CS4323_MINI_GROUP_ASSIGNMENT_F_LINKEDLIST_H
