/**
 * @file lv_draw.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"

#include <stdio.h>
#include <stdbool.h>
#include "lv_draw.h"
#include "lv_draw_rbasic.h"
#include "lv_draw_vbasic.h"
#include "../misc/gfx/text.h"
#include "../misc/gfx/circ.h"
#include "../misc/fs/fsint.h"
#include "../misc/math/math_base.h"
#include "../misc/fs/ufs/ufs.h"
#include "../lv_objx/lv_img.h"

/*********************
 *      DEFINES
 *********************/
#define LINE_WIDTH_CORR_BASE 64
#define LINE_WIDTH_CORR_SHIFT 6

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
static void lv_draw_rect_main_mid(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style);
static void lv_draw_rect_main_corner(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style_p);
static void lv_draw_rect_border_straight(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style_p);
static void lv_draw_rect_border_corner(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style);
static void lv_draw_rect_shadow(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style);
static void lv_draw_cont_shadow_full(const area_t * cords_p, const area_t * mask_p, const  lv_style_t * style);
static void lv_draw_cont_shadow_bottom(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style);
static void lv_draw_cont_shadow_full_straight(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style, const opa_t * map);

static uint16_t lv_draw_cont_radius_corr(uint16_t r, cord_t w, cord_t h);


#if USE_LV_TRIANGLE != 0
static void point_swap(point_t * p1, point_t * p2);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_VDB_SIZE != 0
static void (*px_fp)(cord_t x, cord_t y, const area_t * mask_p, color_t color, opa_t opa) = lv_vpx;
static void (*fill_fp)(const area_t * cords_p, const area_t * mask_p, color_t color, opa_t opa) =  lv_vfill;
static void (*letter_fp)(const point_t * pos_p, const area_t * mask_p, const font_t * font_p, uint8_t letter, color_t color, opa_t opa) = lv_vletter;
#if USE_FSINT != 0
static void (*map_fp)(const area_t * cords_p, const area_t * mask_p, const color_t * map_p, opa_t opa, bool transp, bool upscale, color_t recolor, opa_t recolor_opa) = lv_vmap;
#endif
#else
static void (*px_fp)(cord_t x, cord_t y, const area_t * mask_p, color_t color, opa_t opa) = lv_rpx;
static void (*fill_fp)(const area_t * cords_p, const area_t * mask_p, color_t color, opa_t opa) =  lv_rfill;
static void (*letter_fp)(const point_t * pos_p, const area_t * mask_p, const font_t * font_p, uint8_t letter, color_t color, opa_t opa) = lv_rletter;
#if USE_LV_IMG != 0 && USE_FSINT != 0
static void (*map_fp)(const area_t * cords_p, const area_t * mask_p, const color_t * map_p, opa_t opa, bool transp, bool upscale, color_t recolor, opa_t recolor_opa) = lv_rmap;
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
 * @param cords_p the coordinates of the rectangle
 * @param mask_p the rectangle will be drawn only in this mask
 * @param style_p pointer to a style
 */
void lv_draw_rect(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style_p)
{
    if(area_get_height(cords_p) < 1 || area_get_width(cords_p) < 1) return;

    if(style_p->swidth != 0) {
        lv_draw_rect_shadow(cords_p, mask_p, style_p);
    }

    if(style_p->empty == 0){
        lv_draw_rect_main_mid(cords_p, mask_p, style_p);

        if(style_p->radius != 0) {
            lv_draw_rect_main_corner(cords_p, mask_p, style_p);
        }
    } 
    
    if(style_p->bwidth != 0) {
        lv_draw_rect_border_straight(cords_p, mask_p, style_p);

        if(style_p->radius != 0) {
            lv_draw_rect_border_corner(cords_p, mask_p, style_p);
        }
    }
}

#if USE_LV_TRIANGE != 0
/**
 *
 * @param points pointer to an array with 3 points
 * @param mask_p the triangle will be drawn only in this mask
 * @param color color of the triangle
 */
void lv_draw_triangle(const point_t * points, const area_t * mask_p, color_t color)
{
    point_t tri[3];

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
    point_t edge1;
    cord_t dx1 = MATH_ABS(tri[0].x - tri[1].x);
    cord_t sx1 = tri[0].x < tri[1].x ? 1 : -1;
    cord_t dy1 = MATH_ABS(tri[0].y - tri[1].y);
    cord_t sy1 = tri[0].y < tri[1].y ? 1 : -1;
    cord_t err1 = (dx1 > dy1 ? dx1 : -dy1) / 2;
    cord_t err_tmp1;

    point_t edge2;
    cord_t dx2 = MATH_ABS(tri[0].x - tri[2].x);
    cord_t sx2 = tri[0].x < tri[2].x ? 1 : -1;
    cord_t dy2 = MATH_ABS(tri[0].y - tri[2].y);
    cord_t sy2 = tri[0].y < tri[2].y ? 1 : -1;
    cord_t err2 = (dx1 > dy2 ? dx2 : -dy2) / 2;
    cord_t err_tmp2;

    cord_t y1_tmp;
    cord_t y2_tmp;

    edge1.x = tri[0].x;
    edge1.y = tri[0].y;
    edge2.x = tri[0].x;
    edge2.y = tri[0].y;
    area_t act_area;
    area_t draw_area;

    while(1) {
        act_area.x1 = edge1.x;
        act_area.x2 = edge2.x ;
        act_area.y1 = edge1.y;
        act_area.y2 = edge2.y ;


        draw_area.x1 = MATH_MIN(act_area.x1, act_area.x2);
        draw_area.x2 = MATH_MAX(act_area.x1, act_area.x2);
        draw_area.y1 = MATH_MIN(act_area.y1, act_area.y2);
        draw_area.y2 = MATH_MAX(act_area.y1, act_area.y2);
        draw_area.x2--; /*Do not draw most right pixel because it will be drawn by the adjacent triangle*/
        fill_fp(&draw_area, mask_p, color, OPA_50);

        /*Calc. the next point of edge1*/
        y1_tmp = edge1.y;
        do {
            if (edge1.x == tri[1].x && edge1.y == tri[1].y) {

                dx1 = MATH_ABS(tri[1].x - tri[2].x);
                sx1 = tri[1].x < tri[2].x ? 1 : -1;
                dy1 = MATH_ABS(tri[1].y - tri[2].y);
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
 * @param cords_p coordinates of the label
 * @param mask_p the label will be drawn only in this area
 * @param style pointer to a style
 * @param txt 0 terminated text to write
 * @param flag settings for the text from 'txt_flag_t' enum
 * @param offset text offset in x and y direction (NULL if unused)
 *
 */
void lv_draw_label(const area_t * cords_p,const area_t * mask_p, const lv_style_t * style,
                    const char * txt, txt_flag_t flag, point_t * offset)
{
    const font_t * font = style->font;
    cord_t w;

    if((flag & TXT_FLAG_EXPAND) == 0) {
        w = area_get_width(cords_p);
    } else {
        point_t p;
        txt_get_size(&p, txt, style->font, style->letter_space, style->line_space, CORD_MAX, flag);
        w = p.x;
    }
    /*Init variables for the first line*/
    cord_t line_length = 0;
    uint32_t line_start = 0;
    uint32_t line_end = txt_get_next_line(txt, font, style->letter_space, w, flag);

    point_t pos;
    pos.x = cords_p->x1;
    pos.y = cords_p->y1;

    /*Align the line to middle if enabled*/
    if(style->txt_align  == LV_TXT_ALIGN_MID) {
        line_length = txt_get_width(&txt[line_start], line_end - line_start,
                                    font, style->letter_space, flag);
        pos.x += (w - line_length) / 2;
    }

    cmd_state_t cmd_state = CMD_STATE_WAIT;
    uint32_t i;
    uint16_t par_start;
    color_t recolor;

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

        for(i = line_start; i < line_end; i++) {
            /*Handle the re-color command*/
            if((flag & TXT_FLAG_RECOLOR) != 0) {
                if(txt[i] == TXT_RECOLOR_CMD) {
                    if(cmd_state == CMD_STATE_WAIT) { /*Start char*/
                        par_start = i + 1;
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
                    if(txt[i] == ' ') {
                        /*Get the parameter*/
                        if(i - par_start == LABEL_RECOLOR_PAR_LENGTH) {
                            char buf[LABEL_RECOLOR_PAR_LENGTH + 1];
                            memcpy(buf, &txt[par_start], LABEL_RECOLOR_PAR_LENGTH);
                            buf[LABEL_RECOLOR_PAR_LENGTH] = '\0';
                            int r,g,b;
                            sscanf(buf, "%02x%02x%02x", &r, &g, &b);
                            recolor = COLOR_MAKE(r, g, b);
                        } else {
                            recolor.full = style->ccolor.full;
                        }
                        cmd_state = CMD_STATE_IN; /*After the parameter the text is in the command*/
                    }
                    continue;
                }
            }

            color_t color = style->ccolor;

            if(cmd_state == CMD_STATE_IN) color = recolor;
            letter_fp(&pos, mask_p, font, txt[i], color, style->opa);

            pos.x += (font_get_width(font, txt[i]) >> FONT_ANTIALIAS) + style->letter_space;

        }
        /*Go to next line*/
        line_start = line_end;
        line_end += txt_get_next_line(&txt[line_start], font, style->letter_space, w, flag);

        pos.x = cords_p->x1;
        /*Align to middle*/
        if(style->txt_align == LV_TXT_ALIGN_MID) {
            line_length = txt_get_width(&txt[line_start], line_end - line_start,
                                     font, style->letter_space, flag);
            pos.x += (w - line_length) / 2;
        }
        /*Go the next line position*/
        pos.y += font_get_height(font) >> FONT_ANTIALIAS;
        pos.y += style->line_space;
    }
}

#if USE_FSINT != 0
/**
 * Draw an image
 * @param cords_p the coordinates of the image
 * @param mask_p the image will be drawn only in this area
 * @param map_p pointer to a color_t array which contains the pixels of the image
 * @param opa opacity of the image (0..255)
 */
void lv_draw_img(const area_t * cords_p, const area_t * mask_p, 
             const lv_style_t * style, const char * fn)
{
    if(fn == NULL) {
        lv_draw_rect(cords_p, mask_p, lv_style_get(LV_STYLE_PLAIN, NULL));
        lv_draw_label(cords_p, mask_p, lv_style_get(LV_STYLE_PLAIN, NULL), "No data", TXT_FLAG_NONE, NULL);
    } else {
        fs_file_t file;
        fs_res_t res = fs_open(&file, fn, FS_MODE_RD);
        if(res == FS_RES_OK) {
            lv_img_raw_header_t header;
            uint32_t br;
            res = fs_read(&file, &header, sizeof(lv_img_raw_header_t), &br);

            /*If the width is greater then real img. width then it is upscaled */
            bool upscale = false;
            if(area_get_width(cords_p) > header.w) upscale = true;

            area_t mask_com;    /*Common area of mask and cords*/
            bool union_ok;
            union_ok = area_union(&mask_com, mask_p, cords_p);
            if(union_ok == false) {
                fs_close(&file);
                return;
            }

            /*Round the coordinates with upscale*/
            if(upscale != false) {
                if((mask_com.y1 & 0x1) != 0) mask_com.y1 -= 1; /*Can be only even*/
                if((mask_com.y2 & 0x1) == 0) mask_com.y2 -= 1; /*Can be only odd*/
                if((mask_com.x1 & 0x1) != 0) mask_com.x1 -= 1; /*Can be only even*/
                if((mask_com.x2 & 0x1) == 0) mask_com.x2 -= 1; /*Can be only odd*/
            }


            bool const_data = false;

#if USE_UFS != 0
            /*If the img. data is inside the MCU then do not use FS reading just a pointer*/
            if(fn[0] == UFS_LETTER) {
                const_data = true;
                uint8_t * f_data = ((ufs_file_t*)file.file_d)->ent->data_d;
                f_data += sizeof(lv_img_raw_header_t);
                map_fp(cords_p, &mask_com, (void*)f_data , style->opa, header.transp, upscale, style->ccolor, style->img_recolor);
            }
#endif

            /*Read the img. with the FS interface*/
            if(const_data != false) {
                uint8_t us_shift = 0;
                uint8_t us_val = 1;
                if(upscale != false) {
                    us_shift = 1;
                    us_val = 2;
                }

                /* Move the file pointer to the start address according to mask
                 * But take care, the upscaled maps look greater*/
                uint32_t start_offset = sizeof(lv_img_raw_header_t);
                start_offset += (area_get_width(cords_p) >> us_shift) *
                               ((mask_com.y1 - cords_p->y1) >> us_shift) * sizeof(color_t); /*First row*/
                start_offset += ((mask_com.x1 - cords_p->x1) >> us_shift) * sizeof(color_t); /*First col*/
                fs_seek(&file, start_offset);

                uint32_t useful_data = (area_get_width(&mask_com) >> us_shift) * sizeof(color_t);
                uint32_t next_row = (area_get_width(cords_p) >> us_shift) * sizeof(color_t) - useful_data;

                area_t line;
                area_cpy(&line, &mask_com);
                area_set_height(&line, us_val); /*Create a line area. Hold 2 pixels if upscaled*/

                cord_t row;
                uint32_t act_pos;
                color_t buf[LV_HOR_RES];
                for(row = mask_com.y1; row <= mask_com.y2; row += us_val) {
                    res = fs_read(&file, buf, useful_data, &br);

                    map_fp(&line, &mask_com, buf, style->opa, header.transp, upscale,
                                          style->ccolor, style->img_recolor);

                    fs_tell(&file, &act_pos);
                    fs_seek(&file, act_pos + next_row);
                    line.y1 += us_val;    /*Go down a line*/
                    line.y2 += us_val;
                }
            }
        }

        fs_close(&file);

        if(res != FS_RES_OK) {
            lv_draw_rect(cords_p, mask_p, lv_style_get(LV_STYLE_PLAIN, NULL));
            lv_draw_label(cords_p, mask_p, lv_style_get(LV_STYLE_PLAIN, NULL), "No data", TXT_FLAG_NONE, NULL);
        }
    }
}
#endif

/**
 * Draw a line
 * @param p1 first point of the line
 * @param p2 second point of the line
 * @param mask_pthe line will be drawn only on this area 
 * @param lines_p pointer to a line style
 */
void lv_draw_line(const point_t * p1, const point_t * p2, const area_t * mask_p, 
                  const lv_style_t * style)
{
	if(style->line_width == 0) return;

	if(p1->x == p2->x && p1->y == p2->y) return;

	cord_t dx = MATH_ABS(p2->x - p1->x);
	cord_t sx = p1->x < p2->x ? 1 : -1;
	cord_t dy = MATH_ABS(p2->y - p1->y);
	cord_t sy = p1->y < p2->y ? 1 : -1;
	cord_t err = (dx > dy ? dx : -dy) / 2;
	cord_t e2;
	bool hor = dx > dy ? true : false;	/*Rather horizontal or vertical*/
	cord_t last_x = p1->x;
	cord_t last_y = p1->y;
	point_t act_point;
	act_point.x = p1->x;
	act_point.y = p1->y;


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
	width = ((style->line_width - 1) * width_corr_array[wcor]) >> LINE_WIDTH_CORR_SHIFT;
	width_half = width >> 1;
	width_1 = width & 0x1 ? 1 : 0;

	while(1){
	  if(hor == true && last_y != act_point.y) {
		  area_t act_area;
		  area_t draw_area;
		  act_area.x1 = last_x;
		  act_area.x2 = act_point.x - sx;
		  act_area.y1 = last_y - width_half ;
		  act_area.y2 = act_point.y - sy  + width_half + width_1;
		  last_y = act_point.y;
		  last_x = act_point.x;

		  draw_area.x1 = MATH_MIN(act_area.x1, act_area.x2);
		  draw_area.x2 = MATH_MAX(act_area.x1, act_area.x2);
		  draw_area.y1 = MATH_MIN(act_area.y1, act_area.y2);
		  draw_area.y2 = MATH_MAX(act_area.y1, act_area.y2);
		  fill_fp(&draw_area, mask_p, style->ccolor, style->opa);
	  }
	  if (hor == false && last_x != act_point.x) {
		  area_t act_area;
		  area_t draw_area;
		  act_area.x1 = last_x - width_half;
		  act_area.x2 = act_point.x  - sx + width_half + width_1;
		  act_area.y1 = last_y ;
		  act_area.y2 = act_point.y - sy;
		  last_y = act_point.y;
		  last_x = act_point.x;

		  draw_area.x1 = MATH_MIN(act_area.x1, act_area.x2);
		  draw_area.x2 = MATH_MAX(act_area.x1, act_area.x2);
		  draw_area.y1 = MATH_MIN(act_area.y1, act_area.y2);
		  draw_area.y2 = MATH_MAX(act_area.y1, act_area.y2);
		  fill_fp(&draw_area, mask_p, style->ccolor, style->opa);
	  }

		/*Calc. the next point of the line*/
		if (act_point.x == p2->x && act_point.y == p2->y) break;
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
		area_t act_area;
		area_t draw_area;
		act_area.x1 = last_x;
		act_area.x2 = act_point.x;
		act_area.y1 = last_y - width_half ;
		act_area.y2 = act_point.y + width_half + width_1;

		draw_area.x1 = MATH_MIN(act_area.x1, act_area.x2);
		draw_area.x2 = MATH_MAX(act_area.x1, act_area.x2);
		draw_area.y1 = MATH_MIN(act_area.y1, act_area.y2);
		draw_area.y2 = MATH_MAX(act_area.y1, act_area.y2);
		fill_fp(&draw_area, mask_p, style->ccolor, style->opa);
	}
	if (hor == false) {
		area_t act_area;
		area_t draw_area;
		act_area.x1 = last_x - width_half;
		act_area.x2 = act_point.x + width_half + width_1;
		act_area.y1 = last_y;
		act_area.y2 = act_point.y;

		draw_area.x1 = MATH_MIN(act_area.x1, act_area.x2);
		draw_area.x2 = MATH_MAX(act_area.x1, act_area.x2);
		draw_area.y1 = MATH_MIN(act_area.y1, act_area.y2);
		draw_area.y2 = MATH_MAX(act_area.y1, act_area.y2);
		fill_fp(&draw_area, mask_p, style->ccolor, style->opa);
	}
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Draw the middle part (rectangular) of a rectangle
 * @param cords_p the coordinates of the original rectangle
 * @param mask_p the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 */
static void lv_draw_rect_main_mid(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style)
{
    uint16_t radius = style->radius;

    color_t mcolor = style->mcolor;
    color_t gcolor = style->gcolor;
    uint8_t mix;
    opa_t opa = style->opa;
    cord_t height = area_get_height(cords_p);
    cord_t width = area_get_width(cords_p);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    /*If the radius is too big then there is no body*/
    if(radius > height / 2)  return;

	area_t work_area;
	work_area.x1 = cords_p->x1;
	work_area.x2 = cords_p->x2;

    if(mcolor.full == gcolor.full) {
    	work_area.y1 = cords_p->y1 + radius;
    	work_area.y2 = cords_p->y2 - radius;
		fill_fp(&work_area, mask_p, mcolor, opa);
    } else {

		cord_t row;
		cord_t row_start = cords_p->y1 + radius;
		cord_t row_end = cords_p->y2 - radius;
		color_t act_color;
        if(row_start < 0) row_start = 0;
		for(row = row_start ;
			row <= row_end;
			row ++)
		{
			work_area.y1 = row;
			work_area.y2 = row;
			mix = (uint32_t)((uint32_t)(cords_p->y2 - work_area.y1) * 255) / height;
			act_color = color_mix(mcolor, gcolor, mix);

			fill_fp(&work_area, mask_p, act_color, opa);
		}
    }
}
/**
 * Draw the top and bottom parts (corners) of a rectangle
 * @param cords_p the coordinates of the original rectangle
 * @param mask_p the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 */
static void lv_draw_rect_main_corner(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style_p)
{
    uint16_t radius = style_p->radius;

    color_t mcolor = style_p->mcolor;
    color_t gcolor = style_p->gcolor;
    color_t act_color;
    opa_t opa = style_p->opa;
    uint8_t mix;
    cord_t height = area_get_height(cords_p);
    cord_t width = area_get_width(cords_p);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    point_t lt_origo;   /*Left  Top    origo*/
    point_t lb_origo;   /*Left  Bottom origo*/
    point_t rt_origo;   /*Right Top    origo*/
    point_t rb_origo;   /*Left  Bottom origo*/

    lt_origo.x = cords_p->x1 + radius;
    lt_origo.y = cords_p->y1 + radius;

    lb_origo.x = cords_p->x1 + radius;
    lb_origo.y = cords_p->y2 - radius;

    rt_origo.x = cords_p->x2 - radius;
    rt_origo.y = cords_p->y1 + radius;

    rb_origo.x = cords_p->x2 - radius;
    rb_origo.y = cords_p->y2 - radius;


    area_t edge_top_area;
    area_t mid_top_area;
    area_t mid_bot_area;
    area_t edge_bot_area;


    point_t cir;
    cord_t cir_tmp;
    circ_init(&cir, &cir_tmp, radius);

    /*Init the areas*/
    area_set(&mid_bot_area,  lb_origo.x + CIRC_OCT4_X(cir),
                             lb_origo.y + CIRC_OCT4_Y(cir),
                             rb_origo.x + CIRC_OCT1_X(cir),
                             rb_origo.y + CIRC_OCT1_Y(cir));

    area_set(&edge_bot_area, lb_origo.x + CIRC_OCT3_X(cir),
                             lb_origo.y + CIRC_OCT3_Y(cir),
                             rb_origo.x + CIRC_OCT2_X(cir),
                             rb_origo.y + CIRC_OCT2_Y(cir));

    area_set(&mid_top_area,  lt_origo.x + CIRC_OCT5_X(cir),
                             lt_origo.y + CIRC_OCT5_Y(cir),
                             rt_origo.x + CIRC_OCT8_X(cir),
                             rt_origo.y + CIRC_OCT8_Y(cir));

    area_set(&edge_top_area, lt_origo.x + CIRC_OCT6_X(cir),
                             lt_origo.y + CIRC_OCT6_Y(cir),
                             rt_origo.x + CIRC_OCT7_X(cir),
                             rt_origo.y + CIRC_OCT7_Y(cir));

    while(circ_cont(&cir)) {
        uint8_t edge_top_refr = 0;
        uint8_t mid_top_refr = 0;
        uint8_t mid_bot_refr = 0;
        uint8_t edge_bot_refr = 0;

        /*If a new row coming draw the previous
         * The x coordinate can grow on the same y so wait for the last x*/
        if(mid_bot_area.y1 != CIRC_OCT4_Y(cir) + lb_origo.y ) {
            mid_bot_refr = 1;
        }

        if(edge_bot_area.y1 != CIRC_OCT2_Y(cir) + lb_origo.y) {
            edge_bot_refr = 1;
        }

        if(mid_top_area.y1 != CIRC_OCT8_Y(cir) + lt_origo.y) {
            mid_top_refr = 1;
        }

        if(edge_top_area.y1 != CIRC_OCT7_Y(cir) + lt_origo.y) {
            edge_top_refr = 1;
        }

        /* Do not refresh the first row in the middle
         * because the body drawer makes it*/
        if(mid_bot_area.y1 == cords_p->y2 - radius){
            mid_bot_refr = 0;
        }

        if(mid_top_area.y1 == cords_p->y1 + radius){
            mid_top_refr = 0;
        }

        /*Draw the areas which are not disabled*/
        if(edge_top_refr != 0){
            mix = (uint32_t)((uint32_t)(cords_p->y2 - edge_top_area.y1)  * 255) / height;
            act_color = color_mix(mcolor, gcolor, mix);
            fill_fp(&edge_top_area, mask_p, act_color, opa);
        }

        if(mid_top_refr != 0) {
            mix = (uint32_t)((uint32_t)(cords_p->y2 - mid_top_area.y1) * 255) / height;
            act_color = color_mix(mcolor, gcolor, mix);
            fill_fp(&mid_top_area, mask_p, act_color, opa);
        }

        if(mid_bot_refr != 0) {
            mix = (uint32_t)((uint32_t)(cords_p->y2 - mid_bot_area.y1) * 255) / height;
            act_color = color_mix(mcolor, gcolor, mix);
            fill_fp(&mid_bot_area, mask_p, act_color, opa);
        }

        if(edge_bot_refr != 0) {
            mix = (uint32_t)((uint32_t)(cords_p->y2 - edge_bot_area.y1) * 255) / height;
            act_color = color_mix(mcolor, gcolor, mix);
            fill_fp(&edge_bot_area, mask_p, act_color, opa);
        }
         /*Save the current coordinates*/
        area_set(&mid_bot_area,  lb_origo.x + CIRC_OCT4_X(cir),
                                 lb_origo.y + CIRC_OCT4_Y(cir),
                                 rb_origo.x + CIRC_OCT1_X(cir),
                                 rb_origo.y + CIRC_OCT1_Y(cir));

        area_set(&edge_bot_area, lb_origo.x + CIRC_OCT3_X(cir),
                                 lb_origo.y + CIRC_OCT3_Y(cir),
                                 rb_origo.x + CIRC_OCT2_X(cir),
                                 rb_origo.y + CIRC_OCT2_Y(cir));

        area_set(&mid_top_area,  lt_origo.x + CIRC_OCT5_X(cir),
                                 lt_origo.y + CIRC_OCT5_Y(cir),
                                 rt_origo.x + CIRC_OCT8_X(cir),
                                 rt_origo.y + CIRC_OCT8_Y(cir));

        area_set(&edge_top_area, lt_origo.x + CIRC_OCT6_X(cir),
                                 lt_origo.y + CIRC_OCT6_Y(cir),
                                 rt_origo.x + CIRC_OCT7_X(cir),
                                 rt_origo.y + CIRC_OCT7_Y(cir));

        circ_next(&cir, &cir_tmp);
    }

	mix = (uint32_t)((uint32_t)(cords_p->y2 - edge_top_area.y1)  * 255) / height;
	act_color = color_mix(mcolor, gcolor, mix);
	fill_fp(&edge_top_area, mask_p, act_color, opa);
if(edge_top_area.y1 != mid_top_area.y1) {
		mix = (uint32_t)((uint32_t)(cords_p->y2 - mid_top_area.y1) * 255) / height;
		act_color = color_mix(mcolor, gcolor, mix);
		fill_fp(&mid_top_area, mask_p, act_color, opa);
	}
	mix = (uint32_t)((uint32_t)(cords_p->y2 - mid_bot_area.y1) * 255) / height;
	act_color = color_mix(mcolor, gcolor, mix);
	fill_fp(&mid_bot_area, mask_p, act_color, opa);

	if(edge_bot_area.y1 != mid_bot_area.y1) {
		mix = (uint32_t)((uint32_t)(cords_p->y2 - edge_bot_area.y1) * 255) / height;
		act_color = color_mix(mcolor, gcolor, mix);
		fill_fp(&edge_bot_area, mask_p, act_color, opa);
	}

}

/**
 * Draw the straight parts of a rectangle border
 * @param cords_p the coordinates of the original rectangle
 * @param mask_p the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 */
static void lv_draw_rect_border_straight(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style_p)
{
    uint16_t radius = style_p->radius;

    cord_t width = area_get_width(cords_p);
    cord_t height = area_get_height(cords_p);
    uint16_t bwidth = style_p->bwidth;
    opa_t bopa = (uint16_t)((uint16_t) style_p->opa * style_p->bopa) >> 8;
    area_t work_area;
    cord_t length_corr = 0;
    cord_t corner_size = 0;

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

    color_t b_color = style_p->bcolor;

    /*Left border*/
    work_area.x1 = cords_p->x1;
    work_area.x2 = work_area.x1 + bwidth;
    work_area.y1 = cords_p->y1 + corner_size;
    work_area.y2 = cords_p->y2 - corner_size;
    fill_fp(&work_area, mask_p, b_color, bopa);

    /*Right border*/
    work_area.x2 = cords_p->x2;
    work_area.x1 = work_area.x2 - bwidth;
    fill_fp(&work_area, mask_p, b_color, bopa);

    /*Upper border*/
    work_area.x1 = cords_p->x1 + corner_size - length_corr;
    work_area.x2 = cords_p->x2 - corner_size + length_corr;
    work_area.y1 = cords_p->y1;
    work_area.y2 = cords_p->y1 + bwidth;
    fill_fp(&work_area, mask_p, b_color, bopa);

    /*Lower border*/
    work_area.y2 = cords_p->y2;
    work_area.y1 = work_area.y2 - bwidth;
    fill_fp(&work_area, mask_p, b_color, bopa);

    /*Draw the a remaining rectangles if the radius is smaller then b_width */
    if(length_corr != 0) {
        work_area.x1 = cords_p->x1;
        work_area.x2 = cords_p->x1 + radius;
        work_area.y1 = cords_p->y1 + radius + 1;
        work_area.y2 = cords_p->y1 + bwidth;
        fill_fp(&work_area, mask_p, b_color, bopa);

        work_area.x1 = cords_p->x2 - radius;
        work_area.x2 = cords_p->x2;
        work_area.y1 = cords_p->y1 + radius + 1;
        work_area.y2 = cords_p->y1 + bwidth;
        fill_fp(&work_area, mask_p, b_color, bopa);

        work_area.x1 = cords_p->x1;
        work_area.x2 = cords_p->x1 + radius;
        work_area.y1 = cords_p->y2 - bwidth;
        work_area.y2 = cords_p->y2 - radius - 1;
        fill_fp(&work_area, mask_p, b_color, bopa);

        work_area.x1 = cords_p->x2 - radius;
        work_area.x2 = cords_p->x2;
        work_area.y1 = cords_p->y2 - bwidth;
        work_area.y2 = cords_p->y2 - radius - 1;
        fill_fp(&work_area, mask_p, b_color, bopa);
    }

    /*If radius == 0 one px on the corners are not drawn*/
    if(radius == 0) {
        work_area.x1 = cords_p->x1;
        work_area.x2 = cords_p->x1;
        work_area.y1 = cords_p->y1;
        work_area.y2 = cords_p->y1;
        fill_fp(&work_area, mask_p, b_color, bopa);

        work_area.x1 = cords_p->x2;
        work_area.x2 = cords_p->x2;
        work_area.y1 = cords_p->y1;
        work_area.y2 = cords_p->y1;
        fill_fp(&work_area, mask_p, b_color, bopa);

        work_area.x1 = cords_p->x1;
        work_area.x2 = cords_p->x1;
        work_area.y1 = cords_p->y2;
        work_area.y2 = cords_p->y2;
        fill_fp(&work_area, mask_p, b_color, bopa);

        work_area.x1 = cords_p->x2;
        work_area.x2 = cords_p->x2;
        work_area.y1 = cords_p->y2;
        work_area.y2 = cords_p->y2;
        fill_fp(&work_area, mask_p, b_color, bopa);
    }
}


/**
 * Draw the corners of a rectangle border
 * @param cords_p the coordinates of the original rectangle
 * @param mask_p the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 * @param opa opacity of the rectangle (0..255)
 */
static void lv_draw_rect_border_corner(const area_t * cords_p, const area_t * mask_p, const  lv_style_t * style)
{
    uint16_t radius = style->radius;
    uint16_t bwidth = style->bwidth;
    color_t bcolor = style->bcolor;
    opa_t bopa = (uint16_t)((uint16_t) style->opa * style->bopa ) >> 8;

    /*0 px border width drawn as 1 px, so decrement the bwidth*/
    bwidth--;

    cord_t width = area_get_width(cords_p);
    cord_t height = area_get_height(cords_p);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    point_t lt_origo;   /*Left  Top    origo*/
    point_t lb_origo;   /*Left  Bottom origo*/
    point_t rt_origo;   /*Right Top    origo*/
    point_t rb_origo;   /*Left  Bottom origo*/

    lt_origo.x = cords_p->x1 + radius;
    lt_origo.y = cords_p->y1 + radius;

    lb_origo.x = cords_p->x1 + radius;
    lb_origo.y = cords_p->y2 - radius;

    rt_origo.x = cords_p->x2 - radius;
    rt_origo.y = cords_p->y1 + radius;

    rb_origo.x = cords_p->x2 - radius;
    rb_origo.y = cords_p->y2 - radius;

    point_t cir_out;
    cord_t tmp_out;
    circ_init(&cir_out, &tmp_out, radius);

    point_t cir_in;
    cord_t tmp_in;
    cord_t radius_in = radius - bwidth;

    if(radius_in < 0){
        radius_in = 0;
    }

    circ_init(&cir_in, &tmp_in, radius_in);

    area_t circ_area;
    cord_t act_w1;
    cord_t act_w2;

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
        circ_area.x1 = rb_origo.x + CIRC_OCT1_X(cir_out) - act_w2;
        circ_area.x2 = rb_origo.x + CIRC_OCT1_X(cir_out);
        circ_area.y1 = rb_origo.y + CIRC_OCT1_Y(cir_out);
        circ_area.y2 = rb_origo.y + CIRC_OCT1_Y(cir_out);
        fill_fp(&circ_area, mask_p, bcolor, bopa);

        circ_area.x1 = rb_origo.x + CIRC_OCT2_X(cir_out);
        circ_area.x2 = rb_origo.x + CIRC_OCT2_X(cir_out);
        circ_area.y1 = rb_origo.y + CIRC_OCT2_Y(cir_out)- act_w1;
        circ_area.y2 = rb_origo.y + CIRC_OCT2_Y(cir_out);
        fill_fp(&circ_area, mask_p, bcolor, bopa);

        /*Draw the octets to the left bottom corner*/
        circ_area.x1 = lb_origo.x + CIRC_OCT3_X(cir_out);
        circ_area.x2 = lb_origo.x + CIRC_OCT3_X(cir_out);
        circ_area.y1 = lb_origo.y + CIRC_OCT3_Y(cir_out) - act_w2;
        circ_area.y2 = lb_origo.y + CIRC_OCT3_Y(cir_out);
        fill_fp(&circ_area, mask_p, bcolor, bopa);

        circ_area.x1 = lb_origo.x + CIRC_OCT4_X(cir_out);
        circ_area.x2 = lb_origo.x + CIRC_OCT4_X(cir_out) + act_w1;
        circ_area.y1 = lb_origo.y + CIRC_OCT4_Y(cir_out);
        circ_area.y2 = lb_origo.y + CIRC_OCT4_Y(cir_out);
        fill_fp(&circ_area, mask_p, bcolor, bopa);
        
        /*Draw the octets to the left top corner*/
        /*Don't draw if the lines are common in the middle*/
        if(lb_origo.y + CIRC_OCT4_Y(cir_out) > lt_origo.y + CIRC_OCT5_Y(cir_out)) {
            circ_area.x1 = lt_origo.x + CIRC_OCT5_X(cir_out);
            circ_area.x2 = lt_origo.x + CIRC_OCT5_X(cir_out) + act_w2;
            circ_area.y1 = lt_origo.y + CIRC_OCT5_Y(cir_out);
            circ_area.y2 = lt_origo.y + CIRC_OCT5_Y(cir_out);
            fill_fp(&circ_area, mask_p, bcolor, bopa);
        }

        circ_area.x1 = lt_origo.x + CIRC_OCT6_X(cir_out);
        circ_area.x2 = lt_origo.x + CIRC_OCT6_X(cir_out);
        circ_area.y1 = lt_origo.y + CIRC_OCT6_Y(cir_out);
        circ_area.y2 = lt_origo.y + CIRC_OCT6_Y(cir_out) + act_w1;
        fill_fp(&circ_area, mask_p, bcolor, bopa);
        
        /*Draw the octets to the right top corner*/
        circ_area.x1 = rt_origo.x + CIRC_OCT7_X(cir_out);
        circ_area.x2 = rt_origo.x + CIRC_OCT7_X(cir_out);
        circ_area.y1 = rt_origo.y + CIRC_OCT7_Y(cir_out);
        circ_area.y2 = rt_origo.y + CIRC_OCT7_Y(cir_out) + act_w2;
        fill_fp(&circ_area, mask_p, bcolor, bopa);

        /*Don't draw if the lines are common in the middle*/
        if(rb_origo.y + CIRC_OCT1_Y(cir_out) > rt_origo.y + CIRC_OCT8_Y(cir_out)) {
            circ_area.x1 = rt_origo.x + CIRC_OCT8_X(cir_out) - act_w1;
            circ_area.x2 = rt_origo.x + CIRC_OCT8_X(cir_out);
            circ_area.y1 = rt_origo.y + CIRC_OCT8_Y(cir_out);
            circ_area.y2 = rt_origo.y + CIRC_OCT8_Y(cir_out);
            fill_fp(&circ_area, mask_p, bcolor, bopa);
        }

        circ_next(&cir_out, &tmp_out);

        /*The internal circle will be ready faster
         * so check it! */
        if(cir_in.y < cir_in.x) {
            circ_next(&cir_in, &tmp_in);
        }
    }
}

/**
 * Draw a shadow
 * @param rect pointer to rectangle object
 * @param mask pointer to a mask area (from the design functions)
 */
static void lv_draw_rect_shadow(const area_t * cords_p, const area_t * mask_p, const  lv_style_t * style)
{
    /* If mask is in the middle of cords do not draw shadow*/
    cord_t radius = style->radius;
    cord_t width = area_get_width(cords_p);
    cord_t height = area_get_height(cords_p);
    radius = lv_draw_cont_radius_corr(radius, width, height);
    area_t area_tmp;

    /*Check horizontally without radius*/
    area_cpy(&area_tmp, cords_p);
    area_tmp.x1 += radius;
    area_tmp.x2 -= radius;
    if(area_is_in(mask_p, &area_tmp) != false) return;

    /*Check vertically without radius*/
    area_cpy(&area_tmp, cords_p);
    area_tmp.y1 += radius;
    area_tmp.y2 -= radius;
    if(area_is_in(mask_p, &area_tmp) != false) return;

    if(style->stype == LV_STYPE_FULL) {
        lv_draw_cont_shadow_full(cords_p, mask_p, style);
    } else if(style->stype == LV_STYPE_BOTTOM) {
        lv_draw_cont_shadow_bottom(cords_p, mask_p, style);
    }
}

static void lv_draw_cont_shadow_full(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style)
{
    cord_t radius = style->radius;

    cord_t width = area_get_width(cords_p);
    cord_t height = area_get_height(cords_p);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    cord_t cruve_x[LV_VER_RES] = {CORD_MIN};
    memset(cruve_x, 0, sizeof(cruve_x));
    point_t circ;
    cord_t circ_tmp;
    circ_init(&circ, &circ_tmp, radius);
    while(circ_cont(&circ)) {
        cruve_x[CIRC_OCT1_Y(circ)] = CIRC_OCT1_X(circ);
        cruve_x[CIRC_OCT2_Y(circ)] = CIRC_OCT2_X(circ);
        circ_next(&circ, &circ_tmp);
    }
    int16_t row;

    uint16_t opa_h_result[LV_HOR_RES];
    int16_t filter_size = 2 * style->swidth + 1;

    for(row = 0; row < filter_size; row++) {
        opa_h_result[row] = (uint32_t)((uint32_t)(filter_size - row) * style->opa * 2) / (filter_size);
    }

    uint16_t p;
    opa_t opa_v_result[LV_VER_RES];

    point_t point_rt;
    point_t point_rb;
    point_t point_lt;
    point_t point_lb;
    point_t ofs_rb;
    point_t ofs_rt;
    point_t ofs_lb;
    point_t ofs_lt;
    ofs_rb.x = cords_p->x2 - radius;
    ofs_rb.y = cords_p->y2 - radius;

    ofs_rt.x = cords_p->x2 - radius;
    ofs_rt.y = cords_p->y1 + radius;

    ofs_lb.x = cords_p->x1 + radius;
    ofs_lb.y = cords_p->y2 - radius;

    ofs_lt.x = cords_p->x1 + radius;
    ofs_lt.y = cords_p->y1 + radius;


    for(row = 0; row < radius + style->swidth; row++) {
        for(p = 0; p < radius + style->swidth; p++) {
           int16_t v;
           uint32_t opa_tmp = 0;
           int16_t row_v;
           bool swidth_out = false;
           for(v = -style->swidth; v < style->swidth; v++) {
               row_v = row + v;
               if(row_v < 0) row_v = 0; /*Rows above the corner*/

               /*Rows below the bottom are empty so they won't modify the filter*/
               if(row_v > radius) {
                   break;
               }
               else
               {
                   int16_t p_tmp = p - (cruve_x[row_v] - cruve_x[row]);
                   if(p_tmp < -style->swidth) { /*Cols before the filtered shadow (still not blurred)*/
                       opa_tmp += style->opa * 2;
                   }
                   /*Cols after the filtered shadow (already no effect) */
                   else if (p_tmp > style->swidth) {
                       /* If on the current point the  filter top point is already out of swidth then
                        * the remaining part will not do not anything on this point*/
                       if(v == -style->swidth) { /*Is the first point?*/
                           swidth_out = true;
                       }
                       break;
                   } else {
                       opa_tmp += opa_h_result[p_tmp + style->swidth];
                   }
               }
           }
           if(swidth_out == false) {
               opa_tmp = opa_tmp / (filter_size);
               opa_v_result[p] = opa_tmp > OPA_COVER ? OPA_COVER : opa_tmp;
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
                px_fp(point_lt.x,point_lt.y , mask_p, style->scolor, opa_v_result[d]);
            }

            if(point_rb.x != point_lb.x && point_lt.y != point_lb.y) {
                px_fp(point_lb.x,point_lb.y , mask_p, style->scolor, opa_v_result[d]);
            }

            if(point_lt.y != point_lb.y) {
                px_fp(point_rb.x,point_rb.y , mask_p, style->scolor, opa_v_result[d]);
            }


            px_fp(point_rt.x,point_rt.y , mask_p, style->scolor, opa_v_result[d]);


            point_rb.x++;
            point_lb.x--;

            point_rt.x++;
            point_lt.x--;
        }

        /*When the first row is known draw the straight pars with same opa. map*/
        if(row == 0) {
           lv_draw_cont_shadow_full_straight(cords_p, mask_p, style, opa_v_result);
        }
    }
}


static void lv_draw_cont_shadow_bottom(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style)
{
    cord_t radius = style->radius;

    cord_t width = area_get_width(cords_p);
    cord_t height = area_get_height(cords_p);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    cord_t cruve_x[LV_VER_RES] = {CORD_MIN};
    memset(cruve_x, 0, sizeof(cruve_x));
    point_t circ;
    cord_t circ_tmp;
    circ_init(&circ, &circ_tmp, radius);
    while(circ_cont(&circ)) {
        cruve_x[CIRC_OCT1_Y(circ)] = CIRC_OCT1_X(circ);
        cruve_x[CIRC_OCT2_Y(circ)] = CIRC_OCT2_X(circ);
        circ_next(&circ, &circ_tmp);
    }
    int16_t row;

    opa_t opa_h_result[LV_HOR_RES];
    int16_t filter_size = 2 * style->swidth + 1;

    for(row = 0; row < filter_size; row++) {
        opa_h_result[row] = (uint32_t)((uint32_t)(filter_size - row) * style->opa) / (filter_size);
    }

    point_t point_l;
    point_t point_r;
    area_t area_mid;
    point_t ofs1;
    point_t ofs2;

    ofs1.x = cords_p->x1 + radius;
    ofs1.y = cords_p->y2 - radius;

    ofs2.x = cords_p->x2 - radius;
    ofs2.y = cords_p->y2 - radius;

    for(row = 0; row < radius; row++) {
        point_l.x = ofs1.x + radius - row - radius;
        point_l.y = ofs1.y + cruve_x[row];

        point_r.x = ofs2.x + row;
        point_r.y = ofs2.y + cruve_x[row];

        uint16_t d;
        for(d= style->swidth; d < filter_size; d++) {
            px_fp(point_l.x, point_l.y, mask_p, style->scolor, opa_h_result[d]);
            point_l.y ++;

            px_fp(point_r.x, point_r.y, mask_p, style->scolor, opa_h_result[d]);
            point_r.y ++;
        }

    }

    area_mid.x1 = ofs1.x + 1;
    area_mid.y1 = ofs1.y + radius;
    area_mid.x2 = ofs2.x - 1;
    area_mid.y2 = area_mid.y1;

    uint16_t d;
    for(d= style->swidth; d < filter_size; d++) {
        fill_fp(&area_mid, mask_p, style->scolor, opa_h_result[d]);
        area_mid.y1 ++;
        area_mid.y2 ++;
    }
}

static void lv_draw_cont_shadow_full_straight(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style, const opa_t * map)
{

    cord_t radius = style->radius;

    cord_t width = area_get_width(cords_p);
    cord_t height = area_get_height(cords_p);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    area_t sider_area;
    sider_area.x1 = cords_p->x2;
    sider_area.y1 = cords_p->y1 + radius + 1;
    sider_area.x2 = sider_area.x1;
    sider_area.y2 = cords_p->y2 -  radius - 1;

    area_t sidel_area;
    sidel_area.x1 = cords_p->x1;
    sidel_area.y1 = cords_p->y1 + radius + 1;
    sidel_area.x2 = sidel_area.x1;
    sidel_area.y2 = cords_p->y2 - radius - 1;

    area_t sidet_area;
    sidet_area.x1 = cords_p->x1 + radius + 1;
    sidet_area.y1 = cords_p->y1;
    sidet_area.x2 = cords_p->x2 - radius - 1;
    sidet_area.y2 = sidet_area.y1;

    area_t sideb_area;
    sideb_area.x1 = cords_p->x1 + radius + 1;
    sideb_area.y1 = cords_p->y2;
    sideb_area.x2 = cords_p->x2 - radius - 1;
    sideb_area.y2 = sideb_area.y1;

    int16_t d;
    for(d = 0; d < style->swidth; d++) {
        fill_fp(&sider_area, mask_p, style->scolor, map[d]);
        sider_area.x1++;
        sider_area.x2++;

        fill_fp(&sidel_area, mask_p, style->scolor, map[d]);
        sidel_area.x1--;
        sidel_area.x2--;

        fill_fp(&sidet_area, mask_p, style->scolor, map[d]);
        sidet_area.y1--;
        sidet_area.y2--;

        fill_fp(&sideb_area, mask_p, style->scolor, map[d]);
        sideb_area.y1++;
        sideb_area.y2++;
    }

}
static uint16_t lv_draw_cont_radius_corr(uint16_t r, cord_t w, cord_t h)
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
static void point_swap(point_t * p1, point_t * p2)
{
    point_t tmp;
    tmp.x = p1->x;
    tmp.y = p1->y;

    p1->x = p2->x;
    p1->y = p2->y;

    p2->x = tmp.x;
    p2->y = tmp.y;

}

#endif
