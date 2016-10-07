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
/*Style of line*/
typedef struct
{
	lv_objs_t objs;	/*Style of ancestor*/
	/*New style element for this type */
	uint16_t width;
}lv_lines_t;

/*Built-in styles of line*/
typedef enum
{
	LV_LINES_DEF,
	LV_LINES_DECOR,
	LV_LINES_CHART,
}lv_lines_builtin_t;

/*Data of line*/
typedef struct
{
	/*Inherited from 'base_obj' so inherited ext.*/  /*Ext. of ancestor*/
	const point_t * point_array;
	uint16_t  point_num;
	uint8_t  auto_size	:1;
	uint8_t  y_inv	    :1;
	uint8_t  upscale    :1;
}lv_line_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_line_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_line_signal(lv_obj_t * line, lv_signal_t sign, void * param);
lv_lines_t * lv_lines_get(lv_lines_builtin_t style, lv_lines_t * copy);
void lv_line_set_points(lv_obj_t * line, const point_t * point_a, uint16_t point_num);
void lv_line_set_auto_size(lv_obj_t * line, bool autosize);
void lv_line_set_y_inv(lv_obj_t * line, bool yinv);
void lv_line_set_upscale(lv_obj_t * line, bool unscale);
bool lv_line_get_auto_size(lv_obj_t * line);
bool lv_line_get_y_inv(lv_obj_t * line);
bool lv_line_get_upscale(lv_obj_t * line);

/**********************
 *      MACROS
 **********************/

#endif

#endif
