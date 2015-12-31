/*************************************************************************
    > File Name: wrr.h
    > Author: Gu Shenlong
    > Mail: blackhero98@gmail.com
    > Created Time: Sun 08 Nov 2015 10:04:27 PM EST
 ************************************************************************/
#ifndef _WRR_H
#define _WRR_H
#define MAX_CPUS 4 /* Since we will be testing only on the Nexus 7 */
struct wrr_info {
	int num_cpus;
	int nr_running[MAX_CPUS];
	int total_weight[MAX_CPUS];
};
#endif
