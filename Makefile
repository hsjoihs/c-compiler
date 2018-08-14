.PHONY: test_all_

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CCFLAGS += -D LINUX
endif
ifeq ($(UNAME_S),Darwin)
    CCFLAGS += -D OSX
endif

test_all_:
	make format
	make supplement
	make assembly_sandbox
	make test_valid
	make compile_files
	make check_error

format:
	clang-format -i *.c *.h -style=file

clean:
	rm out/*.out s/*.s

supplement:
	gcc supplement.c -S -o s/supplement.s

assembly_sandbox:
	make format
	clang -Wall -Wextra -Wimplicit-fallthrough assembly_sandbox.c print_x86_64.c $(CCFLAGS) -o out/assembly_sandbox.out
	echo -e '' | ./out/assembly_sandbox.out > s/assembly_sandbox.s
	gcc s/assembly_sandbox.s s/supplement.s -o out/sandbox.out
	./out/sandbox.out || if [ $$? -ne 174 ]; then { echo "\n\033[31mFAIL\033[m"; exit 1; }; else echo "\n\033[32mPASS\033[m"; fi

compile_files:
	clang-format -i misc/*.c -style=file
	make supplement
	make notest
	cat misc/nqueen2.c | ./out/compiler.out > s/nqueen2.s
	gcc s/nqueen2.s s/supplement.s -o out/nqueen2.out
	./out/nqueen2.out
	cat misc/nqueen3.c | ./out/compiler.out > s/nqueen3.s
	gcc s/nqueen3.s s/supplement.s -o out/nqueen3.out
	./out/nqueen3.out
	cat misc/nqueen4.c | ./out/compiler.out > s/nqueen4.s
	gcc s/nqueen4.s s/supplement.s -o out/nqueen4.out
	./out/nqueen4.out
	cat misc/nqueen5.c | ./out/compiler.out > s/nqueen5.s
	gcc s/nqueen5.s s/supplement.s -o out/nqueen5.out
	./out/nqueen5.out

warn:
	make format
	clang -Wall -Wextra -Wimplicit-fallthrough -Weverything -Wno-padded -Wno-missing-prototypes -Wno-switch-enum codegen.c parse_toplevel.c parse_statement.c codegen_expression.c main.c vector.c parser.c error.c type.c parse_binary_expression.c map.c print_x86_64.c $(CCFLAGS) lexer.c -o out/compiler.out

notest:
	make format
	gcc -Wall -Wextra codegen.c parse_toplevel.c parse_statement.c codegen_expression.c main.c vector.c parser.c error.c type.c parse_binary_expression.c map.c print_x86_64.c $(CCFLAGS) lexer.c -o out/compiler.out

test_valid:
	rm out/*.out
	make supplement
	make notest
	./test_cases.sh

check_error:
	make notest
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
	


