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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if USE_LV_LABEL != 0

#include "../lv_core/lv_obj.h"
#include "../lv_misc/lv_font.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_symbol_def.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LABEL_DOT_NUM    3
#define LV_LABEL_POS_LAST   0xFFFF

/**********************
 *      TYPEDEFS
 **********************/

/*Long mode behaviors. Used in 'lv_label_ext_t' */
enum
{
    LV_LABEL_LONG_EXPAND,   /*Expand the object size to the text size*/
    LV_LABEL_LONG_BREAK,    /*Keep the object width, break the too long lines and expand the object height*/
    LV_LABEL_LONG_SCROLL,   /*Expand the object size and scroll the text on the parent (move the label object)*/
    LV_LABEL_LONG_DOT,      /*Keep the size and write dots at the end if the text is too long*/
    LV_LABEL_LONG_ROLL,     /*Keep the size and roll the text infinitely*/
    LV_LABEL_LONG_CROP,     /*Keep the size and crop the text out of it*/
};
typedef uint8_t lv_label_long_mode_t;

/*Label align policy*/
enum {
    LV_LABEL_ALIGN_LEFT,
    LV_LABEL_ALIGN_CENTER,
    LV_LABEL_ALIGN_RIGHT,
};
typedef uint8_t lv_label_align_t;

/*Data of label*/
typedef struct
{
    /*Inherited from 'base_obj' so no inherited ext.*/  /*Ext. of ancestor*/
    /*New data for this type */
    char * text;                            /*Text of the label*/
    lv_label_long_mode_t long_mode;         /*Determinate what to do with the long texts*/
#if LV_TXT_UTF8 == 0
    char dot_tmp[LV_LABEL_DOT_NUM + 1];     /*Store the character which are replaced by dots (Handled by the library)*/
#else
    char dot_tmp[LV_LABEL_DOT_NUM * 4 + 1]; /*Store the character which are replaced by dots (Handled by the library)*/
#endif

#if USE_LV_MULTI_LANG
    uint16_t lang_txt_id;            /*The ID of the text to display*/
#endif
    uint16_t dot_end;               /*The text end position in dot mode (Handled by the library)*/
    uint16_t anim_speed;            /*Speed of scroll and roll animation in px/sec unit*/
    lv_point_t offset;              /*Text draw position offset*/
    uint8_t static_txt  :1;         /*Flag to indicate the text is static*/
    uint8_t align       :2;         /*Align type from 'lv_label_align_t'*/
    uint8_t recolor     :1;         /*Enable in-line letter re-coloring*/
    uint8_t expand      :1;         /*Ignore real width (used by the library with LV_LABEL_LONG_ROLL)*/
    uint8_t body_draw   :1;         /*Draw background body*/
} lv_label_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * Create a label objects
 * @param par pointer to an object, it will be the parent of the new label
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t * lv_label_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

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
void lv_label_set_array_text(lv_obj_t * label, const char * array, uint16_t size);

/**
 * Set a static text. It will not be saved by the label so the 'text' variable
 * has to be 'alive' while the label exist.
 * @param label pointer to a label object
 * @param text pointer to a text. NULL to refresh with the current text.
 */
void lv_label_set_static_text(lv_obj_t * label, const char * text);

/**
 *Set a text ID which means a the same text but on different languages
 * @param label pointer to a label object
 * @param txt_id ID of the text
 */
#if USE_LV_MULTI_LANG
void lv_label_set_text_id(lv_obj_t * label, uint32_t txt_id);
#endif

/**
 * Set the behavior of the label with longer text then the object size
 * @param label pointer to a label object
 * @param long_mode the new mode from 'lv_label_long_mode' enum.
 *                  In LV_LONG_BREAK/LONG/ROLL the size of the label should be set AFTER this function
 */
void lv_label_set_long_mode(lv_obj_t * label, lv_label_long_mode_t long_mode);

/**
 * Set the align of the label (left or center)
 * @param label pointer to a label object
 * @param align 'LV_LABEL_ALIGN_LEFT' or 'LV_LABEL_ALIGN_LEFT'
 */
void lv_label_set_align(lv_obj_t *label, lv_label_align_t align);

/**
 * Enable the recoloring by in-line commands
 * @param label pointer to a label object
 * @param en true: enable recoloring, false: disable
 */
void lv_label_set_recolor(lv_obj_t * label, bool en);

/**
 * Set the label to draw (or not draw) background specified in its style's body
 * @param label pointer to a label object
 * @param en true: draw body; false: don't draw body
 */
void lv_label_set_body_draw(lv_obj_t *label, bool en);

/**
 * Set the label's animation speed in LV_LABEL_LONG_ROLL and SCROLL modes
 * @param label pointer to a label object
 * @param anim_speed speed of animation in px/sec unit
 */
void lv_label_set_anim_speed(lv_obj_t *label, uint16_t anim_speed);

/**
 * Set the style of an label
 * @param label pointer to an label object
 * @param style pointer to a style
 */
static inline void lv_label_set_style(lv_obj_t *label, lv_style_t *style)
{
    lv_obj_set_style(label, style);
}
/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a label
 * @param label pointer to a label object
 * @return the text of the label
 */
char * lv_label_get_text(const lv_obj_t * label);

#if USE_LV_MULTI_LANG
/**
 * Get the text ID of the label. (Used by the multi-language feature)
 * @param label pointer to a label object
 * @return ID of the text
 */
uint16_t lv_label_get_text_id(lv_obj_t * label);
#endif

/**
 * Get the long mode of a label
 * @param label pointer to a label object
 * @return the long mode
 */
lv_label_long_mode_t lv_label_get_long_mode(const lv_obj_t * label);

/**
 * Get the align attribute
 * @param label pointer to a label object
 * @return LV_LABEL_ALIGN_LEFT or LV_LABEL_ALIGN_CENTER
 */
lv_label_align_t lv_label_get_align(const lv_obj_t * label);

/**
 * Get the recoloring attribute
 * @param label pointer to a label object
 * @return true: recoloring is enabled, false: disable
 */
bool lv_label_get_recolor(const lv_obj_t * label);

/**
 * Get the body draw attribute
 * @param label pointer to a label object
 * @return true: draw body; false: don't draw body
 */
bool lv_label_get_body_draw(const lv_obj_t *label);

/**
 * Get the label's animation speed in LV_LABEL_LONG_ROLL and SCROLL modes
 * @param label pointer to a label object
 * @return speed of animation in px/sec unit
 */
uint16_t lv_label_get_anim_speed(const lv_obj_t *label);

/**
 * Get the relative x and y coordinates of a letter
 * @param label pointer to a label object
 * @param index index of the letter [0 ... text length]. Expressed in character index, not byte index (different in UTF-8)
 * @param pos store the result here (E.g. index = 0 gives 0;0 coordinates)
 */
void lv_label_get_letter_pos(const lv_obj_t * label, uint16_t index, lv_point_t * pos);

/**
 * Get the index of letter on a relative point of a label
 * @param label pointer to label object
 * @param pos pointer to point with coordinates on a the label
 * @return the index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter)
 * Expressed in character index and not byte index (different in UTF-8)
 */
uint16_t lv_label_get_letter_on(const lv_obj_t * label, lv_point_t * pos);

/**
 * Get the style of an label object
 * @param label pointer to an label object
 * @return pointer to the label's style
 */
static inline lv_style_t* lv_label_get_style(const lv_obj_t *label)
{
    return lv_obj_get_style(label);
}

/*=====================
 * Other functions
 *====================*/

/**
 * Insert a text to the label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different in UTF-8)
 *            0: before first char.
 *            LV_LABEL_POS_LAST: after last char.
 * @param txt pointer to the text to insert
 */
void lv_label_ins_text(lv_obj_t * label, uint32_t pos,  const char * txt);

/**
 * Delete characters from a label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different in UTF-8)
 *            0: before first char.
 * @param cnt number of characters to cut
 */
void lv_label_cut_text(lv_obj_t * label, uint32_t pos,  uint32_t cnt);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_LABEL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_LABEL_H*/
