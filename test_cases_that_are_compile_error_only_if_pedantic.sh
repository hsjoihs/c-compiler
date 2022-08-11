#!/bin/bash
test_failure_when_pedantic() {
	./out/compiler.out $1 -pedantic > /dev/null
	res=$?
	if [ $res -eq 0 ]; then { echo -e "\033[31mFAIL\033[m"; exit 1; }; else echo -e "\033[32mPASS\033[m"; fi
}

run_test_without_pedantic() {
	./out/compiler.out $2 > s/full_compile$1.s
	gcc s/full_compile$1.s -o out/task$1.out -no-pie -Wno-unused-command-line-argument
	./out/task$1.out
	res=$?
	if [ $res -ne $3 ]; then { echo "got:" $res; echo "expected:" $3; echo -e "\033[31mFAIL\033[m, at test case" $1: $2; exit 1; }; else echo -e "\033[32mPASS\033[m"; fi
}

test() {
	echo "With -pedantic:"
	test_failure_when_pedantic $2
	echo "Without -pedantic:"
	run_test_without_pedantic $1 $2 $3
}

test 5555 test/error_if_pedantic/toplevel_semicolon.c 0
test 5556 test/error_if_pedantic/backslash_e.c 27
test 5557 test/error_if_pedantic/binary_literal.c 3