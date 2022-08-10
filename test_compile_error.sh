#!/bin/bash
test() {
	echo -e $1 | ./out/compiler.out > /dev/null
	res=$?
	if [ $res -eq 0 ]; then { echo -e "\033[31mFAIL\033[m"; exit 1; }; else echo -e "\033[32mPASS\033[m"; fi
}

test 'int main(){int a; {int b;} return b;}'
test 'int main(){main(1}'
test 'int main(){return (123}'
test 'int main(){return 123}'
test 'int main(){if a}'
test 'int main(){do {}}'
test 'int main(){do {}while}'
test 'int main(){do {}while(}'
test 'int main(){do {}while(1}'
test 'int main(){do {}while(1)}'
test 'int main(){do {}while();}'
test 'int main(){while}'
test 'int main(){while(}'
test 'int main(){while()}'
test 'int main(){while(1}'
test 'int main(){while(1)}'
test 'int main(){while(1){break}}'
test 'int main(){break;}'
test 'int main(){while(1){continue}}'
test 'int main(){continue;}'
test 'int main(){for}'
test 'int main(){for(}'
test 'int main(){for(1)}'
test 'int main(){for(1;1)}'
test 'int main(){for(1;1;1)}'
test 'int main(){for(1;1;1}'
test 'int main(){1}'
test 'int main('
test 'int main(int a'
test 'int main(){int a; {int b;} return b;}'
test 'int main() {int a; int *b; b = a; return a;}'
test 'int main() {int a; int *b; 1? b : a; return a;}'
test 'int main() {int a; *a;}'
test 'int main(){int x; int *y; y = &x;int **z; z = y;}'
test 'int main(){int x; int *y; y = &x; **y;}'
test 'int main(){int x;int *y;*y = &x; return x;}'
test 'int main(){int x; int *y; y = &x;int **z; *z = x;}'
test 'int *foo(){return 1;}int main(){return 0;}'
test 'int *foo(){int x; return x;}int main(){return 0;}'
test 'int foo(){int *x; return x;}int main(){return 0;}'
test 'int *foo(){int *x; return x;}int main(){return foo();}'
test 'int *foo(){int *x; return x;}int main(){int x; x= foo();}'
test 'int main(){int *x; int *y; x+y;}'
test 'int main(){int *p; int a[4][1][2]; p = a;}'
test 'int main(){int a[1]; int *b; a = b;}'
test 'int main(){return 1[2];}'
test 'int main(){ foo: int a; return a;}'
test 'struct A{int a; int b;}; int main(){struct A a; if(a){return 12;} return 3;}'
test 'struct A{int a; int b;}; int main(){struct A a; for(;a;){return 12;} return 3;}'
test 'struct A{int a; int b;}; int main(){struct A a; while(a){return 12;} return 3;}'
test 'struct A{int a; int b;}; int main(){struct A a; do{return 12;}while(a); return 3;}'
test 'int main(){case 5: return 0;}'
test 'int main(){default: return 0;}'
test 'int main const(){return const 123 const; const} const'
test 'int main(void){void *p = 0; p += 3;}'
test 'int main(void){char a[5]; a[1] = 74; int *p = a + 3; p -= 2; return *p;}'
test 'int a(); char *a(); int main(void){return 174;}'
test 'int a(void); int a(int b); int main(void){return 174;}'
test 'struct A{int a; int b;}; int main(){struct A a; struct A b; b *= a; return 3;}'
test 'struct A{int a; int b;}; int main(){struct A a; struct A b; b || a; return 3;}'
test 'int main(){3(); return 3;}'
test 'int main(){int a; (a)(); return 3;}'
test 'int main(){int *p = 0; (*p)(); return 3;}'
test 'int main(){int *p = 0; (p)(); return 3;}'
test 'int f(int a); int f(int a){return a;} int main(){int a; f(&a); return 3;}'
test 'int main(){goto; return 3; }'
test 'int main(){goto 3;  return 3;}'
test 'int main(){goto a;  return 3;}'
test 'int main(){goto a;  return 3;} int f(){a: return 0;}'
test 'struct A{int x; int y;}; struct B{int y; int x; }; int main(){struct A a; struct B b; (1? a : b).x; return 3;}'
test 'int main(){ return sizeof(int()); }'
test 'struct A; int main(){ return sizeof(struct A); }'
test 'int main(){ return _Alignof(int()); }'
test 'struct A; int main(){ return _Alignof(struct A); }'
test 'struct A{int x; int y;}; int main() {struct A a; return 3 - a; }'
test 'struct A{int a;}; int main() { struct A x; struct A y; struct A z; y.a = 100; z.a = 2; (x = y) = z; return 0; }'
test 'struct A{int a;}; int main() { struct A x; struct A y; y.a = 100; &(x = y); return 0; }'
test 'struct A{int a;}; int main() { struct A x; x.a = 20; struct A y; y.a = 100; &(1?x : y); return 0; }'
test 'int a[2](void); int main() { return 0; }'
test 'int(*a)[2](void); int main() { a; return 0; }'
test 'int(test())(void); int main() { return 0; }'
test 'int test()[3]; int main() { return 0; }'
test ';;;;;;;;;; int main() { return 0; }'