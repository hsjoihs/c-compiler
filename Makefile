.PHONY: test_all_

test_all_:
	./test_all.sh

lexer_check:
	gcc lexer_check.c lexer.c -o lexer_check
	echo '123+456-789' | ./lexer_check > res.txt
	diff res.txt expected.txt

test_task001:
	gcc compiler2.c vector.c print_assembly.c lexer.c -o compiler.out
	./test_ret.sh '123' test_task001.s task001.out 123 compiler.out

print_assembly_check:
	gcc print_assembly_check.c print_assembly.c -o pac.out
	./test_ret.sh '' testing.s pa.out 174 pac.out

test_task002:
	gcc compiler2.c vector.c print_assembly.c lexer.c -o compiler.out
	./test_ret.sh '123+56-5' test_task002.s task002.out 174 compiler.out

test_task003:
	gcc compiler2.c vector.c print_assembly.c lexer.c -o compiler.out
	./test_ret.sh '41*3+7*8-5*1' test_task003.s task003.out 174 compiler.out
	diff test_task003.s testing.s
