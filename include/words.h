#ifndef _WORDS_H
#define _WORDS_H

struct forth;

struct word *words_init(void);
void words_free(struct word *head);

void show(struct forth *forth);
void pop(struct forth *forth);

#endif
