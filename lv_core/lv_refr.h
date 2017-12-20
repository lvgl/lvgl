/**
 * @file lv_refr.h
 * 
 */

#ifndef LV_REFR_H
#define LV_REFR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include <stdbool.h>


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

/**
 * Initialize the screen refresh subsystem
 */
void lv_refr_init(void);

/**
 * Invalidate an area
 * @param area_p pointer to area which should be invalidated
 */
void lv_inv_area(const lv_area_t * area_p);

/**
 * Set a function to call after every refresh to announce the refresh time and the number of refreshed pixels
 * @param cb pointer to a callback function (void my_refr_cb(uint32_t time_ms, uint32_t px_num))
 */
void lv_refr_set_monitor_cb(void (*cb)(uint32_t, uint32_t));

/**********************
 *   STATIC FUNCTIONS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_REFR_H*/
