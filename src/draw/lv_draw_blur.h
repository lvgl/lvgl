/**
 * @file lv_draw_blur.h
 *
 */

#ifndef LV_DRAW_BLUR_H
#define LV_DRAW_BLUR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_dsc_base_t base;

    /**
     * The intensity of blur.
     */
    int32_t blur_radius;

    /**
     * The corner radius of the blurred area
     */
    int32_t corner_radius;

    /**
     * Sets whether to prefer speed or precision
     */
    lv_blur_quality_t quality;

} lv_draw_blur_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a blur draw descriptor
 * @param dsc       pointer to a draw descriptor
 */
void lv_draw_blur_dsc_init(lv_draw_blur_dsc_t * dsc);

/**
 * Try to get a blur draw descriptor from a draw task.
 * @param task      draw task
 * @return          the task's draw descriptor or NULL if the task is not of type LV_DRAW_TASK_TYPE_BLUR
 */
lv_draw_blur_dsc_t * lv_draw_task_get_blur_dsc(lv_draw_task_t * task);

/**
 * Create a blur draw task
 * @param layer     pointer to a layer
 * @param dsc       pointer to an initialized `lv_draw_blur_dsc_t` variable
 * @param coords    coordinates of the character
 */
void lv_draw_blur(lv_layer_t * layer, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BLUR_H*/
