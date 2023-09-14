/**
 * @file lv_image_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_image_cache.h"
#include "../stdlib/lv_string.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define img_cache_manager LV_GLOBAL_DEFAULT()->img_cache_mgr

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_image_cache_manager_init(lv_image_cache_manager_t * manager)
{
    LV_ASSERT_NULL(manager);
    lv_memzero(manager, sizeof(lv_image_cache_manager_t));
}

void lv_image_cache_manager_apply(const lv_image_cache_manager_t * manager)
{
    LV_ASSERT_NULL(manager);
    lv_memcpy(&img_cache_manager, manager, sizeof(lv_image_cache_manager_t));
}

_lv_image_cache_entry_t * _lv_image_cache_open(const void * src, lv_color_t color, int32_t frame_id)
{
    LV_ASSERT_NULL(img_cache_manager.open_cb);
    return img_cache_manager.open_cb(src, color, frame_id);
}

void lv_image_cache_set_size(uint16_t new_entry_cnt)
{
    LV_ASSERT_NULL(img_cache_manager.set_size_cb);
    img_cache_manager.set_size_cb(new_entry_cnt);
}

void lv_image_cache_invalidate_src(const void * src)
{
    LV_ASSERT_NULL(img_cache_manager.invalidate_src_cb);
    img_cache_manager.invalidate_src_cb(src);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
