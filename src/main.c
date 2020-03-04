#include "forth.h"
#include "words.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define STACK 16384
#define RETURN_STACK 4096
#define MEMORY 16384

#define MAX(a, b) (a < b ? b : a)

int main(void)
{
    enum forth_result status;
    struct forth forth = {0};
    forth_init(&forth, STACK, RETURN_STACK, MEMORY);
    words_init(&forth);
    forth.input = stdin;
    status = forth_run(&forth);
    printf("Complete with status: %d\n", status);
    forth_free(&forth);

    return 0;
}
