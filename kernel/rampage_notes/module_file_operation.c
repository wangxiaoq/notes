file_operation.c
================
参考存在于phys_mem.h中的文档，我们可知：

该模块的行为与/dev/mem相似，但是不像/dev/mem那样没有权限
的限制。它的不通知自出在于：
	* 用户配置需要准备的页框的集合，这些页框之后被用来映射。
	* 模块尽力去得到请求的页框。
		用户进程绝不能访问不是为它所保留的页。
		保留的页可以被用户空间进程映射。

关于这个模块的使用方法：
1）	打开这个设备；
2）	使用'Request'-IOCTL将所请求的页框号列表传给驱动，驱动将
	会尽力去获得这些页框；
	所请求的pfn的顺序是对驱动有影响的的：例如，所请求的是页
	框1，2，3，10，那么模块将按照下面的方式将它们组织在一个
	vma中。2号页框存在于1号页框和3号页框之间，3号页框存在于
	2号页框和10号页框之间；
3）	使用'Query status' IOCTL来查询请求的状态；使用这个ioctl
	你将会得到下一步所需的所有信息，对于每一个物理页框，下面
	的这些信息将会返回：
	* 所请求的PFN；
	* PFN的状态：已获取（准备使用），未被获取（模块不能获取
	  这个页框）；
	* 这就意味着这个页已经被某个部分获取了，在页缓存或者其他
	  部分是否是空闲的；
	* VMA中页框的第一个和最后一个页框的地址（虚拟的用户地址）,
	  例如：

	     Example (requesting 1,2,3,10):
 
     pfn  |  virtual start..end | (more)
     -----+---------------------+-------
      1   |     0.. 4095   <-- Each line is a Pagestatus
      2   |  4096.. 8191
      3   |  8192..12287
     10   | 12288..16383
	  
	  当模块不能获取一个页的时候，这个页就不会被包含在VMA里面，
	  例如：

    Example (requesting 1,2,3,10 & the module could not get pfn 3):
 
     pfn  |  virtual start..end
     -----+--------------------
      1   |     0.. 4095
      2   |  4096.. 8191
     10   |  8192..12287
	
4）	mmap这个区域，这个区域的大小是从0开始，到最后一个页框的结
	束地址。在上面的例子中，区域的大小是0..16383 / 0..12287。
	O_DIRECT和O_SYNC标记在这里使用。

每一次open都创建一个新的session，下面是一个session的状态表：

                               .--- [Request] ----.
                              \/                 |
         .-[Request] --> CONFIGURING             |
         |                    .---------------.  |   .-[ Query status] -.
         .------------.                       \/ |   \/                 |
                      |                      CONFIGURED >---------------.
                      |                         /\  |
  CLOSED --[open]--> OPEN                       |   |
                                                |  [mmap]
                                            [unmap] |
                                                |   |   .-------------------.
                                                |   \/  \/                  |
                                                MAPPED >---[ Query status ]-.

file_operations.c 代码分析
==========================
对于这个文件，我们首先应该关注的结构体是file_operations声明的
字符设备的操作，如下：

struct file_operations phys_mem_fops = {
    .owner = THIS_MODULE,
    .llseek = dispatch_llseek,
    .read = dispatch_read,
    .unlocked_ioctl = dispatch_ioctl,
    .open = phys_mem_open,
    .release = phys_mem_release,
    .mmap = dispatch_mmap,
};

首先来看open函数，open函数为phys_mem_open，其主要作用是创建一个
session，并将其初始化，将该session与filp相关联，核心代码如下：

    dev = container_of(inode->i_cdev, struct phys_mem_dev, cdev);

	/* 从slab分配器中分配一个session */
    session = kmem_cache_alloc(session_mem_cache, GFP_KERNEL);
	...	
	/* 初始化session */
    session->session_id = atomic64_add_return(1, &session_counter);
    session->device = dev;
    sema_init(&session->sem, 1);
    session->vmas = 0;
    session->num_frame_stati = 0;
    session->frame_stati = NULL;
    session->status.state = SESSION_STATE_INVALID;

    SET_STATE(session, SESSION_STATE_OPEN);

    /* 关联filp与session */
    filp->private_data = session;

release函数为phys_mem_release，它的主要作用是关闭设备文件，并
释放相关资源。release函数的实现使用了一个循环，每次循环都让状
态机（前面所示）回退一个状态，直到状态机的状态变为SESSION_STATE_CLOSED
为止。核心代码如下：

	...
    while (GET_STATE(session) != SESSION_STATE_CLOSED) {
        switch (GET_STATE(session)) {

            case SESSION_STATE_OPEN:
                session->device = NULL;
                session->vmas = 0;

                SET_STATE(session, SESSION_STATE_CLOSED);
                break;

            case SESSION_STATE_CONFIGURING:
                // Bad: We need to wait until
                //      we have a valid state again
                //      In theory this could go on for ever.
				...
                up(&session->sem);
                return -ERESTARTSYS;
                break;

            case SESSION_STATE_CONFIGURED:
                // Free all claimed pages
                free_page_stati(session);
                SET_STATE(session, SESSION_STATE_OPEN);
                break;

			/* 因为已获取的页已被映射，不能释放它们 */
            case SESSION_STATE_MAPPED: break;
			/* 感觉下面的代码怎么都不会执行，因为switch已经涵盖了
			 * 所有的状态。
			 */
                // Hope that all mappings have been taken care of ...
                if (session->vmas)
                    printk(KERN_NOTICE "Session %llu: Releasing device while it still holds %d mappings (SESSION_STATE_MAPPED)\n ", session->session_id, session->vmas);

                SET_STATE(session, SESSION_STATE_CONFIGURED);
                break;
        }
    }

