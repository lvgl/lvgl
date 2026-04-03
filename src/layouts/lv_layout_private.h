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

#include "../lvgl_public.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_layout_callbacks_t callbacks;
    void * user_data;
} lv_layout_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_layout_init(void);

void lv_layout_deinit(void);

/**
 * Update the layout of a widget
 * @param obj   pointer to a widget
 */
void lv_layout_update_children_positions(lv_obj_t * obj);

/**
 * TODO
 * @param obj
 * @param iteration
 */
void lv_layout_update_children_sizes(lv_obj_t * obj, int32_t iteration);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LAYOUT_PRIVATE_H*/
