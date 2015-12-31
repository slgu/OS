/*************************************************************************
    > File Name: 1.c
    > Author: Gu Shenlong
    > Mail: blackhero98@gmail.com
    > Created Time: Sat 07 Nov 2015 05:50:34 PM EST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
/* set weight */
int main(int argc, char *argv[])
{
	int fg_weight = 0;
	int bg_weight = 0;
	int ret;

	if (argc != 3) {
		printf("usage ./test2 fg_weight bg_weight\n");
		return 0;
	}
	fg_weight = atoi(argv[1]);
	bg_weight = atoi(argv[2]);
	if (fg_weight == 0 || bg_weight == 0) {
		printf("input error\n");
		return 0;
	}
	ret = syscall(379, fg_weight, bg_weight);
	printf("ret:%d\n", ret);
	return 0;
}
