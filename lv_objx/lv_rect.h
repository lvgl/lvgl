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

/*Layout options*/
typedef enum
{
	LV_RECT_LAYOUT_OFF = 0,
	LV_RECT_LAYOUT_CENTER,
	LV_RECT_LAYOUT_COL_L,	/*Column left align*/
	LV_RECT_LAYOUT_COL_M,	/*Column middle align*/
	LV_RECT_LAYOUT_COL_R,	/*Column right align*/
	LV_RECT_LAYOUT_ROW_T,	/*Row left align*/
	LV_RECT_LAYOUT_ROW_M,	/*Row middle align*/
	LV_RECT_LAYOUT_ROW_B,	/*Row right align*/
	LV_RECT_LAYOUT_PRETTY,	/*Put as many object as possible in row and begin a new row*/
	LV_RECT_LAYOUT_GRID,	/*Align same-sized object into a grid*/
}lv_rect_layout_t;

typedef struct
{
    /*Inherited from 'base_obj' so no inherited ext. */ /*Ext. of ancestor*/
    /*New data for this type */
    uint8_t layout  :5;     /*Set a layout from 'lv_rect_layout_t' enum*/
    uint8_t hpad_en :1;     /*Enable horizontal padding according to the children*/
    uint8_t vpad_en :1;     /*Enable horizontal padding according to the children*/
}lv_rect_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a rectangle objects
 * @param par pointer to an object, it will be the parent of the new rectangle
 * @param copy pointer to a rectangle object, if not NULL then the new object will be copied from it
 * @return pointer to the created rectangle
 */
lv_obj_t * lv_rect_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the rectangle
 * @param rect pointer to a rectangle object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_rect_signal(lv_obj_t * rect, lv_signal_t sign, void * param);

/**
 * Set the layout on a rectangle
 * @param rect pointer to a rectangle object
 * @param layout a layout from 'lv_rect_layout_t'
 */
void lv_rect_set_layout(lv_obj_t * rect, lv_rect_layout_t layout);

/**
 * Enable the horizontal or vertical fit.
 * The rectangle size will be set to involve the children horizontally or vertically.
 * @param rect pointer to a rectangle object
 * @param hor_en true: enable the horizontal padding
 * @param ver_en true: enable the vertical padding
 */
void lv_rect_set_fit(lv_obj_t * rect, bool hor_en, bool ver_en);

/**
 * Get the layout of a rectangle
 * @param rect pointer to rectangle object
 * @return the layout from 'lv_rect_layout_t'
 */
lv_rect_layout_t lv_rect_get_layout(lv_obj_t * rect);

/**
 * Get horizontal fit enable attribute of a rectangle
 * @param rect pointer to a rectangle object
 * @return true: horizontal padding is enabled
 */
bool lv_rect_get_hfit(lv_obj_t * rect);

/**
 * Get vertical fit enable attribute of a rectangle
 * @param obj pointer to a rectangle object
 * @return true: vertical padding is enabled
 */
bool lv_rect_get_vfit(lv_obj_t * rect);


/**********************
 *      MACROS
 **********************/

#endif

#endif
