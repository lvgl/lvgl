/**
 * @file lv_log.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_log.h"
#if LV_USE_LOG

#if LV_LOG_PRINTF
#include <stdio.h>
#endif
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_log_print_g_cb_t custom_print_cb;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register custom print/write function to call when a log is added.
 * It can format its "File path", "Line number" and "Description" as required
 * and send the formatted log message to a consol or serial port.
 * @param print_cb a function pointer to print a log
 */
void lv_log_register_print_cb(lv_log_print_g_cb_t print_cb)
{
    custom_print_cb = print_cb;
}

/**
 * Add a log
 * @param level the level of log. (From `lv_log_level_t` enum)
 * @param file name of the file when the log added
 * @param line line number in the source code where the log added
 * @param dsc description of the log
 */
void lv_log_add(lv_log_level_t level, const char * file, int line, const char * dsc)
{
    if(level >= _LV_LOG_LEVEL_NUM) return; /*Invalid level*/

    if(level >= LV_LOG_LEVEL) {

#if LV_LOG_PRINTF
        static const char * lvl_prefix[] = {"Trace", "Info", "Warn", "Error"};
        printf("%s: %s \t(%s #%d)\n", lvl_prefix[level], dsc, file, line);
#else
        if(custom_print_cb) custom_print_cb(level, file, line, dsc);
#endif
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_LOG*/
