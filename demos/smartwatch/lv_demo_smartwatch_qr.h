/**
 * @file lv_demo_smartwatch_qr.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_QR_H
#define LV_DEMO_SMARTWATCH_QR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"

#if LV_USE_DEMO_SMARTWATCH

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
 * Create the qr page. Called only once.
 */
void lv_demo_smartwatch_qr_create(void);

/**
 * Load the qr screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_qr_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Clear the qr codes in list
 */
void lv_demo_smartwatch_qr_list_clear(void);

/**
 * Add qr codes to the list
 * @param id        determines the icon and name of the qr from inbuilt list. Value (0-9)
 * @param link      link to be added as a qr code
 */
void lv_demo_smartwatch_qr_list_add(uint8_t id, const char * link);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_HOME_H*/
