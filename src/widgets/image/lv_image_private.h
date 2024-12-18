/**
 * @file lv_image_private.h
 *
 */

#ifndef LV_IMAGE_PRIVATE_H
#define LV_IMAGE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_image.h"

#if LV_USE_IMAGE != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Data of image
 */
struct _lv_image_t {
    lv_obj_t obj;
    const void * src;       /**< Image source: Pointer to an array or a file or a symbol*/
    const lv_image_dsc_t * bitmap_mask_src; /**< Pointer to an A8 bitmap mask */
    lv_point_t offset;
    int32_t w;              /**< Width of the image (Handled by the library)*/
    int32_t h;              /**< Height of the image (Handled by the library)*/
    uint32_t rotation;      /**< Rotation angle of the image*/
    uint32_t scale_x;       /**< 256 means no zoom, 512 double size, 128 half size*/
    uint32_t scale_y;       /**< 256 means no zoom, 512 double size, 128 half size*/
    lv_point_t pivot;       /**< Rotation center of the image*/
    uint32_t src_type : 2;  /**< See: lv_image_src_t*/
    uint32_t cf : 5;        /**< Color format from `lv_color_format_t`*/
    uint32_t antialias : 1; /**< Apply anti-aliasing in transformations (rotate, zoom)*/
    uint32_t align: 4;      /**< Image size mode when image size and object size is different. See lv_image_align_t*/
    uint32_t blend_mode: 4; /**< Element of `lv_blend_mode_t`*/
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_IMAGE != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGE_PRIVATE_H*/
