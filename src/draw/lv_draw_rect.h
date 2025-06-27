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

    /*Background img*/
    const void * bg_image_src;
    const void * bg_image_symbol_font;
    lv_color_t bg_image_recolor;
    lv_opa_t bg_image_opa;
    lv_opa_t bg_image_recolor_opa;
    uint8_t bg_image_tiled;
    /*Background*/
    lv_opa_t bg_opa;
    /*Border*/
    lv_opa_t border_opa;
    /*Outline */
    lv_opa_t outline_opa;
    /*Shadow*/
    lv_opa_t shadow_opa;

    /*Background*/
    lv_color_t bg_color;        /**< First element of a gradient is a color, so it maps well here*/
    lv_grad_dsc_t bg_grad;

    /*Border*/
    lv_color_t border_color;
    int32_t border_width;
    lv_border_side_t border_side : 5;
    uint8_t border_post : 1; /*The border will be drawn later*/

    /*Outline*/
    lv_color_t outline_color;
    int32_t outline_width;
    int32_t outline_pad;

    /*Shadow*/
    lv_color_t shadow_color;
    int32_t shadow_width;
    int32_t shadow_offset_x;
    int32_t shadow_offset_y;
    int32_t shadow_spread;
} lv_draw_rect_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    /**Radius, LV_RADIUS_CIRCLE for max. radius */
    int32_t radius;

    /**Opacity in 0...255 range.
     * LV_OPA_TRANSP, LV_OPA_10, LV_OPA_20, .. LV_OPA_COVER can be used as well*/
    lv_opa_t opa;

    /**The color of the rectangle.
     * If the gradient is set (grad.dir!=LV_GRAD_DIR_NONE) it's ignored. */
    lv_color_t color;

    /**Describe a gradient. If `grad.dir` is not `LV_GRAD_DIR_NONE` `color` will be ignored*/
    lv_grad_dsc_t grad;
} lv_draw_fill_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    /**Radius, LV_RADIUS_CIRCLE for max. radius */
    int32_t radius;

    /**The color of the border. */
    lv_color_t color;


    /**The width of the border in pixels */
    int32_t width;

    /**Opacity in 0...255 range.
     * LV_OPA_TRANSP, LV_OPA_10, LV_OPA_20, .. LV_OPA_COVER can be used as well*/
    lv_opa_t opa;

    /**LV_BORDER_SIDE_NONE/LEFT/RIGHT/TOP/BOTTOM/FULL.
     * LV_BORDER_SIDE_INTERNAL is an information for upper layers
     * and shouldn't be used here. */
    lv_border_side_t side : 5;

} lv_draw_border_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    /**Radius, LV_RADIUS_CIRCLE for max. radius */
    int32_t radius;

    /**Color of the the shadow */
    lv_color_t color;

    /**Width of the shadow. (radius of the blur)*/
    int32_t width;

    /**Make the rectangle larger with this value in all directions. Can be negative too. */
    int32_t spread;

    /**Offset the rectangle horizontally.*/
    int32_t ofs_x;

    /**Offset the rectangle vertically.*/
    int32_t ofs_y;

    /**Opacity in 0...255 range.
     * LV_OPA_TRANSP, LV_OPA_10, LV_OPA_20, .. LV_OPA_COVER can be used as well*/
    lv_opa_t opa;

    /**Set `bg_cover` to 1 if the background will cover the shadow.
     * It's a hint to the renderer about it might skip some masking.*/
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
 * Fill an area
 * @param layer         pointer to a layer
 * @param dsc           pointer to an initialized draw descriptor variable
 * @param coords        the coordinates of the rectangle
 */
void lv_draw_fill(lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

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
 * Draw a border
 * @param layer         pointer to a layer
 * @param dsc           pointer to an initialized draw descriptor variable
 * @param coords        the coordinates of the rectangle
 */
void lv_draw_border(lv_layer_t * layer, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);

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
 * Draw a box shadow
 * @param layer         pointer to a layer
 * @param dsc           pointer to an initialized draw descriptor variable
 * @param coords        the coordinates of the rectangle
 */
void lv_draw_box_shadow(lv_layer_t * layer, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);

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
