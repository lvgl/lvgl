/**
 * @file lv_draw_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_label.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LABEL_RECOLOR_PAR_LENGTH 6
#define LV_LABEL_HINT_UPDATE_TH 1024 /*Update the "hint" if the label's y coordinates have changed more then this*/

/**********************
 *      TYPEDEFS
 **********************/
enum {
    CMD_STATE_WAIT,
    CMD_STATE_PAR,
    CMD_STATE_IN,
};
typedef uint8_t cmd_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static uint8_t hex_char_to_num(char hex);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Write a text
 * @param coords coordinates of the label
 * @param mask the label will be drawn only in this area
 * @param style pointer to a style
 * @param opa_scale scale down all opacities by the factor
 * @param txt 0 terminated text to write
 * @param flag settings for the text from 'txt_flag_t' enum
 * @param offset text offset in x and y direction (NULL if unused)
 * @param sel_start start index of selected area (`LV_LABEL_TXT_SEL_OFF` if none)
 * @param sel_end end index of selected area (`LV_LABEL_TXT_SEL_OFF` if none)
 */
void lv_draw_label(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale,
                   const char * txt, lv_txt_flag_t flag, lv_point_t * offset, uint16_t sel_start, uint16_t sel_end,
                   lv_draw_label_hint_t * hint)
{
    const lv_font_t * font = style->text.font;
    lv_coord_t w;
    if((flag & LV_TXT_FLAG_EXPAND) == 0) {
        /*Normally use the label's width as width*/
        w = lv_area_get_width(coords);
    } else {
        /*If EXAPND is enabled then not limit the text's width to the object's width*/
        lv_point_t p;
        lv_txt_get_size(&p, txt, style->text.font, style->text.letter_space, style->text.line_space, LV_COORD_MAX,
                        flag);
        w = p.x;
    }

    lv_coord_t line_height = lv_font_get_line_height(font) + style->text.line_space;

    /*Init variables for the first line*/
    lv_coord_t line_width = 0;
    lv_point_t pos;
    pos.x = coords->x1;
    pos.y = coords->y1;

    lv_coord_t x_ofs = 0;
    lv_coord_t y_ofs = 0;
    if(offset != NULL) {
        x_ofs = offset->x;
        y_ofs = offset->y;
        pos.y += y_ofs;
    }

    uint32_t line_start     = 0;
    int32_t last_line_start = -1;

    /*Check the hint to use the cached info*/
    if(hint && y_ofs == 0) {
        /*If the label changed too much recalculate the hint.*/
        if(LV_MATH_ABS(hint->coord_y - coords->y1) > LV_LABEL_HINT_UPDATE_TH - 2 * line_height) {
            hint->line_start = -1;
        }
        last_line_start = hint->line_start;
    }

    /*Use the hint if it's valid*/
    if(hint && last_line_start >= 0) {
        line_start = last_line_start;
        pos.y += hint->y;
    }

    uint32_t line_end = line_start + lv_txt_get_next_line(&txt[line_start], font, style->text.letter_space, w, flag);

    /*Go the first visible line*/
    while(pos.y + line_height < mask->y1) {
        /*Go to next line*/
        line_start = line_end;
        line_end += lv_txt_get_next_line(&txt[line_start], font, style->text.letter_space, w, flag);
        pos.y += line_height;

        /*Save at the threshold coordinate*/
        if(hint && pos.y >= -LV_LABEL_HINT_UPDATE_TH && hint->line_start < 0) {
            hint->line_start = line_start;
            hint->y          = pos.y - coords->y1;
            hint->coord_y    = coords->y1;
        }

        if(txt[line_start] == '\0') return;
    }

    /*Align to middle*/
    if(flag & LV_TXT_FLAG_CENTER) {
        line_width = lv_txt_get_width(&txt[line_start], line_end - line_start, font, style->text.letter_space, flag);

        pos.x += (lv_area_get_width(coords) - line_width) / 2;

    }
    /*Align to the right*/
    else if(flag & LV_TXT_FLAG_RIGHT) {
        line_width = lv_txt_get_width(&txt[line_start], line_end - line_start, font, style->text.letter_space, flag);
        pos.x += lv_area_get_width(coords) - line_width;
    }

    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->text.opa : (uint16_t)((uint16_t)style->text.opa * opa_scale) >> 8;

    cmd_state_t cmd_state = CMD_STATE_WAIT;
    uint32_t i;
    uint16_t par_start = 0;
    lv_color_t recolor;
    lv_coord_t letter_w;
    lv_style_t sel_style;
    lv_style_copy(&sel_style, &lv_style_plain_color);
    sel_style.body.main_color = sel_style.body.grad_color = style->text.sel_color;

    /*Write out all lines*/
    while(txt[line_start] != '\0') {
        if(offset != NULL) {
            pos.x += x_ofs;
        }
        /*Write all letter of a line*/
        cmd_state = CMD_STATE_WAIT;
        i         = line_start;
        uint32_t letter;
        uint32_t letter_next;
        while(i < line_end) {
            letter      = lv_txt_encoded_next(txt, &i);
            letter_next = lv_txt_encoded_next(&txt[i], NULL);

            /*Handle the re-color command*/
            if((flag & LV_TXT_FLAG_RECOLOR) != 0) {
                if(letter == (uint32_t)LV_TXT_COLOR_CMD[0]) {
                    if(cmd_state == CMD_STATE_WAIT) { /*Start char*/
                        par_start = i;
                        cmd_state = CMD_STATE_PAR;
                        continue;
                    } else if(cmd_state == CMD_STATE_PAR) { /*Other start char in parameter escaped cmd. char */
                        cmd_state = CMD_STATE_WAIT;
                    } else if(cmd_state == CMD_STATE_IN) { /*Command end */
                        cmd_state = CMD_STATE_WAIT;
                        continue;
                    }
                }

                /*Skip the color parameter and wait the space after it*/
                if(cmd_state == CMD_STATE_PAR) {
                    if(letter == ' ') {
                        /*Get the parameter*/
                        if(i - par_start == LABEL_RECOLOR_PAR_LENGTH + 1) {
                            char buf[LABEL_RECOLOR_PAR_LENGTH + 1];
                            memcpy(buf, &txt[par_start], LABEL_RECOLOR_PAR_LENGTH);
                            buf[LABEL_RECOLOR_PAR_LENGTH] = '\0';
                            int r, g, b;
                            r       = (hex_char_to_num(buf[0]) << 4) + hex_char_to_num(buf[1]);
                            g       = (hex_char_to_num(buf[2]) << 4) + hex_char_to_num(buf[3]);
                            b       = (hex_char_to_num(buf[4]) << 4) + hex_char_to_num(buf[5]);
                            recolor = lv_color_make(r, g, b);
                        } else {
                            recolor.full = style->text.color.full;
                        }
                        cmd_state = CMD_STATE_IN; /*After the parameter the text is in the command*/
                    }
                    continue;
                }
            }

            lv_color_t color = style->text.color;

            if(cmd_state == CMD_STATE_IN) color = recolor;

            letter_w = lv_font_get_glyph_width(font, letter, letter_next);

            if(sel_start != 0xFFFF && sel_end != 0xFFFF) {
                int char_ind = lv_encoded_get_char_id(txt, i);
                /*Do not draw the rectangle on the character at `sel_start`.*/
                if(char_ind > sel_start && char_ind <= sel_end) {
                    lv_area_t sel_coords;
                    sel_coords.x1 = pos.x;
                    sel_coords.y1 = pos.y;
                    sel_coords.x2 = pos.x + letter_w + style->text.letter_space - 1;
                    sel_coords.y2 = pos.y + line_height - 1;
                    lv_draw_rect(&sel_coords, mask, &sel_style, opa);
                }
            }
            lv_draw_letter(&pos, mask, font, letter, color, opa);

            if(letter_w > 0) {
                pos.x += letter_w + style->text.letter_space;
            }
        }
        /*Go to next line*/
        line_start = line_end;
        line_end += lv_txt_get_next_line(&txt[line_start], font, style->text.letter_space, w, flag);

        pos.x = coords->x1;
        /*Align to middle*/
        if(flag & LV_TXT_FLAG_CENTER) {
            line_width =
                lv_txt_get_width(&txt[line_start], line_end - line_start, font, style->text.letter_space, flag);

            pos.x += (lv_area_get_width(coords) - line_width) / 2;

        }
        /*Align to the right*/
        else if(flag & LV_TXT_FLAG_RIGHT) {
            line_width =
                lv_txt_get_width(&txt[line_start], line_end - line_start, font, style->text.letter_space, flag);
            pos.x += lv_area_get_width(coords) - line_width;
        }

        /*Go the next line position*/
        pos.y += line_height;

        if(pos.y > mask->y2) return;
    }
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
    uint8_t result = 0;

    if(hex >= '0' && hex <= '9') {
        result = hex - '0';
    } else {
        if(hex >= 'a') hex -= 'a' - 'A'; /*Convert to upper case*/

        switch(hex) {
            case 'A': result = 10; break;
            case 'B': result = 11; break;
            case 'C': result = 12; break;
            case 'D': result = 13; break;
            case 'E': result = 14; break;
            case 'F': result = 15; break;
            default: result = 0; break;
        }
    }

    return result;
}
