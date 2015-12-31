/*************************************************************************
    > File Name: test.c
    > Author: Gu Shenlong
    > Mail: blackhero98@gmail.com
    > Created Time: Sun 08 Nov 2015 10:08:53 PM EST
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define MAX_CPUS 4 /* Since we will be testing only on the Nexus 7 */
struct wrr_info {
	int num_cpus;
	int nr_running[MAX_CPUS];
	int total_weight[MAX_CPUS];
};

int main(void)
{
	struct wrr_info *wrr_info_user =
		(struct wrr_info *)malloc(sizeof(struct wrr_info));
	int i;
	int ret = syscall(378, wrr_info_user);

	printf("%d\n", ret);
	for (i = 0; i < MAX_CPUS; ++i)
		printf("%d %d %d\n", i,
			wrr_info_user->nr_running[i],
			wrr_info_user->total_weight[i]);
	return 0;
}
