/**
 * @file lv_check_obj.h
 *
 */

#ifndef LV_CHECK_OBJ_H
#define LV_CHECK_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../config/lv_conf_internal.h"
#include "../logging/lv_log.h"
#include "lv_check_arg.h"

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

#if LV_USE_CHECK_ARG

#if LV_USE_CHECK_OBJ_CLASSTYPE
    /**
     * Check that `obj` is a descendant of the given class `cls`.
     * Logs a warning and executes `action` if the check fails.
     * Enabled when `LV_USE_CHECK_OBJ_CLASSTYPE = 1`; expands to a NULL check otherwise.
     *
     * @param obj     Pointer to the object to check.
     * @param cls     Pointer to the expected `lv_obj_class_t` (e.g. `&lv_label_class`).
     * @param action  Statement to execute on failure (e.g. `return false`).
     */
    #define LV_CHECK_OBJ_CLASS(obj, cls, action) \
        LV_CHECK_ARG(obj != NULL, action); \
        LV_CHECK_ARG(lv_obj_has_class(obj, cls), action);
#else
    #define LV_CHECK_OBJ_CLASS(obj, cls, action) LV_CHECK_ARG(obj != NULL, action);
#endif

#if LV_USE_CHECK_OBJ_VALIDITY
    /**
     * Check that `obj` is a live, reachable widget (i.e. part of the widget tree).
     * Logs a warning and executes `action` if the check fails.
     * Enabled when `LV_USE_CHECK_OBJ_VALIDITY = 1`; expands to nothing otherwise.
     *
     * @param obj     Pointer to the object to validate. Must not be NULL.
     * @param action  Statement to execute on failure (e.g. `return false`).
     */
    #define LV_CHECK_OBJ_VALID(obj, cls, action) \
      LV_CHECK_OBJ_CLASS(obj, cls, action); \
      LV_CHECK_ARG(lv_obj_is_valid(obj), action);
#else
    #define LV_CHECK_OBJ_VALID(obj, cls, action) LV_CHECK_OBJ_CLASS(obj, cls, action);
#endif

#define LV_CHECK_OBJ(obj, cls, action) LV_CHECK_OBJ_VALID(obj, cls, action)

#else

#define LV_CHECK_OBJ_VALID(obj, cls, action)
#define LV_CHECK_OBJ_CLASS(obj, cls, action)
#define LV_CHECK_OBJ(obj, cls, action)

#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CHECK_OBJ_H*/
