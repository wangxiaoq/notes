###字符指针与字符数组

1. 下面程序：

```

int main(void)
{
	char *str = "abc";
	str[0] = 'c';

	return 0;
}
```

上面的程序中，str指向一个字符串常量，字符串常量存储在程序的文字常量区，
一旦定义是不能被改变的。上面的程序可以编译通过，但是在运行时会发生段错
误。

而下面的程序是可以通过的：

```
int main(void)
{
	char str[] = "abc";
	str[0] = 'c';

	return 0;
}

int main(void)
{
	char c = 'c';
	char *str = "abc";
	str = &c;

	return 0;
}
```
