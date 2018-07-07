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
#include "../../lv_conf.h"
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Possible log level. For compatibility declare it independently from `USE_LV_LOG`*/
typedef enum
{
	LV_LOG_LEVEL_INFO = 0,
	LV_LOG_LEVEL_WARN,
	LV_LOG_LEVEL_ERROR,
	LV_LOG_LEVEL_USER,
	_LV_LOG_LEVEL_NUM
}lv_log_level_t;

#if USE_LV_LOG

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register custom print (or anything else) function to call when log is added
 * @param f a function pointer:
 *          `void my_print (lv_log_level_t level, const char * file, uint32_t line, const char * dsc)`
 */
void lv_log_register_print(void f(lv_log_level_t , const char *, uint32_t ,  const char *));

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

#else /*USE_LV_LOG*/

/*Do nothing if `USE_LV_LOG  0`*/
#define lv_log_add(level, file, line, dsc) {;}

#endif /*USE_LV_LOG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LOG_H*/
