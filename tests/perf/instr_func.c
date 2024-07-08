#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

void __cyg_profile_func_enter (void *this_fn, void *call_site)
{
    struct timespec ts;
	// Get timestamp
	clock_gettime(CLOCK_REALTIME, &ts);
	// Write timestamp with identifier
	printf("prof > %lx: %llu.%llu\n", call_site, ts.tv_sec, ts.tv_nsec);
}

void __cyg_profile_func_exit  (void *this_fn, void *call_site)
{
    struct timespec ts;
	// Get timestamp
	clock_gettime(CLOCK_REALTIME, &ts);
	// Write timestamp with identifier
	printf("prof < %lx: %llu.%llu\n", call_site, ts.tv_sec, ts.tv_nsec);
}