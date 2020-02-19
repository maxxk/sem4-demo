#include "forth.h"
#include "words.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER 32 
// стараемся не использовать в коде «волшебные константы»,
// даём каждой имя
#define STACK 16384

#define MAX(a, b) (a < b ? b : a)

int main(void)
{
    enum forth_result status;
    char *last;

    char buffer[BUFFER+1] = {0}; // 1 символ на завершающий нулевой
    size_t length = 0;
    cell value = 0;
    struct forth forth = {0};
    const struct word *found = NULL;
    struct word *previous = NULL;

    struct word *head = word_create("pop", pop, NULL);
    head = word_create("show", show, head);

    forth_init(&forth, STACK);
    while ((status = read_word(stdin, BUFFER, buffer, &length)) == FORTH_OK) {
        value = strtointptr(buffer, &last, 10);
        if (last - buffer < (int)length) { // считали не всю строку ⇒ не число
            if ((found = word_find(length, buffer, head)) != NULL) {
                found->handler(&forth);
            } else {
                printf("Unknown command\n");
            }
        } else {
            forth_push(&forth, value);
        }
    }
    printf("Complete with status: %d\n", status);
    forth_free(&forth);

    while (head) {
        previous = head;
        head =(struct word*)head->next;
        free(previous);
    }

    return 0;
}
