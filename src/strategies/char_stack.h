#ifndef CHAR_STACK_H
#define CHAR_STACK_H

#include "minimax.h"

#define CAPACITY (3*(BOARD_SIZE))

struct CharStack {
    char a[CAPACITY];
    int head;
};

void
empty_stack(struct CharStack *s);

int
is_empty_stack(struct CharStack const *s);

void
push_stack(struct CharStack *s, char c);

char
peek_stack(struct CharStack const *s);

char
pop_stack(struct CharStack *s);

#endif //CHAR_STACK_H
