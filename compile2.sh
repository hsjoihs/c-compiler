#!/bin/bash
./out/compiler.out $1.c -DOVERRIDE_STD $2 > self_compile_asm/$1$3.s
