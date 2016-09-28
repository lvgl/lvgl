/**
 * @file lv_chart.h
 *
 */

#ifndef LV_CHARTBG_H
#define LV_CHARTBG_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_CHARTBG != 0

#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
#include "lv_line.h"

/*********************
 *      DEFINES
 *********************/
#define LV_CHART_DL_NUM		8	/*Max data line number. Used in the style.*/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum
{
	LV_CHART_LINE,
	LV_CHART_COL,
	LV_CHART_POINT,
}lv_chart_type_t;

/*Style of chart background*/
typedef struct
{
	lv_rects_t bg_rects;
	lv_lines_t div_lines;
	uint8_t div_line_opa;		/*Percentage of obj. opacity*/
	color_t color[LV_CHART_DL_NUM];	/*Line/Point/Col color */
	uint16_t width;			/*Line width or point radius*/
	opa_t data_opa;				/*Line/Point/Col opacity in the percentage of obj. opacity*/
	uint8_t dark_eff;			/*Dark effect on the bottom of ó points and columns*/
}lv_charts_t;

/*Built-in styles of chart background*/
typedef enum
{
	LV_CHARTS_DEF,
}lv_charts_builtin_t;

/*Data of chart background*/
typedef struct
{
	lv_rect_ext_t rect_ext;
	cord_t ymin;
	cord_t ymax;
	uint8_t hdiv_num;
	uint8_t vdiv_num;

	ll_dsc_t dl_ll;				/*Linked list for the data line pointers (stores cord_t * )*/
	uint16_t pnum;				/*Point number in a data line*/
	uint8_t type	:2;			/*Line, column or point chart*/
	uint8_t dl_num;				/*Data line number in dl_ll*/
}lv_chart_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* lv_chart_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);
bool lv_chart_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);
lv_charts_t * lv_charts_get(lv_charts_builtin_t style, lv_charts_t * copy_p);

cord_t * lv_chart_add_dataline(lv_obj_t* obj_dp);

void lv_chart_refr(lv_obj_t * obj_dp);

void lv_chart_set_type(lv_obj_t * obj_dp, lv_chart_type_t type);
void lv_chart_set_hvdiv(lv_obj_t * obj_dp, uint8_t hdiv, uint8_t vdiv);
void lv_chart_set_range(lv_obj_t * obj_dp, cord_t ymin, cord_t ymax);
void lv_chart_set_pnum(lv_obj_t * obj_dp, uint16_t pnum);
void lv_chart_set_next(lv_obj_t * obj_dp, cord_t * dl_p, cord_t y);

lv_chart_type_t lv_chart_get_type(lv_obj_t * obj_dp);
uint16_t lv_chart_get_pnum(lv_obj_t * obj_dp);

/**********************
 *      MACROS
 **********************/

#endif

#endif
