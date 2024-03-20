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
#include "../lv_conf_internal.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"
#include "sw/lv_draw_sw_gradient.h"

/*********************
 *      DEFINES
 *********************/
#define LV_RADIUS_CIRCLE        0x7FFF /**< A very big radius to always draw as circle*/
LV_EXPORT_CONST_INT(LV_RADIUS_CIRCLE);

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_coord_t radius;
    lv_blend_mode_t blend_mode;

    /*Background*/
    lv_opa_t bg_opa;
    lv_color_t bg_color;        /**< First element of a gradient is a color, so it maps well here*/
    lv_grad_dsc_t bg_grad;

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
    lv_border_side_t border_side : 5;

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

struct _lv_draw_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_rect_dsc_init(lv_draw_rect_dsc_t * dsc);

/**
 * Draw a rectangle
 * @param coords the coordinates of the rectangle
 * @param clip the rectangle will be drawn only in this area
 * @param dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_draw_rect(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_RECT_H*/
