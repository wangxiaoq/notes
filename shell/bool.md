###布尔运算

1.bash里的true和false并不是我们通常所认为的0和1。
   true和false是shell的内置命令，返回逻辑值。例如：

```
   $ true
   $ echo $?
     0
   $ false
   $ echo $?
     1
```
   $?是一个特殊的变量，存放有上一个程序的结束状态
   在shell里面，把0作为程序是否成功结束的标志。

2.在shell中，逻辑值表示程序的返回值，如果程序成功返回，
   则为真，如果不成功返回，则为假。

3.test命令可以用来进行数值测试，字符串测试，文件测试。

  * 数值测试：
```
	$ if test 5 -eq 5 ; then echo "YES" ; else echo "NO"; fi
	YES
   	$ if test 5 -ne 5 ; then echo "YES" ; else echo "NO"; fi
   	NO
```
  * 字符串测试

```
	$ if test -n "not empty" ; then echo "YES" ; else echo "NO"; fi
	YES
	-n 测试字符串的长度不为零
	$ if test -z "not empty" ; then echo "YES" ; else echo "NO"; fi
	NO
	-z 测试字符串的长度为零
	$ if test -z "" ; then echo "YES" ; else echo "NO"; fi
	YES
```
  * 文件测试

```
	$ if test -f /boot/System.map ; then echo "YES" ;  else echo "NO"; fi
	YES
	-f 用来测试文件是否存在，以及是否是一个普通的文件
	$ if test -d /boot/System.map ; then echo "YES" ; else echo "NO"; fi
	-d 用来测试文件是否存在，以及是否是一个目录
```

4.test逻辑组合测试

  * 与，例如：

```
	$ a=5; b=4; c=6;
	$ if test $a -eq 5 -a $b -eq 4 -a $c -eq 6 ; then echo "YES" ; else echo "NO" ; fi
	YES
```

  * 或，例如：

```
	$ if test $a -eq 5 -o $b -ne 4 ; then echo "YES" ; else echo "NO" ; fi
	YES
```

  * 非，例如：

```
	$ if test ! -f /etc/profile ; then echo "YES" ; else echo "NO" ; fi
	NO
```

5.test与&&,||

```
	$ str1="test"; str2="test"
	$ if test -z "$str1" -o "$str2" == "$str1" ; then echo "YES" ; else echo "NO" ; fi
	YES
	$ if test -z "$str1" || test "$str1"=="$str2" ; then echo "YES" ; else echo "NO" ; fi
	YES
```
	两个测试语句是等价的。但是可以看到-o使用在test内部，而||使用在test之间
	test可以使用[]来代替，需要注意的是[]的前后都需要空格
	在测试字符串时，建议所有的变量都用双引号括起来，以防止变量代表的字符串为空的情况
	在使用中括号测试时，要注意：

```
	$ i=5
	$ if [ $i=6 ] ; then echo "YES" ; else echo "NO"; fi
	YES
	$ if [ $i = 6 ] ; then echo "YES" ; else echo "NO"; fi
	NO
```

6.命令列表

   * 对于&&，如果前面的命令成功执行，则继续执行后面的命令；如果失败，就不执行后面的命令
   * 对于||，若果前面的命令没有成功执行，则继续执行后面的命令
	例如：
	ping -c1 www.lzu.edu.cn && echo "connected"
   * 命令列表有时用来取代if/then等分支结构，这样可以省略一些代码。
