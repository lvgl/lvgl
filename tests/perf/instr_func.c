#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

/**
 * Entry function for function instrumentation
 * This functions will be called at the start of an instrumented function's execution
 * (see gcc's "-finstrument functions" flag) and provided with the address
 * of the instrumented function (this_fn) and the address where this function
 * was called from in the parent function (call_site)
 *
 * Prints a message to timestamp the entry time of the instrumented function
 */
void __cyg_profile_func_enter(void * this_fn, void * call_site)
{
    struct timespec ts;
    // Get timestamp
    clock_gettime(CLOCK_REALTIME, &ts);
    // Write timestamp with identifier
    printf("prof > %lx: %llu.%llu\n", call_site, ts.tv_sec, ts.tv_nsec);
}

/**
 * Exit function for function instrumentation
 * This functions will be called at the end of an instrumented function's execution
 * (see gcc's "-finstrument functions" flag) and provided with the address
 * of the instrumented function (this_fn) and the address where this function
 * was called from in the parent function (call_site)
 *
 * Prints a message to timestamp the exit time of the instrumented function
 */
void __cyg_profile_func_exit(void * this_fn, void * call_site)
{
    struct timespec ts;
    // Get timestamp
    clock_gettime(CLOCK_REALTIME, &ts);
    // Write timestamp with identifier
    printf("prof < %lx: %llu.%llu\n", call_site, ts.tv_sec, ts.tv_nsec);
}