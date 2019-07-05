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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/*Possible log level. For compatibility declare it independently from `LV_USE_LOG`*/

#define LV_LOG_LEVEL_TRACE 0 /**< A lot of logs to give detailed information*/
#define LV_LOG_LEVEL_INFO 1  /**< Log important events*/
#define LV_LOG_LEVEL_WARN 2  /**< Log if something unwanted happened but didn't caused problem*/
#define LV_LOG_LEVEL_ERROR 3 /**< Only critical issue, when the system may fail*/
#define LV_LOG_LEVEL_NONE 4 /**< Do not log anything*/
#define _LV_LOG_LEVEL_NUM 5 /**< Number of log levels */

typedef int8_t lv_log_level_t;

#if LV_USE_LOG
/**********************
 *      TYPEDEFS
 **********************/

/**
 * Log print function. Receives "Log Level", "File path", "Line number" and "Description".
 */
typedef void (*lv_log_print_g_cb_t)(lv_log_level_t level, const char *, uint32_t, const char *);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register custom print/write function to call when a log is added.
 * It can format its "File path", "Line number" and "Description" as required
 * and send the formatted log message to a consol or serial port.
 * @param print_cb a function pointer to print a log
 */
void lv_log_register_print_cb(lv_log_print_g_cb_t print_cb);

/**
 * Add a log
 * @param level the level of log. (From `lv_log_level_t` enum)
 * @param file name of the file when the log added
 * @param line line number in the source code where the log added
 * @param dsc description of the log
 */
void lv_log_add(lv_log_level_t level, const char * file, int line, const char * dsc);

/**********************
 *      MACROS
 **********************/

#if LV_LOG_LEVEL <= LV_LOG_LEVEL_TRACE
#define LV_LOG_TRACE(dsc) lv_log_add(LV_LOG_LEVEL_TRACE, __FILE__, __LINE__, dsc);
#else
#define LV_LOG_TRACE(dsc)                                                                                              \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#endif

#if LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO
#define LV_LOG_INFO(dsc) lv_log_add(LV_LOG_LEVEL_INFO, __FILE__, __LINE__, dsc);
#else
#define LV_LOG_INFO(dsc)                                                                                               \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#endif

#if LV_LOG_LEVEL <= LV_LOG_LEVEL_WARN
#define LV_LOG_WARN(dsc) lv_log_add(LV_LOG_LEVEL_WARN, __FILE__, __LINE__, dsc);
#else
#define LV_LOG_WARN(dsc)                                                                                               \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#endif

#if LV_LOG_LEVEL <= LV_LOG_LEVEL_ERROR
#define LV_LOG_ERROR(dsc) lv_log_add(LV_LOG_LEVEL_ERROR, __FILE__, __LINE__, dsc);
#else
#define LV_LOG_ERROR(dsc)                                                                                              \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#endif

#else /*LV_USE_LOG*/

/*Do nothing if `LV_USE_LOG  0`*/
#define lv_log_add(level, file, line, dsc)                                                                             \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#define LV_LOG_TRACE(dsc)                                                                                              \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#define LV_LOG_INFO(dsc)                                                                                               \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#define LV_LOG_WARN(dsc)                                                                                               \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#define LV_LOG_ERROR(dsc)                                                                                              \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#endif /*LV_USE_LOG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LOG_H*/
