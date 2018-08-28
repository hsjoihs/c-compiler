#!/bin/bash
gcc -E $1.c -DOVERRIDE_STD -U__STDC__ $2 | grep -v "#" | ./out/compiler.out > self_compile_asm/$1$3.s
