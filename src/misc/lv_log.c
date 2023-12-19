/**
 * @file lv_log.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_log.h"
#if LV_USE_LOG

#include <stdarg.h>
#include <string.h>
#include "../stdlib/lv_sprintf.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"
#include "../tick/lv_tick.h"
#include "../core/lv_global.h"

#if LV_LOG_PRINTF
    #include <stdio.h>
#endif

/*********************
 *      DEFINES
 *********************/
#if LV_LOG_USE_TIMESTAMP
    #define last_log_time LV_GLOBAL_DEFAULT()->log_last_log_time
#endif
#define custom_print_cb LV_GLOBAL_DEFAULT()->custom_log_print_cb

#if LV_LOG_USE_TIMESTAMP
    #define LOG_TIMESTAMP_FMT  "\t(%" LV_PRIu32 ".%03" LV_PRIu32 ", +%" LV_PRIu32 ")\t"
    #define LOG_TIMESTAMP_EXPR t / 1000, t % 1000, t - last_log_time,
#else
    #define LOG_TIMESTAMP_FMT
    #define LOG_TIMESTAMP_EXPR
#endif

#if LV_LOG_USE_FILE_LINE
    #define LOG_FILE_LINE_FMT " %s:%d"
    #define LOG_FILE_LINE_EXPR , &file[p], line
#else
    #define LOG_FILE_LINE_FMT
    #define LOG_FILE_LINE_EXPR
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_log_register_print_cb(lv_log_print_g_cb_t print_cb)
{
    custom_print_cb = print_cb;
}

void _lv_log_add(lv_log_level_t level, const char * file, int line, const char * func, const char * format, ...)
{
    if(level >= _LV_LOG_LEVEL_NUM) return; /*Invalid level*/

    if(level >= LV_LOG_LEVEL) {
        va_list args;
        va_start(args, format);

#if LV_LOG_USE_FILE_LINE
        /*Use only the file name not the path*/
        size_t p;
        for(p = lv_strlen(file); p > 0; p--) {
            if(file[p] == '/' || file[p] == '\\') {
                p++;    /*Skip the slash*/
                break;
            }
        }
#else
        LV_UNUSED(file);
        LV_UNUSED(line);
#endif

#if LV_LOG_USE_TIMESTAMP
        uint32_t t = lv_tick_get();
#endif
        static const char * lvl_prefix[] = {"Trace", "Info", "Warn", "Error", "User"};

#if LV_LOG_PRINTF
        printf("[%s]" LOG_TIMESTAMP_FMT " %s: ",
               lvl_prefix[level], LOG_TIMESTAMP_EXPR func);
        vprintf(format, args);
        printf(LOG_FILE_LINE_FMT "\n" LOG_FILE_LINE_EXPR);
#else
        if(custom_print_cb) {
            char buf[512];
            char msg[256];
            lv_vsnprintf(msg, sizeof(msg), format, args);
            lv_snprintf(buf, sizeof(buf), "[%s]" LOG_TIMESTAMP_FMT " %s: %s" LOG_FILE_LINE_FMT "\n",
                        lvl_prefix[level], LOG_TIMESTAMP_EXPR func, msg LOG_FILE_LINE_EXPR);
            custom_print_cb(level, buf);
        }
#endif

#if LV_LOG_USE_TIMESTAMP
        last_log_time = t;
#endif
        va_end(args);
    }
}

void lv_log(const char * format, ...)
{
    if(LV_LOG_LEVEL >= LV_LOG_LEVEL_NONE) return; /* disable log */

    va_list args;
    va_start(args, format);

#if LV_LOG_PRINTF
    vprintf(format, args);
#else
    if(custom_print_cb) {
        char buf[512];
        lv_vsnprintf(buf, sizeof(buf), format, args);
        custom_print_cb(LV_LOG_LEVEL_USER, buf);
    }
#endif

    va_end(args);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_LOG*/
