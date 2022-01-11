/**
 * @file lv_draw_img.h
 *
 */

#ifndef LV_DRAW_IMG_H
#define LV_DRAW_IMG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_img_decoder.h"
#include "lv_img_buf.h"
#include "../misc/lv_style.h"
#include "lv_img_cache.h"

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

    uint16_t angle;
    uint16_t zoom;
    lv_point_t pivot;

    lv_color_t recolor;
    lv_opa_t recolor_opa;

    lv_opa_t opa;
    lv_blend_mode_t blend_mode : 4;

    lv_img_dec_ctx_t * dec_ctx;
    uint8_t antialias       : 1;
} lv_draw_img_dsc_t;

struct _lv_draw_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_img_dsc_init(lv_draw_img_dsc_t * dsc);
/**
 * Draw an image
 * @param draw_ctx A pointer on a draw context
 * @param coords the coordinates of the image
 * @param src pointer to an image source uri descriptor
 * @param dsc pointer to an initialized `lv_draw_img_dsc_t` variable
 */
void lv_draw_img(struct _lv_draw_ctx_t * draw_ctx, lv_draw_img_dsc_t * dsc, const lv_area_t * coords,
                 const lv_img_src_uri_t * src);

/**
 * Draw an image from cache.
 * This is slighty faster that lv_draw_img above if you've already queried the image cache earlier
 * @param draw_ctx A pointer on a draw context
 * @param coords the coordinates of the image
 * @param entry pointer to a lv_img_cache_entry
 * @param dsc pointer to an initialized `lv_draw_img_dsc_t` variable
 */
void lv_draw_img_cached(struct _lv_draw_ctx_t * draw_ctx, lv_draw_img_dsc_t * dsc, const lv_area_t * coords,
                        lv_img_cache_entry_t * entry);


void lv_draw_img_decoded(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                         const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t color_format);


/**
 * Get the pixel size of a color format in bits
 * @param cf a color format (`LV_IMG_CF_...`)
 * @return the pixel size in bits
 */
uint8_t lv_img_cf_get_px_size(lv_img_cf_t cf);

/**
 * Check if a color format is chroma keyed or not
 * @param cf a color format (`LV_IMG_CF_...`)
 * @return true: chroma keyed; false: not chroma keyed
 */
bool lv_img_cf_is_chroma_keyed(lv_img_cf_t cf);

/**
 * Check if a color format has alpha channel or not
 * @param cf a color format (`LV_IMG_CF_...`)
 * @return true: has alpha channel; false: doesn't have alpha channel
 */
bool lv_img_cf_has_alpha(lv_img_cf_t cf);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_IMG_H*/
