mmap_phys.c
===========
1.	根据file_operation.c中的file_operations数组fops_by_session_state
	找到mmap函数为：

	int file_mmap_configured(struct file * filp, struct vm_area_struct * vma){

	首先获取session：
	struct phys_mem_session* session = (struct phys_mem_session*) filp->private_data;

	然后做一些状态检查，这里就不再赘述。但是下面的代码有点让人迷惑。一个session中
	status结构体所占的内存大小和要映射页的数量有什么关系呢？
	max_size = ROUND_UP_TO_PAGE(SESSION_FRAME_STATI_SIZE(session->num_frame_stati));
	max_size <<= PAGE_SHIFT;

	if ( vma->vm_end - vma->vm_start > max_size){
		ret = -EINVAL;
		printk(KERN_NOTICE "Mmap too large:  %lx > %lx", vma->vm_end - vma->vm_start, max_size );
		goto err;
	}
	
	调用assemble_vma函数：
	ret = assemble_vma(session, vma);

	下面就来分析assemble_vma函数：
	int assemble_vma (struct phys_mem_session* session, struct vm_area_struct * vma){

	开始逐个遍历status：
	for (request_iterator = 0; request_iterator < session->num_frame_stati; request_iterator++){
		struct phys_mem_frame_status* frame_status = &session->frame_stati[request_iterator];

	下面的pgprot_noncached作用是什么？作用就是设置nochche标志（即不使用cache）：
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	将该页映射到用户地址空间：
	insert_status = remap_pfn_range(vma,
									vma->vm_start + frame_status->vma_offset_of_first_byte,
									page_to_pfn(frame_status->page),
									PAGE_SIZE,
									vma->vm_page_prot);

	接下来是错误检查，就可以忽略掉了。
	因此整个循环结束后，就把所有的页都映射到用户空间了。接下来回到mmap函数：
	
	设置虚拟内存区的方法：
	vma->vm_ops = &phys_mem_vm_ops;

	查找phys_mem_vm_ops的定义：
	struct vm_operations_struct phys_mem_vm_ops = {
    .open =     phys_mem_vma_open,
    .close =    phys_mem_vma_close,
	};  
	open函数在每次该虚拟内存区添加到进程地址空间识调用，而close函数在每次该
	虚拟内存区从进程的地址空间删除时调用。这两个函数只做一些追踪工作。
	
	设置虚拟内存区不能被换出，并且标记为内存映射的IO区：
	vma->vm_flags |= VM_RESERVED;
	vma->vm_flags |= VM_IO;

	将session存储到vm_private_data：
	vma->vm_private_data = session;
	
	既然在将vma插入到用户空间时会调用phys_mem_vma_open，为什么又要在这里手动
	调用：
	phys_mem_vma_open(vma);
