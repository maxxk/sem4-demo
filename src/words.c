#include "forth.h"
#include "words.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void words_init(struct forth *forth)
{
    const struct word *dup_word;
    const struct word *mul_word;
    const struct word *exit_word;
    const struct word *square_word;

    word_create_native(forth, "interpret", forth_interpreter_stub);
    word_create_native(forth, "pop", pop);
    word_create_native(forth, "show", show);
    word_create_native(forth, "drop", pop);
    dup_word = word_create_native(forth, "dup", _dup);
    word_create_native(forth, "+", add);
    word_create_native(forth, "-", sub);
    mul_word = word_create_native(forth, "*", mul);
    word_create_native(forth, "/", _div);
    word_create_native(forth, "%", mod);
    word_create_native(forth, "swap", swap);
    word_create_native(forth, "rot", rot);
    word_create_native(forth, "-rot", rot_back);
    word_create_native(forth, "show", show);
    word_create_native(forth, "over", over);
    word_create_native(forth, "true", _true);
    word_create_native(forth, "false", _false);
    word_create_native(forth, "xor", _xor);
    word_create_native(forth, "or", _or);
    word_create_native(forth, "and", _and);
    word_create_native(forth, "not", _not);
    word_create_native(forth, "=", _eq);
    word_create_native(forth, "<", lt);
    word_create_native(forth, "within", within);
    exit_word = word_create_native(forth, "exit", forth_exit);
    word_create_native(forth, ":", compile_start);
    word_create_native(forth, ";", compile_end);
    forth->latest->immediate = true;
    word_create_native(forth, "lit", lit);

    square_word = word_add(forth, "square", true);
    forth_emit(forth, (cell)dup_word);
    forth_emit(forth, (cell)mul_word);
    forth_emit(forth, (cell)exit_word);
    
    word_add(forth, "quad", true);
    forth_emit(forth, (cell)square_word);
    forth_emit(forth, (cell)square_word);
    forth_emit(forth, (cell)exit_word);
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

void _dup(struct forth *forth) {
    forth_push(forth, *forth_top(forth));
}

void add(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a + b);
}

void sub(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a - b);
}

void mul(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a * b);
}

void _div(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a / b);
}

void mod(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a % b);
}

void swap(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, b);
    forth_push(forth, a);
}

void rot_back(struct forth *forth) {
    cell a, b, c;
    c = forth_pop(forth);
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, c);
    forth_push(forth, a);
    forth_push(forth, b);
}

void rot(struct forth *forth) {
    cell a, b, c;
    c = forth_pop(forth);
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, b);
    forth_push(forth, c);
    forth_push(forth, a);
}

void over(struct forth *forth) {
    assert(forth_top(forth) - 1 >= forth->sp0);
    forth_push(forth, *(forth_top(forth)-1));
}

void _true(struct forth *forth) {
    forth_push(forth, -1);
}

void _false(struct forth *forth) {
    forth_push(forth, 0);
}

void _xor(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a ^ b);
}

void _or(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a | b);
}

void _and(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a & b);
}

void _not(struct forth *forth) {
    cell c = forth_pop(forth);
    forth_push(forth, ~c);
}

void _eq(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a == b ? -1 : 0);
}

void lt(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a < b ? -1 : 0);
}

void within(struct forth *forth) {
    cell a, l, r;
    r = forth_pop(forth);
    l = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, l <= a && a < r ? -1 : 0);
}

void forth_exit(struct forth *forth)
{
    cell r;
    r = forth_pop_return(forth);
    forth->ip = (const struct word **)r;
}

void forth_interpreter_stub(struct forth *forth)
{
    (void)forth;
    printf("Error: return stack underflow (should be handled inside interpreter)\n");
    exit(2);
}

void compile_start(struct forth *forth)
{
    struct word *word;
    size_t length = 0;
    char buffer[FORTH_MAX_WORD+1] = {0};
    
    forth->compiling = true;
    
    read_word(forth->input, FORTH_MAX_WORD, buffer, &length);
    assert(length > 0);
    assert(length == strlen(buffer));

    word = word_add(forth, buffer, true);
    assert(word);
    word->hidden = true;
}

void compile_end(struct forth *forth)
{
    const struct word *exit = word_find(strlen("exit"), "exit", forth->latest);
    assert(exit);
    forth_emit(forth, (cell)exit);
    forth->compiling = false;
    forth->latest->hidden = false;
}

void lit(struct forth *forth)
{
    cell value = *(cell *)forth->ip;
    forth->ip += 1;
    forth_push(forth, value);
}
