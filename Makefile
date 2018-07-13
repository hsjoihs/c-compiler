.PHONY: test_all_

test_all_:
	./test_all.sh

clean:
	rm out/*.out s/*.s


print_assembly_check009:
	gcc -Wall print_assembly_check009.c vector.c print_assembly.c -o out/pac009.out
	./test_ret.sh s/print_assembly_check009.s out/pa009.out 174 out/pac009.out

intmap_check:
	gcc -Wall intmap.c intmap_check.c vector.c -o out/intmap_check.out
	./out/intmap_check.out


full_compile:
	gcc -Wall compiler2.c intmap.c vector.c print_assembly.c lexer.c -o out/compiler.out
	./test_ret3.sh '123' s/full_compile001.s out/task001.out 123 out/compiler.out
	./test_ret3.sh '123+56-5' s/full_compile002.s out/task002.out 174 out/compiler.out
	./test_ret3.sh '0x29*3+7*8-5*1' s/full_compile003.s out/task003.out 174 out/compiler.out
	./test_ret3.sh '6*(3+7)-5*1' s/full_compile004.s out/task004.out 55 out/compiler.out
	./test_ret3.sh '043,41*3+07*010-0Xa/(010%(!!1+2))' s/full_compile005.s out/task005.out 174 out/compiler.out
	./test_ret3.sh '7*5,(12,41*3)+7*16/(9,2)-10/(8%3)' s/full_compile006.s out/task006.out 174 out/compiler.out
	./test_ret3.sh '7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1-~1)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	!0  <<3)	%3)' s/full_compile007.s out/task007.out 174 out/compiler.out
	./test_ret3.sh '35,	((	41|	(8   !=     15))*  ((3==3)+2))+((5|2)*(9&10))   -   (10/(8%3))' s/full_compile008.s out/task008.out 174 out/compiler.out

