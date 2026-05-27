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
#include "../config/lv_conf_internal.h"
#include "../logging/lv_log.h"
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

/*----------------------------------------------------------------------
 * Internal helper: assert handler
 * Expands to LV_ASSERT_HANDLER when LV_CHECK_ARG_ASSERT_ON_FAIL is set,
 * otherwise to a no-op.
 *---------------------------------------------------------------------*/
#  if LV_CHECK_ARG_ASSERT_ON_FAIL
#    define LV_CHECK_ARG_ASSERT_HANDLER_ LV_ASSERT_HANDLER
#  else
#    define LV_CHECK_ARG_ASSERT_HANDLER_ do {} while(0)
#  endif

/*----------------------------------------------------------------------
 * Internal helper: log output
 * Controlled by LV_CHECK_ARG_LOG_MODE.
 * In all modes the macro accepts (cond_str, ...) so that
 * LV_CHECK_ARG_INTERNAL_ can call it uniformly.
 *---------------------------------------------------------------------*/
#  if LV_CHECK_ARG_LOG_MODE == LV_CHECK_ARG_LOG_MODE_VERBOSE
#    define LV_CHECK_ARG_LOG_(cond_str, ...) LV_LOG_WARN("Check failed: " cond_str " " __VA_ARGS__)
#  elif LV_CHECK_ARG_LOG_MODE == LV_CHECK_ARG_LOG_MODE_MINIMAL
#    define LV_CHECK_ARG_LOG_(cond_str, ...) LV_LOG_WARN("Check failed")
#  else /* LV_CHECK_ARG_LOG_MODE_NONE */
#    define LV_CHECK_ARG_LOG_(cond_str, ...) do {} while(0)
#  endif

/*----------------------------------------------------------------------
 * Internal macro: single definition, behaviour driven by the helpers above.
 * Do not use directly; use LV_CHECK_ARG instead.
 *---------------------------------------------------------------------*/
#  define LV_CHECK_ARG_INTERNAL_(cond, cond_str, action_on_fail, ...)  \
    if(LV_UNLIKELY(!(cond))) {                                          \
        LV_CHECK_ARG_LOG_(cond_str, __VA_ARGS__);                       \
        LV_CHECK_ARG_ASSERT_HANDLER_;                                   \
        action_on_fail;                                                 \
    } else {}

#else

/** LV_CHECK_ARG is disabled; all checks compile to nothing. */
#  define LV_CHECK_ARG_INTERNAL_(cond, cond_str, action_on_fail, ...) ((void)0)

#endif /*LV_USE_CHECK_ARG*/

/**
 * Check that a condition is true. If the condition is false, log a warning
 * and execute `action_on_fail` (e.g. `return`, `return val`, `break`).
 * Additional printf-style arguments are appended to the log message when
 * LV_CHECK_ARG_LOG_MODE is set to VERBOSE.
 *
 * Can be disabled entirely by setting LV_USE_CHECK_ARG to 0 in lv_conf.h.
 * If LV_CHECK_ARG_ASSERT_ON_FAIL is 1, LV_ASSERT_HANDLER is also invoked
 * before the action.
 * Log output requires LV_USE_LOG to be enabled; if LV_USE_LOG is 0 no
 * output is produced regardless of LV_CHECK_ARG_LOG_MODE.
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
 *                          (only used when LV_CHECK_ARG_LOG_MODE == LV_CHECK_ARG_LOG_MODE_VERBOSE)
 */
#define LV_CHECK_ARG(cond, action_on_fail, ...)                                                \
    LV_CHECK_ARG_INTERNAL_(cond, #cond, action_on_fail, __VA_ARGS__)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CHECK_ARG_H*/

