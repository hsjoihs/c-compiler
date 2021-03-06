#!/bin/bash
build() {
	cat test/$1.c | ./out/compiler.out > s/$1.s
	gcc s/$1.s -o out/$1.out -no-pie -Wno-unused-command-line-argument
	./out/$1.out || { echo -e "\033[31mFAIL\033[m, at test case" $1 ; exit 1; }
}

build2() {
	./out/compiler.out test/$1.c > s/$1.s
	gcc s/$1.s -o out/$1.out -no-pie -Wno-unused-command-line-argument
	./out/$1.out || { echo -e "\033[31mFAIL\033[m, at test case" $1 ; exit 1; }
}

build nqueen2
build2 nqueen3
build nqueen4
build2 nqueen5
build nqueen6
build2 nqueen7
build nqueen8
build2 escape
build duff
build2 char_literal
build2 preprocess
build2 preprocess2
build2 preprocess3
build2 preprocess4
build2 preprocess5
build2 small_struct
