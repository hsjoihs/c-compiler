.PHONY: test_all_

test_all_:
	./test_all.sh

lexer_check:
	gcc lexer_check.c lexer.c -o lexer_check
	echo '123+456-789' | ./lexer_check > res.txt
	diff res.txt expected.txt

test_task001:
	gcc compiler2.c vector.c print_assembly.c lexer.c -o compiler.out
	echo '123' | ./compiler.out > test_task001.s
	gcc test_task001.s -o task001.out
	@	./task001.out; res=$$?; if [ $$res -ne 123 ]; then { echo FAIL; exit 1; }; else echo PASS; fi

print_assembly_check:
	gcc print_assembly_check.c print_assembly.c -o pac.out
	./pac.out > testing.s
	gcc testing.s -o pa.out
	@	./pa.out; res=$$?; if [ $$res -ne 174 ]; then { echo FAIL; exit 1; }; else echo PASS; fi

test_task002:
	gcc compiler2.c vector.c print_assembly.c lexer.c -o compiler.out
	echo '123+56-5' | ./compiler.out > test_task002.s
	gcc test_task002.s -o task002.out
	@	./task002.out; res=$$?; if [ $$res -ne 174 ]; then { echo FAIL; exit 1; }; else echo PASS; fi
