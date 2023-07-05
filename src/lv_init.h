/**
 * @file lv_init.h
 *
 */

#ifndef LV_INIT_H
#define LV_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "lv_conf_internal.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize LVGL library.
 * Should be called before any other LVGL related function.
 */
void lv_init(void);


#if LV_ENABLE_GC || LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN

/**
 * Deinit the 'lv' library
 * Currently only implemented when not using custom allocators, or GC is enabled.
 */
void lv_deinit(void);

#endif

/**
 * Returns whether the 'lv' library is currently initialized
 */
bool lv_is_initialized(void);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_INIT_H*/
