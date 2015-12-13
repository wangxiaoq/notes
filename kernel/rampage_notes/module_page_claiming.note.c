module/page_claiming.c
======================

1. 在该文件中首先声明了一个函数数组，该数组中的每一个元素表示一种
获取页框的方法，如下：
try_claim_method try_claim_methods[] = {try_claim_pages_via_hotplug, free_pages_via_hotplug, try_claim_free_buddy_page, ignore_difficult_pages, NULL};

2. 接下来是标记发生错误页的函数：
int handle_mark_page_poison(struct phys_mem_session* session, const struct mark_page_poison* request) {

这个函数中最关键的代码是下面的这一行，将这个页标记为HWPoison：
SetPageHWPoison(pfn_to_page(request->bad_pfn));

3. 再来看处理页框请求的函数：
int handle_request_pages(struct phys_mem_session* session, const struct phys_mem_request* request)
首先是对状态的检查，如果状态处理有误的状态，这里不再赘述。

    if (GET_STATE(session) == SESSION_STATE_CONFIGURED)
        free_page_stati(session);
	SET_STATE(session, SESSION_STATE_CONFIGURING);
如果是处于CONFIGURED状态请求页框的话，就释放原来已经获取
的页框，并修改状态为CONFIGURING。

    if (request->num_requests == 0) {
        SET_STATE(session, SESSION_STATE_OPEN);
        goto out;
    }
如果请求的页框数为0，将状态设置为OPEN。

	session->frame_stati = SESSION_ALLOC_NUM_FRAME_STATI(request->num_requests);
给每一个请求分配一个元素。

    memset(session->frame_stati, 0, SESSION_FRAME_STATI_SIZE(request->num_requests));
    session->num_frame_stati = request->num_requests;
初始化session相关的内容。

        for (i = 0; i < request->num_requests; i++) {
            struct phys_mem_frame_request __user * current_pfn_request = &request->req[i];

            struct phys_mem_frame_status __kernel * current_pfn_status = &session->frame_stati[i];

            if (copy_from_user(&current_pfn_status->request, current_pfn_request, sizeof (struct phys_mem_frame_request))) 
			...
开始将物理页框请求逐个拷贝到内核空间。

            jiffies_start = get_jiffies_64();
            if (unlikely(!pfn_valid(current_pfn_status->request.requested_pfn))) {
                current_pfn_status->actual_source = SOURCE_INVALID_PFN;
检测当前的页框是否是有效的，并设置actual_source。

            } else {
                struct page* requested_page = pfn_to_page(current_pfn_status->request.requested_pfn);
                try_claim_method claim_method = NULL;
                int claim_method_idx = 0;
                int claim_method_result = CLAIMED_TRY_NEXT;

                if (unlikely(NULL == requested_page)) {
				......
                allocated_page = requested_page;
为请求分页做准备。

                while (CLAIMED_TRY_NEXT == claim_method_result) {
                    claim_method = try_claim_methods[claim_method_idx];

                    if (claim_method)
                        claim_method_result = claim_method(requested_page, current_pfn_status->request.allowed_sources, &allocated_page, &current_pfn_status->actual_source);
                    else
                        claim_method_result = CLAIMED_ABORT;

                    claim_method_idx++;
                }
依次尝试每一种页框分配方式，直到返回结果不等于CLAIMED_TRY_NEXT为止。

                if (CLAIMED_SUCCESSFULLY == claim_method_result) {

                    current_pfn_status->pfn = page_to_pfn(allocated_page);
                    current_pfn_status->page = allocated_page;
                    current_pfn_status->vma_offset_of_first_byte = current_offset_in_vma;
                    current_offset_in_vma += PAGE_SIZE;
如果分配成功的话，更新status中的相关信息。

                } else {
                    /* Nothing to do*/
                    current_pfn_status->page = NULL;
                    current_pfn_status->pfn = 0;
                    current_pfn_status->vma_offset_of_first_byte = 0;
失败的话，同样更新status中的相关信息，不过这些信息是来标记这个请求
没有成功。

            jiffies_end = get_jiffies_64();
            jiffies_used = jiffies_start < jiffies_end ? jiffies_end - jiffies_start : jiffies_start - jiffies_end;
            current_pfn_status->allocation_cost_jiffies = jiffies_used;
更新时间。
			SET_STATE(session, SESSION_STATE_CONFIGURED);
设置状态为CONFIGURED。
