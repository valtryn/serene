#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>
#include "util.h"

void util_delay_ns(unsigned long ns)
{
	struct timespec ts;
	ts.tv_sec  = (ns / NS_PER_SEC);
	ts.tv_nsec = (ns % NS_PER_SEC);
	nanosleep(&ts, NULL);
	/* __asm__ __volatile__("pause\n"); */
	return;
}

unsigned long util_get_ns(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_nsec;
}
