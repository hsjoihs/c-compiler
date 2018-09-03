UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    OSFLAG += -DLINUX
endif
ifeq ($(UNAME_S),Darwin)
    OSFLAG += -DOSX
endif

# out/compiler.out is purely from clang/gcc
1stgen:
	gcc -Wall -Wextra -DOVERRIDE_STD -g std.c codegen.c alignment.c parse_analyze_toplevel.c parse_analyze_statement.c codegen_expression.c main.c vector.c typecheck_expression.c parse_expression.c error.c type.c parse_type.c map.c print_x86_64.c $(OSFLAG) lexer.c codegen_switch.c -o out/compiler.out
	cp -p out/compiler.out out/compiler_1stgen.out

2ndgen:
	make 1stgen
	./compile.sh vector $(OSFLAG)
	./compile.sh map $(OSFLAG)
	./compile.sh print_x86_64 $(OSFLAG)
	./compile.sh alignment $(OSFLAG)
	./compile.sh codegen_expression $(OSFLAG)
	./compile.sh std $(OSFLAG)
	./compile.sh error $(OSFLAG)
	./compile.sh codegen $(OSFLAG)
	./compile.sh main $(OSFLAG)
	gcc -Wall -Wextra -DOVERRIDE_STD self_compile_asm/std.s self_compile_asm/codegen.s self_compile_asm/alignment.s parse_analyze_toplevel.c parse_analyze_statement.c self_compile_asm/codegen_expression.s self_compile_asm/main.s self_compile_asm/vector.s typecheck_expression.c parse_expression.c self_compile_asm/error.s type.c parse_type.c self_compile_asm/map.s self_compile_asm/print_x86_64.s codegen_switch.c $(OSFLAG) lexer.c -o out/compiler.out
	cp -p out/compiler.out out/compiler_2ndgen.out

test_mixed_compiler:
	make 2ndgen
	./test_cases.sh
	./test_compile_error.sh
	./compile.sh vector $(OSFLAG) __with2nd
	./compile.sh map $(OSFLAG) __with2nd
	./compile.sh print_x86_64 $(OSFLAG) __with2nd
	./compile.sh alignment $(OSFLAG) __with2nd
	./compile.sh codegen_expression $(OSFLAG) __with2nd
	./compile.sh std $(OSFLAG) __with2nd
	./compile.sh error $(OSFLAG) __with2nd
	./compile.sh codegen $(OSFLAG) __with2nd
	./compile.sh main $(OSFLAG) __with2nd
	gcc -Wall -Wextra -DOVERRIDE_STD self_compile_asm/std__with2nd.s self_compile_asm/codegen__with2nd.s self_compile_asm/alignment__with2nd.s parse_analyze_toplevel.c parse_analyze_statement.c self_compile_asm/codegen_expression__with2nd.s self_compile_asm/main__with2nd.s self_compile_asm/vector__with2nd.s typecheck_expression.c parse_expression.c self_compile_asm/error__with2nd.s type.c parse_type.c self_compile_asm/map__with2nd.s self_compile_asm/print_x86_64__with2nd.s codegen_switch.c $(OSFLAG) lexer.c -o out/compiler_gen3.out
	diff self_compile_asm/vector.s self_compile_asm/vector__with2nd.s
	diff self_compile_asm/map.s self_compile_asm/map__with2nd.s
	diff self_compile_asm/print_x86_64.s self_compile_asm/print_x86_64__with2nd.s
	diff self_compile_asm/alignment.s self_compile_asm/alignment__with2nd.s
	diff self_compile_asm/codegen_expression.s self_compile_asm/codegen_expression__with2nd.s
	diff self_compile_asm/std.s self_compile_asm/std__with2nd.s
	diff self_compile_asm/error.s self_compile_asm/error__with2nd.s
	diff self_compile_asm/codegen.s self_compile_asm/codegen__with2nd.s

test_all_:
	make supplement
	make assembly_sandbox
	make test_valid
	make verify_typeparse
	make compile_files
	make check_error
	make test_mixed_compiler


clean:
	rm out/*.out s/*.s

supplement:
	gcc misc/supplement.c -S -o s/supplement.s
	gcc misc/supplement2.c -S -o s/supplement2.s

verify_typeparse:
	clang -Wall -Wextra -Wimplicit-fallthrough $(OSFLAG) vector.c verifier/typeparse_checker.c lexer.c type.c parse_type.c error.c -o out/typeparse_check.out
	./out/typeparse_check.out

assembly_sandbox:
	gcc -Wall -Wextra misc/assembly_sandbox.c print_x86_64.c $(OSFLAG) -o out/assembly_sandbox.out
	echo -e '' | ./out/assembly_sandbox.out > s/assembly_sandbox.s
	gcc s/assembly_sandbox.s s/supplement.s -o out/sandbox.out
	./out/sandbox.out || if [ $$? -ne 174 ]; then { echo "\n\033[31mFAIL\033[m"; exit 1; }; else echo "\n\033[32mPASS\033[m"; fi

compile_files:
	make supplement
	make 1stgen
	./build_files.sh
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
	make 1stgen
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
	clang -Wall -Wextra -Wimplicit-fallthrough -Weverything -Wno-documentation -Wno-padded -Wno-missing-prototypes -Wno-switch-enum -DOVERRIDE_STD std.c codegen.c alignment.c parse_analyze_toplevel.c parse_analyze_statement.c codegen_expression.c main.c vector.c typecheck_expression.c parse_expression.c error.c type.c parse_type.c map.c print_x86_64.c $(OSFLAG) lexer.c -o out/compiler.out
	clang -Wall -Wextra -Wimplicit-fallthrough -Weverything -Wno-documentation -Wno-padded -Wno-missing-prototypes -Wno-switch-enum misc/assembly_sandbox.c print_x86_64.c $(OSFLAG) -o out/assembly_sandbox.out

f:
	make format
	make test_all_


format:
	clang-format -i *.c *.h -style=file
