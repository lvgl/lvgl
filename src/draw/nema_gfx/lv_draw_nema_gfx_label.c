/**
 * MIT License
 *
 * -----------------------------------------------------------------------------
 * Copyright (c) 2008-24 Think Silicon Single Member PC
 * -----------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/**
 * @file lv_draw_nema_gfx_fill.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_nema_gfx.h"

#if LV_USE_NEMA_GFX
#include "../../font/lv_font.h"
#include "../../font/lv_font_fmt_txt.h"
#include "../../misc/lv_utils.h"
#include "../../misc/lv_text_private.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LABEL_HINT_UPDATE_TH 1024 /*Update the "hint" if the label's y coordinates have changed more then this*/

/*Forward declarations*/
void nema_set_matrix(nema_matrix3x3_t m);
void nema_raster_rect(int x, int y, int w, int h);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _draw_nema_gfx_letter(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                  lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

static void _draw_label_iterate_characters(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc,
                                           const lv_area_t * coords);

static inline uint8_t _bpp_nema_gfx_format(lv_draw_glyph_dsc_t * glyph_draw_dsc);

static void _draw_letter(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * dsc,  const lv_point_t * pos,
                         const lv_font_t * font, uint32_t letter);

static uint32_t get_glyph_dsc_id(const lv_font_t * font, uint32_t letter);

static int unicode_list_compare(const void * ref, const void * element)
{
    return ((int32_t)(*(uint16_t *)ref)) - ((int32_t)(*(uint16_t *)element));
}

static bool raw_bitmap = false;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_nema_gfx_label(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_layer_t * layer = draw_unit->target_layer;

    lv_area_t clip_area;
    lv_area_copy(&clip_area, draw_unit->clip_area);
    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_color_format_t dst_cf = layer->draw_buf->header.cf;
    uint32_t dst_nema_cf = lv_nemagfx_cf_to_nema(dst_cf);

    nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(layer->draw_buf->data), lv_area_get_width(&(layer->buf_area)),
                      lv_area_get_height(&(layer->buf_area)), dst_nema_cf,
                      lv_area_get_width(&(layer->buf_area))*lv_color_format_get_size(dst_cf));

    nema_set_clip(clip_area.x1, clip_area.y1, lv_area_get_width(&clip_area), lv_area_get_height(&clip_area));

    _draw_label_iterate_characters(draw_unit, dsc, coords);

    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)draw_unit;
    nema_cl_submit(&(draw_nema_gfx_unit->cl));
    nema_cl_wait(&(draw_nema_gfx_unit->cl));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static inline uint8_t _bpp_nema_gfx_format(lv_draw_glyph_dsc_t * glyph_draw_dsc)
{
    uint32_t format = glyph_draw_dsc->g->format;

    switch(format) {
        case LV_FONT_GLYPH_FORMAT_A1:
            return NEMA_A1;
        case LV_FONT_GLYPH_FORMAT_A2:
            return NEMA_A2;
        case LV_FONT_GLYPH_FORMAT_A4:
            return NEMA_A4;
        default:
            return NEMA_A8;
    }
}

static void _draw_nema_gfx_letter(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                  lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    if(glyph_draw_dsc) {
        if(glyph_draw_dsc->format == LV_FONT_GLYPH_FORMAT_NONE) {
#if LV_USE_FONT_PLACEHOLDER
            /* Draw a placeholder rectangle*/
            lv_draw_border_dsc_t border_draw_dsc;
            lv_draw_border_dsc_init(&border_draw_dsc);
            border_draw_dsc.opa = glyph_draw_dsc->opa;
            border_draw_dsc.color = glyph_draw_dsc->color;
            border_draw_dsc.width = 1;
            lv_draw_nema_gfx_border(draw_unit, &border_draw_dsc, glyph_draw_dsc->bg_coords);
#endif
        }
        else if(glyph_draw_dsc->format >= LV_FONT_GLYPH_FORMAT_A1 &&
                glyph_draw_dsc->format <= LV_FONT_GLYPH_FORMAT_A8) {
            /*Do not draw transparent things*/
            if(glyph_draw_dsc->opa <= LV_OPA_MIN)
                return;

            lv_layer_t * layer = draw_unit->target_layer;

            lv_area_t blend_area;
            if(!lv_area_intersect(&blend_area, glyph_draw_dsc->letter_coords, draw_unit->clip_area))
                return;
            lv_area_move(&blend_area, -layer->buf_area.x1, -layer->buf_area.y1);

            const lv_draw_buf_t * draw_buf = glyph_draw_dsc->glyph_data;
            const void * mask_buf = draw_buf->data;


            int32_t x = glyph_draw_dsc->letter_coords->x1 - layer->buf_area.x1;
            int32_t y = glyph_draw_dsc->letter_coords->y1 - layer->buf_area.y1;
            int32_t w = glyph_draw_dsc->g->box_w;
            int32_t h = glyph_draw_dsc->g->box_h;

            if(raw_bitmap) {
                nema_bind_src_tex((uintptr_t)(mask_buf), w * h, 1, _bpp_nema_gfx_format(glyph_draw_dsc), 0, NEMA_FILTER_PS);

                nema_matrix3x3_t m = {
                    {1,    w,   -x - (y * w) - (0.5 * w)},
                    {0,    1,                   0},
                    {0,    0,                   1}
                };

                nema_set_matrix(m);
                nema_raster_rect(x, y, w, h);
            }
            else {
                nema_bind_src_tex((uintptr_t)(mask_buf), w, h, NEMA_A8, w, NEMA_FILTER_PS);
                nema_blit(x, y);
            }
        }
        else if(glyph_draw_dsc->format == LV_FONT_GLYPH_FORMAT_IMAGE) {
#if LV_USE_IMGFONT
            lv_draw_img_dsc_t img_dsc;
            lv_draw_img_dsc_init(&img_dsc);
            img_dsc.angle = 0;
            img_dsc.zoom = LV_ZOOM_NONE;
            img_dsc.opa = glyph_draw_dsc->opa;
            img_dsc.src = glyph_draw_dsc->glyph_data;
            lv_draw_nema_gfx_img(draw_unit, &img_dsc, glyph_draw_dsc->letter_coords);
#endif
        }
    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_nema_gfx_fill(draw_unit, fill_draw_dsc, fill_area);
    }

}

static inline void _set_color_blend(uint32_t color, uint8_t alpha)
{
    nema_set_tex_color(color);

    if(alpha < 255U) {
        nema_set_blend_blit(NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_A);
        nema_set_const_color(color);
    }
    else {
        nema_set_blend_blit(NEMA_BL_SIMPLE);
    }
}

static void _draw_label_iterate_characters(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc,
                                           const lv_area_t * coords)
{
    const lv_font_t * font = dsc->font;
    int32_t w;

    lv_area_t clipped_area;
    bool clip_ok = lv_area_intersect(&clipped_area, coords, draw_unit->clip_area);
    if(!clip_ok) return;

    lv_text_align_t align = dsc->align;
    lv_base_dir_t base_dir = dsc->bidi_dir;

    lv_bidi_calculate_align(&align, &base_dir, dsc->text);

    if((dsc->flag & LV_TEXT_FLAG_EXPAND) == 0) {
        /*Normally use the label's width as width*/
        w = lv_area_get_width(coords);
    }
    else {
        /*If EXPAND is enabled then not limit the text's width to the object's width*/
        lv_point_t p;
        lv_text_get_size(&p, dsc->text, dsc->font, dsc->letter_space, dsc->line_space, LV_COORD_MAX,
                         dsc->flag);
        w = p.x;
    }

    int32_t line_height_font = lv_font_get_line_height(font);
    int32_t line_height = line_height_font + dsc->line_space;

    /*Init variables for the first line*/
    int32_t line_width = 0;
    lv_point_t pos;
    lv_point_set(&pos, coords->x1, coords->y1);

    int32_t x_ofs = 0;
    int32_t y_ofs = 0;
    x_ofs = dsc->ofs_x;
    y_ofs = dsc->ofs_y;
    pos.y += y_ofs;

    uint32_t line_start     = 0;
    int32_t last_line_start = -1;

    /*Check the hint to use the cached info*/
    if(dsc->hint && y_ofs == 0 && coords->y1 < 0) {
        /*If the label changed too much recalculate the hint.*/
        if(LV_ABS(dsc->hint->coord_y - coords->y1) > LV_LABEL_HINT_UPDATE_TH - 2 * line_height) {
            dsc->hint->line_start = -1;
        }
        last_line_start = dsc->hint->line_start;
    }

    /*Use the hint if it's valid*/
    if(dsc->hint && last_line_start >= 0) {
        line_start = last_line_start;
        pos.y += dsc->hint->y;
    }

    uint32_t line_end = line_start + lv_text_get_next_line(&dsc->text[line_start], font, dsc->letter_space, w, NULL,
                                                           dsc->flag);

    /*Go the first visible line*/
    while(pos.y + line_height_font < draw_unit->clip_area->y1) {
        /*Go to next line*/
        line_start = line_end;
        line_end += lv_text_get_next_line(&dsc->text[line_start], font, dsc->letter_space, w, NULL, dsc->flag);
        pos.y += line_height;

        /*Save at the threshold coordinate*/
        if(dsc->hint && pos.y >= -LV_LABEL_HINT_UPDATE_TH && dsc->hint->line_start < 0) {
            dsc->hint->line_start = line_start;
            dsc->hint->y          = pos.y - coords->y1;
            dsc->hint->coord_y    = coords->y1;
        }

        if(dsc->text[line_start] == '\0') return;
    }

    /*Align to middle*/
    if(align == LV_TEXT_ALIGN_CENTER) {
        line_width = lv_text_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space);

        pos.x += (lv_area_get_width(coords) - line_width) / 2;

    }
    /*Align to the right*/
    else if(align == LV_TEXT_ALIGN_RIGHT) {
        line_width = lv_text_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space);
        pos.x += lv_area_get_width(coords) - line_width;
    }

    uint32_t sel_start = dsc->sel_start;
    uint32_t sel_end = dsc->sel_end;
    if(sel_start > sel_end) {
        uint32_t tmp = sel_start;
        sel_start = sel_end;
        sel_end = tmp;
    }

    lv_area_t bg_coords;
    lv_draw_glyph_dsc_t draw_letter_dsc;
    lv_draw_glyph_dsc_init(&draw_letter_dsc);
    draw_letter_dsc.opa = dsc->opa;
    draw_letter_dsc.bg_coords = &bg_coords;
    draw_letter_dsc.color = dsc->color;

    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.opa = dsc->opa;
    int32_t underline_width = font->underline_thickness ? font->underline_thickness : 1;
    int32_t line_start_x;
    uint32_t i;
    int32_t letter_w;

    lv_color32_t dsc_col32 = lv_color_to_32(dsc->color, dsc->opa);
    uint32_t nema_dsc_color = nema_rgba(dsc_col32.red, dsc_col32.green, dsc_col32.blue, dsc_col32.alpha);
    lv_color32_t dsc_sel_col32 = lv_color_to_32(dsc->sel_color, dsc->opa);
    uint32_t nema_dsc_sel_color = nema_rgba(dsc_sel_col32.red, dsc_sel_col32.green, dsc_sel_col32.blue,
                                            dsc_sel_col32.alpha);

    _set_color_blend(nema_dsc_color, dsc_col32.alpha);

    uint32_t is_dsc_color_enabled = 1U;

    /*Write out all lines*/
    while(dsc->text[line_start] != '\0') {
        pos.x += x_ofs;
        line_start_x = pos.x;

        /*Write all letter of a line*/
        i = 0;
#if LV_USE_BIDI
        char * bidi_txt = lv_malloc(line_end - line_start + 1);
        LV_ASSERT_MALLOC(bidi_txt);
        _lv_bidi_process_paragraph(dsc->text + line_start, bidi_txt, line_end - line_start, base_dir, NULL, 0);
#else
        const char * bidi_txt = dsc->text + line_start;
#endif

        while(i < line_end - line_start) {
            uint32_t logical_char_pos = 0;
            if(sel_start != 0xFFFF && sel_end != 0xFFFF) {
#if LV_USE_BIDI
                logical_char_pos = lv_text_encoded_get_char_id(dsc->text, line_start);
                uint32_t t = lv_text_encoded_get_char_id(bidi_txt, i);
                logical_char_pos += _lv_bidi_get_logical_pos(bidi_txt, NULL, line_end - line_start, base_dir, t, NULL);
#else
                logical_char_pos = lv_text_encoded_get_char_id(dsc->text, line_start + i);
#endif
            }

            uint32_t letter;
            uint32_t letter_next;
            lv_text_encoded_letter_next_2(bidi_txt, &letter, &letter_next, &i);

            letter_w = lv_font_get_glyph_width(font, letter, letter_next);

            /*Always set the bg_coordinates for placeholder drawing*/
            bg_coords.x1 = pos.x;
            bg_coords.y1 = pos.y;
            bg_coords.x2 = pos.x + letter_w - 1;
            bg_coords.y2 = pos.y + line_height - 1;

            if(i >= line_end - line_start) {
                if(dsc->decor & LV_TEXT_DECOR_UNDERLINE) {
                    lv_area_t fill_area;
                    fill_area.x1 = line_start_x;
                    fill_area.x2 = pos.x + letter_w - 1;
                    fill_area.y1 = pos.y + font->line_height - font->base_line - font->underline_position;
                    fill_area.y2 = fill_area.y1 + underline_width - 1;

                    fill_dsc.color = dsc->color;
                    lv_draw_nema_gfx_fill(draw_unit, &fill_dsc, &fill_area);
                }
                if(dsc->decor & LV_TEXT_DECOR_STRIKETHROUGH) {
                    lv_area_t fill_area;
                    fill_area.x1 = line_start_x;
                    fill_area.x2 = pos.x + letter_w - 1;
                    fill_area.y1 = pos.y + (font->line_height - font->base_line) * 2 / 3 + font->underline_thickness / 2;
                    fill_area.y2 = fill_area.y1 + underline_width - 1;

                    fill_dsc.color = dsc->color;
                    lv_draw_nema_gfx_fill(draw_unit, &fill_dsc, &fill_area);
                }
            }

            if(sel_start != 0xFFFF && sel_end != 0xFFFF && logical_char_pos >= sel_start && logical_char_pos < sel_end) {
                draw_letter_dsc.color = dsc->sel_color;
                fill_dsc.color = dsc->sel_bg_color;
                lv_draw_nema_gfx_fill(draw_unit, &fill_dsc, &bg_coords);

                if(is_dsc_color_enabled) {
                    _set_color_blend(nema_dsc_sel_color, dsc_sel_col32.alpha);
                    is_dsc_color_enabled = 0U;
                }
            }
            else {
                draw_letter_dsc.color = dsc->color;
                if(!is_dsc_color_enabled) {
                    _set_color_blend(nema_dsc_color, dsc_col32.alpha);
                    is_dsc_color_enabled = 1U;
                }
            }

            _draw_letter(draw_unit, &draw_letter_dsc, &pos, font, letter);

            if(letter_w > 0) {
                pos.x += letter_w + dsc->letter_space;
            }
        }

#if LV_USE_BIDI
        lv_free(bidi_txt);
        bidi_txt = NULL;
#endif
        /*Go to next line*/
        line_start = line_end;
        line_end += lv_text_get_next_line(&dsc->text[line_start], font, dsc->letter_space, w, NULL, dsc->flag);

        pos.x = coords->x1;
        /*Align to middle*/
        if(align == LV_TEXT_ALIGN_CENTER) {
            line_width =
                lv_text_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space);

            pos.x += (lv_area_get_width(coords) - line_width) / 2;
        }
        /*Align to the right*/
        else if(align == LV_TEXT_ALIGN_RIGHT) {
            line_width =
                lv_text_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space);
            pos.x += lv_area_get_width(coords) - line_width;
        }

        /*Go the next line position*/
        pos.y += line_height;

        if(pos.y > draw_unit->clip_area->y2) break;
    }

    if(draw_letter_dsc._draw_buf) lv_draw_buf_destroy(draw_letter_dsc._draw_buf);

    LV_ASSERT_MEM_INTEGRITY();
}

static void _draw_letter(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * dsc,  const lv_point_t * pos,
                         const lv_font_t * font, uint32_t letter)
{
    lv_font_glyph_dsc_t g;

    if(lv_text_is_marker(letter)) /*Markers are valid letters but should not be rendered.*/
        return;

    LV_PROFILER_BEGIN;
    bool g_ret = lv_font_get_glyph_dsc(font, &g, letter, '\0');
    if(g_ret == false) {
        /*Add warning if the dsc is not found*/
        LV_LOG_WARN("lv_draw_letter: glyph dsc. not found for U+%" LV_PRIX32, letter);
    }

    /*Don't draw anything if the character is empty. E.g. space*/
    if((g.box_h == 0) || (g.box_w == 0)) {
        LV_PROFILER_END;
        return;
    }

    lv_area_t letter_coords;
    letter_coords.x1 = pos->x + g.ofs_x;
    letter_coords.x2 = letter_coords.x1 + g.box_w - 1;
    letter_coords.y1 = pos->y + (font->line_height - font->base_line) - g.box_h - g.ofs_y;
    letter_coords.y2 = letter_coords.y1 + g.box_h - 1;

    /*If the letter is completely out of mask don't draw it*/
    if(lv_area_is_out(&letter_coords, draw_unit->clip_area, 0) &&
       lv_area_is_out(dsc->bg_coords, draw_unit->clip_area, 0)) {
        LV_PROFILER_END;
        return;
    }

    if(g.resolved_font) {
        lv_draw_buf_t * draw_buf = NULL;
        if(LV_FONT_GLYPH_FORMAT_NONE < g.format && g.format < LV_FONT_GLYPH_FORMAT_IMAGE) {
            /*Only check draw buf for bitmap glyph*/
            draw_buf = lv_draw_buf_reshape(dsc->_draw_buf, 0, g.box_w, g.box_h, LV_STRIDE_AUTO);
            if(draw_buf == NULL) {
                if(dsc->_draw_buf) lv_draw_buf_destroy(dsc->_draw_buf);

                uint32_t h = g.box_h;
                if(h * g.box_w < 64) h *= 2; /*Alloc a slightly larger buffer*/
                draw_buf = lv_draw_buf_create(g.box_w, h, LV_COLOR_FORMAT_A8, LV_STRIDE_AUTO);
                LV_ASSERT_MALLOC(draw_buf);
                draw_buf->header.h = g.box_h;
                dsc->_draw_buf = draw_buf;
            }
        }
        raw_bitmap = false;
        /* Performance Optimization for lv_font_fmt_txt_dsc_t fonts */
        if(font->get_glyph_bitmap == lv_font_get_bitmap_fmt_txt) {
            lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
            if(fdsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN) {
                const lv_font_t * font_p = g.resolved_font;
                LV_ASSERT_NULL(font_p);

                if(letter == '\t') letter = ' ';

                lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
                uint32_t gid = get_glyph_dsc_id(font, letter);
                if(!gid) dsc->glyph_data = NULL;

                const lv_font_fmt_txt_glyph_dsc_t * gdsc = &fdsc->glyph_dsc[gid];

                int32_t gsize = (int32_t) gdsc->box_w * gdsc->box_h;
                if(gsize == 0) dsc->glyph_data = NULL;
                /*NemaGFX can handle A1, A2, A4 and A8 formats, no need to allocate A8 buffers.
                We will use the original font bitmap, by rewriting the data in draw_buf
                */
                if(draw_buf != NULL) {
                    const uint8_t * bitmap_in = &fdsc->glyph_bitmap[gdsc->bitmap_index];
                    draw_buf->data = (uint8_t *) bitmap_in;
                }
                dsc->glyph_data = (void *)draw_buf;
                raw_bitmap = true;
            }
        }

        if(!raw_bitmap) {
            dsc->glyph_data = (void *)lv_font_get_glyph_bitmap(&g, draw_buf);
        }

        dsc->format = dsc->glyph_data ? g.format : LV_FONT_GLYPH_FORMAT_NONE;
    }
    else {
        dsc->format = LV_FONT_GLYPH_FORMAT_NONE;
    }

    dsc->letter_coords = &letter_coords;
    dsc->g = &g;
    _draw_nema_gfx_letter(draw_unit, dsc, NULL, NULL);

    if(g.resolved_font && font->release_glyph) {
        lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)draw_unit;
        nema_cl_submit(&(draw_nema_gfx_unit->cl));
        nema_cl_wait(&(draw_nema_gfx_unit->cl));
        font->release_glyph(font, &g);
    }
    LV_PROFILER_END;
}

static uint32_t get_glyph_dsc_id(const lv_font_t * font, uint32_t letter)
{
    if(letter == '\0') return 0;

    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;

    uint16_t i;
    for(i = 0; i < fdsc->cmap_num; i++) {

        /*Relative code point*/
        uint32_t rcp = letter - fdsc->cmaps[i].range_start;
        if(rcp >= fdsc->cmaps[i].range_length) continue;
        uint32_t glyph_id = 0;
        if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY) {
            glyph_id = fdsc->cmaps[i].glyph_id_start + rcp;
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL) {
            const uint8_t * gid_ofs_8 = fdsc->cmaps[i].glyph_id_ofs_list;
            glyph_id = fdsc->cmaps[i].glyph_id_start + gid_ofs_8[rcp];
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_SPARSE_TINY) {
            uint16_t key = rcp;
            uint16_t * p = lv_utils_bsearch(&key, fdsc->cmaps[i].unicode_list, fdsc->cmaps[i].list_length,
                                            sizeof(fdsc->cmaps[i].unicode_list[0]), unicode_list_compare);

            if(p) {
                lv_uintptr_t ofs = p - fdsc->cmaps[i].unicode_list;
                glyph_id = fdsc->cmaps[i].glyph_id_start + ofs;
            }
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_SPARSE_FULL) {
            uint16_t key = rcp;
            uint16_t * p = lv_utils_bsearch(&key, fdsc->cmaps[i].unicode_list, fdsc->cmaps[i].list_length,
                                            sizeof(fdsc->cmaps[i].unicode_list[0]), unicode_list_compare);

            if(p) {
                lv_uintptr_t ofs = p - fdsc->cmaps[i].unicode_list;
                const uint16_t * gid_ofs_16 = fdsc->cmaps[i].glyph_id_ofs_list;
                glyph_id = fdsc->cmaps[i].glyph_id_start + gid_ofs_16[ofs];
            }
        }

        return glyph_id;
    }

    return 0;

}

#endif /*LV_USE_NEMA_GFX*/
