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

    lv_point_precise_t p1;
    lv_point_precise_t p2;
    lv_color_t color;
    int32_t width;
    int32_t dash_width;
    int32_t dash_gap;
    lv_opa_t opa;
    lv_blend_mode_t blend_mode  : 2;
    uint8_t round_start : 1;
    uint8_t round_end   : 1;
    uint8_t raw_end     : 1;    /*Do not bother with perpendicular line ending if it's not visible for any reason*/
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
