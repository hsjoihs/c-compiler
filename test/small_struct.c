int rand(void);
int memcmp();
struct A1 {char a[1];}; struct A1 deref1(struct A1 *p){ return *p; }
struct A2 {char a[2];}; struct A2 deref2(struct A2 *p){ return *p; }
struct A3 {char a[3];}; struct A3 deref3(struct A3 *p){ return *p; }
struct A4 {char a[4];}; struct A4 deref4(struct A4 *p){ return *p; }
struct A5 {char a[5];}; struct A5 deref5(struct A5 *p){ return *p; }
struct A6 {char a[6];}; struct A6 deref6(struct A6 *p){ return *p; }
struct A7 {char a[7];}; struct A7 deref7(struct A7 *p){ return *p; }
struct A8 {char a[8];}; struct A8 deref8(struct A8 *p){ return *p; }
struct A9 {char a[9];}; struct A9 deref9(struct A9 *p){ return *p; }
struct A10 {char a[10];}; struct A10 deref10(struct A10 *p) { return *p; }
struct A11 {char a[11];}; struct A11 deref11(struct A11 *p) { return *p; }
struct A12 {char a[12];}; struct A12 deref12(struct A12 *p) { return *p; }
struct A13 {char a[13];}; struct A13 deref13(struct A13 *p) { return *p; }
struct A14 {char a[14];}; struct A14 deref14(struct A14 *p) { return *p; }
struct A15 {char a[15];}; struct A15 deref15(struct A15 *p) { return *p; }
struct A16 {char a[16];}; struct A16 deref16(struct A16 *p) { return *p; }

void garbge(void *p, int n){
	char *q = p;
	for (int i = 0; i < n; i++) {
		q[i] = rand() % 37;
	}
}

int main()
{
	{struct A1 q; garbge(&q, 1); struct A1 r = deref1(&q); if (memcmp(&q, &r, 1)) return 1;}
	{struct A2 q; garbge(&q, 2); struct A2 r = deref2(&q); if (memcmp(&q, &r, 2)) return 2;}
	{struct A3 q; garbge(&q, 3); struct A3 r = deref3(&q); if (memcmp(&q, &r, 3)) return 3;}
	{struct A4 q; garbge(&q, 4); struct A4 r = deref4(&q); if (memcmp(&q, &r, 4)) return 4;}
	{struct A5 q; garbge(&q, 5); struct A5 r = deref5(&q); if (memcmp(&q, &r, 5)) return 5;}
	{struct A6 q; garbge(&q, 6); struct A6 r = deref6(&q); if (memcmp(&q, &r, 6)) return 6;}
	{struct A7 q; garbge(&q, 7); struct A7 r = deref7(&q); if (memcmp(&q, &r, 7)) return 7;}
	{struct A8 q; garbge(&q, 8); struct A8 r = deref8(&q); if (memcmp(&q, &r, 8)) return 8;}
	{struct A9 q; garbge(&q, 9); struct A9 r = deref9(&q); if (memcmp(&q, &r, 9)) return 9;}
	{struct A10 q; garbge(&q, 10); struct A10 r = deref10(&q); if (memcmp(&q, &r, 10)) return 10;}
	{struct A11 q; garbge(&q, 11); struct A11 r = deref11(&q); if (memcmp(&q, &r, 11)) return 11;}
	{struct A12 q; garbge(&q, 12); struct A12 r = deref12(&q); if (memcmp(&q, &r, 12)) return 12;}
	{struct A13 q; garbge(&q, 13); struct A13 r = deref13(&q); if (memcmp(&q, &r, 13)) return 13;}
	{struct A14 q; garbge(&q, 14); struct A14 r = deref14(&q); if (memcmp(&q, &r, 14)) return 14;}
	{struct A15 q; garbge(&q, 15); struct A15 r = deref15(&q); if (memcmp(&q, &r, 15)) return 15;}
	{struct A16 q; garbge(&q, 16); struct A16 r = deref16(&q); if (memcmp(&q, &r, 16)) return 16;}

	return 0;
}