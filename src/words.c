#include "forth.h"
#include "words.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

struct word *words_init(void)
{
    struct word *head = word_create("pop", pop, NULL);
    head = word_create("show", show, head);
    return head;
}

void words_free(struct word *head)
{
    struct word *previous = NULL;

    while (head) {
        previous = head;
        head =(struct word*)head->next;
        free(previous);
    }
}

void show(struct forth *forth)
{
    cell *c = forth->sp0;
    while (c < forth->sp) {
        printf("%" PRIdPTR " ", *c);
        c += 1;
    }
    printf("(top)\n");
}

void pop(struct forth *forth)
{
    forth_pop(forth);
}
