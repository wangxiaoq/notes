字符串操作

1.字符串的长度，例如：

   *	$ var="get the length of me"
	 	$ echo ${#var}
		 20
   *	$ expr length "$var"	#注意这里是双引号
		 20
   *	$ echo $var | awk '{printf("%d",length($0))}'
		 20
   * 	$ echo -n $var | wc -c		# echo每次输出时，会自动的输出换行符，-n表示部署出那一个换行符
		 20
 
2.获取字符串中，某些字符的个数，例如

   *	$ echo $var | tr -cd g | wc -c
	     2
		$ echo -n $var | sed 's/[^g]//g' | wc -c
		 2
		$ echo -n $var | sed 's/[^gt]//g' | wc -c
		 5

3.统计单词的个数

   * 	$ echo $var | wc -w

4.bash提供的数组数据结构，它是以数字为下标的，和C语言从0开始的下一样：

```
   *	$ var="get length of me"
		$ var_arr=($var)	#这里把var字符串，以数组的形式存放到var_arr中，默认以空格分割
		$ echo ${var_arr[0]} ${var_arr[1]} ${var_arr[2]} ${var_arr[3]} ${var_arr[4]}
        get the length of me 
        $ echo ${var_arr[@]} #@表示整个字符串
		get the length of me
        $ echo ${var_arr[*]} #*表示整个字符串
        get the length of me
        $ echo ${#var_arr[@]}#求得整个字符数组的元素个数
        5
		$ echo ${#var_arr[0]}#求得某个字符串的字符个数
 		3
 		$ var_arr[5]="new_element" #直接给数组元素赋值
        $ echo ${var_arr[@]}
   		get the length of me new_element
		$ echo ${#var_arr[@]}
		6
```

	*	$ for i in $var; do echo $i" "; done;
  		get the length of me

5.awk里面的数组：

	* echo $var | awk '{printf("%d %s\n",split($0,var_arr," "),var_arr[1])}'
      split把字符串按照空格分割，存放到var_arr中，并返回数组的长度，注意，这里第一个元素的下标不是0，而是1。
    * awk把一行按照空格分成若干个域，并用$1,$2,$3...来引用，$0表示整行,NF表示该行域的总数。
    * echo $var | awk '{split($0,var_arr," ");for(i in var_arr) printf("%s ",var_arr[i]);}'
      这里的for循环中，循环变量为下标
    * echo $var | awk '{split($0,var_arr," ");delete var_arr[1]}'
      删除第一个数组元素

6.取子串

```
   * 	$ var="get length of me"
		$ echo ${var:0:3}
 		get
		$ echo ${var(-2)}	#反方向
		me
 		$ echo `expr substr "$var" 5 3`
 		the
 		$ echo $var | awk '{printf("%s\n",substr($0,9,6))}' #从第九个位置开始选取6个字符
		length
```

```
	*	使用cut取子串
		$ echo $var | cut -d " " -f 5
		me
```

```
	*	$ echo $var | sed 's/ [a-z]*//g'
        get
		删除空格+字母串
		$ echo $var | sed 's/[a-z]* //g'
		me
		删除字母串+空格
```

7.匹配求子串

	* $ echo ${var%% *}	#从最右边开始，删除最左边空格右边的所有字符
	  get
	* $ echo ${var% *}	#从最右边开始，删除第一个空格右边的所有字符
	  get the length of
	* $ echo ${var##* }	#从最左边开始，删除最右边空格左边的所有字符
	  me
	* $ echo ${var#* }	#从最左边开始，删除第一个空格左边的所有字符
	  the length of me

8.sed有按行打印的功能，记得用tr把空格换为行号

	* $ echo $var | tr " " "\n" | sed -n 5p
	  me
	* $ echo $var | tr " " "\n" | sed -n 1p
	  get

9.tr来取子串

	* $ echo $var | tr -d " "
	  getthelengthofme
	* $ echo $var | tr -cd "[a-z]"	#仅仅保留字母
	  getthelengthofme

10.head和tail

	* $ echo "abcdefghijk" | head -c 4 
	  abcd
	* $ echo "abcdefghijk" | tail -c 4
	  hijk

11.查询子串

	* $ var="get the length of me"
	  $ expr index "$var" t
	  3
	* $ echo $var | awk '{printf("%d\n",match($0,"the"));}'
	  5

12.子串替换

	* $ var="get the length of me"
	  $ echo ${var/ /_}		#把第一个空格替换成下划线
	  get_the length of me
	  $ echo ${var// /_}	#把所有的空格都替换成下划线
	  get_the_length_of_me
	  $ echo $var | awk '{sub(" ","_",$0);printf("%s\n",$0);}'
	  get_the length of me
	  $ echo $var | awk '{gsub(" ","_",$0);printf("%s\n",$0);}'
	  get_the_length_of_me
	  $ echo $var | sed 's/ /_/'
	  get_the length of me
	  $ echo $var | tr " " "_"
	  get_the_length_of_me
	  $ echo $var | tr "[a-z]" "[A-Z]"
	  GET THE LENGTH OF ME

13.tac 会将文本的内容倒置显示，正如名字和 cat 相反一样，
	功能和cat也是相反的

14.插入子串

	*  	$ var="get the length of me"
		$ echo ${var/ /_ }		#在第一个空格前加上_
		get_ the length of me
		$ echo ${var// /_ }		#在所有的空格前都加上_
		get_ the_ length_ of_ me
		$ echo ${var/ / _}
		get _the length of me
		$ echo ${var// / _}		#在所有的空格之后都加上_
		get _the _length _of _me
		
	*	$ echo $var | sed 's/\( \)/\1_/'	#在第一个空格之后加上_
		get _the length of me
		$ echo $var | sed 's/\( \)/\1_/g'
		get _the _length _of _me
		$ echo $var | sed 's/\([a-z]*\) \([a-z]*\)/\2 \1/'	#调换get和the的位置
		the get length of me

15.删除子串

	*	$ var="get the length of me"
		$ echo ${var// /}
		getthelengthofme

	*	$ echo $var | awk '{gsub(" ","",$0);printf("%s",$0);}'
		getthelengthofme

	*	$ echo $var | sed 's/ //g'
		getthelengthofme

	*	$ echo $var | tr -d " "
		getthelengthofme

16.子串比较使用test命令

17.子串排序

	*	$ echo $var | tr " " "\n" | sort	#正排序
		get
		length
		me
		of
		the
		$ echo $var | tr " " "\n" | sort -r
		the
		of
		me
		length
		get
	*	$ cat data.txt | tr " " "\n" | sort -n

18.进制转换

	*	$ echo "ibase=10;obase=16;10" | bc
		A

19.正则表达式处理url

	*	$ url="ftp://anonymous:ftp@mirror.lzu.edu.cn/software/scim-1.4.7.tar.gz"
		$ echo $url | grep "ftp://[a-z]*:[a-z]*@[a-z0-9\./-]*"	#判断url的有效性
		ftp://anonymous:ftp@mirror.lzu.edu.cn/software/scim-1.4.7.tar.gz
	*	$ echo "$url" "$(expr index "$url" :)" | awk '{printf("%s\n",substr($1,1,$2-1));}'	#截取协议
		ftp
		$ echo ${url%%:*}
		ftp
	*	$ echo ${url##*@} | cut -d "/" -f 1		#截取域名
		mirror.lzu.edu.cn
		$ tmp=${url##*@};echo ${tmp%%/*}
		mirror.lzu.edu.cn
	*	$ tmp=${url##*@};echo ${tmp%/*}			#截取路径
		mirror.lzu.edu.cn/software
	*	$ echo ${url##*/}			#截取文件名
		scim-1.4.7.tar.gz
	*	$ echo $url | sed 's/.*[0-9].//g'	#截取文件类型
		tar.gz

20.sed 和 awk 匹配文件中的行

	*	$ sed -n 7,9p README	#打印README7-9行

21.处理格式化的文本

	*	$ cat /etc/passwd | cut -d ":" -f 1,5	#截取/etc/passwd文件中的用户名和组
	*	$ cat /etc/group | cut -d ":" -f 1,3	#截取/etc/group文件中的组名和组id
	
22.文件关联操作

	*	$ join -o 1.1,2.1 -t":" -1 4 -2 3 /etc/passwd /etc/group	
		#join命令用来连接两个文件，类似于数据库中两个表的连接。-t指定分隔符，
		-1 4 -2 3 指定按照第一个文件的第四列和第二个文件的第三列，即组ID进行
		连接，-o 1.1 2.1表示仅仅输出第一个文件的第一列和第二个文件的第一列

23.将某个句子的首字母替换为大写

	$ echo "get the length of me" | sed 's/\b[a-z]/\U&/g'
	Get The Length Of Me
