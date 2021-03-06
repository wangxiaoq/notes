###KSM

1.   KSM全称kernel samepage merging，又叫kernel shared memory。
     它是一种通过消除重复来节省内存的方式，通过CONFIG_KSM=y使能。
     
     KSM的守护线程ksmd周期性的扫描用注册了KSM的用户内存区，查找
     那些内容相同的页，这些内容相同的页可以用一个写保护的页来替
     代，在之后某个进程需要修改这个页的内容时，再给它分配新页，
     并复制这个页的内容，和COW技术相似。

     KSM最早使用在KVM中，正是在KVM中KSM被称作kernel shared memory。
     通过多个虚拟机共享相同的数据，来让多个虚拟机使用内存。当然
     这种技术也可以用于任何一个应用程序产生多个相同的数据实例的
     情形。
 
     KSM只合并匿名页，不合并页高速缓存中的页。最初，KSM页被锁定
     到内核内存，现在KSM页也可以被换出到SWAP，但是在换出时这种
     共享关系就被打破了，当该页被再次换入到内存时，ksmd需要重新
     识别这些页并合并。

     KSM只在那些应用程序建议的地址空间的某些区域运行，应用程序
     通过系统调用：
     int madvise(addr, length, MADV_MERGEABLE)
     来建议。

     应用程序也可以通过
     int madvise(addr, length, MADV_UNMERGEABLE)
     来取消建议并恢复到之前非共享的状态：于是KSM就将之前合并的
     页通通变为非共享的。这可能会导致忽然间需要大量的内存，而造
     成该函数返回EAGAIN或者被OOM杀死。

     在没有配置CONFIG_KSM的内核中，madvise MADV_MERGEABLE和
     MADV_UNMERGEABLE都将返回EINVAL。在支持的内核中，madvise无
     论哪种情况都将成功：尽管此时可能ksmd并没有在运行，但是在之
     后ksmd启动时就会将指定的区域注册为MADV_MERGEABLE；尽管指定
     的区域中并不能不合并；尽管MADV_UNMERGEABLE的某个区域从来没
     有调用过MADV_MERGEABLE。

     像其他的madvise系统调用一样，它们的目的都是为了在用户地址
     空间的映射区使用：当然它会返回ENOMEM，当这个内存区包含一个
     未映射的区间时。[?]

     应用程序使用madvise MADV_UNMERGEABLE需要深思熟虑，因为KSM
     扫描会花费大量的CPU时间。

     ksmd通过用户空间接口/sys/kernel/mm/ksm/进行控制：

     * pages_to_scan - 在ksmd睡眠之前扫描多少个页，默认值是100；

     * sleep_millisecs - 在ksmd再次开始扫描之前应该睡眠多少毫秒，
                         默认是20；

     * merge_across_nodes - 是否允许在多个NUMA节点之间合并页，这
                            需要考虑究竟是节省空间（内存）更重要
                            还是节省时间更重要。该文件的值被改变
                            之前应先将run置为2，在设置merge_across_nodes
                            完成后，再将run置为1；

     * run - 设置为0，表示停止运行ksmd，但是之前已经合并的页仍然
             保持；
             设置为1，表示运行ksmd；
             设置为2，表示停止ksmd，并且分离之前合并的所有页，但
             是之前注册的MADV_MERGEABLE区域仍然保留该标记，以便
             后面继续运行ksmd；

    KSM和MADV_MERGEABLE的效果也在/sys/kernel/mm/ksm/下显示：

    * pages_shared - 目前使用的共享页的数量；
   
    * pages_sharing - 还有其它的多少个地方使用了这个页，即节省了
                      多少页；

    * pages_unshared - 有多少个页是独立的，但是重复的被检测以进
                       行合并；

    * pages_volatile - 有多少页变化太快了，这些页被组织在一个树
                       中；

    * full_scans - 扫描所有注册的区域需要多长时间；

    当pages_sharing和pages_shared比值较高时，说明共享的情况良好；
    当pages_unshared和pages_shared比值较高时，说明内存浪费严重；
    pages_volatile用来在很多活动中使用，当它的值较高时，也就暗
    示了使用madvise MADV_MERGEABLE不当；[1]
    
    
    内核花费了很多的精力来处理共享内存，例如对于程序的代码段就
    是共享的。另外对于可写入的页也可以是共享的，例如当一个进程
    调用fork函数后，该进程所使用的内存就变为和子进程共享的，使 
    用COW技术来处理之后对共享内存的修改，这就节省了内存的使用。

    在fork时使用COW技术是因为内核知道每一个进程都会去那些页中寻
    找相同的内容，如果缺少了这些知识的话，内核就无法将内容相同
    的内存组织为共享的。在KVM的开发过程中，就遇到了很多这样的问
    题。例如：
    
    在传统的gnome桌面系统中，有150位用户同时使用firefox在读lwn
    上面的文章，我们就可以共享firefox中之前已经存储的页来节省内
    存。[2]

    在KSM新的实现中，使用了两棵红黑树：非稳定红黑树和稳定红黑树。
    非稳定红黑树用来查找内容相同的页，通过使用memcmp将扫描过的页
    插入到该树中；稳定的红黑树用来存放已经被合并的内容相同的页。[3]

    在物理内存为16GB的主机上可以运行50个内存为1GB的Win XP虚拟机。[4]

    
*ref*

*1. https://www.kernel.org/doc/Documentation/vm/ksm.txt*

*2. https://lwn.net/Articles/306704/*

*3. https://lwn.net/Articles/330589/*

*4. https://en.wikipedia.org/wiki/Kernel_same-page_merging*
