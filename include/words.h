#ifndef _WORDS_H
#define _WORDS_H

struct forth;

struct word *words_init(void);
void words_free(struct word *head);

void show(struct forth *forth);
void pop(struct forth *forth);
void _dup(struct forth *forth);
void add(struct forth *forth);
void sub(struct forth *forth);
void mul(struct forth *forth);
void _div(struct forth *forth);
void mod(struct forth *forth);
void swap(struct forth *forth);
void rot(struct forth *forth);
void rot_back(struct forth *forth);
void over(struct forth *forth);

void _true(struct forth *forth);
void _false(struct forth *forth);
void _xor(struct forth *forth);
void _or(struct forth *forth);
void _and(struct forth *forth);
void _not(struct forth *forth);
void lt(struct forth *forth);
void _eq(struct forth *forth);
void within(struct forth *forth);

void forth_exit(struct forth *forth);
void forth_interpreter_stub(struct forth *worth);
void compile_start(struct forth *forth);
void compile_end(struct forth *forth);
void lit(struct forth *forth);

#endif
