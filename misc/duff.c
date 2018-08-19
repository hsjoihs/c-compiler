int global;
int printf();

void f(void)
{
    global += 3;
    return;
}

void call_n_times(int count)
{
    global = 0;
    {
        int n; n = (count + 7) / 8;
        switch (count % 8) {
        case 0: do { f();
        case 7:      f();
        case 6:      f();
        case 5:      f();
        case 4:      f();
        case 3:      f();
        case 2:      f();
        case 1:      f();
                } while (--n > 0);
        }
    }
    return;
}

int main()
{
	int i;
	for (i = 1; i < 300; i++) {
		call_n_times(i);
		if (global != 3 * i) {
			printf("FAILED at %d", i);
			return 1;
		}
	}
	return 0;
}