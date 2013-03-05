void foo (char str [], int len)
{
        write (2, str, len);
}

int main ()
{
	char str1 [24];
	char str2 [4];

        str1 = "foo";
        str2 = "bar";

	foo (str1, sizeof (str1));
	foo (str1, sizeof (str2));
}
