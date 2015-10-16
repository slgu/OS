/*************************************************************************
    > File Name: ../include/linux/light.h
    > Author: Gu Shenlong
    > Mail: blackhero98@gmail.com
    > Created Time: Mon 12 Oct 2015 12:39:09 PM EDT
 ************************************************************************/

#ifndef _LIGHT_H
#define _LIGHT_H
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <asm/atomic.h>
/* TODO */
struct light_intensity {
	int cur_intensity; /* scaled intensity as read from the light sensor */
}; 
#define NOISE 10
#define WINDOW 20

#define EVT_REQUIREMENT_SATISFIED 1
#define EVT_NEED_KFREE 2
#define EVT_NOTHING 0
/*
 * Defines a light event.
 *
 * Event is defined by a required intensity and frequency.
 */
struct event_requirements {
	int req_intensity; /* scaled value of light intensity in centi-lux */
	int frequency;     /* number of samples with intensity-noise > req_intensity */
};

struct light_event {
	wait_queue_head_t queue; /* processes waiting for this event */
	struct list_head list; /* list of event */
	int eid; /* event id */
	struct event_requirements er; /*event requirements */
	int flg; /* used for wait event */
	struct mutex lock; /* used for wait_queue_head add remove */
	atomic_t ref_count;
}; 
#endif
