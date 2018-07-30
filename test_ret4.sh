#!/bin/bash
echo -e $2 | ./out/compiler.out > s/full_compile$1.s
gcc s/full_compile$1.s s/supplement.s -o out/task$1.out
./out/task$1.out
res=$?
if [ $res -ne $3 ]; then { echo -e "\033[31mFAIL\033[m"; exit 1; }; else echo -e "\033[32mPASS\033[m"; fi
