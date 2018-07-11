#!/bin/bash
./$4 > $1
gcc $1 -o $2
./$2
res=$?
if [ $res -ne $3 ]; then { echo -e "\033[31mFAIL\033[m"; exit 1; }; else echo -e "\033[32mPASS\033[m"; fi
