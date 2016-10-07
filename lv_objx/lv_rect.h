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
#define LV_RECT_CIRCLE		0xFFFF	/*A very big radius to always draw as circle*/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum
{
	LV_RECT_LAYOUT_OFF = 0,
	LV_RECT_LAYOUT_CENTER,
	LV_RECT_LAYOUT_COL_L,	/*Column left align*/
	LV_RECT_LAYOUT_COL_M,	/*Column middle align*/
	LV_RECT_LAYOUT_COL_R,	/*Column right align*/
	LV_RECT_LAYOUT_ROW_T,	/*Row row left align*/
	LV_RECT_LAYOUT_ROW_M,	/*Row row middle align*/
	LV_RECT_LAYOUT_ROW_B,	/*Row row right align*/
	LV_RECT_LAYOUT_GRID,
}lv_rect_layout_t;

/*Style of template*/
typedef struct
{
	lv_objs_t objs;	/*Style of ancestor*/
	/*New style element for this type */
    color_t gcolor;
    color_t bcolor;
    uint16_t bwidth;
    uint16_t round;
    cord_t hpad;
    cord_t vpad;
    cord_t opad;
    uint8_t bopa;
    uint8_t empty :1;
}lv_rects_t;

/*Built-in styles of template*/
typedef enum
{
	LV_RECTS_DEF,
	LV_RECTS_TRANSP,
	LV_RECTS_BORDER,
}lv_rects_builtin_t;


typedef struct
{
	/*Inherited from 'base_obj' so no inherited ext. */ /*Ext. of ancestor*/
	/*New data for this type */
    uint8_t layout  :5;
	uint8_t hfit_en :1;
	uint8_t vfit_en :1;
}lv_rect_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*Create function*/
lv_obj_t * lv_rect_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_rect_signal(lv_obj_t * rect, lv_signal_t sign, void * param);

void lv_rect_set_fit(lv_obj_t * rect, bool hor_en, bool ver_en);
void lv_rect_set_layout(lv_obj_t * rect, lv_rect_layout_t layout);

lv_rect_layout_t lv_rect_get_layout(lv_obj_t * rect);
bool lv_rect_get_hfit(lv_obj_t * rect);
bool lv_rect_get_vfit(lv_obj_t * rect);
lv_rects_t * lv_rects_get(lv_rects_builtin_t style, lv_rects_t * copy);

/**********************
 *      MACROS
 **********************/

#endif

#endif
