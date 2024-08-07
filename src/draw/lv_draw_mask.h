/**
 * @file lv_draw_mask.h
 *
 */

#ifndef LV_DRAW_MASK_H
#define LV_DRAW_MASK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a rectangle mask draw descriptor.
 * @param dsc       pointer to a draw descriptor
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_mask_rect_dsc_init(lv_draw_mask_rect_dsc_t * dsc);

/**
 * Try to get a rectangle mask draw descriptor from a draw task.
 * @param task      draw task
 * @return          the task's draw descriptor or NULL if the task is not of type LV_DRAW_TASK_TYPE_MASK_RECTANGLE
 */
lv_draw_mask_rect_dsc_t * lv_draw_task_get_mask_rect_dsc(lv_draw_task_t * task);

/**
 * Create a draw task to mask a rectangle from the buffer
 * @param layer     pointer to a layer
 * @param dsc       pointer to a draw descriptor
 */
void lv_draw_mask_rect(lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_MASK_H*/
