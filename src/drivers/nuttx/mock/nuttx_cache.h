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

/*********************
 *      DEFINES
 *********************/

#define up_invalidate_dcache(start, end) do {} while(0)
#define up_flush_dcache(start, end) do {} while(0)

/*********************
 *      TYPEDEFS
 *********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*NUTTX_CACHE_H*/
