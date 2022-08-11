#!/bin/bash
test_failure_when_pedantic() {
	echo -e $1 | ./out/compiler.out -pedantic > /dev/null
	res=$?
	if [ $res -eq 0 ]; then { echo -e "\033[31mFAIL\033[m"; exit 1; }; else echo -e "\033[32mPASS\033[m"; fi
}

run_test() {
	echo -e $2 | ./out/compiler.out > s/full_compile$1.s
	gcc s/full_compile$1.s -o out/task$1.out -no-pie -Wno-unused-command-line-argument
	./out/task$1.out
	res=$?
	if [ $res -ne $3 ]; then { echo "got:" $res; echo "expected:" $3; echo -e "\033[31mFAIL\033[m, at test case" $1: $2; exit 1; }; else echo -e "\033[32mPASS\033[m"; fi
}

test_failure_when_pedantic ';;;;;;;;;; int main() { return 0; }'
run_test 5555 ';;;;;;;;;; int main() { return 0; }' 0