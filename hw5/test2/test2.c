/*************************************************************************
    > File Name: test2.c
    > Author: Gu Shenlong
    > Mail: blackhero98@gmail.com
    > Created Time: Wed 18 Nov 2015 09:31:10 PM EST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
int * a;
int main() {
	a = (int *)malloc(1000 * sizeof(int));
	a[3] = 4;
	while (1) {
	}
}
