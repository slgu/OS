#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include "err.h"

static int print_maps(void);

static void pr_errno(const char *msg)
{
	printf("error: %s, %s", strerror(errno), msg);
}

static void pr_err(const char *msg)
{
	printf("error: %s", msg);
}

static int test_1(void)
{
	return 0;
}

static int test_2(void)
{
	return 0;
}

static int test_3(void)
{
	return 0;
}

static int test_4(void)
{
	return 0;
}

static int test_5(void)
{
	return 0;
}

static int test_6(void)
{
	return 0;
}

static int test_7(void)
{
	return 0;
}

int main(int argc, char **argv)
{
	/*
	 * Change this main function as you see fit.
	 */
	test_1();
	print_maps();
	test_2();
	test_3();
	test_4();
	test_5();
	test_6();
	test_7();
	return 0;
}

static int print_maps(void)
{
	/*
	 * You may not modify print_maps().
	 * Every test should call print_maps() once.
	 */
	char *path;
	char str[25000];
	int fd;
	int r, w;

	path = "/proc/self/maps";
	printf("%s:\n", path);

	fd = open(path, O_RDONLY);

	if (fd < 0)
		pr_errno(path);

	r = read(fd, str, sizeof(str));

	if (r < 0)
		pr_errno("cannot read the mapping");

	if (r == sizeof(str))
		pr_err("mapping too big");

	while (r) {
		w = write(1, str, r);
		if (w < 0)
			pr_errno("cannot write to stdout");
		r -= w;
	}

	return 0;
}
