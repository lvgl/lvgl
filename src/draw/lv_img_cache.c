/**
 * @file lv_img_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img_cache.h"
#include "../stdlib/lv_string.h"

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

void lv_img_cache_manager_init(lv_img_cache_manager_t * manager)
{
    LV_ASSERT_NULL(manager);
    lv_memzero(manager, sizeof(lv_img_cache_manager_t));
}

void lv_img_cache_manager_apply(const lv_img_cache_manager_t * manager)
{
    LV_ASSERT_NULL(manager);
    lv_memcpy(&img_cache_manager, manager, sizeof(lv_img_cache_manager_t));
}

_lv_img_cache_entry_t * _lv_img_cache_open(const void * src, lv_color_t color, int32_t frame_id)
{
    LV_ASSERT_NULL(img_cache_manager.open_cb);
    return img_cache_manager.open_cb(src, color, frame_id);
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
