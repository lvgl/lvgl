/**
 * @file lv_rect.h
 * 
 */

#ifndef LV_RECT_H
#define LV_RECT_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_RECT != 0

#include "../lv_obj/lv_obj.h"
#include "../lv_obj/lv_dispi.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum
{
	LV_LAYOUT_OFF = 0,
	LV_LAYOUT_COL_L,
	LV_LAYOUT_COL_M,
	LV_LAYOUT_COL_R,
	LV_LAYOUT_ROW_T,
	LV_LAYOUT_ROW_M,
	LV_LAYOUT_ROW_B,
	LV_LAYOUT_CENTER,
}lv_layout_t;

typedef struct
{
	lv_objs_t objs;
    color_t gcolor;
    color_t bcolor;
    uint16_t bwidth;
    cord_t hpad;
    cord_t vpad;
    cord_t opad;
    uint16_t round;
    uint8_t bopa;
    uint8_t empty :1;
}lv_rects_t;


typedef struct
{
	uint8_t hfit_en:1;
	uint8_t vfit_en:1;
    lv_layout_t layout;
}lv_rect_ext_t;

typedef enum
{
	LV_RECTS_DEF,
	LV_RECTS_TRANSP,
	LV_RECTS_BORDER,
}lv_rects_builtin_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*Create function*/
lv_obj_t* lv_rect_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);
bool lv_rect_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);

void lv_rect_set_fit(lv_obj_t * obj_dp, bool hor_en, bool ver_en);
void lv_rect_set_layout(lv_obj_t * obj_dp, lv_layout_t layout);

lv_layout_t lv_rect_get_layout(lv_obj_t * obj_dp);
bool lv_rect_get_hpad_en(lv_obj_t * obj_dp);
bool lv_rect_get_vpad_en(lv_obj_t * obj_dp);

lv_rects_t * lv_rects_get(lv_rects_builtin_t style, lv_rects_t * copy_p);

/**********************
 *      MACROS
 **********************/

#endif

#endif
