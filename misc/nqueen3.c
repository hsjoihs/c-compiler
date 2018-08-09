/* Based on https://twitter.com/0x19f/status/1025823053459017728 */
int printf();
int puts();
int A[200][200];
int dfs(int row, int N)
{
	if (row == N)
		return 1;
	int ret;
	ret = 0;
	int col;
	for (col = 0; col < N; col++) {
		int ok;
		ok = 1;
		int i;
		for (i = 1; i < N; i++) {
			if (row - i >= 0 && col - i >= 0) {
				ok = ok && A[row - i][col - i] == 0;
			}
			if (row - i >= 0) {
				ok = ok && A[row - i][col] == 0;
			}
			if (row - i >= 0 && col + i < N) {
				ok = ok && A[row - i][col + i] == 0;
			}
		}
		if (ok) {
			A[row][col] = 1;
			ret += dfs(row + 1, N);
			A[row][col] = 0;
		}
	}
	return ret;
}
int main()
{
	int i;
	for (i = 1; i < 11; i++) {
		int j;
		j = dfs(0, i);
		printf("%d queen: %d", i, j);
		puts("");
	}
	return 0;
}
