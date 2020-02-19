#include "forth.h"
#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

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

void forth_init(struct forth *forth, size_t stack)
{
    forth->stack = stack;
    forth->sp0 = malloc(forth->stack * sizeof(cell));
    forth->sp = forth->sp0;
}

void forth_free(struct forth *forth)
{
    free(forth->sp0);
    forth->sp = 0;
    forth->sp0 = 0;
}

void forth_push(struct forth *forth, cell number)
{
    assert(forth->sp + 1 - forth->sp0 < (int)forth->stack);
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

#define BUFFER 32 
enum forth_result forth_run(struct forth *forth, struct word *head)
{
    enum forth_result status;
    char *last;
    char buffer[BUFFER+1] = {0}; // 1 символ на завершающий нулевой
    size_t length = 0;
    cell value = 0;
    const struct word *found = NULL;

    while ((status = read_word(stdin, BUFFER, buffer, &length)) == FORTH_OK) {
        value = strtointptr(buffer, &last, 10);
        if (last - buffer < (int)length) { // считали не всю строку ⇒ не число
            if ((found = word_find(length, buffer, head)) != NULL) {
                found->handler(forth);
            } else {
                printf("Unknown command\n");
            }
        } else {
            forth_push(forth, value);
        }
    }
    return status;
}
#undef BUFFER

struct word* word_create(const char *name, function handler,
    const struct word *next)
{
    struct word *result = malloc(sizeof(struct word));
    result->name = name;
    result->handler = handler;
    result->next = next;
    return result;
}

const struct word *word_find(size_t length, char name[length],
    const struct word *head)
{
    while (head) {
        if (!strncmp(head->name, name, length+1)) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}
