#ifndef _UTIL_H
#define _UTIL_H

const double NSEC = 1000000000.0f;
/* struct timespec util_get_time(void); */
double util_get_time_ms(void);
void   util_frame_delay(long nanosecond);

#endif /* _UTIL_H */
