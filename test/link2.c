struct A_2 {int a;};
struct A_2 func_ (int u);

int g_fnc(int a)
{
	struct A_2 u = func_(a);
	return u.a;
}
