#include <sys/time.h>
#include <stddef.h>

double timer_now(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (t.tv_sec * 1000.0) + (t.tv_usec/1000.0);
}
