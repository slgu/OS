#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "prinfo.h"
#include <sys/syscall.h> 
#include <stdio.h>


void debug_buf(struct prinfo *p)
{
	printf("%s,%d,%ld,%d,%d,%d,%ld\n", p->comm, p->pid, p->state,
				p->parent_pid, p->first_child_pid, p->next_sibling_pid, p->uid);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("wrong input number\n");
		return 1;
	}
	int n = atoi(argv[1]);
	struct prinfo * buff = (struct prinfo *)malloc(n * sizeof(struct prinfo));
	int i, j;
	int level[10000];
	int ret = syscall(223, buff, &n);
	for (i = 0; i < 10000; ++i)
		level[i] = -1;
	if (ret > 0) {
	        printf("total number: %d\n", ret);
		if (ret < n) 
			n = ret;
	        for (i = 0; i < n; ++i){
			level[buff[i].pid] = level[buff[i].parent_pid] + 1;
			for (j = 0; j < level[buff[i].pid]; ++j)
				printf("\t");
			debug_buf(buff + i);
		}
	}
	else {
	        printf("get process info error\n");
	}
	return 0;
}
