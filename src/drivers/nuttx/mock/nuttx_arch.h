/**
 * @file nuttx_arch.h
 *
 */

#ifndef NUTTX_ARCH_H
#define NUTTX_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline unsigned long up_perf_getfreq(void)
{
    return 1000000; /*1 MHz for mock*/
}

static inline uint32_t up_perf_gettime(void)
{
    return 0;
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*NUTTX_ARCH_H*/
