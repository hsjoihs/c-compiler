# c-compiler
seccamp2018 c compiler

[日記](https://github.com/hsjoihs/c-compiler/blob/master/misc/diary.md)

In case you are also making a toy C compiler, feel free to use [my test cases](./test_cases.sh) to test yours. I also have [negative test cases](./test_compile_error.sh), a list of inputs to which a C compiler should report an error. [test_cases_that_are_compile_error_only_if_pedantic.sh](./test_cases_that_are_compile_error_only_if_pedantic.sh) contains non-ISO extensions commonly implemented in many C compilers. hsjoihs-c-compiler reports an error with respect to those cases only when `-pedantic` is passed.

## Environment

Tested on macOS (Catalina), Ubuntu (20.04.1 LTS) and Debian (10.5).

## How to run

After cloning, run:

```
$ make
```

to compile.

The compiler can be run as 

```
$ cat input.c | ./out/compiler.out -DOSX -DOVERRIDE_STD > s/output.s
```

or as

```
$ ./out/compiler.out -DOSX -DOVERRIDE_STD input.c > s/output.s
```

for macOS; replace `-DOSX` with `-DLINUX` for Ubuntu. Note that, in Ubuntu, the generated assembly requires
`-no-pie` to be passed to gcc in order for the linking to succeed.


To self-compile, run 

```
$ make 2ndgen
```

; this will overwrite `./out/compiler.out`.

## How to test

Run

```
$ make test_2ndgen_compiler
```

to test the self-compiled compiler. To test some more, run 

```
$ make test_all_
```

## Fun samples

Conway's game of life:

```
$ make game_of_life
```
