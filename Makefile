.PHONY: test_all_

test_all_:
	./test_all.sh

format:
	clang-format -i *.c -style=file

clean:
	rm out/*.out s/*.s

supplement:
	echo 'int always87() { return 87; }int always8() { return 8; }int add(int x, int y) { return x + y; }int subtract(int x, int y) { return x - y; }' | ./out/compiler.out > s/supplement.s


assembly_sandbox:
	make format
	gcc -Wall -Wextra assembly_sandbox.c print_x86_64.c -o out/assembly_sandbox.out
	./test_ret3.sh '' s/assembly_sandbox.s out/assembly_sandbox.out 174 out/assembly_sandbox.out

intmap_check:
	gcc -Wall -Wextra map.c intmap_check.c -o out/intmap_check.out
	./out/intmap_check.out

notest:
	make format
	gcc -Wall -Wextra compiler2.c type.c parse_expression.c map.c print_x86_64.c lexer.c -o out/compiler.out

full_compile:
	make supplement
	make notest
	./test_ret4.sh 090 'int main(){int x;x = 86;int *y;y = &x; return (*y) + x + 2;}' 174 out/compiler.out
	./test_ret4.sh 091 'int main(){int x;x = 86;int *y;y = &x; return (*y) + (*y) + 2;}' 174 out/compiler.out
	./test_ret4.sh 092 'int main(){int x;x = 86;int *y;y = &x;int **z;z = &y;return (*y) + (**z) + 2;}' 174 out/compiler.out
	./test_ret4.sh 093 'int main(){int x;x = 86;int *y;y = &x;int **z;z = &y;return*y+**z+2;}' 174 out/compiler.out
	./test_ret4.sh 094 'int main() {int x;int *y;x = 3;y = &x;*y = 174;return x;}' 174 out/compiler.out
	./test_ret4.sh 095 'int main() {int x;int *y;x = 3;y = &x;*y = 171;*y += 3;return x;}' 174 out/compiler.out
	./test_ret4.sh 096 'int main(){int x; int y; int *z; int*a; z=&x; a=&y; *z=*a=87; return(x+y);}' 174 out/compiler.out
	./test_ret4.sh 086 'int main(){int a; a = 3; { a = 174;} return a;}' 174 out/compiler.out
	./test_ret4.sh 085 'int main(){int a; a = 174; {int a; a = 3;} return a;}' 174 out/compiler.out
	./test_ret4.sh 084 'int main(){int a; int b; for(a=0,b=0;a<10;a++){ if(a ==5)continue;b+=a;} return b;}' 40 out/compiler.out
	./test_ret4.sh 089 'int main() {int a; a = 174; int *b; b = &a; return a;}' 174 out/compiler.out
	./test_ret4.sh 083 'int main(){int a; int b; int c; int d; d=0; b = 5; c = 0;for(a = 3;a;d++){for(;b;++d) {c += b;b-=1;if(b == 3) break;}b = 7;a-=1;if(a == 1) break;} return a*7+b*15+c*2;}' 174 out/compiler.out
	./test_ret4.sh 082 'int main(){int a; int b; for(a=0,b=0;a <= 10;a++) {b += a;}return b;}' 55 out/compiler.out
	./test_ret4.sh 081 'int main(){int a; int b; for(a=0,b=0;a <= 10;++a) {b += a;}return b;}' 55 out/compiler.out
	./test_ret4.sh 080 'int main(){int a; int b; for(a=11, b=0;a;){a-=1;b+=a;if(a)continue;break; a+=100;} return b;}' 55 out/compiler.out
	./test_ret4.sh 079 'int main(){int a; int b; for(a=0,b=0;a <= 10;) {b += a; a += 1;}return b;}' 55 out/compiler.out
	./test_ret4.sh 078 'int main(){int a; for (a = 3;;) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174 out/compiler.out
	./test_ret4.sh 077 'int main(){int a; for (a = 3;a;) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174 out/compiler.out
	./test_ret4.sh 076 'int main(){int a; int b; a =0; b=0; do{b+=--a;}while(a+3); return b*a;}' 18 out/compiler.out
	./test_ret4.sh 075 'int main(){int a; int b; a=3; b=0; b+= a++; return !(b-3)+!(a-4)+172;}' 174 out/compiler.out
	./test_ret4.sh 074 'int main(){int a; int b; a=3; b=0; b+= ++a; return a*b*11-2;}' 174 out/compiler.out
	./test_ret4.sh 073 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;}while(a+3); return b*a;}' 18 out/compiler.out
	./test_ret4.sh 072 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;}while(a+3); return a*b;}' 18 out/compiler.out
	./test_ret4.sh 071 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(!a)break;}while(a+3); return a*b;}' 18 out/compiler.out
	./test_ret4.sh 070 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break;}while(a+3); return a*b;}' 18 out/compiler.out
	./test_ret4.sh 069 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return b*a*10;}' 180 out/compiler.out
	./test_ret4.sh 068 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return b*a;}' 18 out/compiler.out
	./test_ret4.sh 067 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b;}' 18 out/compiler.out
	./test_ret4.sh 001 'int main(){return 123;}' 123 out/compiler.out
	./test_ret4.sh 002 'int main(){return (123);}' 123 out/compiler.out
	./test_ret4.sh 003 'int main(){return ((((123))));}' 123 out/compiler.out
	./test_ret4.sh 004 'int main(){return 123+51;}' 174 out/compiler.out
	./test_ret4.sh 005 'int main(){return 123+56-5;}' 174 out/compiler.out
	./test_ret4.sh 006 'int main(){return 175-(4-3);}' 174 out/compiler.out
	./test_ret4.sh 007 'int main(){return 181-4-3;}' 174 out/compiler.out
	./test_ret4.sh 008 'int main(){return 0x29*3+7*8-5*1;}' 174 out/compiler.out
	./test_ret4.sh 009 'int main(){return 6*(3+7)-5*1;}' 55 out/compiler.out
	./test_ret4.sh 010 'int main(){return 43,6*(3+7)-5*1;}' 55 out/compiler.out
	./test_ret4.sh 011 'int main(){return 43,6*(3+(4|3))-(5|1)*1;}' 55 out/compiler.out
	./test_ret4.sh 012 'int main(){return 043,41*3+07*010-0Xa/(010%(1+2));}' 174 out/compiler.out
	./test_ret4.sh 013 'int main(){return 7*5,(12,41*3)+7*16/(9,2)-10/(8%3);}' 174 out/compiler.out
	./test_ret4.sh 014 'int main(){return 7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1+2)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	1  <<3)	%3);}'  174 out/compiler.out
	./test_ret4.sh 015 'int main(){return 35,	((	41|	(8   !=     15))*  ((3==3)+2))+((5|2)*(9&10))   -   (10/(8%3));}'  174 out/compiler.out
	./test_ret4.sh 016 'int main(){return 043,41*3+07*010-0Xa/(010%(!!1+2));}' 174 out/compiler.out
	./test_ret4.sh 017 'int main(){return 7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1-~1)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	!0  <<3)	%3);}' 174 out/compiler.out
	./test_ret4.sh 018 'int main(){return +174;}' 174 out/compiler.out
	./test_ret4.sh 019 'int main(){return -(1-175);}' 174 out/compiler.out
	./test_ret4.sh 020 'int main(){23; 45+37; ((12-1)*75); return -(1-175);}' 174 out/compiler.out
	./test_ret4.sh 021 'int main(){23; 45+37; return -(1-175); ((12-1)*75);}' 174 out/compiler.out
	./test_ret4.sh 022 'int main(){int a; int b; return (a = b = 9, a = 41*3, 55 - (b = 4) + a);}' 174 out/compiler.out
	./test_ret4.sh 023 'int main(){int a; int b; int c; int d; int _q432; a = b = c = 9; d = 5; a = 41*3; return (c, _q432 = 8, d = 11*5) - (b = 4) + a;}'  174 out/compiler.out
	./test_ret4.sh 024 'int main(){return 175^1;}' 174 out/compiler.out
	./test_ret4.sh 025 'int main(){return 2 + (1? 100 + 72 : 17);}' 174 out/compiler.out
	./test_ret4.sh 026 'int main(){return (0? 234 : 2) + (1? 100 + 72 : 17);}' 174 out/compiler.out
	./test_ret4.sh 027 'int main(){return (3, always87() + always87());}' 174 out/compiler.out
	./test_ret4.sh 028 'int main(){return always87() + ((always8()* 11) -1);}' 174 out/compiler.out
	./test_ret4.sh 029 'int main(){return add(170,4);}' 174 out/compiler.out
	./test_ret4.sh 030 'int main(){return always87() + ((always8()* add(4,7)) -1);}' 174 out/compiler.out
	./test_ret4.sh 031 'int main(){return always87() + ((always8()* subtract(12,1)) -1);}' 174 out/compiler.out
	./test_ret4.sh 032 'int main(){3; {5; 7; 11; } return 175^1;}' 174 out/compiler.out
	./test_ret4.sh 033 'int always87_(){return 87;} int main(){return (3, always87() + always87_());}' 174 out/compiler.out
	./test_ret4.sh 034 'int add_(int x, int y){4; return x+y;} int main(){3; return add_(87,87);}' 174 out/compiler.out
	./test_ret4.sh 035 'int fib(int n){ return n < 2? n : fib(n - 1) + fib(n - 2); } int main(){3; return fib(10);}' 55 out/compiler.out
	./test_ret4.sh 036 'int tarai(int x,int y,int z){ return x <= y? y : tarai(tarai(x-1, y, z), tarai(y-1, z, x), tarai(z-1, x, y)); } int main(){return tarai(12,6,0);}' 12 out/compiler.out
	./test_ret4.sh 037 'int main() { return (3 && 2 && 5) + 173; }' 174 out/compiler.out
	./test_ret4.sh 038 'int main() { return (3 && 2) + !(3 && 0) + !(0 && 3)+ !(0 && 0) + 170; }' 174 out/compiler.out
	./test_ret4.sh 039 'int main() { return (3 || 2 || 5) + 173; }' 174 out/compiler.out
	./test_ret4.sh 040 'int main() { return (3 || 2) + (3 || 0) + (0 || 3)+ !(0 || 0) + 170; }' 174 out/compiler.out
	./test_ret4.sh 041 'int main() {int a; a = 3; a += 5;  return a + 166; }' 174 out/compiler.out
	./test_ret4.sh 042 'int main() {int a; int b; a = 3; b = (a += 5);  return a + b + 158; }' 174 out/compiler.out
	./test_ret4.sh 043 'int main() {int a; int b; a = 3; b = 1; b *= (a += 5);  return a + b + 158; }' 174 out/compiler.out
	./test_ret4.sh 044 'int main() {int a; int b; a = 11; a -=5; a /= 2; b = 1; b *= (a += 5);  return a + b + 158; }' 174 out/compiler.out
	./test_ret4.sh 045 'int main() {int a; int b; int c; a = 7; a &= ~2; a <<= 2; a |=2; a >>= 1; a -=5; a /= 2; b = 3; c = 8; b ^= (c%=3); b *= (a += 5);  return a + b + 158; }' 174 out/compiler.out
	./test_ret4.sh 046 'int foo(){ return 2;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();} else { }    return 172+b;}' 174 out/compiler.out
	./test_ret4.sh 047 'int foo(){ return 2;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();}   return 172+b;}' 174 out/compiler.out
	./test_ret4.sh 048 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();} else { c = bar();}    return 172+b;}' 174 out/compiler.out
	./test_ret4.sh 049 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 0;b = 5;c = 2;if(a) {b = foo();} else { c = bar();}    return 162+b+c;}' 174 out/compiler.out
	./test_ret4.sh 050 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) if(0) { b = foo(); } else {  c = bar(); }    return 162+b+c;}' 174 out/compiler.out
	./test_ret4.sh 051 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) if(0)b=foo();else c = bar();return 162+b+c;}' 174 out/compiler.out
	./test_ret4.sh 052 'int main() {int a; a = 4; if(1){return 170+a; a = 7; }else{return 170-a; a = 9;} a = 5; return a;}' 174 out/compiler.out
	./test_ret4.sh 053 'int foo(){return 1;} int main(){int a; a=0;do{a=3;}while(a==foo());return 174;}' 174 out/compiler.out
	./test_ret4.sh 054 'int main(){int a; a=0;do{a+=1;}while(a && a < 174);return a;}' 174 out/compiler.out
	./test_ret4.sh 055 'int main(){int a; a=-8;do{a+=1;}while(a);return a+174;}' 174 out/compiler.out
	./test_ret4.sh 056 'int foo(){return 3;} int main() {int a; a = 0;while(a == foo()) {a = 3;}return 174;}' 174 out/compiler.out
	./test_ret4.sh 057 'int main(){int a; int b; a = 0; b = 0; while(a <= 10) {b += a; a += 1;}return b;}' 55 out/compiler.out
	./test_ret4.sh 058 'int main(){int a; a = 3;while (a) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174 out/compiler.out
	./test_ret4.sh 059 'int main(){int a; int b; int c; a = 3; b = 5; c = 0;while(a){while(b) {c += b;b-=1;if(b == 3) break;}b = 7;a-=1;if(a == 1) break;} return a*7+b*15+c*2;}' 174 out/compiler.out
	./test_ret4.sh 060 'int main(){int a; a = 3;while (a) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174 out/compiler.out
	./test_ret4.sh 061 'int main(){int a; int b; a=11; b=0; while(a){a-=1;b+=a;if(a)continue;break; a+=100;} return b;}' 55 out/compiler.out
	./test_ret4.sh 062 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return -a;}' 3 out/compiler.out
	./test_ret4.sh 063 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return -b;}' 6 out/compiler.out
	./test_ret4.sh 064 'int main(){int a; int b; a =-3; b=-6; return a*b*10+a+b+3;}' 174 out/compiler.out
	./test_ret4.sh 065 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b*10;}' 180 out/compiler.out
	./test_ret4.sh 066 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b*10+a+b+3;}' 174 out/compiler.out
	./test_ret4.sh 087 'int main() {int *b; int a; a = 3; a += 5;  return a + 166; }' 174 out/compiler.out
	./test_ret4.sh 088 'int main() {int *******b; int a; a = 3; a += 5;  return a + 166; }' 174 out/compiler.out
	

check_error:
	make notest
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
	./test_compile_error.sh 'int main() {int a; int *b; b = a; return a;}'
	./test_compile_error.sh 'int main() {int a; int *b; 1? b : a; return a;}'
	./test_compile_error.sh 'int main() {int a; *a;}'
	./test_compile_error.sh 'int main(){int x; int *y; y = &x;int **z; z = y;}'
	./test_compile_error.sh 'int main(){int x; int *y; y = &x; **y;}'
	
	



