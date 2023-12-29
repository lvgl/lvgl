/**
 * @file lv_freetype_image.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_freetype_private.h"

/*********************
 *      DEFINES
 *********************/

#if LV_USE_FREETYPE && LV_FREETYPE_CACHE_TYPE == LV_FREETYPE_CACHE_TYPE_IMAGE

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_freetype_cache_context_t {
    lv_cache_t * cache;

    FT_Face face;
};

struct _lv_freetype_cache_node_t {
    FT_UInt glyph_index;
    uint32_t size;

    lv_font_glyph_dsc_t glyph_dsc;
    lv_draw_buf_t * draw_buf;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool freetype_get_glyph_dsc_cb(const lv_font_t * font,
                                      lv_font_glyph_dsc_t * dsc_out,
                                      uint32_t unicode_letter,
                                      uint32_t unicode_letter_next);

static const uint8_t * freetype_get_glyph_bitmap_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc,
                                                    uint32_t unicode_letter,
                                                    uint8_t * bitmap_out);

static bool freetype_image_create_cb(lv_freetype_cache_node_t * data, void * user_data);
static void freetype_image_free_cb(lv_freetype_cache_node_t * node, void * user_data);
static lv_cache_compare_res_t freetype_image_compare_cb(const lv_freetype_cache_node_t * lhs,
                                                        const lv_freetype_cache_node_t * rhs);

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

lv_freetype_cache_context_t * lv_freetype_cache_context_create(lv_freetype_context_t * ctx)
{
    LV_UNUSED(ctx);

    lv_freetype_cache_context_t * cache_ctx = lv_malloc(sizeof(lv_freetype_cache_context_t));
    LV_ASSERT_MALLOC(cache_ctx);
    lv_memzero(cache_ctx, sizeof(lv_freetype_cache_context_t));

    lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)freetype_image_compare_cb,
        .create_cb = (lv_cache_create_cb_t)freetype_image_create_cb,
        .free_cb = (lv_cache_free_cb_t)freetype_image_free_cb,
    };

    cache_ctx->cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(lv_freetype_cache_node_t),
                                       LV_FREETYPE_CACHE_FT_OUTLINES, ops);
    if(cache_ctx->cache == NULL) {
        LV_LOG_ERROR("lv_cache_create failed");
        lv_free(cache_ctx);
        return NULL;
    }

    return cache_ctx;
}

void lv_freetype_cache_context_delete(lv_freetype_cache_context_t * cache_ctx)
{
    LV_ASSERT_NULL(cache_ctx);
    lv_cache_destroy(cache_ctx->cache, NULL);
    lv_free(cache_ctx);
}

bool lv_freetype_on_font_create(lv_freetype_font_dsc_t * dsc)
{
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);
    dsc->font.get_glyph_dsc = freetype_get_glyph_dsc_cb;
    dsc->font.get_glyph_bitmap = freetype_get_glyph_bitmap_cb;
    dsc->font.release_glyph = freetype_image_release_cb;

    FT_Size ft_size = lv_freetype_lookup_size(dsc);
    if(!ft_size) {
        return false;
    }

    FT_Face face = ft_size->face;

    if(dsc->style & LV_FREETYPE_FONT_STYLE_ITALIC) {
        lv_freetype_italic_transform(face);
    }
    return true;
}

void lv_freetype_on_font_delete(lv_freetype_font_dsc_t * dsc)
{
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool freetype_get_glyph_dsc_cb(const lv_font_t * font,
                                      lv_font_glyph_dsc_t * dsc_out,
                                      uint32_t unicode_letter,
                                      uint32_t unicode_letter_next)
{
    if(unicode_letter < 0x20) {
        dsc_out->adv_w = 0;
        dsc_out->box_h = 0;
        dsc_out->box_w = 0;
        dsc_out->ofs_x = 0;
        dsc_out->ofs_y = 0;
        dsc_out->bpp = 0;
        return true;
    }

    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);

    FT_Size ft_size = lv_freetype_lookup_size(dsc);
    if(!ft_size) {
        return false;
    }

    FT_Face face = ft_size->face;
    FT_UInt charmap_index = FT_Get_Charmap_Index(face->charmap);
    FT_UInt glyph_index = FTC_CMapCache_Lookup(dsc->context->cmap_cache, dsc->face_id, charmap_index, unicode_letter);
    dsc_out->is_placeholder = glyph_index == 0;
    dsc->context->cache_context->face = face;

    lv_freetype_cache_node_t search_key = {
        .glyph_index = glyph_index,
        .size = dsc->size,
    };

    lv_cache_entry_t * entry = lv_cache_acquire_or_create(dsc->context->cache_context->cache, &search_key, dsc);
    if(entry == NULL) {
        LV_LOG_ERROR("glyph lookup failed for glyph_index = %u", glyph_index);
        return false;
    }
    lv_freetype_cache_node_t * data = lv_cache_entry_get_data(entry);
    *dsc_out = data->glyph_dsc;

    if((dsc->style & LV_FREETYPE_FONT_STYLE_ITALIC) && (unicode_letter_next == '\0')) {
        dsc_out->adv_w = dsc_out->box_w + dsc_out->ofs_x;
    }

    dsc_out->entry = NULL;

    lv_cache_release(dsc->context->cache_context->cache, entry, NULL);
    return true;
}

static const uint8_t * freetype_get_glyph_bitmap_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc,
                                                    uint32_t unicode_letter,
                                                    uint8_t * bitmap_out)
{
    LV_UNUSED(unicode_letter);
    LV_UNUSED(bitmap_out);

    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);

    FT_Size ft_size = lv_freetype_lookup_size(dsc);
    if(!ft_size) {
        return false;
    }

    FT_Face face = ft_size->face;
    FT_UInt charmap_index = FT_Get_Charmap_Index(face->charmap);
    FT_UInt glyph_index = FTC_CMapCache_Lookup(dsc->context->cmap_cache, dsc->face_id, charmap_index, unicode_letter);
    dsc->context->cache_context->face = face;

    lv_cache_t * cache = dsc->context->cache_context->cache;

    lv_freetype_cache_node_t search_key = {
        .glyph_index = glyph_index,
        .size = dsc->size
    };

    lv_cache_entry_t * entry = lv_cache_acquire_or_create(cache, &search_key, dsc);

    g_dsc->entry = entry;
    lv_freetype_cache_node_t * cache_node = lv_cache_entry_get_data(entry);

    return cache_node->draw_buf->data;
}

static void freetype_image_release_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc)
{
    LV_ASSERT_NULL(font);
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;
    lv_cache_release(dsc->context->cache_context->cache, g_dsc->entry, NULL);
    g_dsc->entry = NULL;
}

/*-----------------
 * Cache Callbacks
 *----------------*/

static bool freetype_image_create_cb(lv_freetype_cache_node_t * data, void * user_data)
{
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)user_data;

    FT_Error error;

    FT_Face face = dsc->context->cache_context->face;
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

    lv_font_glyph_dsc_t * dsc_out = &data->glyph_dsc;

    dsc_out->adv_w = FT_F16DOT16_TO_INT(glyph_bitmap->root.advance.x);
    dsc_out->box_h = glyph_bitmap->bitmap.rows;         /*Height of the bitmap in [px]*/
    dsc_out->box_w = glyph_bitmap->bitmap.width;        /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = glyph_bitmap->left;                /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = glyph_bitmap->top -
                     glyph_bitmap->bitmap.rows;         /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;                                   /*Bit per pixel: 1/2/4/8*/

    uint32_t stride = lv_draw_buf_width_to_stride(dsc_out->box_w, LV_COLOR_FORMAT_A8);
    data->draw_buf = lv_draw_buf_create(dsc_out->box_w, dsc_out->box_h, LV_COLOR_FORMAT_A8, stride);

    for(int y = 0; y < dsc_out->box_h; ++y) {
        lv_memcpy((uint8_t *)(data->draw_buf->data) + y * stride, glyph_bitmap->bitmap.buffer + y * dsc_out->box_w,
                  dsc_out->box_w);
    }

    return true;
}
static void freetype_image_free_cb(lv_freetype_cache_node_t * data, void * user_data)
{
    LV_UNUSED(user_data);
    lv_draw_buf_destroy(data->draw_buf);
}
static lv_cache_compare_res_t freetype_image_compare_cb(const lv_freetype_cache_node_t * lhs,
                                                        const lv_freetype_cache_node_t * rhs)
{
    if(lhs->glyph_index != rhs->glyph_index) {
        return lhs->glyph_index > rhs->glyph_index ? 1 : -1;
    }
    if(lhs->size != rhs->size) {
        return lhs->size > rhs->size ? 1 : -1;
    }
    return 0;
}
#endif
