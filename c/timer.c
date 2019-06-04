#include "timer.h"

#include <time.h>

double timer_now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return ts.tv_nsec / 1000000000.0 + ts.tv_sec;
}

