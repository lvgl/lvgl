/**
 * @file lv_tab.h
 * 
 */

#ifndef LV_TAB_H
#define LV_TAB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_TAB != 0

#include "../lv_obj/lv_obj.h"
#include "../lv_objx/lv_win.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of tab*/
typedef struct
{
   /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * tabs;
    lv_obj_t * indic;
    lv_obj_t * content;   /*A rectangle to show the current tab*/
    const char ** tab_name_ptr;
    point_t point_last;
    uint16_t tab_act;
    uint16_t tab_num;
    uint8_t draging :1;
    uint8_t drag_h :1;

}lv_tab_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a tab objects
 * @param par pointer to an object, it will be the parent of the new tab
 * @param copy pointer to a tab object, if not NULL then the new object will be copied from it
 * @return pointer to the created tab
 */
lv_obj_t * lv_tab_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the tab
 * @param tab pointer to a tab object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_tab_signal(lv_obj_t * tab, lv_signal_t sign, void * param);

lv_obj_t * lv_tab_add(lv_obj_t * tab, const char * name);


lv_obj_t * lv_tab_add_scrollable(lv_obj_t * tab, const char * name);



/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_TAB*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_TAB_H*/
