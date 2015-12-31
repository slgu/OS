/*************************************************************************
    > File Name: kernel/sched/debug_wrr.c
    > Author: Gu Shenlong
    > Mail: blackhero98@gmail.com
    > Created Time: Sun 08 Nov 2015 09:57:19 PM EST
 ************************************************************************/

#include <linux/wrr.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include "sched.h"
int kernel_fg_weight = 10;
int kernel_bg_weight = 1;
SYSCALL_DEFINE1(get_wrr_info, struct wrr_info *, wrr_info_user)
{
	struct wrr_info buf_wrr_info;
	int cpu;
	struct rq *rq = 0;
	int total = 0;
	rcu_read_lock();
	for_each_online_cpu(cpu) {
		++total;
		rq = cpu_rq(cpu);
		buf_wrr_info.total_weight[cpu] = rq->wrr.wrr_nr_total_weight;
		buf_wrr_info.nr_running[cpu] = rq->nr_running;
	}
	rcu_read_unlock();
	if (copy_to_user(wrr_info_user,
		&buf_wrr_info, sizeof(struct wrr_info)) != 0)
		return -1;
	return total;
}
SYSCALL_DEFINE2(set_wrr_weights, int, fg_weight, int, bg_weight)
{
	kernel_bg_weight = bg_weight;
	kernel_fg_weight = fg_weight;
	return 0;
}
