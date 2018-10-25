int printf();
int main()
{
	const char *str = "\t\n\v\f\r\"\'\\";
	char str2[9];
	str2[0] = '\t';
	str2[1] = '\n';
	str2[2] = '\v';
	str2[3] = '\f';
	str2[4] = '\r';
	str2[5] = '\"';
	str2[6] = '\'';
	str2[7] = '\\';
	str2[8] = 0;
	int i = 0; 
	for (; str[i]; i++) {
		int k = str[i];
		int l = str2[i];
		printf("%d %d\n", k, l);
	}
	return 0;
}