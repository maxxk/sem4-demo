all: build/hello

build/hello: src/hello.c src/main.c include/hello.h
	gcc ./src/hello.c ./src/main.c -I ./include -o build/hello \
		-std=c99 -pedantic -Wall -Werror -Wextra -pedantic-errors \
        -Wpointer-arith -Waggregate-return \
        -Wstrict-prototypes -Wmissing-declarations \
        -Wlong-long -Winline -Wredundant-decls \
        -Wcast-align -Wfloat-equal -D__STRICT_ANSI__ \
		-Wbad-function-cast
