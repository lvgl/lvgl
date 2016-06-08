/**
 * @file lv_line.h
 *
 */

#ifndef LV_LINE_H
#define LV_LINE_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_LINE != 0

#include "lvgl/lv_obj/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
	const point_t * point_p;
	uint16_t  point_num;
	uint8_t  auto_size	:1;
	uint8_t  y_inv	:1;
}lv_line_t;

typedef struct
{
	color_t color;
	color_t bg_color;
	uint16_t width;
	opa_t bg_opa;
}lv_lines_t;

typedef enum
{
	LV_LINES_DEF,
	LV_LINES_DECOR,
	LV_LINES_CHART,
}lv_lines_builtin_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* lv_line_create(lv_obj_t * par_dp, lv_obj_t * copy_dp);
bool lv_line_signal(lv_obj_t * obj_dp, lv_signal_t sign, void * param);
lv_lines_t * lv_lines_get(lv_lines_builtin_t style, lv_lines_t * copy_p);
void lv_line_set_points(lv_obj_t * obj_dp, const point_t * point_a, uint16_t point_num);
void lv_line_set_auto_size(lv_obj_t * obj_dp, bool en);
void lv_line_set_y_inv(lv_obj_t * obj_dp, bool en);
bool lv_line_get_auto_size(lv_obj_t * obj_dp);
bool lv_line_get_y_inv(lv_obj_t * obj_dp);

/**********************
 *      MACROS
 **********************/

#endif

#endif
