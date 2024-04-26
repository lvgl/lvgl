/**
 * @file lv_draw_rect_private.h
 *
 */

#ifndef LV_DRAW_RECT_PRIVATE_H
#define LV_DRAW_RECT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_rect.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_draw_fill_dsc_t {
    lv_draw_dsc_base_t base;

    int32_t radius;

    lv_opa_t opa;
    lv_color_t color;
    lv_grad_dsc_t grad;
};

struct lv_draw_border_dsc_t {
    lv_draw_dsc_base_t base;

    int32_t radius;

    lv_color_t color;
    int32_t width;
    lv_opa_t opa;
    lv_border_side_t side : 5;

};

struct lv_draw_box_shadow_dsc_t {
    lv_draw_dsc_base_t base;

    int32_t radius;

    lv_color_t color;
    int32_t width;
    int32_t spread;
    int32_t ofs_x;
    int32_t ofs_y;
    lv_opa_t opa;
    uint8_t bg_cover    : 1;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_RECT_PRIVATE_H*/
