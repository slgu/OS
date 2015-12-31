/*************************************************************************
    > File Name: 1.c
    > Author: Gu Shenlong
    > Mail: blackhero98@gmail.com
    > Created Time: Sat 07 Nov 2015 05:50:34 PM EST
 ************************************************************************/

#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#define SCHED_WRR 6
#define SCHED_RR 2
int main(void)
{
	struct sched_param param;
	int i;
	int j = 10;
	int k;
	int pid;
	int cnt = 0;

	param.sched_priority = 0;
	for (i = 0; i < 40; ++i) {
		pid = fork();
		if (pid == -1)
			return -1;
		else if (pid == 0) {
			if (sched_setscheduler(getpid(),
					SCHED_WRR, &param) == -1)
				puts("set scheduler false");
			printf("%d begin cpu\n", i);
			for (k = 0; k < 10000; ++k) {
				for (j = 0; j < 10000; ++j)
					cnt += j;
			}
			printf("total:%d\n", cnt);
			printf("%d sleep 1 sec\n", i);
			sleep(1);
			for (k = 0; k < 10000; ++k) {
				for (j = 0; j < 10000; ++j)
					cnt += j;
			}
			printf("total:%d\n", cnt);
			return 0;
		}
	}
	sleep(20);
	printf("main donen\n");
	return 0;
}
