/**
 * @file lv_freetype_sbit.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_freetype_private.h"

/*********************
 *      DEFINES
 *********************/

#if LV_USE_FREETYPE && LV_FREETYPE_CACHE_TYPE == LV_FREETYPE_CACHE_TYPE_SBIT

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_freetype_cache_context_t {
    FTC_SBitCache sbit_cache;
};

struct _lv_freetype_cache_node_t {
    FTC_SBit sbit;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool freetype_get_glyph_dsc_cb(const lv_font_t * font,
                                      lv_font_glyph_dsc_t * dsc_out,
                                      uint32_t unicode_letter,
                                      uint32_t unicode_letter_next);

static const uint8_t * freetype_get_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter,
                                                    uint8_t * bitmap_out);

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

    FT_Error error = FTC_SBitCache_New(ctx->cache_manager, &cache_ctx->sbit_cache);
    if(error) {
        FT_ERROR_MSG("FTC_SBitCache_New", error);
        lv_free(cache_ctx);
        return NULL;
    }

    return cache_ctx;
}

void lv_freetype_cache_context_delete(lv_freetype_cache_context_t * cache_ctx)
{
    LV_ASSERT_NULL(cache_ctx);
    lv_free(cache_ctx);
}

bool lv_freetype_on_font_create(lv_freetype_font_dsc_t * dsc)
{
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);
    lv_freetype_cache_node_t * cache_node = lv_malloc(sizeof(lv_freetype_cache_node_t));
    LV_ASSERT_MALLOC(cache_node);
    lv_memzero(cache_node, sizeof(lv_freetype_cache_context_t));
    dsc->cache_node = cache_node;
    dsc->font.get_glyph_dsc = freetype_get_glyph_dsc_cb;
    dsc->font.get_glyph_bitmap = freetype_get_glyph_bitmap_cb;
    return true;
}

void lv_freetype_on_font_delete(lv_freetype_font_dsc_t * dsc)
{
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);
    LV_ASSERT_NULL(dsc->cache_node);
    lv_free(dsc->cache_node);
    dsc->cache_node = NULL;
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

    FT_Error error;

    FT_Size ft_size = lv_freetype_lookup_size(dsc);
    if(!ft_size) {
        return false;
    }

    FT_Face face = ft_size->face;
    FT_UInt charmap_index = FT_Get_Charmap_Index(face->charmap);
    FT_UInt glyph_index = FTC_CMapCache_Lookup(dsc->context->cmap_cache, dsc->face_id, charmap_index, unicode_letter);
    dsc_out->is_placeholder = glyph_index == 0;

    if(dsc->style & LV_FREETYPE_FONT_STYLE_ITALIC) {
        lv_freetype_italic_transform(face);
    }

    FTC_ImageTypeRec desc_type;
    desc_type.face_id = dsc->face_id;
    desc_type.flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;
    desc_type.height = dsc->size;
    desc_type.width = dsc->size;

    error = FTC_SBitCache_Lookup(dsc->context->cache_context->sbit_cache,
                                 &desc_type,
                                 glyph_index,
                                 &dsc->cache_node->sbit,
                                 NULL);
    if(error) {
        FT_ERROR_MSG("FTC_SBitCache_Lookup", error);
        return false;
    }

    FTC_SBit sbit = dsc->cache_node->sbit;
    dsc_out->adv_w = sbit->xadvance;
    dsc_out->box_h = sbit->height;  /*Height of the bitmap in [px]*/
    dsc_out->box_w = sbit->width;   /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = sbit->left;    /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = sbit->top - sbit->height; /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;               /*Bit per pixel: 1/2/4/8*/

    if((dsc->style & LV_FREETYPE_FONT_STYLE_ITALIC) && (unicode_letter_next == '\0')) {
        dsc_out->adv_w = dsc_out->box_w + dsc_out->ofs_x;
    }

    return true;
}

static const uint8_t * freetype_get_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter,
                                                    uint8_t * bitmap_out)
{
    LV_UNUSED(unicode_letter);
    LV_UNUSED(bitmap_out);

    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);

    return (const uint8_t *)dsc->cache_node->sbit->buffer;
}

#endif
