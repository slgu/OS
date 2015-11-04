#include "sched.h"

#define FOREGROUND_TYPE 0
#define BACKGROUND_TYPE 1
#define FOREGROUND_WEIGHT 10
#define BACKGROUND_WEIGHT 1
static int get_task_group(struct task_struct * p)
{
	char group_path[30];
	cgroup_path(task_group(p)->css.cgroup, group_path, 30);
	if (strcmp(group_path, "/") == 0)
		return FOREGROUND_TYPE;
	else
		return BACKGROUND_TYPE;
}

static void
enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	/* add new task the tail */
	struct sched_wrr_entity * wrr_se = &p.wrr;
	int task_type = get_task_group(p);
	if (flags & ENQUEUE_WAKEUP)
		wrr_se->timeout = 0;
	__enqueue_entity_wrr(wrr_se, rq, 0);
	
	/* add weight to statistics */
	if (task_type == FOREGROUND_TYPE)
		rq->nr_running = rq->nr_running + FOREGROUND_WEIGHT;
	else
		rq->nr_running = rq->nr_running + BACKGROUND_WEIGHT;
		
	/* increase load */
}

static void 
__enqueue_entity_wrr(struct sched_wrr_entity *wrr_se, struct rq *rq, int head)
{
	/* add entity to queue */
	if (head)
		list_add(&wrr_se.run_list, &rq->wrr.wrr_entity_list);
	else
		list_add_tail(&wrr_se.run_list, &rq->wrr.wrr_entity_list);

}

static void
dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	/* delete a item from the wrr_rq */
	__dequeue_entity_wrr(&p->wrr, rq);
	int task_type = get_task_group(p);
	if (task_type == FOREGROUND_TYPE)
		rq->nr_running = rq->nr_running - FOREGROUND_WEIGHT;
	else
		rq->nr_running = rq->nr_running - BACKGROUND_WEIGHT;
}

static void
__dequeue_entity_wrr(struct sched_wrr_entity *wrr_se, struct rq *rq)
{
	list_del(&wrr_se.run_list, &rq->wrr.wrr_entity_list);
}



static void put_prev_task_wrr(struct rq *rq, struct task_struct *prev)
{
	/* just enqueue */
	enqueue_task_wrr(rq, prev, 0); 
}

static void watchdog(struct rq *rq, struct task_struct *p)
{

}
static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued)
{
	struct sched_wrr_entity * wrr_se = &p->wrr;
	update_curr_wrr(rq);
	int task_type = get_task_group(p);
	if (p->policy != SCHED_WRR)
		return;
	if (--p->wrr.time_slice)
		return;
	if (task_type == FOREGROUND_TYPE)
		p->wrr.time_slice = 100 * FOREGROUND_WEIGHT;
	else
		p->wrr.time_slice = 100 * BACKGROUND_WEIGHT;
	/* move to the tail */
	list_move_tail(&p->wrr_se.run_list, &rq->wrr.wrr_entity_list);
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
	if (list_empty(&rq->wrr.wrr_entity_list)) {
		return NULL;
	}
	struct sched_wrr_entity * entity = list_first_entry(&rq->wrr.wrr_entity_list, struct sched_wrr_entity *, run_list); 
	struct task_struct * p = list_entry(entity, struct task_struct *, wrr_se);
	p->se.exec_start = rq_clock_task(rq);
	/* remove from wrr queue */
	dequeue_task_wrr(rq, p, 0);
	return p;
}

static int find_lowest_cpu(void)
{
	struct rq * rq;
	int start_cpu = 0;	
	int target = -1;
	int lowest = -1;
	/* traverse cpu */
	int number_cpus = num_online_cpus();
	for (; start_cpu < number_cpus; ++start_cpu) {
		rq = cpu_rq(start_cpu);
		if (lowest == -1 || lowest > rq->nr_running) {
			lowest = rq->nr_running;
			target = start_cpu;
		}
	}
	return target;
}

static int
select_task_rq_wrr(struct task_struct *p, int cpu, int sd_flag, int flags)
{
	struct task_struct *curr;
	struct rq *rq;
	int target;
	/* For anything but wake ups, just return the task_cpu */
	if (sd_flag != SD_BALANCE_WAKE && sd_flag != SD_BALANCE_FORK)
		return cpu;
	/* lock needed */
	rcu_read_lock();
	int target = find_lowest_cpu();	
	rcu_read_unlock();
	if (target == -1)
		return cpu;
	else
		return target;
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
