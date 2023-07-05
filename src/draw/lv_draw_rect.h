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
#include "lv_draw.h"
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
    lv_draw_dsc_base_t base;

    lv_coord_t radius;

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
    lv_border_side_t border_side : 5;
    uint8_t border_post : 1; /*The border will be drawn later*/

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

typedef struct {
    lv_draw_dsc_base_t base;

    lv_coord_t radius;

    lv_opa_t opa;
    lv_color_t color;
    lv_grad_dsc_t grad;
} lv_draw_fill_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    lv_coord_t radius;

    const void * src;
    const void * font;
    lv_color_t recolor;
    lv_opa_t opa;
    lv_opa_t recolor_opa;
    lv_img_header_t img_header; /*To make it easier for draw_unit to decide if they can draw this image */
    uint8_t tiled : 1;
} lv_draw_bg_img_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    lv_coord_t radius;

    lv_color_t color;
    lv_coord_t width;
    lv_opa_t opa;
    lv_border_side_t side : 5;

} lv_draw_border_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    lv_coord_t radius;

    lv_color_t color;
    lv_coord_t width;
    lv_coord_t spread;
    lv_coord_t ofs_x;
    lv_coord_t ofs_y;
    lv_opa_t opa;
    uint8_t bg_cover    : 1;
} lv_draw_box_shadow_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_rect_dsc_init(lv_draw_rect_dsc_t * dsc);

void lv_draw_fill_dsc_init(lv_draw_fill_dsc_t * dsc);

void lv_draw_border_dsc_init(lv_draw_border_dsc_t * dsc);

void lv_draw_box_shadow_dsc_init(lv_draw_box_shadow_dsc_t * dsc);

void lv_draw_bg_img_dsc_init(lv_draw_bg_img_dsc_t * dsc);


/**
 * Draw a rectangle
 * @param draw_ctx      pointer to the current draw context
 * @param dsc           pointer to an initialized `lv_draw_rect_dsc_t` variable
 * @param coords        the coordinates of the rectangle
 */
void lv_draw_rect(struct _lv_layer_t * layer, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_RECT_H*/
