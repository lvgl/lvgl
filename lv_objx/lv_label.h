/**
 * @file lv_rect.h
 * 
 */

#ifndef LV_LABEL_H
#define LV_LABEL_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_LABEL != 0

#include "../lv_obj/lv_obj.h"
#include "../lv_misc/font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of label*/
typedef struct
{
	lv_objs_t objs;		/*Style of ancestor*/
	/*New style element for this type */
    font_types_t font;
    uint16_t letter_space;
    uint16_t line_space;
    uint8_t mid         :1;
}lv_labels_t;

/*Built-in styles of label*/
typedef enum
{
	LV_LABELS_DEF,
	LV_LABELS_BTN,
	LV_LABELS_TXT,
	LV_LABELS_TITLE,
}lv_labels_builtin_t;

/*Data of label*/
typedef struct
{
	/*Inherited from 'base_obj' so no inherited ext.*/  /*Ext. of ancestor*/
	/*New data for this type */
    char * txt;
    uint8_t fixw  :1;
}lv_label_ext_t;
        
/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*Create function*/
lv_obj_t * lv_label_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_label_signal(lv_obj_t * label, lv_signal_t sign, void * param);
lv_labels_t * lv_labels_get(lv_labels_builtin_t style, lv_labels_t * copy);

void lv_label_set_text(lv_obj_t * label, const char * text);
void lv_label_set_fixw(lv_obj_t * label, bool fixw);
const char * lv_label_get_text(lv_obj_t * label);
bool lv_label_get_fixw(lv_obj_t * label);
void lv_label_get_letter_pos(lv_obj_t * label, uint16_t index, point_t * pos);
uint16_t lv_label_get_letter_on(lv_obj_t * label, point_t * pos);

/**********************
 *      MACROS
 **********************/

#endif

#endif
