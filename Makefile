UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    OSFLAG += -DLINUX
endif
ifeq ($(UNAME_S),Darwin)
    OSFLAG += -DOSX
endif

SRC=std.c codegen.c alignment.c parse_analyze_toplevel.c parse_analyze_statement.c codegen_expression.c main.c vector.c typecheck_expression.c parse_expression.c error.c type.c parse_type.c map.c print_x86_64.c lexer.c codegen_switch.c file_io.c cpp.c

CLANG_WARN=-Wall -Wextra -Wimplicit-fallthrough -Weverything -Wno-documentation -Wno-padded -Wno-missing-prototypes -Wno-switch-enum

# out/compiler.out is purely from clang/gcc
1stgen:
	gcc -Wall -Wextra -DOVERRIDE_STD -g $(SRC) $(OSFLAG) -o out/compiler.out
	cp -p out/compiler.out out/compiler_1stgen.out

test_include:
	make 1stgen
	./compile2.sh map $(OSFLAG)
	./out/compiler.out __.c > self_compile_asm/__.s
	gcc -Wall -Wextra -DOVERRIDE_STD self_compile_asm/__.s self_compile_asm/map.s -o out/test_include.out -no-pie -Wno-unused-command-line-argument
	./out/test_include.out

2ndgen:
	make 1stgen
	./compile2.sh vector $(OSFLAG)
	./compile2.sh map $(OSFLAG)
	./compile2.sh print_x86_64 $(OSFLAG)
	./compile2.sh alignment $(OSFLAG)
	./compile2.sh codegen_expression $(OSFLAG)
	./compile2.sh std $(OSFLAG)
	./compile2.sh error $(OSFLAG)
	./compile2.sh codegen $(OSFLAG)
	./compile2.sh codegen_switch $(OSFLAG)
	./compile2.sh parse_analyze_statement $(OSFLAG)
	./compile2.sh parse_analyze_toplevel $(OSFLAG)
	./compile2.sh parse_expression $(OSFLAG)
	./compile2.sh parse_type $(OSFLAG)
	./compile2.sh type $(OSFLAG)
	./compile2.sh typecheck_expression $(OSFLAG)
	./compile2.sh file_io $(OSFLAG)
	./compile2.sh main $(OSFLAG)
	./compile2.sh lexer $(OSFLAG)
	./compile2.sh cpp $(OSFLAG)
	gcc -Wall -Wextra -DOVERRIDE_STD self_compile_asm/std.s self_compile_asm/codegen.s self_compile_asm/alignment.s self_compile_asm/parse_analyze_toplevel.s self_compile_asm/lexer.s self_compile_asm/codegen_expression.s self_compile_asm/main.s self_compile_asm/vector.s self_compile_asm/typecheck_expression.s self_compile_asm/parse_expression.s self_compile_asm/error.s self_compile_asm/type.s self_compile_asm/parse_type.s self_compile_asm/map.s self_compile_asm/print_x86_64.s self_compile_asm/codegen_switch.s self_compile_asm/file_io.s $(OSFLAG) self_compile_asm/parse_analyze_statement.s self_compile_asm/cpp.s -o out/compiler.out -no-pie -Wno-unused-command-line-argument
	cp -p out/compiler.out out/compiler_2ndgen.out

test_2ndgen_compiler:
	make 2ndgen
	make link_test
	./test_cases.sh
	./test_compile_error.sh
	./compile2.sh vector $(OSFLAG) __with2nd
	./compile2.sh map $(OSFLAG) __with2nd
	./compile2.sh print_x86_64 $(OSFLAG) __with2nd
	./compile2.sh alignment $(OSFLAG) __with2nd
	./compile2.sh codegen_expression $(OSFLAG) __with2nd
	./compile2.sh std $(OSFLAG) __with2nd
	./compile2.sh error $(OSFLAG) __with2nd
	./compile2.sh codegen $(OSFLAG) __with2nd
	./compile2.sh codegen_switch $(OSFLAG) __with2nd
	./compile2.sh parse_analyze_statement $(OSFLAG) __with2nd
	./compile2.sh parse_analyze_toplevel $(OSFLAG) __with2nd
	./compile2.sh parse_expression $(OSFLAG) __with2nd
	./compile2.sh parse_type $(OSFLAG) __with2nd
	./compile2.sh type $(OSFLAG) __with2nd
	./compile2.sh typecheck_expression $(OSFLAG) __with2nd
	./compile2.sh file_io $(OSFLAG) __with2nd
	./compile2.sh main $(OSFLAG) __with2nd
	./compile2.sh lexer $(OSFLAG) __with2nd
	./compile2.sh cpp $(OSFLAG) __with2nd
	gcc -Wall -Wextra -DOVERRIDE_STD self_compile_asm/std__with2nd.s self_compile_asm/codegen__with2nd.s self_compile_asm/alignment__with2nd.s self_compile_asm/parse_analyze_toplevel__with2nd.s self_compile_asm/lexer.s self_compile_asm/codegen_expression__with2nd.s self_compile_asm/main__with2nd.s self_compile_asm/vector__with2nd.s self_compile_asm/typecheck_expression__with2nd.s self_compile_asm/parse_expression__with2nd.s self_compile_asm/error__with2nd.s self_compile_asm/type__with2nd.s self_compile_asm/parse_type__with2nd.s self_compile_asm/map__with2nd.s self_compile_asm/print_x86_64__with2nd.s self_compile_asm/codegen_switch__with2nd.s $(OSFLAG) self_compile_asm/parse_analyze_statement__with2nd.s self_compile_asm/file_io__with2nd.s self_compile_asm/cpp__with2nd.s -o out/compiler_gen3.out -no-pie -Wno-unused-command-line-argument
	diff self_compile_asm/vector.s self_compile_asm/vector__with2nd.s
	diff self_compile_asm/map.s self_compile_asm/map__with2nd.s
	diff self_compile_asm/print_x86_64.s self_compile_asm/print_x86_64__with2nd.s
	diff self_compile_asm/alignment.s self_compile_asm/alignment__with2nd.s
	diff self_compile_asm/codegen_expression.s self_compile_asm/codegen_expression__with2nd.s
	diff self_compile_asm/std.s self_compile_asm/std__with2nd.s
	diff self_compile_asm/error.s self_compile_asm/error__with2nd.s
	diff self_compile_asm/codegen.s self_compile_asm/codegen__with2nd.s
	diff self_compile_asm/codegen_switch.s self_compile_asm/codegen_switch__with2nd.s
	diff self_compile_asm/parse_analyze_statement.s self_compile_asm/parse_analyze_statement__with2nd.s
	diff self_compile_asm/parse_analyze_toplevel.s self_compile_asm/parse_analyze_toplevel__with2nd.s
	diff self_compile_asm/parse_expression.s self_compile_asm/parse_expression__with2nd.s
	diff self_compile_asm/parse_type.s self_compile_asm/parse_type__with2nd.s
	diff self_compile_asm/type.s self_compile_asm/type__with2nd.s
	diff self_compile_asm/typecheck_expression.s self_compile_asm/typecheck_expression__with2nd.s
	diff self_compile_asm/lexer.s self_compile_asm/lexer__with2nd.s
	diff self_compile_asm/file_io.s self_compile_asm/file_io__with2nd.s
	diff self_compile_asm/cpp.s self_compile_asm/cpp__with2nd.s

test_all_:
	make assembly_sandbox
	make test_valid
	make verify_typeparse
	make compile_files
	make check_error
	make test_2ndgen_compiler
	make test_include

clean:
	rm out/*.out s/*.s

verify_typeparse:
	gcc -Wall -Wextra $(OSFLAG) vector.c verifier/typeparse_checker.c lexer.c type.c parse_type.c error.c map.c file_io.c cpp.c -o out/typeparse_check.out
	./out/typeparse_check.out

assembly_sandbox:
	gcc -Wall -Wextra misc/assembly_sandbox.c print_x86_64.c $(OSFLAG) -o out/assembly_sandbox.out
	echo -e '' | ./out/assembly_sandbox.out > s/assembly_sandbox.s
	gcc misc/supplement.c -S -o s/supplement.s
	gcc s/assembly_sandbox.s s/supplement.s -o out/sandbox.out
	./out/sandbox.out || if [ $$? -ne 174 ]; then { echo "\n\033[31mFAIL\033[m"; exit 1; }; else echo "\n\033[32mPASS\033[m"; fi

compile_files:
	make 1stgen
	./build_files.sh
	cat test/quine.c | ./out/compiler.out > s/quine.s
	gcc s/quine.s -o out/quine.out -no-pie -Wno-unused-command-line-argument
	./out/quine.out > test/quine_res.c
	diff test/quine.c test/quine_res.c
	cat test/quine2.c | ./out/compiler.out > s/quine2.s
	gcc s/quine2.s -o out/quine2.out -no-pie -Wno-unused-command-line-argument
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

link_test:
	./out/compiler.out test/link1.c > s/link1.s
	./out/compiler.out test/link2.c > s/link2.s
	gcc s/link1.s s/link2.s -o out/link.out -no-pie -Wno-unused-command-line-argument
	./out/link.out; test $$? -eq 174 


test_valid:
	rm out/*.out
	make 1stgen
	make link_test
	./test_cases.sh

check_error:
	make 1stgen
	./test_compile_error.sh

# clang-format or clang
notest:
	make format
	make 1stgen

warn:
	make format
	clang $(CLANG_WARN) -DOVERRIDE_STD $(SRC) $(OSFLAG) -o out/compiler.out 
	clang $(CLANG_WARN) misc/assembly_sandbox.c print_x86_64.c $(OSFLAG) -o out/assembly_sandbox.out
	clang -Wall -Wextra -Wimplicit-fallthrough $(OSFLAG) vector.c verifier/typeparse_checker.c lexer.c type.c parse_type.c error.c file_io.c -o out/typeparse_check.out

f:
	make format
	make test_all_
	make test_sanitized_1stgen


format:
	clang-format -i *.c *.h -style=file

1stgen_sanitized:
	clang -Wall -Wextra -DOVERRIDE_STD -g $(SRC) $(OSFLAG) -o out/compiler.out  -fsanitize=address -fno-omit-frame-pointer
	cp -p out/compiler.out out/compiler_1stgen_sanitized.out

test_sanitized_1stgen:
	make 1stgen_sanitized
	./compile2.sh vector $(OSFLAG)
	./compile2.sh map $(OSFLAG)
	./compile2.sh print_x86_64 $(OSFLAG)
	./compile2.sh alignment $(OSFLAG)
	./compile2.sh codegen_expression $(OSFLAG)
	./compile2.sh std $(OSFLAG)
	./compile2.sh error $(OSFLAG)
	./compile2.sh codegen $(OSFLAG)
	./compile2.sh  main $(OSFLAG)
	./compile2.sh codegen_switch $(OSFLAG)
	./compile2.sh parse_analyze_statement $(OSFLAG)
	./compile2.sh parse_analyze_toplevel $(OSFLAG)
	./compile2.sh parse_expression $(OSFLAG)
	./compile2.sh parse_type $(OSFLAG)
	./compile2.sh type $(OSFLAG)
	./compile2.sh typecheck_expression $(OSFLAG)
	./compile2.sh lexer $(OSFLAG)
	./compile2.sh file_io $(OSFLAG)
	./compile2.sh cpp $(OSFLAG)
