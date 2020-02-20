#include "forth.h"
#include <assert.h>

void test_forth_init(void);
void test_forth_push_pop(void);

void test_forth_init(void)
{
    struct forth f;
    forth_init(&f, 10);
    assert(f.sp0 != 0);
    assert(f.sp != 0);
    assert(f.stack == 10);
    forth_free(&f);
}

void test_forth_push_pop(void)
{
    struct forth f;
    cell c;
    forth_init(&f, 1);
    forth_push(&f, -3);
    assert(f.sp - f.sp0 == 1);
    c = forth_pop(&f);
    assert(f.sp == f.sp0);
    assert(c == -3);
}
