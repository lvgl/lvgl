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
#include "../lv_misc/lv_circ.h"
#include "../lv_misc/lv_fs.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_ufs.h"
#include "../lv_objx/lv_img.h"

/*********************
 *      DEFINES
 *********************/
#define LINE_WIDTH_CORR_BASE 64
#define LINE_WIDTH_CORR_SHIFT 6

#define LABEL_RELV_COLOR_PAR_LENGTH    6

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
static void lv_draw_rect_main_mid(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style);
static void lv_draw_rect_main_corner(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style);
static void lv_draw_rect_border_straight(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style);
static void lv_draw_rect_border_corner(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style);
#if USE_LV_SHADOW && LV_VDB_SIZE
static void lv_draw_rect_shadow(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style);
static void lv_draw_cont_shadow_full(const lv_area_t * coords, const lv_area_t * mask, const  lv_style_t * style);
static void lv_draw_cont_shadow_bottom(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style);
static void lv_draw_cont_shadow_full_straight(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, const lv_opa_t * map);
#endif
static uint16_t lv_draw_cont_radius_corr(uint16_t r, lv_coord_t w, lv_coord_t h);


#if USE_LV_TRIANGLE != 0
static void point_swap(lv_point_t * p1, lv_point_t * p2);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_VDB_SIZE != 0
#if USE_LV_SHADOW
static void (*px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) = lv_vpx;
#endif
static void (*fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) =  lv_vfill;
static void (*letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa) = lv_vletter;
#if USE_LV_IMG
static void (*map_fp)(const lv_area_t * coords, const lv_area_t * mask, const lv_color_t * map_p, lv_opa_t opa, bool transp, bool upscale, lv_color_t recolor, lv_opa_t recolor_opa) = lv_vmap;
#endif
#else
/* px_fp used only by shadow drawing the shadows are not drawn with out VDB
 * static void (*px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) = lv_rpx;
 */
static void (*fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) =  lv_rfill;
static void (*letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa) = lv_rletter;
#if USE_LV_IMG
static void (*map_fp)(const lv_area_t * coords, const lv_area_t * mask, const lv_color_t * map_p, lv_opa_t opa, bool transp, bool upscale, lv_color_t recolor, lv_opa_t recolor_opa) = lv_rmap;
#endif
#endif


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Draw a rectangle 
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param style pointer to a style
 */
void lv_draw_rect(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style)
{
    if(lv_area_get_height(coords) < 1 || lv_area_get_width(coords) < 1) return;

    lv_area_t coord_aa;
    lv_area_t mask_aa;
#if LV_ANTIALIAS == 0
    lv_area_copy(&coord_aa, coords);
    lv_area_copy(&mask_aa, mask);
#else
    coord_aa.x1 = coords->x1 << LV_AA;
    coord_aa.y1 = coords->y1 << LV_AA;
    coord_aa.x2 = (coords->x2 << LV_AA) + 1;
    coord_aa.y2 = (coords->y2 << LV_AA) + 1;

    mask_aa.x1 = mask->x1 << LV_AA;
    mask_aa.y1 = mask->y1 << LV_AA;
    mask_aa.x2 = (mask->x2 << LV_AA) + 1;
    mask_aa.y2 = (mask->y2 << LV_AA) + 1;
#endif

#if USE_LV_SHADOW && LV_VDB_SIZE
    if(style->body.shadow.width != 0) {
        lv_draw_rect_shadow(&coord_aa, &mask_aa, style);
    }
#endif
    if(style->body.empty == 0){
        lv_draw_rect_main_mid(&coord_aa, &mask_aa, style);

        if(style->body.radius != 0) {
            lv_draw_rect_main_corner(&coord_aa, &mask_aa, style);
        }
    } 
    
    if(style->body.border.width != 0 && style->body.border.part != LV_BORDER_NONE) {
        lv_draw_rect_border_straight(&coord_aa, &mask_aa, style);

        if(style->body.radius != 0) {
            lv_draw_rect_border_corner(&coord_aa, &mask_aa, style);
        }
    }
}

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
    lv_area_t coord_aa;
    lv_area_t mask_aa;

#if LV_ANTIALIAS == 0
    lv_area_copy(&coord_aa, coords);
    lv_area_copy(&mask_aa, mask);
#else
    coord_aa.x1 = coords->x1 << LV_AA;
    coord_aa.y1 = coords->y1 << LV_AA;
    coord_aa.x2 = (coords->x2 << LV_AA) + 1;
    coord_aa.y2 = (coords->y2 << LV_AA) + 1;

    mask_aa.x1 = mask->x1 << LV_AA;
    mask_aa.y1 = mask->y1 << LV_AA;
    mask_aa.x2 = (mask->x2 << LV_AA) + 1;
    mask_aa.y2 = (mask->y2 << LV_AA) + 1;
#endif

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
    pos.x = coord_aa.x1;
    pos.y = coord_aa.y1;

    /*Align the line to middle if enabled*/
    if(flag & LV_TXT_FLAG_CENTER) {
        line_length = lv_txt_get_width(&txt[line_start], line_end - line_start,
                                    font, style->text.letter_space, flag);
        pos.x += ((w - line_length) / 2) << LV_AA;
    }

    cmd_state_t cmd_state = CMD_STATE_WAIT;
    uint32_t i;
    uint16_t par_start = 0;
    lv_color_t recolor;
    lv_coord_t letter_w;

    if(offset != NULL) {
        pos.y += offset->y;
    }

    /*Write out all lines*/
    while(txt[line_start] != '\0') {
        if(offset != NULL) {
            pos.x += offset->x;
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
                        if(i - par_start == LABEL_RELV_COLOR_PAR_LENGTH + 1) {
                            char buf[LABEL_RELV_COLOR_PAR_LENGTH + 1];
                            memcpy(buf, &txt[par_start], LABEL_RELV_COLOR_PAR_LENGTH);
                            buf[LABEL_RELV_COLOR_PAR_LENGTH] = '\0';
                            int r,g,b;
                            sscanf(buf, "%02x%02x%02x", &r, &g, &b);
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
            letter_fp(&pos, &mask_aa, font, letter, color, style->text.opa);
            letter_w = lv_font_get_width(font, letter) >> LV_FONT_ANTIALIAS;

            pos.x += letter_w + (style->text.letter_space << LV_AA);
            /* Round error occurs in x position
             * When odd widths are scaled down the last 1 is lost. So the letters seems shorter.
             * Now calculate according to is to be consequent  */
            if(letter_w & 0x01) pos.x--;
        }
        /*Go to next line*/
        line_start = line_end;
        line_end += lv_txt_get_next_line(&txt[line_start], font, style->text.letter_space, w, flag);

        pos.x = coord_aa.x1;
        /*Align to middle*/
        if(flag & LV_TXT_FLAG_CENTER) {
            line_length = lv_txt_get_width(&txt[line_start], line_end - line_start,
                                     font, style->text.letter_space, flag);
            pos.x += ((w - line_length) / 2) << LV_AA;
        }
        /*Go the next line position*/
        pos.y += lv_font_get_height(font) >> LV_FONT_ANTIALIAS;
        pos.y += style->text.line_space << LV_AA;
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
             const lv_style_t * style, const char * fn)
{
    if(fn == NULL) {
        lv_draw_rect(coords, mask, &lv_style_plain);
        lv_draw_label(coords, mask, &lv_style_plain, "No data", LV_TXT_FLAG_NONE, NULL);
    } else {
        lv_area_t coord_aa;
        lv_area_t mask_aa;

#if LV_ANTIALIAS == 0
        lv_area_copy(&coord_aa, coords);
        lv_area_copy(&mask_aa, mask);
#else
        coord_aa.x1 = coords->x1 << LV_AA;
        coord_aa.y1 = coords->y1 << LV_AA;
        coord_aa.x2 = (coords->x2 << LV_AA) + 1;
        coord_aa.y2 = (coords->y2 << LV_AA) + 1;

        mask_aa.x1 = mask->x1 << LV_AA;
        mask_aa.y1 = mask->y1 << LV_AA;
        mask_aa.x2 = (mask->x2 << LV_AA) + 1;
        mask_aa.y2 = (mask->y2 << LV_AA) + 1;
#endif

        lv_fs_file_t file;
        lv_fs_res_t res = lv_fs_open(&file, fn, LV_FS_MODE_RD);
        if(res == LV_FS_RES_OK) {
            lv_img_raw_header_t header;
            uint32_t br;
            res = lv_fs_read(&file, &header, sizeof(lv_img_raw_header_t), &br);

            lv_area_t mask_com;    /*Common area of mask and cords*/
            bool union_ok;
            union_ok = lv_area_union(&mask_com, &mask_aa, &coord_aa);
            if(union_ok == false) {
                lv_fs_close(&file);
                return;
            }


            /*If the width is greater then real img. width then it is upscaled */
            bool upscale = false;
#if LV_ANTIALIAS
            if(lv_area_get_width(coords) < header.w) {
                upscale = false;
                lv_area_set_width(&coord_aa,  header.w);
            }
            else upscale = true;

#endif

            bool const_data = false;

            /*If the img. data is inside the MCU then do not use FS reading just a pointer*/
            if(fn[0] == UFS_LETTER) {
                const_data = true;
                uint8_t * f_data = ((lv_ufs_file_t*)file.file_d)->ent->data_d;
                f_data += sizeof(lv_img_raw_header_t);
                map_fp(&coord_aa, &mask_com, (void*)f_data , style->image.opa, header.transp, upscale, style->image.color, style->image.intense);
            }

            /*Read the img. with the FS interface*/
            if(const_data == false) {
                uint8_t us_shift = 0;
                uint8_t us_val = 1;
                if(upscale != false) {
                    us_shift = 1;
                    us_val = 2;
                }

                /* Move the file pointer to the start address according to mask
                 * But take care, the upscaled maps look greater*/
                uint32_t start_offset = sizeof(lv_img_raw_header_t);
                start_offset += (lv_area_get_width(&coord_aa) >> us_shift) *
                               ((mask_com.y1 - coord_aa.y1) >> us_shift) * sizeof(lv_color_t); /*First row*/
                start_offset += ((mask_com.x1 - coord_aa.x1) >> us_shift) * sizeof(lv_color_t); /*First col*/
                lv_fs_seek(&file, start_offset);

                uint32_t useful_data = (lv_area_get_width(&mask_com) >> us_shift) * sizeof(lv_color_t);
                uint32_t next_row = (lv_area_get_width(&coord_aa) >> us_shift) * sizeof(lv_color_t) - useful_data;

                lv_area_t line;
                lv_area_copy(&line, &mask_com);
                lv_area_set_height(&line, us_val); /*Create a line area. Hold 2 pixels if upscaled*/

                lv_coord_t row;
                uint32_t act_pos;
                lv_color_t buf[useful_data];
                for(row = mask_com.y1; row <= mask_com.y2; row += us_val) {
                    res = lv_fs_read(&file, buf, useful_data, &br);

                    map_fp(&line, &mask_com, buf, style->image.opa, header.transp, upscale,
                                          style->image.color, style->image.intense);

                    lv_fs_tell(&file, &act_pos);
                    lv_fs_seek(&file, act_pos + next_row);
                    line.y1 += us_val;    /*Go down a line*/
                    line.y2 += us_val;
                }
            }
        }

        lv_fs_close(&file);

        if(res != LV_FS_RES_OK) {
            lv_draw_rect(coords, mask, &lv_style_plain);
            lv_draw_label(coords, mask, &lv_style_plain, "No data", LV_TXT_FLAG_NONE, NULL);
        }
    }
}
#endif


/**
 * Draw a line
 * @param p1 first point of the line
 * @param p2 second point of the line
 * @param maskthe line will be drawn only on this area
 * @param lines_p pointer to a line style
 */
void lv_draw_line(const lv_point_t * p1, const lv_point_t * p2, const lv_area_t * mask,
                  const lv_style_t * style)
{

	if(style->line.width == 0) return;
	if(p1->x == p2->x && p1->y == p2->y) return;


	lv_point_t p1_aa;
    lv_point_t p2_aa;

    p1_aa.x = p1->x << LV_AA;
    p1_aa.y = p1->y << LV_AA;

    p2_aa.x = p2->x << LV_AA;
    p2_aa.y = p2->y << LV_AA;


    lv_area_t mask_aa;
#if LV_ANTIALIAS == 0
    lv_area_copy(&mask_aa, mask);
#else
    mask_aa.x1 = mask->x1 << LV_AA;
    mask_aa.y1 = mask->y1 << LV_AA;
    mask_aa.x2 = (mask->x2 << LV_AA) + 1;
    mask_aa.y2 = (mask->y2 << LV_AA) + 1;
#endif


	lv_coord_t dx = LV_MATH_ABS(p2_aa.x - p1_aa.x);
	lv_coord_t sx = p1_aa.x < p2_aa.x ? 1 : -1;
	lv_coord_t dy = LV_MATH_ABS(p2_aa.y - p1_aa.y);
	lv_coord_t sy = p1_aa.y < p2_aa.y ? 1 : -1;
	lv_coord_t err = (dx > dy ? dx : -dy) / 2;
	lv_coord_t e2;
	bool hor = dx > dy ? true : false;	/*Rather horizontal or vertical*/
	lv_coord_t last_x = p1_aa.x;
	lv_coord_t last_y = p1_aa.y;
	lv_point_t act_point;
	act_point.x = p1_aa.x;
	act_point.y = p1_aa.y;


	uint16_t width;
	uint16_t wcor;
	uint16_t width_half;
	uint16_t width_1;
    static const uint8_t width_corr_array[] = {
            64, 64, 64, 64, 64, 64, 64, 64, 64, 65, 65, 65, 65, 65, 66, 66, 66, 66, 66,
            67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74,
            74, 75, 75, 76, 77, 77, 78, 78, 79, 79, 80, 81, 81, 82, 82, 83, 84, 84, 85,
            86, 86, 87, 88, 88, 89, 90, 91,
    };

	if(hor == false) {
		wcor = (dx * LINE_WIDTH_CORR_BASE) / dy;
	} else  {
		wcor = (dy * LINE_WIDTH_CORR_BASE) / dx;
	}

    /*Make the correction on lie width*/
	width = ((style->line.width - 1) * width_corr_array[wcor]) >> LINE_WIDTH_CORR_SHIFT;
	width = width << LV_ANTIALIAS;
	width_half = width >> 1;
	width_1 = width & 0x1 ? 1 : 0;

	while(1){
	  if(hor == true && last_y != act_point.y) {
		  lv_area_t act_area;
		  lv_area_t draw_area;
		  act_area.x1 = last_x;
		  act_area.x2 = act_point.x - sx;
		  act_area.y1 = last_y - width_half ;
		  act_area.y2 = act_point.y - sy  + width_half + width_1;
		  last_y = act_point.y;
		  last_x = act_point.x;

		  draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
		  draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
		  draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
		  draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
		  fill_fp(&draw_area, &mask_aa, style->line.color, style->line.opa);
	  }
	  if (hor == false && last_x != act_point.x) {
		  lv_area_t act_area;
		  lv_area_t draw_area;
		  act_area.x1 = last_x - width_half;
		  act_area.x2 = act_point.x  - sx + width_half + width_1;
		  act_area.y1 = last_y ;
		  act_area.y2 = act_point.y - sy;
		  last_y = act_point.y;
		  last_x = act_point.x;

		  draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
		  draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
		  draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
		  draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
		  fill_fp(&draw_area, &mask_aa, style->line.color, style->line.opa);
	  }

		/*Calc. the next point of the line*/
		if (act_point.x == p2_aa.x && act_point.y == p2_aa.y) break;
		e2 = err;
		if (e2 >-dx) {
			err -= dy;
			act_point.x += sx;
		}
		if (e2 < dy) {
			err += dx;
			act_point.y += sy;
		}
	}
	/*Draw the last part of the line*/
	if(hor == true) {
		lv_area_t act_area;
		lv_area_t draw_area;
		act_area.x1 = last_x;
		act_area.x2 = act_point.x;
		act_area.y1 = last_y - width_half ;
		act_area.y2 = act_point.y + width_half + width_1;

		draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
		draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
		draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
		draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
		fill_fp(&draw_area, &mask_aa, style->line.color, style->line.opa);
	}
	if (hor == false) {
		lv_area_t act_area;
		lv_area_t draw_area;
		act_area.x1 = last_x - width_half;
		act_area.x2 = act_point.x + width_half + width_1;
		act_area.y1 = last_y;
		act_area.y2 = act_point.y;

		draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
		draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
		draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
		draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
		fill_fp(&draw_area, &mask_aa, style->line.color, style->line.opa);
	}
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Draw the middle part (rectangular) of a rectangle
 * @param coords the coordinates of the original rectangle
 * @param mask the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 */
static void lv_draw_rect_main_mid(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style)
{
    uint16_t radius = style->body.radius << LV_AA;

    lv_color_t mcolor = style->body.main_color;
    lv_color_t gcolor = style->body.grad_color;
    uint8_t mix;
    lv_opa_t opa = style->body.opa;
    lv_coord_t height = lv_area_get_height(coords);
    lv_coord_t width = lv_area_get_width(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    /*If the radius is too big then there is no body*/
    if(radius > height / 2)  return;

	lv_area_t work_area;
	work_area.x1 = coords->x1;
	work_area.x2 = coords->x2;

    if(mcolor.full == gcolor.full) {
    	work_area.y1 = coords->y1 + radius;
    	work_area.y2 = coords->y2 - radius;
		fill_fp(&work_area, mask, mcolor, opa);
    } else {

		lv_coord_t row;
		lv_coord_t row_start = coords->y1 + radius;
		lv_coord_t row_end = coords->y2 - radius;
		lv_color_t act_color;
        if(row_start < 0) row_start = 0;
		for(row = row_start ;
			row <= row_end;
			row ++)
		{
			work_area.y1 = row;
			work_area.y2 = row;
			mix = (uint32_t)((uint32_t)(coords->y2 - work_area.y1) * 255) / height;
			act_color = lv_color_mix(mcolor, gcolor, mix);

			fill_fp(&work_area, mask, act_color, opa);
		}
    }
}
/**
 * Draw the top and bottom parts (corners) of a rectangle
 * @param coords the coordinates of the original rectangle
 * @param mask the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 */
static void lv_draw_rect_main_corner(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style)
{
    uint16_t radius = style->body.radius << LV_AA;

    lv_color_t mcolor = style->body.main_color;
    lv_color_t gcolor = style->body.grad_color;
    lv_color_t act_color;
    lv_opa_t opa = style->body.opa;
    uint8_t mix;
    lv_coord_t height = lv_area_get_height(coords);
    lv_coord_t width = lv_area_get_width(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    lv_point_t lt_origo;   /*Left  Top    origo*/
    lv_point_t lb_origo;   /*Left  Bottom origo*/
    lv_point_t rt_origo;   /*Right Top    origo*/
    lv_point_t rb_origo;   /*Left  Bottom origo*/

    lt_origo.x = coords->x1 + radius;
    lt_origo.y = coords->y1 + radius;

    lb_origo.x = coords->x1 + radius;
    lb_origo.y = coords->y2 - radius;

    rt_origo.x = coords->x2 - radius;
    rt_origo.y = coords->y1 + radius;

    rb_origo.x = coords->x2 - radius;
    rb_origo.y = coords->y2 - radius;


    lv_area_t edge_top_area;
    lv_area_t mid_top_area;
    lv_area_t mid_bot_area;
    lv_area_t edge_bot_area;


    lv_point_t cir;
    lv_coord_t cir_tmp;
    lv_circ_init(&cir, &cir_tmp, radius);

    /*Init the areas*/
    lv_area_set(&mid_bot_area,  lb_origo.x + LV_CIRC_OCT4_X(cir),
                             lb_origo.y + LV_CIRC_OCT4_Y(cir),
                             rb_origo.x + LV_CIRC_OCT1_X(cir),
                             rb_origo.y + LV_CIRC_OCT1_Y(cir));

    lv_area_set(&edge_bot_area, lb_origo.x + LV_CIRC_OCT3_X(cir),
                             lb_origo.y + LV_CIRC_OCT3_Y(cir),
                             rb_origo.x + LV_CIRC_OCT2_X(cir),
                             rb_origo.y + LV_CIRC_OCT2_Y(cir));

    lv_area_set(&mid_top_area,  lt_origo.x + LV_CIRC_OCT5_X(cir),
                             lt_origo.y + LV_CIRC_OCT5_Y(cir),
                             rt_origo.x + LV_CIRC_OCT8_X(cir),
                             rt_origo.y + LV_CIRC_OCT8_Y(cir));

    lv_area_set(&edge_top_area, lt_origo.x + LV_CIRC_OCT6_X(cir),
                             lt_origo.y + LV_CIRC_OCT6_Y(cir),
                             rt_origo.x + LV_CIRC_OCT7_X(cir),
                             rt_origo.y + LV_CIRC_OCT7_Y(cir));

    while(lv_circ_cont(&cir)) {
        uint8_t edge_top_refr = 0;
        uint8_t mid_top_refr = 0;
        uint8_t mid_bot_refr = 0;
        uint8_t edge_bot_refr = 0;

        /*If a new row coming draw the previous
         * The x coordinate can grow on the same y so wait for the last x*/
        if(mid_bot_area.y1 != LV_CIRC_OCT4_Y(cir) + lb_origo.y ) {
            mid_bot_refr = 1;
        }

        if(edge_bot_area.y1 != LV_CIRC_OCT2_Y(cir) + lb_origo.y) {
            edge_bot_refr = 1;
        }

        if(mid_top_area.y1 != LV_CIRC_OCT8_Y(cir) + lt_origo.y) {
            mid_top_refr = 1;
        }

        if(edge_top_area.y1 != LV_CIRC_OCT7_Y(cir) + lt_origo.y) {
            edge_top_refr = 1;
        }

        /* Do not refresh the first row in the middle
         * because the body drawer makes it*/
        if(mid_bot_area.y1 == coords->y2 - radius){
            mid_bot_refr = 0;
        }

        if(mid_top_area.y1 == coords->y1 + radius){
            mid_top_refr = 0;
        }

        /*Draw the areas which are not disabled*/
        if(edge_top_refr != 0){
            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - edge_top_area.y1)  * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&edge_top_area, mask, act_color, opa);
        }

        if(mid_top_refr != 0) {
            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - mid_top_area.y1) * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&mid_top_area, mask, act_color, opa);
        }

        if(mid_bot_refr != 0) {
            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - mid_bot_area.y1) * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&mid_bot_area, mask, act_color, opa);
        }

        if(edge_bot_refr != 0) {

            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - edge_bot_area.y1) * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&edge_bot_area, mask, act_color, opa);
        }
         /*Save the current coordinates*/
        lv_area_set(&mid_bot_area,  lb_origo.x + LV_CIRC_OCT4_X(cir),
                                 lb_origo.y + LV_CIRC_OCT4_Y(cir),
                                 rb_origo.x + LV_CIRC_OCT1_X(cir),
                                 rb_origo.y + LV_CIRC_OCT1_Y(cir));

        lv_area_set(&edge_bot_area, lb_origo.x + LV_CIRC_OCT3_X(cir),
                                 lb_origo.y + LV_CIRC_OCT3_Y(cir),
                                 rb_origo.x + LV_CIRC_OCT2_X(cir),
                                 rb_origo.y + LV_CIRC_OCT2_Y(cir));

        lv_area_set(&mid_top_area,  lt_origo.x + LV_CIRC_OCT5_X(cir),
                                 lt_origo.y + LV_CIRC_OCT5_Y(cir),
                                 rt_origo.x + LV_CIRC_OCT8_X(cir),
                                 rt_origo.y + LV_CIRC_OCT8_Y(cir));

        lv_area_set(&edge_top_area, lt_origo.x + LV_CIRC_OCT6_X(cir),
                                 lt_origo.y + LV_CIRC_OCT6_Y(cir),
                                 rt_origo.x + LV_CIRC_OCT7_X(cir),
                                 rt_origo.y + LV_CIRC_OCT7_Y(cir));

        lv_circ_next(&cir, &cir_tmp);
    }


    if(mcolor.full == gcolor.full) act_color = mcolor;
    else {
        mix = (uint32_t)((uint32_t)(coords->y2 - edge_top_area.y1)  * 255) / height;
        act_color = lv_color_mix(mcolor, gcolor, mix);
    }
	fill_fp(&edge_top_area, mask, act_color, opa);

	if(edge_top_area.y1 != mid_top_area.y1) {

        if(mcolor.full == gcolor.full) act_color = mcolor;
        else {
            mix = (uint32_t)((uint32_t)(coords->y2 - mid_top_area.y1) * 255) / height;
            act_color = lv_color_mix(mcolor, gcolor, mix);
	    }
		fill_fp(&mid_top_area, mask, act_color, opa);
	}

    if(mcolor.full == gcolor.full) act_color = mcolor;
    else {
        mix = (uint32_t)((uint32_t)(coords->y2 - mid_bot_area.y1) * 255) / height;
        act_color = lv_color_mix(mcolor, gcolor, mix);
    }
	fill_fp(&mid_bot_area, mask, act_color, opa);

	if(edge_bot_area.y1 != mid_bot_area.y1) {

        if(mcolor.full == gcolor.full) act_color = mcolor;
        else {
            mix = (uint32_t)((uint32_t)(coords->y2 - edge_bot_area.y1) * 255) / height;
            act_color = lv_color_mix(mcolor, gcolor, mix);
        }
		fill_fp(&edge_bot_area, mask, act_color, opa);
	}

}

/**
 * Draw the straight parts of a rectangle border
 * @param coords the coordinates of the original rectangle
 * @param mask_ the rectangle will be drawn only  on this area
 * @param rstyle pointer to a rectangle style
 */
static void lv_draw_rect_border_straight(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style)
{
    uint16_t radius = style->body.radius << LV_AA;

    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);
    uint16_t bwidth = style->body.border.width << LV_AA;
    lv_opa_t opa = style->body.border.opa;
    lv_border_part_t part = style->body.border.part;
    lv_color_t color = style->body.border.color;
    lv_area_t work_area;
    lv_coord_t length_corr = 0;
    lv_coord_t corner_size = 0;

    /*the 0 px border width drawn as 1 px, so decrement the b_width*/
    bwidth--;

    radius = lv_draw_cont_radius_corr(radius, width, height);

    if(radius < bwidth) {
        length_corr = bwidth - radius;
        corner_size = bwidth;
    } else {
        corner_size = radius;
    }

    /* Modify the corner_size if corner is drawn */
    corner_size ++;


    /*Depending one which part's are drawn modify the area lengths */
    if(part & LV_BORDER_TOP) work_area.y1 = coords->y1 + corner_size;
    else  work_area.y1 = coords->y1 + radius;

    if(part & LV_BORDER_BOTTOM) work_area.y2 = coords->y2 - corner_size;
    else work_area.y2 = coords->y2 - radius;

    /*Left border*/
    if(part & LV_BORDER_LEFT) {
        work_area.x1 = coords->x1;
        work_area.x2 = work_area.x1 + bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    /*Right border*/
    if(part & LV_BORDER_RIGHT) {
        work_area.x2 = coords->x2;
        work_area.x1 = work_area.x2 - bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    work_area.x1 = coords->x1 + corner_size - length_corr;
    work_area.x2 = coords->x2 - corner_size + length_corr;

    /*Upper border*/
    if(part & LV_BORDER_TOP) {
        work_area.y1 = coords->y1;
        work_area.y2 = coords->y1 + bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    /*Lower border*/
    if(part & LV_BORDER_BOTTOM) {
        work_area.y2 = coords->y2;
        work_area.y1 = work_area.y2 - bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    /*Draw the a remaining rectangles if the radius is smaller then b_width */
    if(length_corr != 0) {
        /*Left top correction*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1 + radius;
            work_area.y1 = coords->y1 + radius + 1;
            work_area.y2 = coords->y1 + bwidth;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right top correction*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
            work_area.x1 = coords->x2 - radius;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y1 + radius + 1;
            work_area.y2 = coords->y1 + bwidth;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Left bottom correction*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1 + radius;
            work_area.y1 = coords->y2 - bwidth;
            work_area.y2 = coords->y2 - radius - 1;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right bottom correction*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
            work_area.x1 = coords->x2 - radius;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y2 - bwidth;
            work_area.y2 = coords->y2 - radius - 1;
            fill_fp(&work_area, mask, color, opa);
        }
    }

    /*If radius == 0 one px on the corners are not drawn*/
    if(radius == 0) {

        /*Left top corner*/
        if(part & (LV_BORDER_TOP | LV_BORDER_LEFT)) {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1;
            work_area.y1 = coords->y1;
            work_area.y2 = coords->y1;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right top corner*/
        if(part & (LV_BORDER_TOP | LV_BORDER_RIGHT)) {
            work_area.x1 = coords->x2;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y1;
            work_area.y2 = coords->y1;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Left bottom corner*/
        if(part & (LV_BORDER_BOTTOM | LV_BORDER_LEFT)) {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1;
            work_area.y1 = coords->y2;
            work_area.y2 = coords->y2;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right bottom corner*/
        if(part & (LV_BORDER_BOTTOM | LV_BORDER_RIGHT)) {
            work_area.x1 = coords->x2;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y2;
            work_area.y2 = coords->y2;
            fill_fp(&work_area, mask, color, opa);
        }
    }
}


/**
 * Draw the corners of a rectangle border
 * @param coords the coordinates of the original rectangle
 * @param mask the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 * @param opa opacity of the rectangle (0..255)
 */
static void lv_draw_rect_border_corner(const lv_area_t * coords, const lv_area_t * mask, const  lv_style_t * style)
{
    uint16_t radius = style->body.radius << LV_AA;
    uint16_t bwidth = style->body.border.width << LV_AA;
    lv_color_t color = style->body.border.color;
    lv_opa_t opa = style->body.border.opa;
    lv_border_part_t part = style->body.border.part;

    /*0 px border width drawn as 1 px, so decrement the bwidth*/
    bwidth--;

    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    lv_point_t lt_origo;   /*Left  Top    origo*/
    lv_point_t lb_origo;   /*Left  Bottom origo*/
    lv_point_t rt_origo;   /*Right Top    origo*/
    lv_point_t rb_origo;   /*Left  Bottom origo*/

    lt_origo.x = coords->x1 + radius;
    lt_origo.y = coords->y1 + radius;

    lb_origo.x = coords->x1 + radius;
    lb_origo.y = coords->y2 - radius;

    rt_origo.x = coords->x2 - radius;
    rt_origo.y = coords->y1 + radius;

    rb_origo.x = coords->x2 - radius;
    rb_origo.y = coords->y2 - radius;

    lv_point_t cir_out;
    lv_coord_t tmp_out;
    lv_circ_init(&cir_out, &tmp_out, radius);

    lv_point_t cir_in;
    lv_coord_t tmp_in;
    lv_coord_t radius_in = radius - bwidth;

    if(radius_in < 0){
        radius_in = 0;
    }

    lv_circ_init(&cir_in, &tmp_in, radius_in);

    lv_area_t circ_area;
    lv_coord_t act_w1;
    lv_coord_t act_w2;

    while( cir_out.y <= cir_out.x) {

        /*Calculate the actual width to avoid overwriting pixels*/
        if(cir_in.y < cir_in.x) {
            act_w1 = cir_out.x - cir_in.x;
            act_w2 = act_w1;
        } else {
            act_w1 = cir_out.x - cir_out.y;
            act_w2 = act_w1 - 1;
        }

        /*Draw the octets to the right bottom corner*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
            circ_area.x1 = rb_origo.x + LV_CIRC_OCT1_X(cir_out) - act_w2;
            circ_area.x2 = rb_origo.x + LV_CIRC_OCT1_X(cir_out);
            circ_area.y1 = rb_origo.y + LV_CIRC_OCT1_Y(cir_out);
            circ_area.y2 = rb_origo.y + LV_CIRC_OCT1_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);

            circ_area.x1 = rb_origo.x + LV_CIRC_OCT2_X(cir_out);
            circ_area.x2 = rb_origo.x + LV_CIRC_OCT2_X(cir_out);
            circ_area.y1 = rb_origo.y + LV_CIRC_OCT2_Y(cir_out)- act_w1;
            circ_area.y2 = rb_origo.y + LV_CIRC_OCT2_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);
        }

        /*Draw the octets to the left bottom corner*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
            circ_area.x1 = lb_origo.x + LV_CIRC_OCT3_X(cir_out);
            circ_area.x2 = lb_origo.x + LV_CIRC_OCT3_X(cir_out);
            circ_area.y1 = lb_origo.y + LV_CIRC_OCT3_Y(cir_out) - act_w2;
            circ_area.y2 = lb_origo.y + LV_CIRC_OCT3_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);

            circ_area.x1 = lb_origo.x + LV_CIRC_OCT4_X(cir_out);
            circ_area.x2 = lb_origo.x + LV_CIRC_OCT4_X(cir_out) + act_w1;
            circ_area.y1 = lb_origo.y + LV_CIRC_OCT4_Y(cir_out);
            circ_area.y2 = lb_origo.y + LV_CIRC_OCT4_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);
        }
        
        /*Draw the octets to the left top corner*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
            if(lb_origo.y + LV_CIRC_OCT4_Y(cir_out) > lt_origo.y + LV_CIRC_OCT5_Y(cir_out)) {
                /*Don't draw if the lines are common in the middle*/
                circ_area.x1 = lt_origo.x + LV_CIRC_OCT5_X(cir_out);
                circ_area.x2 = lt_origo.x + LV_CIRC_OCT5_X(cir_out) + act_w2;
                circ_area.y1 = lt_origo.y + LV_CIRC_OCT5_Y(cir_out);
                circ_area.y2 = lt_origo.y + LV_CIRC_OCT5_Y(cir_out);
                fill_fp(&circ_area, mask, color, opa);
            }

            circ_area.x1 = lt_origo.x + LV_CIRC_OCT6_X(cir_out);
            circ_area.x2 = lt_origo.x + LV_CIRC_OCT6_X(cir_out);
            circ_area.y1 = lt_origo.y + LV_CIRC_OCT6_Y(cir_out);
            circ_area.y2 = lt_origo.y + LV_CIRC_OCT6_Y(cir_out) + act_w1;
            fill_fp(&circ_area, mask, color, opa);
        }
        
        /*Draw the octets to the right top corner*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
            circ_area.x1 = rt_origo.x + LV_CIRC_OCT7_X(cir_out);
            circ_area.x2 = rt_origo.x + LV_CIRC_OCT7_X(cir_out);
            circ_area.y1 = rt_origo.y + LV_CIRC_OCT7_Y(cir_out);
            circ_area.y2 = rt_origo.y + LV_CIRC_OCT7_Y(cir_out) + act_w2;
            fill_fp(&circ_area, mask, color, opa);

            /*Don't draw if the lines are common in the middle*/
            if(rb_origo.y + LV_CIRC_OCT1_Y(cir_out) > rt_origo.y + LV_CIRC_OCT8_Y(cir_out)) {
                circ_area.x1 = rt_origo.x + LV_CIRC_OCT8_X(cir_out) - act_w1;
                circ_area.x2 = rt_origo.x + LV_CIRC_OCT8_X(cir_out);
                circ_area.y1 = rt_origo.y + LV_CIRC_OCT8_Y(cir_out);
                circ_area.y2 = rt_origo.y + LV_CIRC_OCT8_Y(cir_out);
                fill_fp(&circ_area, mask, color, opa);
            }
        }
        lv_circ_next(&cir_out, &tmp_out);

        /*The internal circle will be ready faster
         * so check it! */
        if(cir_in.y < cir_in.x) {
            lv_circ_next(&cir_in, &tmp_in);
        }
    }
}

#if USE_LV_SHADOW && LV_VDB_SIZE

/**
 * Draw a shadow
 * @param rect pointer to rectangle object
 * @param mask pointer to a mask area (from the design functions)
 */
static void lv_draw_rect_shadow(const lv_area_t * coords, const lv_area_t * mask, const  lv_style_t * style)
{
    /* If mask is in the middle of cords do not draw shadow*/
    lv_coord_t radius = style->body.radius << LV_AA;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);
    radius = lv_draw_cont_radius_corr(radius, width, height);
    lv_area_t area_tmp;

    /*Check horizontally without radius*/
    lv_area_copy(&area_tmp, coords);
    area_tmp.x1 += radius;
    area_tmp.x2 -= radius;
    if(lv_area_is_in(mask, &area_tmp) != false) return;

    /*Check vertically without radius*/
    lv_area_copy(&area_tmp, coords);
    area_tmp.y1 += radius;
    area_tmp.y2 -= radius;
    if(lv_area_is_in(mask, &area_tmp) != false) return;

    if(style->body.shadow.type == LV_SHADOW_FULL) {
        lv_draw_cont_shadow_full(coords, mask, style);
    } else if(style->body.shadow.type == LV_SHADOW_BOTTOM) {
        lv_draw_cont_shadow_bottom(coords, mask, style);
    }
}

static void lv_draw_cont_shadow_full(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style)
{

    lv_coord_t radius = style->body.radius << LV_AA;
    lv_coord_t swidth = style->body.shadow.width << LV_AA;

    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    lv_coord_t cruve_x[radius + swidth];     /*Stores the 'x' coordinates of a quarter circle.*/
    memset(cruve_x, 0, sizeof(cruve_x));
    lv_point_t circ;
    lv_coord_t circ_tmp;
    lv_circ_init(&circ, &circ_tmp, radius);
    while(lv_circ_cont(&circ)) {
        cruve_x[LV_CIRC_OCT1_Y(circ)] = LV_CIRC_OCT1_X(circ);
        cruve_x[LV_CIRC_OCT2_Y(circ)] = LV_CIRC_OCT2_X(circ);
        lv_circ_next(&circ, &circ_tmp);
    }
    int16_t row;

    int16_t filter_size = 2 * swidth + 1;
    uint16_t opa_h_result[filter_size];

    for(row = 0; row < filter_size; row++) {
        opa_h_result[row] = (uint32_t)((uint32_t)(filter_size - row) * style->body.opa * 2) / (filter_size);
    }

    uint16_t p;
    lv_opa_t opa_v_result[radius + swidth];

    lv_point_t point_rt;
    lv_point_t point_rb;
    lv_point_t point_lt;
    lv_point_t point_lb;
    lv_point_t ofs_rb;
    lv_point_t ofs_rt;
    lv_point_t ofs_lb;
    lv_point_t ofs_lt;
    ofs_rb.x = coords->x2 - radius;
    ofs_rb.y = coords->y2 - radius;

    ofs_rt.x = coords->x2 - radius;
    ofs_rt.y = coords->y1 + radius;

    ofs_lb.x = coords->x1 + radius;
    ofs_lb.y = coords->y2 - radius;

    ofs_lt.x = coords->x1 + radius;
    ofs_lt.y = coords->y1 + radius;

    for(row = 0; row < radius + swidth; row++) {
        for(p = 0; p < radius + swidth; p++) {
           int16_t v;
           uint32_t opa_tmp = 0;
           int16_t row_v;
           bool swidth_out = false;
           for(v = -swidth; v < swidth; v++) {
               row_v = row + v;
               if(row_v < 0) row_v = 0; /*Rows above the corner*/

               /*Rows below the bottom are empty so they won't modify the filter*/
               if(row_v > radius) {
                   break;
               }
               else
               {
                   int16_t p_tmp = p - (cruve_x[row_v] - cruve_x[row]);
                   if(p_tmp < -swidth) { /*Cols before the filtered shadow (still not blurred)*/
                       opa_tmp += style->body.opa * 2;
                   }
                   /*Cols after the filtered shadow (already no effect) */
                   else if (p_tmp > swidth) {
                       /* If on the current point the  filter top point is already out of swidth then
                        * the remaining part will not do not anything on this point*/
                       if(v == -swidth) { /*Is the first point?*/
                           swidth_out = true;
                       }
                       break;
                   } else {
                       opa_tmp += opa_h_result[p_tmp + swidth];
                   }
               }
           }
           if(swidth_out == false) {
               opa_tmp = opa_tmp / (filter_size);
               opa_v_result[p] = opa_tmp > LV_OPA_COVER ? LV_OPA_COVER : opa_tmp;
           }
           else {
               break;
           }
        }

        point_rt.x = cruve_x[row] + ofs_rt.x;
        point_rt.y = ofs_rt.y - row;

        point_rb.x = cruve_x[row] + ofs_rb.x;
        point_rb.y = ofs_rb.y + row;

        point_lt.x = ofs_lt.x -  cruve_x[row];
        point_lt.y = ofs_lt.y - row;

        point_lb.x = ofs_lb.x - cruve_x[row];
        point_lb.y = ofs_lb.y + row;

        uint16_t d;
        for(d = 0; d < p; d++) {

            if(point_rt.x != point_lt.x) {
                px_fp(point_lt.x,point_lt.y , mask, style->body.shadow.color, opa_v_result[d]);
            }

            if(point_rb.x != point_lb.x && point_lt.y != point_lb.y) {
                px_fp(point_lb.x,point_lb.y , mask, style->body.shadow.color, opa_v_result[d]);
            }

            if(point_lt.y != point_lb.y) {
                px_fp(point_rb.x,point_rb.y , mask, style->body.shadow.color, opa_v_result[d]);
            }


            px_fp(point_rt.x,point_rt.y , mask, style->body.shadow.color, opa_v_result[d]);


            point_rb.x++;
            point_lb.x--;

            point_rt.x++;
            point_lt.x--;
        }

        /*When the first row is known draw the straight pars with same opa. map*/
        if(row == 0) {
           lv_draw_cont_shadow_full_straight(coords, mask, style, opa_v_result);
        }
    }
}


static void lv_draw_cont_shadow_bottom(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style)
{
    lv_coord_t radius = style->body.radius << LV_AA;
    lv_coord_t swidth = style->body.shadow.width << LV_AA;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    lv_coord_t cruve_x[radius + swidth];     /*Stores the 'x' coordinates of a quarter circle.*/
    memset(cruve_x, 0, sizeof(cruve_x));
    lv_point_t circ;
    lv_coord_t circ_tmp;
    lv_circ_init(&circ, &circ_tmp, radius);
    while(lv_circ_cont(&circ)) {
        cruve_x[LV_CIRC_OCT1_Y(circ)] = LV_CIRC_OCT1_X(circ);
        cruve_x[LV_CIRC_OCT2_Y(circ)] = LV_CIRC_OCT2_X(circ);
        lv_circ_next(&circ, &circ_tmp);
    }
    int16_t row;

    int16_t filter_size = 2 * swidth + 1;
    lv_opa_t opa_h_result[filter_size];

    for(row = 0; row < filter_size; row++) {
        opa_h_result[row] = (uint32_t)((uint32_t)(filter_size - row) * style->body.opa) / (filter_size);
    }

    lv_point_t point_l;
    lv_point_t point_r;
    lv_area_t area_mid;
    lv_point_t ofs1;
    lv_point_t ofs2;

    ofs1.x = coords->x1 + radius;
    ofs1.y = coords->y2 - radius;

    ofs2.x = coords->x2 - radius;
    ofs2.y = coords->y2 - radius;

    for(row = 0; row < radius; row++) {
        point_l.x = ofs1.x + radius - row - radius;
        point_l.y = ofs1.y + cruve_x[row];

        point_r.x = ofs2.x + row;
        point_r.y = ofs2.y + cruve_x[row];

        uint16_t d;
        for(d = swidth; d < filter_size; d++) {
            px_fp(point_l.x, point_l.y, mask, style->body.shadow.color, opa_h_result[d]);
            point_l.y ++;

            px_fp(point_r.x, point_r.y, mask, style->body.shadow.color, opa_h_result[d]);
            point_r.y ++;
        }

    }

    area_mid.x1 = ofs1.x + 1;
    area_mid.y1 = ofs1.y + radius;
    area_mid.x2 = ofs2.x - 1;
    area_mid.y2 = area_mid.y1;

    uint16_t d;
    for(d = swidth; d < filter_size; d++) {
        fill_fp(&area_mid, mask, style->body.shadow.color, opa_h_result[d]);
        area_mid.y1 ++;
        area_mid.y2 ++;
    }
}

static void lv_draw_cont_shadow_full_straight(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, const lv_opa_t * map)
{

    lv_coord_t radius = style->body.radius << LV_AA;
    lv_coord_t swidth = style->body.shadow.width << LV_AA;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    lv_area_t sider_area;
    sider_area.x1 = coords->x2;
    sider_area.y1 = coords->y1 + radius + 1;
    sider_area.x2 = sider_area.x1;
    sider_area.y2 = coords->y2 -  radius - 1;

    lv_area_t sidel_area;
    sidel_area.x1 = coords->x1;
    sidel_area.y1 = coords->y1 + radius + 1;
    sidel_area.x2 = sidel_area.x1;
    sidel_area.y2 = coords->y2 - radius - 1;

    lv_area_t sidet_area;
    sidet_area.x1 = coords->x1 + radius + 1;
    sidet_area.y1 = coords->y1;
    sidet_area.x2 = coords->x2 - radius - 1;
    sidet_area.y2 = sidet_area.y1;

    lv_area_t sideb_area;
    sideb_area.x1 = coords->x1 + radius + 1;
    sideb_area.y1 = coords->y2;
    sideb_area.x2 = coords->x2 - radius - 1;
    sideb_area.y2 = sideb_area.y1;

    int16_t d;
    for(d = 0; d < swidth; d++) {
        fill_fp(&sider_area, mask, style->body.shadow.color, map[d]);
        sider_area.x1++;
        sider_area.x2++;

        fill_fp(&sidel_area, mask, style->body.shadow.color, map[d]);
        sidel_area.x1--;
        sidel_area.x2--;

        fill_fp(&sidet_area, mask, style->body.shadow.color, map[d]);
        sidet_area.y1--;
        sidet_area.y2--;

        fill_fp(&sideb_area, mask, style->body.shadow.color, map[d]);
        sideb_area.y1++;
        sideb_area.y2++;
    }

}

#endif

static uint16_t lv_draw_cont_radius_corr(uint16_t r, lv_coord_t w, lv_coord_t h)
{
	if(r >= (w >> 1)){
		r = (w >> 1);
		if(r != 0) r--;
	}
	if(r >= (h >> 1)) {
		r = (h >> 1);
		if(r != 0) r--;
	}

	return r;
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
