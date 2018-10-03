# c-compiler
seccamp2018 c compiler

[日記](https://github.com/hsjoihs/c-compiler/blob/master/misc/diary.md)

## Environment

Tested on both macOS (High Sierra) and Ubuntu (18.04.1 LTS).

## How to run

After cloning, run:

```
$ make
```

to compile.

The compiler can be run as 

```
$ cat input.c | ./out/compiler.out > s/output.s
```

or as

```
$ ./out/compiler.out input.c > s/output.s
```

. To self-compile, run 

```
$ make 2ndgen
```

; this will overwrite `./out/compiler.out`.

## How to test

Run

```
$ make test_mixed_compiler
```
