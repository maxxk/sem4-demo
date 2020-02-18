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

#endif
