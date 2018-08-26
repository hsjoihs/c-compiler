#!/bin/bash
build() {
	cat test/$1.c | ./out/compiler.out > s/$1.s
	gcc s/$1.s -o out/$1.out
	./out/$1.out || { echo -e "\033[31mFAIL\033[m, at test case" $1 ; exit 1; }
}

build nqueen2
build nqueen3
build nqueen4
build nqueen5
build nqueen6
build nqueen7
build nqueen8
build escape
build duff
