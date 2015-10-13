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
LIST_HEAD(event_list);
static DEFINE_MUTEX(light_event_list_lock); /* lock for event list */
#define MAX_EID 1000
int eid[MAX_EID];

int allocate_eid(void) {
	int i;

	for (i = 0; i < MAX_EID; ++i) {
		if (eid[i] == 0) {
			eid[i] = 1;
			return i;
		}
	}
	return -1;
}

void free_eid(int idx) {
	if (idx >= 0 && idx < MAX_EID)
		eid[idx] = 0;
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

/*
 * Create an event based on light intensity.
 *
 * If frequency exceeds WINDOW, cap it at WINDOW.
 * Return an event_id on success and the appropriate error on failure.
 *
 * system call number 380
 */
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
	
	if (copy_from_user(&ptr->er, intensity_params, sizeof(struct event_requirements)) != 0) {
		kfree(ptr);
		return -ENOMEM;
	}
	
	ptr->eid = allocate_eid();
	/* lock the light event list and add ptr*/
	mutex_lock(&light_event_list_lock);
	/* TODO insert */
	list_add_tail(&ptr->list, &event_list); 
	mutex_unlock(&light_event_list_lock);
	return 0;
}
/*
 * Block a process on an event.
 *
 * It takes the event_id as parameter. The event_id requires verification.
 * Return 0 on success and the appropriate error on failure.
 *
 * system call number 381
 */
SYSCALL_DEFINE1(light_evt_wait, int, event_id)
{
	return 0;
}
/*
 * The light_evt_signal system call.
 *
 * Takes sensor data from user, stores the data in the kernel,
 * and notifies all open events whose
 * baseline is surpassed.  All processes waiting on a given event 
 * are unblocked.
 *
 * Return 0 success and the appropriate error on failure.
 *
 * system call number 382
 */
SYSCALL_DEFINE1(light_evt_signal, struct light_intensity __user *, user_light_intensity)
{
	return 0;
}
/*
 * Destroy an event using the event_id.
 *
 * Return 0 on success and the appropriate error on failure.
 *
 * system call number 383
 */
SYSCALL_DEFINE1(light_evt_destroy, int, event_id)
{
	return 0;
}
