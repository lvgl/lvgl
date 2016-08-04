/**
 * @file lv_graphbg.h
 *
 */

#ifndef LV_GRAPHBG_H
#define LV_GRAPHBG_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_GRAPHBG != 0

#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
#include "lv_line.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of graph. background*/
typedef struct
{
	lv_rects_t bg_rects;
	lv_lines_t div_lines;
	uint8_t div_line_opa;		/*Percentage of obj. opacity*/
}lv_graphbgs_t;

/*Built-in styles of graph. background*/
typedef enum
{
	LV_GRAPHBGS_DEF,
}lv_graphbgs_builtin_t;

/*Data of graph. background*/
typedef struct
{
	lv_rect_ext_t rect_ext;
	int32_t xmin;
	int32_t xmax;
	int32_t ymin;
	int32_t ymax;
	uint8_t hdiv_num;
	uint8_t vdiv_num;
}lv_graphbg_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* lv_graphbg_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);
bool lv_graphbg_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);
lv_graphbgs_t * lv_graphbgs_get(lv_graphbgs_builtin_t style, lv_graphbgs_t * copy_p);

/**********************
 *      MACROS
 **********************/

#endif

#endif
