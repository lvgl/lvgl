/**
 * @file lv_vg_lite_bitmap_font_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_bitmap_font_cache.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_pending.h"
#include "lv_vg_lite_utils.h"
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define font_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->font_draw_buf_handlers)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /* key */
    lv_font_glyph_dsc_t g_dsc;

    /* value */
    lv_draw_buf_t * draw_buf;
} cache_item_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void cache_release_cb(void * entry, void * user_data);
static bool cache_create_cb(cache_item_t * item, void * user_data);
static void cache_free_cb(cache_item_t * item, void * user_data);
static lv_cache_compare_res_t cache_compare_cb(const cache_item_t * lhs, const cache_item_t * rhs);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_vg_lite_bitmap_font_cache_init(struct _lv_draw_vg_lite_unit_t * unit, uint32_t cache_cnt)
{
    LV_ASSERT_NULL(unit);
    LV_ASSERT(unit->bitmap_font_cache == NULL);
    LV_ASSERT(unit->bitmap_font_pending == NULL);
    LV_ASSERT(cache_cnt > 0);

    const lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)cache_compare_cb,
        .create_cb = (lv_cache_create_cb_t)cache_create_cb,
        .free_cb = (lv_cache_free_cb_t)cache_free_cb,
    };

    unit->bitmap_font_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(cache_item_t), cache_cnt, ops);
    lv_cache_set_name(unit->bitmap_font_cache, "VG_BITMAP_FONT");
    unit->bitmap_font_pending = lv_vg_lite_pending_create(sizeof(lv_cache_entry_t *), 8);
    lv_vg_lite_pending_set_free_cb(unit->bitmap_font_pending, cache_release_cb, unit->bitmap_font_cache);
}

void lv_vg_lite_bitmap_font_cache_deinit(struct _lv_draw_vg_lite_unit_t * unit)
{
    LV_ASSERT_NULL(unit);
    LV_ASSERT_NULL(unit->bitmap_font_cache);
    LV_ASSERT_NULL(unit->bitmap_font_pending);

    lv_vg_lite_pending_destroy(unit->bitmap_font_pending);
    unit->bitmap_font_pending = NULL;

    lv_cache_destroy(unit->bitmap_font_cache, NULL);
    unit->bitmap_font_cache = NULL;
}

lv_draw_buf_t * lv_vg_lite_bitmap_font_cache_get(struct _lv_draw_vg_lite_unit_t * unit,
                                                 const lv_font_glyph_dsc_t * g_dsc)
{
    LV_PROFILER_FONT_BEGIN;
    LV_ASSERT_NULL(unit);
    LV_ASSERT_NULL(g_dsc);

    uint32_t gid = g_dsc->gid.index;
    if(!gid) {
        LV_PROFILER_FONT_END;
        return NULL;
    }

    cache_item_t search_key = { 0 };
    search_key.g_dsc = *g_dsc;

    lv_cache_entry_t * cache_node_entry = lv_cache_acquire(unit->bitmap_font_cache, &search_key, NULL);

    if(cache_node_entry == NULL) {
        /* check if the cache is full */
        size_t free_size = lv_cache_get_free_size(unit->bitmap_font_cache, NULL);
        if(free_size == 0) {
            LV_LOG_INFO("bitmap font cache is full, release all pending cache entries");
            lv_vg_lite_finish(unit);
        }

        cache_node_entry = lv_cache_acquire_or_create(unit->bitmap_font_cache, &search_key, NULL);
        if(cache_node_entry == NULL) {
            LV_LOG_ERROR("bitmap cache creating failed");
            LV_PROFILER_FONT_END;
            return NULL;
        }
    }

    /* Add the new entry to the pending list */
    lv_vg_lite_pending_add(unit->bitmap_font_pending, &cache_node_entry);

    cache_item_t * cache_item = lv_cache_entry_get_data(cache_node_entry);

    LV_PROFILER_FONT_END;
    return cache_item->draw_buf;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void cache_release_cb(void * entry, void * user_data)
{
    lv_cache_entry_t ** entry_p = entry;
    lv_cache_t * cache = user_data;
    lv_cache_release(cache, *entry_p, NULL);
}

static bool cache_create_cb(cache_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);
    LV_PROFILER_FONT_BEGIN;

    LV_LOG_TRACE("gid: %" LV_PRIu32 ", W%" LV_PRIu32 "xH%" LV_PRIu32,
                 item->g_dsc.gid.index, item->g_dsc.box_w, item->g_dsc.box_h);

    lv_draw_buf_t * draw_buf = lv_draw_buf_create_ex(font_draw_buf_handlers,
                                                     item->g_dsc.box_w,
                                                     item->g_dsc.box_h,
                                                     LV_COLOR_FORMAT_A8,
                                                     LV_STRIDE_AUTO);
    if(!draw_buf) {
        LV_LOG_ERROR("Failed to create draw buffer for bitmap font cache");
        LV_PROFILER_FONT_END;
        return false;
    }

    if(!lv_font_get_glyph_bitmap(&item->g_dsc, draw_buf)) {
        LV_LOG_WARN("Failed to get glyph bitmap for bitmap font cache");
        lv_draw_buf_destroy(draw_buf);
        LV_PROFILER_FONT_END;
        return false;
    }

    item->draw_buf = draw_buf;

    LV_PROFILER_FONT_END;
    return true;
}

static void cache_free_cb(cache_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);
    LV_PROFILER_FONT_BEGIN;

    LV_LOG_TRACE("gid: %" LV_PRIu32 ", W%" LV_PRIu32 "xH%" LV_PRIu32,
                 item->g_dsc.gid.index, item->g_dsc.box_w, item->g_dsc.box_h);

    lv_draw_buf_destroy(item->draw_buf);
    item->draw_buf = NULL;

    LV_PROFILER_FONT_END;
}

static lv_cache_compare_res_t cache_compare_cb(const cache_item_t * lhs, const cache_item_t * rhs)
{
    /* Because const font pointers are unique, matching can be performed using only the pointer. */
    if(lhs->g_dsc.resolved_font != rhs->g_dsc.resolved_font) {
        return lhs->g_dsc.resolved_font > rhs->g_dsc.resolved_font ? 1 : -1;
    }

    if(lhs->g_dsc.gid.index != rhs->g_dsc.gid.index) {
        return lhs->g_dsc.gid.index > rhs->g_dsc.gid.index ? 1 : -1;
    }

    return 0;
}

#endif /*LV_USE_DRAW_VG_LITE*/
