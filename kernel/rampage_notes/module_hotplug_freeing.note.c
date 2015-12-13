module/page_claiming/hotplug_freeing.c
======================================
1. hotplug_bounce函数的作用就是离线一块内存，然后再使其上线，如下：
ret = remove_memory(address, OFFLINE_AT_ONCE << PAGE_SHIFT);
  if (!ret) {
    ret = online_pages(address >> PAGE_SHIFT, OFFLINE_AT_ONCE);
......

2. 函数free_pages_via_hotplug的作用实际上就是释放系统中用作缓存的内存，
使其变为空闲的，进而可以由内存检测模块获取，进行内存检测，核心代码如下：
    if (realaddress != last_addr) {
      last_addr = realaddress;
      if (!hotplug_bounce(realaddress)) {
        if (allowed_sources & SOURCE_SHAKING) {
          /* Failed, shake page and try again */
          shake_page(requested_page, 1);
          hotplug_bounce(realaddress);
          // ignore return value, we only retry once
        }
      }
    }
可以看到，这里就是离线在线的一个过程，经过这个过程，相应的内存可变为空闲
的，这样就可以由后面的内存页框获取函数来获取相应的页框。
