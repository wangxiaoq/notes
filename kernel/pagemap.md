###pagemap

1.pagemap通过/proc文件系统相应用程序提供页表以及相关的信息。
     它共有三个部分组成：
     
     * /proc/pid/pagemap - 它包含了进程的每一个虚拟页映射到的
       物理页框信息，对于每一个虚拟页都由一个64位的值与其相对
       应：
       
       * 0-54： PFN
       * 0-4 ： 如果被swap的话，表示swap的类型
       * 5-54： 如果被swap的话，表示swap的偏移
       * 55  ： PTE是soft-dirty
	   * 56  ： 页被互斥的映射（since 4.2）
       * 57-60： 0
       * 61  ： 页是文件页（file-page）或共享匿名页（shared-anon）（since 3.5）
       * 62  ： 页被交换出去
       * 63  ： 页存在

       当页在swap中时，PFN表示swap的文件号以及页在swap中的偏移。
       如果页没有被映射的话，PFN的值为空。因此就可以精确的确定
       页到底是在内存中还是在swap中，这样就可以比较不同进程之间
       页的映射。

       对该接口更有效的使用方式是先到/proc/pid/maps中查询究竟哪
       一个内存区被映射了，然后使用llseek直接跳过pagemap中未映
       射的内存区。

     * /proc/kpagecount - 一个64位计数器，表示每个物理页框被映
       射的次数。

     * /proc/kpageflags - 每个物理页的标记的集合，也是64位，每
       个物理页框都有一个。

       这些标记包括：

       0. LOCKED
       1. ERROR
       2. REFERENCED
       3. UPTODATE
       4. DIRTY
       5. LRU
       6. ACTIVE
       7. SLAB
       8. WRITEBACK
       9. RECLAIM
      10. BUDDY
      11. MMAP
      12. ANON
      13. SWAPCACHE
      14. SWAPBACKED
      15. COMPOUND_HEAD
      16. COMPOUND_TAIL
      16. HUGE
      18. UNEVICTABLE
      19. HWPOISON
      20. NOPAGE
      21. KSM
      22. THP
      23. BALLOON
      24. ZERO_PAGE

      下面简单的描述以下这些标记的作用：

      0. LOCKED： 当前页正被锁定以进行互斥访问，例如进行读写
         等IO操作。

      7. SLAB： 当前页由SLAB/SLOB/SLUB/SLQB内核分配器管理，
         如果是复合页的话，SLUB/SLQB只会将头页中SLAB标记设置，
         而SLOB完全不设置。

      10. BUDDY： 由伙伴系统分配器管理的空闲内存内存块，需要
          注意的是该标记仅在内存块的第一个页中设置。

      15. COMPOUND_HEAD
      16. COMPOUND_TAIL： 复合页中有2^N个页，当N=2时，页的组
          织为：HTTT，H表示头页，T表示尾页。复合页的使用场景
          包括：hugetlbfs，SLUB，内存分配器以及设备驱动程序。
          然而这种情况下，只有大到G级别的复合页才能够被终端用
          户看到。

      17. HUGE： 这是一个hugepage的一部分。

      19. HWPOISON： 硬件在这个页上检测到了错误。

      20. NOPAGE： 所请求的地址处没有页框。

      21. KSM： 一个或多个进程之间动态共享的页。

      22. THP： 构成THP的连续内存页。

      23. BALLOON： 需要压缩的页。
 
      24. ZERO_PAGE： pfn_zero或者huge_zero的页。

      [IO 相关的页标记]

      1. ERROR： 遇到了一个IO错误。

      3. UPTODATE： 页中的数据已经是最新了。
 
      4. DIRTY： 页中有数据要写回到磁盘。

      8. WRITEBACK： 页中内容正在与磁盘进行同步。

      [LRU 相关的页标记]

      5. LRU： 页在一个LRU链表上。

      6. ACTIVE： 页在激活的LRU链表上。

      18. UNEVICTABLE： 页存在于无法驱逐的（non-）LRU链表上，
          它被某种方式钉住了，并且不是LRU页回收算法的候选者。

      2. REFERENCED： 自从上次入队之后，该页被引用过。

      9. RECLAIM： 页很快将被回收，在它的pageout IO完成以后。

      11. MMAP： 内存映射的页。

      12. ANON： 内存映射的页，但不是文件的一部分。

      13. SWAPCACHE： 页被映射到了交换空间，例如有对应的swap入口。

      14. SWAPBACKED： 页被swap或者RAM备份。
      
      存在于tools/vm/page-types工具可以用来查询上述的标记。

    使用pagemap可以做一些有用的事情，使用pagemap的查找一个进程使
    用内存典型步骤如下：

    1. 读取/proc/pid/maps文件，获取每一块内存内存空间被映射为什
       么？
    
    2. 选取你所感兴趣的映射 - 所有，或者某个库，或者栈和堆。

    3. 打开/proc/pid/pagemap找到你所感兴趣的页。

    4. 从pagemap中为感兴趣的每个页读取一个u64的数。

    5. 打开/proc/kpagecount和/proc/kpageflags从中读取你所感兴趣的
       物理页框的任何信息。

      


*ref*

*1. Documentation/pagemap.txt*
