/**
 * @file lv_draw_rect.h
 *
 */

#ifndef LV_DRAW_RECT_H
#define LV_DRAW_RECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_core/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_coord_t radius;

    /*Background*/
    lv_color_t bg_color;
    lv_color_t bg_grad_color;
    lv_grad_dir_t bg_grad_dir;
    uint8_t bg_main_color_stop;
    uint8_t bg_grad_color_stop;
    lv_opa_t bg_opa;
    lv_blend_mode_t bg_blend_mode;

    /*Border*/
    lv_color_t border_color;
    lv_coord_t border_width;
    lv_border_side_t border_side;
    lv_opa_t border_opa;
    lv_blend_mode_t border_blend_mode;
    uint8_t border_post : 1;        /*There is a border it will be drawn later. */

    /*Outline*/
    lv_color_t outline_color;
    lv_coord_t outline_width;
    lv_coord_t outline_pad;
    lv_opa_t outline_opa;
    lv_blend_mode_t outline_blend_mode;

    /*Shadow*/
    lv_color_t shadow_color;
    lv_coord_t shadow_width;
    lv_coord_t shadow_ofs_x;
    lv_coord_t shadow_ofs_y;
    lv_coord_t shadow_spread;
    lv_opa_t shadow_opa;
    lv_blend_mode_t shadow_blend_mode;

    /*Content*/
    const void * content_src;
    const void * content_img;
    lv_align_t content_align;
    lv_coord_t content_ofs_x;
    lv_coord_t content_ofs_y;
    lv_opa_t content_opa;
    const lv_font_t * content_font;
    lv_opa_t content_recolor_opa;
    lv_blend_mode_t content_blend_mode;
    lv_color_t content_color;
    lv_coord_t content_letter_space;
    lv_coord_t content_line_space;
} lv_draw_rect_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_rect_dsc_init(lv_draw_rect_dsc_t * dsc);

//! @endcond

/**
 * Draw a rectangle
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_draw_rect(const lv_area_t * coords, const lv_area_t * mask, const lv_draw_rect_dsc_t * dsc);

/**
 * Draw a pixel
 * @param point the coordinates of the point to draw
 * @param mask the pixel will be drawn only in this mask
 * @param style pointer to a style
 */
void lv_draw_px(const lv_point_t * point, const lv_area_t * clip_area, const lv_style_t * style);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DRAW_RECT_H*/
