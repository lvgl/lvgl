/**
 * @file lv_theme_simple.h
 *
 */

#ifndef LV_THEME_SIMPLE_H
#define LV_THEME_SIMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_theme.h"
#include "../../display/lv_display.h"

#if LV_USE_THEME_SIMPLE

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
 * Initialize the theme
 * @param disp pointer to display to attach the theme
 * @return a pointer to reference this theme later
 */
lv_theme_t * lv_theme_simple_init(lv_display_t * disp);

/**
* Check if the theme is initialized
* @return true if default theme is initialized, false otherwise
*/
bool lv_theme_simple_is_inited(void);

/**
 * Get simple theme
 * @return a pointer to simple theme, or NULL if this is not initialized
 */
lv_theme_t * lv_theme_simple_get(void);

/**
 * Deinitialize the simple theme
 */
void lv_theme_simple_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_THEME_SIMPLE_H*/
