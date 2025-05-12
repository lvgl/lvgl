/**
 * @file lv_draw_line.h
 *
 */

#ifndef LV_DRAW_LINE_H
#define LV_DRAW_LINE_H

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

    /**The first point of the line. If `LV_USE_FLOAT` is enabled float number can be also used*/
    lv_point_precise_t p1;

    /**The second point of the line. If `LV_USE_FLOAT` is enabled float number can be also used*/
    lv_point_precise_t p2;

    /**The color of the line*/
    lv_color_t color;

    /**The width (thickness) of the line*/
    int32_t width;

    /** The length of a dash (0: don't dash)*/
    int32_t dash_width;

    /** The length of the gaps between dashes (0: don't dash)*/
    int32_t dash_gap;

    /**Opacity of the line in 0...255 range.
     * LV_OPA_TRANSP, LV_OPA_10, LV_OPA_20, .. LV_OPA_COVER can be used as well*/
    lv_opa_t opa;

    /**Make the line start rounded*/
    uint8_t round_start : 1;

    /**Make the line end rounded*/
    uint8_t round_end   : 1;

    /**1: Do not bother with line ending (if it's not visible for any reason) */
    uint8_t raw_end     : 1;
} lv_draw_line_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a line draw descriptor
 * @param dsc       pointer to a draw descriptor
 */
void lv_draw_line_dsc_init(lv_draw_line_dsc_t * dsc);

/**
 * Try to get a line draw descriptor from a draw task.
 * @param task      draw task
 * @return          the task's draw descriptor or NULL if the task is not of type LV_DRAW_TASK_TYPE_LINE
 */
lv_draw_line_dsc_t * lv_draw_task_get_line_dsc(lv_draw_task_t * task);

/**
 * Create a line draw task
 * @param layer     pointer to a layer
 * @param dsc       pointer to an initialized `lv_draw_line_dsc_t` variable
 */
void lv_draw_line(lv_layer_t * layer, const lv_draw_line_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_LINE_H*/
