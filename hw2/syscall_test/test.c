#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "prinfo.h"
#include <sys/syscall.h> 
#include <stdio.h>

void debug_buf(struct prinfo * buf)
{
        printf("pid=[%d],parent_pid=[%d],name=[%s],state=[%d],uid=[%d]\n", buf->pid, buf->parent_pid, buf->comm, buf->state, buf->uid);
	printf("firstchildpid=[%d], next_sibling_pid=[%d]\n", buf->first_child_pid, buf->next_sibling_pid);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("wrong input number\n");
		return 1;
	}
	int n = atoi(argv[1]);
	struct prinfo * buff = (struct prinfo *)malloc(n * sizeof(struct prinfo));
	int ret = syscall(223, buff, &n);
	int i;
	if (ret > 0) {
	        printf("total number: %d\n", ret);
	        for (i = 0; i < n; ++i)
	    	    debug_buf(buff + i);
	}
	else {
	        printf("get process info error\n");
	}
	return 0;
}
