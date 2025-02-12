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
#define font_static_bitmap_draw_buf (LV_GLOBAL_DEFAULT()->font_static_bitmap_draw_buf)

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

static inline bool has_static_bitmap(const lv_font_t * font);
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
    if(draw_buf || !has_static_bitmap(font)) {
        /* If the font is not built-in, use the original function to get the bitmap */
        return font->get_glyph_bitmap(g_dsc, draw_buf);
    }

    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
    uint32_t gid = g_dsc->gid.index;
    if(!gid) return NULL;

    const lv_font_fmt_txt_glyph_dsc_t * gdsc = &fdsc->glyph_dsc[gid];
    void * raw_glyph_bitmap = (void *)&fdsc->glyph_bitmap[gdsc->bitmap_index];

    if(g_dsc->req_raw_bitmap) return raw_glyph_bitmap;

    int32_t gsize = (int32_t) gdsc->box_w * gdsc->box_h;
    if(gsize == 0) return NULL;

    /**
     * To prevent multithreaded access to the font_static_bitmap_draw_buf global variable,
     * only non-OS mode is supported
     */
#if LV_USE_OS == LV_OS_NONE
    /* If the alignment, stride, and color format is correct, bypass the cache */
    if(g_dsc->format == LV_FONT_GLYPH_FORMAT_A8
       && raw_glyph_bitmap == lv_draw_buf_align(raw_glyph_bitmap, LV_COLOR_FORMAT_A8)
       && g_dsc->box_w == lv_draw_buf_width_to_stride(g_dsc->box_w, LV_COLOR_FORMAT_A8)) {

        const uint32_t a8_stride = gdsc->box_w * sizeof(uint8_t);

        /* Use the static bitmap draw buffer to avoid memory allocation */
        lv_draw_buf_init(
            &font_static_bitmap_draw_buf,
            g_dsc->box_w,
            g_dsc->box_h,
            LV_COLOR_FORMAT_A8,
            a8_stride,
            raw_glyph_bitmap,
            a8_stride * g_dsc->box_h);

        return &font_static_bitmap_draw_buf;
    }
#endif

    return get_bitmap_cached(g_dsc, fdsc, gid);
}

void lv_font_cache_release_glyph(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc)
{
    LV_ASSERT_NULL(font);
    LV_ASSERT_NULL(g_dsc);

    if(!g_dsc->entry) {
        return;
    }

    lv_cache_release(font_bitmap_cache, g_dsc->entry, NULL);
    g_dsc->entry = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline bool has_static_bitmap(const lv_font_t * font)
{
    if(lv_font_has_static_bitmap(font)) {
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
