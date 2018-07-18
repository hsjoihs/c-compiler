#!/bin/bash
echo -e $1 | ./$5 > $2
gcc $2 s/supplement.s -o $3
./$3
res=$?
if [ $res -ne $4 ]; then { echo -e "\033[31mFAIL\033[m"; exit 1; }; else echo -e "\033[32mPASS\033[m"; fi
