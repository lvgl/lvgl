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

    int32_t radius;

    /*Background*/
    lv_opa_t bg_opa;
    lv_color_t bg_color;        /**< First element of a gradient is a color, so it maps well here*/
    lv_grad_dsc_t bg_grad;

    /*Background img*/
    const void * bg_image_src;
    const void * bg_image_symbol_font;
    lv_color_t bg_image_recolor;
    lv_opa_t bg_image_opa;
    lv_opa_t bg_image_recolor_opa;
    uint8_t bg_image_tiled;

    /*Border*/
    lv_color_t border_color;
    int32_t border_width;
    lv_opa_t border_opa;
    lv_border_side_t border_side : 5;
    uint8_t border_post : 1; /*The border will be drawn later*/

    /*Outline*/
    lv_color_t outline_color;
    int32_t outline_width;
    int32_t outline_pad;
    lv_opa_t outline_opa;

    /*Shadow*/
    lv_color_t shadow_color;
    int32_t shadow_width;
    int32_t shadow_offset_x;
    int32_t shadow_offset_y;
    int32_t shadow_spread;
    lv_opa_t shadow_opa;
} lv_draw_rect_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    int32_t radius;

    lv_opa_t opa;
    lv_color_t color;
    lv_grad_dsc_t grad;
} lv_draw_fill_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    int32_t radius;

    lv_color_t color;
    int32_t width;
    lv_opa_t opa;
    lv_border_side_t side : 5;

} lv_draw_border_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    int32_t radius;

    lv_color_t color;
    int32_t width;
    int32_t spread;
    int32_t ofs_x;
    int32_t ofs_y;
    lv_opa_t opa;
    uint8_t bg_cover    : 1;
} lv_draw_box_shadow_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a rectangle draw descriptor.
 * @param dsc       pointer to a draw descriptor
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_rect_dsc_init(lv_draw_rect_dsc_t * dsc);

/**
 * Initialize a fill draw descriptor.
 * @param dsc       pointer to a draw descriptor
 */
void lv_draw_fill_dsc_init(lv_draw_fill_dsc_t * dsc);

/**
 * Try to get a fill draw descriptor from a draw task.
 * @param task      draw task
 * @return          the task's draw descriptor or NULL if the task is not of type LV_DRAW_TASK_TYPE_FILL
 */
lv_draw_fill_dsc_t * lv_draw_task_get_fill_dsc(lv_draw_task_t * task);

/**
 * Initialize a border draw descriptor.
 * @param dsc       pointer to a draw descriptor
 */
void lv_draw_border_dsc_init(lv_draw_border_dsc_t * dsc);

/**
 * Try to get a border draw descriptor from a draw task.
 * @param task      draw task
 * @return          the task's draw descriptor or NULL if the task is not of type LV_DRAW_TASK_TYPE_BORDER
 */
lv_draw_border_dsc_t * lv_draw_task_get_border_dsc(lv_draw_task_t * task);

/**
 * Initialize a box shadow draw descriptor.
 * @param dsc       pointer to a draw descriptor
 */
void lv_draw_box_shadow_dsc_init(lv_draw_box_shadow_dsc_t * dsc);

/**
 * Try to get a box shadow draw descriptor from a draw task.
 * @param task      draw task
 * @return          the task's draw descriptor or NULL if the task is not of type LV_DRAW_TASK_TYPE_BOX_SHADOW
 */
lv_draw_box_shadow_dsc_t * lv_draw_task_get_box_shadow_dsc(lv_draw_task_t * task);

/**
 * The rectangle is a wrapper for fill, border, bg. image and box shadow.
 * Internally fill, border, image and box shadow draw tasks will be created.
 * @param layer         pointer to a layer
 * @param dsc           pointer to an initialized draw descriptor variable
 * @param coords        the coordinates of the rectangle
 */
void lv_draw_rect(lv_layer_t * layer, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_RECT_H*/
