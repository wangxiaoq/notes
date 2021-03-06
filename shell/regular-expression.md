### 正则表达式

1.grep 使用：

	* grep -n 'the' -A 2 -B 2 --color=auto regular_express.txt
	  对该命令的介绍：
	  -n 表示显示行号；
	  -A 和 -B 分别表示显示找到的关键字所在行的前面2行和后面两行；
	  --color 让关键字显示出语种不同的颜色
  	* grep -n -v 'the' regular_express.txt
	  -v 选项表示选择 *不包含* eth0 的行
	* grep -n -i 'the' regular_express.txt 
	  -i 表示忽略大小写
	* grep -n 't[ae]st' regular_express.txt
	  [] 表示选择括号中多个字符中的一个，例如，可选tast或test
	* grep -n '[^g]oo' regular_express.txt
	  [^g]oo 表示选择前面没有g的oo
	  [^a-z]oo 选择oo前面不为24个小写字符的情况
	* grep -n '[0-9]' regular_express.txt
	  选择包含数字的那一行
	* grep -n '^the' regular_express.txt
	  选择以the开头的那一行
	  grep -n '^[a-z]' regular_express.txt
	  选择以小写字母开头的那一行
	  grep -n '^[^A-Za-z]' regular_express.txt
	  选择不以英文字母开头的那一行
	  ^ 在中括号里，表示反向选择，在中括号外，表示行首
	* grep -n '\.$' regular_express.txt
	  选择以 . 结尾的行
	  $ 表示行尾
	* grep -n '^$' regular_express.txt
	  查询空行
	* 注：在～/.bashrc中可以设置不用给grep加--color=auto选项就可以
	  显示颜色，在里面添加一行 alias grep='grep --color=auto'
	

2.在正规表示法中：

	* “.”表示一定有一个字符
	* “*”表示零个或者多个字符
	* 例如：
			grep -n 'g..d' regular_express.txt
	  表示查找g和d之间有两个任意字符的单词，所在的行，可能时good，gold等
			grep -n 'goo*g' regular_express.txt
	  表示查找两个g之间有一个或者多个o的关键字所在的行
			grep -n '[0-9][0-9]*' regular_express.txt	
	  表示查找数字
	  
	* grep -n 'o\{2\}' regular_express.txt
	  查找连续出现2个字符o所在的行
	  grep -n 'go\{2,5\}g' regular_express.txt
	  查找两个g之间有2到5个o所在的行
	  grep -n 'go\{2,\}g' regular_express.txt
	  查找含有两个以上的o所在的行
	
3.sed工具的使用

	* nl regular_express.txt | sed '2,5d'
	  删除第二行和第五行，注意并没有对regular_express.txt文件做任何修改
	  nl regular_express.txt | sed '2d'
	  删除第二行
	  nl regular_express.txt | sed '2,$d'
	  删除第二行到最后一行，$表示最后一行
	* nl regular_express.txt | sed '2a drink tea'
	  在第二行后（即第三行）加上drink tea字样
	  nl regular_express.txt | sed '2i drink tea'
	  在第二行之前（即第二行）加上drink tea字样
	  nl regular_express.txt | sed '2a drink tea \
								or beer ?'
	  在第二行之后增加两行
	* nl regular_express.txt | sed '2,5c No 2-5'
	  将2到5行替换为No 2-5
	* nl regular_express.txt | sed -n '2,7p'
	  打印第2-7行的内容
	* nl regular_express.txt | sed 's/Open/Close/g'
	  将文件中的Open字符串替换为Close
	* /sbin/ifconfig eth0 | grep 'inet addr' | sed 's/^.*addr://g' | sed 's/Bcast.*$//g'
	  用来提取系统的IP地址，尤其注意这里使用将要删除的字符串替换为空，达到删除的目的
	* cat /etc/manpath.config | sed '/^$/d'
	  用来删除空行
	* sed -i 's/\.$/\!/g' regular_express.txt
	  将文件末尾的“.”替换为“!”，直接对文件本身进行修改
