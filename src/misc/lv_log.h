/**
 * @file lv_log.h
 *
 */

#ifndef LV_LOG_H
#define LV_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include <stdint.h>

#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

/*Possible log level. For compatibility declare it independently from `LV_USE_LOG`*/

#define LV_LOG_LEVEL_TRACE 0 /**< A lot of logs to give detailed information*/
#define LV_LOG_LEVEL_INFO  1 /**< Log important events*/
#define LV_LOG_LEVEL_WARN  2 /**< Log if something unwanted happened but didn't caused problem*/
#define LV_LOG_LEVEL_ERROR 3 /**< Only critical issue, when the system may fail*/
#define LV_LOG_LEVEL_USER  4 /**< Custom logs from the user*/
#define LV_LOG_LEVEL_NONE  5 /**< Do not log anything*/
#define _LV_LOG_LEVEL_NUM  6 /**< Number of log levels*/

LV_EXPORT_CONST_INT(LV_LOG_LEVEL_TRACE);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_INFO);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_WARN);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_ERROR);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_USER);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_NONE);

typedef int8_t lv_log_level_t;

#if LV_USE_LOG

#if LV_LOG_USE_FILE_LINE
#define LV_LOG_FILE __FILE__
#define LV_LOG_LINE __LINE__
#else
#define LV_LOG_FILE NULL
#define LV_LOG_LINE 0
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Log print function. Receives a string buffer to print".
 */
typedef void (*lv_log_print_g_cb_t)(lv_log_level_t level, const char * buf);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register custom print/write function to call when a log is added.
 * It can format its "File path", "Line number" and "Description" as required
 * and send the formatted log message to a console or serial port.
 * @param           print_cb a function pointer to print a log
 */
void lv_log_register_print_cb(lv_log_print_g_cb_t print_cb);

/**
 * Print a log message via `printf` if enabled with `LV_LOG_PRINTF` in `lv_conf.h`
 * and/or a print callback if registered with `lv_log_register_print_cb`
 * @param format    printf-like format string
 * @param ...       parameters for `format`
 */
void lv_log(const char * format, ...) LV_FORMAT_ATTRIBUTE(1, 2);

/**
 * Add a log
 * @param level     the level of log. (From `lv_log_level_t` enum)
 * @param file      name of the file when the log added
 * @param line      line number in the source code where the log added
 * @param func      name of the function when the log added
 * @param format    printf-like format string
 * @param ...       parameters for `format`
 */
void _lv_log_add(lv_log_level_t level, const char * file, int line,
                 const char * func, const char * format, ...) LV_FORMAT_ATTRIBUTE(5, 6);

/**********************
 *      MACROS
 **********************/
#ifndef LV_LOG_TRACE
#  if LV_LOG_LEVEL <= LV_LOG_LEVEL_TRACE
#    define LV_LOG_TRACE(...) _lv_log_add(LV_LOG_LEVEL_TRACE, LV_LOG_FILE, LV_LOG_LINE, __func__, __VA_ARGS__)
#  else
#    define LV_LOG_TRACE(...) do {}while(0)
#  endif
#endif

#ifndef LV_LOG_INFO
#  if LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO
#    define LV_LOG_INFO(...) _lv_log_add(LV_LOG_LEVEL_INFO, LV_LOG_FILE, LV_LOG_LINE, __func__, __VA_ARGS__)
#  else
#    define LV_LOG_INFO(...) do {}while(0)
#  endif
#endif

#ifndef LV_LOG_WARN
#  if LV_LOG_LEVEL <= LV_LOG_LEVEL_WARN
#    define LV_LOG_WARN(...) _lv_log_add(LV_LOG_LEVEL_WARN, LV_LOG_FILE, LV_LOG_LINE, __func__, __VA_ARGS__)
#  else
#    define LV_LOG_WARN(...) do {}while(0)
#  endif
#endif

#ifndef LV_LOG_ERROR
#  if LV_LOG_LEVEL <= LV_LOG_LEVEL_ERROR
#    define LV_LOG_ERROR(...) _lv_log_add(LV_LOG_LEVEL_ERROR, LV_LOG_FILE, LV_LOG_LINE, __func__, __VA_ARGS__)
#  else
#    define LV_LOG_ERROR(...) do {}while(0)
#  endif
#endif

#ifndef LV_LOG_USER
#  if LV_LOG_LEVEL <= LV_LOG_LEVEL_USER
#    define LV_LOG_USER(...) _lv_log_add(LV_LOG_LEVEL_USER, LV_LOG_FILE, LV_LOG_LINE, __func__, __VA_ARGS__)
#  else
#    define LV_LOG_USER(...) do {}while(0)
#  endif
#endif

#ifndef LV_LOG
#  if LV_LOG_LEVEL < LV_LOG_LEVEL_NONE
#    define LV_LOG(...) lv_log(__VA_ARGS__)
#  else
#    define LV_LOG(...) do {} while(0)
#  endif
#endif

#else /*LV_USE_LOG*/

/*Do nothing if `LV_USE_LOG 0`*/
#define _lv_log_add(level, file, line, ...)
#define LV_LOG_TRACE(...) do {}while(0)
#define LV_LOG_INFO(...) do {}while(0)
#define LV_LOG_WARN(...) do {}while(0)
#define LV_LOG_ERROR(...) do {}while(0)
#define LV_LOG_USER(...) do {}while(0)
#define LV_LOG(...) do {}while(0)

#endif /*LV_USE_LOG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LOG_H*/
