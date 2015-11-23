#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include "err.h"
#define NUM 10
#define BIG_NUM 2000
#define PAGE_SIZE 4096 
#define SUPER_BIG_NUM 20000
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
	char * p = (char *)mmap(0, NUM * PAGE_SIZE, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);	
	if (0)
		p[0] = '0';
	return 0;
}

static int test_2(void)
{
	char * p = (char *)mmap(0, NUM * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);	
	if (0)
		p[0] = '0';
	int i = 0;
	char a;
	for (i = 0; i < NUM; ++i)
		a = p[i * PAGE_SIZE];
	if (0)
		printf("%c", a);
	return 0;
}

static int test_3(void)
{
	char * p = (char *)mmap(0, NUM * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);	
	if (0)
		p[0] = '0';
	int i = 0;
	char a;
	for (i = 1; i < NUM; i += 2)
		a = p[i * PAGE_SIZE];
	if (0)
		printf("%c", a);
	return 0;
}

static int test_4(void)
{
	char * p = (char *)mmap(0, NUM * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);	
	if (0)
		p[0] = '0';
	int i;
	for (i = 0; i < NUM / 2; ++i)
		p[i * PAGE_SIZE] = 0;
	return 0;
}

static int test_5(void)
{
	char * p = (char *)mmap(0, NUM * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);	
	if (0)
		p[0] = '0';
	int i;
	char a;

	for (i = 0; i < 4; ++i)
		a = p[i * PAGE_SIZE];
	for (i = 6; i < NUM; ++i)
		p[i * PAGE_SIZE] = 0;
	if (0)
		printf("%c",a);
	return 0;
}

static int test_6(void)
{
	char * p = (char *)mmap(0, BIG_NUM * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);	
	if (0)
		p[0] = '0';
	return 0;
}

static int test_7(void)
{
	char * p = (char *)mmap(0, SUPER_BIG_NUM * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);	
	if (0)
		p[0] = 0;
	return 0;
}

int main(int argc, char **argv)
{
	/*
	 * Change this main function as you see fit.
	 */
	if (0) {
		test_1();
		test_2();
		test_3();
		test_4();
		test_5();
		test_6();
	}
	test_7();
	print_maps();
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
