/**
 * @file lv_rect.h
 * 
 */

#ifndef LV_LABEL_H
#define LV_LABEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_LABEL != 0

#include "../lv_obj/lv_obj.h"
#include "misc/gfx/font.h"
#include "misc/gfx/text.h"
#include "misc/gfx/fonts/symbol_def.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LABEL_DOT_NUM 3
#define LV_LABEL_POS_LAST   0xFFFF
/**********************
 *      TYPEDEFS
 **********************/

/*Long mode behaviors. Used in 'lv_label_ext_t' */
typedef enum
{
    LV_LABEL_LONG_EXPAND,   /*Expand the object size to the text size*/
    LV_LABEL_LONG_BREAK,    /*Keep the object width, break the too long lines and expand the object height*/
    LV_LABEL_LONG_SCROLL,   /*Expand the object size and scroll the text on the parent (move the label object)*/
    LV_LABEL_LONG_ROLL,     /*Keep the size and roll the text infinitely*/
}lv_label_long_mode_t;

/*Data of label*/
typedef struct
{
    /*Inherited from 'base_obj' so no inherited ext.*/  /*Ext. of ancestor*/
    /*New data for this type */
    char * txt;                     /*Text of the label*/
    lv_label_long_mode_t long_mode; /*Determinate what to do with the long texts*/
#if TXT_UTF8 == 0
    char dot_tmp[LV_LABEL_DOT_NUM + 1]; /*Store the character which are replaced by dots (Handled by the library)*/
#else
    uint32_t dot_tmp[LV_LABEL_DOT_NUM + 1]; /*Store the character which are replaced by dots (Handled by the library)*/
#endif
    uint16_t dot_end;               /*The text end position in dot mode (Handled by the library)*/
    point_t offset;                 /*Text draw position offset*/
    uint8_t static_txt  :1;         /*Flag to indicate the text is static*/
    uint8_t recolor  :1;            /*Enable in-line letter re-coloring*/
    uint8_t expand  :1;             /*Ignore real width (used by the library with LV_LABEL_LONG_ROLL)*/
    uint8_t no_break  :1;           /*Ignore new line characters*/
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
 * Insert a text to the label. The label current label text can not be static.
 * @param label pointer to label object
 * @param pos character index to insert
 *            0: before first char.
 *            LV_LABEL_POS_LAST: after last char.
 *            < 0: count from the end
 *            -1: before the last char.
 * @param txt pointer to the text to insert
 */
void lv_label_ins_text(lv_obj_t * label, uint32_t pos,  const char * txt);

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
 * Set the label the ignore (or accept) line breaks on '\n'
 * @param label pointer to a label object
 * @param en true: ignore line breaks, false: make line breaks on '\n'
 */
void lv_label_set_no_break(lv_obj_t * label, bool en);

/**
 * Get the text of a label
 * @param label pointer to a label object
 * @return the text of the label
 */
char * lv_label_get_text(lv_obj_t * label);

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
 * Get the password mode
 * @param label pointer to a label object
 * @return true: password mode is enabled, false: disable
 */
bool lv_label_get_pwd_mode(lv_obj_t * label);

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

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_LABEL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_LABEL_H*/
