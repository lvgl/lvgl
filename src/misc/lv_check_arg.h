/**
 * @file lv_check_arg.h
 *
 */

#ifndef LV_CHECK_ARG_H
#define LV_CHECK_ARG_H

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

#if defined(__GNUC__) || defined(__clang__)
#define LV_LIKELY(x)   (__builtin_expect(!!(x), 1))
#define LV_UNLIKELY(x) (__builtin_expect(!!(x), 0))
#else
#define LV_LIKELY(x)   (x)
#define LV_UNLIKELY(x) (x)
#endif

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

#if LV_CHECK_ARG_ASSERT_ON_FAIL

/**
 * Internal macro: checks a condition, logs a warning, calls the assert handler,
 * and then executes the specified action on failure.
 * Do not use directly; use LV_CHECK_ARG instead.
 */
#define LV_CHECK_ARG_INTERNAL_(cond, cond_str, action_on_fail, ...)                         \
    if(LV_UNLIKELY(!(cond))) {                                                              \
        LV_LOG_WARN("Check failed: " cond_str " " __VA_ARGS__);                             \
        LV_ASSERT_HANDLER                                                                   \
        action_on_fail;                                                                     \
    } else {}

#else

/**
 * Internal macro: checks a condition, logs a warning, and executes the
 * specified action on failure.
 * Do not use directly; use LV_CHECK_ARG instead.
 */
#define LV_CHECK_ARG_INTERNAL_(cond, cond_str, action_on_fail, ...)                         \
    if(LV_UNLIKELY(!(cond))) {                                                              \
        LV_LOG_WARN("Check failed: " cond_str " " __VA_ARGS__);                             \
        action_on_fail;                                                                     \
    } else {}

#endif /*LV_CHECK_ARG_ASSERT_ON_FAIL*/

#else

/** LV_CHECK_ARG is disabled; all checks compile to nothing. */
#define LV_CHECK_ARG_INTERNAL_(cond, cond_str, action_on_fail, ...) ((void)0)

#endif /*LV_USE_CHECK_ARG*/

/**
 * Check that a condition is true. If the condition is false, log a warning
 * and execute `action_on_fail` (e.g. `return`, `return val`, `break`).
 * Additional printf-style arguments are appended to the log message.
 *
 * Can be disabled entirely by setting LV_USE_CHECK_ARG to 0 in lv_conf.h.
 * If LV_CHECK_ARG_ASSERT_ON_FAIL is 1, LV_ASSERT_HANDLER is also invoked
 * before the action.
 *
 * Example:
 * @code
 * LV_CHECK_ARG(ptr != NULL, return, "pointer must not be NULL");
 * LV_CHECK_ARG(len > 0, return -1, "len=%d", (int)len);
 * @endcode
 *
 * @param cond              condition to check
 * @param action_on_fail    statement to execute on failure (e.g. `return`, `return 0`, `break`)
 * @param ...               optional printf-style format string and arguments appended to the log
 */
#define LV_CHECK_ARG(cond, action_on_fail, ...)                                                \
    LV_CHECK_ARG_INTERNAL_(cond, #cond, action_on_fail, __VA_ARGS__)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CHECK_ARG_H*/

