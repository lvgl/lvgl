/**
 * @file lv_freetype_harfbuzz.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"
#include "../../misc/lv_text_private.h"
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

lv_hb_shaped_text_t * lv_hb_shape_text(const lv_font_t * font, const char * text, uint32_t byte_len,
                                        lv_base_dir_t dir_hint)
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

    /* Set pixel size before creating/using HarfBuzz font */
    FT_Error error = FT_Set_Pixel_Sizes(face, 0, dsc->size);
    if(error) {
        FT_ERROR_MSG("FT_Set_Pixel_Sizes", error);
        lv_mutex_unlock(&cache_node->face_lock);
        return NULL;
    }

    /* Get or create cached HarfBuzz font.
     * Recreate if the pixel size has changed since the last call. */
    hb_font_t * hb_font = (hb_font_t *)cache_node->hb_font;
    if(hb_font == NULL || cache_node->hb_font_size != dsc->size) {
        if(hb_font) {
            hb_font_destroy(hb_font);
        }
        hb_font = hb_ft_font_create_referenced(face);
        if(hb_font == NULL) {
            LV_LOG_ERROR("hb_ft_font_create_referenced failed");
            cache_node->hb_font = NULL;
            lv_mutex_unlock(&cache_node->face_lock);
            return NULL;
        }
        cache_node->hb_font = hb_font;
        cache_node->hb_font_size = dsc->size;
    }

    /* Create and configure HarfBuzz buffer */
    hb_buffer_t * hb_buf = hb_buffer_create();
    if(!hb_buffer_allocation_successful(hb_buf)) {
        LV_LOG_ERROR("hb_buffer_create failed");
        lv_mutex_unlock(&cache_node->face_lock);
        return NULL;
    }

    hb_buffer_add_utf8(hb_buf, text, (int)byte_len, 0, (int)byte_len);

    /* Set direction based on hint to avoid double-reordering when LVGL BIDI
     * has already processed the text into visual order. */
    if(dir_hint == LV_BASE_DIR_LTR) {
        hb_buffer_set_direction(hb_buf, HB_DIRECTION_LTR);
        hb_buffer_guess_segment_properties(hb_buf); /* still guess script and language */
        hb_buffer_set_direction(hb_buf, HB_DIRECTION_LTR); /* re-force after guess */
    }
    else if(dir_hint == LV_BASE_DIR_RTL) {
        hb_buffer_set_direction(hb_buf, HB_DIRECTION_RTL);
        hb_buffer_guess_segment_properties(hb_buf);
        hb_buffer_set_direction(hb_buf, HB_DIRECTION_RTL);
    }
    else {
        /* LV_BASE_DIR_AUTO: let HarfBuzz auto-detect everything */
        hb_buffer_guess_segment_properties(hb_buf);
    }

    /* Perform shaping */
    hb_shape(hb_font, hb_buf, NULL, 0);

    /* Extract results */
    unsigned int glyph_count = 0;
    hb_glyph_info_t * hb_glyph_infos = hb_buffer_get_glyph_infos(hb_buf, &glyph_count);
    hb_glyph_position_t * hb_glyph_positions = hb_buffer_get_glyph_positions(hb_buf, &glyph_count);

    if(glyph_count == 0) {
        hb_buffer_destroy(hb_buf);
        lv_mutex_unlock(&cache_node->face_lock);
        return NULL;
    }

    /* Allocate result */
    lv_hb_shaped_text_t * result = lv_malloc(sizeof(lv_hb_shaped_text_t));
    LV_ASSERT_MALLOC(result);
    if(result == NULL) {
        hb_buffer_destroy(hb_buf);
        lv_mutex_unlock(&cache_node->face_lock);
        return NULL;
    }

    result->glyphs = lv_malloc(sizeof(lv_hb_glyph_info_t) * glyph_count);
    LV_ASSERT_MALLOC(result->glyphs);
    if(result->glyphs == NULL) {
        lv_free(result);
        hb_buffer_destroy(hb_buf);
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
    lv_mutex_unlock(&cache_node->face_lock);

    return result;
}

void lv_hb_shaped_text_destroy(lv_hb_shaped_text_t * shaped)
{
    if(shaped == NULL) return;
    if(shaped->glyphs) lv_free(shaped->glyphs);
    lv_free(shaped);
}

int32_t lv_hb_get_text_width(const lv_font_t * font, const char * text, uint32_t byte_len, int32_t letter_space)
{
    if(font == NULL || text == NULL || byte_len == 0) return 0;

    lv_hb_shaped_text_t * shaped = lv_hb_shape_text(font, text, byte_len, LV_BASE_DIR_AUTO);
    if(shaped == NULL) return -1;

    int32_t width = 0;
    for(uint32_t i = 0; i < shaped->count; i++) {
        int32_t glyph_w = shaped->glyphs[i].x_advance;

        /* For .notdef glyphs (glyph_id == 0), try the fallback font chain */
        if(shaped->glyphs[i].glyph_id == 0 && font->fallback != NULL) {
            uint32_t tmp_ofs = shaped->glyphs[i].cluster;
            uint32_t letter = lv_text_encoded_next(text, &tmp_ofs);
            if(letter) {
                glyph_w = lv_font_get_glyph_width(font->fallback, letter, 0);
            }
        }

        if(glyph_w > 0) {
            width += glyph_w + letter_space;
        }
    }

    /* Trim the last letter space */
    if(width > 0) {
        width -= letter_space;
    }

    lv_hb_shaped_text_destroy(shaped);
    return width;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_FREETYPE && LV_USE_HARFBUZZ */
