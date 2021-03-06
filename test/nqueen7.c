/* http://www.cc.kyoto-su.ac.jp/~yamada/ap/backtrack.html */
int printf(); int puts();
void changeBoard(int board[30][30], int i, int j, int d, int N) {
	int k;
	for (k = 0; k < N; k++) { board[i][k] += d; board[k][j] += d;}
	if (i > j) for (k = 0; k < N - (i - j); k++)
		board[k + (i - j)][k] += d;
	else for (k = 0; k < N - (j - i); k++) 
		board[k][k + (j - i)] += d;
	if (i + j < N) for (k = 0; k <= i + j; k++) 
		board[i + j - k][k] += d;
	else for (k = i + j - N + 1; k < N; k++) 
		board[i + j - k][k] += d;
	return;
}
void setQueen(int board[30][30], int num_placed, int *ptr_sol_num, int N) {
	if (num_placed == N) { (*ptr_sol_num)++; return;}
	int j; for (j = 0; j < N; j++) 
		if (board[num_placed][j] == 0) {
			changeBoard(board, num_placed, j, +1, N);
			setQueen(board, num_placed + 1, ptr_sol_num, N);
			changeBoard(board, num_placed, j, -1, N);
		}
	return;
}
int board_[30][30];
int main() {
	int i; for (i = 1; i < 12; i++) {
		int sol_num; sol_num = 0; setQueen(board_, 0, &sol_num, i);
		printf("%d queen(s): %d", i, sol_num); puts("");
	}
	return 0;
}