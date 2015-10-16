#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "light.h"

#define N 100
#define LOW_INTENSITY  2000
#define MID_INTENSITY  2000
#define HIGH_INTENSITY 40000
#define FREQUENCY 10
struct event_requirements low;
struct event_requirements mid;
struct event_requirements high;

int low_eid;
int mid_eid;
int high_eid;
void add_intensity_event(int level)
{
	switch(level) {
		case 0:
			printf("%d\n", low_eid = syscall(380, &low));
			break;
		case 1:
			printf("%d\n", mid_eid = syscall(380, &mid));
			break;
		case 2:
			printf("%d\n", high_eid = syscall(380, &high));
			break;
	}
}

void init(void)
{
	low.req_intensity = LOW_INTENSITY;
	low.frequency = FREQUENCY;
	mid.req_intensity = MID_INTENSITY;
	mid.frequency = FREQUENCY;
	high.req_intensity = HIGH_INTENSITY;
	high.frequency = FREQUENCY;
}

int main (void)
{
	/* Write your test program here */
	int i;
	int pid;
	
	init();
	add_intensity_event(0);
	add_intensity_event(1);
	add_intensity_event(2);
	for (i = 0; i < N; ++i) {
		pid = fork();
		if (pid == 0) {
			int idx = (i % 3);
			int ret;

			printf("idx:%d begin to wait\n", i);
			switch(idx) {
				case 0:
					ret = syscall(381, low_eid);
					break;
				case 1:
					ret = syscall(381, mid_eid);
					break;
				case 2:
					ret = syscall(381, high_eid);
					break;
			}
			printf("process idx:%d pid:%d detect the event %d, retvalue:%d \n", i, pid, idx, ret);
			return 0;
		}
		else if (pid < 0) {

		}
		else {

		}
	}
	printf("main process sleep waiting\n");
	sleep(30);
	printf("main process begin destroy event\n");
	/* delete */
	syscall(383, low_eid);
	printf("delete 1 done\n");
	syscall(383, mid_eid);
	printf("delete 2 done\n");
	syscall(383, high_eid);
	printf("delete 3 done\n");
	sleep(10);
	return 0;
}
