/**
 * @file lv_demo_ebike_settings.h
 *
 */

#ifndef LV_DEMO_EBIKE_SETTINGS_H
#define LV_DEMO_EBIKE_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_ebike.h"

#if LV_USE_DEMO_EBIKE

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
 * Create the settings page
 * @param parent    the parent of the settings page
 */
void lv_demo_ebike_settings_create(lv_obj_t * parent);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_EBIKE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_EBIKE_SETTINGS_H*/
