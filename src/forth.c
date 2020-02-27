#include "forth.h"
#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static void run_compiled(struct forth *forth,
    const struct word **instructions, const struct word *stopword);

enum forth_result read_word(FILE* input, size_t size, char buffer[size],
    size_t *length)
{
    size_t l = 0;
    int c;
    while ((c = fgetc(input)) != EOF && l < size) {
        if (isspace(c)) { // считали пробел
            // нужно игнорировать ведущие пробелы
            if (l == 0) { // если мы не считали ни одного символа, пропускаем пробел
                continue;
            } else {
                break;
            }
        }
        buffer[l] = c; // записали не-пробельный символ
        l += 1;
    }

    if (l > 0 && l < size) { // успешно считали
        *length = l;
        buffer[l] = 0; // в С строки должны завершаться нулевыми символами
        return FORTH_OK;
    }

    if (l >= size) {
        return FORTH_OVERFLOW;
    }

    return FORTH_EOF;
}

intptr_t strtointptr(const char *str, char **str_end, int base)
{
    if (INTPTR_MAX > LONG_MAX) {
        return (intptr_t)strtoll(str, str_end, base);
    } else {
        return (intptr_t)strtol(str, str_end, base);
    }
}

void forth_init(struct forth *forth, size_t stack, size_t return_stack,
    size_t memory)
{
    forth->stack = stack;
    forth->sp0 = malloc(forth->stack * sizeof(cell));
    forth->sp = forth->sp0;

    forth->return_stack = return_stack;
    forth->rp0 = malloc(forth->return_stack * sizeof(cell));
    forth->rp = forth->rp0;

    forth->memory = memory;
    forth->mp0 = malloc(forth->memory * sizeof(cell));
    forth->mp = forth->mp0;

    forth->compiling = false;
}

void forth_free(struct forth *forth)
{
    free(forth->sp0);
    forth->sp = 0;
    forth->sp0 = 0;

    free(forth->rp0);
    forth->rp = 0;
    forth->rp0 = 0;

    free(forth->mp0);
    forth->mp = 0;
    forth->mp0 = 0;
}

void forth_push(struct forth *forth, cell number)
{
    assert(forth->sp - forth->sp0 < (int)forth->stack);
    *forth->sp = number;
    forth->sp += 1;
}

cell forth_pop(struct forth *forth)
{
    assert(forth->sp > forth->sp0);
    forth->sp -= 1;
    return *forth->sp;
}

cell *forth_top(struct forth *forth)
{
    assert(forth->sp > forth->sp0);
    return forth->sp - 1;
}

void forth_push_return(struct forth *forth, cell value)
{
    assert(forth->rp - forth->rp0 < (int)forth->return_stack);
    forth->rp[0] = value;
    forth->rp += 1;
}

cell forth_pop_return(struct forth *forth)
{
    assert(forth->rp > forth->rp0);
    forth->rp -= 1;
    return forth->rp[0];
}

void forth_emit(struct forth *forth, cell value)
{
    assert(forth->mp - forth->mp0 < (int)forth->memory);
    forth->mp[0] = value;
    forth->mp += 1;
}

enum forth_result forth_run(struct forth *forth)
{
    enum forth_result status;
    char *last;
    char buffer[FORTH_MAX_WORD+1] = {0}; // 1 символ на завершающий нулевой
    size_t length = 0;
    cell value = 0;
    const struct word *found = NULL;
    const struct word *const stopword =
        word_find(strlen("interpret"), "interpret", forth->latest);
    const struct word *const literal =
        word_find(strlen("lit"), "lit", forth->latest);
    assert(stopword);
    assert(literal);

    forth->ip = &stopword;
    while ((status = read_word(forth->input, FORTH_MAX_WORD, buffer, &length))
            == FORTH_OK) {
        value = strtointptr(buffer, &last, 10);
        if (last - buffer < (int)length) { // считали не всю строку ⇒ не число
            if ((found = word_find(length, buffer, forth->latest)) != NULL) {
                if (forth->compiling && !found->immediate) {
                    forth_emit(forth, (cell)found);
                } else if (!found->compiled) {
                    found->handler.native(forth);
                } else {
                    run_compiled(forth, found->handler.instructions, stopword);
                }
            } else {
                printf("Unknown command\n");
            }
        } else {
            if (forth->compiling) {
                forth_emit(forth, (cell)literal);
                forth_emit(forth, value);
            } else {
                forth_push(forth, value);
            }
        }
    }
    return status;
}

static void run_compiled(struct forth *forth,
    const struct word **instructions, const struct word *const stopword)
{
    forth_push_return(forth, (cell)forth->ip);
    forth->ip = instructions;
    while (forth->ip[0] != stopword) {
        const struct word *instr = forth->ip[0];
        // printf("Running: %s", instr->name);
        forth->ip += 1;

        if (!instr->compiled) {
            // printf(" native\n");
            instr->handler.native(forth);
        } else {
            // printf(" call\n");
            forth_push_return(forth, (cell)forth->ip);
            forth->ip = instr->handler.instructions;
        }
    }
}

struct word* word_create_native(const char *name, function handler,
    const struct word *next)
{
    struct word *result = malloc(sizeof(struct word));
    result->name = name;
    result->compiled = false;
    result->immediate = false;
    result->handler.native = handler;
    result->next = next;
    return result;
}

struct word *word_create_compiled(const char *name, const struct word **instructions,
    const struct word *next)
{
    struct word *result = malloc(sizeof(struct word));
    result->name = name;
    result->compiled = true;
    result->immediate = false;
    result->handler.instructions = instructions;
    result->next = next;
    return result;
}

const struct word *word_find(size_t length, char name[length],
    const struct word *head)
{
    while (head) {
        if (!strncmp(head->name, name, length+1) && !head->hidden) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}
