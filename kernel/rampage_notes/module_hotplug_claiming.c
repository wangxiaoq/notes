hotplug_claiming.c
==================
该文件主要包括了两个函数try_claim_pages_via_hotplug和
unclaim_pages_via_hotplug。这两个函数的作用是通过hotplug
机制来获取内存和释放内存。

首先看try_claim_pages_via_hotplug函数：

