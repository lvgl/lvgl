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
#include "lv_obj.h"

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

bool lv_debug_check_obj_type(const lv_obj_t * obj, const char * obj_type);

bool lv_debug_check_obj_valid(const lv_obj_t * obj);

bool lv_debug_check_style(const void * str);

bool lv_debug_check_str(const void * str);

void lv_debug_log_error(const char * msg, uint64_t value);

/**********************
 *      MACROS
 **********************/

#ifndef LV_DEBUG_ASSERT
#define LV_DEBUG_ASSERT(expr, msg, value)       \
{                                               \
    if(!(expr)) {                               \
        LV_LOG_ERROR(__func__);                 \
        lv_debug_log_error(msg, (unsigned long int)value);         \
        while(1);                               \
    }                                           \
}
#endif

/*----------------
 * CHECKS
 *----------------*/

#ifndef LV_DEBUG_IS_NULL
#define LV_DEBUG_IS_NULL(p)    (lv_debug_check_null(p))
#endif

#ifndef LV_DEBUG_IS_STR
#define LV_DEBUG_IS_STR(str)   (lv_debug_check_str(str))
#endif

#ifndef LV_DEBUG_IS_OBJ
#define LV_DEBUG_IS_OBJ(obj_p, obj_type) (lv_debug_check_null(obj_p) &&      \
                                          lv_debug_check_obj_valid(obj_p) && \
                                          lv_debug_check_obj_type(obj_p, obj_type))
#endif

#ifndef LV_DEBUG_IS_STYLE
#define LV_DEBUG_IS_STYLE(style_p) (lv_debug_check_style(style_p))
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
# define LV_ASSERT_NULL(p) true
#endif

#if LV_USE_ASSERT_MEM
# ifndef LV_ASSERT_MEM
#  define LV_ASSERT_MEM(p) LV_DEBUG_ASSERT(LV_DEBUG_IS_NULL(p), "Out of memory", p);
# endif
#else
# define LV_ASSERT_MEM(p) true
#endif

#if LV_USE_ASSERT_STR
# ifndef LV_ASSERT_STR
#  define LV_ASSERT_STR(str) LV_DEBUG_ASSERT(LV_DEBUG_IS_STR(str), "Strange or invalid string", p);
# endif
#else
# define LV_ASSERT_STR(p) true
#endif


#if LV_USE_ASSERT_OBJ
# ifndef LV_ASSERT_OBJ
#  define LV_ASSERT_OBJ(obj_p, obj_type) LV_DEBUG_ASSERT(LV_DEBUG_IS_OBJ(obj_p, obj_type), "Invalid object", obj_p);
# endif
#else
# define LV_ASSERT_OBJ(obj_p, obj_type) true
#endif


#if LV_USE_ASSERT_STYLE
# ifndef LV_ASSERT_STYLE
#  define LV_ASSERT_STYLE(style_p) LV_DEBUG_ASSERT(LV_DEBUG_IS_STYLE(style_p, obj_type), "Invalid style", style_p);
# endif
#else
#  define LV_ASSERT_STYLE(style) true
#endif

/*clang-format on*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEBUG_H*/
