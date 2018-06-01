/**
 * @file lv_draw.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"

#include <stdio.h>
#include <stdbool.h>
#include "lv_draw.h"
#include "lv_draw_rbasic.h"
#include "lv_draw_vbasic.h"
#include "../lv_misc/lv_fs.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_ufs.h"
#include "../lv_objx/lv_img.h"

/*********************
 *      DEFINES
 *********************/
#define LABEL_RECOLOR_PAR_LENGTH    6

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    CMD_STATE_WAIT,
    CMD_STATE_PAR,
    CMD_STATE_IN,
}cmd_state_t;



/**********************
 *  STATIC PROTOTYPES
 **********************/



static uint8_t hex_char_to_num(char hex);

#if USE_LV_TRIANGLE != 0
static void point_swap(lv_point_t * p1, lv_point_t * p2);
#endif



/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_VDB_SIZE != 0
void (*px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) = lv_vpx;
void (*fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) =  lv_vfill;
void (*letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa) = lv_vletter;
#  if USE_LV_IMG
void (*map_fp)(const lv_area_t * cords_p, const lv_area_t * mask_p,
        const uint8_t * map_p, lv_opa_t opa, bool chroma_key, bool alpha_byte,
        lv_color_t recolor, lv_opa_t recolor_opa) = lv_vmap;
#  endif /*USE_LV_IMG*/
#elif USE_LV_REAL_DRAW != 0
/* px_fp used only by shadow drawing and anti aliasing
 * static void (*px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) = lv_rpx;
 */
static void (*fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) =  lv_rfill;
static void (*letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa) = lv_rletter;
#  if USE_LV_IMG
static void (*map_fp)(const lv_area_t * cords_p, const lv_area_t * mask_p,
        const uint8_t * map_p, lv_opa_t opa, bool chroma_key, bool alpha_byte,
        lv_color_t recolor, lv_opa_t recolor_opa) = lv_rmap;
#  endif /*USE_LV_IMG*/
#else
/*Invalid settings. Compiler error will be thrown*/
static void (*px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) = NULL;
static void (*fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) =  NULL;
static void (*letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa) = NULL;
static void (*map_fp)(const lv_area_t * cords_p, const lv_area_t * mask_p,
        const uint8_t * map_p, lv_opa_t opa, bool chroma_key, bool alpha_byte,
        lv_color_t recolor, lv_opa_t recolor_opa) = NULL;
#endif


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


#if USE_LV_TRIANGLE != 0
/**
 *
 * @param points pointer to an array with 3 points
 * @param mask the triangle will be drawn only in this mask
 * @param color color of the triangle
 */
void lv_draw_triangle(const lv_point_t * points, const lv_area_t * mask, lv_color_t color)
{
    lv_point_t tri[3];

    memcpy(tri, points, sizeof(tri));

    /*Sort the vertices according to their y coordinate (0: y max, 1: y mid, 2:y min)*/
    if(tri[1].y < tri[0].y) point_swap(&tri[1], &tri[0]);
    if(tri[2].y < tri[1].y) point_swap(&tri[2], &tri[1]);
    if(tri[1].y < tri[0].y) point_swap(&tri[1], &tri[0]);

    /*Return is the triangle is degenerated*/
    if(tri[0].x == tri[1].x && tri[0].y == tri[1].y) return;
    if(tri[1].x == tri[2].x && tri[1].y == tri[2].y) return;
    if(tri[0].x == tri[2].x && tri[0].y == tri[2].y) return;

    if(tri[0].x == tri[1].x && tri[1].x == tri[2].x) return;
    if(tri[0].y == tri[1].y && tri[1].y == tri[2].y) return;

    /*Draw the triangle*/
    lv_point_t edge1;
    lv_coord_t dx1 = LV_MATH_ABS(tri[0].x - tri[1].x);
    lv_coord_t sx1 = tri[0].x < tri[1].x ? 1 : -1;
    lv_coord_t dy1 = LV_MATH_ABS(tri[0].y - tri[1].y);
    lv_coord_t sy1 = tri[0].y < tri[1].y ? 1 : -1;
    lv_coord_t err1 = (dx1 > dy1 ? dx1 : -dy1) / 2;
    lv_coord_t err_tmp1;

    lv_point_t edge2;
    lv_coord_t dx2 = LV_MATH_ABS(tri[0].x - tri[2].x);
    lv_coord_t sx2 = tri[0].x < tri[2].x ? 1 : -1;
    lv_coord_t dy2 = LV_MATH_ABS(tri[0].y - tri[2].y);
    lv_coord_t sy2 = tri[0].y < tri[2].y ? 1 : -1;
    lv_coord_t err2 = (dx1 > dy2 ? dx2 : -dy2) / 2;
    lv_coord_t err_tmp2;

    lv_coord_t y1_tmp;
    lv_coord_t y2_tmp;

    edge1.x = tri[0].x;
    edge1.y = tri[0].y;
    edge2.x = tri[0].x;
    edge2.y = tri[0].y;
    lv_area_t act_area;
    lv_area_t draw_area;

    while(1) {
        act_area.x1 = edge1.x;
        act_area.x2 = edge2.x ;
        act_area.y1 = edge1.y;
        act_area.y2 = edge2.y ;


        draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
        draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
        draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
        draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
        draw_area.x2--; /*Do not draw most right pixel because it will be drawn by the adjacent triangle*/
        fill_fp(&draw_area, mask, color, LV_OPA_50);

        /*Calc. the next point of edge1*/
        y1_tmp = edge1.y;
        do {
            if (edge1.x == tri[1].x && edge1.y == tri[1].y) {

                dx1 = LV_MATH_ABS(tri[1].x - tri[2].x);
                sx1 = tri[1].x < tri[2].x ? 1 : -1;
                dy1 = LV_MATH_ABS(tri[1].y - tri[2].y);
                sy1 = tri[1].y < tri[2].y ? 1 : -1;
                err1 = (dx1 > dy1 ? dx1 : -dy1) / 2;
            }
            else if (edge1.x == tri[2].x && edge1.y == tri[2].y) return;
            err_tmp1 = err1;
            if (err_tmp1 >-dx1) {
                err1 -= dy1;
                edge1.x += sx1;
            }
            if (err_tmp1 < dy1) {
                err1 += dx1;
                edge1.y += sy1;
            }
        } while(edge1.y == y1_tmp);

        /*Calc. the next point of edge2*/
        y2_tmp = edge2.y;
        do {
            if (edge2.x == tri[2].x && edge2.y == tri[2].y) return;
            err_tmp2 = err2;
            if (err_tmp2 > -dx2) {
                err2 -= dy2;
                edge2.x += sx2;
            }
            if (err_tmp2 < dy2) {
                err2 += dx2;
                edge2.y += sy2;
            }
        } while(edge2.y == y2_tmp);
    }
}
#endif

/**
 * Write a text
 * @param coords coordinates of the label
 * @param mask the label will be drawn only in this area
 * @param style pointer to a style
 * @param txt 0 terminated text to write
 * @param flag settings for the text from 'txt_flag_t' enum
 * @param offset text offset in x and y direction (NULL if unused)
 *
 */
void lv_draw_label(const lv_area_t * coords,const lv_area_t * mask, const lv_style_t * style,
                    const char * txt, lv_txt_flag_t flag, lv_point_t * offset)
{

    const lv_font_t * font = style->text.font;
    lv_coord_t w;
    if((flag & LV_TXT_FLAG_EXPAND) == 0) {
        w = lv_area_get_width(coords);
    } else {
        lv_point_t p;
        lv_txt_get_size(&p, txt, style->text.font, style->text.letter_space, style->text.line_space, LV_COORD_MAX, flag);
        w = p.x;
    }

    /*Init variables for the first line*/
    lv_coord_t line_length = 0;
    uint32_t line_start = 0;
    uint32_t line_end = lv_txt_get_next_line(txt, font, style->text.letter_space, w, flag);

    lv_point_t pos;
    pos.x = coords->x1;
    pos.y = coords->y1;

    /*Align the line to middle if enabled*/
    if(flag & LV_TXT_FLAG_CENTER) {
        line_length = lv_txt_get_width(&txt[line_start], line_end - line_start,
                                    font, style->text.letter_space, flag);
        pos.x += (w - line_length) / 2;
    }

    cmd_state_t cmd_state = CMD_STATE_WAIT;
    uint32_t i;
    uint16_t par_start = 0;
    lv_color_t recolor;
    lv_coord_t letter_w;

    lv_coord_t x_ofs = 0;
    lv_coord_t y_ofs = 0;
    if(offset != NULL) {
        x_ofs = offset->x;
        y_ofs = offset->y;
        pos.y += y_ofs;
    }

    /*Real draw need a background color for higher bpp letter*/
#if LV_VDB_SIZE == 0
    lv_rletter_set_background(style->body.main_color);
#endif

    /*Write out all lines*/
    while(txt[line_start] != '\0') {
        if(offset != NULL) {
            pos.x += x_ofs;
        }
        /*Write all letter of a line*/
        cmd_state = CMD_STATE_WAIT;
        i = line_start;
        uint32_t letter;
        while(i < line_end) {
            letter = lv_txt_utf8_next(txt, &i);
            /*Handle the re-color command*/
            if((flag & LV_TXT_FLAG_RECOLOR) != 0) {
                if(letter == (uint32_t)LV_TXT_COLOR_CMD[0]) {
                    if(cmd_state == CMD_STATE_WAIT) { /*Start char*/
                        par_start = i;// + lv_txt_utf8_size(txt[i]);
                        cmd_state = CMD_STATE_PAR;
                        continue;
                    } else if(cmd_state == CMD_STATE_PAR) { /*Other start char in parameter escaped cmd. char */
                        cmd_state = CMD_STATE_WAIT;
                    }else if(cmd_state == CMD_STATE_IN) { /*Command end */
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
                            int r,g,b;
                            r = (hex_char_to_num(buf[0]) << 4) + hex_char_to_num(buf[1]);
                            g = (hex_char_to_num(buf[2]) << 4) + hex_char_to_num(buf[3]);
                            b = (hex_char_to_num(buf[4]) << 4) + hex_char_to_num(buf[5]);
                            recolor = LV_COLOR_MAKE(r, g, b);
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

            letter_fp(&pos, mask, font, letter, color, style->text.opa);
            letter_w = lv_font_get_width(font, letter);

            pos.x += letter_w + style->text.letter_space;
        }
        /*Go to next line*/
        line_start = line_end;
        line_end += lv_txt_get_next_line(&txt[line_start], font, style->text.letter_space, w, flag);

        pos.x = coords->x1;
        /*Align to middle*/
        if(flag & LV_TXT_FLAG_CENTER) {
            line_length = lv_txt_get_width(&txt[line_start], line_end - line_start,
                                     font, style->text.letter_space, flag);
            pos.x += (w - line_length) / 2;
        }
        /*Go the next line position*/
        pos.y += lv_font_get_height(font);
        pos.y += style->text.line_space;
    }
}

#if USE_LV_IMG
/**
 * Draw an image
 * @param coords the coordinates of the image
 * @param mask the image will be drawn only in this area
 * @param map_p pointer to a lv_color_t array which contains the pixels of the image
 * @param opa opacity of the image (0..255)
 */
void lv_draw_img(const lv_area_t * coords, const lv_area_t * mask,
             const lv_style_t * style, const void * src)
{

    if(src == NULL) {
        lv_draw_rect(coords, mask, &lv_style_plain);
        lv_draw_label(coords, mask, &lv_style_plain, "No\ndata", LV_TXT_FLAG_NONE, NULL);
        return;
    }

    const uint8_t * u8_p = (uint8_t*) src;
    if(u8_p[0] >= 'A' &&  u8_p[0] <= 'Z') { /*It will be a path of a file*/
#if USE_LV_FILESYSTEM
        lv_fs_file_t file;
        lv_fs_res_t res = lv_fs_open(&file, src, LV_FS_MODE_RD);
        if(res == LV_FS_RES_OK) {
            lv_img_t img_data;
            uint32_t br;
            res = lv_fs_read(&file, &img_data, sizeof(lv_img_t), &br);

            lv_area_t mask_com;    /*Common area of mask and cords*/
            bool union_ok;
            union_ok = lv_area_union(&mask_com, mask, coords);
            if(union_ok == false) {
                lv_fs_close(&file);
                return;
            }

            uint8_t px_size = 0;
            switch(img_data.header.format) {
                case LV_IMG_FORMAT_FILE_RAW_RGB332: px_size = 1; break;
                case LV_IMG_FORMAT_FILE_RAW_RGB565: px_size = 2; break;
                case LV_IMG_FORMAT_FILE_RAW_RGB888: px_size = 4; break;
                default: return;
            }

            if(img_data.header.alpha_byte) {    /*Correction with the alpha byte*/
                px_size++;
                if(img_data.header.format == LV_IMG_FORMAT_FILE_RAW_RGB888) px_size--; /*Stored in the 4 byte anyway*/
            }


            /* Move the file pointer to the start address according to mask*/
            uint32_t start_offset = sizeof(img_data.header);
            start_offset += (lv_area_get_width(coords)) * (mask_com.y1 - coords->y1) * px_size;      /*First row*/
            start_offset += (mask_com.x1 - coords->x1) * px_size;                                    /*First col*/
            lv_fs_seek(&file, start_offset);

            uint32_t useful_data = lv_area_get_width(&mask_com) * px_size;
            uint32_t next_row = lv_area_get_width(coords) * px_size - useful_data;

            lv_area_t line;
            lv_area_copy(&line, &mask_com);
            lv_area_set_height(&line, 1);

            lv_coord_t row;
            uint32_t act_pos;
#if LV_COMPILER_VLA_SUPPORTED
            uint8_t buf[lv_area_get_width(&mask_com) * px_size];
#else
# if LV_HOR_RES > LV_VER_RES
            uint8_t buf[LV_HOR_RES * px_size];
# else
            uint8_t buf[LV_VER_RES * px_size];
# endif
#endif
            for(row = mask_com.y1; row <= mask_com.y2; row ++) {
                res = lv_fs_read(&file, buf, useful_data, &br);

                map_fp(&line, &mask_com, (uint8_t *)buf, style->image.opa, img_data.header.chroma_keyed, img_data.header.alpha_byte,
                        style->image.color, style->image.intense);

                lv_fs_tell(&file, &act_pos);
                lv_fs_seek(&file, act_pos + next_row);
                line.y1++;    /*Go down a line*/
                line.y2++;
            }

            lv_fs_close(&file);

            if(res != LV_FS_RES_OK) {
                lv_draw_rect(coords, mask, &lv_style_plain);
                lv_draw_label(coords, mask, &lv_style_plain, "No data", LV_TXT_FLAG_NONE, NULL);
            }
        }
#endif
    }
    else {
        const lv_img_t * img_var = src;
        lv_area_t mask_com;    /*Common area of mask and coords*/
        bool union_ok;
        union_ok = lv_area_union(&mask_com, mask, coords);
        if(union_ok == false) {
            return;         /*Out of mask*/
        }

        map_fp(coords, mask, img_var->pixel_map, style->image.opa, img_var->header.chroma_keyed, img_var->header.alpha_byte, style->image.color, style->image.intense);

    }

}
#endif



/**********************
 *   STATIC FUNCTIONS
 **********************/



 #if LV_ANTIALIAS != 0
 lv_opa_t antialias_get_opa(lv_coord_t seg, lv_coord_t px_id, lv_opa_t line_opa)
 {
     /* How to calculate the opacity of pixels on the edges which makes the anti-aliasing?
      * For example we have a line like this (y = -0.5 * x):
      *
      *  | _ _
      *    * * |
      *
      * Anti-aliased pixels come to the '*' characters
      * Calculate what percentage of the pixels should be covered if real line (not rasterized) would be drawn:
      * 1. A real line should start on (0;0) and end on (2;1)
      * 2. So the line intersection coordinates on the first pixel: (0;0) (1;0.5) -> 25% covered pixel in average
      * 3. For the second pixel: (1;0.5) (2;1) -> 75% covered pixel in average
      * 4. The equation: (px_id * 2 + 1) / (segment_width * 2)
      *                   segment_width: the line segment which is being anti-aliased (was 2 in the example)
      *                   px_id: pixel ID from 0 to  (segment_width - 1)
      *                   result: [0..1] coverage of the pixel
      */

     /*Accelerate the common segment sizes to avoid division*/
     static const  lv_opa_t seg1[1] = {128};
     static const  lv_opa_t seg2[2] = {64, 192};
     static const  lv_opa_t seg3[3] = {42, 128, 212};
     static const  lv_opa_t seg4[4] = {32, 96, 159, 223};
     static const  lv_opa_t seg5[5] = {26,  76, 128, 178, 230};
     static const  lv_opa_t seg6[6] = {21,  64, 106, 148, 191, 234};
     static const  lv_opa_t seg7[7] = {18,  55, 91, 128, 164, 200, 237};
     static const  lv_opa_t seg8[8] = {16,  48, 80, 112, 143, 175, 207, 239};

     static const lv_opa_t * seg_map[] = {seg1, seg2, seg3, seg4,
                                          seg5, seg6, seg7, seg8};

     if(seg == 0) return LV_OPA_TRANSP;
     else if(seg < 8) return (uint32_t)((uint32_t)seg_map[seg - 1][px_id] * line_opa) >> 8;
     else {
         return ((px_id * 2 + 1) * line_opa) / (2 * seg);
     }

 }



#endif


/**
 * Convert a hexadecimal characters to a number (0..15)
 * @param hex Pointer to a hexadecimal character (0..9, A..F)
 * @return the numerical value of `hex` or 0 on error
 */
static uint8_t hex_char_to_num(char hex)
{
    if(hex >= '0' && hex <= '9') {
        return hex - '0';
    }

    if(hex >= 'a') hex -= 'a' - 'A';    /*Convert to upper case*/

    switch(hex) {
        case 'A': return 10;
        case 'B': return 11;
        case 'C': return 12;
        case 'D': return 13;
        case 'E': return 14;
        case 'F': return 15;
        default: return 0;
    }

    return 0;

}

#if USE_LV_TRIANGLE != 0
/**
 * Swap two points
 * p1 pointer to the first point
 * p2 pointer to the second point
 */
static void point_swap(lv_point_t * p1, lv_point_t * p2)
{
    lv_point_t tmp;
    tmp.x = p1->x;
    tmp.y = p1->y;

    p1->x = p2->x;
    p1->y = p2->y;

    p2->x = tmp.x;
    p2->y = tmp.y;

}

#endif
