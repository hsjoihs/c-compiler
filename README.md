# c-compiler
seccamp2018 c compiler

[日記](https://github.com/hsjoihs/c-compiler/blob/master/misc/diary.md)

In case you are also making a toy c compiler, feel free to use [my test cases](./test_cases.sh) to test yours.

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
