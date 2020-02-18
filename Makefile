all: build/forth

build/forth: build/forth.c.o build/main.c.o
	gcc $^ -o $@ 

CFLAGS_WARN = -std=c99 -pedantic -Wall -Werror -Wextra -pedantic-errors \
        -Wpointer-arith -Waggregate-return \
        -Wstrict-prototypes -Wmissing-declarations \
        -Wlong-long -Winline -Wredundant-decls \
        -Wcast-align -Wfloat-equal -D__STRICT_ANSI__ \
		-Wbad-function-cast

build/%.c.o: src/%.c build
	gcc -c $<  -o $@ $(CFLAGS_WARN) -I./include -MMD

DEPS := $(shell find build -name *.d)
-include $(DEPS)

build:
	mkdir -p build
