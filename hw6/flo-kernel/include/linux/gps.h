/*************************************************************************
    > File Name: gps.h
    > Author: Gu Shenlong
    > Mail: blackhero98@gmail.com
    > Created Time: Sat 12 Dec 2015 11:15:29 AM EST
 ************************************************************************/


#ifndef _H_GPS
#define _H_GPS
#include <linux/time.h>
struct gps_location {
	double latitude;
	double longitude;
	float  accuracy;  /* in meters */
};

/* not double representation */
struct gps_kernel_location {
	u64 latitude;
	u64 longitude;
	u32 accuracy;
	time_t time;
	/* read write lock needed */
};

extern struct gps_kernel_location kernel_gps;
extern rwlock_t kernel_gps_lock;
#endif
