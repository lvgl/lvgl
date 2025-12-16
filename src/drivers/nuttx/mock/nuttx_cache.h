/**
 * @file nuttx_cache.h
 *
 */

#ifndef NUTTX_CACHE_H
#define NUTTX_CACHE_H

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

/*********************
 *      TYPEDEFS
 *********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline void up_invalidate_dcache(uintptr_t start, uintptr_t end)
{
    (void)start;
    (void)end;
}

static inline void up_flush_dcache(uintptr_t start, uintptr_t end)
{
    (void)start;
    (void)end;
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*NUTTX_CACHE_H*/
