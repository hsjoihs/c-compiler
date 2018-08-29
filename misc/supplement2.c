struct A {int a;};
struct A f (int u);

int g(int a)
{
	struct A u = f(a);
	return u.a;
}
