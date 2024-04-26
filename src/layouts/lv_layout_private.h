/**
 * @file lv_layout_private.h
 *
 */

#ifndef LV_LAYOUT_PRIVATE_H
#define LV_LAYOUT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_layout.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_layout_init(void);

void lv_layout_deinit(void);

/**
 * Update the layout of a widget
 * @param obj   pointer to a widget
 */
void lv_layout_apply(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LAYOUT_PRIVATE_H*/
