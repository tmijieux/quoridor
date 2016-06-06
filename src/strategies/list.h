#ifndef LIST_H
#define LIST_H

struct ListNode {
    void *element;
    struct ListNode *next;
    char isSentinel;
};

struct List {
    struct ListNode *frontSentinel;
    struct ListNode *cursor;
    unsigned int nbElement;
    unsigned int posCursor;
};

struct List *
empty_list(void);

void
destruct_list(struct List *list);

void *
get_element_node(struct ListNode *node);


struct ListNode *
get_node_list(unsigned int n, struct List *list);

void *
get_element_list(unsigned int n, struct List *list);

void
add_element_list(void *element, struct List *list);

#endif //LIST_H
