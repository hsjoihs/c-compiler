UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CCFLAGS += -D LINUX
endif
ifeq ($(UNAME_S),Darwin)
    CCFLAGS += -D OSX
endif

notest2:
	gcc -Wall -Wextra -DOVERRIDE_STD -g std.c codegen.c alignment.c parse_analyze_toplevel.c parse_analyze_statement.c codegen_expression.c main.c vector.c typecheck_expression.c parse_expression.c error.c type.c parse_type.c map.c print_x86_64.c $(CCFLAGS) lexer.c -o out/compiler.out

test_all_:
	make supplement
	make assembly_sandbox
	make test_valid
	make verify_typeparse
	make compile_files
	make check_error


clean:
	rm out/*.out s/*.s

supplement:
	gcc supplement.c -S -o s/supplement.s

verify_typeparse:
	clang -Wall -Wextra -Wimplicit-fallthrough vector.c verifier/typeparse_checker.c lexer.c type.c parse_type.c error.c -o out/typeparse_check.out
	./out/typeparse_check.out

assembly_sandbox:
	gcc -Wall -Wextra assembly_sandbox.c print_x86_64.c $(CCFLAGS) -o out/assembly_sandbox.out
	echo -e '' | ./out/assembly_sandbox.out > s/assembly_sandbox.s
	gcc s/assembly_sandbox.s s/supplement.s -o out/sandbox.out
	./out/sandbox.out || if [ $$? -ne 174 ]; then { echo "\n\033[31mFAIL\033[m"; exit 1; }; else echo "\n\033[32mPASS\033[m"; fi

compile_files:
	make supplement
	make notest2
	cat test/nqueen2.c | ./out/compiler.out > s/nqueen2.s
	gcc s/nqueen2.s -o out/nqueen2.out
	./out/nqueen2.out
	cat test/nqueen3.c | ./out/compiler.out > s/nqueen3.s
	gcc s/nqueen3.s -o out/nqueen3.out
	./out/nqueen3.out
	cat test/nqueen4.c | ./out/compiler.out > s/nqueen4.s
	gcc s/nqueen4.s  -o out/nqueen4.out
	./out/nqueen4.out
	cat test/nqueen5.c | ./out/compiler.out > s/nqueen5.s
	gcc s/nqueen5.s  -o out/nqueen5.out
	./out/nqueen5.out
	cat test/nqueen6.c | ./out/compiler.out > s/nqueen6.s
	gcc s/nqueen6.s -o out/nqueen6.out
	./out/nqueen6.out
	cat test/nqueen7.c | ./out/compiler.out > s/nqueen7.s
	gcc s/nqueen7.s -o out/nqueen7.out
	./out/nqueen7.out
	cat test/nqueen8.c | ./out/compiler.out > s/nqueen8.s
	gcc s/nqueen8.s -o out/nqueen8.out
	./out/nqueen8.out
	cat test/escape.c | ./out/compiler.out > s/escape.s
	gcc s/escape.s -o out/escape.out
	./out/escape.out
	cat test/duff.c | ./out/compiler.out > s/duff.s
	gcc s/duff.s -o out/duff.out
	./out/duff.out
	cat test/quine.c | ./out/compiler.out > s/quine.s
	gcc s/quine.s -o out/quine.out
	./out/quine.out > test/quine_res.c
	diff test/quine.c test/quine_res.c
	cat test/quine2.c | ./out/compiler.out > s/quine2.s
	gcc s/quine2.s -o out/quine2.out
	./out/quine2.out > test/quine2_res.c
	diff test/quine2.c test/quine2_res.c
	cat test/vector_test.c | ./out/compiler.out > s/vector_test.s
	cat test/vector_test2.c | ./out/compiler.out > s/vector_test2.s
	cat test/vector_test3.c | ./out/compiler.out > s/vector_test3.s
	cat test/vector_test4.c | ./out/compiler.out > s/vector_test4.s
	gcc s/vector_test.s -c
	gcc s/vector_test2.s -c
	gcc s/vector_test3.s -c
	gcc s/vector_test4.s -c

test_valid:
	rm out/*.out
	make supplement
	make notest2
	./test_cases.sh

check_error:
	make notest2
	./test_compile_error.sh 'int main(){int a; {int b;} return b;}'
	./test_compile_error.sh 'int main(){main(1}'
	./test_compile_error.sh 'int main(){return (123}'
	./test_compile_error.sh 'int main(){return 123}'
	./test_compile_error.sh 'int main(){if a}'
	./test_compile_error.sh 'int main(){do {}}'
	./test_compile_error.sh 'int main(){do {}while}'
	./test_compile_error.sh 'int main(){do {}while(}'
	./test_compile_error.sh 'int main(){do {}while(1}'
	./test_compile_error.sh 'int main(){do {}while(1)}'
	./test_compile_error.sh 'int main(){do {}while();}'
	./test_compile_error.sh 'int main(){while}'
	./test_compile_error.sh 'int main(){while(}'
	./test_compile_error.sh 'int main(){while()}'
	./test_compile_error.sh 'int main(){while(1}'
	./test_compile_error.sh 'int main(){while(1)}'
	./test_compile_error.sh 'int main(){while(1){break}}'
	./test_compile_error.sh 'int main(){break;}'
	./test_compile_error.sh 'int main(){while(1){continue}}'
	./test_compile_error.sh 'int main(){continue;}'
	./test_compile_error.sh 'int main(){for}'
	./test_compile_error.sh 'int main(){for(}'
	./test_compile_error.sh 'int main(){for(1)}'
	./test_compile_error.sh 'int main(){for(1;1)}'
	./test_compile_error.sh 'int main(){for(1;1;1)}'
	./test_compile_error.sh 'int main(){for(1;1;1}'
	./test_compile_error.sh 'int main(){1}'
	./test_compile_error.sh 'int main('
	./test_compile_error.sh 'int main(int a'
	./test_compile_error.sh 'int main(){int a; {int b;} return b;}'
	./test_compile_error.sh 'int main() {int a; int *b; b = a; return a;}'
	./test_compile_error.sh 'int main() {int a; int *b; 1? b : a; return a;}'
	./test_compile_error.sh 'int main() {int a; *a;}'
	./test_compile_error.sh 'int main(){int x; int *y; y = &x;int **z; z = y;}'
	./test_compile_error.sh 'int main(){int x; int *y; y = &x; **y;}'
	./test_compile_error.sh 'int main(){int x;int *y;*y = &x; return x;}'
	./test_compile_error.sh 'int main(){int x; int *y; y = &x;int **z; *z = x;}'
	./test_compile_error.sh 'int *foo(){return 1;}int main(){return 0;}'
	./test_compile_error.sh 'int *foo(){int x; return x;}int main(){return 0;}'
	./test_compile_error.sh 'int foo(){int *x; return x;}int main(){return 0;}'
	./test_compile_error.sh 'int *foo(){int *x; return x;}int main(){return foo();}'
	./test_compile_error.sh 'int *foo(){int *x; return x;}int main(){int x; x= foo();}'
	./test_compile_error.sh 'int main(){int *x; int *y; x+y;}'
	./test_compile_error.sh 'int main(){int *p; int a[4][1][2]; p = a;}'
	./test_compile_error.sh 'int main(){int a[1]; int *b; a = b;}'
	./test_compile_error.sh 'int main(){return 1[2];}'
	./test_compile_error.sh 'int main(){ foo: int a; return a;}'
	./test_compile_error.sh 'struct A{int a; int b;}; int main(){struct A a; if(a){return 12;} return 3;}'
	./test_compile_error.sh 'struct A{int a; int b;}; int main(){struct A a; for(;a;){return 12;} return 3;}'
	./test_compile_error.sh 'struct A{int a; int b;}; int main(){struct A a; while(a){return 12;} return 3;}'
	./test_compile_error.sh 'struct A{int a; int b;}; int main(){struct A a; do{return 12;}while(a); return 3;}'
	./test_compile_error.sh 'int main(){case 5: return 0;}'
	./test_compile_error.sh 'int main(){default: return 0;}'
	./test_compile_error.sh 'int main const(){return const 123 const; const} const'
	./test_compile_error.sh 'int main(void){void *p = 0; p += 3;}'
	./test_compile_error.sh 'int main(void){char a[5]; a[1] = 74; int *p = a + 3; p -= 2; return *p;}'
	./test_compile_error.sh 'int a(); char *a(); int main(void){return 174;}'

# clang-format or clang
notest:
	make format
	make notest2


warn:
	make format
	clang -Wall -Wextra -Wimplicit-fallthrough -Weverything -Wno-documentation -Wno-padded -Wno-missing-prototypes -Wno-switch-enum -DOVERRIDE_STD std.c codegen.c alignment.c parse_analyze_toplevel.c parse_analyze_statement.c codegen_expression.c main.c vector.c typecheck_expression.c parse_expression.c error.c type.c parse_type.c map.c print_x86_64.c $(CCFLAGS) lexer.c -o out/compiler.out
	clang -Wall -Wextra -Wimplicit-fallthrough -Weverything -Wno-documentation -Wno-padded -Wno-missing-prototypes -Wno-switch-enum assembly_sandbox.c print_x86_64.c $(CCFLAGS) -o out/assembly_sandbox.out
	

fmt_sandbox:
	make format
	make assembly_sandbox


f:
	make format
	make supplement
	make fmt_sandbox
	make test_valid
	make verify_typeparse
	make compile_files
	make check_error


format:
	clang-format -i *.c *.h -style=file
