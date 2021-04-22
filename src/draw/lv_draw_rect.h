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
#include "lv_draw_blend.h"
#include "../font/lv_font.h"

/*********************
 *      DEFINES
 *********************/
#define LV_RADIUS_CIRCLE 0x7FFF /**< A very big radius to always draw as circle*/
LV_EXPORT_CONST_INT(LV_RADIUS_CIRCLE);


/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_coord_t radius;
    lv_blend_mode_t blend_mode;

    /*Background*/
    lv_color_t bg_color;
    lv_color_t bg_grad_color;
    uint8_t bg_main_color_stop;
    uint8_t bg_grad_color_stop;
    lv_opa_t bg_opa;
    lv_grad_dir_t bg_grad_dir :3;

    /*Background img*/
    const void * bg_img_src;
    const void * bg_img_symbol_font;
    lv_color_t bg_img_recolor;
    lv_opa_t bg_img_opa;
    lv_opa_t bg_img_recolor_opa;
    uint8_t bg_img_tiled;

    /*Border*/
    lv_color_t border_color;
    lv_coord_t border_width;
    lv_opa_t border_opa;
    uint8_t border_post : 1;        /*There is a border it will be drawn later.*/
    lv_border_side_t border_side :5;

    /*Outline*/
    lv_color_t outline_color;
    lv_coord_t outline_width;
    lv_coord_t outline_pad;
    lv_opa_t outline_opa;

    /*Shadow*/
    lv_color_t shadow_color;
    lv_coord_t shadow_width;
    lv_coord_t shadow_ofs_x;
    lv_coord_t shadow_ofs_y;
    lv_coord_t shadow_spread;
    lv_opa_t shadow_opa;
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
//void lv_draw_px(const lv_point_t * point, const lv_area_t * clip_area, const lv_style_t * style);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_RECT_H*/
