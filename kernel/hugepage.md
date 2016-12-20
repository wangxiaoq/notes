### hugepage part 1 - Introduction

本文是对LWN上5篇关于hugepage文章所做的笔记。

CPU使用的都是虚拟地址，这些虚拟地址需要被内存管理单元（MMU）转换为
物理地址才能输出到总线上。MMU使用一个称为页表的表格进行这个转换过程，
由于页表存在于内存中，因此一次真正的内存操作往往伴随着额外的访存操作，
例如在32位机器上，一次内存访问往往伴随着一次页目录访问和一次页表访问，
为了加快这个过程，引入了TLB（Translation Lookaside Buffer）。TLB可以
将经过转换的虚拟地址和物理地址进行记录，当下次访问到之前被转换过的虚
拟地址时，就可以直接使用TLB中记录的结果，不需要再次进行转换，因为TLB
是硬件高速缓存，速度要比访问内存快的多。然而由于TLB的容量是有限的，
总会有些虚拟地址没有存在于TLB中，需要使用页表进行转换，这种情况被称为
TLB miss。TLB miss会影响计算机系统的性能，为了减少TLB miss的影响，提
出了hugepage tlbfs。

TLB reach - 通过TLB所能访问的内存总量，可以使用下面的公式进行
计算：

*TLB reach = Page Table Entry Number \* Page Size*

提升TLB reach可以降低TLB miss发生的频率，因此可以提升TLB中页表项的数
目或者是页的大小，根据实际情况，提升页的大小是可行的，因此将多个普通
页组合到一起，构成hugepage。

基于下面两个原因，使用hugepage可以提升系统的性能：

1. 使用hugepage提升了TLB reach，减少了TLB miss的频率；
2. 地址转换信息通常会存放在L2缓存中，使用hugepage减少了存放在L2中的
转换信息，更多的缓存可以被代码和数据使用，间接的提升了系统的性能；

#### hugepage与page fault

最初使用hugepage时，在mmap函数返回之前就会触发page fault，当mmap返回
之后，hugepage已经被分配和映射好了。对于共享映射，不存在什么问题。
而对于私有映射，如果父进程调用fork，最好尽快执行exec或者将hugepage这
块内存标记为MADV_DONTFORK，否则子进程对hugepage的一个COW操作将会造成
父子进程都被杀死。

提前对hugepage触发page fault会造成系统的性能下降，为了解决这个问题，
之后的一个内核版本将page fault推迟到需要访问huge page时，才建立映射
和物理地址之间的关系。Linux内核在共享映射被创建的时候就为这个映射保
留了huge page，因此当访问到共享映射触发page fault时，Linux内核可以
进行处理。但是对于私有映射，Linux内核并没有保留任何huge page，也就是
说进程可能在没有fork，就会被杀死。libhugetlbfs使用readv保证只有这个
映射在安全的时候再被访问，但是这并不是一个完美的解决方案。

在后面的内核中，不论对于共享映射还是私有映射都保留了huge page，对于共
享映射，一定会保证这个映射会成功。对于私有映射，只为创建映射的进程保
留了huge page，因为子进程的数目是不确定的。如果创建映射的进程执行了
fork，这时就有两个进程会对映射进行访问，如果子进程执行了COW，内核会
尝试给子进程分配huge page，如果分配失败，子进程就会被终止，并且会在
内核中打印huge page不足的信息。如果父进程执行COW，那么会尝试给父进程
分配huge page，如果分配失败，就将子进程对huge page的映射进行解除，父
进程现在可以独占huge page，如果后来子进程再次访问对huge page的映射时，
就会被终止。

#### huge page与swap

huge page不会在swap中使用；

#### huge page与NUMA

在使用huge page的时候，要注意数据的分割，不要让一个节点的CPU去访问
另一个节点内存中的数据。OpenMP并不以页为单位对数据进行分割，因此当
OpenMP在NUMA系统中运行时，就有可能会造成一个节点的CPU去访问另一个
节点中的内存这种情况，而huge page加重了这种情况，因此在NUMA系统上
使用huge page时，一定要合理的分割数据。

### huge page part 2 - Interfaces
