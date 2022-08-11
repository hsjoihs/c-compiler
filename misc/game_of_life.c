// Edited by uint256_t
// Taken from: https://gist.github.com/maekawatoshiki/8ea2f392e22698593d7ee8522aabbbd0

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>

#define SIZE 20

int printf(char *msg);
int usleep(int time);

int count_nbr(int grid[SIZE][SIZE], int i, int j, int size);

void init_grid(int grid[SIZE][SIZE], int grid_binaries[SIZE])
{
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			grid[i][j] = (grid_binaries[i] >> (SIZE - 1 - j)) & 1;
		}
	}
}

int main()
{
	int neighbour_count[SIZE][SIZE];
	int grid[SIZE][SIZE];
	int grid_binaries[SIZE];
	grid_binaries[0] = 0b00000000000000000000,
	grid_binaries[1] = 0b00000000000000000000,
	grid_binaries[2] = 0b00000000000000000000,
	grid_binaries[3] = 0b00000000000000000000,
	grid_binaries[4] = 0b00000000000000000000,
	grid_binaries[5] = 0b00000000000000000000,
	grid_binaries[6] = 0b00000000000000000000,
	grid_binaries[7] = 0b00000111111111100000,
	grid_binaries[8] = 0b00000000000000000000,
	grid_binaries[9] = 0b00000000000000000000,
	grid_binaries[10] = 0b00000000000000000000,
	grid_binaries[11] = 0b00000000000000000000,
	grid_binaries[12] = 0b00000000000000000000,
	grid_binaries[13] = 0b00000000000000000000,
	grid_binaries[14] = 0b00111100000000000000,
	grid_binaries[15] = 0b01000100000000000000,
	grid_binaries[16] = 0b00000100000000000000,
	grid_binaries[17] = 0b01001000000000000000,
	grid_binaries[18] = 0b00000000000000000000,
	grid_binaries[19] = 0b00000000000000000000;
	init_grid(grid, grid_binaries);
	int i;
	int j;
	int steps;

	for (steps = 0; steps < 50; ++steps) {
		printf("\e[0;0H");
		for (i = 0; i < SIZE; ++i) {
			printf("\n");
			for (j = 0; j < SIZE; ++j) {
				if (grid[i][j] == 1)
					printf("\e[42m  \e[m");
				else
					printf("\e[47m  \e[m");
				neighbour_count[i][j] = count_nbr(grid, i, j, SIZE);
			}
		}

		for (i = 0; i < SIZE; ++i) {
			for (j = 0; j < SIZE; ++j) {
				if (grid[i][j] >= 1) {
					if (neighbour_count[i][j] <= 1 ||
					    neighbour_count[i][j] >= 4)
						grid[i][j] = 0;
				} else if (neighbour_count[i][j] == 3)
					grid[i][j] = 1;
			}
		}

		usleep(100000);
	}

	return 0;
}

int count_nbr(int grid[SIZE][SIZE], int i, int j, int size)
{
	int n_count = 0;
	if (i - 1 >= 0 && j - 1 >= 0) {
		if (grid[i - 1][j - 1] >= 1)
			n_count++;
	}

	if (i - 1 >= 0) {
		if (grid[i - 1][j] >= 1)
			n_count++;
	}

	if (i - 1 >= 0 && j + 1 < size) {
		if (grid[i - 1][j + 1] >= 1)
			n_count++;
	}

	if (j - 1 >= 0) {
		if (grid[i][j - 1] >= 1)
			n_count++;
	}

	if (j + 1 < size) {
		if (grid[i][j + 1] >= 1)
			n_count++;
	}

	if (i + 1 < size && j - 1 >= 0) {
		if (grid[i + 1][j - 1] >= 1)
			n_count++;
	}

	if (i + 1 < size) {
		if (grid[i + 1][j] >= 1)
			n_count++;
	}

	if (i + 1 < size && j + 1 < size) {
		if (grid[i + 1][j + 1] >= 1)
			n_count++;
	}

	return n_count;
}