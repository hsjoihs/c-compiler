.PHONY: test_all_

test_all_:
	./test_all.sh

format:
	clang-format -i *.c -style=file

clean:
	rm out/*.out s/*.s


assembly_sandbox:
	gcc -Wall assembly_sandbox.c vector.c print_assembly.c -o out/assembly_sandbox.out
	./test_ret3.sh '' s/assembly_sandbox.s out/assembly_sandbox.out 174 out/assembly_sandbox.out

intmap_check:
	gcc -Wall intmap.c intmap_check.c vector.c -o out/intmap_check.out
	./out/intmap_check.out


full_compile:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	./test_ret3.sh 'return 123;' s/full_compile001.s out/task001.out 123 out/compiler.out
	./test_ret3.sh 'return (123);' s/full_compile010.s out/task010.out 123 out/compiler.out
	./test_ret3.sh 'return ((((123))));' s/full_compile011.s out/task011.out 123 out/compiler.out
	./test_ret3.sh 'return 123+51;' s/full_compile012.s out/task012.out 174 out/compiler.out
	./test_ret3.sh 'return 123+56-5;' s/full_compile002.s out/task002.out 174 out/compiler.out
	./test_ret3.sh 'return 175-(4-3);' s/full_compile013.s out/task013.out 174 out/compiler.out
	./test_ret3.sh 'return 181-4-3;' s/full_compile014.s out/task014.out 174 out/compiler.out
	./test_ret3.sh 'return 0x29*3+7*8-5*1;' s/full_compile003.s out/task003.out 174 out/compiler.out
	./test_ret3.sh 'return 6*(3+7)-5*1;' s/full_compile004.s out/task004.out 55 out/compiler.out
	./test_ret3.sh 'return 43,6*(3+7)-5*1;' s/full_compile015.s out/task015.out 55 out/compiler.out
	./test_ret3.sh 'return 43,6*(3+(4|3))-(5|1)*1;' s/full_compile016.s out/task016.out 55 out/compiler.out
	./test_ret3.sh 'return 043,41*3+07*010-0Xa/(010%(1+2));' s/full_compile017.s out/task017.out 174 out/compiler.out
	./test_ret3.sh 'return 7*5,(12,41*3)+7*16/(9,2)-10/(8%3);' s/full_compile006.s out/task006.out 174 out/compiler.out
	./test_ret3.sh 'return 7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1+2)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	1  <<3)	%3);' s/full_compile018.s out/task018.out 174 out/compiler.out
	./test_ret3.sh 'return 35,	((	41|	(8   !=     15))*  ((3==3)+2))+((5|2)*(9&10))   -   (10/(8%3));' s/full_compile008.s out/task008.out 174 out/compiler.out
	./test_ret3.sh 'return 043,41*3+07*010-0Xa/(010%(!!1+2));' s/full_compile005.s out/task005.out 174 out/compiler.out
	./test_ret3.sh 'return 7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1-~1)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	!0  <<3)	%3);' s/full_compile007.s out/task007.out 174 out/compiler.out
	./test_ret3.sh 'return +174;' s/full_compile019.s out/task019.out 174 out/compiler.out
	./test_ret3.sh 'return -(1-175);' s/full_compile019.s out/task019.out 174 out/compiler.out
	./test_ret3.sh '23; 45+37; ((12-1)*75); return -(1-175);' s/full_compile020.s out/task020.out 174 out/compiler.out
	./test_ret3.sh '23; 45+37; return -(1-175); ((12-1)*75);' s/full_compile021.s out/task021.out 174 out/compiler.out
	./test_ret3.sh 'return (a = b = 9, a = 41*3, 55 - (b = 4) + a);' s/full_compile022.s out/task022.out 174 out/compiler.out
	./test_ret3.sh 'a = b = c = 9; d = 5; a = 41*3; return (c, _q432 = 8, d = 11*5) - (b = 4) + a;' s/full_compile023.s out/task023.out 174 out/compiler.out
	./test_ret3.sh 'return 175^1;' s/full_compile024.s out/task024.out 174 out/compiler.out
	./test_ret3.sh 'return 2 + (1? 100 + 72 : 17);' s/full_compile025.s out/task025.out 174 out/compiler.out
	./test_ret3.sh 'return (0? 234 : 2) + (1? 100 + 72 : 17);' s/full_compile026.s out/task026.out 174 out/compiler.out
	./test_ret3.sh 'return (3, always87() + always87());' s/full_compile027.s out/task027.out 174 out/compiler.out

