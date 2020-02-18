#ifndef _FORTH_H
#define _FORTH_H

#include <stdint.h>
#include <stdio.h>

typedef intptr_t cell; // 1 ячейка = 1 указатель ~ 1 машинное слово

enum forth_result {
    FORTH_OK,
    FORTH_EOF,
    FORTH_OVERFLOW
};

enum forth_result read_word(FILE* input, size_t size, char buffer[size],
    size_t *read); // количество прочитанных символов

intptr_t strtointptr(const char *str, char **str_end, int base);

struct forth {
    cell *sp; // Stack Pointer — указатель на первую свободную ячейку стека
    cell *sp0; // Stack Pointer 0 — исходный указатель, для освобождения памяти
    size_t stack; // размер стека в ячейках
};

void forth_init(struct forth *forth, size_t stack);
void forth_free(struct forth *forth);
void forth_push(struct forth *forth, cell number);
cell forth_pop(struct forth *forth);

#endif
