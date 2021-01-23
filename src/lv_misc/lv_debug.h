/**
 * @file lv_debug.h
 *
 */

#ifndef LV_DEBUG_H
#define LV_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_DEBUG
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
bool lv_debug_check_null(const void * p);

bool lv_debug_check_mem_integrity(void);

bool lv_debug_check_str(const void * str);

void lv_debug_log_error(const char * msg, uint64_t value);

/**********************
 *      MACROS
 **********************/

#ifndef LV_DEBUG_ASSERT
#define LV_DEBUG_ASSERT(expr, msg, value)       \
    do {                                            \
        if(!(expr)) {                               \
            LV_LOG_ERROR(__func__);                 \
            lv_debug_log_error(msg, (uint64_t)((uintptr_t)value));         \
            while(1);                               \
        }                                           \
    } while(0)
#endif

/*----------------
 * CHECKS
 *----------------*/

#ifndef LV_DEBUG_IS_NULL
#define LV_DEBUG_IS_NULL(p)    (lv_debug_check_null(p))
#endif

#ifndef LV_DEBUG_CHECK_MEM_INTEGRITY
#define LV_DEBUG_CHECK_MEM_INTEGRITY()    (lv_debug_check_mem_integrity())
#endif

#ifndef LV_DEBUG_IS_STR
#define LV_DEBUG_IS_STR(str)   (lv_debug_check_null(str) &&      \
                                lv_debug_check_str(str))
#endif

/*-----------------
 * ASSERTS
 *-----------------*/

/*clang-format off*/

#if LV_USE_ASSERT_NULL
# ifndef LV_ASSERT_NULL
#  define LV_ASSERT_NULL(p) LV_DEBUG_ASSERT(LV_DEBUG_IS_NULL(p), "NULL pointer", p);
# endif
#else
# define LV_ASSERT_NULL(p)
#endif

#if LV_USE_ASSERT_MEM
# ifndef LV_ASSERT_MEM
#  define LV_ASSERT_MEM(p) LV_DEBUG_ASSERT(LV_DEBUG_IS_NULL(p), "Out of memory", p);
# endif
#else
# define LV_ASSERT_MEM(p)
#endif

#if LV_USE_ASSERT_MEM_INTEGRITY
# ifndef LV_ASSERT_MEM_INTEGRITY
#  define LV_ASSERT_MEM_INTEGRITY() LV_DEBUG_ASSERT(LV_DEBUG_CHECK_MEM_INTEGRITY(), "Memory integrity error", 0);
# endif
#else
# define LV_ASSERT_MEM_INTEGRITY()
#endif

#if LV_USE_ASSERT_STR
# ifndef LV_ASSERT_STR
#  define LV_ASSERT_STR(str) LV_DEBUG_ASSERT(LV_DEBUG_IS_STR(str), "Strange or invalid string", str);
# endif
#else /* LV_USE_ASSERT_OBJ == 0 */
# if LV_USE_ASSERT_NULL /*Use at least LV_ASSERT_NULL if enabled*/
#   define LV_ASSERT_STR(str) LV_ASSERT_NULL(str)
# else
#   define LV_ASSERT_STR(str)
# endif
#endif

#else /* LV_USE_DEBUG == 0 */

#define LV_DEBUG_ASSERT(expr, msg, value) do{}while(0)

#define LV_ASSERT_NULL(p)
#define LV_ASSERT_MEM(p)
#define LV_ASSERT_MEM_INTEGRITY()
#define LV_ASSERT_STR(p)
#define LV_ASSERT_OBJ(obj, obj_type)

#endif /* LV_USE_DEBUG */
/*clang-format on*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEBUG_H*/
