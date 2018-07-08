#!/bin/bash
echo $1 | ./compiler.out > $2
gcc $2 -o $3
./$3
res=$?
if [ $res -ne $4 ]; then { echo FAIL; exit 1; }; else echo PASS; fi