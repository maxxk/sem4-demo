#include <stdio.h>

long b0(long a) { return a + 1; }

long b1(long a) { return b0(b0(b0(b0(a)))); }
long b2(long a) { return b1(b1(b1(b1(a)))); }
long b3(long a) { return b2(b2(b2(b2(a)))); }
long b4(long a) { return b3(b3(b3(b3(a)))); }
long b5(long a) { return b4(b4(b4(b4(a)))); }
long b6(long a) { return b5(b5(b5(b5(a)))); }
long b7(long a) { return b6(b6(b6(b6(a)))); }
long b8(long a) { return b7(b7(b7(b7(a)))); }
long b9(long a) { return b8(b8(b8(b8(a)))); }
long b10(long a) { return b9(b9(b9(b9(a)))); }
long b11(long a) { return b10(b10(b10(b10(a)))); }
long b12(long a) { return b11(b11(b11(b11(a)))); }
long b13(long a) { return b12(b12(b12(b12(a)))); }

int main(void) {
    printf("%ld\n", b13(0));
    return 0;
}
