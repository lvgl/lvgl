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

bool lv_debug_check_obj_type(lv_obj_t * obj, const char * obj_type);

bool lv_debug_check_obj_valid(lv_obj_t * obj);

bool lv_debug_check_malloc(void * p);

void lv_debug_log_error(const char * msg, uint64_t value);

/**********************
 *      MACROS
 **********************/

#define LV_DEBUG_HALT(msg, value)              \
    {                                          \
        lv_debug_log_error(msg, value);        \
        while(1);                              \
    }                                          \

#ifndef LV_ASSERT_NULL
#define LV_ASSERT_NULL(p)                                                \
        if(lv_debug_check_null(p) == false) {                            \
            LV_LOG_ERROR(__func__);                                      \
            LV_DEBUG_HALT("NULL obj. found", (lv_uintptr_t)p);           \
        }
#endif

#ifndef LV_ASSERT_OBJ_NOT_EXISTS
#define LV_ASSERT_OBJ_NOT_EXISTS(obj)                                    \
        if(lv_debug_check_obj_valid(obj) == false) {                     \
            LV_LOG_ERROR(__func__);                                      \
            LV_DEBUG_HALT("Invalid obj, found", (lv_uintptr_t)obj);      \
        }
#endif

#ifndef LV_ASSERT_OBJ_TYPE_ERROR
#define LV_ASSERT_OBJ_TYPE_ERROR(obj, type)                              \
        if(lv_debug_check_obj_type(obj, __LV_OBJX_TYPE) == false) {      \
            LV_LOG_ERROR(__func__);                                      \
            LV_DEBUG_HALT("Obj. type mismatch", (lv_uintptr_t)obj);      \
        }
#endif

#ifndef LV_ASSERT_NO_MEM
#define LV_ASSERT_NO_MEM(p)                                              \
        if(lv_debug_check_malloc(p) == false) {                          \
            LV_LOG_ERROR(__func__);                                      \
            LV_DEBUG_HALT("Out of memory", (lv_uintptr_t)p);      \
        }
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEBUG_H*/
