/**
 * @file lv_draw_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_label_private.h"
#include "lv_draw_private.h"
#include "../misc/lv_area_private.h"
#include "lv_draw_vector_private.h"
#include "lv_draw_rect_private.h"
#include "../core/lv_obj.h"
#include "../misc/lv_math.h"
#include "../core/lv_obj_event.h"
#include "../misc/lv_bidi_private.h"
#include "../misc/lv_text_private.h"
#include "../misc/lv_assert.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define LABEL_RECOLOR_PAR_LENGTH 6
#define LV_LABEL_HINT_UPDATE_TH 1024 /*Update the "hint" if the label's y coordinates have changed more then this*/

#define font_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->font_draw_buf_handlers)

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
static uint8_t hex_char_to_num(char hex);
static void draw_letter(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * dsc,  const lv_point_t * pos,
                        const lv_font_t * font, uint32_t letter, lv_draw_glyph_cb_t cb);

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
    dsc->text_length = LV_TEXT_LEN_MAX;
    dsc->font = LV_FONT_DEFAULT;
    dsc->sel_start = LV_DRAW_LABEL_NO_TXT_SEL;
    dsc->sel_end = LV_DRAW_LABEL_NO_TXT_SEL;
    dsc->sel_color = lv_color_black();
    dsc->sel_bg_color = lv_palette_main(LV_PALETTE_BLUE);
    dsc->bidi_dir = LV_BASE_DIR_LTR;
    dsc->base.dsc_size = sizeof(lv_draw_label_dsc_t);
}

lv_draw_label_dsc_t * lv_draw_task_get_label_dsc(lv_draw_task_t * task)
{
    return task->type == LV_DRAW_TASK_TYPE_LABEL ? (lv_draw_label_dsc_t *)task->draw_dsc : NULL;
}

void lv_draw_glyph_dsc_init(lv_draw_glyph_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_glyph_dsc_t));
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_label(lv_layer_t * layer, const lv_draw_label_dsc_t * dsc,
                                         const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->text == NULL || dsc->text[0] == '\0') return;
    if(dsc->font == NULL) {
        LV_LOG_WARN("dsc->font == NULL");
        return;
    }

    LV_PROFILER_DRAW_BEGIN;
    lv_draw_task_t * t = lv_draw_add_task(layer, coords);

    t->draw_dsc = lv_malloc(sizeof(*dsc));
    LV_ASSERT_MALLOC(t->draw_dsc);
    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));
    t->type = LV_DRAW_TASK_TYPE_LABEL;

    /*The text is stored in a local variable so malloc memory for it*/
    if(dsc->text_local) {
        lv_draw_label_dsc_t * new_dsc = t->draw_dsc;
        new_dsc->text = lv_strdup(dsc->text);
    }

    lv_draw_finalize_task_creation(layer, t);
    LV_PROFILER_DRAW_END;
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_character(lv_layer_t * layer, lv_draw_label_dsc_t * dsc,
                                             const lv_point_t * point, uint32_t unicode_letter)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->font == NULL) {
        LV_LOG_WARN("dsc->font == NULL");
        return;
    }

    if(lv_text_is_marker(unicode_letter)) return;

    LV_PROFILER_DRAW_BEGIN;

    lv_font_glyph_dsc_t g;
    lv_font_get_glyph_dsc(dsc->font, &g, unicode_letter, 0);

    lv_area_t a;
    a.x1 = point->x;
    a.y1 = point->y;
    a.x2 = a.x1 + g.adv_w;
    a.y2 = a.y1 + lv_font_get_line_height(g.resolved_font ? g.resolved_font : dsc->font);

    /*lv_draw_label needs UTF8 text so convert the Unicode character to an UTF8 string */
    uint32_t letter_buf[2];
    letter_buf[0] = lv_text_unicode_to_encoded(unicode_letter);
    letter_buf[1] = '\0';

    const char * letter_buf_char = (const char *)letter_buf;

#if LV_BIG_ENDIAN_SYSTEM
    while(*letter_buf_char == 0) ++letter_buf_char;
#endif

    dsc->text = letter_buf_char;
    dsc->text_local = 1;

    lv_draw_label(layer, dsc, &a);
    LV_PROFILER_DRAW_END;
}

void lv_draw_label_iterate_characters(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc,
                                      const lv_area_t * coords,
                                      lv_draw_glyph_cb_t cb)
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

    uint32_t remaining_len = dsc->text_length;

    uint32_t line_end = line_start + lv_text_get_next_line(&dsc->text[line_start], remaining_len, font, dsc->letter_space,
                                                           w, NULL, dsc->flag);

    /*Go the first visible line*/
    while(pos.y + line_height_font < draw_unit->clip_area->y1) {
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
                uint32_t t = lv_text_encoded_get_char_id(bidi_txt, next_char_offset);
                logical_char_pos += lv_bidi_get_logical_pos(bidi_txt, NULL, line_end - line_start, base_dir, t, NULL);
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
                    cb(draw_unit, NULL, &fill_dsc, &fill_area);
                }
                if(dsc->decor & LV_TEXT_DECOR_STRIKETHROUGH) {
                    lv_area_t fill_area;
                    fill_area.x1 = line_start_x;
                    fill_area.x2 = pos.x + letter_w - 1;
                    fill_area.y1 = pos.y + (font->line_height - font->base_line) * 2 / 3 + font->underline_thickness / 2;
                    fill_area.y2 = fill_area.y1 + underline_width - 1;

                    fill_dsc.color = dsc->color;
                    cb(draw_unit, NULL, &fill_dsc, &fill_area);
                }
            }

            /* Handle text selection */
            if(sel_start != LV_DRAW_LABEL_NO_TXT_SEL && sel_end != LV_DRAW_LABEL_NO_TXT_SEL
               && logical_char_pos >= sel_start && logical_char_pos < sel_end) {
                draw_letter_dsc.color = dsc->sel_color;
                fill_dsc.color = dsc->sel_bg_color;
                cb(draw_unit, NULL, &fill_dsc, &bg_coords);
            }
            else if(recolor_cmd_state == RECOLOR_CMD_STATE_TEXT_INPUT) {
                draw_letter_dsc.color = recolor;
            }
            else {
                draw_letter_dsc.color = dsc->color;
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

        if(pos.y > draw_unit->clip_area->y2) break;
    }

    if(draw_letter_dsc._draw_buf) lv_draw_buf_destroy(draw_letter_dsc._draw_buf);

    LV_ASSERT_MEM_INTEGRITY();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

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
static void draw_letter(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * dsc,  const lv_point_t * pos,
                        const lv_font_t * font, uint32_t letter, lv_draw_glyph_cb_t cb)
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

    /*If the letter is completely out of mask don't draw it*/
    if(lv_area_is_out(&letter_coords, draw_unit->clip_area, 0) &&
       lv_area_is_out(dsc->bg_coords, draw_unit->clip_area, 0)) {
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

                uint32_t h = g.box_h;
                if(h * g.box_w < 64) h *= 2; /*Alloc a slightly larger buffer*/
                draw_buf = lv_draw_buf_create_ex(font_draw_buf_handlers, g.box_w, h, LV_COLOR_FORMAT_A8, LV_STRIDE_AUTO);
                LV_ASSERT_MALLOC(draw_buf);
                draw_buf->header.h = g.box_h;
                dsc->_draw_buf = draw_buf;
            }
        }

        dsc->format = g.format;
    }
    else {
        dsc->format = LV_FONT_GLYPH_FORMAT_NONE;
    }

    dsc->letter_coords = &letter_coords;
    dsc->g = &g;
    cb(draw_unit, dsc, NULL, NULL);

    lv_font_glyph_release_draw_data(&g);

    LV_PROFILER_DRAW_END;
}
