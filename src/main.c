#include "forth.h"
#include <stdio.h>

#define BUFFER 16
int main(void)
{
    enum forth_result status;
    char buffer[BUFFER+1] = {0}; // 1 символ на завершающий нулевой
    size_t length = 0;
    while ((status = read_word(stdin, BUFFER, buffer, &length)) == FORTH_OK) {
        printf("Read: '%.*s'\n", (int)length, buffer);
    }
    printf("Complete with status: %d\n", status);
    return 0;
}
