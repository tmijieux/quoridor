#include <stdio.h>
#include <stdlib.h>
#include "char_stack.h"

void
empty_stack(struct CharStack *s)
{
    s->head = -1;
}

int
is_empty_stack(struct CharStack const *s)
{
    return (s->head == -1);
}

void
push_stack(struct CharStack *s, char c)
{
    if (s->head == CAPACITY - 1) {
	fprintf(stderr, "push: fullstack\n");
	exit(EXIT_FAILURE);
    }
    s->head += 1;
    s->a[s->head] = c;
}

char
peek_stack(struct CharStack const *s)
{
    if (is_empty_stack(s)) {
	fprintf(stderr, "peek: empty stack \n");
	exit(EXIT_FAILURE);
    }
    return s->a[s->head];
}

char
pop_stack(struct CharStack *s)
{
    char c;
    if (is_empty_stack(s)) {
	fprintf(stderr, "pop: empty stack \n");
	exit(EXIT_FAILURE);
    }
    c = s->a[s->head];
    s->head -= 1;
    return c;
}

