###man 5 inittab

	这篇文档基本是从'man 5 inittab'中翻译过来的。

INITTAB(5)

NAME
	inittab - 被sysv兼容的init进程所使用的inittab文件的格式。

DESCRIPTION
	inittab文件描述了在系统启动和正常运行的过程中，需要启动哪
些进程（e.g. /etc/init.d/boot, /etc/init.d/rc, gettys...）。
init可以区分多个不同的运行级别，每个运行级别包括他自己需要启动
的一个进程的集合。表项包括0-6以及ondemand表项A，B和C。一个在inittab
中表项符合下面的格式：
	id:runlevels:action:process
以#开头的行被忽略掉；
	
	id - 是一个文件中独一无二的字符序列，它由1-4个字符组成，标
识了inittab中的一个表项；
	注意：传统的，getty和其他登录程序会采用其所使用的tty的后缀
来作为标识，例如1是tty1；

	runlevels - 指定了应该在哪些runlevel运行该表项；

	action - 描述了应该采取什么措施；

	process - 指定哪个进程应该被执行。如果process域以一个'+'号
开头，那么utmp和wtmp程序将不会对该进程进行统计。这对于那些坚持
要自己做utmp和wtmp统计的gettys来说是必需的。这同样是历史上遗留
的bug；

	runlevels会包含多个表示不同runlevel的字符；例如，123表示进
程应该在123 runlevel启动。对于ondemand表项可以包括A，B和C。sysinit，
boot以及bootwait表项的runlevels域被忽略掉了。

	当系统的runlevel改变的时候，任何没有在新的runlevel中指定的
进程都会被杀死，现发送SIGTERM信号，然后发送SIGKILL信号。

	有效的action域如下：

	respawn
		不管什么时候，只要进程终止就会被重启，例如getty；

	wait
		一旦进入到指定的runlevel，进程就会被启动，并且init进程
	会等待它的结束；

	once
		进程只会在进入到指定的runlevel时运行一次；

	boot
		进程只会在系统启动的时候执行，runlevels域被忽略；

	bootwait
		进程只会在系统启动的时候执行，init会等待它的终止，例如
	/etc/rc。runlevel域被忽略；

	off
		什么也不做；

	ondemand
		一个被标记为ondemand runlevel的进程将会在任何指定ondemand
	runlevel被调用时执行，然而不会进行任何runlevel的切换。ondemand
	runlevel的级别为a，b和c；

	initdefault
		initdefault表项指定了在系统启动后应该进入到哪一个runlevel；
	如果这样的表项不存在的话，init将会在console上面询问要进入到哪一
	个runlevel；process域被忽略掉；

	sysinit
		进程将会在系统的启动过程中执行；它将会在所有的boot和bootwait
	的表项在执行前执行；runlevels域被忽略掉了；

	powerwait
		当电源断电（go down）时，相应的进程被执行；init通过一个进程
	告知UPS（不间断电源）电源连接到这台计算机上获得通知；init在继续
	执行前会等待这个进程的完成；

	powerfail
		和powerwait一样，只不过是powerfail不会等待进程的执行结束；
	
	powerokwait
		当init进程接到通知电源已经恢复，进程会立即被执行；

	powerfailnow
		当init进程被告知UPS电量已经为空，并且电源已经失效，这时进程
	会立即被执行；

	ctrlaltdel
		当init进程接收到SIGINT信号时，进程会立即被执行；这就意味着系
	统中某个用户在系统的console，按下了组合键CTRL+ALT+DEL；通常的操作
	是执行某些类型的shutdown，切换到单用户模式或者是重启；

	kbrequest
		当init进程接收到来自键盘处理函数的一个信号，这个信号是某个用
	户在console下按下了系统特殊的组合键，进程就会被执行；

EXAMPLES
	下面是一个inittab的例子，它有点像旧版本的Linux中的inittab：
		
		# inittab for Linux
		id:1:initdefault:
		rc::bootwait:/etc/rc
		1:1:respawn:/etc/getty 9600 tty1
		2:1:respawn:/etc/getty 9600 tty2
		3:1:respawn:/etc/getty 9600 tty3
		4:1:respawn:/etc/getty 9600 tty4

	这个inittab文件在系统启动的过程中执行/ect/rc，然后在tty1 - tty4上
启动gettys。
	
	下面是一个有着不同runlevel的更精细的inittab：
	
		# LEVEL to run in
		id:2:initdefault:

		# Boot-time system configuration/initialization script
		si::sysinit:/etc/init.d/rcS

		# What to do in single user mode
		~:S:wait:/sbin/sulogin

		# /etc/init.d executes S and K scripts upon change
		# of runlevel
		# Runlevel 0 is halt
		# Runlevel 1 is singgle user mode
		# Runlevel 2-5 is multi-user mode
		# Runlevel 6 is reboot
		l0:0:wait:/etc/init.d/rc 0
		l1:1:wait:/etc/init.d/rc 1
		l2:2:wait:/etc/init.d/rc 2
		l3:3:wait:/etc/init.d/rc 3
		l4:4:wait:/etc/init.d/rc 4
		l5:5:wait:/etc/init.d/rc 5
		l6:6:wait:/etc/init.d/rc 6

		# What to do at "3 finger salute"
		ca::ctrlaltdel:/sbin/shutdown -t1 -h now

		# Runlevel 2,3: gettty on virtual consoles
		# Runlevel 3: getty on terminal(ttyS0) and modem(ttyS1)
		1:23:respawn:/sbin/getty tty1 VC linux
		2:23:respawn:/sbin/getty tty2 VC linux
		3:23:respawn:/sbin/getty tty3 VC linux
		4:23:respawn:/sbin/getty tty4 VC linux
		S0:3:respqwn:/sbin/getty -L 9600 ttyS0 vt320
		S1:3:respawn:/sbin/mgetty -x0 -D ttyS1
