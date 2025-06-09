/**
 * @file lv_font.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_font.h"
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

    const uint8_t save_req = g_dsc->req_raw_bitmap;
    g_dsc->req_raw_bitmap = 0;
    const void * bitmap = font_p->get_glyph_bitmap(g_dsc, draw_buf);
    g_dsc->req_raw_bitmap = save_req;

    return bitmap;
}

const void * lv_font_get_glyph_static_bitmap(lv_font_glyph_dsc_t * g_dsc)
{
    const lv_font_t * font_p = g_dsc->resolved_font;
    LV_ASSERT_NULL(font_p);

    if(font_p->static_bitmap == 0) {
        LV_LOG_WARN("Requesting static bitmap of a non-static bitmap of %p font", (void *)font_p);
        return NULL;
    }

    const uint8_t save_req = g_dsc->req_raw_bitmap;
    g_dsc->req_raw_bitmap = 1;
    const void * bitmap = font_p->get_glyph_bitmap(g_dsc, NULL);
    g_dsc->req_raw_bitmap = save_req;

    return bitmap;
}

void lv_font_glyph_release_draw_data(lv_font_glyph_dsc_t * g_dsc)
{
    LV_ASSERT_NULL(g_dsc);
    if(!g_dsc->entry) {
        return;
    }

    const lv_font_t * font = g_dsc->resolved_font;

    if(font != NULL && font->release_glyph) {
        font->release_glyph(font, g_dsc);
    }
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

    dsc_out->stride = 0;
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
    return LV_FONT_DEFAULT;
}

bool lv_font_info_is_equal(const lv_font_info_t * ft_info_1, const lv_font_info_t * ft_info_2)
{
    LV_ASSERT_NULL(ft_info_1);
    LV_ASSERT_NULL(ft_info_2);

    bool is_equal = (ft_info_1->size == ft_info_2->size
                     && ft_info_1->style == ft_info_2->style
                     && ft_info_1->render_mode == ft_info_2->render_mode
                     && ft_info_1->kerning == ft_info_2->kerning
                     && lv_strcmp(ft_info_1->name, ft_info_2->name) == 0);

    return is_equal;
}

bool lv_font_has_static_bitmap(const lv_font_t * font)
{
    return font->static_bitmap;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
