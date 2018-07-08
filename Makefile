.PHONY: foo test_task001

foo:
	gcc compiler.c -o compiler

test_task001:
	gcc compiler.c -o compiler
	echo '123' | ./compiler > test.s
	gcc test.s -o task001
	@	./task001; res=$$?; if [ $$res -ne 123 ]; then { echo FAIL; exit 1; }; else echo PASS; fi
