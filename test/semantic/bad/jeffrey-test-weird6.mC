int b;
int c;
foo(int i)
{
	b = 5;
	c = 6;
	a = b + c + 11; //22
	a = a + i;
	return a;
}

int a;

int main()
{
	char r;
	int j;
	int k;
	int p;
	p = foo(foo(foo(foo(foo(1)))));
	j = foo(foo(p));
	k = 5; //e
	r = k;
	return 0;
}