/**
 * @file lv_assert.h
 *
 */

#ifndef LV_ASSERT_H
#define LV_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_log.h"
#include "lv_mem.h"
#include LV_ASSERT_HANDLER_INCLUDE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#define LV_ASSERT(expr)                                        \
    do {                                                       \
        if(!(expr)) {                                          \
            LV_LOG_ERROR("Asserted at expression: %s", #expr); \
            LV_ASSERT_HANDLER                                  \
        }                                                      \
    } while(0)

#define LV_ASSERT_MSG(expr, msg)                                         \
    do {                                                                 \
        if(!(expr)) {                                                    \
            LV_LOG_ERROR("Asserted at expression: %s (%s)", #expr, msg); \
            LV_ASSERT_HANDLER                                            \
        }                                                                \
    } while(0)

/*-----------------
 * ASSERTS
 *-----------------*/

#if LV_USE_ASSERT_NULL
#   define LV_ASSERT_NULL(p) LV_ASSERT_MSG(p != NULL, "NULL pointer");
#else
#   define LV_ASSERT_NULL(p)
#endif

#if LV_USE_ASSERT_MALLOC
#   define LV_ASSERT_MALLOC(p) LV_ASSERT_MSG(p != NULL, "Out of memory");
#else
#   define LV_ASSERT_MALLOC(p)
#endif

#if LV_USE_ASSERT_MEM_INTEGRITY
#   define LV_ASSERT_MEM_INTEGRITY() LV_ASSERT_MSG(lv_mem_test() == LV_RES_OK, "Memory integrity error");
#else
#   define LV_ASSERT_MEM_INTEGRITY()
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ASSERT_H*/
