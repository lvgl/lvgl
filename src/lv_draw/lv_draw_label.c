/**
 * @file lv_draw_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_label.h"
#include "../lv_misc/lv_math.h"
#include "../lv_hal/lv_hal_disp.h"
#include "../lv_core/lv_refr.h"
#include "../lv_misc/lv_bidi.h"
#include "../lv_misc/lv_debug.h"

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
LV_ATTRIBUTE_FAST_MEM static void lv_draw_letter(const lv_point_t * pos_p, const lv_area_t * clip_area,
                                                 const lv_font_t * font_p,
                                                 uint32_t letter, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);
LV_ATTRIBUTE_FAST_MEM static void draw_letter_normal(lv_coord_t pos_x, lv_coord_t pos_y, lv_font_glyph_dsc_t * g,
                                                     const lv_area_t * clip_area,
                                                     const uint8_t * map_p, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);
static void draw_letter_subpx(lv_coord_t pos_x, lv_coord_t pos_y, lv_font_glyph_dsc_t * g, const lv_area_t * clip_area,
                              const uint8_t * map_p, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);


static uint8_t hex_char_to_num(char hex);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

const uint8_t _lv_bpp1_opa_table[2]  = {0, 255};          /*Opacity mapping with bpp = 1 (Just for compatibility)*/
const uint8_t _lv_bpp2_opa_table[4]  = {0, 85, 170, 255}; /*Opacity mapping with bpp = 2*/

const uint8_t _lv_bpp3_opa_table[8]  = {0, 36,  73, 109,   /*Opacity mapping with bpp = 3*/
                                        146, 182,  219, 255
                                       };

const uint8_t _lv_bpp4_opa_table[16] = {0,  17, 34,  51,  /*Opacity mapping with bpp = 4*/
                                        68, 85, 102, 119,
                                        136, 153, 170, 187,
                                        204, 221, 238, 255
                                       };
const uint8_t _lv_bpp8_opa_table[256] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                                         16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                                         32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                                         48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
                                         64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
                                         80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
                                         96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
                                         112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
                                         128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
                                         144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
                                         160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
                                         176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
                                         192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
                                         208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
                                         224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
                                         240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
                                        };

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_label_dsc_init(lv_draw_label_dsc_t * dsc)
{
    _lv_memset_00(dsc, sizeof(lv_draw_label_dsc_t));
    dsc->opa = LV_OPA_COVER;
    dsc->color = LV_COLOR_BLACK;
    dsc->font = LV_THEME_DEFAULT_FONT_NORMAL;
    dsc->sel_start = LV_DRAW_LABEL_NO_TXT_SEL;
    dsc->sel_end = LV_DRAW_LABEL_NO_TXT_SEL;
    dsc->sel_color = LV_COLOR_BLACK;
    dsc->sel_bg_color = LV_COLOR_BLUE;
    dsc->bidi_dir = LV_BIDI_DIR_LTR;
}

/**
 * Write a text
 * @param coords coordinates of the label
 * @param mask the label will be drawn only in this area
 * @param dsc pointer to draw descriptor
 * @param txt `\0` terminated text to write
 * @param hint pointer to a `lv_draw_label_hint_t` variable.
 * It is managed by the drawer to speed up the drawing of very long texts (thousands of lines).
 */
LV_ATTRIBUTE_FAST_MEM void lv_draw_label(const lv_area_t * coords, const lv_area_t * mask,
                                         const lv_draw_label_dsc_t * dsc,
                                         const char * txt,
                                         lv_draw_label_hint_t * hint)
{

    if(dsc->opa <= LV_OPA_MIN) return;
    const lv_font_t * font = dsc->font;
    int32_t w;

    /*No need to waste processor time if string is empty*/
    if(txt[0] == '\0')  return;

    lv_area_t clipped_area;
    bool clip_ok = _lv_area_intersect(&clipped_area, coords, mask);
    if(!clip_ok) return;


    if((dsc->flag & LV_TXT_FLAG_EXPAND) == 0) {
        /*Normally use the label's width as width*/
        w = lv_area_get_width(coords);
    }
    else {
        /*If EXAPND is enabled then not limit the text's width to the object's width*/
        lv_point_t p;
        _lv_txt_get_size(&p, txt, dsc->font, dsc->letter_space, dsc->line_space, LV_COORD_MAX,
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
    if(hint && y_ofs == 0 && coords->y1 < 0) {
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

    uint32_t line_end = line_start + _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, dsc->flag);

    /*Go the first visible line*/
    while(pos.y + line_height_font < mask->y1) {
        /*Go to next line*/
        line_start = line_end;
        line_end += _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, dsc->flag);
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
    if(dsc->flag & LV_TXT_FLAG_CENTER) {
        line_width = _lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);

        pos.x += (lv_area_get_width(coords) - line_width) / 2;

    }
    /*Align to the right*/
    else if(dsc->flag & LV_TXT_FLAG_RIGHT) {
        line_width = _lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);
        pos.x += lv_area_get_width(coords) - line_width;
    }

    lv_opa_t opa = dsc->opa;

    uint32_t sel_start = dsc->sel_start;
    uint32_t sel_end = dsc->sel_end;
    if(sel_start > sel_end) {
        uint32_t tmp = sel_start;
        sel_start = sel_end;
        sel_end = tmp;
    }
    lv_draw_line_dsc_t line_dsc;

    if((dsc->decor & LV_TEXT_DECOR_UNDERLINE) || (dsc->decor & LV_TEXT_DECOR_STRIKETHROUGH)) {
        lv_draw_line_dsc_init(&line_dsc);
        line_dsc.color = dsc->color;
        line_dsc.width = font->underline_thickness ? font->underline_thickness : 1;
        line_dsc.opa = dsc->opa;
        line_dsc.blend_mode = dsc->blend_mode;
    }

    cmd_state_t cmd_state = CMD_STATE_WAIT;
    uint32_t i;
    uint32_t par_start = 0;
    lv_color_t recolor;
    int32_t letter_w;


    lv_draw_rect_dsc_t draw_dsc_sel;
    lv_draw_rect_dsc_init(&draw_dsc_sel);
    draw_dsc_sel.bg_color = dsc->sel_bg_color;

    int32_t pos_x_start = pos.x;
    /*Write out all lines*/
    while(txt[line_start] != '\0') {
        pos.x += x_ofs;

        /*Write all letter of a line*/
        cmd_state = CMD_STATE_WAIT;
        i         = 0;
#if LV_USE_BIDI
        char * bidi_txt = _lv_mem_buf_get(line_end - line_start + 1);
        _lv_bidi_process_paragraph(txt + line_start, bidi_txt, line_end - line_start, dsc->bidi_dir, NULL, 0);
#else
        const char * bidi_txt = txt + line_start;
#endif

        while(i < line_end - line_start) {
            uint32_t logical_char_pos = 0;
            if(sel_start != 0xFFFF && sel_end != 0xFFFF) {
#if LV_USE_BIDI
                logical_char_pos = _lv_txt_encoded_get_char_id(txt, line_start);
                uint32_t t = _lv_txt_encoded_get_char_id(bidi_txt, i);
                logical_char_pos += _lv_bidi_get_logical_pos(bidi_txt, NULL, line_end - line_start, dsc->bidi_dir, t, NULL);
#else
                logical_char_pos = _lv_txt_encoded_get_char_id(txt, line_start + i);
#endif
            }

            uint32_t letter      = _lv_txt_encoded_next(bidi_txt, &i);
            uint32_t letter_next = _lv_txt_encoded_next(&bidi_txt[i], NULL);

            /*Handle the re-color command*/
            if((dsc->flag & LV_TXT_FLAG_RECOLOR) != 0) {
                if(letter == (uint32_t)LV_TXT_COLOR_CMD[0]) {
                    if(cmd_state == CMD_STATE_WAIT) { /*Start char*/
                        par_start = i;
                        cmd_state = CMD_STATE_PAR;
                        continue;
                    }
                    else if(cmd_state == CMD_STATE_PAR) {   /*Other start char in parameter escaped cmd. char */
                        cmd_state = CMD_STATE_WAIT;
                    }
                    else if(cmd_state == CMD_STATE_IN) {   /*Command end */
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
                            _lv_memcpy_small(buf, &bidi_txt[par_start], LABEL_RECOLOR_PAR_LENGTH);
                            buf[LABEL_RECOLOR_PAR_LENGTH] = '\0';
                            int r, g, b;
                            r       = (hex_char_to_num(buf[0]) << 4) + hex_char_to_num(buf[1]);
                            g       = (hex_char_to_num(buf[2]) << 4) + hex_char_to_num(buf[3]);
                            b       = (hex_char_to_num(buf[4]) << 4) + hex_char_to_num(buf[5]);
                            recolor = lv_color_make(r, g, b);
                        }
                        else {
                            recolor.full = dsc->color.full;
                        }
                        cmd_state = CMD_STATE_IN; /*After the parameter the text is in the command*/
                    }
                    continue;
                }
            }

            lv_color_t color = dsc->color;

            if(cmd_state == CMD_STATE_IN) color = recolor;

            letter_w = lv_font_get_glyph_width(font, letter, letter_next);

            if(sel_start != 0xFFFF && sel_end != 0xFFFF) {
                if(logical_char_pos >= sel_start && logical_char_pos < sel_end) {
                    lv_area_t sel_coords;
                    sel_coords.x1 = pos.x;
                    sel_coords.y1 = pos.y;
                    sel_coords.x2 = pos.x + letter_w + dsc->letter_space - 1;
                    sel_coords.y2 = pos.y + line_height - 1;
                    lv_draw_rect(&sel_coords, mask, &draw_dsc_sel);
                    color = dsc->sel_color;
                }
            }

            lv_draw_letter(&pos, mask, font, letter, color, opa, dsc->blend_mode);

            if(letter_w > 0) {
                pos.x += letter_w + dsc->letter_space;
            }
        }

        if(dsc->decor & LV_TEXT_DECOR_STRIKETHROUGH) {
            lv_point_t p1;
            lv_point_t p2;
            p1.x = pos_x_start;
            p1.y = pos.y + (dsc->font->line_height / 2)  + line_dsc.width / 2;
            p2.x = pos.x;
            p2.y = p1.y;
            lv_draw_line(&p1, &p2, mask, &line_dsc);
        }

        if(dsc->decor  & LV_TEXT_DECOR_UNDERLINE) {
            lv_point_t p1;
            lv_point_t p2;
            p1.x = pos_x_start;
            p1.y = pos.y + dsc->font->line_height - dsc->font->base_line - font->underline_position;
            p2.x = pos.x;
            p2.y = p1.y;
            lv_draw_line(&p1, &p2, mask, &line_dsc);
        }

#if LV_USE_BIDI
        _lv_mem_buf_release(bidi_txt);
        bidi_txt = NULL;
#endif
        /*Go to next line*/
        line_start = line_end;
        line_end += _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, dsc->flag);

        pos.x = coords->x1;
        /*Align to middle*/
        if(dsc->flag & LV_TXT_FLAG_CENTER) {
            line_width =
                _lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);

            pos.x += (lv_area_get_width(coords) - line_width) / 2;

        }
        /*Align to the right*/
        else if(dsc->flag & LV_TXT_FLAG_RIGHT) {
            line_width =
                _lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);
            pos.x += lv_area_get_width(coords) - line_width;
        }

        /*Go the next line position*/
        pos.y += line_height;

        if(pos.y > mask->y2) return;
    }

    LV_ASSERT_MEM_INTEGRITY();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Draw a letter in the Virtual Display Buffer
 * @param pos_p left-top coordinate of the latter
 * @param mask_p the letter will be drawn only on this area  (truncated to VDB area)
 * @param font_p pointer to font
 * @param letter a letter to draw
 * @param color color of letter
 * @param opa opacity of letter (0..255)
 */
LV_ATTRIBUTE_FAST_MEM static void lv_draw_letter(const lv_point_t * pos_p, const lv_area_t * clip_area,
                                                 const lv_font_t * font_p,
                                                 uint32_t letter,
                                                 lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode)
{
    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    if(font_p == NULL) {
        LV_LOG_WARN("lv_draw_letter: font is NULL");
        return;
    }

    lv_font_glyph_dsc_t g;
    bool g_ret = lv_font_get_glyph_dsc(font_p, &g, letter, '\0');
    if(g_ret == false)  {
        /* Add waring if the dsc is not found
         * but do not print warning for non printable ASCII chars (e.g. '\n')*/
        if(letter >= 0x20) {
            LV_LOG_WARN("lv_draw_letter: glyph dsc. not found");
        }
        return;
    }

    /* Don't draw anything if the character is empty. E.g. space */
    if((g.box_h == 0) || (g.box_w == 0)) return;

    int32_t pos_x = pos_p->x + g.ofs_x;
    int32_t pos_y = pos_p->y + (font_p->line_height - font_p->base_line) - g.box_h - g.ofs_y;

    /*If the letter is completely out of mask don't draw it */
    if(pos_x + g.box_w < clip_area->x1 ||
       pos_x > clip_area->x2 ||
       pos_y + g.box_h < clip_area->y1 ||
       pos_y > clip_area->y2)  {
        return;
    }


    const uint8_t * map_p = lv_font_get_glyph_bitmap(font_p, letter);
    if(map_p == NULL) {
        LV_LOG_WARN("lv_draw_letter: character's bitmap not found");
        return;
    }

    if(font_p->subpx) {
        draw_letter_subpx(pos_x, pos_y, &g, clip_area, map_p, color, opa, blend_mode);
    }
    else {
        draw_letter_normal(pos_x, pos_y, &g, clip_area, map_p, color, opa, blend_mode);
    }
}


LV_ATTRIBUTE_FAST_MEM static void draw_letter_normal(lv_coord_t pos_x, lv_coord_t pos_y, lv_font_glyph_dsc_t * g,
                                                     const lv_area_t * clip_area,
                                                     const uint8_t * map_p, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode)
{
    const uint8_t * bpp_opa_table_p;
    uint32_t bitmask_init;
    uint32_t bitmask;
    uint32_t bpp = g->bpp;
    uint32_t shades;
    if(bpp == 3) bpp = 4;

    switch(bpp) {
        case 1:
            bpp_opa_table_p = _lv_bpp1_opa_table;
            bitmask_init  = 0x80;
            shades = 2;
            break;
        case 2:
            bpp_opa_table_p = _lv_bpp2_opa_table;
            bitmask_init  = 0xC0;
            shades = 4;
            break;
        case 4:
            bpp_opa_table_p = _lv_bpp4_opa_table;
            bitmask_init  = 0xF0;
            shades = 16;
            break;
        case 8:
            bpp_opa_table_p = _lv_bpp8_opa_table;
            bitmask_init  = 0xFF;
            shades = 256;
            break;       /*No opa table, pixel value will be used directly*/
        default:
            LV_LOG_WARN("lv_draw_letter: invalid bpp");
            return; /*Invalid bpp. Can't render the letter*/
    }

    static lv_opa_t opa_table[256];
    static lv_opa_t prev_opa = LV_OPA_TRANSP;
    static uint32_t prev_bpp = 0;
    if(opa < LV_OPA_MAX) {
        if(prev_opa != opa || prev_bpp != bpp) {
            uint32_t i;
            for(i = 0; i < shades; i++) {
                opa_table[i] = bpp_opa_table_p[i] == LV_OPA_COVER ? opa : ((bpp_opa_table_p[i] * opa) >> 8);
            }
        }
        bpp_opa_table_p = opa_table;
        prev_opa = opa;
        prev_bpp = bpp;
    }

    int32_t col, row;
    int32_t box_w = g->box_w;
    int32_t box_h = g->box_h;
    int32_t width_bit = box_w * bpp; /*Letter width in bits*/

    /* Calculate the col/row start/end on the map*/
    int32_t col_start = pos_x >= clip_area->x1 ? 0 : clip_area->x1 - pos_x;
    int32_t col_end   = pos_x + box_w <= clip_area->x2 ? box_w : clip_area->x2 - pos_x + 1;
    int32_t row_start = pos_y >= clip_area->y1 ? 0 : clip_area->y1 - pos_y;
    int32_t row_end   = pos_y + box_h <= clip_area->y2 ? box_h : clip_area->y2 - pos_y + 1;

    /*Move on the map too*/
    uint32_t bit_ofs = (row_start * width_bit) + (col_start * bpp);
    map_p += bit_ofs >> 3;

    uint8_t letter_px;
    uint32_t col_bit;
    col_bit = bit_ofs & 0x7; /* "& 0x7" equals to "% 8" just faster */

    lv_coord_t hor_res = lv_disp_get_hor_res(_lv_refr_get_disp_refreshing());
    uint32_t mask_buf_size = box_w * box_h > hor_res ? hor_res : box_w * box_h;
    lv_opa_t * mask_buf = _lv_mem_buf_get(mask_buf_size);
    int32_t mask_p = 0;

    lv_area_t fill_area;
    fill_area.x1 = col_start + pos_x;
    fill_area.x2 = col_end  + pos_x - 1;
    fill_area.y1 = row_start + pos_y;
    fill_area.y2 = fill_area.y1;

    uint8_t other_mask_cnt = lv_draw_mask_get_cnt();

    uint32_t col_bit_max = 8 - bpp;
    uint32_t col_bit_row_ofs = (box_w + col_start - col_end) * bpp;

    for(row = row_start ; row < row_end; row++) {
        int32_t mask_p_start = mask_p;

        bitmask = bitmask_init >> col_bit;
        for(col = col_start; col < col_end; col++) {
            /*Load the pixel's opacity into the mask*/
            letter_px = (*map_p & bitmask) >> (col_bit_max - col_bit);
            if(letter_px) {
                mask_buf[mask_p] = bpp_opa_table_p[letter_px];
            }
            else {
                mask_buf[mask_p] = 0;
            }

            /*Go to the next column*/
            if(col_bit < col_bit_max) {
                col_bit += bpp;
                bitmask = bitmask >> bpp;
            }
            else {
                col_bit = 0;
                bitmask = bitmask_init;
                map_p++;
            }

            /*Next mask byte*/
            mask_p++;
        }


        /*Apply masks if any*/
        if(other_mask_cnt) {
            lv_draw_mask_res_t mask_res = lv_draw_mask_apply(mask_buf + mask_p_start, fill_area.x1, fill_area.y2,
                                                             lv_area_get_width(&fill_area));
            if(mask_res == LV_DRAW_MASK_RES_TRANSP) {
                _lv_memset_00(mask_buf + mask_p_start, lv_area_get_width(&fill_area));
            }
        }

        if((uint32_t) mask_p + (col_end - col_start) < mask_buf_size) {
            fill_area.y2 ++;
        }
        else {
            _lv_blend_fill(clip_area, &fill_area,
                           color, mask_buf, LV_DRAW_MASK_RES_CHANGED, LV_OPA_COVER,
                           blend_mode);

            fill_area.y1 = fill_area.y2 + 1;
            fill_area.y2 = fill_area.y1;
            mask_p = 0;
        }

        col_bit += col_bit_row_ofs;
        map_p += (col_bit >> 3);
        col_bit = col_bit & 0x7;
    }

    /*Flush the last part*/
    if(fill_area.y1 != fill_area.y2) {
        fill_area.y2--;
        _lv_blend_fill(clip_area, &fill_area,
                       color, mask_buf, LV_DRAW_MASK_RES_CHANGED, LV_OPA_COVER,
                       blend_mode);
        mask_p = 0;
    }

    _lv_mem_buf_release(mask_buf);
}

static void draw_letter_subpx(lv_coord_t pos_x, lv_coord_t pos_y, lv_font_glyph_dsc_t * g, const lv_area_t * clip_area,
                              const uint8_t * map_p, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode)
{
#if LV_USE_FONT_SUBPX
    const uint8_t * bpp_opa_table;
    uint32_t bitmask_init;
    uint32_t bitmask;
    uint32_t bpp = g->bpp;
    if(bpp == 3) bpp = 4;

    switch(bpp) {
        case 1:
            bpp_opa_table = _lv_bpp1_opa_table;
            bitmask_init  = 0x80;
            break;
        case 2:
            bpp_opa_table = _lv_bpp2_opa_table;
            bitmask_init  = 0xC0;
            break;
        case 4:
            bpp_opa_table = _lv_bpp4_opa_table;
            bitmask_init  = 0xF0;
            break;
        case 8:
            bpp_opa_table = _lv_bpp8_opa_table;
            bitmask_init  = 0xFF;
            break;       /*No opa table, pixel value will be used directly*/
        default:
            LV_LOG_WARN("lv_draw_letter: invalid bpp not found");
            return; /*Invalid bpp. Can't render the letter*/
    }

    int32_t col, row;

    int32_t box_w = g->box_w;
    int32_t box_h = g->box_h;
    int32_t width_bit = box_w * bpp; /*Letter width in bits*/


    /* Calculate the col/row start/end on the map*/
    int32_t col_start = pos_x >= clip_area->x1 ? 0 : (clip_area->x1 - pos_x) * 3;
    int32_t col_end   = pos_x + box_w / 3 <= clip_area->x2 ? box_w : (clip_area->x2 - pos_x + 1) * 3;
    int32_t row_start = pos_y >= clip_area->y1 ? 0 : clip_area->y1 - pos_y;
    int32_t row_end   = pos_y + box_h <= clip_area->y2 ? box_h : clip_area->y2 - pos_y + 1;

    /*Move on the map too*/
    int32_t bit_ofs = (row_start * width_bit) + (col_start * bpp);
    map_p += bit_ofs >> 3;

    uint8_t letter_px;
    lv_opa_t px_opa;
    int32_t col_bit;
    col_bit = bit_ofs & 0x7; /* "& 0x7" equals to "% 8" just faster */

    int32_t mask_buf_size = box_w * box_h > LV_HOR_RES_MAX ? LV_HOR_RES_MAX : g->box_w * g->box_h;
    lv_opa_t * mask_buf = _lv_mem_buf_get(mask_buf_size);
    int32_t mask_p = 0;

    lv_color_t * color_buf = _lv_mem_buf_get(mask_buf_size * sizeof(lv_color_t));

    lv_disp_t * disp    = _lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    int32_t vdb_width     = lv_area_get_width(&vdb->area);
    lv_color_t * vdb_buf_tmp = vdb->buf_act;

    /*Set a pointer on VDB to the first pixel of the letter*/
    vdb_buf_tmp += ((pos_y - vdb->area.y1) * vdb_width) + pos_x - vdb->area.x1;

    /*If the letter is partially out of mask the move there on VDB*/
    vdb_buf_tmp += (row_start * vdb_width) + col_start / 3;

    lv_area_t map_area;
    map_area.x1 = col_start / 3 + pos_x;
    map_area.x2 = col_end / 3  + pos_x - 1;
    map_area.y1 = row_start + pos_y;
    map_area.y2 = map_area.y1;

    uint8_t other_mask_cnt = lv_draw_mask_get_cnt();

    uint8_t font_rgb[3];

#if LV_COLOR_16_SWAP == 0
    uint8_t txt_rgb[3] = {color.ch.red, color.ch.green, color.ch.blue};
#else
    uint8_t txt_rgb[3] = {color.ch.red, (color.ch.green_h << 3) + color.ch.green_l, color.ch.blue};
#endif

    for(row = row_start ; row < row_end; row++) {
        uint32_t subpx_cnt = 0;
        bitmask = bitmask_init >> col_bit;
        int32_t mask_p_start = mask_p;

        for(col = col_start; col < col_end; col++) {
            /*Load the pixel's opacity into the mask*/
            letter_px = (*map_p & bitmask) >> (8 - col_bit - bpp);
            if(letter_px != 0) {
                if(opa == LV_OPA_COVER) {
                    px_opa = bpp == 8 ? letter_px : bpp_opa_table[letter_px];
                }
                else {
                    px_opa = bpp == 8 ? (uint32_t)((uint32_t)letter_px * opa) >> 8
                             : (uint32_t)((uint32_t)bpp_opa_table[letter_px] * opa) >> 8;
                }
            }
            else {
                px_opa = 0;
            }

            font_rgb[subpx_cnt] = px_opa;

            subpx_cnt ++;
            if(subpx_cnt == 3) {
                subpx_cnt = 0;

                lv_color_t res_color;
#if LV_COLOR_16_SWAP == 0
                uint8_t bg_rgb[3] = {vdb_buf_tmp->ch.red, vdb_buf_tmp->ch.green, vdb_buf_tmp->ch.blue};
#else
                uint8_t bg_rgb[3] = {vdb_buf_tmp->ch.red,
                                     (vdb_buf_tmp->ch.green_h << 3) + vdb_buf_tmp->ch.green_l,
                                     vdb_buf_tmp->ch.blue
                                    };
#endif

#if LV_FONT_SUBPX_BGR
                res_color.ch.blue = (uint32_t)((uint32_t)txt_rgb[0] * font_rgb[0] + (bg_rgb[0] * (255 - font_rgb[0]))) >> 8;
                res_color.ch.red = (uint32_t)((uint32_t)txt_rgb[2] * font_rgb[2] + (bg_rgb[2] * (255 - font_rgb[2]))) >> 8;
#else
                res_color.ch.red = (uint32_t)((uint16_t)txt_rgb[0] * font_rgb[0] + (bg_rgb[0] * (255 - font_rgb[0]))) >> 8;
                res_color.ch.blue = (uint32_t)((uint16_t)txt_rgb[2] * font_rgb[2] + (bg_rgb[2] * (255 - font_rgb[2]))) >> 8;
#endif

#if LV_COLOR_16_SWAP == 0
                res_color.ch.green = (uint32_t)((uint32_t)txt_rgb[1] * font_rgb[1] + (bg_rgb[1] * (255 - font_rgb[1]))) >> 8;
#else
                uint8_t green = (uint32_t)((uint32_t)txt_rgb[1] * font_rgb[1] + (bg_rgb[1] * (255 - font_rgb[1]))) >> 8;
                res_color.ch.green_h = green >> 3;
                res_color.ch.green_l = green & 0x7;
#endif

#if LV_COLOR_DEPTH == 32
                res_color.ch.alpha =  0xff;
#endif

                if(font_rgb[0] == 0 && font_rgb[1] == 0 && font_rgb[2] == 0) mask_buf[mask_p] = LV_OPA_TRANSP;
                else mask_buf[mask_p] = LV_OPA_COVER;
                color_buf[mask_p] = res_color;

                /*Next mask byte*/
                mask_p++;
                vdb_buf_tmp++;
            }

            /*Go to the next column*/
            if(col_bit < (int32_t)(8 - bpp)) {
                col_bit += bpp;
                bitmask = bitmask >> bpp;
            }
            else {
                col_bit = 0;
                bitmask = bitmask_init;
                map_p++;
            }
        }

        /*Apply masks if any*/
        if(other_mask_cnt) {
            lv_draw_mask_res_t mask_res = lv_draw_mask_apply(mask_buf + mask_p_start, map_area.x1, map_area.y2,
                                                             lv_area_get_width(&map_area));
            if(mask_res == LV_DRAW_MASK_RES_TRANSP) {
                _lv_memset_00(mask_buf + mask_p_start, lv_area_get_width(&map_area));
            }
        }

        if((int32_t) mask_p + (col_end - col_start) < mask_buf_size) {
            map_area.y2 ++;
        }
        else {
            _lv_blend_map(clip_area, &map_area, color_buf, mask_buf, LV_DRAW_MASK_RES_CHANGED, opa, blend_mode);

            map_area.y1 = map_area.y2 + 1;
            map_area.y2 = map_area.y1;
            mask_p = 0;
        }

        col_bit += ((box_w - col_end) + col_start) * bpp;

        map_p += (col_bit >> 3);
        col_bit = col_bit & 0x7;

        /*Next row in VDB*/
        vdb_buf_tmp += vdb_width - (col_end - col_start) / 3;
    }

    /*Flush the last part*/
    if(map_area.y1 != map_area.y2) {
        map_area.y2--;
        _lv_blend_map(clip_area, &map_area, color_buf, mask_buf, LV_DRAW_MASK_RES_CHANGED, opa, blend_mode);
    }

    _lv_mem_buf_release(mask_buf);
    _lv_mem_buf_release(color_buf);
#else
    LV_LOG_WARN("Can't draw sub-pixel rendered letter because LV_USE_FONT_SUBPX == 0 in lv_conf.h");
#endif
}


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
    }
    else {
        if(hex >= 'a') hex -= 'a' - 'A'; /*Convert to upper case*/

        switch(hex) {
            case 'A':
                result = 10;
                break;
            case 'B':
                result = 11;
                break;
            case 'C':
                result = 12;
                break;
            case 'D':
                result = 13;
                break;
            case 'E':
                result = 14;
                break;
            case 'F':
                result = 15;
                break;
            default:
                result = 0;
                break;
        }
    }

    return result;
}
