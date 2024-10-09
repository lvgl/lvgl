/**
 * @file lv_demo_ebike_home.h
 *
 */

#ifndef LV_DEMO_EBIKE_HOME_H
#define LV_DEMO_EBIKE_HOME_H

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
 * Initialize the home page. Called only once.
 */
void lv_demo_ebike_home_init(void);

/**
 * Clean up the data of the home pages
 */
void lv_demo_ebike_home_deinit(void);

/**
 * Create the home page
 * @param parent    the parent of the home page
 */
void lv_demo_ebike_home_create(lv_obj_t * parent);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_EBIKE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_EBIKE_HOME_H*/
