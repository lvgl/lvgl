/**
 * @file lv_draw_img.h
 *
 */

#ifndef LV_DRAW_IMAGE_H
#define LV_DRAW_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "lv_image_decoder.h"
#include "lv_draw_buf.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_color_t alpha_color;
    const lv_color32_t * palette;
    uint32_t palette_size   : 9;
} lv_draw_image_sup_t;

typedef struct _lv_draw_image_dsc_t {
    lv_draw_dsc_base_t base;

    const void * src;
    lv_image_header_t header;

    int32_t rotation;
    int32_t scale_x;
    int32_t scale_y;
    int32_t skew_x;
    int32_t skew_y;
    lv_point_t pivot;

    lv_color_t recolor;
    lv_opa_t recolor_opa;

    lv_opa_t opa;
    lv_blend_mode_t blend_mode : 4;

    uint16_t antialias          : 1;
    uint16_t tile               : 1;
    lv_draw_image_sup_t * sup;

    /** Might be used to indicate the original size of the image if only a small portion is rendered now.
     * Used when a part of a layer is rendered to show the total layer size*/
    lv_area_t original_area;
    const lv_image_dsc_t * bitmap_mask_src;
} lv_draw_image_dsc_t;

/**
 * PErform the actual rendering of a decoded image
 * @param draw_unit         pointer to a draw unit
 * @param draw_dsc          the draw descriptor of the image
 * @param decoder_dsc       pointer to the decoded image's descriptor
 * @param sup               supplementary data
 * @param img_coords        the absolute coordinates of the image
 * @param clipped_img_area  the absolute clip coordinates
 */
typedef void (*lv_draw_image_core_cb)(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                      const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                      const lv_area_t * img_coords, const lv_area_t * clipped_img_area);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize an image draw descriptor.
 * @param dsc       pointer to a draw descriptor
 */
void lv_draw_image_dsc_init(lv_draw_image_dsc_t * dsc);

/**
 * Try to get an image draw descriptor from a draw task.
 * @param task      draw task
 * @return          the task's draw descriptor or NULL if the task is not of type LV_DRAW_TASK_TYPE_IMAGE
 */
lv_draw_image_dsc_t * lv_draw_task_get_image_dsc(lv_draw_task_t * task);

/**
 * Create an image draw task
 * @param layer         pointer to a layer
 * @param dsc           pointer to an initialized draw descriptor
 * @param coords        the coordinates of the image
 */
void lv_draw_image(lv_layer_t * layer, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

/**
 * Create a draw task to blend a layer to an other layer
 * @param layer         pointer to a layer
 * @param dsc           pointer to an initialized draw descriptor
 * @param coords        the coordinates of the layer
 */
void lv_draw_layer(lv_layer_t * layer, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'lv_image_t' variable (image stored internally and compiled into the code)
 *  - a path to a file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. LV_SYMBOL_CLOSE)
 * @return type of the image source LV_IMAGE_SRC_VARIABLE/FILE/SYMBOL/UNKNOWN
 */
lv_image_src_t lv_image_src_get_type(const void * src);

/**
 * Can be used by draw units to handle the decoding and
 * prepare everything for the actual image rendering
 * @param draw_unit     pointer to a draw unit
 * @param draw_dsc      the draw descriptor of the image
 * @param coords        the absolute coordinates of the image
 * @param draw_core_cb  a callback to perform the actual rendering
 */
void _lv_draw_image_normal_helper(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                  const lv_area_t * coords, lv_draw_image_core_cb draw_core_cb);

/**
 * Can be used by draw units for TILED images to handle the decoding and
 * prepare everything for the actual image rendering
 * @param draw_unit     pointer to a draw unit
 * @param draw_dsc      the draw descriptor of the image
 * @param coords        the absolute coordinates of the image
 * @param draw_core_cb  a callback to perform the actual rendering
 */
void _lv_draw_image_tiled_helper(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
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
void _lv_image_buf_get_transformed_area(lv_area_t * res, int32_t w, int32_t h, int32_t angle,
                                        uint16_t scale_x, uint16_t scale_y, const lv_point_t * pivot);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_IMAGE_H*/
