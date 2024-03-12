/**
 * @file lv_freetype_glyph.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_freetype_private.h"

#if LV_USE_FREETYPE

/*********************
 *      DEFINES
 *********************/

#define LV_FREETYPE_GLYPH_DSC_CACHE_SIZE (LV_FREETYPE_CACHE_FT_GLYPH_CNT * 2)
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

lv_cache_t * lv_freetype_create_glyph_cache(void)
{
    lv_cache_ops_t ops = {
        .create_cb = (lv_cache_create_cb_t)freetype_glyph_create_cb,
        .free_cb = (lv_cache_free_cb_t)freetype_glyph_free_cb,
        .compare_cb = (lv_cache_compare_cb_t)freetype_glyph_compare_cb,
    };

    lv_cache_t * glyph_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(lv_freetype_glyph_cache_data_t),
                                               LV_FREETYPE_GLYPH_DSC_CACHE_SIZE, ops);

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

    if(unicode_letter < 0x20) {
        g_dsc->adv_w  = 0;
        g_dsc->box_h  = 0;
        g_dsc->box_w  = 0;
        g_dsc->ofs_x  = 0;
        g_dsc->ofs_y  = 0;
        g_dsc->format = LV_FONT_GLYPH_FORMAT_NONE;
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
        return false;
    }
    lv_freetype_glyph_cache_data_t * data = lv_cache_entry_get_data(entry);
    *g_dsc = data->glyph_dsc;

    if((dsc->style & LV_FREETYPE_FONT_STYLE_ITALIC) && (unicode_letter_next == '\0')) {
        g_dsc->adv_w = g_dsc->box_w + g_dsc->ofs_x;
    }

    g_dsc->entry = NULL;

    lv_cache_release(glyph_cache, entry, NULL);
    return true;
}

/*-----------------
 * Cache Callbacks
 *----------------*/

static bool freetype_glyph_create_cb(lv_freetype_glyph_cache_data_t * data, void * user_data)
{
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)user_data;

    FT_Error error;

    lv_font_glyph_dsc_t * dsc_out = &data->glyph_dsc;

    FT_Face face = dsc->cache_node->face;
    FT_UInt charmap_index = FT_Get_Charmap_Index(face->charmap);
    FT_UInt glyph_index = FTC_CMapCache_Lookup(dsc->context->cmap_cache, dsc->face_id, charmap_index, data->unicode);

    FT_Set_Pixel_Sizes(face, 0, dsc->size);
    error = FT_Load_Glyph(face, glyph_index,  FT_LOAD_COMPUTE_METRICS | FT_LOAD_NO_BITMAP);
    if(error) {
        FT_ERROR_MSG("FT_Load_Glyph", error);
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

        /*Transform the glyph to italic if required*/
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
        dsc_out->format = LV_FONT_GLYPH_FORMAT_A8;
    }

    dsc_out->is_placeholder = glyph_index == 0;
    dsc_out->glyph_index = glyph_index;

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

#endif /*LV_USE_FREETYPE*/
