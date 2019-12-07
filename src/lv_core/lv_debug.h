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

#if LV_USE_DEBUG

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

bool lv_debug_check_style(const lv_style_t * style);

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
        lv_debug_log_error(msg, (uint64_t)((uintptr_t)value));         \
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
#define LV_DEBUG_IS_STR(str)   (lv_debug_check_null(str) &&      \
                                lv_debug_check_str(str))
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
#  define LV_ASSERT_STR(str) LV_DEBUG_ASSERT(LV_DEBUG_IS_STR(str), "Strange or invalid string", str);
# endif
#else /* LV_USE_ASSERT_OBJ == 0 */
# if LV_USE_ASSERT_NULL /*Use at least LV_ASSERT_NULL if enabled*/
#   define LV_ASSERT_STR(str) LV_ASSERT_NULL(str)
# else
#   define LV_ASSERT_STR(str) true
# endif
#endif


#if LV_USE_ASSERT_OBJ
# ifndef LV_ASSERT_OBJ
#  define LV_ASSERT_OBJ(obj_p, obj_type) LV_DEBUG_ASSERT(LV_DEBUG_IS_OBJ(obj_p, obj_type), "Invalid object", obj_p);
# endif
#else /* LV_USE_ASSERT_OBJ == 0 */
# if LV_USE_ASSERT_NULL /*Use at least LV_ASSERT_NULL if enabled*/
#   define LV_ASSERT_OBJ(obj_p, obj_type) LV_ASSERT_NULL(obj_p)
# else
#   define LV_ASSERT_OBJ(obj_p, obj_type) true
# endif
#endif


#if LV_USE_ASSERT_STYLE
# ifndef LV_ASSERT_STYLE
#  define LV_ASSERT_STYLE(style_p) LV_DEBUG_ASSERT(LV_DEBUG_IS_STYLE(style_p), "Invalid style", style_p);
# endif
#else
#  define LV_ASSERT_STYLE(style) true
#endif

#else /* LV_USE_DEBUG == 0 */

#define LV_DEBUG_ASSERT(expr, msg, value) do{}while(0)

#define LV_ASSERT_NULL(p) true
#define LV_ASSERT_MEM(p) true
#define LV_ASSERT_STR(p) true
#define LV_ASSERT_OBJ(obj, obj_type) true
#define LV_ASSERT_STYLE(p) true

#endif /* LV_USE_DEBUG */
/*clang-format on*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEBUG_H*/
