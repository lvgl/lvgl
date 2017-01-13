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

/**
 * Create a label objects
 * @param par pointer to an object, it will be the parent of the new label
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t * lv_label_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the label
 * @param label pointer to a label object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_label_signal(lv_obj_t * label, lv_signal_t sign, void * param);

/**
 * Set a new text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param text '\0' terminated character string. NULL to refresh with the current text.
 */
void lv_label_set_text(lv_obj_t * label, const char * text);

/**
 * Set a new text for a label from a character array. The array don't has to be '\0' terminated.
 * Memory will be allocated to store the array by the label.
 * @param label pointer to a label object
 * @param array array of characters or NULL to refresh the label
 * @param size the size of 'array' in bytes
 */
void lv_label_set_text_array(lv_obj_t * label, const char * array, uint16_t size);

/**
 * Set a static text. It will not be saved by the label so the 'text' variable
 * has to be 'alive' while the label exist.
 * @param label pointer to a label object
 * @param text pointer to a text. NULL to refresh with the current text.
 */
void lv_label_set_text_static(lv_obj_t * label, const char * text);

/**
 * Set the behavior of the label with longer text then the object size
 * @param label pointer to a label object
 * @param long_mode the new mode from 'lv_label_long_mode' enum.
 */
void lv_label_set_long_mode(lv_obj_t * label, lv_label_long_mode_t long_mode);

/**
 * Get the text of a label
 * @param label pointer to a label object
 * @return the text of the label
 */
const char * lv_label_get_text(lv_obj_t * label);

/**
 * Get the fix width attribute of a label
 * @param label pointer to a label object
 * @return true: fix width is enabled
 */
lv_label_long_mode_t lv_label_get_long_mode(lv_obj_t * label);

/**
 * Get the relative x and y coordinates of a letter
 * @param label pointer to a label object
 * @param index index of the letter (0 ... text length)
 * @param pos store the result here (E.g. index = 0 gives 0;0 coordinates)
 */
void lv_label_get_letter_pos(lv_obj_t * label, uint16_t index, point_t * pos);

/**
 * Get the index of letter on a relative point of a label
 * @param label pointer to label object
 * @param pos pointer to point with coordinates on a the label
 * @return the index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter)
 */
uint16_t lv_label_get_letter_on(lv_obj_t * label, point_t * pos);

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_labels_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_labels_t style
 */
lv_labels_t * lv_labels_get(lv_labels_builtin_t style, lv_labels_t * copy);

/**********************
 *      MACROS
 **********************/

#endif

#endif
