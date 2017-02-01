/**
 * @file lv_draw_img.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl/lv_misc/circ.h>
#include "lv_conf.h"

#include <stdio.h>
#include <stdbool.h>
#include "lvgl/lv_misc/text.h"
#include "lv_draw.h"
#include "misc/fs/fsint.h"
#include "misc/math/math_base.h"
#include "lv_draw_rbasic.h"
#include "lv_draw_vbasic.h"

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
#if USE_LV_RECT != 0
static void lv_draw_rect_main_mid(const area_t * cords_p, const area_t * mask_p, const lv_rects_t * rects_p, opa_t opa);
static void lv_draw_rect_main_corner(const area_t * cords_p, const area_t * mask_p, const lv_rects_t * rects_p, opa_t opa);
static void lv_draw_rect_border_straight(const area_t * cords_p, const area_t * mask_p, const lv_rects_t * rects_p, opa_t opa);
static void lv_draw_rect_border_corner(const area_t * cords_p, const area_t * mask_p, const lv_rects_t * rects_p, opa_t opa);
static uint16_t lv_draw_rect_radius_corr(uint16_t r, cord_t w, cord_t h);
#endif /*USE_LV_RECT != 0*/

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_VDB_SIZE != 0
static void (*fill_fp)(const area_t * cords_p, const area_t * mask_p, color_t color, opa_t opa) =  lv_vfill;
static void (*letter_fp)(const point_t * pos_p, const area_t * mask_p, const font_t * font_p, uint8_t letter, color_t color, opa_t opa) = lv_vletter;
static void (*map_fp)(const area_t * cords_p, const area_t * mask_p, const color_t * map_p, opa_t opa, bool transp, bool upscale, color_t recolor, opa_t recolor_opa) = lv_vmap;
#else
static void (*fill_fp)(const area_t * cords_p, const area_t * mask_p, color_t color, opa_t opa) =  lv_rfill;
static void (*letter_fp)(const point_t * pos_p, const area_t * mask_p, const font_t * font_p, uint8_t letter, color_t color, opa_t opa) = lv_rletter;
static void (*map_fp)(const area_t * cords_p, const area_t * mask_p, const color_t * map_p, opa_t opa, bool transp, bool upscale, color_t recolor, opa_t recolor_opa) = lv_rmap;
#endif


#if USE_LV_IMG != 0 && USE_FSINT != 0 && USE_UFS != 0
static lv_rects_t lv_img_no_pic_rects = {
  .objs.color = COLOR_BLACK, .gcolor = COLOR_BLACK,
  .bcolor = COLOR_RED, .bwidth = 2 * LV_DOWNSCALE, .bopa = 100,
  .round = 0, .empty = 0
};

static lv_labels_t lv_img_no_pic_labels = {
  .font = LV_FONT_DEFAULT, .objs.color = COLOR_WHITE,
  .letter_space = 1 * LV_DOWNSCALE, .line_space =  1 * LV_DOWNSCALE,
  .mid =  1,
};
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if USE_LV_RECT != 0
/**
 * Draw a rectangle 
 * @param cords_p the coordinates of the rectangle
 * @param mask_p the rectangle will be drawn only in this mask
 * @param rects_p pointer to a rectangle style
 * @param opa the opacity of the rectangle (0..255)
 */
void lv_draw_rect(const area_t * cords_p, const area_t * mask_p, 
                  const lv_rects_t * rects_p, opa_t opa)
{
    if(area_get_height(cords_p) < 1 || area_get_width(cords_p) < 1) return;

    if(rects_p->empty == 0){
        lv_draw_rect_main_mid(cords_p, mask_p, rects_p, opa);

        if(rects_p->round != 0) {
            lv_draw_rect_main_corner(cords_p, mask_p, rects_p, opa);
        }
    } 
    
    if(rects_p->bwidth != 0) {
        lv_draw_rect_border_straight(cords_p, mask_p, rects_p, opa);

        if(rects_p->round != 0) {
            lv_draw_rect_border_corner(cords_p, mask_p, rects_p, opa);
        }
    }
}
#endif /*USE_LV_RECT != 0*/

#if USE_LV_LABEL != 0
/**
 * Write a text
 * @param cords_p coordinates of the label
 * @param mask_p the label will be drawn only in this area
 * @param labels_p pointer to a label style
 * @param opa opacity of the text (0..255)
 * @param txt 0 terminated text to write
 * @param flag settings for the text from 'txt_flag_t' enum
 */
void lv_draw_label(const area_t * cords_p,const area_t * mask_p,
                   const lv_labels_t * style, opa_t opa, const char * txt, txt_flag_t flag)
{
    const font_t * font_p = font_get(style->font);

    cord_t w = area_get_width(cords_p);

    /*Init variables for the first line*/
    cord_t line_length = 0;
    uint32_t line_start = 0;
    uint32_t line_end = txt_get_next_line(txt, font_p, style->letter_space, w, flag);

    point_t pos;
    pos.x = cords_p->x1;
    pos.y = cords_p->y1;

    /*Align the line to middle if enabled*/
    if(style->mid != 0) {
        line_length = txt_get_width(&txt[line_start], line_end - line_start,
                                    font_p, style->letter_space, flag);
        pos.x += (w - line_length) / 2;
    }

    cmd_state_t cmd_state = CMD_STATE_WAIT;
    uint32_t i;
    uint16_t par_start;
    color_t recolor;

    /*Write out all lines*/
    while(txt[line_start] != '\0') {
        /*Write all letter of a line*/
        cmd_state = CMD_STATE_WAIT;

        for(i = line_start; i < line_end; i++) {
            /*Handle the recolor command*/
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
                            char buf[LABEL_RECOLOR_PAR_LENGTH];
                            memcpy(buf, &txt[par_start], LABEL_RECOLOR_PAR_LENGTH);
                            buf[LABEL_RECOLOR_PAR_LENGTH] = '\0';
                            int r,g,b;
                            sscanf(buf, "%02x%02x%02x", &r, &g, &b);
                            recolor = COLOR_MAKE(r, g, b);
                        } else {
                            recolor.full = style->objs.color.full;
                        }
                        cmd_state = CMD_STATE_IN; /*After the parameter the text is in the command*/
                    }
                    continue;
                }
            }

            if(cmd_state == CMD_STATE_IN)  letter_fp(&pos, mask_p, font_p, txt[i], recolor, opa);
            else letter_fp(&pos, mask_p, font_p, txt[i], style->objs.color, opa);
            pos.x += font_get_width(font_p, txt[i]) + style->letter_space;
        }
        /*Go to next line*/
        line_start = line_end;
        line_end += txt_get_next_line(&txt[line_start], font_p, style->letter_space, w, flag);

        pos.x = cords_p->x1;
        /*Align to middle*/
        if(style->mid != 0) {
            line_length = txt_get_width(&txt[line_start], line_end - line_start,
                                     font_p, style->letter_space, flag);
            pos.x += (w - line_length) / 2;
        }
        /*Go the next line position*/
        pos.y += font_get_height(font_p);
        pos.y += style->line_space;
    }
}

#endif /* USE_LV_LABEL != 0*/

#if USE_LV_IMG != 0 && USE_FSINT != 0 && USE_UFS != 0
/**
 * Draw an image
 * @param cords_p the coordinates of the image
 * @param mask_p the image will be drawn only in this area
 * @param map_p pointer to a color_t array which contains the pixels of the image
 * @param opa opacity of the image (0..255)
 */
void lv_draw_img(const area_t * cords_p, const area_t * mask_p, 
             const lv_imgs_t * imgs_p,  opa_t opa, const char * fn)
{
    if(fn == NULL) {
        lv_draw_rect(cords_p, mask_p, &lv_img_no_pic_rects, opa);
        lv_draw_label(cords_p, mask_p,&lv_img_no_pic_labels, opa, "No data", TXT_FLAG_NONE);
    } else {
        fs_file_t file;
        fs_res_t res = fs_open(&file, fn, FS_MODE_RD);
        if(res == FS_RES_OK) {
            lv_img_raw_header_t header;
            uint32_t br;
            res = fs_read(&file, &header, sizeof(lv_img_raw_header_t), &br);

            /*If the width is greater then map width then it is upscaled */
            bool upscale = false;
            if(area_get_width(cords_p) > header.w) upscale = true;

            cord_t row;
            area_t act_area;


            area_t mask_sub;
            bool union_ok;
            union_ok = area_union(&mask_sub, mask_p, cords_p);
            if(union_ok == false) {
                fs_close(&file);
                return;
            }

            uint8_t ds_shift = 0;
            uint8_t ds_num = 1;
            /*Set some values if upscale enabled*/
            if(upscale != false) {
                ds_shift = 1;
                ds_num = 2;
            }

            uint32_t start_offset = sizeof(lv_img_raw_header_t);
            start_offset += (area_get_width(cords_p) >> ds_shift) *
                           ((mask_sub.y1 - cords_p->y1) >> ds_shift) * sizeof(color_t); /*First row*/
            start_offset += ((mask_sub.x1 - cords_p->x1) >> ds_shift) * sizeof(color_t); /*First col*/
            fs_seek(&file, start_offset);

            uint32_t useful_data = (area_get_width(&mask_sub) >> ds_shift) * sizeof(color_t);
            uint32_t next_row = (area_get_width(cords_p) >> ds_shift) * sizeof(color_t) - useful_data;


            /*Round the coordinates with upscale*/
            if(upscale != false) {
                if((mask_sub.x1 & 0x1) != 0) mask_sub.x1 -= 1; /*Can be only even*/
                if((mask_sub.x2 & 0x1) == 0) mask_sub.x2 -= 1; /*Can be only odd*/
            }
            area_cpy(&act_area, &mask_sub);

            /* Round down the start coordinate, because the upscaled images
             * can start only LV_DOWNSCALE 'y' coordinates */
            act_area.y1 &= ~(cord_t)(ds_num - 1) ;
            act_area.y2 = act_area.y1 + ds_num - 1;
            uint32_t act_pos;

            color_t buf[LV_HOR_RES];
            for(row = mask_sub.y1; row <= mask_sub.y2; row += ds_num) {
                res = fs_read(&file, buf, useful_data, &br);
                map_fp(&act_area, &mask_sub, buf, opa, header.transp, upscale,
                                  imgs_p->objs.color, imgs_p->recolor_opa);
                fs_tell(&file, &act_pos);
                fs_seek(&file, act_pos + next_row);
                act_area.y1 += ds_num;
                act_area.y2 += ds_num;
            }

        }
        fs_close(&file);

        if(res != FS_RES_OK) {
            lv_draw_rect(cords_p, mask_p, &lv_img_no_pic_rects, opa);
            lv_draw_label(cords_p, mask_p,&lv_img_no_pic_labels, opa, fn, TXT_FLAG_NONE);
        }
    }
}


#endif /*USE_LV_IMG != 0 && USE_FSINT != 0 && USE_UFS != 0*/

#if USE_LV_LINE != 0
/**
 * Draw a line
 * @param p1 first point of the line
 * @param p2 second point of the line
 * @param mask_pthe line will be drawn only on this area 
 * @param lines_p pointer to a line style
 * @param opa opacity of the line (0..255)
 */
void lv_draw_line(const point_t * p1, const point_t * p2, const area_t * mask_p, 
                  const lv_lines_t * lines_p, opa_t opa)
{
	if(lines_p->width == 0) return;

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
	width = ((lines_p->width - 1) * width_corr_array[wcor]) >> LINE_WIDTH_CORR_SHIFT;
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
		  fill_fp(&draw_area, mask_p, lines_p->objs.color, opa);
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
		  fill_fp(&draw_area, mask_p, lines_p->objs.color, opa);
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
		fill_fp(&draw_area, mask_p, lines_p->objs.color, opa);
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
		fill_fp(&draw_area, mask_p, lines_p->objs.color, opa);
	}
}
#endif /*USE_LV_LINE != 0*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if USE_LV_RECT != 0
/**
 * Draw the middle part (rectangular) of a rectangle
 * @param cords_p the coordinates of the original rectangle
 * @param mask_p the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 * @param opa opacity of the rectangle (0..255)
 */
static void lv_draw_rect_main_mid(const area_t * cords_p, const area_t * mask_p, const lv_rects_t * rects_p, opa_t opa)
{
    uint16_t radius = rects_p->round;

    color_t main_color = rects_p->objs.color;
    color_t grad_color = rects_p->gcolor;
    uint8_t mix;
    cord_t height = area_get_height(cords_p);
    cord_t width = area_get_width(cords_p);

    radius = lv_draw_rect_radius_corr(radius, width, height);

    /*If the radius is too big then there is no body*/
    if(radius > height / 2)  return;

	area_t work_area;
	work_area.x1 = cords_p->x1;
	work_area.x2 = cords_p->x2;

    if(main_color.full == grad_color.full) {
    	work_area.y1 = cords_p->y1 + radius;
    	work_area.y2 = cords_p->y2 - radius;
		fill_fp(&work_area, mask_p, main_color, opa);
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
			act_color = color_mix(main_color, grad_color, mix);

			fill_fp(&work_area, mask_p, act_color, opa);
		}
    }
}
/**
 * Draw the top and bottom parts (corners) of a rectangle
 * @param cords_p the coordinates of the original rectangle
 * @param mask_p the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 * @param opa opacity of the rectangle (0..255)
 */
static void lv_draw_rect_main_corner(const area_t * cords_p, const area_t * mask_p, const lv_rects_t * rects_p, opa_t opa)
{
    uint16_t radius = rects_p->round;

    color_t main_color = rects_p->objs.color;
    color_t grad_color = rects_p->gcolor;
    color_t act_color;
    uint8_t mix;
    cord_t height = area_get_height(cords_p);
    cord_t width = area_get_width(cords_p);

    radius = lv_draw_rect_radius_corr(radius, width, height);

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
            act_color = color_mix(main_color, grad_color, mix);
            fill_fp(&edge_top_area, mask_p, act_color, opa);
        }

        if(mid_top_refr != 0) {
            mix = (uint32_t)((uint32_t)(cords_p->y2 - mid_top_area.y1) * 255) / height;
            act_color = color_mix(main_color, grad_color, mix);
            fill_fp(&mid_top_area, mask_p, act_color, opa);
        }

        if(mid_bot_refr != 0) {
            mix = (uint32_t)((uint32_t)(cords_p->y2 - mid_bot_area.y1) * 255) / height;
            act_color = color_mix(main_color, grad_color, mix);
            fill_fp(&mid_bot_area, mask_p, act_color, opa);
        }

        if(edge_bot_refr != 0) {
            mix = (uint32_t)((uint32_t)(cords_p->y2 - edge_bot_area.y1) * 255) / height;
            act_color = color_mix(main_color, grad_color, mix);
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
	act_color = color_mix(main_color, grad_color, mix);
	fill_fp(&edge_top_area, mask_p, act_color, opa);
if(edge_top_area.y1 != mid_top_area.y1) {
		mix = (uint32_t)((uint32_t)(cords_p->y2 - mid_top_area.y1) * 255) / height;
		act_color = color_mix(main_color, grad_color, mix);
		fill_fp(&mid_top_area, mask_p, act_color, opa);
	}
	mix = (uint32_t)((uint32_t)(cords_p->y2 - mid_bot_area.y1) * 255) / height;
	act_color = color_mix(main_color, grad_color, mix);
	fill_fp(&mid_bot_area, mask_p, act_color, opa);

	if(edge_bot_area.y1 != mid_bot_area.y1) {
		mix = (uint32_t)((uint32_t)(cords_p->y2 - edge_bot_area.y1) * 255) / height;
		act_color = color_mix(main_color, grad_color, mix);
		fill_fp(&edge_bot_area, mask_p, act_color, opa);
	}

}

/**
 * Draw the straight parts of a rectangle border
 * @param cords_p the coordinates of the original rectangle
 * @param mask_p the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 * @param opa opacity of the rectangle (0..255)
 */
static void lv_draw_rect_border_straight(const area_t * cords_p, const area_t * mask_p, const lv_rects_t * rects_p, opa_t opa)
{
    uint16_t radius = rects_p->round;

    cord_t width = area_get_width(cords_p);
    cord_t height = area_get_height(cords_p);
    uint16_t b_width = rects_p->bwidth;
    opa_t b_opa = (uint16_t)((uint16_t) opa * rects_p->bopa) / 100;
    area_t work_area;
    cord_t length_corr = 0;
    cord_t corner_size = 0;

    /*the 0 px border width drawn as 1 px, so decrement the b_width*/
    b_width--;

    radius = lv_draw_rect_radius_corr(radius, width, height);

    if(radius < b_width) {
        length_corr = b_width - radius;
        corner_size = b_width;
    } else {
        corner_size = radius;
    }

    /* Modify the corner_size if corner is drawn */
    corner_size ++;

    color_t b_color = rects_p->bcolor;

    /*Left border*/
    work_area.x1 = cords_p->x1;
    work_area.x2 = work_area.x1 + b_width;
    work_area.y1 = cords_p->y1 + corner_size;
    work_area.y2 = cords_p->y2 - corner_size;
    fill_fp(&work_area, mask_p, b_color, b_opa);

    /*Right border*/
    work_area.x2 = cords_p->x2;
    work_area.x1 = work_area.x2 - b_width;
    fill_fp(&work_area, mask_p, b_color, b_opa);

    /*Upper border*/
    work_area.x1 = cords_p->x1 + corner_size - length_corr;
    work_area.x2 = cords_p->x2 - corner_size + length_corr;
    work_area.y1 = cords_p->y1;
    work_area.y2 = cords_p->y1 + b_width;
    fill_fp(&work_area, mask_p, b_color, b_opa);

    /*Lower border*/
    work_area.y2 = cords_p->y2;
    work_area.y1 = work_area.y2 - b_width;
    fill_fp(&work_area, mask_p, b_color, b_opa);

    /*Draw the a remaining rectangles if the radius is smaller then b_width */
    if(length_corr != 0) {
        work_area.x1 = cords_p->x1;
        work_area.x2 = cords_p->x1 + radius;
        work_area.y1 = cords_p->y1 + radius + 1;
        work_area.y2 = cords_p->y1 + b_width;
        fill_fp(&work_area, mask_p, b_color, b_opa);

        work_area.x1 = cords_p->x2 - radius;
        work_area.x2 = cords_p->x2;
        work_area.y1 = cords_p->y1 + radius + 1;
        work_area.y2 = cords_p->y1 + b_width;
        fill_fp(&work_area, mask_p, b_color, b_opa);

        work_area.x1 = cords_p->x1;
        work_area.x2 = cords_p->x1 + radius;
        work_area.y1 = cords_p->y2 - b_width;
        work_area.y2 = cords_p->y2 - radius - 1;
        fill_fp(&work_area, mask_p, b_color, b_opa);

        work_area.x1 = cords_p->x2 - radius;
        work_area.x2 = cords_p->x2;
        work_area.y1 = cords_p->y2 - b_width;
        work_area.y2 = cords_p->y2 - radius - 1;
        fill_fp(&work_area, mask_p, b_color, b_opa);
    }

    /*If radius == 0 one px on the corners are not drawn*/
    if(radius == 0) {
        work_area.x1 = cords_p->x1;
        work_area.x2 = cords_p->x1;
        work_area.y1 = cords_p->y1;
        work_area.y2 = cords_p->y1;
        fill_fp(&work_area, mask_p, b_color, b_opa);

        work_area.x1 = cords_p->x2;
        work_area.x2 = cords_p->x2;
        work_area.y1 = cords_p->y1;
        work_area.y2 = cords_p->y1;
        fill_fp(&work_area, mask_p, b_color, b_opa);

        work_area.x1 = cords_p->x1;
        work_area.x2 = cords_p->x1;
        work_area.y1 = cords_p->y2;
        work_area.y2 = cords_p->y2;
        fill_fp(&work_area, mask_p, b_color, b_opa);

        work_area.x1 = cords_p->x2;
        work_area.x2 = cords_p->x2;
        work_area.y1 = cords_p->y2;
        work_area.y2 = cords_p->y2;
        fill_fp(&work_area, mask_p, b_color, b_opa);
    }
}


/**
 * Draw the corners of a rectangle border
 * @param cords_p the coordinates of the original rectangle
 * @param mask_p the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 * @param opa opacity of the rectangle (0..255)
 */
static void lv_draw_rect_border_corner(const area_t * cords_p, const area_t * mask_p, const  lv_rects_t * rects_p, opa_t opa)
{
    uint16_t radius = rects_p->round;
    uint16_t b_width = rects_p->bwidth;
    color_t b_color = rects_p->bcolor;
    opa_t b_opa = (uint16_t)((uint16_t) opa * rects_p->bopa ) / 100;

    /*0 px border width drawn as 1 px, so decrement the b_width*/
    b_width--;

    cord_t width = area_get_width(cords_p);
    cord_t height = area_get_height(cords_p);

    radius = lv_draw_rect_radius_corr(radius, width, height);

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
    cord_t radius_in = radius - b_width;

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
        fill_fp(&circ_area, mask_p, b_color, b_opa);

        circ_area.x1 = rb_origo.x + CIRC_OCT2_X(cir_out);
        circ_area.x2 = rb_origo.x + CIRC_OCT2_X(cir_out);
        circ_area.y1 = rb_origo.y + CIRC_OCT2_Y(cir_out)- act_w1;
        circ_area.y2 = rb_origo.y + CIRC_OCT2_Y(cir_out);
        fill_fp(&circ_area, mask_p, b_color, b_opa);

        /*Draw the octets to the left bottom corner*/
        circ_area.x1 = lb_origo.x + CIRC_OCT3_X(cir_out);
        circ_area.x2 = lb_origo.x + CIRC_OCT3_X(cir_out);
        circ_area.y1 = lb_origo.y + CIRC_OCT3_Y(cir_out) - act_w2;
        circ_area.y2 = lb_origo.y + CIRC_OCT3_Y(cir_out);
        fill_fp(&circ_area, mask_p, b_color, b_opa);

        circ_area.x1 = lb_origo.x + CIRC_OCT4_X(cir_out);
        circ_area.x2 = lb_origo.x + CIRC_OCT4_X(cir_out) + act_w1;
        circ_area.y1 = lb_origo.y + CIRC_OCT4_Y(cir_out);
        circ_area.y2 = lb_origo.y + CIRC_OCT4_Y(cir_out);
        fill_fp(&circ_area, mask_p, b_color, b_opa);
        
        /*Draw the octets to the left top corner*/
        /*Don't draw if the lines are common in the middle*/
        if(lb_origo.y + CIRC_OCT4_Y(cir_out) > lt_origo.y + CIRC_OCT5_Y(cir_out)) {
            circ_area.x1 = lt_origo.x + CIRC_OCT5_X(cir_out);
            circ_area.x2 = lt_origo.x + CIRC_OCT5_X(cir_out) + act_w2;
            circ_area.y1 = lt_origo.y + CIRC_OCT5_Y(cir_out);
            circ_area.y2 = lt_origo.y + CIRC_OCT5_Y(cir_out);
            fill_fp(&circ_area, mask_p, b_color, b_opa);
        }

        circ_area.x1 = lt_origo.x + CIRC_OCT6_X(cir_out);
        circ_area.x2 = lt_origo.x + CIRC_OCT6_X(cir_out);
        circ_area.y1 = lt_origo.y + CIRC_OCT6_Y(cir_out);
        circ_area.y2 = lt_origo.y + CIRC_OCT6_Y(cir_out) + act_w1;
        fill_fp(&circ_area, mask_p, b_color, b_opa);
        
        /*Draw the octets to the right top corner*/
        circ_area.x1 = rt_origo.x + CIRC_OCT7_X(cir_out);
        circ_area.x2 = rt_origo.x + CIRC_OCT7_X(cir_out);
        circ_area.y1 = rt_origo.y + CIRC_OCT7_Y(cir_out);
        circ_area.y2 = rt_origo.y + CIRC_OCT7_Y(cir_out) + act_w2;
        fill_fp(&circ_area, mask_p, b_color, b_opa);

        /*Don't draw if the lines are common in the middle*/
        if(rb_origo.y + CIRC_OCT1_Y(cir_out) > rt_origo.y + CIRC_OCT8_Y(cir_out)) {
            circ_area.x1 = rt_origo.x + CIRC_OCT8_X(cir_out) - act_w1;
            circ_area.x2 = rt_origo.x + CIRC_OCT8_X(cir_out);
            circ_area.y1 = rt_origo.y + CIRC_OCT8_Y(cir_out);
            circ_area.y2 = rt_origo.y + CIRC_OCT8_Y(cir_out);
            fill_fp(&circ_area, mask_p, b_color, b_opa);
        }

        circ_next(&cir_out, &tmp_out);

        /*The internal circle will be ready faster
         * so check it! */
        if(cir_in.y < cir_in.x) {
            circ_next(&cir_in, &tmp_in);
        }
    }
}


static uint16_t lv_draw_rect_radius_corr(uint16_t r, cord_t w, cord_t h)
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

#endif /*USE_LV_RECT != 0*/

