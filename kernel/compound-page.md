### Compound Page


     复合页（Compound Page）就是将物理上连续的两个或多个页看成一个
     独立的大页，它可以用来创建hugetlbfs中使用的大页（hugepage），
     也可以用来创建透明大页（transparent huge page）子系统。但是
     它不能用在页缓存（page cache）中，这是因为页缓存中管理的都是
     单个页。

     分配一个复合页的方式是：使用alloc_pages函数，参数order至少为1，
     且设置__GFP_COMP标记。因为根据复合页的定义，它通常包括2个或多
     个连续的物理内存页，这是由它的实现决定的，因而order参数不可能
     为0。

     通常调用alloc_pages的内存分配方式如下：

     p = alloc_pages(GFP_KERNEL, 2);

     但是这种方式和创建一个复合页有什么不同呢？不同点就是在创建复合
     页的时候会创建与这个复合页相关的元数据（metadata）。

     表示复合页的元数据都存在于Page结构体中，Page页中的flag标记用来
     识别复合页。在复合页中，打头的第一个普通页成为“head page”，用
     PG_head标记，而后面的所有页被称为“tail pages”，用PG_tail标记。
     在64位系统中，可以有多余的标记来表示复合页的页头和页尾；但是在
     32位系统中却没有那么多的标记，因此采用了一种复用其他标记的方案，
     即将复合页中的所有页都用PG_compound标记，然后，对于尾页同时也
     使用PG_reclaim标记，这是因为PG_reclaim只有在页缓存中会用到，而
     复合页根本就不会在页缓存中使用。

     可以使用PageCompound函数来检测一个页是否是复合页，另外函数PageHead
     和函数PageTail用来检测一个页是否是页头或者页尾。在每个尾页的page
     结构体中都包含一个指向头页的指针 - first_page，可以使用compound_head
     函数获得。

     那么当一个复合页不再被系统使用时，我们如何知道该复合页包含多少
     个普通页，又如何知道该复合页的析构函数（destructor）存在哪里呢？
     首先，人们可能会认为这些信息存在于头页的page结构体中，但是很不
     幸，在这个结构体中已经没有可用的空间了。因此，这些信息全部存储
     在第一个尾页的lru字段中，将该复合页的大小（order）首先强制转换
     为指针类型，然后存储在lru.prev中，将析构函数存储在lru.next中。
     这里就解释了为什么复合页必须至少是两个页。

     在内核中生命了两个复合页的析构函数，默认情况下会调用free_compound_page
     来将所有的页返回给系统的页框分配器，而hugetlbfs子系统会调用free_huge_page     
     来做一些统计并释放。

     使用复合页的最经典的一个例子就是THP（transparent huge page），
     另外一些驱动使用复合页来方便缓存的管理。

*ref*

*1. https://lwn.net/Articles/619514/*

问题

1. page cache?
2. hugetlbfs?
3. transparent huge page?
