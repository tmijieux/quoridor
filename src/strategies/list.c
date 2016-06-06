#include <stdlib.h>
#include "list.h"

struct List *
empty_list(void)
{
    struct List *list = malloc(sizeof(*list));

    list->frontSentinel = malloc(sizeof(struct ListNode));
    list->frontSentinel->next = malloc(sizeof(struct ListNode));
    list->frontSentinel->isSentinel = 1;
    list->frontSentinel->next->isSentinel = 1;

    list->cursor = list->frontSentinel;
    list->posCursor = 0;
    list->nbElement = 0;

    return list;
}

void
destruct_list(struct List *list)
{
    struct ListNode *node = list->frontSentinel->next;
    struct ListNode *tmp = NULL;

    while (!node->isSentinel) {
	tmp = node->next;
	free(node->element);
	free(node);
	node = tmp;
    }
    free(node);	// backSentinel
    free(list->frontSentinel);
    free(list);
}

void *
get_element_node(struct ListNode *node)
{
    return node->element;
}

struct ListNode *
get_node_list(unsigned int n, struct List *list)
{
    int i, k = n;
    struct ListNode *node = list->frontSentinel;

    if (list->posCursor <= k) {
	k -= list->posCursor;
	node = list->cursor;
    }

    for (i = 0; i < k; i++)
	node = node->next;

    list->cursor = node;
    list->posCursor = n;

    return node;
}

void *
get_element_list(unsigned int n, struct List *list)
{
    return get_element_node(get_node_list(n, list));
}

void
add_element_list(void *element, struct List *list)
{
    struct ListNode *tmp;
    
    tmp = malloc(sizeof(*tmp));
    tmp->element = element;
    tmp->isSentinel = 0;
    tmp->next = list->frontSentinel->next;
    
    list->frontSentinel->next = tmp;
    list->nbElement ++;
}

