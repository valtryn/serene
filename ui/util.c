#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include "util.h"

double util_get_time_ms(void)
{
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_nsec;
}

struct timespec util_get_time(void)
{
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts;
}

void util_frame_delay(long nanosecond)
{    
}
