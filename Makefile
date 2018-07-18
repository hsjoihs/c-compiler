.PHONY: test_all_

test_all_:
	./test_all.sh

format:
	clang-format -i *.c -style=file

clean:
	rm out/*.out s/*.s


assembly_sandbox:
	make format
	gcc -Wall assembly_sandbox.c print_x86_64.c -o out/assembly_sandbox.out
	./test_ret3.sh '' s/assembly_sandbox.s out/assembly_sandbox.out 75 out/assembly_sandbox.out

intmap_check:
	gcc -Wall intmap.c intmap_check.c -o out/intmap_check.out
	./out/intmap_check.out

notest:
	gcc -Wall compiler2.c intmap.c print_x86_64.c lexer.c -o out/compiler.out

full_compile:
	gcc -Wall compiler2.c intmap.c print_x86_64.c lexer.c -o out/compiler.out
	./test_ret4.sh 080 'main(){for(a=11, b=0;a;){a-=1;b+=a;if(a)continue;break; a+=100;} return b;}' 55 out/compiler.out
	./test_ret4.sh 079 'main(){for(a=0,b=0;a <= 10;) {b += a; a += 1;}return b;}' 55 out/compiler.out
	./test_ret4.sh 078 'main(){for (a = 3;;) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174 out/compiler.out
	./test_ret4.sh 077 'main(){for (a = 3;a;) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174 out/compiler.out
	./test_ret4.sh 076 'main(){a =0; b=0; do{b+=--a;}while(a+3); return b*a;}' 18 out/compiler.out
	./test_ret4.sh 075 'main(){a=3; b=0; b+= a++; return !(b-3)+!(a-4)+172;}' 174 out/compiler.out
	./test_ret4.sh 074 'main(){a=3; b=0; b+= ++a; return a*b*11-2;}' 174 out/compiler.out
	./test_ret4.sh 073 'main(){a =0; b=0; do{a-=1;b+=a;}while(a+3); return b*a;}' 18 out/compiler.out
	./test_ret4.sh 072 'main(){a =0; b=0; do{a-=1;b+=a;}while(a+3); return a*b;}' 18 out/compiler.out
	./test_ret4.sh 071 'main(){a =0; b=0; do{a-=1;b+=a;if(!a)break;}while(a+3); return a*b;}' 18 out/compiler.out
	./test_ret4.sh 070 'main(){a =0; b=0; do{a-=1;b+=a;if(a)continue;break;}while(a+3); return a*b;}' 18 out/compiler.out
	./test_ret4.sh 069 'main(){a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return b*a*10;}' 180 out/compiler.out
	./test_ret4.sh 068 'main(){a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return b*a;}' 18 out/compiler.out
	./test_ret4.sh 067 'main(){a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b;}' 18 out/compiler.out
	./test_ret4.sh 001 'main(){return 123;}' 123 out/compiler.out
	./test_ret4.sh 002 'main(){return (123);}' 123 out/compiler.out
	./test_ret4.sh 003 'main(){return ((((123))));}' 123 out/compiler.out
	./test_ret4.sh 004 'main(){return 123+51;}' 174 out/compiler.out
	./test_ret4.sh 005 'main(){return 123+56-5;}' 174 out/compiler.out
	./test_ret4.sh 006 'main(){return 175-(4-3);}' 174 out/compiler.out
	./test_ret4.sh 007 'main(){return 181-4-3;}' 174 out/compiler.out
	./test_ret4.sh 008 'main(){return 0x29*3+7*8-5*1;}' 174 out/compiler.out
	./test_ret4.sh 009 'main(){return 6*(3+7)-5*1;}' 55 out/compiler.out
	./test_ret4.sh 010 'main(){return 43,6*(3+7)-5*1;}' 55 out/compiler.out
	./test_ret4.sh 011 'main(){return 43,6*(3+(4|3))-(5|1)*1;}' 55 out/compiler.out
	./test_ret4.sh 012 'main(){return 043,41*3+07*010-0Xa/(010%(1+2));}' 174 out/compiler.out
	./test_ret4.sh 013 'main(){return 7*5,(12,41*3)+7*16/(9,2)-10/(8%3);}' 174 out/compiler.out
	./test_ret4.sh 014 'main(){return 7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1+2)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	1  <<3)	%3);}'  174 out/compiler.out
	./test_ret4.sh 015 'main(){return 35,	((	41|	(8   !=     15))*  ((3==3)+2))+((5|2)*(9&10))   -   (10/(8%3));}'  174 out/compiler.out
	./test_ret4.sh 016 'main(){return 043,41*3+07*010-0Xa/(010%(!!1+2));}' 174 out/compiler.out
	./test_ret4.sh 017 'main(){return 7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1-~1)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	!0  <<3)	%3);}' 174 out/compiler.out
	./test_ret4.sh 018 'main(){return +174;}' 174 out/compiler.out
	./test_ret4.sh 019 'main(){return -(1-175);}' 174 out/compiler.out
	./test_ret4.sh 020 'main(){23; 45+37; ((12-1)*75); return -(1-175);}' 174 out/compiler.out
	./test_ret4.sh 021 'main(){23; 45+37; return -(1-175); ((12-1)*75);}' 174 out/compiler.out
	./test_ret4.sh 022 'main(){return (a = b = 9, a = 41*3, 55 - (b = 4) + a);}' 174 out/compiler.out
	./test_ret4.sh 023 'main(){a = b = c = 9; d = 5; a = 41*3; return (c, _q432 = 8, d = 11*5) - (b = 4) + a;}'  174 out/compiler.out
	./test_ret4.sh 024 'main(){return 175^1;}' 174 out/compiler.out
	./test_ret4.sh 025 'main(){return 2 + (1? 100 + 72 : 17);}' 174 out/compiler.out
	./test_ret4.sh 026 'main(){return (0? 234 : 2) + (1? 100 + 72 : 17);}' 174 out/compiler.out
	./test_ret4.sh 027 'main(){return (3, always87() + always87());}' 174 out/compiler.out
	./test_ret4.sh 028 'main(){return always87() + ((always8()* 11) -1);}' 174 out/compiler.out
	./test_ret4.sh 029 'main(){return add(170,4);}' 174 out/compiler.out
	./test_ret4.sh 030 'main(){return always87() + ((always8()* add(4,7)) -1);}' 174 out/compiler.out
	./test_ret4.sh 031 'main(){return always87() + ((always8()* subtract(12,1)) -1);}' 174 out/compiler.out
	./test_ret4.sh 032 'main(){3; {5; 7; 11; } return 175^1;}' 174 out/compiler.out
	./test_ret4.sh 033 'always87_(){return 87;} main(){return (3, always87() + always87_());}' 174 out/compiler.out
	./test_ret4.sh 034 'add_(x,y){4; return x+y;} main(){3; return add_(87,87);}' 174 out/compiler.out
	./test_ret4.sh 035 'fib(n){ return n < 2? n : fib(n - 1) + fib(n - 2); } main(){3; return fib(10);}' 55 out/compiler.out
	./test_ret4.sh 036 'tarai(x,y,z){ return x <= y? y : tarai(tarai(x-1, y, z), tarai(y-1, z, x), tarai(z-1, x, y)); } main(){return tarai(12,6,0);}' 12 out/compiler.out
	./test_ret4.sh 037 'main() { return (3 && 2 && 5) + 173; }' 174 out/compiler.out
	./test_ret4.sh 038 'main() { return (3 && 2) + !(3 && 0) + !(0 && 3)+ !(0 && 0) + 170; }' 174 out/compiler.out
	./test_ret4.sh 039 'main() { return (3 || 2 || 5) + 173; }' 174 out/compiler.out
	./test_ret4.sh 040 'main() { return (3 || 2) + (3 || 0) + (0 || 3)+ !(0 || 0) + 170; }' 174 out/compiler.out
	./test_ret4.sh 041 'main() { a = 3; a += 5;  return a + 166; }' 174 out/compiler.out
	./test_ret4.sh 042 'main() { a = 3; b = (a += 5);  return a + b + 158; }' 174 out/compiler.out
	./test_ret4.sh 043 'main() { a = 3; b = 1; b *= (a += 5);  return a + b + 158; }' 174 out/compiler.out
	./test_ret4.sh 044 'main() { a = 11; a -=5; a /= 2; b = 1; b *= (a += 5);  return a + b + 158; }' 174 out/compiler.out
	./test_ret4.sh 045 'main() { a = 7; a &= ~2; a <<= 2; a |=2; a >>= 1; a -=5; a /= 2; b = 3; c = 8; b ^= (c%=3); b *= (a += 5);  return a + b + 158; }' 174 out/compiler.out
	./test_ret4.sh 046 'foo(){ return 2;} main() {a = 3;b = 5;c = 2;if(a) {b = foo();} else { }    return 172+b;}' 174 out/compiler.out
	./test_ret4.sh 047 'foo(){ return 2;} main() {a = 3;b = 5;c = 2;if(a) {b = foo();}   return 172+b;}' 174 out/compiler.out
	./test_ret4.sh 048 'foo(){ return 2;} bar(){ return 7;} main() {a = 3;b = 5;c = 2;if(a) {b = foo();} else { c = bar();}    return 172+b;}' 174 out/compiler.out
	./test_ret4.sh 049 'foo(){ return 2;} bar(){ return 7;} main() {a = 0;b = 5;c = 2;if(a) {b = foo();} else { c = bar();}    return 162+b+c;}' 174 out/compiler.out
	./test_ret4.sh 050 'foo(){ return 2;} bar(){ return 7;} main() {a = 3;b = 5;c = 2;if(a) if(0) { b = foo(); } else {  c = bar(); }    return 162+b+c;}' 174 out/compiler.out
	./test_ret4.sh 051 'foo(){ return 2;} bar(){ return 7;} main() {a = 3;b = 5;c = 2;if(a) if(0)b=foo();else c = bar();return 162+b+c;}' 174 out/compiler.out
	./test_ret4.sh 052 'main() {a = 4; if(1){return 170+a; a = 7; }else{return 170-a; a = 9;} a = 5; return a;}' 174 out/compiler.out
	./test_ret4.sh 053 'foo(){return 1;}main(){a=0;do{a=3;}while(a==foo());return 174;}' 174 out/compiler.out
	./test_ret4.sh 054 'main(){a=0;do{a+=1;}while(a && a < 174);return a;}' 174 out/compiler.out
	./test_ret4.sh 055 'main(){a=-8;do{a+=1;}while(a);return a+174;}' 174 out/compiler.out
	./test_ret4.sh 056 'foo(){return 3;}main() {a = 0;while(a == foo()) {a = 3;}return 174;}' 174 out/compiler.out
	./test_ret4.sh 057 'main(){a = 0; b = 0; while(a <= 10) {b += a; a += 1;}return b;}' 55 out/compiler.out
	./test_ret4.sh 058 'main(){a = 3;while (a) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174 out/compiler.out
	./test_ret4.sh 059 'main(){ a = 3; b = 5; c = 0;while(a){while(b) {c += b;b-=1;if(b == 3) break;}b = 7;a-=1;if(a == 1) break;} return a*7+b*15+c*2;}' 174 out/compiler.out
	./test_ret4.sh 060 'main(){a = 3;while (a) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174 out/compiler.out
	./test_ret4.sh 061 'main(){a=11; b=0; while(a){a-=1;b+=a;if(a)continue;break; a+=100;} return b;}' 55 out/compiler.out
	./test_ret4.sh 062 'main(){a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return -a;}' 3 out/compiler.out
	./test_ret4.sh 063 'main(){a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return -b;}' 6 out/compiler.out
	./test_ret4.sh 064 'main(){a =-3; b=-6; return a*b*10+a+b+3;}' 174 out/compiler.out
	./test_ret4.sh 065 'main(){a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b*10;}' 180 out/compiler.out
	./test_ret4.sh 066 'main(){a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b*10+a+b+3;}' 174 out/compiler.out
