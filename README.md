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

### Conway's game of life 

Based on https://gist.github.com/maekawatoshiki/8ea2f392e22698593d7ee8522aabbbd0

```
$ make game_of_life
```

### Calculating digits of pi / e 

Based on https://xn--w6q13e505b.jp/program/spigot.html
```
$ make pi
```

```
$ make digits_of_e
```

### Donut

```
$ make donut
```

The following is the source code, which is greatly modified from [the original](https://www.a1k0n.net/2011/07/20/donut-math.html) to avoid floating-point calculations

```c
void *memset();int printf();int usleep();int putchar();
int m(int a,int b){return (a*b+5000)/10000;}void a(int*c,int*s,int d,int t){int k=m(*c,d)-m(*s,t);int l=m(*s,d)+m(*c,t);*c=k;*s=l;}
int main(){int z[1760];char b[1760];printf("\e[2J");int q=10000;int r=0;int u=10000;int v=0;for(;;a(&q,&r,9992,400),a(&u,&v,9998,200),usleep(50000)){memset(b,32,1760);memset(z,0,1760*sizeof(int));int l=0;int p=10000;for(int i=0;i<88;i++,a(&p,&l,9974+i%2,714)){int w=0;int e=10000;for(int j=0;j<314;j++,a(&e,&w,9998,200)){int f=p+20000;int g=100000000/(m(m(w,f),r)+m(l,q)+50000);int t=m(m(w,q),f)-m(l,r);int x=40+30*m(g,m(m(e,u),f)-m(t,v))/10000;int y=12+15*m(g,m(m(e,v),f)+m(t,u))/10000;int o=x+80*y;int N=8*(m(m(l,r)-m(m(w,q),p),u)-m(m(w,r),p)-m(l,q)-m(m(e,v),p))/10000;if(22>y&&y>0&&x>0&&80>x&&g>z[o]){z[o]=g;b[o]=".,-~:;=!*#$@"[N>=1?N:0];}}}printf("\e[H");for(int k=0;k<1761;k++)putchar(k%80?b[k]:10);}return 0;}
```
