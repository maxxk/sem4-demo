#include "forth.h"
#include "words.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct word *words_init(void)
{
    const struct word *dup_word;
    const struct word *mul_word;
    const struct word *exit_word;
    const struct word **instructions;

    struct word *head = word_create_native("interpret", forth_interpreter_stub, NULL);
    head = word_create_native("pop", pop, head);
    head = word_create_native("show", show, head);
    head = word_create_native("drop", pop, head);
    head = word_create_native("dup", _dup, head);
    dup_word = head;
    head = word_create_native("+", add, head);
    head = word_create_native("-", sub, head);
    head = word_create_native("*", mul, head);
    mul_word = head;
    head = word_create_native("/", _div, head);
    head = word_create_native("%", mod, head);
    head = word_create_native("swap", swap, head);
    head = word_create_native("rot", rot, head);
    head = word_create_native("-rot", rot_back, head);
    head = word_create_native("show", show, head);
    head = word_create_native("over", over, head);
    head = word_create_native("true", _true, head);
    head = word_create_native("false", _false, head);
    head = word_create_native("xor", _xor, head);
    head = word_create_native("or", _or, head);
    head = word_create_native("and", _and, head);
    head = word_create_native("not", _not, head);
    head = word_create_native("=", _eq, head);
    head = word_create_native("<", lt, head);
    head = word_create_native("within", within, head);
    head = word_create_native("exit", forth_exit, head);
    exit_word = head;
    head = word_create_native(":", compile_start, head);
    head = word_create_native(";", compile_end, head);
    head->immediate = true;

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

static char* strdup_n(size_t length, const char original[length]);

void compile_start(struct forth *forth)
{
    struct word *word;
    size_t length = 0;
    char buffer[FORTH_MAX_WORD+1] = {0};
    
    forth->compiling = true;
    
    read_word(forth->input, FORTH_MAX_WORD, buffer, &length);
    assert(length > 0);

    word = word_create_compiled(strdup_n(length, buffer),
        (const struct word**)forth->mp, forth->latest);
    assert(word);
    forth->latest = word;
}

void compile_end(struct forth *forth)
{
    const struct word *exit = word_find(strlen("exit"), "exit", forth->latest);
    assert(exit);
    forth_emit(forth, (cell)exit);
    forth->compiling = false;
}

static char* strdup_n(size_t length, const char original[length])
{
    char *result = malloc(length+1);
    strncpy(result, original, length);
    result[length] = '\0';
    return result;
}
