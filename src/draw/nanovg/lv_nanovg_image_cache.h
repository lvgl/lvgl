/**
 * @file lv_nanovg_image_cache.h
 *
 */

#ifndef LV_NANOVG_IMAGE_CACHE_H
#define LV_NANOVG_IMAGE_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_NANOVG

#include "../lv_draw_image_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_nanovg_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Initialize the image cache
 * @param u pointer to the nanovg unit
 */
void lv_nanovg_image_cache_init(struct _lv_draw_nanovg_unit_t * u);

/**
 * @brief Deinitialize the image cache
 * @param u pointer to the nanovg unit
 */
void lv_nanovg_image_cache_deinit(struct _lv_draw_nanovg_unit_t * u);

/**
 * @brief Get the image handle from the cache, create a new one if not found
 * @param u pointer to the nanovg unit
 * @param src the source image data
 * @param color the color to apply
 * @param image_flags the image flags
 * @param header the image header to fill (can be NULL)
 * @return the image handle, or -1 on failure
 */
int lv_nanovg_image_cache_get_handle(struct _lv_draw_nanovg_unit_t * u,
                                     const void * src,
                                     lv_color32_t color,
                                     int image_flags,
                                     lv_image_header_t * header);

/**
 * @brief Drop the image from the cache
 * @param u pointer to the nanovg unit
 * @param src the source image data
 */
void lv_nanovg_image_cache_drop(struct _lv_draw_nanovg_unit_t * u, const void * src);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_NANOVG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NANOVG_IMAGE_CACHE_H*/
