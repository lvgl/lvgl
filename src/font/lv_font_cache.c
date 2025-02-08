/**
 * @file lv_font_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_font_cache.h"

#if LV_FONT_CACHE_GLYPH_CNT > 0

#include "lv_font_fmt_txt.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define font_bitmap_cache LV_GLOBAL_DEFAULT()->font_bitmap_cache
#define font_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->font_draw_buf_handlers)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /* key */
    const lv_font_fmt_txt_dsc_t * fdsc;
    uint32_t glyph_index;

    /* value */
    lv_draw_buf_t * draw_buf;
} font_bitmap_cache_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline bool font_is_built_in(const lv_font_t * font);
static void * get_bitmap_cached(lv_font_glyph_dsc_t * g_dsc, const lv_font_fmt_txt_dsc_t * fdsc, uint32_t gid);
static bool font_bitmap_create_cb(font_bitmap_cache_data_t * data, void * user_data);
static void font_bitmap_free_cb(font_bitmap_cache_data_t * data, void * user_data);
static lv_cache_compare_res_t font_bitmap_compare_cb(const font_bitmap_cache_data_t * lhs,
                                                     const font_bitmap_cache_data_t * rhs);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_font_cache_init(uint32_t max_glyph_cnt)
{
    LV_ASSERT_MSG(font_bitmap_cache == NULL, "Font cache already initialized");

    lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)font_bitmap_compare_cb,
        .create_cb = (lv_cache_create_cb_t)font_bitmap_create_cb,
        .free_cb = (lv_cache_free_cb_t)font_bitmap_free_cb,
    };

    lv_cache_t * cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(font_bitmap_cache_data_t),
                                         max_glyph_cnt, ops);
    lv_cache_set_name(cache, "FONT_BITMAP");

    font_bitmap_cache = cache;
}

void lv_font_cache_deinit(void)
{
    lv_cache_destroy(font_bitmap_cache, NULL);
    font_bitmap_cache = NULL;
}

const void * lv_font_cache_get_glyph_bitmap(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf)
{
    LV_ASSERT_NULL(g_dsc);

    const lv_font_t * font = g_dsc->resolved_font;
    if(!font_is_built_in(font)) {
        /* If the font is not built-in, use the original function to get the bitmap */
        return font->get_glyph_bitmap(g_dsc, draw_buf);
    }

    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
    uint32_t gid = g_dsc->gid.index;
    if(!gid) return NULL;

    const lv_font_fmt_txt_glyph_dsc_t * gdsc = &fdsc->glyph_dsc[gid];
    if(g_dsc->req_raw_bitmap) return &fdsc->glyph_bitmap[gdsc->bitmap_index];

    int32_t gsize = (int32_t) gdsc->box_w * gdsc->box_h;
    if(gsize == 0) return NULL;

    return get_bitmap_cached(g_dsc, fdsc, gid);
}

void lv_font_cache_release_glyph(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc)
{
    LV_ASSERT_NULL(font);
    LV_ASSERT_NULL(g_dsc);

    if(!font_is_built_in(font)) {
        /*Font cache can be released only for built-in fonts*/
        return;
    }

    lv_cache_release(font_bitmap_cache, g_dsc->entry, NULL);
    g_dsc->entry = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline bool font_is_built_in(const lv_font_t * font)
{
    if(font->static_bitmap) {
        return true;
    }

    /* Use function pointer to simply determine whether the current font is a built-in font */
    return font->get_glyph_bitmap == lv_font_get_bitmap_fmt_txt;
}

static void * get_bitmap_cached(lv_font_glyph_dsc_t * g_dsc, const lv_font_fmt_txt_dsc_t * fdsc, uint32_t glyph_index)
{
    LV_PROFILER_FONT_BEGIN;
    font_bitmap_cache_data_t search_key = {
        .fdsc = fdsc,
        .glyph_index = glyph_index,
    };

    lv_cache_entry_t * entry = lv_cache_acquire_or_create(font_bitmap_cache, &search_key, g_dsc);

    if(!entry) {
        LV_LOG_TRACE("Glyph %d not found in cache", glyph_index);
        LV_PROFILER_FONT_END;
        return NULL;
    }

    g_dsc->entry = entry;
    font_bitmap_cache_data_t * cache_node = lv_cache_entry_get_data(entry);

    LV_PROFILER_FONT_END;
    return cache_node->draw_buf;
}

/**
 * Create a cache for font bitmaps.
 * @param data the font bitmap cache data
 * @param user_data unused
 * @return true: create success, false: create failed
 */
static bool font_bitmap_create_cb(font_bitmap_cache_data_t * data, void * user_data)
{
    LV_PROFILER_FONT_BEGIN;
    lv_font_glyph_dsc_t * g_dsc = user_data;
    const lv_font_t * font = g_dsc->resolved_font;
    const lv_font_fmt_txt_glyph_dsc_t * gdsc = &data->fdsc->glyph_dsc[data->glyph_index];
    lv_draw_buf_t * draw_buf = lv_draw_buf_create_ex(
                                   font_draw_buf_handlers,
                                   gdsc->box_w,
                                   gdsc->box_h,
                                   LV_COLOR_FORMAT_A8,
                                   LV_STRIDE_AUTO);
    if(!draw_buf) {
        LV_PROFILER_FONT_END;
        return false;
    }

    if(!font->get_glyph_bitmap(g_dsc, draw_buf)) {
        lv_draw_buf_destroy(draw_buf);
        LV_PROFILER_FONT_END;
        return false;
    }

    LV_LOG_TRACE("Created font bitmap cache entry for glyph_index %d, size %dx%d",
                 data->glyph_index, draw_buf->header.w, draw_buf->header.h);

    data->draw_buf = draw_buf;
    LV_PROFILER_FONT_END;
    return true;
}

static void font_bitmap_free_cb(font_bitmap_cache_data_t * data, void * user_data)
{
    LV_UNUSED(user_data);
    lv_draw_buf_destroy(data->draw_buf);
}

static lv_cache_compare_res_t font_bitmap_compare_cb(const font_bitmap_cache_data_t * lhs,
                                                     const font_bitmap_cache_data_t * rhs)
{
    if(lhs->glyph_index != rhs->glyph_index) {
        return lhs->glyph_index > rhs->glyph_index ? 1 : -1;
    }

    if(lhs->fdsc != rhs->fdsc) {
        return lhs->fdsc > rhs->fdsc ? 1 : -1;
    }

    return 0;
}

#endif /* LV_FONT_CACHE_GLYPH_CNT */
