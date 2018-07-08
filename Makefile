.PHONY: foo test_task001

foo:
	gcc compiler.c print_assembly.c -o compiler

lexer_check:
	gcc lexer_check.c lexer.c -o lexer_check
	echo '123+456-789' | ./lexer_check > res.txt
	diff res.txt expected.txt

test_task001:
	gcc compiler.c print_assembly.c -o compiler
	echo '123' | ./compiler > test.s
	gcc test.s -o task001
	@	./task001; res=$$?; if [ $$res -ne 123 ]; then { echo FAIL; exit 1; }; else echo PASS; fi

print_assembly_check:
	gcc print_assembly_check.c print_assembly.c -o pac
	./pac > testing.s
	gcc testing.s -o pa
	@	./pa; res=$$?; if [ $$res -ne 174 ]; then { echo FAIL; exit 1; }; else echo PASS; fi

test_task002:
	gcc parse.c print_assembly.c lexer.c -o parse_and_dump.out
	echo '123+56-5' | ./parse_and_dump.out > parse_and_dump.s
	gcc parse_and_dump.s -o task002.out
	@	./task002.out; res=$$?; if [ $$res -ne 174 ]; then { echo FAIL; exit 1; }; else echo PASS; fi
