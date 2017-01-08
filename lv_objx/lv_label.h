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
#define LV_LABEL_DOT_NUM 3

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


typedef enum
{
	LV_LABEL_LONG_EXPAND,	/*Expand the object size to the text size*/
	LV_LABEL_LONG_BREAK,    /*Keep the width and break the text and expand the object height*/
	LV_LABEL_LONG_DOTS,     /*Keep the size, break the text and write dots in the last line*/
	LV_LABEL_LONG_SCROLL,   /*Expand the object size and scroll the text (move the label object)*/
}lv_label_long_mode_t;

/*Data of label*/
typedef struct
{
	/*Inherited from 'base_obj' so no inherited ext.*/  /*Ext. of ancestor*/
	/*New data for this type */
    char * txt;
    lv_label_long_mode_t long_mode;
    char dot_tmp[LV_LABEL_DOT_NUM];	/*Store character which are replaced with dots*/
    uint16_t dot_end;			/* The text end in dot mode*/
    uint8_t static_txt  :1;     /* Flag to indicate the text is static*/
}lv_label_ext_t;
        
/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*Create function*/
lv_obj_t * lv_label_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_label_signal(lv_obj_t * label, lv_signal_t sign, void * param);
lv_labels_t * lv_labels_get(lv_labels_builtin_t style, lv_labels_t * copy);

void lv_label_set_text(lv_obj_t * label, const char * text);
void lv_label_set_text_array(lv_obj_t * label, const char * array, uint16_t size);
void lv_label_set_text_static(lv_obj_t * label, const char * text);
void lv_label_set_long_mode(lv_obj_t * label, lv_label_long_mode_t long_mode);
const char * lv_label_get_text(lv_obj_t * label);
lv_label_long_mode_t lv_label_get_long_mode(lv_obj_t * label);
void lv_label_get_letter_pos(lv_obj_t * label, uint16_t index, point_t * pos);
uint16_t lv_label_get_letter_on(lv_obj_t * label, point_t * pos);

/**********************
 *      MACROS
 **********************/

#endif

#endif
