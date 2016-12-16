/**
 * @file lv_win.h
 * 
 */

#ifndef LV_WIN_H
#define LV_WIN_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_WIN != 0

#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
#include "lv_btn.h"
#include "lv_label.h"
#include "lv_img.h"
#include "lv_page.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of window*/
typedef struct
{
	lv_objs_t bg; /*Style of ancestor*/
	/*New style element for this type */
	/*Header settings*/
	lv_rects_t header;
	lv_labels_t title;
	lv_rects_t ctrl_holder;
	lv_btns_t ctrl_btn;
	lv_imgs_t ctrl_img;
	cord_t ctrl_btn_w;
	cord_t ctrl_btn_h;
	/*Content settings*/
	lv_pages_t content;
}lv_wins_t;

/*Built-in styles of window*/
typedef enum
{
	LV_WINS_DEF,
}lv_wins_builtin_t;

/*Data of window*/
typedef struct
{
	/*Inherited from 'base_obj' so there is no ext. data*/ /*Ext. of ancestor*/
	/*New data for this type */
	lv_obj_t * header;
	lv_obj_t * title;
	lv_obj_t * ctrl_holder;
	lv_obj_t * content;
}lv_win_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_win_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_win_signal(lv_obj_t * win, lv_signal_t sign, void * param);
lv_wins_t * lv_wins_get(lv_wins_builtin_t style, lv_wins_t * copy);

lv_obj_t * lv_win_add_ctrl_btn(lv_obj_t * win, const char * img, bool (*rel_action)(lv_obj_t *, lv_dispi_t *));
bool lv_win_close_action(lv_obj_t * btn, lv_dispi_t * dispi);
void lv_win_set_title(lv_obj_t * win, const char * title);

const char * lv_win_get_title(lv_obj_t * win);
lv_obj_t * lv_win_get_from_ctrl_btn(lv_obj_t * ctrl_btn);
/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_WIN*/

#endif /*LV_WIN_H*/
