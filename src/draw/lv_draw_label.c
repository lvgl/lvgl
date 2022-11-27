/**
 * @file lv_draw_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "lv_draw_label.h"
#include "../misc/lv_math.h"
#include "../core/lv_disp.h"
#include "../core/lv_refr.h"
#include "../misc/lv_bidi.h"
#include "../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define LABEL_RECOLOR_PAR_LENGTH 6
#define LV_LABEL_HINT_UPDATE_TH 1024 /*Update the "hint" if the label's y coordinates have changed more then this*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_letter(lv_draw_unit_t * draw_unit, lv_draw_letter_dsc_t * dsc,  const lv_point_t * pos,
                        const lv_font_t * font, uint32_t letter, lv_draw_letter_cb_t cb);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_label_dsc_init(lv_draw_label_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_label_dsc_t));
    dsc->opa = LV_OPA_COVER;
    dsc->color = lv_color_black();
    dsc->font = LV_FONT_DEFAULT;
    dsc->sel_start = LV_DRAW_LABEL_NO_TXT_SEL;
    dsc->sel_end = LV_DRAW_LABEL_NO_TXT_SEL;
    dsc->sel_color = lv_color_black();
    dsc->sel_bg_color = lv_palette_main(LV_PALETTE_BLUE);
    dsc->bidi_dir = LV_BASE_DIR_LTR;
}


void lv_draw_letter_dsc_init(lv_draw_letter_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_letter_dsc_t));
}

LV_ATTRIBUTE_FAST_MEM void lv_draw_label(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,
                                         const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->text == NULL || dsc->text[0] == '\0') return;
    if(dsc->font == NULL) {
        LV_LOG_WARN("dsc->font == NULL");
        return;
    }

    lv_draw_task_t * new_task = lv_malloc(sizeof(lv_draw_task_t));
    lv_memzero(new_task, sizeof(*new_task));

    new_task->draw_dsc = lv_malloc(sizeof(lv_draw_label_dsc_t));
    lv_memcpy(new_task->draw_dsc, dsc, sizeof(*dsc));
#if DRAW_LOG
    printf("Add  (%p, %p): %d, %d, %d, %d\n", new_task, new_task->draw_dsc, coords->x1, coords->y1,
           lv_area_get_width(coords), lv_area_get_height(coords));
#endif
    new_task->type = LV_DRAW_TASK_TYPE_LABEL;

    new_task->area = *coords;
    new_task->clip_area = *draw_ctx->clip_area;

    /*Find the tail*/
    if(draw_ctx->draw_task_head == NULL) {
        draw_ctx->draw_task_head = new_task;
    }
    else {
        lv_draw_task_t * tail = draw_ctx->draw_task_head;
        while(tail->next) tail = tail->next;

        tail->next = new_task;
    }

    lv_draw_dispatch(draw_ctx);
}

void lv_draw_label_interate_letters(lv_draw_unit_t * draw_unit, lv_draw_label_dsc_t * dsc, const lv_area_t * coords,
                                    lv_draw_letter_cb_t cb)
{
    const lv_font_t * font = dsc->font;
    int32_t w;

    lv_draw_ctx_t * draw_ctx = draw_unit->draw_ctx;
    lv_area_t clipped_area;
    bool clip_ok = _lv_area_intersect(&clipped_area, coords, draw_ctx->clip_area);
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
        lv_txt_get_size(&p, dsc->text, dsc->font, dsc->letter_space, dsc->line_space, LV_COORD_MAX,
                        dsc->flag);
        w = p.x;
    }

    int32_t line_height_font = lv_font_get_line_height(font);
    int32_t line_height = line_height_font + dsc->line_space;

    /*Init variables for the first line*/
    int32_t line_width = 0;
    lv_point_t pos;
    pos.x = coords->x1;
    pos.y = coords->y1;

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

    uint32_t line_end = line_start + _lv_txt_get_next_line(&dsc->text[line_start], font, dsc->letter_space, w, NULL,
                                                           dsc->flag);

    /*Go the first visible line*/
    while(pos.y + line_height_font < draw_ctx->clip_area->y1) {
        /*Go to next line*/
        line_start = line_end;
        line_end += _lv_txt_get_next_line(&dsc->text[line_start], font, dsc->letter_space, w, NULL, dsc->flag);
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
        line_width = lv_txt_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);

        pos.x += (lv_area_get_width(coords) - line_width) / 2;

    }
    /*Align to the right*/
    else if(align == LV_TEXT_ALIGN_RIGHT) {
        line_width = lv_txt_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);
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
    lv_draw_letter_dsc_t draw_letter_dsc;
    lv_draw_letter_dsc_init(&draw_letter_dsc);
    draw_letter_dsc.bg_color = dsc->sel_bg_color;
    draw_letter_dsc.bg_opa = dsc->opa;
    draw_letter_dsc.bg_coords = &bg_coords;
    draw_letter_dsc.letter_color = dsc->color;
    draw_letter_dsc.letter_opa = dsc->opa;
    draw_letter_dsc.decor = dsc->decor;

    if((dsc->decor & LV_TEXT_DECOR_UNDERLINE) || (dsc->decor & LV_TEXT_DECOR_STRIKETHROUGH)) {
        draw_letter_dsc.line_opa = dsc->opa;
        draw_letter_dsc.line_color = dsc->color;
        draw_letter_dsc.line_width = font->underline_thickness ? font->underline_thickness : 1;
    }

    uint32_t i;
    int32_t letter_w;

    /*Write out all lines*/
    while(dsc->text[line_start] != '\0') {
        pos.x += x_ofs;

        /*Write all letter of a line*/
        i = 0;
#if LV_USE_BIDI
        char * bidi_txt = lv_malloc(line_end - line_start + 1);
        _lv_bidi_process_paragraph(dsc->text + line_start, bidi_txt, line_end - line_start, base_dir, NULL, 0);
#else
        const char * bidi_txt = dsc->text + line_start;
#endif

        while(i < line_end - line_start) {
            uint32_t logical_char_pos = 0;
            if(sel_start != 0xFFFF && sel_end != 0xFFFF) {
#if LV_USE_BIDI
                logical_char_pos = _lv_txt_encoded_get_char_id(dsc->text, line_start);
                uint32_t t = _lv_txt_encoded_get_char_id(bidi_txt, i);
                logical_char_pos += _lv_bidi_get_logical_pos(bidi_txt, NULL, line_end - line_start, base_dir, t, NULL);
#else
                logical_char_pos = _lv_txt_encoded_get_char_id(dsc->text, line_start + i);
#endif
            }

            uint32_t letter;
            uint32_t letter_next;
            _lv_txt_encoded_letter_next_2(bidi_txt, &letter, &letter_next, &i);

            letter_w = lv_font_get_glyph_width(font, letter, letter_next);

            bg_coords.x1 = pos.x;
            bg_coords.y1 = pos.y;
            bg_coords.x2 = pos.x + letter_w + dsc->letter_space - 1;
            bg_coords.y2 = pos.y + line_height - 1;
            if(sel_start != 0xFFFF && sel_end != 0xFFFF && logical_char_pos >= sel_start && logical_char_pos < sel_end) {
                draw_letter_dsc.letter_color = dsc->sel_color;
                draw_letter_dsc.bg_opa = dsc->opa;
            }
            else {
                draw_letter_dsc.bg_opa = LV_OPA_TRANSP;
                draw_letter_dsc.letter_color = dsc->color;
            }

            draw_letter(draw_unit, &draw_letter_dsc, &pos, font, letter, cb);

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
        line_end += _lv_txt_get_next_line(&dsc->text[line_start], font, dsc->letter_space, w, NULL, dsc->flag);

        pos.x = coords->x1;
        /*Align to middle*/
        if(align == LV_TEXT_ALIGN_CENTER) {
            line_width =
                lv_txt_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);

            pos.x += (lv_area_get_width(coords) - line_width) / 2;
        }
        /*Align to the right*/
        else if(align == LV_TEXT_ALIGN_RIGHT) {
            line_width =
                lv_txt_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);
            pos.x += lv_area_get_width(coords) - line_width;
        }

        /*Go the next line position*/
        pos.y += line_height;

        if(pos.y > draw_ctx->clip_area->y2) return;
    }

    LV_ASSERT_MEM_INTEGRITY();
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_letter(lv_draw_unit_t * draw_unit, lv_draw_letter_dsc_t * dsc,  const lv_point_t * pos,
                        const lv_font_t * font, uint32_t letter, lv_draw_letter_cb_t cb)
{
    lv_draw_ctx_t * draw_ctx = draw_unit->draw_ctx;
    lv_font_glyph_dsc_t g;

    bool g_ret = lv_font_get_glyph_dsc(font, &g, letter, '\0');
    if(g_ret == false) {
        /*Add warning if the dsc is not found
         *but do not print warning for non printable ASCII chars (e.g. '\n')*/
        if(letter >= 0x20 &&
           letter != 0xf8ff && /*LV_SYMBOL_DUMMY*/
           letter != 0x200c) { /*ZERO WIDTH NON-JOINER*/
            LV_LOG_INFO("lv_draw_letter: glyph dsc. not found for U+%" LV_PRIX32, letter);
        }
        return;
    }

    /*Don't draw anything if the character is empty. E.g. space*/
    if((g.box_h == 0) || (g.box_w == 0)) return;

    lv_area_t letter_coords;
    letter_coords.x1 = pos->x + g.ofs_x;
    letter_coords.x2 = letter_coords.x1 + g.box_w - 1;
    letter_coords.y1 = pos->y + (font->line_height - font->base_line) - g.box_h - g.ofs_y;
    letter_coords.y2 = letter_coords.y1 + g.box_h - 1;

    /*If the letter is completely out of mask don't draw it*/
    if(_lv_area_is_out(&letter_coords, draw_ctx->clip_area, 0)) return;

    dsc->bitmap = lv_font_get_glyph_bitmap(g.resolved_font, letter);
    dsc->letter_coords = &letter_coords;
    cb(draw_unit, dsc);
}

