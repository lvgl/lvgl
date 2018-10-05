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
#include "../../lv_conf.h"
#endif
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/*Possible log level. For compatibility declare it independently from `USE_LV_LOG`*/

#define LV_LOG_LEVEL_TRACE 0     /*A lot of logs to give detailed information*/
#define LV_LOG_LEVEL_INFO  1     /*Log important events*/
#define LV_LOG_LEVEL_WARN  2     /*Log if something unwanted happened but didn't caused problem*/
#define LV_LOG_LEVEL_ERROR 3     /*Only critical issue, when the system may fail*/
#define _LV_LOG_LEVEL_NUM  4

typedef uint8_t lv_log_level_t;

#if USE_LV_LOG
/**********************
 *      TYPEDEFS
 **********************/


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register custom print (or anything else) function to call when log is added
 * @param f a function pointer:
 *          `void my_print (lv_log_level_t level, const char * file, uint32_t line, const char * dsc)`
 */
void lv_log_register_print(void f(lv_log_level_t, const char *, uint32_t,  const char *));

/**
 * Add a log
 * @param level the level of log. (From `lv_log_level_t` enum)
 * @param file name of the file when the log added
 * @param line line number in the source code where the log added
 * @param dsc description of the log
 */
void lv_log_add(lv_log_level_t level, const char * file, uint32_t line, const char * dsc);

/**********************
 *      MACROS
 **********************/

#define LV_LOG_TRACE(dsc)   lv_log_add(LV_LOG_LEVEL_TRACE, __FILE__, __LINE__, dsc);
#define LV_LOG_INFO(dsc)    lv_log_add(LV_LOG_LEVEL_INFO, __FILE__, __LINE__, dsc);
#define LV_LOG_WARN(dsc)    lv_log_add(LV_LOG_LEVEL_WARN, __FILE__, __LINE__, dsc);
#define LV_LOG_ERROR(dsc)   lv_log_add(LV_LOG_LEVEL_ERROR, __FILE__, __LINE__, dsc);

#else /*USE_LV_LOG*/

/*Do nothing if `USE_LV_LOG  0`*/
#define lv_log_add(level, file, line, dsc) {;}
#define LV_LOG_TRACE(dsc) {;}
#define LV_LOG_INFO(dsc) {;}
#define LV_LOG_WARN(dsc) {;}
#define LV_LOG_ERROR(dsc) {;}
#endif /*USE_LV_LOG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LOG_H*/
