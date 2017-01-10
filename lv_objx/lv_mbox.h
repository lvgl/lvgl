/**
 * @file lv_mbox.h
 * 
 */

#ifndef LV_MBOX_H
#define LV_MBOX_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_MBOX != 0

/*Testing of dependencies*/
#if USE_LV_RECT == 0
#error "lv_mbox: lv_rect is required. Enable it in lv_conf.h (USE_LV_RECT  1) "
#endif

#if USE_LV_BTN == 0
#error "lv_mbox: lv_btn is required. Enable it in lv_conf.h (USE_LV_BTN  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_mbox: lv_rlabel is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif


#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
#include "lv_btn.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of message box*/
typedef struct
{
	lv_rects_t bg; /*Style of ancestor*/
	/*New style element for this type */
	lv_labels_t title;
	lv_labels_t txt;
	lv_rects_t btnh;
	lv_btns_t btn;
	lv_labels_t btn_label;
	cord_t btn_w;
	cord_t btn_h;
	uint8_t hide_title	:1;
	uint8_t hide_btns	:1;
	uint8_t btn_fit     :1;
}lv_mboxs_t;

/*Built-in styles of message box*/
typedef enum
{
	LV_MBOXS_DEF,
	LV_MBOXS_INFO,
	LV_MBOXS_WARN,
	LV_MBOXS_ERR,
	LV_MBOXS_BUBBLE,
}lv_mboxs_builtin_t;

/*Data of message box*/
typedef struct
{
	lv_rect_ext_t rect; /*Ext. of ancestor*/
	/*New data for this type */
	lv_obj_t * title;
	lv_obj_t * txt;
	lv_obj_t * btnh;
}lv_mbox_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_mbox_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_mbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param);
lv_mboxs_t * lv_mboxs_get(lv_mboxs_builtin_t style, lv_mboxs_t * copy);

lv_obj_t * lv_mbox_add_btn(lv_obj_t * mbox, const char * btn_txt, lv_action_t rel_action);
bool lv_mbox_close_action (lv_obj_t * mbox, lv_dispi_t *dispi);
void lv_mbox_auto_close(lv_obj_t * mbox, uint16_t tout);
void lv_mbox_set_title(lv_obj_t * mbox, const char * title);
void lv_mbox_set_txt(lv_obj_t * mbox, const char * txt);

const char * lv_mbox_get_title(lv_obj_t * mbox);
const char * lv_mbox_get_txt(lv_obj_t * mbox);
lv_obj_t * lv_mbox_get_from_btn(lv_obj_t * btn);


/**********************
 *      MACROS
 **********************/

#endif

#endif
