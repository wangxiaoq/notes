hotplug_claiming.c
==================
该文件主要包括了两个函数try_claim_pages_via_hotplug和
unclaim_pages_via_hotplug。这两个函数的作用是通过hotplug
机制来获取内存和释放内存。

首先看try_claim_pages_via_hotplug函数：

if (allowed_sources & SOURCE_HOTPLUG_CLAIM) {
如果可以通过HOTPLUG来获取内存页的话，就尝试hotplug来获取，
否则的话，就返回CLAIMED_TRY_NEXT尝试下一种内存分配方式。
return CLAIMED_TRY_NEXT;

if (start_address != -1) {
	if (aligned_address == start_address) {
		/* in currently removed memory block */
		*actual_source = SOURCE_HOTPLUG_CLAIM;
		return CLAIMED_SUCCESSFULLY;
	} else {
	/* requested page not in current hotplug area */
		return CLAIMED_TRY_NEXT;
	}
}
start_address的作用就是用来标识最近一次分配的内存的起始地址，
如果是在多核系统上，发现这个start_address一旦不为-1，就说明
可能有另一个处理器这个在执行分配以start_address起始的内存地
址。早但和系统上，说明这块内存最近已被分配（当然，多核系统上
也可能是这样）。
如果aligned_address不等于start_address，说明最近一次分配的内
存不存在于start_address起始的内存中，那么我们就直接尝试下一种
方法。难道每次只能有一块内存是通过hotplug获取的吗？

/* Don't try to hotplug the same area twice if it failed the last time */
if (prev_failed_claim == aligned_address) {
	return CLAIMED_TRY_NEXT;
}
如果是之前已经尝试失败的地址的话，直接尝试下一种方式。

// test: refuse to claim the first block of memory
if (aligned_address == 0)
	return CLAIMED_TRY_NEXT;
如果是从零开始的物理内存块，直接尝试下一种方式。

ret = remove_memory(aligned_address, OFFLINE_AT_ONCE << PAGE_SHIFT);
在3.5的内核中，remove_memory相当于是offline一块物理内存。

    if (ret) {
      if (allowed_sources & SOURCE_SHAKING) {
        /* Failed, shake page and try again */
        shake_page(requested_page, 1);
        ret = remove_memory(aligned_address, OFFLINE_AT_ONCE << PAGE_SHIFT);
        if (ret) {
          prev_failed_claim = aligned_address;
          return CLAIMED_TRY_NEXT;
        }
      } else {
        /* Failed, shaking not enabled */
        prev_failed_claim = aligned_address;
        return CLAIMED_TRY_NEXT;
      }
    }
如果返回错误的话，检查是否允许shake_page，如果允许的话，就
再尝试一次，否则将prev_failed_claim设置为发生错误的物理地址。

    start_address = aligned_address;
    prev_start_address = -1;
    *actual_source = SOURCE_HOTPLUG_CLAIM;
    return CLAIMED_SUCCESSFULLY;
分配成功，将start_address设置为aligned_address。

再来看unclaim_pages_via_hotplug，
  /* Sanity check */
  if (aligned_address != start_address &&
      aligned_address != prev_start_address) {
    printk(KERN_CRIT "physmem: Attempted to unclaim_pages_via_hotplug on nonclaimed area!\n");
    return;
  }
检查如果要回收的内存是不是之前通过hotplug所分配出去的。

  if (start_address != -1) {
    ret = online_pages(aligned_address >> PAGE_SHIFT, OFFLINE_AT_ONCE);
    if (ret) {
      printk(KERN_CRIT "physmem: unclaim failed for pfn %08llx: ret %d\n",
             aligned_address >> PAGE_SHIFT, ret);
    }

    prev_start_address = start_address;
    start_address = -1;
  } else {
    /* unclaim in already-onlined block, do nothing */
  }
如果start_address不为-1，说明当前这块内存还没有被回收，进行
回收工作，回收工作会把prev_start_address设置为start_address，
而将start_address设置为-1，表示内存已经被回收。
