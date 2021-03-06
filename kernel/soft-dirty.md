###soft-dirty ptes

	soft-dirty是PTE中用来跟踪一个进程修改了哪些页面的一个位。
为了达到这个目的，	应该满足下面条件：

	1. 清除一个task所有PTE的soft-dirty标记；
	   这是通过写“4”到特定进程的/pro/PID/clear_refs文件实现的；
	2. 等待一段时间；
	3. 从PTEs中读取soft-dirty位的值；
	   这是通过读取/proc/PID/pagemap的值实现的；在pagemap文件中，
	   每一个表项的第55位表示soft-dirty；如果被设置的话，相应的
	   PTE（这里应该是对应的PTE对应的页）从第1步开始，被写入了；
	
	为了达到跟踪的目的，当soft-dirty位被清零时，相应的PTE中的可写（writable）位被清零；所以在这之后，当进程尝试修改某个虚拟地址处的页时，会触发一个#PF，内核就把相应的PTE中的soft-dirty位设置为1。

	注意：尽管在soft-dirty位被清除以后，进程的地址空间都被标记为r/o，在这之后的#PF-s都被快速处理。由于页仍然是映射到物理内存的，因此内核所需要做的所有工作就是搞清楚这个事实，然后把PTE的soft-dirty位和writable位标记为1。

	尽管在大多数情况下，通过#PF-s来跟踪内存变化是足够的，但是仍存在一种情况可能会丢掉soft-dirty位。一个人物unmap了一个之前映射的内存区域，然后又重新映射了同样物理内存。当unmap被调用时，内核会清除PTE的值，包括soft-dirty。为了提示用户空间应用程序这个内存区域的更新，内核总是把新的内存区域（包括扩展区域）标记为soft-dirty。

	这个特征被check-restore项目使用。

####问题
1. 既然是通过将soft-dirty和writable标记都清零，实现每次写操作时，
   都会触发#PF来实现将soft-dirty位置位，那么在writable标记被清零
   后如何判定该页是否允许写操作?
