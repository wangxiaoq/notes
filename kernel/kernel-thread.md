###kernel thread

- First, define a function that kernel thread will execute, function 
  prototype is as follows:

```
	int thread_function(void *data);
```

  This function can do anything it wants, but it must handle the signal
  by itself, that is it should check if there are some signals pending.

- Use the following function to create a kernel thread:

```
	struct task_struct *kthread_create(int (*threadfn)(void *data),
			void *data, const char *namefmt, ...);
```

  The above function just creates a new kernel thread, but the kernel 
  thread is not running, to make it run, pass the return value of kthread_create
  to wake_up_process funxtion.

- There is a function to create a kernel thread and run it:

```
	struct task_struct *kthread_run(int (*threadfn)(void *data),
			void *data, const char *namefmt, ...);
```

- There are two ways for a kernel thread to exit:
	* the thread explicitly call do_exit();
	* some one call kthread_stop();

  kthread_stop function is as follows:

```
	int kthread_stop(struct task_struct *thread);
```

  kthread_stop works through sending signals to the target thread, so
  when the thread is executing some important work, it will not be interrupted.
  However, if the thread never handle signals, it will run forever.

- Kernel thread always run on a specified cpu, so the following functions is needed:

```
	void kthread_bind(struct task_struct *thread, int cpu);
```

*references*

*1. https://lwn.net/Articles/65178/*
