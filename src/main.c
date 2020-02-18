#include "forth.h"
#include <inttypes.h>
#include <stdio.h>

#define BUFFER 32 
int main(void)
{
    enum forth_result status;
    char *last;
    char buffer[BUFFER+1] = {0}; // 1 символ на завершающий нулевой
    size_t length = 0;
    cell value = 0;
    while ((status = read_word(stdin, BUFFER, buffer, &length)) == FORTH_OK) {
        printf("Read: '%.*s'\n", (int)length, buffer);
        value = strtointptr(buffer, &last, 10);
        if (last - buffer < (int)length) { // считали не всю строку ⇒ не число
            printf("Not a number\n");
        } else {
            printf("Number: %" PRIdPTR "\n", value);
        }
    }
    printf("Complete with status: %d\n", status);
    return 0;
}
