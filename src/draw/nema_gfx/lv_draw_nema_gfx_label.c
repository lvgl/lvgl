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
 * @file lv_draw_nema_gfx_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_nema_gfx.h"

#if LV_USE_NEMA_GFX
#include "../../misc/lv_utils.h"
#include "../../misc/lv_bidi_private.h"
#include "../../misc/lv_text_private.h"
#include "../../lvgl.h"
#include "../../libs/freetype/lv_freetype_private.h"
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define LABEL_RECOLOR_PAR_LENGTH 6
#define LV_LABEL_HINT_UPDATE_TH 1024 /*Update the "hint" if the label's y coordinates have changed more then this*/
#define FT_F26DOT6_SHIFT 6
#define NEMA_COORD_LIMIT 2046

#define font_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->font_draw_buf_handlers)

/** After converting the font reference size, it is also necessary to scale the 26dot6 data
 * in the path to the real physical size
 */
#define FT_F26DOT6_TO_PATH_SCALE(x) (LV_FREETYPE_F26DOT6_TO_FLOAT(x) / (1 << FT_F26DOT6_SHIFT))

/*Forward declarations*/
void nema_set_matrix(nema_matrix3x3_t m);
void nema_raster_rect(int x, int y, int w, int h);

/**********************
 *      TYPEDEFS
 **********************/
enum {
    RECOLOR_CMD_STATE_WAIT_FOR_PARAMETER,
    RECOLOR_CMD_STATE_PARAMETER,
    RECOLOR_CMD_STATE_TEXT_INPUT,
};
typedef unsigned char cmd_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _draw_nema_gfx_letter(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                  lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

static void _draw_label_iterate_characters(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc,
                                           const lv_area_t * coords);

static inline uint8_t _bpp_nema_gfx_format(lv_draw_glyph_dsc_t * glyph_draw_dsc);

static void _draw_letter(lv_draw_task_t * t, lv_draw_glyph_dsc_t * dsc,  const lv_point_t * pos,
                         const lv_font_t * font, uint32_t letter);

static uint8_t hex_char_to_num(char hex);

static bool is_raw_bitmap;

#if LV_USE_FREETYPE && LV_USE_NEMA_VG

    #include "lv_nema_gfx_path.h"

    static void _draw_nema_gfx_outline(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc);

    static void freetype_outline_event_cb(lv_event_t * e);

    static void lv_nema_gfx_outline_push(const lv_freetype_outline_event_param_t * param);

    static void lv_nema_outline_event_alloc(const lv_freetype_outline_event_param_t * param);
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_nema_gfx_label_init(lv_draw_unit_t * draw_unit)
{
#if LV_USE_FREETYPE
    /*Set up the freetype outline event*/
    lv_freetype_outline_add_event(freetype_outline_event_cb, LV_EVENT_ALL, draw_unit);
#else
    LV_UNUSED(draw_unit);
#endif /* LV_USE_FREETYPE */
}

void lv_draw_nema_gfx_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_layer_t * layer = t->target_layer;

    lv_area_t clip_area;
    lv_area_copy(&clip_area, &t->clip_area);
    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_color_format_t dst_cf = layer->draw_buf->header.cf;
    uint32_t dst_nema_cf = lv_nemagfx_cf_to_nema(dst_cf);

    int32_t stride = (dst_cf >= LV_COLOR_FORMAT_NEMA_TSC_START && dst_cf <= LV_COLOR_FORMAT_NEMA_TSC_END) ?
                     -1 : lv_area_get_width(&(layer->buf_area)) * lv_color_format_get_size(dst_cf);

    nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(layer->draw_buf->data), lv_area_get_width(&(layer->buf_area)),
                      lv_area_get_height(&(layer->buf_area)), dst_nema_cf, stride);

    nema_set_clip(clip_area.x1, clip_area.y1, lv_area_get_width(&clip_area), lv_area_get_height(&clip_area));

    _draw_label_iterate_characters(t, dsc, coords);

    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)t->draw_unit;
    nema_cl_submit(&(draw_nema_gfx_unit->cl));
    nema_cl_wait(&(draw_nema_gfx_unit->cl));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#if LV_USE_FREETYPE && LV_USE_NEMA_VG

static void _draw_nema_gfx_outline(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc)
{

    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, glyph_draw_dsc->letter_coords, &t->clip_area))
        return;

    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)t->draw_unit;

    lv_nema_gfx_path_t * nema_gfx_path = (lv_nema_gfx_path_t *)glyph_draw_dsc->glyph_data;

    lv_point_t pos = {glyph_draw_dsc->letter_coords->x1, glyph_draw_dsc->letter_coords->y1};

    float scale = FT_F26DOT6_TO_PATH_SCALE(lv_freetype_outline_get_scale(glyph_draw_dsc->g->resolved_font));

    /*Calculate Path Matrix*/
    nema_matrix3x3_t matrix;
    nema_mat3x3_load_identity(matrix);
    nema_mat3x3_scale(matrix, scale, -scale);
    nema_mat3x3_translate(matrix, pos.x - glyph_draw_dsc->g->ofs_x,
                          pos.y + glyph_draw_dsc->g->box_h + glyph_draw_dsc->g->ofs_y);

    nema_vg_path_clear(nema_gfx_path->path);
    nema_vg_paint_clear(nema_gfx_path->paint);

    nema_vg_set_fill_rule(NEMA_VG_FILL_EVEN_ODD);

    nema_vg_path_set_shape(nema_gfx_path->path, nema_gfx_path->seg_size, nema_gfx_path->seg, nema_gfx_path->data_size,
                           nema_gfx_path->data);

    nema_vg_paint_set_type(nema_gfx_path->paint, NEMA_VG_PAINT_COLOR);

    lv_color32_t dsc_col32 = lv_color_to_32(glyph_draw_dsc->color, glyph_draw_dsc->opa);
    uint32_t nema_dsc_color = nema_rgba(dsc_col32.red, dsc_col32.green, dsc_col32.blue, dsc_col32.alpha);

    nema_vg_paint_set_paint_color(nema_gfx_path->paint, nema_dsc_color);

    nema_vg_path_set_matrix(nema_gfx_path->path, matrix);
    nema_vg_draw_path(nema_gfx_path->path, nema_gfx_path->paint);

    return;
}

static void freetype_outline_event_cb(lv_event_t * e)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_event_code_t code = lv_event_get_code(e);
    lv_freetype_outline_event_param_t * param = lv_event_get_param(e);

    switch(code) {
        case LV_EVENT_CREATE:
            param->outline = lv_nema_gfx_path_create();
            lv_nema_outline_event_alloc(param);
            break;
        case LV_EVENT_DELETE:
            lv_nema_gfx_path_destroy(param->outline);
            break;
        case LV_EVENT_INSERT:
            lv_nema_gfx_outline_push(param);
            break;
        default:
            LV_LOG_WARN("unknown event code: %d", code);
            break;
    }
    LV_PROFILER_DRAW_END;
}

static void lv_nema_gfx_outline_push(const lv_freetype_outline_event_param_t * param)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_nema_gfx_path_t * outline = param->outline;
    LV_ASSERT_NULL(outline);

    lv_freetype_outline_type_t type = param->type;
    switch(type) {
        case LV_FREETYPE_OUTLINE_END:
            lv_nema_gfx_path_end(outline);
            break;
        case LV_FREETYPE_OUTLINE_MOVE_TO:
            lv_nema_gfx_path_move_to(outline, param->to.x, param->to.y);
            break;
        case LV_FREETYPE_OUTLINE_LINE_TO:
            lv_nema_gfx_path_line_to(outline, param->to.x, param->to.y);
            break;
        case LV_FREETYPE_OUTLINE_CUBIC_TO:
            lv_nema_gfx_path_cubic_to(outline, param->control1.x, param->control1.y,
                                      param->control2.x, param->control2.y,
                                      param->to.x, param->to.y);
            break;
        case LV_FREETYPE_OUTLINE_CONIC_TO:
            lv_nema_gfx_path_quad_to(outline, param->control1.x, param->control1.y,
                                     param->to.x, param->to.y);
            break;
        default:
            LV_LOG_ERROR("unknown point type: %d", type);
            LV_ASSERT(false);
            break;
    }
    LV_PROFILER_DRAW_END;
}

static void lv_nema_outline_event_alloc(const lv_freetype_outline_event_param_t * param)
{
    lv_nema_gfx_path_t * outline = param->outline;
    outline->data_size = param->sizes.data_size;
    outline->seg_size = param->sizes.segments_size;
    lv_nema_gfx_path_alloc(outline);
}

#endif /* LV_USE_FREETYPE && LV_USE_NEMA_VG */

/**
 * Convert a hexadecimal characters to a number (0..15)
 * @param hex Pointer to a hexadecimal character (0..9, A..F)
 * @return the numerical value of `hex` or 0 on error
 */
static uint8_t hex_char_to_num(char hex)
{
    if(hex >= '0' && hex <= '9') return hex - '0';
    if(hex >= 'a') hex -= 'a' - 'A'; /*Convert to upper case*/
    return 'A' <= hex && hex <= 'F' ? hex - 'A' + 10 : 0;
}


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
        case LV_FONT_GLYPH_FORMAT_A8:
        default:
            return NEMA_A8;
    }
}

static void _draw_nema_gfx_letter(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
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
            lv_draw_nema_gfx_border(t, &border_draw_dsc, glyph_draw_dsc->bg_coords);
#endif
        }
        else if(glyph_draw_dsc->format >= LV_FONT_GLYPH_FORMAT_A1 &&
                glyph_draw_dsc->format <= LV_FONT_GLYPH_FORMAT_A8) {
            /*Do not draw transparent things*/
            if(glyph_draw_dsc->opa <= LV_OPA_MIN)
                return;

            lv_layer_t * layer = t->target_layer;

            lv_area_t blend_area;
            if(!lv_area_intersect(&blend_area, glyph_draw_dsc->letter_coords, &t->clip_area))
                return;

            const lv_draw_buf_t * draw_buf = glyph_draw_dsc->glyph_data;
            const void * mask_buf;
            uint32_t src_cf;
            lv_area_t mask_area = *glyph_draw_dsc->letter_coords;

            lv_area_t rel_coords;
            lv_area_copy(&rel_coords, &blend_area);
            lv_area_move(&rel_coords, -layer->buf_area.x1, -layer->buf_area.y1);

            int32_t x, y, w, h;

            /*Read the static font*/
            if(is_raw_bitmap) {
                mask_buf = glyph_draw_dsc->glyph_data;
                src_cf = _bpp_nema_gfx_format(glyph_draw_dsc);
                x = glyph_draw_dsc->letter_coords->x1 - layer->buf_area.x1;
                y = glyph_draw_dsc->letter_coords->y1 - layer->buf_area.y1;
                w = glyph_draw_dsc->g->box_w;
                h = glyph_draw_dsc->g->box_h;
            }
            /*Read the draw buffer*/
            else {
                mask_buf = draw_buf->data;
                src_cf = lv_nemagfx_cf_to_nema(draw_buf->header.cf);
                mask_area.x2 = mask_area.x1 + lv_draw_buf_width_to_stride(lv_area_get_width(&mask_area), LV_COLOR_FORMAT_A8) - 1;
                mask_buf += draw_buf->header.stride * (blend_area.y1 - mask_area.y1) + (blend_area.x1 - mask_area.x1);
                x = rel_coords.x1;
                y = rel_coords.y1;
                w = lv_area_get_width(&rel_coords);
                h = lv_area_get_height(&rel_coords);
            }

            if(is_raw_bitmap && (glyph_draw_dsc->format <= LV_FONT_GLYPH_FORMAT_A4)) {
                nema_bind_src_tex((uintptr_t)(mask_buf), w * h, 1, src_cf, glyph_draw_dsc->g->stride, NEMA_FILTER_PS);
                nema_matrix3x3_t m = {
                    {1,    w,   -x - (y * w) - (0.5 * w)},
                    {0,    1,                   0},
                    {0,    0,                   1}
                };
                nema_set_matrix(m);
                nema_raster_rect(x, y, w, h);
            }
            else {
                nema_bind_src_tex((uintptr_t)(mask_buf), lv_area_get_width(&mask_area), lv_area_get_height(&mask_area), src_cf,
                                  glyph_draw_dsc->g->stride ? glyph_draw_dsc->g->stride : lv_area_get_width(&mask_area),
                                  NEMA_FILTER_BL);
                nema_blit_rect(x, y, w, h);
                lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)t->draw_unit;
                nema_cl_submit(&(draw_nema_gfx_unit->cl));
                nema_cl_wait(&(draw_nema_gfx_unit->cl));
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
            lv_draw_nema_gfx_img(t, &img_dsc, glyph_draw_dsc->letter_coords);
#endif
        }

#if LV_USE_FREETYPE && LV_USE_NEMA_VG
        else if(glyph_draw_dsc->format == LV_FONT_GLYPH_FORMAT_VECTOR) {
            if(lv_freetype_is_outline_font(glyph_draw_dsc->g->resolved_font)) {
                _draw_nema_gfx_outline(t, glyph_draw_dsc);
            }
        }
#endif

    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_nema_gfx_fill(t, fill_draw_dsc, fill_area);
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

static void _draw_label_iterate_characters(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc,
                                           const lv_area_t * coords)
{
    const lv_font_t * font = dsc->font;
    int32_t w;

    lv_area_t clipped_area;
    bool clip_ok = lv_area_intersect(&clipped_area, coords, &t->clip_area);
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

    uint32_t remaining_len = dsc->text_length;

    uint32_t line_end = line_start + lv_text_get_next_line(&dsc->text[line_start], remaining_len, font, dsc->letter_space,
                                                           w, NULL, dsc->flag);

    /*Go the first visible line*/
    while(pos.y + line_height_font < t->clip_area.y1) {
        /*Go to next line*/
        line_start = line_end;
        line_end += lv_text_get_next_line(&dsc->text[line_start], remaining_len, font, dsc->letter_space, w, NULL, dsc->flag);
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
        line_width = lv_text_get_width_with_flags(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space,
                                                  dsc->flag);

        pos.x += (lv_area_get_width(coords) - line_width) / 2;

    }
    /*Align to the right*/
    else if(align == LV_TEXT_ALIGN_RIGHT) {
        line_width = lv_text_get_width_with_flags(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space,
                                                  dsc->flag);
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
    draw_letter_dsc.rotation = dsc->rotation;

    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.opa = dsc->opa;
    int32_t underline_width = font->underline_thickness ? font->underline_thickness : 1;
    int32_t line_start_x;
    uint32_t next_char_offset;
    uint32_t recolor_command_start_index = 0;
    int32_t letter_w;
    cmd_state_t recolor_cmd_state = RECOLOR_CMD_STATE_WAIT_FOR_PARAMETER;
    lv_color_t recolor = lv_color_black(); /* Holds the selected color inside the recolor command */
    uint8_t is_first_space_after_cmd = 0;

    lv_color32_t dsc_col32 = lv_color_to_32(dsc->color, dsc->opa);
    uint32_t nema_dsc_color = nema_rgba(dsc_col32.red, dsc_col32.green, dsc_col32.blue, dsc_col32.alpha);
    lv_color32_t dsc_sel_col32 = lv_color_to_32(dsc->sel_color, dsc->opa);
    uint32_t nema_dsc_sel_color = nema_rgba(dsc_sel_col32.red, dsc_sel_col32.green, dsc_sel_col32.blue,
                                            dsc_sel_col32.alpha);
    uint32_t blend_color;
    uint8_t blend_alpha = 255;

    _set_color_blend(nema_dsc_color, dsc_col32.alpha);

    uint8_t cur_state = 2;
    uint8_t prev_state = 2;

    /*Write out all lines*/
    while(remaining_len && dsc->text[line_start] != '\0') {
        pos.x += x_ofs;
        line_start_x = pos.x;

        /*Write all letter of a line*/
        recolor_cmd_state = RECOLOR_CMD_STATE_WAIT_FOR_PARAMETER;
        next_char_offset = 0;
#if LV_USE_BIDI
        char * bidi_txt = lv_malloc(line_end - line_start + 1);
        LV_ASSERT_MALLOC(bidi_txt);
        lv_bidi_process_paragraph(dsc->text + line_start, bidi_txt, line_end - line_start, base_dir, NULL, 0);
#else
        const char * bidi_txt = dsc->text + line_start;
#endif

        while(next_char_offset < remaining_len && next_char_offset < line_end - line_start) {
            uint32_t logical_char_pos = 0;

            /* Check if the text selection is enabled */
            if(sel_start != LV_DRAW_LABEL_NO_TXT_SEL && sel_end != LV_DRAW_LABEL_NO_TXT_SEL) {
#if LV_USE_BIDI
                logical_char_pos = lv_text_encoded_get_char_id(dsc->text, line_start);
                uint32_t c_idx = lv_text_encoded_get_char_id(bidi_txt, next_char_offset);
                logical_char_pos += lv_bidi_get_logical_pos(bidi_txt, NULL, line_end - line_start, base_dir, c_idx, NULL);
#else
                logical_char_pos = lv_text_encoded_get_char_id(dsc->text, line_start + next_char_offset);
#endif
            }

            uint32_t letter;
            uint32_t letter_next;
            lv_text_encoded_letter_next_2(bidi_txt, &letter, &letter_next, &next_char_offset);

            /* If recolor is enabled */
            if((dsc->flag & LV_TEXT_FLAG_RECOLOR) != 0) {

                if(letter == (uint32_t)LV_TXT_COLOR_CMD[0]) {
                    /* Handle the recolor command marker depending of the current recolor state */

                    if(recolor_cmd_state == RECOLOR_CMD_STATE_WAIT_FOR_PARAMETER) {
                        recolor_command_start_index = next_char_offset;
                        recolor_cmd_state = RECOLOR_CMD_STATE_PARAMETER;
                        continue;
                    }
                    /*Other start char in parameter escaped cmd. char*/
                    else if(recolor_cmd_state == RECOLOR_CMD_STATE_PARAMETER) {
                        recolor_cmd_state = RECOLOR_CMD_STATE_WAIT_FOR_PARAMETER;
                    }
                    /* If letter is LV_TXT_COLOR_CMD and we were in the CMD_STATE_IN then the recolor close marked has been found */
                    else if(recolor_cmd_state == RECOLOR_CMD_STATE_TEXT_INPUT) {
                        recolor_cmd_state = RECOLOR_CMD_STATE_WAIT_FOR_PARAMETER;
                        continue;
                    }
                }

                /* Find the first space (aka ' ') after the recolor command parameter, we need to skip rendering it */
                if((recolor_cmd_state == RECOLOR_CMD_STATE_PARAMETER) && (letter == ' ') && (is_first_space_after_cmd == 0)) {
                    is_first_space_after_cmd = 1;
                }
                else {
                    is_first_space_after_cmd = 0;
                }

                /* Skip the color parameter and wait the space after it
                 * Once we have reach the space ' ', then we will extract the color information
                 * and store it into the recolor variable */
                if(recolor_cmd_state == RECOLOR_CMD_STATE_PARAMETER) {
                    /* Not an space? Continue with the next character */
                    if(letter != ' ') {
                        continue;
                    }

                    /*Get the recolor parameter*/
                    if((next_char_offset - recolor_command_start_index) == LABEL_RECOLOR_PAR_LENGTH + 1) {
                        /* Temporary buffer to hold the recolor information */
                        char buf[LABEL_RECOLOR_PAR_LENGTH + 1];
                        lv_memcpy(buf, &bidi_txt[recolor_command_start_index], LABEL_RECOLOR_PAR_LENGTH);
                        buf[LABEL_RECOLOR_PAR_LENGTH] = '\0';

                        uint8_t r, g, b;
                        r = (hex_char_to_num(buf[0]) << 4) + hex_char_to_num(buf[1]);
                        g = (hex_char_to_num(buf[2]) << 4) + hex_char_to_num(buf[3]);
                        b = (hex_char_to_num(buf[4]) << 4) + hex_char_to_num(buf[5]);

                        recolor = lv_color_make(r, g, b);
                    }
                    else {
                        recolor.red = dsc->color.red;
                        recolor.blue = dsc->color.blue;
                        recolor.green = dsc->color.green;
                    }

                    /*After the parameter the text is in the command*/
                    recolor_cmd_state = RECOLOR_CMD_STATE_TEXT_INPUT;
                }

                /* Don't draw the first space after the recolor command */
                if(is_first_space_after_cmd) {
                    continue;
                }
            }

            /* If we're in the CMD_STATE_IN state then we need to subtract the recolor command length */
            if(((dsc->flag & LV_TEXT_FLAG_RECOLOR) != 0) && (recolor_cmd_state == RECOLOR_CMD_STATE_TEXT_INPUT)) {
                logical_char_pos -= (LABEL_RECOLOR_PAR_LENGTH + 1);
            }

            letter_w = lv_font_get_glyph_width(font, letter, letter_next);

            /*Always set the bg_coordinates for placeholder drawing*/
            bg_coords.x1 = pos.x;
            bg_coords.y1 = pos.y;
            bg_coords.x2 = pos.x + letter_w - 1;
            bg_coords.y2 = pos.y + line_height - 1;

            if(next_char_offset >= line_end - line_start) {
                if(dsc->decor & LV_TEXT_DECOR_UNDERLINE) {
                    lv_area_t fill_area;
                    fill_area.x1 = line_start_x;
                    fill_area.x2 = pos.x + letter_w - 1;
                    fill_area.y1 = pos.y + font->line_height - font->base_line - font->underline_position;
                    fill_area.y2 = fill_area.y1 + underline_width - 1;

                    fill_dsc.color = dsc->color;
                    lv_draw_nema_gfx_fill(t, &fill_dsc, &fill_area);
                }
                if(dsc->decor & LV_TEXT_DECOR_STRIKETHROUGH) {
                    lv_area_t fill_area;
                    fill_area.x1 = line_start_x;
                    fill_area.x2 = pos.x + letter_w - 1;
                    fill_area.y1 = pos.y + (font->line_height - font->base_line) * 2 / 3 + font->underline_thickness / 2;
                    fill_area.y2 = fill_area.y1 + underline_width - 1;

                    fill_dsc.color = dsc->color;
                    lv_draw_nema_gfx_fill(t, &fill_dsc, &fill_area);
                }
            }

            /* Handle text selection */
            if(sel_start != LV_DRAW_LABEL_NO_TXT_SEL && sel_end != LV_DRAW_LABEL_NO_TXT_SEL
               && logical_char_pos >= sel_start && logical_char_pos < sel_end) {
                draw_letter_dsc.color = dsc->sel_color;
                fill_dsc.color = dsc->sel_bg_color;
                lv_draw_nema_gfx_fill(t, &fill_dsc, &bg_coords);
                cur_state = 0 ;
                blend_alpha = dsc_sel_col32.alpha;
                blend_color = nema_dsc_sel_color;
            }
            else if(recolor_cmd_state == RECOLOR_CMD_STATE_TEXT_INPUT) {
                draw_letter_dsc.color = recolor;
                cur_state = 1 ;
                blend_alpha = dsc_col32.alpha;
                lv_color32_t dsc_recolor_col32 = lv_color_to_32(recolor, dsc->opa);
                blend_color = nema_rgba(dsc_recolor_col32.red, dsc_recolor_col32.green, dsc_recolor_col32.blue,
                                        dsc_recolor_col32.alpha);
            }
            else {
                draw_letter_dsc.color = dsc->color;
                cur_state = 2;
                blend_alpha = dsc_col32.alpha;
                blend_color = nema_dsc_color;
            }

            if(cur_state != prev_state) {
                _set_color_blend(blend_color, blend_alpha);
                prev_state = cur_state;
            }

            _draw_letter(t, &draw_letter_dsc, &pos, font, letter);

            if(letter_w > 0) {
                pos.x += letter_w + dsc->letter_space;
            }
        }

#if LV_USE_BIDI
        lv_free(bidi_txt);
        bidi_txt = NULL;
#endif
        /*Go to next line*/
        remaining_len -= line_end - line_start;
        line_start = line_end;
        if(remaining_len) {
            line_end += lv_text_get_next_line(&dsc->text[line_start], remaining_len, font, dsc->letter_space, w, NULL, dsc->flag);
        }

        pos.x = coords->x1;
        /*Align to middle*/
        if(align == LV_TEXT_ALIGN_CENTER) {
            line_width =
                lv_text_get_width_with_flags(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);

            pos.x += (lv_area_get_width(coords) - line_width) / 2;
        }
        /*Align to the right*/
        else if(align == LV_TEXT_ALIGN_RIGHT) {
            line_width =
                lv_text_get_width_with_flags(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);
            pos.x += lv_area_get_width(coords) - line_width;
        }

        /*Go the next line position*/
        pos.y += line_height;

        if(pos.y > t->clip_area.y2) break;
    }

    if(draw_letter_dsc._draw_buf) lv_draw_buf_destroy(draw_letter_dsc._draw_buf);

    LV_ASSERT_MEM_INTEGRITY();
}

static void _draw_letter(lv_draw_task_t * t, lv_draw_glyph_dsc_t * dsc,  const lv_point_t * pos,
                         const lv_font_t * font, uint32_t letter)
{
    lv_font_glyph_dsc_t g;

    if(lv_text_is_marker(letter)) /*Markers are valid letters but should not be rendered.*/
        return;

    LV_PROFILER_DRAW_BEGIN;
    bool g_ret = lv_font_get_glyph_dsc(font, &g, letter, '\0');
    if(g_ret == false) {
        /*Add warning if the dsc is not found*/
        LV_LOG_WARN("lv_draw_letter: glyph dsc. not found for U+%" LV_PRIX32, letter);
    }

    /*Don't draw anything if the character is empty. E.g. space*/
    if((g.box_h == 0) || (g.box_w == 0)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_area_t letter_coords;
    letter_coords.x1 = pos->x + g.ofs_x;
    letter_coords.x2 = letter_coords.x1 + g.box_w - 1;
    letter_coords.y1 = pos->y + (font->line_height - font->base_line) - g.box_h - g.ofs_y;
    letter_coords.y2 = letter_coords.y1 + g.box_h - 1;
    lv_area_move(&letter_coords, -dsc->pivot.x, -dsc->pivot.y);

    /*If the letter is completely out of mask don't draw it*/
    if(lv_area_is_out(&letter_coords, &t->clip_area, 0) &&
       dsc->bg_coords &&
       lv_area_is_out(dsc->bg_coords, &t->clip_area, 0)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    if(g.resolved_font) {
        lv_draw_buf_t * draw_buf = NULL;
        if(LV_FONT_GLYPH_FORMAT_NONE < g.format && g.format < LV_FONT_GLYPH_FORMAT_IMAGE) {
            /*Only check draw buf for bitmap glyph*/
            draw_buf = lv_draw_buf_reshape(dsc->_draw_buf, 0, g.box_w, g.box_h, LV_STRIDE_AUTO);
            if(draw_buf == NULL) {
                if(dsc->_draw_buf) lv_draw_buf_destroy(dsc->_draw_buf);

                uint32_t h = LV_ROUND_UP(g.box_h, 32); /*Assume a larger size to avoid many reallocations*/
                draw_buf = lv_draw_buf_create_ex(font_draw_buf_handlers, g.box_w, h, LV_COLOR_FORMAT_A8, LV_STRIDE_AUTO);
                LV_ASSERT_MALLOC(draw_buf);
                draw_buf->header.h = g.box_h;
                dsc->_draw_buf = draw_buf;
            }
        }

        /* Performance Optimization for lv_font_fmt_txt_dsc_t fonts, always request raw bitmaps */
        /*Exception for w*h >= NEMA_COORD_LIMIT due to HW limitation on data handling*/
        is_raw_bitmap = false;
        if(g.box_h * g.box_w <= NEMA_COORD_LIMIT) {
            g.req_raw_bitmap = 1;
            if(font->get_glyph_bitmap == lv_font_get_bitmap_fmt_txt) {
                lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
                if(fdsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN) {
                    is_raw_bitmap = true;
                }
            }
        }

        dsc->glyph_data = g.resolved_font->get_glyph_bitmap(&g, draw_buf);

        dsc->format = dsc->glyph_data ? g.format : LV_FONT_GLYPH_FORMAT_NONE;
    }
    else {
        dsc->format = LV_FONT_GLYPH_FORMAT_NONE;
    }

    dsc->letter_coords = &letter_coords;
    dsc->g = &g;
    _draw_nema_gfx_letter(t, dsc, NULL, NULL);

    if(g.resolved_font && font->release_glyph) {
        lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)t->draw_unit;
        nema_cl_submit(&(draw_nema_gfx_unit->cl));
        nema_cl_wait(&(draw_nema_gfx_unit->cl));
        font->release_glyph(font, &g);
    }

    LV_PROFILER_DRAW_END;
}

#endif /*LV_USE_NEMA_GFX*/
