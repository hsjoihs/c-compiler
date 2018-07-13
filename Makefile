.PHONY: test_all_

test_all_:
	./test_all.sh

clean:
	rm out/*.out s/*.s

lexer_check:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	echo '123+456-789' | ./out/compiler.out --lexer-debug 2> res.txt
	diff res.txt expected.txt

lexer_check2:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	echo '7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1-~1)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	!0  <<3)	%3)' | ./out/compiler.out --lexer-debug 2> res2.txt
	diff res2.txt expected2.txt

lexer_check3:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	echo '_1qw12_er345ty = 123, 51 + _1qw12_er345ty' | ./out/compiler.out --lexer-debug 2> res3.txt
	diff res3.txt expected3.txt

full_compile001:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	./test_ret2.sh input001.txt s/full_compile001.s out/task001.out 123 out/compiler.out

print_assembly_check009:
	gcc -Wall print_assembly_check009.c vector.c print_assembly.c -o out/pac009.out
	./test_ret.sh s/print_assembly_check009.s out/pa009.out 174 out/pac009.out

intmap_check:
	gcc -Wall intmap.c intmap_check.c vector.c -o out/intmap_check.out
	./out/intmap_check.out

full_compile002:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	./test_ret2.sh input002.txt s/full_compile002.s out/task002.out 174 out/compiler.out

full_compile003:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	./test_ret2.sh input003.txt s/full_compile003.s out/task003.out 174 out/compiler.out

full_compile004:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	./test_ret2.sh input004.txt s/full_compile004.s out/task004.out 55 out/compiler.out

full_compile005:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	./test_ret2.sh input005.txt s/full_compile005.s out/task005.out 174 out/compiler.out

full_compile006:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	./test_ret2.sh input006.txt s/full_compile006.s out/task006.out 174 out/compiler.out

full_compile007:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	./test_ret2.sh input007.txt s/full_compile007.s out/task007.out 174 out/compiler.out

full_compile008:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	./test_ret2.sh input008.txt s/full_compile008.s out/task008.out 174 out/compiler.out

