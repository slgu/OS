#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */
#include <linux/sched.h>        /* Needed by current macros */


struct prinfo {
	pid_t parent_pid;		/* process id of parent */
	pid_t pid;			/* process id */
	pid_t first_child_pid;  	/* pid of youngest child */
	pid_t next_sibling_pid;  	/* pid of older sibling */
	long state;			/* current state of process */
	long uid;			/* user id of process owner */
	char comm[64];			/* name of program executed */
};

/*kernel copy*/
void copy_task_to_buf(struct prinfo *buf, struct task_struct *p)
{
	int l = strlen(p->comm);
	buf->pid = p->pid;
	buf->parent_pid = p->real_parent->pid;
	buf->state = p->state;
	buf->uid = p->real_cred->uid.val;
	if (l < 64) {
		memcpy(buf->comm, p->comm, l);
		buf->comm[l] = '\0';
	}
	else
		buf->comm[0] = '\0';
	if (p->children.next == (&(p->children)))
		buf->first_child_pid = 0;
	else
		buf->first_child_pid = list_entry(p->children.next, struct task_struct, sibling)->pid;
	if (p->sibling.next == &(p->real_parent->children))
		buf->next_sibling_pid = 0;
	else
		buf->next_sibling_pid = list_entry(p->sibling.next, struct task_struct, sibling)->pid;
}

void debug_buf(struct prinfo * buf)
{
	printf("%s,%d,%ld,%d,%d,%d,%d\n", p.comm, p.pid, p.state,
				p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);
        printk("pid=[%d],parent_pid=[%d],name=[%s],state=[%d],uid=[%d]\n", buf->pid, buf->parent_pid, buf->comm, buf->state, buf->uid);
	printk("firstchildpid=[%d], next_sibling_pid=[%d]\n", buf->first_child_pid, buf->next_sibling_pid);
}


int ptree(struct prinfo * buf, int * nr)
{
	struct task_struct *p = &init_task;
	struct prinfo kernel_buff;
	int total_cp_number; 
	int ret;
	int total_cnt = 0;
	int cp_cnt = 0;
	int total_cp_number = *br;

	/* Not NULL or zero*/
	if (buf == NULL || total_cp_number < 1)
		return -EINVAL;

	/* lock the process lists */
	//read_lock(&tasklist_lock);
	while (1) {

		/* copy this task */
		if (cp_cnt < total_cp_number) {
			copy_task_to_buf(&kernel_buff, p); 
			debug_buf(&kernel_buff);
			copy_to_user(buf + cp_cnt, &kernel_buff, sizeof(struct prinfo)); 
			++cp_cnt;
		}
		
		/* accumulate total cnt*/
		++total_cnt;

		/* first child */
		if (p->children.next != (&(p->children))) {
			p = list_entry(p->children.next,struct task_struct, sibling);
		}
		/* no children then sibling */
		else if (p->sibling.next != &(p->real_parent->children))			
			p = list_entry(p->sibling.next, struct task_struct, sibling);
		/* no children no sibling then trace back*/
		else {
			p = p->real_parent;
			while(1) {
				if (p == &init_task)
					break;
				if (p->sibling.next != &(p->real_parent->children)) {
					p = list_entry(p->sibling.next, struct task_struct, sibling);
					break;
				}
				/*no sibling then backtrack*/
				else {
					p = p->real_parent;
				}
			}
			if (p == &init_task)
				break;
		}
	}
	//read_unlock(&tasklist_lock);
	return total_cnt;
}

void print_pid_list(void)
{
        struct task_struct *task = &init_task;
        struct task_struct *p = NULL;
        struct list_head *pos;
	struct prinfo buf;
        list_for_each(pos, &task->tasks)
        {
                p = list_entry(pos, struct task_struct, tasks);
		copy_task_to_buf(&buf, p);
		debug_buf(&buf);
        }
}

static int __init test_init(void)
{
        //print_pid_list();
	struct prinfo buf[10];
	int n = 10;
	int i = 0;
	int total;
        printk(KERN_INFO "test module init\n");
        printk(KERN_INFO "current pid = [%d]\n", current->pid);
	total = ptree(buf, &n);
	printk("total processes number:%d\n", total);
	for (;i < n; ++i) {
		//debug_buf(buf + i);
	}
        return 0;
}

static void __exit test_exit(void)
{
        printk(KERN_INFO "test module exit\n");
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");
