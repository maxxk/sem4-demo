all: build/forth build/test

CFLAGS := -O0 -g

build/forth: build/forth.c.o build/main.c.o build/words.c.o
	gcc $^ -o $@ $(CFLAGS) -g

CFLAGS_PROFILE = -O0 # -fprofile-arcs -ftest-coverage
# Для тестов и для финальной программы мы используем разные флаги
# Если мы запустим make, forth.c.o скомпилируется без флагов профилирования
build/test: build/forth.c-profile.o build/forth.test.c-profile.o build/test.c-profile.o
	gcc $^ -o $@ $(CFLAGS_PROFILE)

CFLAGS_WARN = -std=c99 -pedantic -Wall -Werror -Wextra -pedantic-errors \
        -Wpointer-arith -Waggregate-return \
        -Wstrict-prototypes -Wmissing-declarations \
        -Wlong-long -Winline -Wredundant-decls \
        -Wcast-align -Wfloat-equal -D__STRICT_ANSI__ \
		-Wbad-function-cast

build/%.c.o: src/%.c | build
	gcc -c $<  -o $@ $(CFLAGS_WARN) -I./include -MMD $(CFLAGS)

build/%.c-profile.o: src/%.c | build
	gcc -c $<  -o $@ $(CFLAGS_WARN) $(CFLAGS_PROFILE) -I./include -MMD -g

DEPS := $(shell find build -name *.d)
-include $(DEPS)

build:
	mkdir -p build

check: build/test
	build/test

coverage: build/test
	#cd build && gcovr -r .. --html --html-details -o index.html
	kcov build/coverage build/test

clean:
	rm -rf build/*
