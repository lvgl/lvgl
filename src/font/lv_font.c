/**
 * @file lv_font.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_font.h"
#include "lv_font_cache.h"
#include "../misc/lv_text_private.h"
#include "../misc/lv_utils.h"
#include "../misc/lv_log.h"
#include "../misc/lv_assert.h"
#include "../stdlib/lv_string.h"

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
 *  GLOBAL VARIABLES
 **********************/

const lv_font_t * const lv_font_default = LV_FONT_DEFAULT;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

const void * lv_font_get_glyph_bitmap(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf)
{
    const lv_font_t * font_p = g_dsc->resolved_font;
    LV_ASSERT_NULL(font_p);
#if LV_FONT_CACHE_GLYPH_CNT > 0
    /* If the font cache is enabled, try to use it to get the glyph bitmap */
    return lv_font_cache_get_glyph_bitmap(g_dsc, draw_buf);
#else
    return font_p->get_glyph_bitmap(g_dsc, draw_buf);
#endif
}

lv_result_t lv_font_get_glyph_static_bitmap(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf)
{
    LV_ASSERT_NULL(g_dsc);
    LV_ASSERT_NULL(draw_buf);
    const lv_font_t * font_p = g_dsc->resolved_font;

    if(!lv_font_has_static_bitmap(font_p)) {
        return LV_RESULT_INVALID;
    }

    return font_p->get_glyph_bitmap(g_dsc, draw_buf) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

void lv_font_glyph_release_draw_data(lv_font_glyph_dsc_t * g_dsc)
{
    LV_ASSERT_NULL(g_dsc);
    if(!g_dsc->entry) {
        return;
    }

    const lv_font_t * font = g_dsc->resolved_font;

    if(!font) {
        return;
    }

    if(font->release_glyph) {
        font->release_glyph(font, g_dsc);
        return;
    }

#if LV_FONT_CACHE_GLYPH_CNT > 0
    /* Since the old font resource does not register the release_glyph function in the font structure,
     * it is processed here as a fallback to ensure forward compatibility.
     */
    lv_font_cache_release_glyph(font, g_dsc);
#endif
}

bool lv_font_get_glyph_dsc(const lv_font_t * font_p, lv_font_glyph_dsc_t * dsc_out, uint32_t letter,
                           uint32_t letter_next)
{

    LV_ASSERT_NULL(font_p);
    LV_ASSERT_NULL(dsc_out);

#if LV_USE_FONT_PLACEHOLDER
    const lv_font_t * placeholder_font = NULL;
#endif

    const lv_font_t * f = font_p;

    lv_memzero(dsc_out, sizeof(lv_font_glyph_dsc_t));

    while(f) {
        bool found = f->get_glyph_dsc(f, dsc_out, letter, f->kerning == LV_FONT_KERNING_NONE ? 0 : letter_next);
        if(found) {
            if(!dsc_out->is_placeholder) {
                dsc_out->resolved_font = f;
                return true;
            }
#if LV_USE_FONT_PLACEHOLDER
            else if(placeholder_font == NULL) {
                placeholder_font = f;
            }
#endif
        }
        f = f->fallback;
    }

#if LV_USE_FONT_PLACEHOLDER
    if(placeholder_font != NULL) {
        placeholder_font->get_glyph_dsc(placeholder_font, dsc_out, letter,
                                        placeholder_font->kerning == LV_FONT_KERNING_NONE ? 0 : letter_next);
        dsc_out->resolved_font = placeholder_font;
        return true;
    }
#endif

#if LV_USE_FONT_PLACEHOLDER
    dsc_out->box_w = font_p->line_height / 2;
    dsc_out->adv_w = dsc_out->box_w + 2;
#else
    dsc_out->box_w = 0;
    dsc_out->adv_w = 0;
#endif

    dsc_out->resolved_font = NULL;
    dsc_out->box_h = font_p->line_height;
    dsc_out->ofs_x = 0;
    dsc_out->ofs_y = 0;
    dsc_out->format = LV_FONT_GLYPH_FORMAT_A1;
    dsc_out->is_placeholder = true;

    return false;
}

uint16_t lv_font_get_glyph_width(const lv_font_t * font, uint32_t letter, uint32_t letter_next)
{
    LV_ASSERT_NULL(font);
    lv_font_glyph_dsc_t g;

    /*Return zero if letter is marker*/
    if(lv_text_is_marker(letter)) return 0;

    lv_font_get_glyph_dsc(font, &g, letter, letter_next);
    return g.adv_w;
}

void lv_font_set_kerning(lv_font_t * font, lv_font_kerning_t kerning)
{
    LV_ASSERT_NULL(font);
    font->kerning = kerning;
}

int32_t lv_font_get_line_height(const lv_font_t * font)
{
    return font->line_height;
}


const lv_font_t * lv_font_get_default(void)
{
    return lv_font_default;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
