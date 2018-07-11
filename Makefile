.PHONY: test_all_

test_all_:
	./test_all.sh

clean:
	rm *.out *.s

lexer_check:
	gcc -Wall lexer_check.c lexer.c -o lexer_check.out
	echo '123+456-789' | ./lexer_check.out 2> res.txt
	diff res.txt expected.txt

test_task001:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o compiler.out
	./test_ret.sh '123' test_task001.s task001.out 123 compiler.out

print_assembly_check:
	gcc -Wall print_assembly_check.c print_assembly.c -o pac.out
	./test_ret.sh '' testing.s pa.out 174 pac.out

print_assembly_check2:
	gcc -Wall print_assembly_check2.c print_assembly.c -o pac2.out
	./test_ret.sh '' testing2.s pa2.out 174 pac2.out

print_assembly_check3:
	gcc -Wall print_assembly_check3.c print_assembly.c -o pac3.out
	./test_ret.sh '' testing3.s pa3.out 174 pac3.out

print_assembly_check4:
	gcc -Wall print_assembly_check4.c print_assembly.c -o pac4.out
	./test_ret.sh '' testing4.s pa4.out 174 pac4.out

print_assembly_check5:
	gcc -Wall print_assembly_check5.c print_assembly.c -o pac5.out
	./test_ret.sh '' testing5.s pa4.out 174 pac5.out

intmap_check:
	gcc -Wall intmap.c intmap_check.c vector.c -o intmap_check.out
	./intmap_check.out

test_task002:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o compiler.out
	./test_ret.sh '123+56-5' test_task002.s task002.out 174 compiler.out

test_task003:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o compiler.out
	./test_ret.sh '41*3+7*8-5*1' test_task003.s task003.out 174 compiler.out
	diff test_task003.s testing.s

test_task004:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o compiler.out
	./test_ret.sh '6*(3+7)-5*1' test_task004.s task004.out 55 compiler.out

test_task005:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o compiler.out
	./test_ret.sh '35,41*3+7*8-10/(8%(!!1+2))' test_task005.s task005.out 174 compiler.out
	diff test_task005.s testing2.s

test_task006:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o compiler.out
	./test_ret.sh '7*5,(12,41*3)+7*16/(9,2)-10/(8%3)' test_task006.s task006.out 174 compiler.out
	diff test_task006.s testing3.s

test_task007:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o compiler.out
	./test_ret.sh '7*5 	,	(12,(41   )*(4-(9>8)))+7*(((1-~1)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	!0  <<3)	%3)' test_task007.s task007.out 174 compiler.out
	diff test_task007.s testing4.s

test_task008:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o compiler.out
	./test_ret.sh '35,	((	41|	(8   !=     15))*  ((3==3)+2))+((5|2)*(9&10))   -   (10/(8%3))' test_task008.s task008.out 174 compiler.out
	diff test_task008.s testing5.s

