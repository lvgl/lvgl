/**
 * @file lv_sprintf_rtthread.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"

#if LV_USE_STDLIB_SPRINTF == LV_STDLIB_RTTHREAD

#include <rtthread.h>
#include <stdarg.h>

#if LV_USE_FLOAT == 1
    #warning "lv_sprintf_rtthread: rtthread not support float in sprintf"
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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int lv_snprintf(char * buffer, size_t count, const char * format, ...)
{
    va_list va;
    va_start(va, format);
    const int ret = rt_vsnprintf(buffer, count, format, va);
    va_end(va);
    return ret;
}

int lv_vsnprintf(char * buffer, size_t count, const char * format, va_list va)
{
    return rt_vsnprintf(buffer, count, format, va);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_STDLIB_RTTHREAD*/
