/**
 * @file lv_list.h
 * 
 */

#ifndef LV_LIST_H
#define LV_LIST_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_LIST != 0

/*Testing of dependencies*/
#if USE_LV_BTN == 0
#error "lv_list: lv_btn is required. Enable it in lv_conf.h (USE_LV_BTN  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_list: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

#if USE_LV_IMG == 0
#error "lv_list: lv_img is required. Enable it in lv_conf.h (USE_LV_IMG  1) "
#endif

#include "../lv_obj/lv_obj.h"
#include "lv_page.h"
#include "lv_btn.h"
#include "lv_label.h"
#include "lv_img.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of list*/
typedef struct
{
    lv_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */
    lv_style_t * styles_liste[LV_BTN_STATE_NUM];    /*Styles of the list element buttons*/
    uint8_t width_sb   :1;        /*1: Keep space for the scrollbar*/
}lv_list_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a list objects
 * @param par pointer to an object, it will be the parent of the new list
 * @param copy pointer to a list object, if not NULL then the new object will be copied from it
 * @return pointer to the created list
 */
lv_obj_t * lv_list_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the list
 * @param list pointer to a list object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_list_signal(lv_obj_t * list, lv_signal_t sign, void * param);

/**
 * Add a list element to the list
 * @param list pointer to list object
 * @param img_fn file name of an image before the text (NULL if unused)
 * @param txt text of the list element (NULL if unused)
 * @param rel_action pointer to release action function (like with lv_btn)
 * @return pointer to the new list element which can be customized (a button)
 */
lv_obj_t * lv_list_add(lv_obj_t * list, const char * img_fn, const char * txt, lv_action_t rel_action);

/**
 * Move the list elements up by one
 * @param list pointer a to list object
 */
void lv_list_up(lv_obj_t * list);

/**
 * Move the list elements down by one
 * @param list pointer to a list object
 */
void lv_list_down(lv_obj_t * list);

void lv_list_set_styles_liste(lv_obj_t * list, lv_style_t * rel, lv_style_t * pr, lv_style_t * trel, lv_style_t * tpr, lv_style_t * ina);
/**
 * Get the text of a list element
 * @param liste pointer to list element
 * @return pointer to the text
 */
const char * lv_list_element_get_txt(lv_obj_t * liste);

lv_style_t * lv_list_get_style_liste(lv_obj_t * list, lv_btn_state_t state);
/**********************
 *      MACROS
 **********************/

#endif

#endif
