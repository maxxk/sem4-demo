#include "forth.h"
#include <ctype.h>

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
