/**
 * @file lv_freetype_glyph.c
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

#define CACHE_NAME  "FREETYPE_GLYPH"

#if LV_USE_HARFBUZZ
/* Tag bit to distinguish glyph-ID cache entries from unicode entries.
 * Unicode codepoints max at 0x10FFFF, so bit 31 is always free. */
#define GID_TAG  0x80000000u
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_freetype_glyph_cache_data_t {
    uint32_t unicode;
    uint32_t size;

    lv_font_glyph_dsc_t glyph_dsc;
} lv_freetype_glyph_cache_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool freetype_get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc, uint32_t unicode_letter,
                                      uint32_t unicode_letter_next);

static bool freetype_glyph_create_cb(lv_freetype_glyph_cache_data_t * data, void * user_data);
static void freetype_glyph_free_cb(lv_freetype_glyph_cache_data_t * data, void * user_data);
static lv_cache_compare_res_t freetype_glyph_compare_cb(const lv_freetype_glyph_cache_data_t * lhs,
                                                        const lv_freetype_glyph_cache_data_t * rhs);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_cache_t * lv_freetype_create_glyph_cache(uint32_t cache_size)
{
    lv_cache_ops_t ops = {
        .create_cb = (lv_cache_create_cb_t)freetype_glyph_create_cb,
        .free_cb = (lv_cache_free_cb_t)freetype_glyph_free_cb,
        .compare_cb = (lv_cache_compare_cb_t)freetype_glyph_compare_cb,
    };

    lv_cache_t * glyph_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(lv_freetype_glyph_cache_data_t),
                                               cache_size, ops);
    lv_cache_set_name(glyph_cache, CACHE_NAME);

    return glyph_cache;
}

void lv_freetype_set_cbs_glyph(lv_freetype_font_dsc_t * dsc)
{
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);
    dsc->font.get_glyph_dsc = freetype_get_glyph_dsc_cb;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool freetype_get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc, uint32_t unicode_letter,
                                      uint32_t unicode_letter_next)
{
    LV_ASSERT_NULL(font);
    LV_ASSERT_NULL(g_dsc);
    LV_PROFILER_FONT_BEGIN;

    if(unicode_letter < 0x20) {
        g_dsc->adv_w  = 0;
        g_dsc->box_h  = 0;
        g_dsc->box_w  = 0;
        g_dsc->ofs_x  = 0;
        g_dsc->ofs_y  = 0;
        g_dsc->format = LV_FONT_GLYPH_FORMAT_NONE;
        LV_PROFILER_FONT_END;
        return true;
    }

    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);

    lv_freetype_glyph_cache_data_t search_key = {
        .unicode = unicode_letter,
        .size = dsc->size,
    };

    lv_cache_t * glyph_cache = dsc->cache_node->glyph_cache;

    lv_cache_entry_t * entry = lv_cache_acquire_or_create(glyph_cache, &search_key, dsc);
    if(entry == NULL) {
        LV_LOG_ERROR("glyph lookup failed for unicode = 0x%" LV_PRIx32, unicode_letter);
        LV_PROFILER_FONT_END;
        return false;
    }
    lv_freetype_glyph_cache_data_t * data = lv_cache_entry_get_data(entry);
    *g_dsc = data->glyph_dsc;

    if((dsc->style & LV_FREETYPE_FONT_STYLE_ITALIC) && (unicode_letter_next == '\0')) {
        g_dsc->adv_w = g_dsc->box_w + g_dsc->ofs_x;
    }

    if(dsc->kerning == LV_FONT_KERNING_NORMAL && dsc->cache_node->face_has_kerning && unicode_letter_next != '\0') {
        lv_mutex_lock(&dsc->cache_node->face_lock);
        FT_Face face = dsc->cache_node->face;
        if(FT_IS_SCALABLE(face) && dsc->cache_node->last_pixel_size != dsc->size) {
            FT_Error set_size_error = FT_Set_Pixel_Sizes(face, 0, dsc->size);
            if(set_size_error) {
                FT_ERROR_MSG("FT_Set_Pixel_Sizes", set_size_error);
            }
            else {
                dsc->cache_node->last_pixel_size = dsc->size;
            }
        }
        FT_UInt glyph_index_next = FT_Get_Char_Index(face, unicode_letter_next);
        FT_Vector kerning;
        FT_Error error = FT_Get_Kerning(face, g_dsc->gid.index, glyph_index_next, FT_KERNING_DEFAULT, &kerning);
        if(!error) {
            g_dsc->adv_w += LV_FREETYPE_F26DOT6_TO_INT(kerning.x);
        }
        else {
            FT_ERROR_MSG("FT_Get_Kerning", error);
        }
        lv_mutex_unlock(&dsc->cache_node->face_lock);
    }

    g_dsc->entry = NULL;

    lv_cache_release(glyph_cache, entry, NULL);
    LV_PROFILER_FONT_END;
    return true;
}

/*-----------------
 * Cache Callbacks
 *----------------*/

static bool freetype_glyph_create_cb(lv_freetype_glyph_cache_data_t * data, void * user_data)
{
    LV_PROFILER_FONT_BEGIN;

    FT_Error error;
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)user_data;
    lv_font_glyph_dsc_t * dsc_out = &data->glyph_dsc;

    lv_mutex_lock(&dsc->cache_node->face_lock);
    FT_Face face = dsc->cache_node->face;

#if LV_USE_HARFBUZZ
    /* If the high bit is set, the "unicode" field is actually a tagged glyph ID
     * from the HarfBuzz shaping path — use it directly, skip cmap lookup. */
    FT_UInt glyph_index;
    if(data->unicode & GID_TAG) {
        glyph_index = (FT_UInt)(data->unicode & ~GID_TAG);
    }
    else {
        glyph_index = FT_Get_Char_Index(face, data->unicode);
    }
#else
    FT_UInt glyph_index = FT_Get_Char_Index(face, data->unicode);
#endif

    if(FT_IS_SCALABLE(face)) {
        if(dsc->cache_node->last_pixel_size != dsc->size) {
            error = FT_Set_Pixel_Sizes(face, 0, dsc->size);
            if(error) {
                FT_ERROR_MSG("FT_Set_Pixel_Sizes", error);
                lv_mutex_unlock(&dsc->cache_node->face_lock);
                return false;
            }
            dsc->cache_node->last_pixel_size = dsc->size;
        }
    }
    else {
        error = FT_Select_Size(face, 0);
        if(error) {
            FT_ERROR_MSG("FT_Select_Size", error);
            lv_mutex_unlock(&dsc->cache_node->face_lock);
            return false;
        }
    }

    if(dsc->render_mode == LV_FREETYPE_FONT_RENDER_MODE_OUTLINE) {
        error = FT_Load_Glyph(face, glyph_index, FT_LOAD_COMPUTE_METRICS | FT_LOAD_NO_BITMAP | FT_LOAD_NO_AUTOHINT);
    }
    else if(dsc->render_mode == LV_FREETYPE_FONT_RENDER_MODE_BITMAP) {
        /* Load AND render in one step so we get metrics + bitmap together.
         * The bitmap is cached in prerender for the image callback to reuse,
         * avoiding a second FT_Load_Glyph call. */
        error = FT_Load_Glyph(face, glyph_index,
                               FT_LOAD_RENDER | FT_LOAD_COLOR | FT_LOAD_TARGET_NORMAL | FT_LOAD_NO_AUTOHINT);
    }
    if(error) {
        FT_ERROR_MSG("FT_Load_Glyph", error);
        lv_mutex_unlock(&dsc->cache_node->face_lock);
        LV_PROFILER_FONT_END;
        return false;
    }

    FT_GlyphSlot glyph = face->glyph;

    if(dsc->render_mode == LV_FREETYPE_FONT_RENDER_MODE_OUTLINE) {

        dsc_out->adv_w = FT_F26DOT6_TO_INT(glyph->metrics.horiAdvance);
        dsc_out->box_h = FT_F26DOT6_TO_INT(glyph->metrics.height);          /*Height of the bitmap in [px]*/
        dsc_out->box_w = FT_F26DOT6_TO_INT(glyph->metrics.width);           /*Width of the bitmap in [px]*/
        dsc_out->ofs_x = FT_F26DOT6_TO_INT(glyph->metrics.horiBearingX);    /*X offset of the bitmap in [pf]*/
        dsc_out->ofs_y = FT_F26DOT6_TO_INT(glyph->metrics.horiBearingY -
                                           glyph->metrics.height);          /*Y offset of the bitmap measured from the as line*/
        dsc_out->format = LV_FONT_GLYPH_FORMAT_VECTOR;

        /*Transform the glyph to italic if required */
        if(dsc->style & LV_FREETYPE_FONT_STYLE_ITALIC) {
            dsc_out->box_w = lv_freetype_italic_transform_on_pos((lv_point_t) {
                dsc_out->box_w, dsc_out->box_h
            });
        }
    }
    else if(dsc->render_mode == LV_FREETYPE_FONT_RENDER_MODE_BITMAP) {
        FT_Bitmap * glyph_bitmap = &face->glyph->bitmap;

        dsc_out->adv_w = FT_F26DOT6_TO_INT(glyph->advance.x);        /*Width of the glyph in [pf]*/
        dsc_out->box_h = glyph_bitmap->rows;                         /*Height of the bitmap in [px]*/
        dsc_out->box_w = glyph_bitmap->width;                        /*Width of the bitmap in [px]*/
        dsc_out->ofs_x = glyph->bitmap_left;                         /*X offset of the bitmap in [pf]*/
        dsc_out->ofs_y = glyph->bitmap_top -
                         dsc_out->box_h;                             /*Y offset of the bitmap measured from the as line*/
        if(glyph_bitmap->pixel_mode == FT_PIXEL_MODE_BGRA)
            dsc_out->format = LV_FONT_GLYPH_FORMAT_IMAGE;
        else
            dsc_out->format = LV_FONT_GLYPH_FORMAT_A8;

        /* Stash the rendered bitmap in prerender cache so the image callback
         * can reuse it without a second FT_Load_Glyph. */
        if(glyph_bitmap->rows > 0 && glyph_bitmap->width > 0) {
            lv_freetype_cache_node_t * cn = dsc->cache_node;
            if(cn->prerender.buffer) {
                lv_free(cn->prerender.buffer);
                cn->prerender.buffer = NULL;
            }
            uint32_t buf_size = (uint32_t)glyph_bitmap->rows * (uint32_t)LV_ABS(glyph_bitmap->pitch);
            cn->prerender.buffer = lv_malloc(buf_size);
            if(cn->prerender.buffer) {
                lv_memcpy(cn->prerender.buffer, glyph_bitmap->buffer, buf_size);
                cn->prerender.rows = glyph_bitmap->rows;
                cn->prerender.width = glyph_bitmap->width;
                cn->prerender.pitch = glyph_bitmap->pitch;
                cn->prerender.pixel_mode = glyph_bitmap->pixel_mode;
                cn->prerender.glyph_index = glyph_index;
                cn->prerender.size = dsc->size;
            }
        }
    }

    dsc_out->is_placeholder = glyph_index == 0;
    dsc_out->gid.index = (uint32_t)glyph_index;

    lv_mutex_unlock(&dsc->cache_node->face_lock);

    LV_PROFILER_FONT_END;
    return true;
}
static void freetype_glyph_free_cb(lv_freetype_glyph_cache_data_t * data, void * user_data)
{
    LV_UNUSED(data);
    LV_UNUSED(user_data);
}
static lv_cache_compare_res_t freetype_glyph_compare_cb(const lv_freetype_glyph_cache_data_t * lhs,
                                                        const lv_freetype_glyph_cache_data_t * rhs)
{
    if(lhs->unicode != rhs->unicode) {
        return lhs->unicode > rhs->unicode ? 1 : -1;
    }
    if(lhs->size != rhs->size) {
        return lhs->size > rhs->size ? 1 : -1;
    }
    return 0;
}

#if LV_USE_HARFBUZZ
bool lv_freetype_get_glyph_dsc_by_gid(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc, uint32_t glyph_id)
{
    LV_ASSERT_NULL(font);
    LV_ASSERT_NULL(g_dsc);

    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);

    /* Use the same glyph cache as unicode lookups, but tag the key with GID_TAG
     * so glyph-ID entries don't collide with unicode entries. */
    lv_freetype_glyph_cache_data_t search_key = {
        .unicode = GID_TAG | glyph_id,
        .size = dsc->size,
    };

    lv_cache_t * glyph_cache = dsc->cache_node->glyph_cache;

    lv_cache_entry_t * entry = lv_cache_acquire_or_create(glyph_cache, &search_key, dsc);
    if(entry == NULL) {
        LV_LOG_ERROR("glyph lookup failed for gid = %" LV_PRIu32, glyph_id);
        return false;
    }
    lv_freetype_glyph_cache_data_t * data = lv_cache_entry_get_data(entry);
    *g_dsc = data->glyph_dsc;

    g_dsc->resolved_font = font;
    g_dsc->entry = NULL;

    lv_cache_release(glyph_cache, entry, NULL);
    return true;
}
#endif /*LV_USE_HARFBUZZ*/

#endif /*LV_USE_FREETYPE*/
