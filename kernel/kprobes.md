###Kernel Probes(kprobes)

1.	Concepts: kprobes, jprobes, and return probes

	kprobes允许动态的插入到任何内核例程中，收集一些debug和性能
	信息，而不会打扰内核例程的执行。kprobes几乎可以插入到任何的
	内核例程中，还有一小部分kprobes不能插入（在1.5小节的blacklist
	中有描述）。当例程执行到kprobes的插入点时，就执行定义的处理
	函数。

	目前存在三种probes：kprobes，jprobes，kretprobes（return probes）；
	kprobe可以插入在任何指令的位置；jprobe只能插在函数的入口处，为访问
	函数参数提供便利；kretprobe只能插入在函数返回的时候。

	通常情况下，kprobes使用模块来注册probe。在模块的初始化函数中
	注册kprobe，在模块的退出函数中，删除kprobe；使用register_kprobe
	来进行注册，需要指定probe的位置以及当该probe被命中时应该执行
	的处理函数。

	同样也提供了register/unregister_*probes函数，来注册/删除一批probe，
	这尤其在删除一批probe时非常有用，可以加快删除的速度。

	接下来的四个小节将介绍各种类型的probe是如何工作的，以及是如
	何进行跳转优化的；并且介绍了一些必需的知识，可以让你更好的使
	用kprobes--例如，pre_handler与post_handler的区别，以及如何使
	用中的maxactive以及nmissed域。

	1.1	kprobe是如何工作的？

	当一个kprobe被注册的时候，它会复制将要被替换的指令，并且将被
	替换的指令替换为一个断点指令（例如，i386和x86_64平台上的int3）。

	当CPU命中一个断点指令的时候，就会发生一个陷阱，这时CPU的寄存器
	内容会被保存，并且控制流通过notifier_call_chain进入到kprobe去执
	行；首先会执行pre_handler函数，把kprobe结构体的地址和保存的寄存
	器传递给该函数；

```
	Next, Kprobes single-steps its copy of the probed instruction.
	(It would be simpler to single-step the actual instruction in place,
	but then Kprobes would have to temporarily remove the breakpoint
	instruction.  This would open a small time window when another CPU
	could sail right past the probepoint.)
```

	在指令被singgle-step之后，Kprobes开始执行"post_handler"，这是和kprobe
	相关的。之后继续沿着kprobe之后的指令开始执行。

	1.2 jprobe是如何工作的？
	
	jprobe是利用kprobe在函数的入口处实现的；通过简单映射原理可以无
	缝的访问被探测函数的参数；jprobe的处理函数必须和被探测函数有着
	相同的签名（即函数原型必须一致），并且在处理函数必须以调用jprobe_return
	作为结束。

	
