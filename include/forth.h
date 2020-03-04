#ifndef _FORTH_H
#define _FORTH_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef intptr_t cell; // 1 ячейка = 1 указатель ~ 1 машинное слово

enum forth_result {
    FORTH_OK,
    FORTH_EOF,
    FORTH_OVERFLOW
};

#define FORTH_MAX_WORD 32 

enum forth_result read_word(FILE* input, size_t size, char buffer[size],
    size_t *read); // количество прочитанных символов

intptr_t strtointptr(const char *str, char **str_end, int base);

struct forth {
    cell *sp; // Stack Pointer — указатель на первую свободную ячейку стека
    cell *sp0; // Stack Pointer 0 — исходный указатель, для освобождения памяти
    size_t stack; // размер стека в ячейках

    const struct word *const *ip; // Instruction Pointer — указатель на следующую инструкцию

    cell *rp; // Return Pointer — указатель на первую свободную ячейку стека адресов возврата
    cell *rp0; // Return Pointer 0 — 
    size_t return_stack;

    cell *mp; // Memory Pointer — указатель на первую свободную ячейку памяти
    cell *mp0; // Memory Pointer 0
    size_t memory;

    bool compiling;
    FILE *input;
    struct word *latest;
};

void forth_init(struct forth *forth, size_t stack, size_t return_stack,
    size_t memory);
void forth_free(struct forth *forth);
void forth_push(struct forth *forth, cell number);
cell forth_pop(struct forth *forth);
cell *forth_top(struct forth *forth);
// Записать value в память и сдвинуть на следующую ячейку
// указатель на свободную память
void forth_emit(struct forth *forth, cell value);

void forth_push_return(struct forth *forth, cell value);
cell forth_pop_return(struct forth *forth);

enum forth_result forth_run(struct forth *forth);

typedef void (*function)(struct forth*);

union handler {
        function native; // обработчик
        const struct word *instructions;
};

struct word {
    const struct word *next; // следующий элемент списка
    bool compiled;
    bool immediate;
    bool hidden;
    uint8_t length;
    const char name[];
};

const union handler *word_code(const struct word *word);

struct word* word_add(struct forth *forth, const char *name, bool compiled);
struct word *word_create_native(struct forth *forth,
    const char *name, function handler);

// поиск в односвязном списке
const struct word *word_find(size_t length, char name[length],
    const struct word *head);

#endif
