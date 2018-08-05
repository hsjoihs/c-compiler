/* https://rosettacode.org/wiki/N-queens_problem#C */
int count;
int solve(int n, int col, int *hist)
{
	if (col == n) {
		++count;
		return 0;
	}
	int i;
	int j;
	for (i = 0, j = 0; i < n; i++) {
		for (j = 0;
		     j < col && *(hist + j) != i && (*(hist + j) - i) != col - j &&
		     (*(hist + j) - i) != j - col;
		     j++) {
		}
		if (j < col)
			continue;

		*(hist + col) = i;
		solve(n, col + 1, hist);
	}
	return 0;
}

int main()
{
	int hist[8];
	solve(8, 0, hist);
	return count;
}