#include "forth.h"
#include "words.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define STACK 16384
#define RETURN_STACK 4096

#define MAX(a, b) (a < b ? b : a)

int main(void)
{
    enum forth_result status;
    struct forth forth = {0};
    struct word *head = words_init();

    forth_init(&forth, STACK, RETURN_STACK);
    status = forth_run(&forth, head);
    printf("Complete with status: %d\n", status);
    forth_free(&forth);
    words_free(head);

    return 0;
}
