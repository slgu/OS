/*************************************************************************
    > File Name: gps.c
    > Author: Gu Shenlong
    > Mail: blackhero98@gmail.com
    > Created Time: Sat 12 Dec 2015 11:16:50 AM EST
 ************************************************************************/

#include <linux/gps.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/limits.h>
#include <linux/path.h>
#include <linux/errno.h>
#include <linux/namei.h>
#include <linux/rwlock.h>
struct gps_kernel_location kernel_gps;
/* lock for kernel gps */
DEFINE_RWLOCK(kernel_gps_lock);
SYSCALL_DEFINE1(set_gps_location, struct gps_location __user *, loc)
{
	struct gps_location tmp_gps;
	/* copy to kernel gps */
	/* cnmb bug */
	if (copy_from_user(&tmp_gps, loc, sizeof(struct gps_location)) != 0)
		return -1;

	/* store not in double and lock*/
	write_lock(&kernel_gps_lock);
	kernel_gps.latitude = *(u64 *)(&tmp_gps.latitude);
	kernel_gps.longitude = *(u64 *)(&tmp_gps.longitude);
	kernel_gps.accuracy = *(u32 *)(&tmp_gps.accuracy);
	kernel_gps.time = current_kernel_time().tv_sec;
	write_unlock(&kernel_gps_lock);
	return 0;
}
SYSCALL_DEFINE2(get_gps_location, const char __user *, pathname,
		struct gps_location __user *, loc)
{
	int error;
	int time_pass;
	char *pname;
	struct file *fp;
	struct inode *finode;
	struct gps_location gps_loc;
	int (*get_gps_location)(struct inode *, struct gps_location *);

	pname = kmalloc_array(PATH_MAX, sizeof(char), GFP_KERNEL);
	if (pname == NULL)
		return -ENOMEM;
	error = strncpy_from_user(pname, pathname, PATH_MAX);
	if (error < 0) {
		kfree(pname);
		return -4;
	}
	fp = filp_open(pname, O_RDONLY, 0);
	kfree(pname);
	if (IS_ERR(fp))
		return -1;

	finode = fp->f_dentry->d_inode;
	if (finode == NULL) {
		error =  -ENODEV;
		goto error_return;
	}
	if (finode->i_op->get_gps_location == NULL) {
		error = -ENODEV;
		goto error_return;
	}
	get_gps_location = finode->i_op->get_gps_location;
	time_pass = get_gps_location(finode, &gps_loc);
	filp_close(fp, NULL);

	if (time_pass < 0)
		return -1;
	error = copy_to_user(loc, &gps_loc, sizeof(struct gps_location));
	if (error < 0)
		return -5;
	return time_pass;

error_return:
	filp_close(fp, NULL);
	return error;
}
