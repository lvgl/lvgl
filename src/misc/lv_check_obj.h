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
#include "../lv_conf_internal.h"
#include "lv_log.h"
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

#define __LV_CHECK_OBJ_1(obj, action) \
    LV_CHECK_ARG((obj) != NULL, action)

#if LV_USE_CHECK_OBJ_CLASSTYPE
    /**
     * Check that `obj` belongs to the given class `cls`.
     * Logs a warning and executes `action` if the check fails.
     * Enabled when `LV_USE_CHECK_OBJ_CLASSTYPE = 1`; expands to nothing otherwise.
     *
     * @param obj     Pointer to the object to check. Must not be NULL.
     * @param cls     Pointer to the expected `lv_obj_class_t` (e.g. `&lv_label_class`).
     * @param action  Statement to execute on failure (e.g. `return false`).
     */
    #define LV_CHECK_OBJ_CLASS(obj, cls, action) LV_CHECK_ARG(lv_obj_has_class(obj, cls), action);
#else
    #define LV_CHECK_OBJ_CLASS(obj, cls, action)
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
    #define LV_CHECK_OBJ_VALID(obj, action) LV_CHECK_ARG(lv_obj_is_valid(obj), action);
#else
    #define LV_CHECK_OBJ_VALID(obj, action)
#endif

#define __LV_CHECK_OBJ_2(obj, cls, action) \
    LV_CHECK_ARG((obj) != NULL, action); \
    LV_CHECK_OBJ_CLASS(obj, cls, action)

#define __LV_CHECK_OBJ_3(obj, cls, associated, action) \
    LV_CHECK_ARG((obj) != NULL, action); \
    LV_CHECK_OBJ_CLASS(obj, cls, action) \
    LV_CHECK_OBJ_VALID(obj, action)

/* Internal: picks __LV_CHECK_OBJ_1/2/3 based on the number of __VA_ARGS__ */
#define __LV_CHECK_OBJ_PICK_(_1, _2, _3, NAME, ...) NAME

/**
 * Check that an object pointer is valid, optionally also checking its class
 * and whether it's part of the widget tree. Logs a warning and executes `action` on failure.
 *
 * Usage:
 *   LV_CHECK_OBJ(obj, return false)                         // NULL check only
 *   LV_CHECK_OBJ(obj, &lv_label_class, return false)        // NULL + class check
 *   LV_CHECK_OBJ(obj, &lv_label_class, ext, return false)   // NULL + class + in_tree check
 */
#define LV_CHECK_OBJ(obj, ...) \
    __LV_CHECK_OBJ_PICK_(__VA_ARGS__, __LV_CHECK_OBJ_3, __LV_CHECK_OBJ_2, __LV_CHECK_OBJ_1, ~)(obj, __VA_ARGS__)

#else

#define LV_CHECK_OBJ(...)

#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CHECK_OBJ_H*/
