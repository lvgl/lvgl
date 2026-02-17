/**
 * @file lv_freetype_harfbuzz.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"
#include "lv_freetype_private.h"
#include "lv_freetype_harfbuzz.h"

#if LV_USE_FREETYPE && LV_USE_HARFBUZZ

#include <hb.h>
#include <hb-ft.h>

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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_freetype_is_harfbuzz_font(const lv_font_t * font)
{
    if(font == NULL || font->dsc == NULL) return false;
    const lv_freetype_font_dsc_t * dsc = (const lv_freetype_font_dsc_t *)font->dsc;
    return LV_FREETYPE_FONT_DSC_HAS_MAGIC_NUM(dsc);
}

lv_hb_shaped_text_t * lv_hb_shape_text(const lv_font_t * font, const char * text, uint32_t byte_len)
{
    if(font == NULL || text == NULL || byte_len == 0) return NULL;

    const lv_freetype_font_dsc_t * dsc = (const lv_freetype_font_dsc_t *)font->dsc;
    if(!LV_FREETYPE_FONT_DSC_HAS_MAGIC_NUM(dsc)) {
        LV_LOG_WARN("lv_hb_shape_text: not a FreeType font");
        return NULL;
    }

    lv_freetype_cache_node_t * cache_node = dsc->cache_node;

    lv_mutex_lock(&cache_node->face_lock);
    FT_Face face = cache_node->face;

    /* Set pixel size before creating HarfBuzz font */
    FT_Error error = FT_Set_Pixel_Sizes(face, 0, dsc->size);
    if(error) {
        FT_ERROR_MSG("FT_Set_Pixel_Sizes", error);
        lv_mutex_unlock(&cache_node->face_lock);
        return NULL;
    }

    /* Create HarfBuzz font from FreeType face */
    hb_font_t * hb_font = hb_ft_font_create_referenced(face);
    if(hb_font == NULL) {
        LV_LOG_ERROR("hb_ft_font_create_referenced failed");
        lv_mutex_unlock(&cache_node->face_lock);
        return NULL;
    }

    /* Create and configure HarfBuzz buffer */
    hb_buffer_t * hb_buf = hb_buffer_create();
    if(!hb_buffer_allocation_successful(hb_buf)) {
        LV_LOG_ERROR("hb_buffer_create failed");
        hb_font_destroy(hb_font);
        lv_mutex_unlock(&cache_node->face_lock);
        return NULL;
    }

    hb_buffer_add_utf8(hb_buf, text, (int)byte_len, 0, (int)byte_len);
    hb_buffer_guess_segment_properties(hb_buf);

    /* Perform shaping */
    hb_shape(hb_font, hb_buf, NULL, 0);

    /* Extract results */
    unsigned int glyph_count = 0;
    hb_glyph_info_t * hb_glyph_infos = hb_buffer_get_glyph_infos(hb_buf, &glyph_count);
    hb_glyph_position_t * hb_glyph_positions = hb_buffer_get_glyph_positions(hb_buf, &glyph_count);

    if(glyph_count == 0) {
        hb_buffer_destroy(hb_buf);
        hb_font_destroy(hb_font);
        lv_mutex_unlock(&cache_node->face_lock);
        return NULL;
    }

    /* Allocate result */
    lv_hb_shaped_text_t * result = lv_malloc(sizeof(lv_hb_shaped_text_t));
    LV_ASSERT_MALLOC(result);
    if(result == NULL) {
        hb_buffer_destroy(hb_buf);
        hb_font_destroy(hb_font);
        lv_mutex_unlock(&cache_node->face_lock);
        return NULL;
    }

    result->glyphs = lv_malloc(sizeof(lv_hb_glyph_info_t) * glyph_count);
    LV_ASSERT_MALLOC(result->glyphs);
    if(result->glyphs == NULL) {
        lv_free(result);
        hb_buffer_destroy(hb_buf);
        hb_font_destroy(hb_font);
        lv_mutex_unlock(&cache_node->face_lock);
        return NULL;
    }

    result->count = glyph_count;

    /* Convert HarfBuzz results (26.6 fixed-point) to pixels */
    for(unsigned int i = 0; i < glyph_count; i++) {
        result->glyphs[i].glyph_id = hb_glyph_infos[i].codepoint;
        result->glyphs[i].x_offset = hb_glyph_positions[i].x_offset / 64;
        result->glyphs[i].y_offset = hb_glyph_positions[i].y_offset / 64;
        result->glyphs[i].x_advance = hb_glyph_positions[i].x_advance / 64;
        result->glyphs[i].y_advance = hb_glyph_positions[i].y_advance / 64;
        result->glyphs[i].cluster = hb_glyph_infos[i].cluster;
    }

    hb_buffer_destroy(hb_buf);
    hb_font_destroy(hb_font);
    lv_mutex_unlock(&cache_node->face_lock);

    return result;
}

void lv_hb_shaped_text_destroy(lv_hb_shaped_text_t * shaped)
{
    if(shaped == NULL) return;
    if(shaped->glyphs) lv_free(shaped->glyphs);
    lv_free(shaped);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_FREETYPE && LV_USE_HARFBUZZ */
