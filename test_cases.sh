#!/bin/bash

run_test() {
	echo -e $2 | ./out/compiler.out > s/full_compile$1.s
	gcc s/full_compile$1.s s/supplement.s -o out/task$1.out
	./out/task$1.out
	res=$?
	if [ $res -ne $3 ]; then { echo "got:" $res; echo "expected:" $3; echo -e "\033[31mFAIL\033[m, at test case" $1: $2; exit 1; }; else echo -e "\033[32mPASS\033[m"; fi
}

run_test 247 'int (*func(int (*a)[5]))[5]{return a;} int main(){int a[6][5]; a[1][2] = 174; return func(a)[1][2];}' 174

run_test 239 'int *foo(int *(p)){*p = 4;return p;} int main(){int (x);int (y); int (*(*(z))); *foo(&x) += 170;return x;}' 174
run_test 240 'int main(){int a[1][2];int (*p)[2];p = a;int *q;q = *p;return 174;}' 174
run_test 241 'int main(){int a[1][2];int (*p)[2];p = a;int *q;q = *p; *q=174; return **a;}' 174
run_test 242 'int main(){int a[1][2];int (*p)[2];p = a;int *q;q = *(p+1); *q=174; return **(a+1);}' 174
run_test 243 'int main(){int a[5][6];int (*p)[6];p = a;int *q;q = *(p+1); *(2+q)=174; return *(*(1+a)+2);}' 174
run_test 244 'int changeBoard(int (*board)[30], int i, int j, int d, int N){int k;for (k = 0; k < N; k++) {*(*(board + i) + k) += d;*(*(board + k) + j) += d;}if (i > j) {for (k = 0; k < N - (i - j); k++) {*(*(board + k + (i - j)) + k) += d;}} else {for (k = 0; k < N - (j - i); k++) {*(*(board + k) + k + (j - i)) += d;}}if (i + j < N) {for (k = 0; k <= i + j; k++) {*(*(board + i + j - k) + k) += d;}} else {for (k = i + j - N + 1; k < N; k++) {*(*(board + i + j - k) + k) += d;}}return 0;}int setQueen(int (*board)[30], int num_placed, int *ptr_sol_num, int N){int j;if (num_placed == N) {(*ptr_sol_num)+=1;return 0;}for (j = 0; j < N; j++) {if (*(*(board+num_placed)+j) == 0) {changeBoard(board, num_placed, j, +1, N);setQueen(board, num_placed + 1, ptr_sol_num, N);changeBoard(board, num_placed, j, -1, N);}}return 0;}int board_[30][30];int main(){int sol_num;sol_num = 0;setQueen(board_, 0, &sol_num, 8);return sol_num;}' 92
run_test 245 'int changeBoard(int (*board)[30], int i, int j, int d, int N){int k;for (k = 0; k < N; k++) {board[i][k] += d;board[k][j] += d;}if (i > j) {for (k = 0; k < N - (i - j); k++) {board [k + (i - j)][k] += d;}} else {for (k = 0; k < N - (j - i); k++) {board[k][k + (j - i)] += d;}}if (i + j < N) {for (k = 0; k <= i + j; k++) {board[i + j - k][k] += d;}} else {for (k = i + j - N + 1; k < N; k++) {board[i + j - k][k] += d;}}return 0;}int setQueen(int (*board)[30], int num_placed, int *ptr_sol_num, int N){int j;if (num_placed == N) {(*ptr_sol_num)+=1;return 0;}for (j = 0; j < N; j++) {if (board[num_placed][j] == 0) {changeBoard(board, num_placed, j, +1, N);setQueen(board, num_placed + 1, ptr_sol_num, N);changeBoard(board, num_placed, j, -1, N);}}return 0;}int board_[30][30];int main(){int sol_num;sol_num = 0;setQueen(board_, 0, &sol_num, 8);return sol_num;}' 92
run_test 246 'int main(){int a[5][6];int (*p)[6];p = a;int *q;q = p[1]; 2[q]=174; return 1[a][2];}' 174



run_test 203 'int ptrdiff(); int main(){int *p; p = 0; return ptrdiff(p+1, p);}' 4
run_test 204 'int main(){int *p; p = 0; if(p) {return 4; } return 174;}' 174
run_test 205 'int main(){int *p; int a; p = &a; if(p) {return 4; } return 174;}' 4
run_test 206 'int main(){int *p; int a; p = &a; return p && &p;}' 1
run_test 207 'int main(){int *p; int a; p = &a; return p || &p;}' 1
run_test 208 'int main(){int *p; int a; p = &a; return p?174:1;}' 174
run_test 209 'int main(){int *p; p = 0; return p?174:1;}' 1
run_test 210 'int main(void){return 174;}' 174
run_test 211 'int main(void){void *p; p = 0; p = p; return 174;}' 174
run_test 212 'struct A{int a; int b;}; int main(){ struct A *p; void *q1; void *q2; q1 = p; q2 = p+1; char *r1; char *r2; r1 = q1; r2 = q2; return r2-r1;}' 8
run_test 213 'void f(int *p){*p = 174; return;} int main(void){ int a; f(&a); return a;}' 174
run_test 214 'int main(void){ foo: return 174;}' 174
run_test 215 'int main(void){ foo: bar: return 174;}' 174
run_test 216 'int main(void){ foo: {baz: hoge: 1;} bar: return 174;}' 174
run_test 217 'int main(void){ int a; a = 174; switch(1){a = 2; 1;} return a;}' 174
run_test 218 'int main(void){ int a; a = 174; switch(1){a = 2; break; a = 3;} return a;}' 174
run_test 219 'int main(void){ int a; a = 1; int b; b = 0; switch(1){ b = 15; default: a = 174; break; a = 3;} return a+b ;}' 174
run_test 220 'int main(void){ switch(1){ if(0){ default: return 174; } } return 3; }' 174
run_test 221 'int main(void){ int a; a = 1; switch(1){ default: a = 173; switch(0){ default: return a+1; } return 5; } return 3; }' 174
run_test 222 'int main(void){ int a; a = 1; switch(1){ case 1: a = 174; } return a; }' 174
run_test 223 'int main(void){ int a; a = 174; switch(2){ case 1: a = 1; } return a; }' 174
run_test 224 'int f(int a){switch(a){case 1: return 3; case 2: return 5; default: return 8;}} int main(void){ return (f(1)-3) || (f(2)-5) || (f(3)-8) || (f(100)-8);}' 0
run_test 225 'int main(){return _Alignof(int);}' 4
run_test 226 'int main(){return _Alignof(int*);}' 8
run_test 227 'struct A{int a; int b;}; int main(){ return _Alignof(struct A);}' 4
run_test 228 'struct A{int a; char c; char d; int b;}; int main(){ return _Alignof(struct A);}' 4
run_test 229 'struct A{int a; int *b; int c;}; int main(){return _Alignof(struct A [5]);}' 8
run_test 230 'void f(int *p){*p = 174;} int main(void){ int a; f(&a); return a;}' 174
run_test 231 'int main(void){ char a; a = 0; switch(a){case 0: a = 174; break; case 256: a = 3; break; default: a = 5; break;}  return a;}' 174
run_test 232 'enum A{B, C}; int main(void){ enum A b; return 174; }' 174
run_test 233 'enum A{B, C,}; int main(void){ enum A b; return 174; }' 174
run_test 234 'enum A{B, C,}; int main(void){ enum A b; b = 5; return 174; }' 174
run_test 235 'enum A{B, C,}; int main(void){ enum A b; b = B; return 174+b; }' 174
run_test 236 'enum A{B, C,D}; int main(void){ enum A b; b = D; return 172+b; }' 174
run_test 237 'enum A{B, C,D}; int f(enum A b){switch(b){case B: return 1; case C: return 5; case D: return 8;}} int main(void){ return (f(B) - 1) || (f(C) - 5) || (f(D) - 8);}' 0
run_test 238 'int main(){int a[5]; if(a){return 174;} return 0;}' 174


run_test 190 'int main(){return sizeof(int);}' 4
run_test 191 'int main(){return sizeof(int*);}' 8
run_test 192 'struct A{int a; int b;}; int main(){ return sizeof(struct A);}' 8
run_test 193 'struct A{int a; char c; char d; int b;}; int main(){ return sizeof(struct A);}' 12
run_test 194 'struct A{int a; int *b; int c;}; int main(){return sizeof(struct A [5]);}' 120
run_test 195 'struct A{int a; int *b; int c;}; struct B{char d; struct A e;}; int main(){return sizeof(struct B [4]);}' 128
run_test 196 'int *f(int *p){return p;} int main(){int a[4]; a[0] = 1; f(a)[0]++; f(a)[1] = 172; return a[1]+a[0];}' 174 
run_test 197 'struct A{char a; int b;}; int main(){struct A a; a.a = 74; return a.a;}' 74
run_test 198 'struct A{int a; int b;}; int main(){struct A a; a.a = 174; return a.a;}' 174
run_test 199 'struct A{int a; int b;}; int main(){struct A a; a.a = 174; return a.a;}' 174
run_test 200 'int add_two_ints(); struct A{int a; int b;}; int main(){struct A a; a.a = 100; a.b = 74; return add_two_ints(&a);}' 174
run_test 201 'int add_two_ints2(); struct A{int a; char c; char d; int b;}; int main(){struct A a; a.a = 100; a.b = 74; return add_two_ints2(&a);}' 174
run_test 202 'struct A{int a; char c; char d; int b;}; struct A *get_struct_pointer(); int main(){ struct A *p; p = get_struct_pointer(100, 74); return p->a + p->b;  }' 174


run_test 169 'int a(int b){ return b; }int main(){int i; i=1; a(i == 1? 1 : 2); return 0;}' 0
run_test 170 'int a(int b){ return b; }int main(){int i; for (i = 1; i < 11; i++) { a(i == 1? 1 : 2); } return 0;}' 0
run_test 171 'int a(int b){ return b; }int main(){int i; i=1; return a(i == 1? 174 : 2);}' 174

run_test 172 'int printf();int puts();int count;int main(){int i; int hist[20]; for (i = 1; i < 11; i++) { printf(i == 1? "a" : "b"); puts("");} return 0;}' 0
run_test 173 'int printf();int puts();int count;int main(){int i; int hist[20]; for (i = 1; i < 11; i++) { printf("%s", (i == 1? "a" : "b")); puts("");} return 0;}' 0
run_test 174 'int printf();int puts();int count;int main(){int i; int hist[20]; for (i = 1; i < 11; i++) { printf("%d %s: %d", i, (i == 1? " " : "s "), i); puts("");} return 0;}' 0
run_test 175 'int printf();int puts();int count;int solve(int n, int col, int *hist){if (col == n) {count+=1;return 0;}int i;int j;for (i = 0, j = 0; i < n; i++) {for (j = 0; j < col && hist [j] != i && (hist [j] - i) != col - j && (hist[j] - i) != j - col; j++){}if (j < col)continue;hist[col] = i;solve(n, col + 1, hist);}return 0;}int main(){int i; int hist[20]; for (i = 1; i < 11; i++) { count=0; solve(i, 0, hist); printf("%d queen%s: %d", i, (i == 1? " " : "s "), count); puts("");} return 0;}' 0
run_test 176 'int main(){int a; int *p; p = &a; *p = 2; int *q; q = &*p; *q = 174; return a;}' 174
run_test 177 'int main(){int a; int *p; p = &a; *p = 2; int *q; q = &(*p); *q = 174; return a;}' 174
run_test 178 'char foo(char *p){char a; return a;} int main(){char q; foo(&(q)); return 174;}' 174
run_test 179 'char; char     ; char; int; int ; int; int;int;char foo(char *p){char a; return a;} int main(){char q; foo(&(q)); return 174;}' 174
run_test 180 ' struct A; char; char     ; char; int; int ; int; struct B;  int;int;  struct C; int main(){return 174;}' 174
run_test 181 ' struct A{int a; int b;}; char; char     ; char; int; int ; int; struct B{int c; int b;};  int;int;  struct C; int main(){return 174;}' 174
run_test 182 'int main(){ int; return 174;}' 174
run_test 183 'struct A{int a; int b;}; int main(){ struct A; return 174;}' 174
run_test 184 'struct A{int a; int b;}; int main(){ struct A a; return 174;}' 174
run_test 185 'struct A{int a; int b;}; int main(){ struct A a[10]; return 174;}' 174
run_test 186 'struct A{int a; int b;};  struct A a[10]; int main(){return 174;}' 174
run_test 187 'int ptrdiff();struct A{int a; int b;}; int main(){ struct A *p; return ptrdiff(p+1, p);}' 8
run_test 188 'int ptrdiff();struct A{int a; char c; char d; int b;}; int main(){ struct A *p; return ptrdiff(p+1, p);}' 12
run_test 189 'int ptrdiff();struct A{int a; int *b; int c;};  struct A a[1][5]; int main(){return ptrdiff(a+1, a);}' 120



run_test 162 'int printf();int a() {return 3;}int main() {int i; printf("%d %d", i, a()); return 0;}' 0
run_test 163 'int foo(char *a, int b, int c){return 0;} int a(int N) {return 3;}int main() {int i; foo("%d %d", i, a(i)); return 0;}' 0
run_test 164 'int foobar();int a(int N) {return 3;}int main() {int i; foobar("%d %d", i, a(i)); return 0;}' 0
run_test 165 'int printf();int a(int N) {return 3;}int main() {int i; printf("%d %d", i, a(i)); return 0;}' 0
run_test 166 'int printf();int puts();int a(int N) {return 3;}int main() {int i; for (i = 1; i <= 12; i++) { printf("%d %d", i, a(i)); puts("");} return 0;}' 0
run_test 167 'int printf();int puts();int A[200][200];int a(int row, int N) {return 3;}int main() {int i; for (i = 1; i <= 12; i++) { printf("%d %d", i, a(0, i)); puts("");} return 0;}' 0
run_test 168 'int printf();int puts();int A[200][200];int dfs(int row, int N) { if (row == N) return 1; int ret;ret = 0; int col;for (col = 0; col < N; col++) { int ok; ok = 1; int i; for (i = 1; i < N; i++) { if (row - i >= 0 && col - i >= 0) { ok = ok && A[row - i][col - i] == 0; } if (row - i >= 0) { ok = ok && A[row - i][col] == 0; } if (row - i >= 0 && col + i < N) { ok = ok && A[row - i][col + i] == 0; } } if (ok) { A[row][col] = 1; ret += dfs(row + 1, N); A[row][col] = 0; } } return ret;}int main() {int i; for (i = 1; i < 12; i++) { printf("%d queen: %d", i, dfs(0, i)); puts("");} return 0;}' 0


run_test 133 'char foo(){char a; return a;} int main(){foo(); return 174;}' 174
run_test 134 'char foo(char *p){char a; return a;} int main(){char q; foo(&q); return 174;}' 174
run_test 135 'char foo(char *p){char a; a = 5; return a;} int main(){char q; foo(&q); return 174;}' 174
run_test 136 'int main(){char x[3]; x[0] = -1; x[1] = 2; int y; y = 4; return x[0] + y + 171;}' 174
run_test 137 'char foo(char *p){*p = 5; char a;a = 3; return a;} int main(){char q; char r; r = foo(&q); return 172-r+q;}' 174
run_test 138 'char a;char foo(char *p){*p = 5; a = 3; return a;} int main(){char q; char r; r = foo(&q); return 172-r+q;}' 174
run_test 139 'int foo(char a){int d;d = 3;char c;c = a+d;return c;} int main(){char f;f=3;return foo(f)*4+150;}' 174
run_test 140 'int foo(char a){int d;d = 3;char c;c = a+d;return c*4;} int main(){char f;f=3;return foo(f)+150;}' 174
run_test 141 'int qwerty(); int main(){char f;f=3;return qwerty(f,4)+150;}' 174
run_test 142 'int qwer(); int main(){char f;f=3;return qwer(f,4)+150;}' 174
run_test 143 'int foo(char a, char b){return 23;} int main(){char f;f=3;return foo(f,4)+151;}' 174
run_test 144 'int foo(char a, char b){return a*4+11;} int main(){char f;f=3;return foo(f,4)+151;}' 174
run_test 145 'int foo(char a, char b){return a*4+12;} int main(){char f;f=3;return foo(f,4)+150;}' 174
run_test 146 'int foo(char a, char b){return (a+3)*4;} int main(){char f;f=3;return foo(f,4)+150;}' 174
run_test 147 'int foo(char a, char b){char c;c = a+3;return c*4;} int main(){char f;f=3;return foo(f,4)+150;}' 174
run_test 148 'int foo(char a, char b){int d;d = 3;char c;c = a+d;return c*4;} int main(){char f;f=3;return foo(f,4)+150;}' 174
run_test 149 'int foo(char a, char b){int d;d = 3;char c;c = a+d;return c*b;} int main(){char f;f=3;return foo(f,4)+150;}' 174
run_test 150 'char foo() { char *x;x = "1ab"; return x[0]; }int main(){ char *y;y = "a2b"; int z;z = 12; char a;a = y[1]; return (a-foo())*z+162;}' 174
run_test 151 'int printf();int main(){printf("%d %s", 1, "a");return 174;}' 174
run_test 152 'int printf();int puts();int A[200][200];int main() {int i; for (i = 1; i <= 12; i++) { printf("%d %d", i, i); puts(""); } return 0;}' 0
run_test 153 'int printf();int puts();int a(int b, int c) {return 3;}int main() {int i; for (i = 1; i <= 12; i++) { int j;j = a(0, i); printf("%d %d", i, j); puts("");} return 0;}' 0
run_test 154 'int printf();int puts();int A[200][200];int dfs(int row, int N) { if (row == N) return 1; int ret;ret = 0; int col;for (col = 0; col < N; col++) { int ok; ok = 1; int i; for (i = 1; i < N; i++) { if (row - i >= 0 && col - i >= 0) { ok = ok && A[row - i][col - i] == 0; } if (row - i >= 0) { ok = ok && A[row - i][col] == 0; } if (row - i >= 0 && col + i < N) { ok = ok && A[row - i][col + i] == 0; } } if (ok) { A[row][col] = 1; ret += dfs(row + 1, N); A[row][col] = 0; } } return ret;}int main() {int i; for (i = 1; i < 11; i++) { int j; j = dfs(0, i); printf("%d queen: %d", i, j); puts("");} return 0;}' 0
run_test 155 'int printf();int puts();int count;int solve(int n, int col, int *hist){if (col == n) {count+=1;return 0;}int i;int j;for (i = 0, j = 0; i < n; i++) {for (j = 0; j < col && hist [j] != i && (hist [j] - i) != col - j && (hist[j] - i) != j - col; j++){}if (j < col)continue;hist[col] = i;solve(n, col + 1, hist);}return 0;}int main(){int i; int hist[20]; for (i = 2; i < 11; i++) { count=0; solve(i, 0, hist); printf("%d queens: %d", i, count); puts("");} return 0;}' 0
run_test 156 'int main(){/**/return 123;}' 123
run_test 157 'int main(){/*u89g3wihu-@w3erolk*/ return (123);}' 123
run_test 158 'int/*/* 0^[o;:._/-*/main(){return ((((123))));}' 123
run_test 159 'int a; int main(){int *p; p = &a; int i; for(i=0;i<174;i++){++*p;} return a;}' 174
run_test 160 'int a; int main(){int *p; p = &a; int i; for(i=0;i<174;((i))++){++*p;} return a;}' 174
run_test 161 'int main(){int a[10]; a[5] = 173; int b; b = a[5]++; return a[5]*!(a[5]-b-1);}' 174



run_test 118 'int main(){int a[1]; int *p; p = a; *p=2; return 174;}' 174
run_test 119 'int main(){int a[1]; *(a+0)=2;return 174;}' 174
run_test 120 'int x; int *y; int main(){x=3; int a[1]; *a=2; y=a; return x+*y+169;}' 174
run_test 121 'int x; int main(){x=3; int *y; y=&x; return *y+171;}' 174
run_test 122 'int a[1]; int main(){ *a=2;return 174;}' 174
run_test 123 'int main(){int a[1][2];int *q;q = *a;return 174;}' 174
run_test 124 'int main(){int a[1][2];int *q;q = *a; *q=174; return **a;}' 174
run_test 125 'int main(){int a[1][2];int *q;q = *(a+1); *q=174; return **(a+1);}' 174
run_test 126 'int main(){int a[5][6];int *q;q = *(a+1); *(2+q)=174; return *(*(1+a)+2);}' 174
run_test 127 'int changeBoard(int board[30][30], int i, int j, int d, int N){int k;for (k = 0; k < N; k++) {*(*(board + i) + k) += d;*(*(board + k) + j) += d;}if (i > j) {for (k = 0; k < N - (i - j); k++) {*(*(board + k + (i - j)) + k) += d;}} else {for (k = 0; k < N - (j - i); k++) {*(*(board + k) + k + (j - i)) += d;}}if (i + j < N) {for (k = 0; k <= i + j; k++) {*(*(board + i + j - k) + k) += d;}} else {for (k = i + j - N + 1; k < N; k++) {*(*(board + i + j - k) + k) += d;}}return 0;}int setQueen(int board[30][30], int num_placed, int *ptr_sol_num, int N){int j;if (num_placed == N) {(*ptr_sol_num)+=1;return 0;}for (j = 0; j < N; j++) {if (*(*(board+num_placed)+j) == 0) {changeBoard(board, num_placed, j, +1, N);setQueen(board, num_placed + 1, ptr_sol_num, N);changeBoard(board, num_placed, j, -1, N);}}return 0;}int board_[30][30];int main(){int sol_num;sol_num = 0;setQueen(board_, 0, &sol_num, 8);return sol_num;}' 92
run_test 128 'int count;int solve(int n, int col, int *hist){if (col == n) {count+=1;return 0;}int i;int j;for (i = 0, j = 0; i < n; i++) {for (j = 0; j < col && *(hist + j) != i && (*(hist + j) - i) != col - j && (*(hist + j) - i) != j - col; j++){}if (j < col)continue;*(hist+col) = i;solve(n, col + 1, hist);}return 0;}int main(){int hist[8];solve(8, 0, hist);return count;}' 92
run_test 129 'int count;int solve(int n, int col, int *hist){if (col == n) {count+=1;return 0;}int i;int j;for (i = 0, j = 0; i < n; i++) {for (j = 0; j < col && *(hist + j) != i && (hist [j] - i) != col - j && (*(hist + j) - i) != j - col; j++){}if (j < col)continue;*(hist+col) = i;solve(n, col + 1, hist);}return 0;}int main(){int hist[8];solve(8, 0, hist);return count;}' 92
run_test 130 'int count;int solve(int n, int col, int *hist){if (col == n) {count+=1;return 0;}int i;int j;for (i = 0, j = 0; i < n; i++) {for (j = 0; j < col && hist [j] != i && (hist [j] - i) != col - j && (hist[j] - i) != j - col; j++){}if (j < col)continue;hist[col] = i;solve(n, col + 1, hist);}return 0;}int main(){int hist[8];solve(8, 0, hist);return count;}' 92
run_test 131 'int changeBoard(int board[30][30], int i, int j, int d, int N){int k;for (k = 0; k < N; k++) {board[i][k] += d;board[k][j] += d;}if (i > j) {for (k = 0; k < N - (i - j); k++) {board [k + (i - j)][k] += d;}} else {for (k = 0; k < N - (j - i); k++) {board[k][k + (j - i)] += d;}}if (i + j < N) {for (k = 0; k <= i + j; k++) {board[i + j - k][k] += d;}} else {for (k = i + j - N + 1; k < N; k++) {board[i + j - k][k] += d;}}return 0;}int setQueen(int board[30][30], int num_placed, int *ptr_sol_num, int N){int j;if (num_placed == N) {(*ptr_sol_num)+=1;return 0;}for (j = 0; j < N; j++) {if (board[num_placed][j] == 0) {changeBoard(board, num_placed, j, +1, N);setQueen(board, num_placed + 1, ptr_sol_num, N);changeBoard(board, num_placed, j, -1, N);}}return 0;}int board_[30][30];int main(){int sol_num;sol_num = 0;setQueen(board_, 0, &sol_num, 8);return sol_num;}' 92
run_test 132 'int main(){int a[5][6];int *q;q = a[1]; 2[q]=174; return 1[a][2];}' 174

run_test 001 'int main(){return 123;}' 123
run_test 002 'int main(){return (123);}' 123
run_test 003 'int main(){return ((((123))));}' 123
run_test 004 'int main(){return 123+51;}' 174
run_test 005 'int main(){return 123+56-5;}' 174
run_test 006 'int main(){return 175-(4-3);}' 174
run_test 007 'int main(){return 181-4-3;}' 174
#run_test 008 'int main(){return 0x29*3+7*8-5*1;}' 174
run_test 009 'int main(){return 6*(3+7)-5*1;}' 55
run_test 010 'int main(){return 43,6*(3+7)-5*1;}' 55
run_test 011 'int main(){return 43,6*(3+(4|3))-(5|1)*1;}' 55
#run_test 012 'int main(){return 043,41*3+07*010-0Xa/(010%(1+2));}' 174
run_test 013 'int main(){return 7*5,(12,41*3)+7*16/(9,2)-10/(8%3);}' 174
#run_test 014 'int main(){return 7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1+2)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	1  <<3)	%3);}'  174
run_test 015 'int main(){return 35,	((	41|	(8   !=     15))*  ((3==3)+2))+((5|2)*(9&10))   -   (10/(8%3));}'  174
#run_test 016 'int main(){return 043,41*3+07*010-0Xa/(010%(!!1+2));}' 174
#run_test 017 'int main(){return 7*5 	,	(0xC,(41   )*(4-(011>8)))+7*(((1-~1)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/(	  ( 	!0  <<3)	%3);}' 174
run_test 018 'int main(){return +174;}' 174
run_test 019 'int main(){return -(1-175);}' 174
run_test 020 'int main(){23; 45+37; ((12-1)*75); return -(1-175);}' 174
run_test 021 'int main(){23; 45+37; return -(1-175); ((12-1)*75);}' 174
run_test 022 'int main(){int a; int b; return (a = b = 9, a = 41*3, 55 - (b = 4) + a);}' 174
run_test 023 'int main(){int a; int b; int c; int d; int _q432; a = b = c = 9; d = 5; a = 41*3; return (c, _q432 = 8, d = 11*5) - (b = 4) + a;}'  174
run_test 024 'int main(){return 175^1;}' 174
run_test 025 'int main(){return 2 + (1? 100 + 72 : 17);}' 174
run_test 026 'int main(){return (0? 234 : 2) + (1? 100 + 72 : 17);}' 174
run_test 027 'int always87(); int main(){return (3, always87() + always87());}' 174
run_test 028 'int always87();int always8();int main(){return always87() + ((always8()* 11) -1);}' 174
run_test 029 'int add();int main(){return add(170,4);}' 174
run_test 030 'int always87();int always8();int add();int main(){return always87() + ((always8()* add(4,7)) -1);}' 174
run_test 031 'int always87();int always8();int subtract();int main(){return always87() + ((always8()* subtract(12,1)) -1);}' 174
run_test 032 'int main(){3; {5; 7; 11; } return 175^1;}' 174
run_test 033 'int always87();int always87_(){return 87;} int main(){return (3, always87() + always87_());}' 174
run_test 034 'int add_(int x, int y){4; return x+y;} int main(){3; return add_(87,87);}' 174
run_test 035 'int fib(int n){ return n < 2? n : fib(n - 1) + fib(n - 2); } int main(){3; return fib(10);}' 55
run_test 036 'int tarai(int x,int y,int z){ return x <= y? y : tarai(tarai(x-1, y, z), tarai(y-1, z, x), tarai(z-1, x, y)); } int main(){return tarai(12,6,0);}' 12
run_test 037 'int main() { return (3 && 2 && 5) + 173; }' 174
run_test 038 'int main() { return (3 && 2) + !(3 && 0) + !(0 && 3)+ !(0 && 0) + 170; }' 174
run_test 039 'int main() { return (3 || 2 || 5) + 173; }' 174
run_test 040 'int main() { return (3 || 2) + (3 || 0) + (0 || 3)+ !(0 || 0) + 170; }' 174
run_test 041 'int main() {int a; a = 3; a += 5;  return a + 166; }' 174
run_test 042 'int main() {int a; int b; a = 3; b = (a += 5);  return a + b + 158; }' 174
run_test 043 'int main() {int a; int b; a = 3; b = 1; b *= (a += 5);  return a + b + 158; }' 174
run_test 044 'int main() {int a; int b; a = 11; a -=5; a /= 2; b = 1; b *= (a += 5);  return a + b + 158; }' 174
run_test 045 'int main() {int a; int b; int c; a = 7; a &= ~2; a <<= 2; a |=2; a >>= 1; a -=5; a /= 2; b = 3; c = 8; b ^= (c%=3); b *= (a += 5);  return a + b + 158; }' 174
run_test 046 'int foo(){ return 2;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();} else { }    return 172+b;}' 174
run_test 047 'int foo(){ return 2;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();}   return 172+b;}' 174
run_test 048 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();} else { c = bar();}    return 172+b;}' 174
run_test 049 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 0;b = 5;c = 2;if(a) {b = foo();} else { c = bar();}    return 162+b+c;}' 174
run_test 050 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) if(0) { b = foo(); } else {  c = bar(); }    return 162+b+c;}' 174
run_test 051 'int foo(){ return 2;} int bar(){ return 7;} int main() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) if(0)b=foo();else c = bar();return 162+b+c;}' 174
run_test 052 'int main() {int a; a = 4; if(1){return 170+a; a = 7; }else{return 170-a; a = 9;} a = 5; return a;}' 174
run_test 053 'int foo(){return 1;} int main(){int a; a=0;do{a=3;}while(a==foo());return 174;}' 174
run_test 054 'int main(){int a; a=0;do{a+=1;}while(a && a < 174);return a;}' 174
run_test 055 'int main(){int a; a=-8;do{a+=1;}while(a);return a+174;}' 174
run_test 056 'int foo(){return 3;} int main() {int a; a = 0;while(a == foo()) {a = 3;}return 174;}' 174
run_test 057 'int main(){int a; int b; a = 0; b = 0; while(a <= 10) {b += a; a += 1;}return b;}' 55
run_test 058 'int main(){int a; a = 3;while (a) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174
run_test 059 'int main(){int a; int b; int c; a = 3; b = 5; c = 0;while(a){while(b) {c += b;b-=1;if(b == 3) break;}b = 7;a-=1;if(a == 1) break;} return a*7+b*15+c*2;}' 174
run_test 060 'int main(){int a; a = 3;while (a) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174
run_test 061 'int main(){int a; int b; a=11; b=0; while(a){a-=1;b+=a;if(a)continue;break; a+=100;} return b;}' 55
run_test 062 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return -a;}' 3
run_test 063 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return -b;}' 6
run_test 064 'int main(){int a; int b; a =-3; b=-6; return a*b*10+a+b+3;}' 174
run_test 065 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b*10;}' 180
run_test 066 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b*10+a+b+3;}' 174
run_test 067 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return a*b;}' 18
run_test 068 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return b*a;}' 18
run_test 069 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; a+=100;}while(a+3); return b*a*10;}' 180
run_test 070 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break;}while(a+3); return a*b;}' 18
run_test 071 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(!a)break;}while(a+3); return a*b;}' 18
run_test 072 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;}while(a+3); return a*b;}' 18
run_test 073 'int main(){int a; int b; a =0; b=0; do{a-=1;b+=a;}while(a+3); return b*a;}' 18
run_test 074 'int main(){int a; int b; a=3; b=0; b+= ++a; return a*b*11-2;}' 174
run_test 075 'int main(){int a; int b; a=3; b=0; b+= a++; return !(b-3)+!(a-4)+172;}' 174
run_test 076 'int main(){int a; int b; a =0; b=0; do{b+=--a;}while(a+3); return b*a;}' 18
run_test 077 'int main(){int a; for (a = 3;a;) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174
run_test 078 'int main(){int a; for (a = 3;;) {a = 2;if (a - 3) {break;}a += 3;}return 174;}' 174
run_test 079 'int main(){int a; int b; for(a=0,b=0;a <= 10;) {b += a; a += 1;}return b;}' 55
run_test 080 'int main(){int a; int b; for(a=11, b=0;a;){a-=1;b+=a;if(a)continue;break; a+=100;} return b;}' 55
run_test 081 'int main(){int a; int b; for(a=0,b=0;a <= 10;++a) {b += a;}return b;}' 55
run_test 082 'int main(){int a; int b; for(a=0,b=0;a <= 10;a++) {b += a;}return b;}' 55
run_test 083 'int main(){int a; int b; int c; int d; d=0; b = 5; c = 0;for(a = 3;a;d++){for(;b;++d) {c += b;b-=1;if(b == 3) break;}b = 7;a-=1;if(a == 1) break;} return a*7+b*15+c*2;}' 174
run_test 084 'int main(){int a; int b; for(a=0,b=0;a<10;a++){ if(a ==5)continue;b+=a;} return b;}' 40
run_test 085 'int main(){int a; a = 174; {int a; a = 3;} return a;}' 174
run_test 086 'int main(){int a; a = 3; { a = 174;} return a;}' 174
run_test 087 'int main() {int *b; int a; a = 3; a += 5;  return a + 166; }' 174
run_test 088 'int main() {int *******b; int a; a = 3; a += 5;  return a + 166; }' 174
run_test 089 'int main() {int a; a = 174; int *b; b = &a; return a;}' 174
run_test 090 'int main(){int x;x = 86;int *y;y = &x; return (*y) + x + 2;}' 174
run_test 091 'int main(){int x;x = 86;int *y;y = &x; return (*y) + (*y) + 2;}' 174
run_test 092 'int main(){int x;x = 86;int *y;y = &x;int **z;z = &y;return (*y) + (**z) + 2;}' 174
run_test 093 'int main(){int x;x = 86;int *y;y = &x;int **z;z = &y;return*y+**z+2;}' 174
run_test 094 'int main() {int x;int *y;x = 3;y = &x;*y = 174;return x;}' 174
run_test 095 'int main() {int x;int *y;x = 3;y = &x;*y = 171;*y += 3;return x;}' 174
run_test 096 'int main(){int x; int y; int *z; int*a; z=&x; a=&y; *z=*a=87; return(x+y);}' 174
run_test 097 'int main(){int x; int *y; int **z; z = &y; *z = &x; *y = 174; return x;}' 174
run_test 098 'int foo(int* p){return 3;} int main(){int x; return 174;}' 174
run_test 099 'int foo(int* p){return *p;} int main(){int x; x = 174; return foo(&x);}' 174
run_test 100 'int foo(int* p){*p = 172; return *p+2;} int main(){int x; return foo(&x);}' 174
run_test 101 'int *foo(int *p){*p = 4;return p;} int main(){int x;int *y;y = foo(&x); *y+= 170;return x;}' 174
run_test 102 'int *foo(int *p){*p = 4;return p;} int main(){int x;int y;*foo(&x) += 170;return x;}' 174
run_test 103 'int *alloc4();int main(){int *p;p = alloc4(62, 8, 31, 85);return *p;}' 62
run_test 104 'int *alloc4();int main(){int *p;int *r;p = alloc4(62, 8, 31, 85);int *q;q = p;return *q;}' 62
run_test 105 'int *alloc4();int main(){int *p;int *r;p = alloc4(62, 8, 31, 85);int *q;q = p + 2;return *q;}' 31
run_test 106 'int *alloc4();int main(){int *p;int c;int e;c = 2;int d;d = 1;p = alloc4(62, 8, 31, 85);int *q;q = p + c;return *(q+d) - *q + (*p-2)*2;}' 174
run_test 107 'int *alloc4();int main(){int *p;p = alloc4(62, 8, 31, 85);int *q;q = p;return *q;}' 62
run_test 108 'int *alloc4();int main(){int *p;p = alloc4(62, 8, 31, 85);int *q;q = p + 2;return *q;}' 31
run_test 109 'int *alloc4();int main(){int *p;int c;c = 2;int d;d = 1;p = alloc4(62, 8, 31, 85);int *q;q = p + c;return *(q+d) - *q + (*p-2)*2;}' 174
run_test 110 'int *alloc4();int main(){int *p;int c;c = 2;int d;d = 1;p = alloc4(62, 8, 31, 85);int *q;q = c + p;return *(d+q) - *q + (*p-2)*2;}' 174
run_test 111 'int *alloc4();int main(){int *p;int c;c = -2;int d;d = 1;p = alloc4(62, 8, 31, 85);int *q;q = p - c;return *(d+q) - *q + (*p-2)*2;}' 174
run_test 112 'int *alloc4();int main(){int *p; int *q; q = p+174; return q-p;}' 174
run_test 113 'int *foo(int *p){*p = 4;return p;} int main(){int x;int y; int **z; *foo(&x) += 170;return x;}' 174
run_test 114 'int main(){int a[2][3]; return 174;}' 174
run_test 115 'int x; int *y; int main(){return 174;}' 174
run_test 116 'int x; int *y; int main(){return x+174;}' 174
run_test 117 'int x; int *y; int main(){x=3; int a; a=2; y=&a; return x+*y+169;}' 174
