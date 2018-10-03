int printf();
int main()
{
	const char *str = "\t\n\v\f\r\"\'\\";
	int i = 0; 
	for (; str[i]; i++) {
		int k = str[i];
		printf("%d\n", k);
	}
	return 0;
}