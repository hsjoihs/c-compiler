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
	make intmap_check
	make typeparse_check
	make assembly_sandbox
	make full_compile
	make check_error

format:
	clang-format -i *.c -style=file

clean:
	rm out/*.out s/*.s

supplement:
	gcc supplement.c -S -o s/supplement.s

init:
	mkdir -p out
	mkdir -p s


assembly_sandbox:
	make format
	clang -Wall -Wextra -Wimplicit-fallthrough assembly_sandbox.c print_x86_64.c $(CCFLAGS) -o out/assembly_sandbox.out
	./test_ret3.sh '' s/assembly_sandbox.s out/assembly_sandbox.out 174 out/assembly_sandbox.out

typeparse_check:
	clang -Wall -Wextra -Wimplicit-fallthrough  typeparse_check.c lexer.c type.c error.c -o out/typeparse_check.out
	./out/typeparse_check.out

intmap_check:
	clang -Wall -Wextra -Wimplicit-fallthrough map.c intmap_check.c -o out/intmap_check.out
	./out/intmap_check.out

warn:
	make format
	clang -Wall -Wextra -Wimplicit-fallthrough -Weverything -Wno-padded -Wno-missing-prototypes -Wno-switch-enum compiler2.c parser.c error.c type.c parse_binary_expression.c map.c print_x86_64.c $(CCFLAGS) lexer.c -o out/compiler.out

notest:
	make format
	clang -Wall -Wextra -Wimplicit-fallthrough compiler2.c parser.c error.c type.c parse_binary_expression.c map.c print_x86_64.c $(CCFLAGS) lexer.c -o out/compiler.out

full_compile:
	rm out/*.out
	make supplement
	make notest
	./test_ret4.sh 133 'char foo(){char a; return a;} int main(){foo(); return 174;}' 174
	./test_ret4.sh 134 'char foo(char *p){char a; return a;} int main(){char q; foo(&q); return 174;}' 174
	./test_ret4.sh 135 'char foo(char *p){char a; a = 5; return a;} int main(){char q; foo(&q); return 174;}' 174
	./test_ret4.sh 136 'int main(){char x[3]; x[0] = -1; x[1] = 2; int y; y = 4; return x[0] + y + 171;}' 174
	./test_ret4.sh 137 'char foo(char *p){*p = 5; char a;a = 3; return a;} int main(){char q; char r; r = foo(&q); return 172-r+q;}' 174
	./test_ret4.sh 138 'char a;char foo(char *p){*p = 5; a = 3; return a;} int main(){char q; char r; r = foo(&q); return 172-r+q;}' 174
	./test_ret4.sh 139 'int foo(char a){int d;d = 3;char c;c = a+d;return c;} int main(){char f;f=3;return foo(f)*4+150;}' 174
	./test_ret4.sh 140 'int foo(char a){int d;d = 3;char c;c = a+d;return c*4;} int main(){char f;f=3;return foo(f)+150;}' 174
	./test_ret4.sh 141 'int qwerty(); int main(){char f;f=3;return qwerty(f,4)+150;}' 174
	./test_ret4.sh 142 'int qwer(); int main(){char f;f=3;return qwer(f,4)+150;}' 174
	./test_ret4.sh 143 'int foo(char a, char b){return 23;} int main(){char f;f=3;return foo(f,4)+151;}' 174
	./test_ret4.sh 144 'int foo(char a, char b){return a*4+11;} int main(){char f;f=3;return foo(f,4)+151;}' 174
	./test_ret4.sh 145 'int foo(char a, char b){return a*4+12;} int main(){char f;f=3;return foo(f,4)+150;}' 174
	./test_ret4.sh 146 'int foo(char a, char b){return (a+3)*4;} int main(){char f;f=3;return foo(f,4)+150;}' 174
	./test_ret4.sh 147 'int foo(char a, char b){char c;c = a+3;return c*4;} int main(){char f;f=3;return foo(f,4)+150;}' 174
	./test_ret4.sh 148 'int foo(char a, char b){int d;d = 3;char c;c = a+d;return c*4;} int main(){char f;f=3;return foo(f,4)+150;}' 174
	./test_ret4.sh 149 'int foo(char a, char b){int d;d = 3;char c;c = a+d;return c*b;} int main(){char f;f=3;return foo(f,4)+150;}' 174
	./test_ret4.sh 150 'char foo() { char *x;x = "1ab"; return x[0]; }int main(){ char *y;y = "a2b"; int z;z = 12; char a;a = y[1]; return (a-foo())*z+162;}' 174

	./test_ret4.sh 118 'int main(){int a[1]; int *p; p = a; *p=2; return 174;}' 174
	./test_ret4.sh 119 'int main(){int a[1]; *(a+0)=2;return 174;}' 174
	./test_ret4.sh 120 'int x; int *y; int main(){x=3; int a[1]; *a=2; y=a; return x+*y+169;}' 174
	./test_ret4.sh 121 'int x; int main(){x=3; int *y; y=&x; return *y+171;}' 174
	./test_ret4.sh 122 'int a[1]; int main(){ *a=2;return 174;}' 174
	./test_ret4.sh 123 'int main(){int a[1][2];int (*p)[2];p = a;int *q;q = *p;return 174;}' 174
	./test_ret4.sh 124 'int main(){int a[1][2];int (*p)[2];p = a;int *q;q = *p; *q=174; return **a;}' 174
	./test_ret4.sh 125 'int main(){int a[1][2];int (*p)[2];p = a;int *q;q = *(p+1); *q=174; return **(a+1);}' 174
	./test_ret4.sh 126 'int main(){int a[5][6];int (*p)[6];p = a;int *q;q = *(p+1); *(2+q)=174; return *(*(1+a)+2);}' 174
	./test_ret4.sh 127 'int changeBoard(int (*board)[30], int i, int j, int d, int N){int k;for (k = 0; k < N; k++) {*(*(board + i) + k) += d;*(*(board + k) + j) += d;}if (i > j) {for (k = 0; k < N - (i - j); k++) {*(*(board + k + (i - j)) + k) += d;}} else {for (k = 0; k < N - (j - i); k++) {*(*(board + k) + k + (j - i)) += d;}}if (i + j < N) {for (k = 0; k <= i + j; k++) {*(*(board + i + j - k) + k) += d;}} else {for (k = i + j - N + 1; k < N; k++) {*(*(board + i + j - k) + k) += d;}}return 0;}int setQueen(int (*board)[30], int num_placed, int *ptr_sol_num, int N){int j;if (num_placed == N) {(*ptr_sol_num)+=1;return 0;}for (j = 0; j < N; j++) {if (*(*(board+num_placed)+j) == 0) {changeBoard(board, num_placed, j, +1, N);setQueen(board, num_placed + 1, ptr_sol_num, N);changeBoard(board, num_placed, j, -1, N);}}return 0;}int board_[30][30];int main(){int sol_num;sol_num = 0;setQueen(board_, 0, &sol_num, 8);return sol_num;}' 92
	./test_ret4.sh 128 'int count;int solve(int n, int col, int *hist){if (col == n) {count+=1;return 0;}int i;int j;for (i = 0, j = 0; i < n; i++) {for (j = 0; j < col && *(hist + j) != i && (*(hist + j) - i) != col - j && (*(hist + j) - i) != j - col; j++){}if (j < col)continue;*(hist+col) = i;solve(n, col + 1, hist);}return 0;}int main(){int hist[8];solve(8, 0, hist);return count;}' 92
	./test_ret4.sh 129 'int count;int solve(int n, int col, int *hist){if (col == n) {count+=1;return 0;}int i;int j;for (i = 0, j = 0; i < n; i++) {for (j = 0; j < col && *(hist + j) != i && (hist [j] - i) != col - j && (*(hist + j) - i) != j - col; j++){}if (j < col)continue;*(hist+col) = i;solve(n, col + 1, hist);}return 0;}int main(){int hist[8];solve(8, 0, hist);return count;}' 92
	./test_ret4.sh 130 'int count;int solve(int n, int col, int *hist){if (col == n) {count+=1;return 0;}int i;int j;for (i = 0, j = 0; i < n; i++) {for (j = 0; j < col && hist [j] != i && (hist [j] - i) != col - j && (hist[j] - i) != j - col; j++){}if (j < col)continue;hist[col] = i;solve(n, col + 1, hist);}return 0;}int main(){int hist[8];solve(8, 0, hist);return count;}' 92
	./test_ret4.sh 131 'int changeBoard(int (*board)[30], int i, int j, int d, int N){int k;for (k = 0; k < N; k++) {board[i][k] += d;board[k][j] += d;}if (i > j) {for (k = 0; k < N - (i - j); k++) {board [k + (i - j)][k] += d;}} else {for (k = 0; k < N - (j - i); k++) {board[k][k + (j - i)] += d;}}if (i + j < N) {for (k = 0; k <= i + j; k++) {board[i + j - k][k] += d;}} else {for (k = i + j - N + 1; k < N; k++) {board[i + j - k][k] += d;}}return 0;}int setQueen(int (*board)[30], int num_placed, int *ptr_sol_num, int N){int j;if (num_placed == N) {(*ptr_sol_num)+=1;return 0;}for (j = 0; j < N; j++) {if (board[num_placed][j] == 0) {changeBoard(board, num_placed, j, +1, N);setQueen(board, num_placed + 1, ptr_sol_num, N);changeBoard(board, num_placed, j, -1, N);}}return 0;}int board_[30][30];int main(){int sol_num;sol_num = 0;setQueen(board_, 0, &sol_num, 8);return sol_num;}' 92
	./test_ret4.sh 132 'int main(){int a[5][6];int (*p)[6];p = a;int *q;q = p[1]; 2[q]=174; return 1[a][2];}' 174

	./test_ret4.sh 001 'int main(){return 123;}' 123
	./test_ret4.sh 002 'int main(){return (123);}' 123
	./test_ret4.sh 003 'int main(){return ((((123))));}' 123
	./test_ret4.sh 004 'int main(){return 123+51;}' 174
	./test_ret4.sh 005 'int main(){return 123+56-5;}' 174
	./test_ret4.sh 006 'int main(){return 175-(4-3);}' 174
	./test_ret4.sh 007 'int main(){return 181-4-3;}' 174
	./test_ret4.sh 008 'int main(){return 0x29*3+7*8-5*1;}' 174
	./test_ret4.sh 009 'int main(){return 6*(3+7)-5*1;}' 55
	./test_ret4.sh 010 'int main(){return 43,6*(3+7)-5*1;}' 55
	./test_ret4.sh 011 'int main(){return 43,6*(3+(4|3))-(5|1)*1;}' 55
	./test_ret4.sh 012 'int main(){return 043,41*3+07*010-0Xa/(010%(1+2));}' 174
	./test_ret4.sh 013 'int main(){return 7*5,(12,41*3)+7*16/(9,2)-10/(8%3);}' 174
	./test_ret4.sh 014 'int main(){return 7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1+2)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	1  <<3)	%3);}'  174
	./test_ret4.sh 015 'int main(){return 35,	((	41|	(8   !=     15))*  ((3==3)+2))+((5|2)*(9&10))   -   (10/(8%3));}'  174
	./test_ret4.sh 016 'int main(){return 043,41*3+07*010-0Xa/(010%(!!1+2));}' 174
	./test_ret4.sh 017 'int main(){return 7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1-~1)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	!0  <<3)	%3);}' 174
	./test_ret4.sh 018 'int main(){return +174;}' 174
	./test_ret4.sh 019 'int main(){return -(1-175);}' 174
	./test_ret4.sh 020 'int main(){23; 45+37; ((12-1)*75); return -(1-175);}' 174
	./test_ret4.sh 021 'int main(){23; 45+37; return -(1-175); ((12-1)*75);}' 174
	./test_ret4.sh 022 'int main(){int a; int b; return (a = b = 9, a = 41*3, 55 - (b = 4) + a);}' 174
	./test_ret4.sh 023 'int main(){int a; int b; int c; int d; int _q432; a = b = c = 9; d = 5; a = 41*3; return (c, _q432 = 8, d = 11*5) - (b = 4) + a;}'  174
	./test_ret4.sh 024 'int main(){return 175^1;}' 174
	./test_ret4.sh 025 'int main(){return 2 + (1? 100 + 72 : 17);}' 174
	./test_ret4.sh 026 'int main(){return (0? 234 : 2) + (1? 100 + 72 : 17);}' 174
	./test_ret4.sh 027 'int always87(); int main(){return (3, always87() + always87());}' 174
	./test_ret4.sh 028 'int always87();int always8();int main(){return always87() + ((always8()* 11) -1);}' 174
	./test_ret4.sh 029 'int add();int main(){return add(170,4);}' 174
	./test_ret4.sh 030 'int always87();int always8();int add();int main(){return always87() + ((always8()* add(4,7)) -1);}' 174
	./test_ret4.sh 031 'int always87();int always8();int subtract();int main(){return always87() + ((always8()* subtract(12,1)) -1);}' 174
	./test_ret4.sh 032 'int main(){3; {5; 7; 11; } return 175^1;}' 174
	./test_ret4.sh 033 'int always87();int always87_(){return 87;} int main(){return (3, always87() + always87_());}' 174
	./test_ret4.sh 034 'int add_(int x, int y){4; return x+y;} int main(){3; return add_(87,87);}' 174
	./test_ret4.sh 035 'int fib(int n){ return n < 2? n : fib(n - 1) + fib(n - 2); } int main(){3; return fib(10);}' 55
	./test_ret4.sh 036 'int tarai(int x,int y,int z){ return x <= y? y : tarai(tarai(x-1, y, z), tarai(y-1, z, x), tarai(z-1, x, y)); } int main(){return tarai(12,6,0);}' 12
	./test_ret4.sh 037 'int main() { return (3 && 2 && 5) + 173; }' 174
	./test_ret4.sh 038 'int main() { return (3 && 2) + !(3 && 0) + !(0 && 3)+ !(0 && 0) + 170; }' 174
	./test_ret4.sh 039 'int main() { return (3 || 2 || 5) + 173; }' 174
	./test_ret4.sh 040 'int main() { return (3 || 2) + (3 || 0) + (0 || 3)+ !(0 || 0) + 170; }' 174
	./test_ret4.sh 041 'int main() {int a; a = 3; a += 5;  return a + 166; }' 174
	./test_ret4.sh 042 'int main() {int a; int b; a = 3; b = (a += 5);  return a + b + 158; }' 174
	./test_ret4.sh 043 'int main() {int a; int b; a = 3; b = 1; b *= (a += 5);  return a + b + 158; }' 174
	./test_ret4.sh 044 'int main() {int a; int b; a = 11; a -=5; a /= 2; b = 1; b *= (a += 5);  return a + b + 158; }' 174
	./test_ret4.sh 045 'int main() {int a; int b; int c; a = 7; a &= ~2; a <<= 2; a |=2; a >>= 1; a -=5; a /= 2; b = 3; c = 8; b ^= (c%=3); b *= (a += 5);  return a + b + 158; }' 174
	./test_ret4.sh 046 'int foo(){ return 2;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();} else { }    return 172+b;}' 174
	./test_ret4.sh 047 'int foo(){ return 2;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();}   return 172+b;}' 174
	./test_ret4.sh 048 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();} else { c = bar();}    return 172+b;}' 174
	./test_ret4.sh 049 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 0;b = 5;c = 2;if(a) {b = foo();} else { c = bar();}    return 162+b+c;}' 174
	./test_ret4.sh 050 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) if(0) { b = foo(); } else {  c = bar(); }    return 162+b+c;}' 174
	./test_ret4.sh 051 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) if(0)b=foo();else c = bar();return 162+b+c;}' 174
	./test_ret4.sh 052 'int main() {int a; a = 4; if(1){return 170+a; a = 7; }else{return 170-a; a = 9;} a = 5; return a;}' 174
	./test_ret4.sh 053 'int foo(){return 1;} int main(){int a; a=0;do{a=3;}while(a==foo());return 174;}' 174
	./test_ret4.sh 054 'int main(){int a; a=0;do{a+=1;}while(a && a < 174);return a;}' 174
	./test_ret4.sh 055 'int main(){int a; a=-8;do{a+=1;}while(a);return a+174;}' 174
	./test_ret4.sh 056 'int foo(){return 3;} int main() {int a; a = 0;while(a == foo()) {a = 3;}return 174;}' 174
	./test_ret4.sh 057 'int main(){int a; int b; a = 0; b = 0; while(a <= 10) {b += a; a += 1;}return b;}' 55
	./test_ret4.sh 058 'int main(){int a; a = 3;while (a) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174
	./test_ret4.sh 059 'int main(){int a; int b; int c; a = 3; b = 5; c = 0;while(a){while(b) {c += b;b-=1;if(b == 3) break;}b = 7;a-=1;if(a == 1) break;} return a*7+b*15+c*2;}' 174
	./test_ret4.sh 060 'int main(){int a; a = 3;while (a) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174
	./test_ret4.sh 061 'int main(){int a; int b; a=11; b=0; while(a){a-=1;b+=a;if(a)continue;break; a+=100;} return b;}' 55
	./test_ret4.sh 062 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return -a;}' 3
	./test_ret4.sh 063 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return -b;}' 6
	./test_ret4.sh 064 'int main(){int a; int b; a =-3; b=-6; return a*b*10+a+b+3;}' 174
	./test_ret4.sh 065 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b*10;}' 180
	./test_ret4.sh 066 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b*10+a+b+3;}' 174
	./test_ret4.sh 067 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b;}' 18
	./test_ret4.sh 068 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return b*a;}' 18
	./test_ret4.sh 069 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return b*a*10;}' 180
	./test_ret4.sh 070 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break;}while(a+3); return a*b;}' 18
	./test_ret4.sh 071 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(!a)break;}while(a+3); return a*b;}' 18
	./test_ret4.sh 072 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;}while(a+3); return a*b;}' 18
	./test_ret4.sh 073 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;}while(a+3); return b*a;}' 18
	./test_ret4.sh 074 'int main(){int a; int b; a=3; b=0; b+= ++a; return a*b*11-2;}' 174
	./test_ret4.sh 075 'int main(){int a; int b; a=3; b=0; b+= a++; return !(b-3)+!(a-4)+172;}' 174
	./test_ret4.sh 076 'int main(){int a; int b; a =0; b=0; do{b+=--a;}while(a+3); return b*a;}' 18
	./test_ret4.sh 077 'int main(){int a; for (a = 3;a;) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174
	./test_ret4.sh 078 'int main(){int a; for (a = 3;;) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174
	./test_ret4.sh 079 'int main(){int a; int b; for(a=0,b=0;a <= 10;) {b += a; a += 1;}return b;}' 55
	./test_ret4.sh 080 'int main(){int a; int b; for(a=11, b=0;a;){a-=1;b+=a;if(a)continue;break; a+=100;} return b;}' 55
	./test_ret4.sh 081 'int main(){int a; int b; for(a=0,b=0;a <= 10;++a) {b += a;}return b;}' 55
	./test_ret4.sh 082 'int main(){int a; int b; for(a=0,b=0;a <= 10;a++) {b += a;}return b;}' 55
	./test_ret4.sh 083 'int main(){int a; int b; int c; int d; d=0; b = 5; c = 0;for(a = 3;a;d++){for(;b;++d) {c += b;b-=1;if(b == 3) break;}b = 7;a-=1;if(a == 1) break;} return a*7+b*15+c*2;}' 174
	./test_ret4.sh 084 'int main(){int a; int b; for(a=0,b=0;a<10;a++){ if(a ==5)continue;b+=a;} return b;}' 40
	./test_ret4.sh 085 'int main(){int a; a = 174; {int a; a = 3;} return a;}' 174
	./test_ret4.sh 086 'int main(){int a; a = 3; { a = 174;} return a;}' 174
	./test_ret4.sh 087 'int main() {int *b; int a; a = 3; a += 5;  return a + 166; }' 174
	./test_ret4.sh 088 'int main() {int *******b; int a; a = 3; a += 5;  return a + 166; }' 174
	./test_ret4.sh 089 'int main() {int a; a = 174; int *b; b = &a; return a;}' 174
	./test_ret4.sh 090 'int main(){int x;x = 86;int *y;y = &x; return (*y) + x + 2;}' 174
	./test_ret4.sh 091 'int main(){int x;x = 86;int *y;y = &x; return (*y) + (*y) + 2;}' 174
	./test_ret4.sh 092 'int main(){int x;x = 86;int *y;y = &x;int **z;z = &y;return (*y) + (**z) + 2;}' 174
	./test_ret4.sh 093 'int main(){int x;x = 86;int *y;y = &x;int **z;z = &y;return*y+**z+2;}' 174
	./test_ret4.sh 094 'int main() {int x;int *y;x = 3;y = &x;*y = 174;return x;}' 174
	./test_ret4.sh 095 'int main() {int x;int *y;x = 3;y = &x;*y = 171;*y += 3;return x;}' 174
	./test_ret4.sh 096 'int main(){int x; int y; int *z; int*a; z=&x; a=&y; *z=*a=87; return(x+y);}' 174
	./test_ret4.sh 097 'int main(){int x; int *y; int **z; z = &y; *z = &x; *y = 174; return x;}' 174
	./test_ret4.sh 098 'int foo(int* p){return 3;} int main(){int x; return 174;}' 174
	./test_ret4.sh 099 'int foo(int* p){return *p;} int main(){int x; x = 174; return foo(&x);}' 174
	./test_ret4.sh 100 'int foo(int* p){*p = 172; return *p+2;} int main(){int x; return foo(&x);}' 174
	./test_ret4.sh 101 'int *foo(int *p){*p = 4;return p;} int main(){int x;int *y;y = foo(&x); *y+= 170;return x;}' 174
	./test_ret4.sh 102 'int *foo(int *p){*p = 4;return p;} int main(){int x;int y;*foo(&x) += 170;return x;}' 174
	./test_ret4.sh 103 'int *alloc4();int main(){int *p;p = alloc4(62, 8, 31, 85);return *p;}' 62
	./test_ret4.sh 104 'int *alloc4();int main(){int *p;int *r;p = alloc4(62, 8, 31, 85);int *q;q = p;return *q;}' 62
	./test_ret4.sh 105 'int *alloc4();int main(){int *p;int *r;p = alloc4(62, 8, 31, 85);int *q;q = p + 2;return *q;}' 31
	./test_ret4.sh 106 'int *alloc4();int main(){int *p;int c;int e;c = 2;int d;d = 1;p = alloc4(62, 8, 31, 85);int *q;q = p + c;return *(q+d) - *q + (*p-2)*2;}' 174
	./test_ret4.sh 107 'int *alloc4();int main(){int *p;p = alloc4(62, 8, 31, 85);int *q;q = p;return *q;}' 62
	./test_ret4.sh 108 'int *alloc4();int main(){int *p;p = alloc4(62, 8, 31, 85);int *q;q = p + 2;return *q;}' 31
	./test_ret4.sh 109 'int *alloc4();int main(){int *p;int c;c = 2;int d;d = 1;p = alloc4(62, 8, 31, 85);int *q;q = p + c;return *(q+d) - *q + (*p-2)*2;}' 174
	./test_ret4.sh 110 'int *alloc4();int main(){int *p;int c;c = 2;int d;d = 1;p = alloc4(62, 8, 31, 85);int *q;q = c + p;return *(d+q) - *q + (*p-2)*2;}' 174
	./test_ret4.sh 111 'int *alloc4();int main(){int *p;int c;c = -2;int d;d = 1;p = alloc4(62, 8, 31, 85);int *q;q = p - c;return *(d+q) - *q + (*p-2)*2;}' 174
	./test_ret4.sh 112 'int *alloc4();int main(){int *p; int *q; q = p+174; return q-p;}' 174
	./test_ret4.sh 113 'int *foo(int *(p)){*p = 4;return p;} int main(){int (x);int (y); int (*(*(z))); *foo(&x) += 170;return x;}' 174
	./test_ret4.sh 114 'int main(){int a[2][3]; return 174;}' 174
	./test_ret4.sh 115 'int x; int *y; int main(){return 174;}' 174
	./test_ret4.sh 116 'int x; int *y; int main(){return x+174;}' 174
	./test_ret4.sh 117 'int x; int *y; int main(){x=3; int a; a=2; y=&a; return x+*y+169;}' 174

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
	./test_compile_error.sh 'int main(){int x;int *y;*y = &x; return x;}'
	./test_compile_error.sh 'int main(){int x; int *y; y = &x;int **z; *z = x;}'
	./test_compile_error.sh 'int *foo(){return 1;}int main(){return 0;}'
	./test_compile_error.sh 'int *foo(){int x; return x;}int main(){return 0;}'
	./test_compile_error.sh 'int foo(){int *x; return x;}int main(){return 0;}'
	./test_compile_error.sh 'int *foo(){int *x; return x;}int main(){return foo();}'
	./test_compile_error.sh 'int *foo(){int *x; return x;}int main(){int x; x= foo();}'
	./test_compile_error.sh 'int main(){int *x; int *y; x+y;}'
	./test_compile_error.sh 'int *alloc4();int main(){int *p; int *q; q = p+174; return q-p;'
	./test_compile_error.sh 'int *foo(int *(p)){*p = 4;return p;} int main(){int (x;int (y); int (*(*(z))); *foo(&x) += 170;return x;}'
	./test_compile_error.sh 'int main(){int *p; int (*a)[1][2]; p = a;}'
	./test_compile_error.sh 'int main(){int a[1]; int *b; a = b;}'
	./test_compile_error.sh 'int main(){return 1[2];}'
	


