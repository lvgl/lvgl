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
#include "../lv_misc/text.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LABEL_DOT_NUM 3

/**********************
 *      TYPEDEFS
 **********************/

/*Long mode behaviors. Used in 'lv_label_ext_t' */
typedef enum
{
    LV_LABEL_LONG_EXPAND,   /*Expand the object size to the text size*/
    LV_LABEL_LONG_BREAK,    /*Keep the width and break the text and expand the object height*/
    LV_LABEL_LONG_DOTS,     /*Keep the size, break the text and write dots in the last line*/
    LV_LABEL_LONG_SCROLL,   /*Expand the object size and scroll the text on the parent (move the label object)*/
}lv_label_long_mode_t;

/*Data of label*/
typedef struct
{
    /*Inherited from 'base_obj' so no inherited ext.*/  /*Ext. of ancestor*/
    /*New data for this type */
    char * txt;                     /*Text of the label*/
    lv_label_long_mode_t long_mode; /*Determinate what to do with the long texts*/
    char dot_tmp[LV_LABEL_DOT_NUM]; /*Store character which are replaced with dots*/
    uint16_t dot_end;               /*The text end position in dot mode*/
    uint8_t static_txt  :1;         /*Flag to indicate the text is static*/
    uint8_t recolor  :1;            /*Enable in-line letter re-coloring*/
}lv_label_ext_t;

/*Style of label*/
typedef struct
{
	lv_objs_t base;		/*Style of ancestor*/
	/*New style element for this type */
    const font_t * font;        /*Pointer to a font*/
    cord_t letter_space;        /*Letter space in px*/
    cord_t line_space;          /*Line space in px*/
    uint8_t mid:1;              /*1: Align the lines into the middle*/
}lv_labels_t;

/*Built-in styles of label*/
typedef enum
{
    LV_LABELS_TXT,          /*General text style*/
	LV_LABELS_TITLE,        /*Like text style but greater spaces*/
	LV_LABELS_BTN,          /*Mid. aligned style for buttons*/
}lv_labels_builtin_t;

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
 * Append a text to the label. The label current label text can not be static.
 * @param label pointer to label object
 * @param text pointe rto the new text
 */
void lv_label_append_text(lv_obj_t * label, const char * text);
/**
 * Set the behavior of the label with longer text then the object size
 * @param label pointer to a label object
 * @param long_mode the new mode from 'lv_label_long_mode' enum.
 */
void lv_label_set_long_mode(lv_obj_t * label, lv_label_long_mode_t long_mode);

/**
 * Enable the recoloring by in-line commands
 * @param label pointer to a label object
 * @param recolor true: enable recoloring, false: disable
 */
void lv_label_set_recolor(lv_obj_t * label, bool recolor);

/**
 * Get the text of a label
 * @param label pointer to a label object
 * @return the text of the label
 */
const char * lv_label_get_text(lv_obj_t * label);

/**
 * Get the long mode of a label
 * @param label pointer to a label object
 * @return the long mode
 */
lv_label_long_mode_t lv_label_get_long_mode(lv_obj_t * label);


/**
 * Get the recoloring attribute
 * @param label pointer to a label object
 * @return true: recoloring is enabled, false: disable
 */
bool lv_label_get_recolor(lv_obj_t * label);

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
