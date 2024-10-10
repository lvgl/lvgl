/**
 * @file lv_demo_ebike_stats.h
 *
 */

#ifndef LV_DEMO_EBIKE_STATS_H
#define LV_DEMO_EBIKE_STATS_H

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
 * Initialize the statistics page. Called only once.
 */
void lv_demo_ebike_stats_init(void);

/**
 * Clean up the data of the statistics page
 */
void lv_demo_ebike_stats_deinit(void);

/**
 * Create the statistic page
 * @param parent    the parent of the statistics page
 */
void lv_demo_ebike_stats_create(lv_obj_t * parent);

#endif /*LV_USE_DEMO_EBIKE*/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_EBIKE_STATS_H*/
