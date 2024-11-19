/**
 * @file lv_draw_image_private.h
 *
 */

#ifndef LV_DRAW_IMAGE_PRIVATE_H
#define LV_DRAW_IMAGE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_image.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_image_sup_t {
    lv_color_t alpha_color;
    const lv_color32_t * palette;
    uint32_t palette_size   : 9;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Can be used by draw units to handle the decoding and
 * prepare everything for the actual image rendering
 * @param draw_unit     pointer to a draw unit
 * @param draw_dsc      the draw descriptor of the image
 * @param coords        the absolute coordinates of the image
 * @param draw_core_cb  a callback to perform the actual rendering
 */
void lv_draw_image_normal_helper(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                 const lv_area_t * coords, lv_draw_image_core_cb draw_core_cb);

/**
 * Can be used by draw units for TILED images to handle the decoding and
 * prepare everything for the actual image rendering
 * @param draw_unit     pointer to a draw unit
 * @param draw_dsc      the draw descriptor of the image
 * @param coords        the absolute coordinates of the image
 * @param draw_core_cb  a callback to perform the actual rendering
 */
void lv_draw_image_tiled_helper(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                const lv_area_t * coords, lv_draw_image_core_cb draw_core_cb);

/**
 * Get the area of a rectangle if its rotated and scaled
 * @param res store the coordinates here
 * @param w width of the rectangle to transform
 * @param h height of the rectangle to transform
 * @param angle angle of rotation
 * @param scale_x zoom in x direction, (256 no zoom)
 * @param scale_y zoom in y direction, (256 no zoom)
 * @param pivot x,y pivot coordinates of rotation
 */
void lv_image_buf_get_transformed_area(lv_area_t * res, int32_t w, int32_t h, int32_t angle,
                                       uint16_t scale_x, uint16_t scale_y, const lv_point_t * pivot);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_IMAGE_PRIVATE_H*/
