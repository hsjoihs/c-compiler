#!/bin/bash
echo -e $1 | ./out/compiler.out > /dev/null
res=$?
if [ $res -eq 0 ]; then { echo -e "\033[31mFAIL\033[m"; exit 1; }; else echo -e "\033[32mPASS\033[m"; fi
