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

#include "../lv_obj/lv_obj.h"
#include "lv_page.h"
#include "lv_btn.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of LIST*/
typedef struct
{
	/*Ancestor page style*/
	lv_pages_t pages;
	/*List style*/
	lv_layout_t list_layout;
	cord_t list_layout_space;
	/*List element style*/
	lv_btns_t liste_btns;
	lv_layout_t liste_layout;
	cord_t liste_layout_space;
}lv_lists_t;


/*Built-in styles of LISTATE*/
typedef enum
{
	LV_LISTS_DEF,
}lv_lists_builtin_t;

/*Data of LIST*/
typedef struct
{
	lv_page_ext_t page_ext;
	uint8_t fit_size :1;	/*Automatically set the size of list elements to the holder */
	uint8_t sel_en   :1;	/*Enable selecting list elements by toggling them */
	uint8_t sel_one  :1;	/*Enable to select only one list element*/
}lv_list_ext_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* lv_list_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);
bool lv_list_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);
void lv_list_add(lv_obj_t * obj_dp, const char * img_fn, const char * txt, void (*release) (lv_obj_t *));
lv_lists_t * lv_lists_get(lv_lists_builtin_t style, lv_lists_t * copy_p);

/**********************
 *      MACROS
 **********************/

#endif

#endif
