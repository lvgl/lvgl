/**
 * @file lv_freetype_image.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"
#include "lv_freetype_private.h"

#if LV_USE_FREETYPE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_freetype_image_cache_data_t {
    FT_UInt glyph_index;
    uint32_t size;

    lv_draw_buf_t * draw_buf;
} lv_freetype_image_cache_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static const void * freetype_get_glyph_bitmap_cb(lv_font_glyph_dsc_t * g_dsc,
                                                 uint32_t unicode_letter,
                                                 lv_draw_buf_t * draw_buf);

static bool freetype_image_create_cb(lv_freetype_image_cache_data_t * data, void * user_data);
static void freetype_image_free_cb(lv_freetype_image_cache_data_t * node, void * user_data);
static lv_cache_compare_res_t freetype_image_compare_cb(const lv_freetype_image_cache_data_t * lhs,
                                                        const lv_freetype_image_cache_data_t * rhs);

static void freetype_image_release_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_cache_t * lv_freetype_create_draw_data_image(uint32_t cache_size)
{
    lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)freetype_image_compare_cb,
        .create_cb = (lv_cache_create_cb_t)freetype_image_create_cb,
        .free_cb = (lv_cache_free_cb_t)freetype_image_free_cb,
    };

    lv_cache_t * draw_data_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(lv_freetype_image_cache_data_t),
                                                   cache_size, ops);

    return draw_data_cache;
}

void lv_freetype_set_cbs_image_font(lv_freetype_font_dsc_t * dsc)
{
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);
    dsc->font.get_glyph_bitmap = freetype_get_glyph_bitmap_cb;
    dsc->font.release_glyph = freetype_image_release_cb;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static const void * freetype_get_glyph_bitmap_cb(lv_font_glyph_dsc_t * g_dsc,
                                                 uint32_t unicode_letter,
                                                 lv_draw_buf_t * draw_buf)
{
    LV_UNUSED(unicode_letter);
    LV_UNUSED(draw_buf);
    const lv_font_t * font = g_dsc->resolved_font;
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);

    FT_Face face = dsc->cache_node->face;
    FT_UInt glyph_index = FT_Get_Char_Index(face, unicode_letter);

    lv_cache_t * cache = dsc->cache_node->draw_data_cache;

    lv_freetype_image_cache_data_t search_key = {
        .glyph_index = glyph_index,
        .size = dsc->size,
    };

    lv_cache_entry_t * entry = lv_cache_acquire_or_create(cache, &search_key, dsc);

    g_dsc->entry = entry;
    lv_freetype_image_cache_data_t * cache_node = lv_cache_entry_get_data(entry);

    return cache_node->draw_buf;
}

static void freetype_image_release_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc)
{
    LV_ASSERT_NULL(font);
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;
    lv_cache_release(dsc->cache_node->draw_data_cache, g_dsc->entry, NULL);
    g_dsc->entry = NULL;
}

/*-----------------
 * Cache Callbacks
 *----------------*/

static bool freetype_image_create_cb(lv_freetype_image_cache_data_t * data, void * user_data)
{
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)user_data;

    FT_Error error;

    FT_Face face = dsc->cache_node->face;
    FT_Set_Pixel_Sizes(face, 0, dsc->size);
    error = FT_Load_Glyph(face, data->glyph_index,  FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL);
    if(error) {
        FT_ERROR_MSG("FT_Load_Glyph", error);
        return false;
    }
    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if(error) {
        FT_ERROR_MSG("FT_Render_Glyph", error);
        return false;
    }

    FT_Glyph glyph;
    error = FT_Get_Glyph(face->glyph, &glyph);
    if(error) {
        FT_ERROR_MSG("FT_Get_Glyph", error);
        return false;
    }

    FT_BitmapGlyph glyph_bitmap = (FT_BitmapGlyph)glyph;

    uint16_t box_h = glyph_bitmap->bitmap.rows;         /*Height of the bitmap in [px]*/
    uint16_t box_w = glyph_bitmap->bitmap.width;        /*Width of the bitmap in [px]*/

    uint32_t stride = lv_draw_buf_width_to_stride(box_w, LV_COLOR_FORMAT_A8);
    data->draw_buf = lv_draw_buf_create(box_w, box_h, LV_COLOR_FORMAT_A8, stride);

    for(int y = 0; y < box_h; ++y) {
        lv_memcpy((uint8_t *)(data->draw_buf->data) + y * stride, glyph_bitmap->bitmap.buffer + y * box_w,
                  box_w);
    }

    FT_Done_Glyph(glyph);

    return true;
}
static void freetype_image_free_cb(lv_freetype_image_cache_data_t * data, void * user_data)
{
    LV_UNUSED(user_data);
    lv_draw_buf_destroy(data->draw_buf);
}
static lv_cache_compare_res_t freetype_image_compare_cb(const lv_freetype_image_cache_data_t * lhs,
                                                        const lv_freetype_image_cache_data_t * rhs)
{
    if(lhs->glyph_index != rhs->glyph_index) {
        return lhs->glyph_index > rhs->glyph_index ? 1 : -1;
    }
    if(lhs->size != rhs->size) {
        return lhs->size > rhs->size ? 1 : -1;
    }
    return 0;
}

#endif /*LV_USE_FREETYPE*/
