#ifndef CS4323_MINI_GROUP_ASSIGNMENT_F_LINKEDLIST_H
#define CS4323_MINI_GROUP_ASSIGNMENT_F_LINKEDLIST_H

/*
 * Represents a single node within a linked list
 */
struct node_t {
    void* data;
    struct node_t* next;
};

/*
 * Represents a single-linked list
 */
struct linkedList_t {
    unsigned int count;
    struct node_t* head;
    struct node_t* tail;
};

/*
 * Creates an empty linked list
 *
 * Parameters:
 *     None
 * Returns:
 *     A pointer to the created linked list
 */
struct linkedList_t* mallocLinkedList();

/*
 * Appends data to the end of the linked list
 *
 * Parameters:
 *     list -> the linked list to append to
 *     data -> the data to append to the linked list
 * Returns:
 *     None
 */
void linkedListAppend(struct linkedList_t* list, void* data);

/*
 * Removes the head element from the list and returns the data.
 *
 * Parameters:
 *     list -> the linked list to remove the head of
 * Returns:
 *     A pointer to the data that was contained within the
 *     head node if the list isn't empty. Returns NULL otherwise
 */
void* linkedListPop(struct linkedList_t* list);

/*
 * Peeks at the data held in the head node of the linked list
 *
 * Parameters:
 *     list -> the linked list to peek
 * Returns:
 *     A pointer to the data that was contained within the
 *     head node if the list isn't empty. Returns NULL otherwise
 */
void* linkedListPeek(const struct linkedList_t* list);

/*
 * Frees an allocated linked list.
 *
 * Parameters:
 *     list -> the linked list free
 * Returns:
 *     None
 */
void freeLinkedList(struct linkedList_t* list);

#endif //CS4323_MINI_GROUP_ASSIGNMENT_F_LINKEDLIST_H
