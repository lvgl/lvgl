/**
 * @file lv_cont.h
 * 
 */

#ifndef LV_CONT_H
#define LV_CONT_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_CONT != 0

#include "../lv_obj/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Layout options*/
typedef enum
{
	LV_CONT_LAYOUT_OFF = 0,
	LV_CONT_LAYOUT_CENTER,
	LV_CONT_LAYOUT_COL_L,	/*Column left align*/
	LV_CONT_LAYOUT_COL_M,	/*Column middle align*/
	LV_CONT_LAYOUT_COL_R,	/*Column right align*/
	LV_CONT_LAYOUT_ROW_T,	/*Row top align*/
	LV_CONT_LAYOUT_ROW_M,	/*Row middle align*/
	LV_CONT_LAYOUT_ROW_B,	/*Row bottom align*/
	LV_CONT_LAYOUT_PRETTY,	/*Put as many object as possible in row and begin a new row*/
	LV_CONT_LAYOUT_GRID,	/*Align same-sized object into a grid*/
}lv_cont_layout_t;

typedef struct
{
    /*Inherited from 'base_obj' so no inherited ext. */ /*Ext. of ancestor*/
    /*New data for this type */
    uint8_t layout  :5;     /*A layout from 'lv_cont_layout_t' enum*/
    uint8_t hfit_en :1;     /*Enable horizontal padding to involve all children*/
    uint8_t vfit_en :1;     /*Enable horizontal padding to involve all children*/
}lv_cont_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a container objects
 * @param par pointer to an object, it will be the parent of the new container
 * @param copy pointer to a container object, if not NULL then the new object will be copied from it
 * @return pointer to the created container
 */
lv_obj_t * lv_cont_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the container
 * @param cont pointer to a container object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_cont_signal(lv_obj_t * cont, lv_signal_t sign, void * param);

/**
 * Set the layout on a container
 * @param cont pointer to a container object
 * @param layout a layout from 'lv_cont_layout_t'
 */
void lv_cont_set_layout(lv_obj_t * cont, lv_cont_layout_t layout);

/**
 * Enable the horizontal or vertical fit.
 * The container size will be set to involve the children horizontally or vertically.
 * @param cont pointer to a container object
 * @param hor_en true: enable the horizontal padding
 * @param ver_en true: enable the vertical padding
 */
void lv_cont_set_fit(lv_obj_t * cont, bool hor_en, bool ver_en);

/**
 * Get the layout of a container
 * @param cont pointer to container object
 * @return the layout from 'lv_cont_layout_t'
 */
lv_cont_layout_t lv_cont_get_layout(lv_obj_t * cont);

/**
 * Get horizontal fit enable attribute of a container
 * @param cont pointer to a container object
 * @return true: horizontal padding is enabled
 */
bool lv_cont_get_hfit(lv_obj_t * cont);

/**
 * Get vertical fit enable attribute of a container
 * @param cont pointer to a container object
 * @return true: vertical padding is enabled
 */
bool lv_cont_get_vfit(lv_obj_t * cont);

/**********************
 *      MACROS
 **********************/

#endif

#endif
