/**
 * @file lv_img_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_img_cache_manager_t img_cache_manager = { 0 };

/**********************
 *      MACROS
 **********************/



/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Open an image using the image decoder interface and cache it.
 * The image will be left open meaning if the image decoder open callback allocated memory then it will remain.
 * The image is closed if a new image is opened and the new image takes its place in the cache.
 * @param src source of the image. Path to file or pointer to an `lv_img_dsc_t` variable
 * @param color color The color of the image with `LV_IMG_CF_ALPHA_...`
 * @return pointer to the cache entry or NULL if can open the image
 */
_lv_img_cache_entry_t * _lv_img_cache_open(const lv_img_dec_dsc_in_t * dsc, lv_img_dec_ctx_t * dec_ctx)
{
    LV_ASSERT_NULL(img_cache_manager.open_cb);
    return img_cache_manager.open_cb(dsc, dec_ctx);
}

lv_res_t _lv_img_cache_query(const lv_img_dec_dsc_in_t * dsc, lv_img_header_t * header, uint8_t * caps,
                            lv_img_dec_ctx_t * dec_ctx)
{
    if(img_cache_manager.query_cb);
        return img_cache_manager.query_cb(dsc, header, caps, dec_ctx);
    return LV_RES_INV;
}

void _lv_img_cache_cleanup(_lv_img_cache_entry_t * entry)
{
    if(img_cache_manager.cleanup_cb);
        return img_cache_manager.cleanup_cb(entry);
}

void lv_img_cache_set_size(uint16_t new_entry_cnt)
{
    LV_ASSERT_NULL(img_cache_manager.set_size_cb);
    img_cache_manager.set_size_cb(new_entry_cnt);
}

void lv_img_cache_invalidate_src(const void * src)
{
    LV_ASSERT_NULL(img_cache_manager.invalidate_src_cb);
    img_cache_manager.invalidate_src_cb(src);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
