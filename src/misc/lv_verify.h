/**
 * @file lv_verify.h
 *
 */

#ifndef LV_VERIFY_H
#define LV_VERIFY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_log.h"
#include "lv_assert.h"

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

/**
 * Internal macro: stringify condition at the point of use to prevent recursive macro expansion.
 * Do not use directly; use LV_VERIFY or one of its wrappers instead.
 *
 */
#define LV_VERIFY_INTERNAL_(cond, cond_str, action_on_fail, ...)                            \
    if(!(cond)) {                                                                           \
        LV_LOG_WARN("Verification failed: " cond_str __VA_ARGS__);                          \
        action_on_fail;                                                                     \
    }

/**
 * Verify that a condition is true. If the condition is false, log a warning
 * and execute `action_on_fail` (e.g. `return`, `return val`, `break`).
 * Additional printf-style arguments are appended to the log message.
 *
 * Example:
 * @code
 * LV_VERIFY(ptr != NULL, return, ": pointer must not be NULL");
 * LV_VERIFY(len > 0, return -1, ": len=%d", (int)len);
 * @endcode
 *
 * @param cond              condition to verify
 * @param action_on_fail    statement to execute on failure (e.g. `return`, `return 0`, `break)
 * @param ...               optional printf-style format string and arguments appended to the log
 */
#define LV_VERIFY(cond, action_on_fail, ...)                                                \
    LV_VERIFY_INTERNAL_(cond, #cond, action_on_fail, __VA_ARGS__)

/**
 * Verify a condition; on failure only log a warning (no return or other action).
 * Additional printf-style arguments are appended to the log message.
 *
 * Example:
 * @code
 * LV_VERIFY_OR_LOG(obj != NULL, ": obj is NULL, continuing with defaults");
 * LV_VERIFY_OR_LOG(len > 0, ": unexpected len=%d", (int)len);
 * @endcode
 *
 * @param cond  condition to verify
 * @param ...   optional printf-style format string and arguments appended to the log
 */
#define LV_VERIFY_OR_LOG(cond, ...) LV_VERIFY_INTERNAL_(cond, #cond, LV_UNUSED(0), __VA_ARGS__)

/**
 * Verify a condition; on failure only log a warning (no return or other action).
 * Unlike LV_VERIFY_OR_LOG, the message is a plain string, not a format string.
 *
 * Example:
 * @code
 * LV_VERIFY_OR_LOG_MSG(obj != NULL, "obj is NULL: continuing with defaults");
 * @endcode
 *
 * @param cond  condition to verify
 * @param msg   plain string message to log on failure
 */
#define LV_VERIFY_OR_LOG_MSG(cond, msg) LV_VERIFY_INTERNAL_(cond, #cond, LV_UNUSED(0), ": %s", (msg))

/**
 * Verify a condition; on failure log a message string and execute `action_on_fail`.
 * Unlike LV_VERIFY, the message is a plain string, not a format string.
 *
 * Example:
 * @code
 * LV_VERIFY_MSG(obj != NULL, return, "object is NULL");
 * @endcode
 *
 * @param cond              condition to verify
 * @param action_on_fail    statement to execute on failure
 * @param msg               plain string message to log on failure
 */
#define LV_VERIFY_MSG(cond, action_on_fail, msg) LV_VERIFY_INTERNAL_(cond, #cond, action_on_fail, ": %s", (msg))

/**
 * Verify a condition; on failure log a warning and return from the current (void) function.
 *
 * Example:
 * @code
 * void my_func(lv_obj_t * obj) {
 *     LV_VERIFY_OR_RETURN(obj != NULL, "obj is NULL");
 * }
 * @endcode
 *
 * @param cond  condition to verify
 * @param msg   plain string message to log on failure
 */
#define LV_VERIFY_OR_RETURN(cond, msg) LV_VERIFY_INTERNAL_(cond, #cond, return, ": %s", (msg))

/**
 * Verify a condition; on failure log a warning and return `error_val` from the current function.
 *
 * Example:
 * @code
 * lv_result_t my_func(lv_obj_t * obj) {
 *     LV_VERIFY_OR_RETURN_VAL(obj != NULL, LV_RESULT_INVALID, "obj is NULL");
 *     ...
 * }
 * @endcode
 *
 * @param cond          condition to verify
 * @param error_val     value to return on failure
 * @param msg           plain string message to log on failure
 */
#define LV_VERIFY_OR_RETURN_VAL(cond, error_val, msg) LV_VERIFY_INTERNAL_(cond, #cond, return (error_val), ": %s", (msg))

/**
 * Verify a condition; on failure log a warning and invoke `LV_ASSERT_HANDLER` (typically halts).
 * Additional printf-style arguments are appended to the log message.
 *
 * Example:
 * @code
 * LV_VERIFY_OR_ASSERT(obj != NULL, ": obj must not be NULL");
 * LV_VERIFY_OR_ASSERT(len > 0, ": bad len=%d", (int)len);
 * @endcode
 *
 * @param cond  condition to verify
 * @param ...   optional printf-style format string and arguments appended to the log
 */
#define LV_VERIFY_OR_ASSERT(cond, ...) LV_VERIFY_INTERNAL_(cond, #cond, LV_ASSERT_HANDLER, __VA_ARGS__)

/**
 * Verify a condition; on failure log a plain message and invoke `LV_ASSERT_HANDLER`.
 *
 * Example:
 * @code
 * LV_VERIFY_OR_ASSERT_MSG(obj != NULL, "obj must not be NULL");
 * @endcode
 *
 * @param cond  condition to verify
 * @param msg   plain string message to log on failure
 */
#define LV_VERIFY_OR_ASSERT_MSG(cond, msg) LV_VERIFY_INTERNAL_(cond, #cond, LV_ASSERT_HANDLER, ": %s", (msg))

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VERIFY_H*/
