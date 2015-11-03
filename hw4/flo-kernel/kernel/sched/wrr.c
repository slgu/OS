#include "sched.h"


static void
enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	/* add new task the tail */
	struct sched_wrr_entity * wrr_se = &p.wrr;
	if (flags & ENQUEUE_WAKEUP)
		wrr_se->timeout = 0;
	__enqueue_entity_wrr(wrr_se, rq, 0);
}

static void 
__enqueue_entity_wrr(struct sched_wrr_entity *wrr_se, struct rq *rq, int head)
{
	/* add entity to queue */
	if (head)
		list_add(&wrr_se.run_list, &rq->wrr_rq.wrr_entity_list);
	else
		list_add_tail(&wrr_se.run_list, &rq->wrr_rq.wrr_entity_list);

}
static void
dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	/* delete a item from the wrr_rq */
	__dequeue_entity_wrr(&p->wrr, rq);
}

static void
__dequeue_entity_wrr(struct sched_wrr_entity *wrr_se, struct rq *rq)
{
	list_del(&wrr_se.run_list, &rq->wrr_rq.wrr_entity_list);
}



static void put_prev_task_wrr(struct rq *rq, struct task_struct *prev)
{
	/* just enqueue */
	enqueue_task_wrr(rq, prev, 0); 
}

static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued)
{
	struct sched_wrr_entity * wrr_se = &p->wrr;
	if (p->policy != SCHED_WRR)
		return;
	if (--p->wrr.time_slice)
		return;
	/* need to change */
	p->wrr.time_slice = 100;
	/* move to the tail */
	list_move_tail(&p->pushable_wrr_tasks, &rq->wrr.pushable_tasks_list);
	/* reschedule */
	resched_curr(rq);
}

static void set_curr_task_wrr(struct rq *rq)
{
}
static void update_curr_wrr(struct rq * rq)
{
	struct task_struct *curr = rq->curr;
	u64 delta_exec;
	if (curr->sched_class != &wrr_sched_class)
		return;
	delta_exec = rq_clock_task(rq) - curr->se.exec_start;
	if (unlikely((s64)delta_exec <= 0))
		return;
	schedstat_set(curr->se.statistics.exec_max,
			max(curr->se.statistics.exec_max, delta_exec));
	curr->se.sum_exec_runtime += delta_exec;
	account_group_exec_runtime(curr, delta_exec);
	curr->se.exec_start = rq_clock_task(rq);
	cpuacct_charge(curr, delta_exec);
}
static void switched_to_wrr(struct rq *rq, struct task_struct *p)
{
	return;
}

static void
prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio)
{
	return;
}

static unsigned int get_rr_interval_idle(struct rq *rq, struct task_struct *task)
{
	return 0;
}

static check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	/* if preempt is needed */
	return;
}
static struct task_struct *
pick_next_task_wrr(struct rq *rq, struct task_struct *prev)
{
	/* set prev task to end of sche class queue */
	put_prev_task(rq, prev);
	if (list_empty(&rq->wrr_rq.pushable_tasks_list)) {
		return NULL;
	}
	struct list_head * real_head = &rq->wrr_rq.pushable_tasks_list.next
	struct task_struct * p = list_entry(real_head, struct task_struct *, pushable_wrr_tasks);
	p->se.exec_start = rq_clock_task(rq);
	return p;
}
static int
select_task_rq_rt(struct task_struct *p, int cpu, int sd_flag, int flags)
{

}
const struct sched_class wrr_sched_class = {
	.next			= &idle_sched_class, 
	.enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr,

	.check_preempt_curr	= check_preempt_curr_wrr,

	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task		= put_prev_task_wrr,

#ifdef CONFIG_SMP
	/* choose which run queue (which cpu) a waking-up task to be queued */
	.select_task_rq		= select_task_rq_wrr,
#endif

	/* set a task running move from the queue */
	.set_curr_task          = set_curr_task_wrr,
	.task_tick		= task_tick_wrr,

	.get_rr_interval	= get_rr_interval_wrr,

	.prio_changed		= prio_changed_wrr,
	.switched_to		= switched_to_wrr,
	.update_curr		= update_curr_wrr,
};
