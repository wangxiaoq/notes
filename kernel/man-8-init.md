###man 8 init


这个文档是从man 8 init中翻译过来的。


NAME

```
       init, telinit - process control initialization
``

SYNOPSIS

```
       /sbin/init [ -a ] [ -s ] [ -b ] [ -z xxx ] [ 0123456Ss ]
       /sbin/telinit [ -t SECONDS ] [ 0123456sSQqabcUu ]
       /sbin/telinit [ -e VAR[=VAL] ]
```

DESCRIPTION

Init

	Init是所有用户态进程的祖先，它从一个脚本中读取信息并创建进程，这个脚本是/etc/inittab。根据/etc/inittab中指定用户登录的每一行，来创建相应的gettys。并且管理特定操作系统的自治的程序。

RUNLEVELS

	runlevel是一个系统软件配置，它允许一个选中的组的进程存在。在每
一个runlevel中，由init创建的进程是在/etc/inittab中定义的。Init可以
处于8个runlevel中的一个 -- 0-6 和 S 。init的runlevel的改变是由一个
具有超级用户权限的用户，使用命令telinit来向init发送一个合适的信号，
来告诉init应该切换到哪一个runlevel。

	runlevel中，0，1，6和S是保留的，0是关机，6是重启，S是在启动的时候用来初始化系统，而1是从多用户级别切换到单用户级别。当开始进入S或1runlevel时，系统就开始进入单用户模式。
	
	在通过S runlevel之后，系统就进入到了2-5多用户runlevel中的一个，除非系统出了某些问题，管理员需要进入到单用户模式，进行一些维护工作，然后重启系统。
	
	runlevel 7-9也是有效的，尽管在文档中没有写出。由于传统的原因，UNIX变体没有使用7-9 。
	
	S 和 s是同一个运行级别。

BOOTING

	作为内核启动序列的最后一个步骤--调用init进程，init进程将会查看/etc/inittab中的initdefault表项，initdefault表项定义了系统最初的runlevel 。如果没有这个initdefault表项，或者压根就没有/etc/inittab这个文件，那么在系统console中必须进入一个runlevel 。

	runlevel S初始化系统，不需要/etc/inittab文件。

	在单用户模式，/sbin/sulogin在/dev/console上被调用。

	当进入单用户模式时，init初始化stty设置为健全的值。Clocal模式被设置，硬件速度和握手未变。

	当第一次进入到多用户模式时，init进程会执行/etc/inittab中的boot和bootwait表项，在允许用户登录之前挂载文件系统。然后处理和当前runlevel向匹配的表项。

	当启动一个新的进程时，init进程首先检查/etc/initscript是否存在，如果存在的话，就使用这个脚本去启动新的进程。

	每当一个进程终止的时候，init会把事实和它终止的原因记录到/var/run/utmp和/var/run/wtmp中。

CHANGING RUNLEVELS

	当init进程启动完所指定的进程之后，它就会等待以下三种情况的发生：
	* 一个后代进程终止
	* powerfail信号
	* telinit发送的信号，改变init的runlevvel
	当以上三种情况发生时，init会重新检查/etc/inittab文件。该文件可以
在任何时候被修改，然而这种修改并不会立即生效，而是继续等待上述三个事件的发生后，才会区检查这个文件。如果想立即生效的话，可以使用命令：telinit Q 或telinit q，来唤醒init重新检查/etc/inittab的值。

	如果init在非单用户模式时受到了powerfail信号（SIGPWR），那么它会去读取/etc/powerstatus文件，并且根据文件的内容执行一个命令：
	F（AIL）：电源已经失效，由UPS供电，此时执行powerwait和powerfail对应的表项；
	O（K）：电源已经恢复，执行powerokwait表项；
	L（OW）：电源已经失效，UPS电量较低，此时执行powerfailnow对应的表项；
	
	如果/etc/powerstatus不存在，或者不包含F，O，L表项，那么init的行为是和上述三种情况中的FAIL一样的。
	直接使用SIGPWR和/etc/powerstatus是不提倡的，可以使用/run/initctl来进行控制。
	
	当init被请求更换runlevel时，它会给在新的runlevel中未定义的进程发送警告信号SIGTERM。在它强制给这些进程发送SIGKILL信号之前会等待5s。需要注意的是init假设，所有这些进程都存在于由init最初为它们所创建的进程组中，如果有某个进程改变了自己所属的进程组，则该进程需要被另外单独的杀死。

TELINIT

	/sbin/telinit被连接到/sbin/init。它接受一个字符的参数，并且给init发
送信号做出合适的操作。下面的参数作为telinit的指令：

	0,1,2,3,4,5 or 6
		告诉init切换到指定的runlevel。
	a,b or c
		告诉init仅仅处理/etc/inittab中含有a,c或c的表项。
	Q or q
		告诉init去重新检查/etc/inittab文件。
	S or s
		告诉init切换到单用户模式。
	U or u
		告诉init重新执行它自己（保留状态），不必重新检查/etc/inittab文件。
	runlevel应该是Ss0123456中的一个，否则请求会被忽略。

	telinit可以告诉init在发送SIGTERM和SIGKILL信号之间需要等待多长时间，默认是5s，可以通过-t参数进行修改。
	
	telinit -e可以修改由init产生的进程的执行环境，它由两种格式：VAR=VAL，将环境变量VAR设置为VAL；VAR取消环境变量VAR的值。

	telinit必须被有合适的特权级的用户所调用。

	init二进制文件通过查看进程的PID来区分telinit和init进程，init进程的PID总是1。因此可以使用init来替代telinit，可以少输一些字符。

ENVIRONMENT

	init为它的孩子设置下面这些环境变量：

	PATH   /bin:/usr/bin:/sbin:/usr/sbin

	INIT_VERSION
		就像名字所暗示的那样，对于决定一个脚本是否是由init直接执行的很有用；

	RUNLEVEL
		当前系统的runlevel；

	PREVLEVEL
		之前的运行级别，对于切换runlevel很有用；

	CONSOLE
		系统的console，直接从内核继承；如果没有被设置的话，init会将它设置为
	/dev/console；

BOOTFLAGS
	在启动监视器中可以传递给init几个标记（例如LILO），init接收下面几个标记：

	-s, S, single
		单用户模式启动。在这种模式下，/ect/inittab被检查，并且启动rc脚本会在单用户模式shell启动之前执行；

	1-5  Runlevel to boot into.

	-b, emergency
		直接启动进入到单用户模式shell，而不用执行其他任何的脚本；

	-a, auto
		LILO boot loader在内核以默认的命令行启动时，它会把"auto"添加到内核命令行；如果发现上述情况，init把"AUTOBOOT"缓降变量设置为"yes"。注意，你不能在任何安全方法中使用这种方式--当然用户可以手动的指定"auto"和"-a"参数；

	-z xxx
		-z参数会被忽略。可以利用这一点来扩展命令行，所以它会使栈空间变大。init之后可以操作命令行，所以ps会显示当前的运行级别；

INTERFACE

	init监听存在于/run/下面的一个管道initctl，telinit使用这种方式来和init通信。

SIGNALS

	init会对几个信号进行相应：
	SIGHUP
		和telinit q效果一样；

	SIGUSR1
		当受到这个信号时，init关闭并重新打开/run/initctl；

	SIGINT
		当内核发送这个俄信号给init时，说明CTRL+ALT+DEL被按下了，这时inittab中ctrlaltdel表项就被激活了；

	SIGWINCH
		当内核发送这个信号给init时，说明KeyboardSignal按键被按下了，一般是ALT+上箭头，会激活/etc/inittab中的kbrequest表项；

CONFORMING TO
	init和System V init是兼容的。它需要和/etc/init.d和/etc/rc{runlevel}.d中的脚本紧密的合作。

FILES

	/etc/inittab
	/etc/initscript
	/dev/console
	/var/run/utmp
	/var/log/wtmp
	/run/initctl
	
WARNINGS

	在debian中，进入runlevel 1会导致系统中的所有进程都被杀死，除了内核线程和执行kill的脚本以及和它在同一个会话中的进程。结果就是，从runlevel 1返回到多用户的runlevel是不安全的：在runlevel S中启动的为一些普通操作服务精灵进程已经不存在了，系统应该被重启。

DIAGNOSTICS

	如果init发现它在两分钟内连续执行一个表项超过10次，它就会假设命令字符串发生了错误，在系统console上产生一个错误信息，并且拒绝继续执行这个表项，直到过了5分钟，或者是受到了一个信号。这就避免了因为/etc/inittab中的拼写错误，或者是某个表项对应的程序被删除，而造成的系统资源被消耗光。
