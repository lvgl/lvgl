/**
 * @file lv_log.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_log.h"
#if USE_LV_LOG

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
static void (*print_cb)(lv_log_level_t, const char *, uint32_t,  const char *);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register custom print (or anything else) function to call when log is added
 * @param f a function pointer:
 *          `void my_print (lv_log_level_t level, const char * file, uint32_t line, const char * dsc)`
 */
void lv_log_register_print(void f(lv_log_level_t, const char *, uint32_t,  const char *))
{
    print_cb = f;
}

/**
 * Add a log
 * @param level the level of log. (From `lv_log_level_t` enum)
 * @param file name of the file when the log added
 * @param line line number in the source code where the log added
 * @param dsc description of the log
 */
void lv_log_add(lv_log_level_t level, const char * file, uint32_t line, const char * dsc)
{
    if(level >= _LV_LOG_LEVEL_NUM) return;      /*Invalid level*/

    if(level >= LV_LOG_LEVEL) {

#if LV_LOG_PRINTF
        static const char * lvl_prefix[] = {"Trace", "Info", "Warn", "Error"};
        printf("%s: %s \t(%s #%d)\n", lvl_prefix[level], dsc,  file, line);
#else
        if(print_cb) print_cb(level, file, line, dsc);
#endif
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_LOG*/
