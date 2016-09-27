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
#include "lv_label.h"
#include "lv_img.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of LIST*/
typedef struct
{
	lv_pages_t pages; /*Ancestor page style*/
	lv_btns_t liste_btns;
	lv_labels_t liste_labels;
	lv_imgs_t liste_imgs;
	lv_rect_layout_t liste_layout;
}lv_lists_t;

/*Built-in styles of LISTATE*/
typedef enum
{
	LV_LISTS_DEF,
}lv_lists_builtin_t;

typedef enum
{
	LV_LIST_FIT_CONTENT,
	LV_LIST_FIT_HOLDER,
	LV_LIST_FIT_LONGEST,
}lv_list_fit_t;


/*Data of LIST*/
typedef struct
{
	lv_page_ext_t page_ext;
	uint8_t fit;	/*Width adjustment of list elements (from lv_list_adjsut_t)*/
}lv_list_ext_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* lv_list_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);
bool lv_list_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);
lv_obj_t * lv_list_add(lv_obj_t * obj_dp, const char * img_fn, const char * txt, bool (*rel_action)(lv_obj_t*, lv_dispi_t *));
lv_lists_t * lv_lists_get(lv_lists_builtin_t style, lv_lists_t * copy_p);

void lv_list_down(lv_obj_t * obj_dp);
void lv_list_up(lv_obj_t * obj_dp);
void lv_list_set_fit(lv_obj_t * obj_dp, lv_list_fit_t fit);
lv_list_fit_t lv_list_get_fit(lv_obj_t * obj_dp);

/**********************
 *      MACROS
 **********************/

#endif

#endif
