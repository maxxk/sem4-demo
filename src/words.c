#include "forth.h"
#include "words.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

struct word *words_init(void)
{
    const struct word *dup_word;
    const struct word *mul_word;
    const struct word *exit_word;
    const struct word **instructions;

    struct word *head = word_create("interpret", forth_interpreter_stub, NULL);
    head = word_create("pop", pop, head);
    head = word_create("show", show, head);
    head = word_create("drop", pop, head);
    head = word_create("dup", _dup, head);
    dup_word = head;
    head = word_create("+", add, head);
    head = word_create("-", sub, head);
    head = word_create("*", mul, head);
    mul_word = head;
    head = word_create("/", _div, head);
    head = word_create("%", mod, head);
    head = word_create("swap", swap, head);
    head = word_create("rot", rot, head);
    head = word_create("-rot", rot_back, head);
    head = word_create("show", show, head);
    head = word_create("over", over, head);
    head = word_create("true", _true, head);
    head = word_create("false", _false, head);
    head = word_create("xor", _xor, head);
    head = word_create("or", _or, head);
    head = word_create("and", _and, head);
    head = word_create("not", _not, head);
    head = word_create("=", _eq, head);
    head = word_create("<", lt, head);
    head = word_create("within", within, head);
    head = word_create("exit", forth_exit, head);
    exit_word = head;

    instructions = malloc(3 * sizeof(struct word **));
    instructions[0] = dup_word;
    instructions[1] = mul_word;
    instructions[2] = exit_word;
    head = word_create_compiled("square", instructions, head);

    // Проверим второй уровень вызова
    instructions = malloc(3 * sizeof(struct word**));
    instructions[0] = head;
    instructions[1] = head;
    instructions[2] = exit_word;
    head = word_create_compiled("quad", instructions, head);
    return head;
}

void words_free(struct word *head)
{
    struct word *previous = NULL;

    while (head) {
        previous = head;
        head =(struct word*)head->next;
        if (previous->compiled) {
            free(previous->handler.instructions);
        }
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
