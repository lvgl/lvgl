/**
 * @file lv_draw_image.h
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

struct _lv_draw_image_dsc_t {
    lv_draw_dsc_base_t base;

    /**The image source: pointer to `lv_image_dsc_t` or a path to a file*/
    const void * src;

    /**The header of the image. Initialized internally in `lv_draw_image` */
    lv_image_header_t header;

    /**Clip the corner of the image with this radius. Use `LV_RADIUS_CIRCLE` for max. radius */
    int32_t clip_radius;

    /**The rotation of the image in 0.1 degree unit. E.g. 234 means 23.4° */
    int32_t rotation;

    /**Horizontal scale (zoom) of the image.
     * 256 (LV_SCALE_NONE): means no zoom, 512 double size, 128 half size.*/
    int32_t scale_x;

    /**Same as `scale_y` but vertically*/
    int32_t scale_y;

    /**Parallelogram like transformation of the image horizontally in 0.1 degree unit. E.g. 456 means 45.6°.*/
    int32_t skew_x;

    /**Same as `skew_x` but vertically*/
    int32_t skew_y;

    /**The pivot point of transformation (scale and rotation).
     * 0;0 is the top left corner of the image. Can be outside of the image too.*/
    lv_point_t pivot;

    /**Mix this color to the images. In case of `LV_COLOR_FORMAT_A8` it will be the color of the visible pixels*/
    lv_color_t recolor;

    /**The intensity of recoloring. 0 means, no recolor, 255 means full cover (transparent pixels remain transparent)*/
    lv_opa_t recolor_opa;

    /**Opacity in 0...255 range.
     * LV_OPA_TRANSP, LV_OPA_10, LV_OPA_20, .. LV_OPA_COVER can be used as well*/
    lv_opa_t opa;

    /**Describes how to blend the pixels of the image to the background.
     * See `lv_blend_mode_t` for more details.
     */
    lv_blend_mode_t blend_mode : 4;

    /**1: perform the transformation with anti-alaising */
    uint16_t antialias          : 1;

    /**If the image is smaller than the `image_area` field of `lv_draw_image_dsc_t`
     * tile the image (repeat is both horizontally and vertically) to fill the
     * `image_area` area*/
    uint16_t tile               : 1;

    /**Used internally to store some information about the palette or the color of A8 images*/
    lv_draw_image_sup_t * sup;

    /** Used to indicate the entire original, non-clipped area where the image is to be drawn.
     * This is important for:
     *  1. Layer rendering, where it might happen that only a smaller area of the layer is rendered and e.g.
     *     `clip_radius` needs to know what the original image was.
     *  2. Tiled images, where the target draw area is larger than the image to be tiled.
     */
    lv_area_t image_area;

    /**Pointer to an A8 or L8 image descriptor to mask the image with.
     * The mask is always center aligned. */
    const lv_image_dsc_t * bitmap_mask_src;
};

/**
 * PErform the actual rendering of a decoded image
 * @param t                 pointer to a draw task
 * @param draw_dsc          the draw descriptor of the image
 * @param decoder_dsc       pointer to the decoded image's descriptor
 * @param sup               supplementary data
 * @param img_coords        the absolute coordinates of the image
 * @param clipped_img_area  the absolute clip coordinates
 */
typedef void (*lv_draw_image_core_cb)(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
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
 * @note                `coords` can be small than the real image area
 *                      (if only a part of the image is rendered)
 *                      or can be larger (in case of tiled images).   .
 */
void lv_draw_image(lv_layer_t * layer, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

/**
 * Create a draw task to blend a layer to another layer
 * @param layer         pointer to a layer
 * @param dsc           pointer to an initialized draw descriptor. `src` must be set to the layer to blend
 * @param coords        the coordinates of the layer.
 * @note                `coords` can be small than the total widget area from which the layer is created
 *                      (if only a part of the widget was rendered to a layer)
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

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_IMAGE_H*/
