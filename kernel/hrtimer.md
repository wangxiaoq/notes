### hrtimer(high resolution timers)

	hrtimer使用了一个排序的链表来组织定时器，即将超时的定时器被放在了链表的头部。另外，hrtimer还用一个红黑树来进行组织，这样在从链表中删除定时器时就不需要遍历链表了。这种组织方式与进程地址空间中的vma组织结构类似。

	hrtimer引入了一个新的结构ktime_t，它存在于linux/ktime.h中，是一个不透明的结构，用来表示纳秒。它依赖于底层的架构，在64位体系结构上是一个64位的整数，在32位体系结构上是两个32位整数，一个用来表示纳秒，另一个用来表示秒。

	与ktime相关的接口为：
	定义：

```
	DEFINE_KTIME(name);		/* initialize to zero */

	ktime_t kt;
	kt = ktime_set(long secs, long nanosecs);
```

	改变ktime_t的接口：

```
	ktime_t ktime_add(ktime_t kt1, ktime_t kt2);
	ktime_t ktime_sub(ktime_t kt1, ktime_t kt2);	/* kt1 - kt2 */
	ktime_t ktime_add_ns(ktime_t kt, u64 nanosecs);
```

	转换接口：

```
	ktime_t timespec_to_ktime(struct timespec tspec);
	ktime_t timeval_to_time(struct timeval tval);
	struct ktime_to_timespec(ktime_t kt);
	struct ktime_to_timeval(ktime_t kt);
	clock_t ktime_to_clock_t(ktime_t kt);
    u64 ktime_to_ns(ktime_t kt);
```
	与hrtimer相关的接口都在文件linux/hrtimer.h中，hrtimer通过一个struct hrtimer结构体来表示，可以用下面的函数来初始化这个结构体：

```
	void hrtimer_init(struct hrtimer *timer, clockid_t which_clock);
```

	每一个hrtimer绑定到一个特定的时钟上，目前系统支持两个时钟：

```
	CLOCK_MONOTONIC：这个时钟保证一直不断的增长，但是无法表示“wall clock
time”。它和jiffies相似，从系统启动开始就从0开始增长。
	CLOCK_REALTIME：和当前真实世界的时间相符。
```
	
	这两个是时钟的差别在系统时间被调整的时候就可以看到。CLOCK_MONOTONIC感受不到系统时间的调整，就像什么都没有发生一样。CLOCK_REALTIME却会感觉到不连续的时钟变化。hrtimer_init把时钟绑定到一个定时器上，如果需要调整定时器的时钟可以使用hrtimer_rebase：

```
	void hrtimer_rebase(struct hrtimer *timer, clockid_t new_clock);
```

	hrtimer中的大多数域不应该被修改，然而有两个域必须由用户来进行设置：

```
	int (*function)(void*);
	void *data;
```
	function就是超时处理函数，而data就是该函数的参数。

	通过下面的函数来完成：

```
	int hrtimer_start(struct hrtimer *timer, ktime_t kt, 
			enum hrtimer_mode mode);
```

	mode参数描述了时间参数如何被解释。HRTIMER_ABS表示时间是一个绝对时间。而HRTIMER_REL表示时间应该解释为相对于当前的时间。
	
	在一般情况下，function会在时钟超时后调用。对于function的作用仅仅是唤醒睡眠进程的情况，hrtimer提供了一种捷径：将function置为NULL，将data参数指向需要被唤醒的进程的task_struct，在时钟超时后，就会唤醒data指向的进程。该函数必须返回一个int类型的整数，HRTIMER_RESTART或者HRTIMER_NORESTART。

	对于HRTIMER_RESTART类型，是由内核子系统用来周期性的执行一些任务。必须在处理函数返回之前重新设置超时时间。内核提供了下面的函数用来设置：

```
	unsigned long hrtimer_forward(struct hrtimer *timer, ktime_t interval);
```

	函数会将定时器设置为在未来的interval时间内超时，如果由必要的话，会将interval增加多次，来放弃未来的一次或多次超时。通常需要增加多个interval就意味着系统已经超过了定时器的时间范围，这可能是由于系统负载造成的。从hrtimer_forward的返回值表示错过的interval。

	定时器可以通过下面两个函数来进行撤销：

```
	int hrtimer_cancel(struct hrtimer *timer);
	int hrtimer_try_to_cancel(struct hrtimer *timer);
```

	hrtimer_cancel返回0，表示在调用这个函数之前定时器已经失效了，返回1表示定时器被成功的取消。hrtimer_try_to_cancel类似，但是在超时处理函数在运行的情况下该函数不会等待，而是直接返回-1。

	一个被取消的时钟可以通过函数：

```
	int hrtimer_restart(struct hrtimer *timer);
```

	重新启动。

	hrtimer_get_remaining返回超时前的剩余时间，hrtimer_active会返回非0值，如果当前定时器存在于队列上。

```	
	int hrtimer_get_res(clockid_t which_clock, struct time_spec *tp);
```

将会返回指定时钟的以纳秒级别的分辨率。

*ref*

*https://lwn.net/Articles/167897/*
