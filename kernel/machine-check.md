###MCE

####基础概念

1. MCE机制用于检测内部芯片的错误和外部总线事务的错误[1]。
2. 由外部代理检测到的总线错误，通过CPU上的特定引脚报告。
   对这个引脚的触发，会导致错误信息加载到MSR寄存器中，
   并产生一个MCE[2]。
3. MCE机制是否使能与CR4控制寄存器中的一个标记是否设置相
   关。若MCE机制没有使能，当产生一个MCE时，处理器会进入
   shutdown状态[2]。
4. 处理器的shutdown状态，就是将这个处理器离线，之后调度
   器不会再考虑这个处理器，但是该处理器仍处于空转的状态，
   耗电较少。在linux下，我们可以通过下面的命令让一个处理
   器进入shutdown状态：
   $ echo 0 > /sys/devices/system/cpu/cpu1/online
   需要注意的是cpu0没有online文件[3]。

*ref*

*1. 6.4.3 of Intel SDM Volume 3A*

*2. 6.15 Interrupt 18—Machine-Check Exception (#MC) of SDM Volume 3A*

*3. http://superuser.com/questions/366969/linux-cpu-core-shutdown-instead-of-sleep-state*

####代码笔记

1.在mce.c中，找到mcheck_cpu_init函数，如下：

```
   /*
    * Called for each booted CPU to set up machine checks.
    * Must be called with preempt off:
    */
   void mcheck_cpu_init(struct cpuinfo_x86 *c)
   {
   	...
   	machine_check_vector = do_machine_check;
   	...
   }
```

   在这个函数中为每一个CPU设置好machine check，尤其是在
   这里设置了machine_check_vector为do_machine_check函数。
   在entry_64.S中定义了MCE处理的入口：

```
   #ifdef CONFIG_X86_MCE
   idtentry machine_check has_error_code=0 paranoid=1 do_sym=*machine_check_vector(%rip)
   #endif
```

2.由上面的分析可知，mce.c文件中do_machine_check函数是真
   正的MCE处理函数下面将对do_machine_check进行分析：

   2.1   这个函数处理的是通过int 18中断报告的某些硬件错误。
         由于这个处理函数所处的上下文是NMI上下文，因此内核
         中的锁机制在这个函数中就不能使用，例如printk。在
         Intel处理器平台上，所有的CPU会通过MCE广播进入到这
         个处理函数中。

   2.2   `struct mca_config *cfg = &mca_cfg;`
   
         struct mca_config定义在mce.h中，该结构体表示当前硬
         件平台MCA架构的一些配置。mca_cfg在mce.c中定义如下：

```
         struct mca_config mca_cfg __read_mostly = {
  		  .bootlog  = -1,
             /*
              * Tolerant levels:
              * 0: always panic on uncorrected errors, log corrected errors
              * 1: panic or SIGBUS on uncorrected errors, log corrected errors
              * 2: SIGBUS or log uncorrected errors (if possible), log corr. errors
              * 3: never panic or SIGBUS, log all errors (for testing only)
              */
             .tolerant = 1,
             .monarch_timeout = -1
	     };
```
	
   2.3   `mce_gather_info(&m, regs);`
   
         收集关于当前发生的MCE的全局状态信息，以便在后来读
         取各个BANK的信息时，可以访问错误的严重级别。
         mce_gather_info的实现如下：

```
	    mce_setup(m);    //初始化mce结构体；

         m->mcgstatus = mce_rdmsrl(MSR_IA32_MCG_STATUS);
         
         if (m->mcgstatus & (MCG_STATUS_RIPV|MCG_STATUS_EIPV)) {
                m->ip = regs->ip;
                m->cs = regs->cs;
                ...
         }

         /* Use accurate RIP reporting if available. */
         if (mca_cfg.rip_msr)
              m->ip = mce_rdmsrl(mca_cfg.rip_msr);
```

   2.4   
         ```
         final = &__get_cpu_var(mces_seen);
         *final = m;
         ```

         在mce.c中定义了mces_seen，如下：

```
         static DEFINE_PER_CPU(struct mce, mces_seen);
```

         上面的两条语句的含义显然是首先获取每CPU变量mces_seen，
         然后再将之前收集到的MCE信息赋值给它。

   2.5   ```
         memset(valid_banks, 0, sizeof(valid_banks));      //初始化
         no_way_out = mce_no_way_out(&m, &msg, valid_banks, regs);
         ```
         
         mce_no_way_out依次遍历系统中的各个BANK，读取MSR_IA32_MCi_STATUS
         寄存器中的信息，并根据MCi_STATUS寄存器中的信息判断当前的
         错误严重级别是否已经到达了PANIC。其核心代码如下：

```
         for (i = 0; i < mca_cfg.banks; i++) {
             m->status = mce_rdmsrl(MSR_IA32_MCx_STATUS(i));
             if (m->status & MCI_STATUS_VAL) {
                 __set_bit(i, validp);
                 if (quirk_no_way_out)
                      quirk_no_way_out(i, m, regs);
             }
             if (mce_severity(m, mca_cfg.tolerant, msg) >= MCE_PANIC_SEVERITY)
                 ret = 1;
          }
```
         
         2.5.1 quirk_no_way_out
         其中比较有意思的是函数quirk_no_way_out，在mce.c中的函数
         __mcheck_cpu_apply_quirks中可以发现下面的代码：

```
         if (c->x86 == 6 && c->x86_model == 45)
                quirk_no_way_out = quirk_sandybridge_ifu;
```

         在mce.c中找到了函数quirk_sandybridge_ifu，从注释可以看
         出这个函数存在的意义是补救之前在mce_gather_info函数中漏
         掉登记的cs/ip信息。
         在Intel SDM 3A 15-20表中可以看到，对于Instruction fetch
         类型的SRAR错误，MCG_STATUS中的EIPV和RIPV均被清零，这就给
         错误严重程度估计代码造成了困惑。因此，在这里进行检测看是
         否该错误就是Instruction fetch类型的错误，如果符合条件，
         就将读取到mce中的错误信息进行修改，如下：
        
         m->mcgstatus |= MCG_STATUS_EIPV;
         m->ip = regs->ip;
         m->cs = regs->cs;

         2.5.2 mce_severity
         该函数存在于mce_severity.c文件中，将收集到mce结构体中的
         信息和预定义的错误类型相比较，找到该错误所属的错误类型，
         并返回错误的严重级别。

   2.6   `order = mce_start(&no_way_out);`

         根据进入该函数的CPU的顺序，给相应的CPU指定次序。第一个进
         入的为Monarch（主），其余的为从。代码如下：

         `order = atomic_inc_return(&mce_callin);`

         然后等待所有的CPU都进入mce_start：

         `while (atomic_read(&mce_callin) != cpus)` 
         
         等所有的CPU都就绪以后，所有的CPU按照之前排好的次序，依次
         串行执行do_machine_check后面的代码，代码如下：

```
         if (order == 1) {
         		 atomic_set(&mce_executing, 1);
         } else {
         	 	 while (atomic_read(&mce_executing) < order) {
                     ...
                 }
         }
```

         所有CPU串行执行的目的是，对于某些共享的BANK只需要由一个
         CPU读取一次，然后清零，防止多个CPU重复读取。
         
   2.7   ```
         if (!mce_banks[i].ctl)
                continue;
         ```
         
         mce_banks保存了与系统中所有的BANK信息，它是一个mce_bank
         数组，mce_bank在mce-internal.h中定义如下：

```
         /* One object for each MCE bank, shared by all CPUs */
         struct mce_bank {
              u64            ctl;                /* subevents to enable */
              unsigned char init;                 /* initialise bank? */
              struct device_attribute attr;              /* device attribute */
              char attrname[ATTR_LEN]; /* attribute name */ 
         };
```

         在函数__mcheck_cpu_mce_banks_init中，可以看到对mce_banks
         的初始化代码：

```
         mce_banks = kzalloc(num_banks * sizeof(struct mce_bank), GFP_KERNEL);
         ...
         for (i = 0; i < num_banks; i++) {
          	struct mce_bank *b = &mce_banks[i];

	          b->ctl = -1ULL;  //注意：这里实际上相当于向b->ctl写入全1
     	     b->init = 1;
         }
```

         在__mcheck_cpu_init_generic函数中，可以看到对每一个BANK的
         初始化代码：

```
         for (i = 0; i < mca_cfg.banks; i++) {
      	     struct mce_bank *b = &mce_banks[i];

          	if (!b->init)
                	continue;
	          wrmsrl(MSR_IA32_MCx_CTL(i), b->ctl);
     	     wrmsrl(MSR_IA32_MCx_STATUS(i), 0);
         }
```

   2.8   
         ```
         if (severity == MCE_AO_SEVERITY && mce_usable_address(&m))
                mce_ring_add(m.addr >> PAGE_SHIFT);
          ```
         
         如果是SRAO类型的错误，并且报告的错误发生的地址是有效的，就
         将错误地址记录到ring_buffer中，当缓冲区溢出以后，忽略后来
         报告的错误地址。
         mce_usable_address函数检测报告的错误地址是否有效，目前只处
         理报告的错误的地址是物理地址的情况。

   2.9   `mce_log(&m);`
         
         该函数首先会调用内核中其他机制的钩子函数，如下：

```
         trace_mce_record(mce);

         ret = atomic_notifier_call_chain(&x86_mce_decoder_chain, 0, mce);
```

         然后将当前的mce信息存入到mcelog中，代码如下：

```
         memcpy(mcelog.entry + entry, mce, sizeof(struct mce));
         wmb();
         mcelog.entry[entry].finished = 1;
```

   2.10  ```
         if (mce_end(order) < 0)
            no_way_out = worst >= MCE_PANIC_SEVERITY;
         ```

         由于在mce_start中只让Monarch CPU先运行，其他的CPU都在等待，
         因此Monarch CPU会首先进入mce_end函数，将控制变量mce_executing
         增1，允许第二个CPU开始遍历所有的BANK，同理第二个进入mce_end
         后，又会允许第三个CPU开始执行。代码如下：

         `atomic_inc(&mce_executing);`

         Monarch会等待其他所有的CPU都进入到mce_end，然后开始执行mce_reign。
         代码如下：
       
         `while (atomic_read(&mce_executing) <= cpus)`

         2.10.1 mce_reign
         mce_reign检测所有的CPU收集的错误信息，找到最严重的错误类型，
         然后检测是否需要PANIC。代码如下：

```
         for_each_possible_cpu(cpu) {
        		int severity = mce_severity(&per_cpu(mces_seen, cpu),
                        mca_cfg.tolerant,
                        &nmsg);
	       	    if (severity > global_worst) {
                msg = nmsg;
                global_worst = severity;
                m = &per_cpu(mces_seen, cpu);
            }
         }
```

         每CPU变量mces_seen中存放的就是该CPU看到的最严重的错误类型，
         上面的代码就是找到系统中最严重的错误。
         
         最后在mce_reign中将每CPU变量mces_seen清零，如下：

```
         for_each_possible_cpu(cpu)
	        memset(&per_cpu(mces_seen, cpu), 0, sizeof(struct mce));
```

         2.10.2 
         在Monarch执行mce_reign的过程中，其他的CPU处于等待的状态，如
         下：
       
         `while (atomic_read(&mce_executing) != 0)`

         在Monarch执行完mce_reign后，将mce_executing设置为0，允许其他
         的CPU开始执行，代码如下：
   
         `atomic_set(&mce_executing, 0);`

   2.11  ```
         if (worst == MCE_AR_SEVERITY) {
            /* schedule action before return to userland */
            mce_save_info(m.addr, m.mcgstatus & MCG_STATUS_RIPV);
            set_thread_flag(TIF_MCE_NOTIFY);
         } else if (kill_it) {
            force_sig(SIGBUS, current);
         }
         ```

         如果是SRAR错误，将错误的地址信息记录到mce_info数组中。

   2.12  ```
        if (worst > 0)
            mce_report_event(regs);

         mce_report_event的实现如下：

         if (regs->flags & (X86_VM_MASK|X86_EFLAGS_IF)) {
             mce_notify_irq();
             mce_schedule_work();
             return;
         }

         irq_work_queue(&__get_cpu_var(mce_irq_work));
         ```

         这里比较奇怪的一个问题是mce_notify_irq函数的注释中写道：

```
         Notify the user(s) about new machine check events.
         Can be called from interrupt context, but not from machine check/NMI context.
```
         即不能在MCE/NMI上下文中调用，而当前该函数所处的上下文就
         是MCE上下文，感觉有点矛盾啊！

         2.12.1 mce_notify_irq

```
         if (test_and_clear_bit(0, &mce_need_notify)) {
         /* wake processes polling /dev/mcelog */
         wake_up_interruptible(&mce_chrdev_wait);

         if (mce_helper[0])
             schedule_work(&mce_trigger_work);
         ...
```
         
         （1）
         mce_need_notify第0位在mce_log中被设置为1，表示需要通知用
         户态的错误收集进程收集MCE错误信息。
         接着唤醒等待在字符设备/dev/mcelog上的用户空间的程序（如
         mcelog）。在mce.c中可以发现下面的代码：

```
         static DECLARE_WAIT_QUEUE_HEAD(mce_chrdev_wait);
```

         这行代码定义了一个等待队列，在mce_chrdev_poll中将用户空间
         的程序等在这个队列上，代码如下：

```
         poll_wait(file, &mce_chrdev_wait, wait);
```

         用户空间的进程调用poll函数就等待在这个等待队列上。所以，
         如果发生MCE错误的话，内核就会通知用户空间的进程收集错误
         信息。
                 
         （2）
         mce_helper是用户空间的一个函数的名字，我们需要在这里进行
         调用。使用schedule_work进行调度，mce_trigger_work是一个工
         作，定义在mce.c中：

```
         static DECLARE_WORK(mce_trigger_work, mce_do_trigger);
```

         在mce_do_trigger中所做的工作就是调用用户空间的程序，代码如
         下：

```
         call_usermodehelper(mce_helper, mce_helper_argv, NULL, UMH_NO_WAIT);

```
         
         在/sys/devices/system/machinecheck/machinecheck0/trigger
         中可以设置mce_helper的值，代码如下：
 
         `static DEVICE_ATTR(trigger, 0644, show_trigger, set_trigger);`

         show_trigger函数负责显示，set_trigger负责设置trigger内容。

         2.12.2 mce_schedule_work
         该函数的实现非常简单，代码如下：

```
         if (!mce_ring_empty())
              schedule_work(&__get_cpu_var(mce_work));         
```

         mce_work的定义如下：

```
         static DEFINE_PER_CPU(struct work_struct, mce_work);
```

         mce_work需要执行的任务由下面的代码指定：

```
         INIT_WORK(&__get_cpu_var(mce_work), mce_process_work);
```

         可以看到指定的任务为mce_process_work，而mce_process_work
         函数是用来处理SRAO类型的错误，在该函数中依次遍历之前在
         do_machine_check函数中记录到ring buffer中的错误地址，调用
         memory_failure函数来进行处理。代码如下：

```
         while (mce_ring_get(&pfn))
                memory_failure(pfn, MCE_VECTOR, 0);
```

         2.12.3 memory_failure
         该函数在两个地方都有定义，一个是mce.c中，它的定义如下：

```
         #ifndef CONFIG_MEMORY_FAILURE
		 int memory_failure(unsigned long pfn, int vector, int flags)
		 {
             /* mce_severity() should not hand us an ACTION_REQUIRED error */
             BUG_ON(flags & MF_ACTION_REQUIRED);
             pr_err("Uncorrected memory error in page 0x%lx ignored\n"
                    "Rebuild kernel with CONFIG_MEMORY_FAILURE=y for smarter handling\n",
                    pfn);

             return 0;
         }
         #endif
```

         可见该函数只是打印一些提示信息并没有做什么实际的工作，而
         它的真正实现存在于memory-failure.c中。

         memory_failure用来处理一个发生硬件错误的内存页框，或者隔
         离页框，或者杀死进程。且发生的错误与当前进程运行的上下文
         无关，一般是由一个后台运行的scruber发现的，因此该函数主要
         处理SRAO类型的错误。需要注意的是，该函数只能在进程上下文
         中调用（例如，工作队列），且不能持有自旋锁。

```
         if (!sysctl_memory_failure_recovery)
              panic("Memory failure from trap %d on page %lx", trapno, pfn);         
```

         sysctl_memory_failure_recovery的值可以由用户空间通过proc
         文件系统进行设置，设置的入口为：

         /proc/sys/vm/memory_failure_recovery

         如果该值被设置为0的话，直接panic内核。

```
         if (!pfn_valid(pfn)) {
                printk(KERN_ERR
                       "MCE %#lx: memory outside kernel control\n",
                       pfn);
                return -ENXIO;
         }
```

         检测当前的页框号是否有效。首先根据页框号获取当前页框所属的
         节点（如果配置了NUMA），然后检测当前页框号是否在给定的节点
         中。

         `p = pfn_to_page(pfn);`

         由页框号获取该页框的页描述符（struct page）。

         `hpage = compound_head(p);`


*ref*

*1. Documentation/x86/entry_64.txt*

*2. 工作队列：
   http://www.ibm.com/developerworks/linux/library/l-tasklets/index.html*

####问题

1. IST（interrupt stack table）的作用?
2. 有符号数和无符号数的区别?
   http://bbs.bccn.net/thread-334562-1-1.html
3. mce_rdmsrl(msr)
   这个函数是rdmsrl的封装，主要是配合错误注入使用。在mce.h中定义了
   每CPU变量：

   `DECLARE_PER_CPU(struct mce, injectm);`

   在mce_rdmsrl中首先检测当前CPU中injectm.finished标记是否置位，如
   果置位表明当前发生的MCE是通过软件注入模拟的，injetcm中的bank表示
   错误注入在哪一个BANK中。如果是模拟的话，根据需要读取的寄存器msr
   计算表示这个msr的成员在struct mce结构体中的相对于结构体的起始地
   址injectm的偏移。然后再根据偏移返回injectm中的相应成员，如下：

   `return *(u64 *)((char *)&__get_cpu_var(injectm) + offset);`

   如果发生的MCE错误不是模拟注入的话，就直接进行读取并返回，如下：

```
   if (rdmsrl_safe(msr, &v)) 
   ...
   return v;
```
   
   
