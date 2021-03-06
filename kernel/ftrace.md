###Linux内核调试工具 Ftrace 进阶使用手册

本资料来自 http://blog.csdn.net/longerzone/article/details/16884703，在此致谢

Ftrace 是一个内核中的追踪器，用于帮助系统开发者或设计者查看内核运行情况，它可以被用来调试或者分析延迟/性能问题。最早 ftrace 是一个 function tracer，仅能够记录内核的函数调用流程。如今 ftrace 已经成为一个framework，采用 plugin 的方式支持开发人员添加更多种类的 trace 功能。

一、Ftrace 的内核配置

ftrace 相关的配置选项列表

```
     CONFIG_FUNCTION_TRACER
     CONFIG_FUNCTION_GRAPH_TRACER
     CONFIG_CONTEXT_SWITCH_TRACER
     CONFIG_NOP_TRACER
     CONFIG_SCHED_TRACER
```

在内核的Menuconfig中查看更加直观：

```
    Kernel hacking  --->
        Tracers ─>
            [*]   Kernel Function Tracer
            [*]     Kernel Function Graph Tracer (NEW)
            ...  (下面还有几个追踪器的选项，可以根据自己的需要选择)
```

注： 如果是在 32 位 x86 机器上，编译时不要选中 General setup 菜单项下的 Optimize for size 选项，否则就无法看到 Kernel Function Graph Tracer 选项。这是因为在 Konfig 文件中，针对 32 位 x86 机器，表项 FUNCTION_GRAPH_TRACER 有一个特殊的依赖条件：

```
             depends on !X86_32 || !CC_OPTIMIZE_FOR_SIZE
```

Ftrace 通过 debugfs 向用户态提供了访问接口，所以还需要将 debugfs 编译进内核。激活对 debugfs 的支持，可以直接编辑内核配置文件 .config ，设置CONFIG_DEBUG_FS=y ；或者在 make menuconfig 时到 Kernel hacking 菜单下选中对 debugfs 文件系统的支持：

```
    Kernel hacking  --->
        -*- Debug Filesystem
```

二、Ftrace 的基本使用步骤


2.1. 挂载Debugfs:

Ftrace 通过 debugfs 向用户态提供访问接口。配置内核时激活 debugfs 后会创建目录 /sys/kernel/debug ，debugfs 文件系统就是挂载
到该目录。

2.1.1 运行时挂载：

官方挂载方法 :

```
# mount -t debugfs nodev /sys/kernel/debug
```

我觉得自己建一个 /debug 文件夹，挂载在这个路径下使用比较方便：

```
# mkdir /debug
# mount -t debugfs nodev /debug
# cd /debug/tracing                    // debugfs挂载路径下的tracing 才是 ftrace 的“大本营”！
```

或者我们可以使用官方挂载办法后建立一个软连接：

```
# mount -t debugfs nodev /sys/kernel/debug
# ln -s /sys/kernel/debug  /debug 
```

2.1.2 系统启动自动挂载：

要在系统启动自动挂载debugfs，需要将如下内容添加到 /etc/fstab 文件：

```
        debugfs  /sys/kernel/debug  debugfs  defaults  0  0
```


2.2. 选择一种 tracer:

```
# cat current_tracer           // 查看当前追踪器
nop   // no option
# cat available_tracers      // 查看当前内核中可用跟踪器
blk function_graph function nop
# echo function_graph > current_tracer        // 我们选用 function_graph 追踪器
```

2.3. 打开关闭追踪

在老一点版本的内核上tracing目录下有tracing_enabled，需要给tracing_enabled和tracing_on同时赋值 1 才能打开追踪，而在比较新的内核上已经去掉 tracing_enabled ，我们只需要控制tracing_on 即可打开关闭追踪。

```
#echo 1 > tracing_on             // 打开跟踪
# echo 0 > tracing_on             // 关闭跟踪
# echo 1 > tracing_on; run_test; echo 0 > tracing_on          //  打开跟踪后做一件事再关闭跟踪，所以此次跟踪的结果基本上是这个运行的程序的跟踪结果，但是肯定会包括很多杂讯，所以后面会介绍跟踪某个 pid 的或者跟踪某个函数。
```

注：对于为什么去掉 tracing_enabled 我问过 Ftrace 的维护人Steven Rostedt，他说使用 tracing_on 可以快速的打开 Ftrace 的追踪，这让 tracing_enabled 显得很轻量级或者说显得比较冗余，下面可以会说到，我们写内核程序时可以使用Ftrace 提供的内核函数 tracing_on() or tracing_off() 直接打开追踪，这其实就是使用的 tracing_on ，所以在新内核中 tracing_enabled 这个看起来比较冗余的选项已经被删除。

2.4. 查看追踪结果

ftrace 的输出信息主要保存在 3 个文件中。

*   trace，该文件保存 ftrace 的输出信息，其内容可以直接阅读。
*   latency_trace，保存与 trace 相同的信息，不过组织方式略有不同。主要为了用户能方便地分析系统中有关延迟的信息。
*   trace_pipe 是一个管道文件，主要为了方便应用程序读取 trace 内容。算是扩展接口吧。


所以可以直接查看 trace 追踪文件，也可以在追踪之前使用trace_pipe 将追踪结果直接导向其他的文件。
比如： 

```
# cat trace_pipe > /tmp/log &     // 使用trace_pipe 将跟踪结果导入 /tmp/log 里，我们可以直接 “ cat /tmp/log” 查看跟踪信息。
```

当然也可以直接查看trace文件 #cat trace 或者使用 cat trace  > /tmp/log 将跟踪信息导入 /tmp/log


三、 Ftrace 的进阶使用：

3.1. 追踪指定的进程

使用 echo pid > set_ftrace_pid  来追踪指定的进程！
我们写程序时可以使用getpid 获取进程PID，然后使用 write 将pid 写入 /debug/tracing/set_ftrace_pid ，并使用write 写1 到 tracing_on 打开追踪（因为在用户空间使用不了tracing_on函数），此时即可追踪当前这个进程。

3.2. 追踪事件：

3.2.1 首先查看事件文件夹下面有哪些选项

```
# ls events/
block    ext4    header_event  jbd2 napi
 raw_syscalls…… enable
# ls events/sched/
enable     sched_kthread_stop_ret  sched_process_exit  sched_process_wait ……
```

3.2.2 追踪一个/若干事件

```
# echo 1 > events/sched/sched_wakeup/enable
                ...（省略追踪过程）
# cat trace | head -10
# tracer: nop
#TASK-PID  CPU#  TIMESTAMP    FUNCTION
# ||
| |
bash-2613 [001] 425.078164: sched_wakeup: task bash:2613 [120] success=0 [001]
bash-2613 [001] 425.078184: sched_wakeup: task bash:2613 [120] success=0 [001]
...
```

3.2.3 追踪一类事件

```
# echo 1 > events/sched/enable
                ... 
# cat trace | head -10
# tracer: nop
#TASK-PID            CPU#  TIMESTAMP    FUNCTION
#   |                           |                |                     | 
events/0-9               [000]   638.042792:   sched_switch: task events/0:9 [120] (S) ==> kondemand/0:1305 [120]
ondemand/0-1305   [000]    638.042796:  sched_stat_wait: task: restorecond:1395 wait: 15023 [ns]
...
```

3.2.4 追踪所有事件

```
# echo 1 > events/enable
                ...
# cat trace | head -10
# tracer: nop
#TASK-PID     CPU#     TIMESTAMP    FUNCTION
#   |                    |                    |                   | 
cpid-1470       [001]   794.947181:         kfree: call_site=ffffffff810c996d ptr=(null)
acpid-1470     [001]
794.947182:      sys_read -> 0x1
acpid-1470     [001]   794.947183:      sys_exit: NR 0 = 1
...
```

3.3. stack_trace

```
# echo 1 > /proc/sys/kernel/stack_tracer_enabled
OR # kernel command line “stacktrace”
查看： # cat stack_trace
```

3.4. 设置追踪过滤器

将要跟踪的函数写入文件 set_ftrace_filter ，将不希望跟踪的函数写入文件 set_ftrace_notrace。通常直接操作文件 set_ftrace_filter 就可以了. 


四、 Ftrace 提供的函数使用

内核头文件 include/linux/kernel.h 中描述了 ftrace 提供的工具函数的原型，这些函数包括 trace_printk、tracing_on/tracing_off 等。


4.1. 使用 trace_printk 打印跟踪信息

ftrace 提供了一个用于向 ftrace 跟踪缓冲区输出跟踪信息的工具函数，叫做 trace_printk()，它的使用方式与 printk() 类似。可以通过 trace 文件读取该函数的输出。从头文件 include/linux/kernel.h 中可以看到，在激活配置 CONFIG_TRACING 后，trace_printk() 定义为宏：

```
     #define trace_printk(fmt, args...)   \ 
        ...
```

所以在使用时：(例子是在一个内核模块中添加打印信息)

```
 #include <linux/init.h>   
 #include <linux/module.h>   
 #include <linux/kernel.h>   
  
 MODULE_LICENSE("GPL");   
  
 static int ftrace_demo_init(void)   
 {   
     trace_printk("Can not see this in trace unless loaded for the second time\n");   
     return 0;   
 }   
  
 static void ftrace_demo_exit(void)   
 {   
     trace_printk("Module unloading\n");   
 }   
  
 module_init(ftrace_demo_init);   
 module_exit(ftrace_demo_exit);  
```

4.2. 使用 tracing_on/tracing_off 控制跟踪信息的记录

在跟踪过程中，有时候在检测到某些事件发生时，想要停止跟踪信息的记录，这样，跟踪缓冲区中较新的数据是与该事件有关的。在用户态，可以通过向文件 tracing_on 写入 0 来停止记录跟踪信息，写入 1 会继续记录跟踪信息。而在内核代码中，可以通过函数 tracing_on() 和 tracing_off() 来做到这一点，它们的行为类似于对 /sys/kernel/debug/tracing 下的文件 tracing_on 分别执行写 1 和 写 0 的操作。

使用这两个函数，会对跟踪信息的记录控制地更准确一些，这是因为在用户态写文件 tracing_on 到实际暂停跟踪，中间由于上下文切换、系统调度控制等可能已经经过较长的时间，这样会积累大量的跟踪信息，而感兴趣的那部分可能会被覆盖掉了。

实际代码中，可以通过特定条件（比如检测到某种异常状况，等等）来控制跟踪信息的记录，函数的使用方式类似如下的形式：

```
 if (condition) 
tracing_on() or tracing_off()
```

跟踪模块运行状况时，使用 ftrace 命令操作序列在用户态进行必要的设置，而在代码中则可以通过 traceing_on() 控制在进入特定代码区域时开启跟踪信息，并在遇到某些条件时通过 tracing_off() 暂停；读者可以在查看完感兴趣的信息后，将 1 写入 tracing_on 文件以继续记录跟踪信息。实践中，可以通过宏来控制是否将对这些函数的调用编译进内核模块，这样可以在调试时将其开启，在最终发布时将其关闭。

用户态的应用程序可以通过直接读写文件 tracing_on 来控制记录跟踪信息的暂停状态，以便了解应用程序运行期间内核中发生的活动。

五、 简单的 Ftrace 脚本案例：

我在下面使用一个脚本执行 Ftrace 的操作，自动追踪 HelloWorld 程序执行：

```
#!/bin/bash  
debugfs_path=/debug                       
ftrace_call_path=$debugfs_path/tracing  
app_path=/home/dslab/test  
  
# 测试我们的 /debug 路径是否已经建立  
if ! test -d $debugfs_path; then  
        echo "I'll create the directory /debug"  
        mkdir $debugfs_path  
else echo "Hum ,the /debug directory is standing by"  
fi   
  
# 测试debugfs是否已经挂载  
mount | grep "debugfs" > /dev/null  
if [ $? != 0 ]; then  
        echo "we have not mount debugfs"  
        mount -t debugfs nodev $debugfs_path  
else echo "yeah,debugfs has been mounted"  
fi  
  
# 测试ftrace 是否可用  
if ! test -d $ftrace_call_path; then  
        echo "sorry, may be your kernel is not support for the ftrace"  
        exit -1;  
else echo "Hum ,Ftrace is standing by"  
fi        
# 将目前的trace 文件清空  
echo "" >  $ftrace_call_path/trace  
# 选择 function_graph 跟踪器  
echo "function_graph" > $ftrace_call_path/current_tracer  
# 我的Debian7 使用的3.2 内核上还有tracing_enabled ，先给它赋值 1  
echo 1 > $ftrace_call_path/tracing_enabled  
# 使用 trace_pipe 将追踪结果重定向到/tmp/result（后台运行，使得下面的步骤得以进行）  
cat $ftrace_call_path/trace_pipe > /tmp/result &  
# 打开追踪  
echo 1 > $ftrace_call_path/tracing_on  
# 我在这里执行了一个已经编译好helloworld 程序  
exec $app_path/hello  
# 关闭追踪  
echo 0 > $ftrace_call_path/tracing_on  
```

运行效果如下:

```
dslab@wheezy:~$ sudo ./ftrace.sh  
[sudo] password for dslab:   
Hum ,the /debug directory is standing by  
yeah,debugfs has been mounted  
Hum ,Ftrace is standing by  
Hello World  
dslab@wheezy:~$   
```

参考资料：

```
【1】Linux内核中的ftrace 文档，路径为：linux-source-3.2/Documentation/trace/ftrace.txt (还有个ftrace-design.txt也很值得学习)
【2】ftrace 简介 :   http://www.ibm.com/developerworks/cn/linux/l-cn-ftrace/
【3】使用 ftrace 调试 Linux 内核:
            part1   :   http://www.ibm.com/developerworks/cn/linux/l-cn-ftrace1/
            part2   :   http://www.ibm.com/developerworks/cn/linux/l-cn-ftrace2/
            part3   :   http://www.ibm.com/developerworks/cn/linux/l-cn-ftrace3/
```