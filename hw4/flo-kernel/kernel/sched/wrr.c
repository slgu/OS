#include "sched.h"
#define FOREGROUND_TYPE 0
#define BACKGROUND_TYPE 1
#define U64_MAX ((u64) (~0ULL))
#define PERIOD_WEIGHT (500 * HZ / 1000)
/* helper function */

static int move_one_task(int src_cpu, int des_cpu);
/* called in schduler_tick in core.c */
static int find_highest_cpu(void)
{
	u64 max_total_weight = 0;
	int cpu, max_cpu = 0;
	struct rq *rq;

	for_each_online_cpu(cpu) {
		rq = cpu_rq(cpu);
		if (max_total_weight < rq->wrr.wrr_nr_total_weight) {
			max_total_weight = rq->wrr.wrr_nr_total_weight;
			max_cpu = cpu;
		}
	}
	return max_cpu;
}

static int find_lowest_cpu(void)
{
	u64 min_total_weight = U64_MAX;
	int cpu, min_cpu = 0;
	struct rq *rq;

	for_each_online_cpu(cpu) {
		rq = cpu_rq(cpu);
		if (min_total_weight > rq->wrr.wrr_nr_total_weight) {
			min_total_weight = rq->wrr.wrr_nr_total_weight;
			min_cpu = cpu;
		}
	}
	return min_cpu;
}

void load_balance_wrr(int min_cpu, int max_cpu)
{
	struct rq *min_rq, *max_rq;
	unsigned long flags;

	min_rq = cpu_rq(min_cpu);
	max_rq = cpu_rq(max_cpu);
	if (max_rq->nr_running > 1) {
		local_irq_save(flags);
		double_rq_lock(min_rq, max_rq);
		move_one_task(max_cpu, min_cpu);
		double_rq_unlock(min_rq, max_rq);
		local_irq_restore(flags);
	}
}

void idle_balance_wrr(int this_cpu, struct rq *rq)
{
	int max_cpu;

	raw_spin_unlock(&rq->lock);
	rcu_read_lock();
	max_cpu = find_highest_cpu();
	rcu_read_unlock();
	load_balance_wrr(this_cpu, max_cpu);
	raw_spin_lock(&rq->lock);
}

void period_load_balance_wrr(struct rq *rq)
{
	int max_cpu;
	int min_cpu;
	int cpu = smp_processor_id();
	/* cpu 0 to do period load balance */
	if (cpu != 0)
		return;
	++rq->wrr.clock_tick;
	if (rq->wrr.clock_tick != PERIOD_WEIGHT)
		return;
	/* reset clock */
	rq->wrr.clock_tick = 0;
	rcu_read_lock();
	max_cpu = find_highest_cpu();
	min_cpu = find_lowest_cpu();
	rcu_read_unlock();
	if (max_cpu == min_cpu)
		return;
	/* do load balance */
	load_balance_wrr(min_cpu, max_cpu);
}

static int get_task_group(struct task_struct *p)
{
	char group_path[30];

	cgroup_path(task_group(p)->css.cgroup, group_path, 30);
	if (strcmp(group_path, "/") == 0)
		return FOREGROUND_TYPE;
	return BACKGROUND_TYPE;
}

static void
enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_wrr_entity *wrr_se = &p->wrr;
	int weight;

	weight = get_task_group(p);
	if (weight == FOREGROUND_TYPE)
		weight = kernel_fg_weight;
	else
		weight = kernel_bg_weight;
	rq->wrr.wrr_nr_total_weight += weight;
	wrr_se->time_slice = wrr_se->time_weight = RR_TIMESLICE * weight;
	if (flags & ENQUEUE_HEAD)
		list_add(&wrr_se->run_list, &(rq->wrr.wrr_entity_list));
	else
		list_add_tail(&wrr_se->run_list, &(rq->wrr.wrr_entity_list));
	inc_nr_running(rq);
}


static void
dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_wrr_entity *wrr_se = &p->wrr;

	rq->wrr.wrr_nr_total_weight -= p->wrr.time_weight / RR_TIMESLICE;
	list_del_init(&wrr_se->run_list);
	dec_nr_running(rq);
}



static void put_prev_task_wrr(struct rq *rq, struct task_struct *prev)
{
}


static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued)
{
	if (p->policy != SCHED_WRR)
		return;
	--p->wrr.time_slice;
	if (p->wrr.time_slice > 0)
		return;
	p->wrr.time_slice = p->wrr.time_weight;
	if (p->wrr.run_list.prev != p->wrr.run_list.next) {
		list_move_tail(&(p->wrr.run_list), &(rq->wrr.wrr_entity_list));
		set_tsk_need_resched(p);
	}
}

static void set_curr_task_wrr(struct rq *rq)
{
	struct task_struct *p = rq->curr;

	p->se.exec_start = rq->clock_task;
}

static void switched_to_wrr(struct rq *rq, struct task_struct *p)
{
}

static void
prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio)
{
}


static void check_preempt_curr_wrr(struct rq *rq,
	struct task_struct *p,
	int flags)
{
}

static struct task_struct *
pick_next_task_wrr(struct rq *rq)
{
	struct sched_wrr_entity *entity;
	struct task_struct *p;

	if (list_empty(&rq->wrr.wrr_entity_list))
		return NULL;
	entity = list_first_entry(&rq->wrr.wrr_entity_list,
			struct sched_wrr_entity, run_list);
	p = list_entry(entity, struct task_struct, wrr);
	p->se.exec_start = rq->clock_task;
	return p;
}

static int
select_task_rq_wrr(struct task_struct *p, int sd_flag, int flags)
{
	struct rq *rq;
	u64 min_total_weight = U64_MAX;
	int cpu, min_cpu;

	min_cpu = cpu = task_cpu(p);
	if ((sd_flag & SD_BALANCE_WAKE) | (sd_flag & SD_BALANCE_FORK)) {
		rcu_read_lock();
		for_each_cpu(cpu, &p->cpus_allowed) {
			rq = cpu_rq(cpu);
			if (min_total_weight > rq->wrr.wrr_nr_total_weight) {
				min_total_weight = rq->wrr.wrr_nr_total_weight;
				min_cpu = cpu;
			}
		}
		rcu_read_unlock();
	}
	return min_cpu;
}
static unsigned int get_rr_interval_wrr(struct rq *rq, struct task_struct *task)
{
	return RR_TIMESLICE;
}

static void
__move_task(struct task_struct *p, struct rq *src,
	    int dest_cpu, struct rq *dest)
{
	deactivate_task(src, p, 0);
	set_task_cpu(p, dest_cpu);
	activate_task(dest, p, 0);
	check_preempt_curr(dest, p, 0);
}

static int
can_migrate_task(struct task_struct *p, struct rq *src,
		 int dest_cpu, struct rq *dest)
{
	u64 p_weight = p->wrr.time_weight;

	if (task_running(src, p))
		return 0;

	if (!cpumask_test_cpu(dest_cpu, tsk_cpus_allowed(p)))
		return 0;

	if (p_weight + dest->wrr.wrr_nr_total_weight
	    > src->wrr.wrr_nr_total_weight - p_weight)
		return 0;

	/* we don't care about affinity for now */
	return 1;
}

static struct task_struct *wrr_task_of(struct sched_wrr_entity *wrr_se)
{
	return container_of(wrr_se, struct task_struct, wrr);
}

static int
move_one_task(int src_cpu, int dest_cpu)
{
	struct sched_wrr_entity *pse, *nse;
	struct task_struct *p;
	struct rq *src = cpu_rq(src_cpu);
	struct rq *dest = cpu_rq(dest_cpu);

	list_for_each_entry_safe(pse, nse, &src->wrr.wrr_entity_list,
				 run_list) {
		p = wrr_task_of(pse);

		if (!can_migrate_task(p, src, dest_cpu, dest))
			continue;

		__move_task(p, src, dest_cpu, dest);

		return 1;
	}
	return 0;
}

static void yield_task_wrr(struct rq *rq)
{

}
const struct sched_class wrr_sched_class = {
	.next			= &fair_sched_class,
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
	.yield_task		= yield_task_wrr,
};
void init_wrr_rq(struct wrr_rq *wrr_rq, struct rq *rq)
{
	wrr_rq->rq = rq;
	wrr_rq->wrr_nr_total_weight = 0;
	wrr_rq->clock_tick = 0;
	INIT_LIST_HEAD(&wrr_rq->wrr_entity_list);
}
