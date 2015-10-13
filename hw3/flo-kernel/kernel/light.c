/*************************************************************************
    > File Name: light.c
    > Author: Gu Shenlong
    > Mail: blackhero98@gmail.com
    > Created Time: Mon 12 Oct 2015 12:31:15 PM EDT
 ************************************************************************/

#include <linux/light.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/rwlock.h>
#include <linux/list.h>
#include <linux/slab.h>
/* TODO */

/* kernel data */
struct light_intensity kernel_light_intensity_arr[WINDOW];
int start = 0;
int end = 0;
int total = 0;
struct light_intensity kernel_light_intensity;
struct light_event * event_list = NULL;
int flg = 0;
static DEFINE_MUTEX(light_event_list_lock); /* lock for event list */
int init_once(void) {
	if (flg)
		return 0;
	event_list = (struct light_event *)kmalloc(sizeof(struct light_event), GFP_KERNEL);
	if (!event_list) {
		return 1;
	}
	/* init event list */
	INIT_LIST_HEAD(&event_list->list);
	flg = 1;
	return 0;
}
void add_light_intensity(struct light_intensity * another) {
	kernel_light_intensity_arr[end].cur_intensity = 
		another->cur_intensity;
	end = (end + 1) % WINDOW;
	if (total == WINDOW) {
		start = (start + 1) % WINDOW;
	} else {
		total = total + 1;		
	}
}

SYSCALL_DEFINE1(set_light_intensity, struct light_intensity __user *, user_light_intensity)
{
	int ret;

	if (current_cred()->uid != 0)
	     return -EPERM;

	ret = copy_from_user(&kernel_light_intensity, user_light_intensity, sizeof(struct light_intensity));
	if (ret != 0)
		return ret;
	/* add to kernel buffer */
	add_light_intensity(&kernel_light_intensity);
	return 0;
}

SYSCALL_DEFINE1(get_light_intensity, struct light_intensity __user *, user_light_intensity)
{
	return copy_to_user(user_light_intensity, &kernel_light_intensity, sizeof(struct light_intensity));	
}

SYSCALL_DEFINE1(light_evt_create, struct event_requirements __user *, intensity_params)
{
	struct light_event * ptr = kmalloc(sizeof(struct light_event), GFP_KERNEL);
	if (!ptr) {
		return -ENOMEM;
	}
	/* init lock */
	mutex_init(&ptr->lock);
	/* init waiting queue */
	init_waitqueue_head(&ptr->queue);
	/* lock the light event list and add ptr*/
	mutex_lock(&light_event_list_lock);
	/* TODO insert */
	mutex_unlock(&light_event_list_lock);
	return 0;
}
