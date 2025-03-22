#ifndef _UTIL_H
#define _UTIL_H

#define NS_PER_SEC 1000000000LL

void util_delay_ns(long ns);
long util_get_ns(void);
float percentage_of(float percent, float value);

#endif /* _UTIL_H */
