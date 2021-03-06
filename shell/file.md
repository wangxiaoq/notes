### 文件操作

1.$ echo "fewfewfef" > /dev/null #/dev/null像个黑洞,什么东西丢进去都消失殆尽

2.$ vimdiff passwd group #用来比较两个文件的不同

3.硬链接不可以跨越文件系统，而软链接可以跨越文件系统.
	不允许给目录创建硬链接

4.使用file命令获取详细的文件信息

5.使用mkfifo创建命名管道

6.ls
```
    $ ls -n /etc/passwd  #将文件所属的UID和GID都用数字表示

	-rw-r--r-- 1 0 0 1429 Sep  1 10:40 /etc/passwd
	
	$ ls -l /etc/passwd
	
	-rw-r--r-- 1 root root 1429 Sep  1 10:40 /etc/passwd
```
	可以使用strace命令来检测使用 ls -l 命令的过程中，是否读取了
	/etc/passwd 和 /etc/group 文件

```
	$ strace -f -o strace.log ls -l 
```
	strace是一个非常有用的工具，用来跟踪系统调用和信号。基于系统ptrace实现

7.给重要的文件加锁，添加不可修改位（immutable）,以避免各种误操作带来的后果。

```
	$ sudo chattr +i group1
	$ lsattr group1
	----i--------e-- group1
	$ rm group1
	rm: remove write-protected regular file `group1'? y
	rm: cannot remove `group1': Operation not permitted
	$ sudo chattr -i group1
	$ rm group1
	$ ls group1
	ls: cannot access group1: No such file or directory
```
	注：chattr可以设置文件的特殊权限

8.符号链接中存放的是该链接所指向的文件的名字的长度。

9.atime,access time: 文件最后一次访问的时间

  ctime,status time: 文件状态改变的时间，比如修改文件的权限等       
  
  mtime,modify time: 文件内容的修改时间，一般使用ls -l默认显示的时间

```
	$ ls -l --time=atime hello	#显示文件hello最后一次被访问的时间	
	-rwxr-xr-x 1 wangxq wangxq 12 Sep 19 21:16 hello
	$ ls -l --time=ctime hello	#显示文件hello最近一次状态被修改的时间
	-rwxr-xr-x 1 wangxq wangxq 12 Sep 19 21:18 hello
	$ ls -l hello	#显示文件最近被修改的时间
	-rwxr-xr-x 1 wangxq wangxq 12 Sep 19 21:17 hello
```

10.$ while read line;do echo "$line";done < hello	#用来按行读取一个文件的内容

11.重定向

```
    $ com="echo \"hello world\" > b"	#包含重定向字符的字符串，需要使用eval来执行
	$ $com
	"hello world" > b
	$ eval $com
	$ cat b
	hello world
```