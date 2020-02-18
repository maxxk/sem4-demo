#ifndef _FORTH_H
#define _FORTH_H

#include <stdio.h>

enum forth_result {
    FORTH_OK,
    FORTH_EOF,
    FORTH_OVERFLOW
};

enum forth_result read_word(FILE* input, size_t size, char buffer[size],
    size_t *read); // количество прочитанных символов

#endif
