#!/bin/bash
ASAN_OPTIONS=detect_leaks=0 ./out/compiler.out $1.c -DOVERRIDE_STD --ir=ir/_$1$3.txt $2 > self_compile_asm/$1$3.s
