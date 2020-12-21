/**
 * @file lv_label.h
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
#include "../lv_conf_internal.h"

#if LV_USE_LABEL != 0

#include <stdarg.h>
#include "../lv_core/lv_obj.h"
#include "../lv_font/lv_font.h"
#include "../lv_font/lv_symbol_def.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LABEL_DOT_NUM 3
#define LV_LABEL_POS_LAST 0xFFFF
#define LV_LABEL_TEXT_SEL_OFF LV_DRAW_LABEL_NO_TXT_SEL

LV_EXPORT_CONST_INT(LV_LABEL_DOT_NUM);
LV_EXPORT_CONST_INT(LV_LABEL_POS_LAST);
LV_EXPORT_CONST_INT(LV_LABEL_TEXT_SEL_OFF);

/**********************
 *      TYPEDEFS
 **********************/

/** Long mode behaviors. Used in 'lv_label_ext_t' */
enum {
    LV_LABEL_LONG_EXPAND,      /**< Expand the object size to the text size*/
    LV_LABEL_LONG_WRAP,        /**< Keep the object width, wrap the too long lines and expand the object height*/
    LV_LABEL_LONG_DOT,         /**< Keep the size and write dots at the end if the text is too long*/
    LV_LABEL_LONG_SROLL,       /**< Keep the size and roll the text back and forth*/
    LV_LABEL_LONG_SROLL_CIRC,  /**< Keep the size and roll the text circularly*/
    LV_LABEL_LONG_CLIP,        /**< Keep the size and clip the text out of it*/
};
typedef uint8_t lv_label_long_mode_t;

/** Label align policy*/
enum {
    LV_LABEL_ALIGN_LEFT, /**< Align text to left */
    LV_LABEL_ALIGN_CENTER, /**< Align text to center */
    LV_LABEL_ALIGN_RIGHT, /**< Align text to right */
    LV_LABEL_ALIGN_AUTO, /**< Use LEFT or RIGHT depending on the direction of the text (LTR/RTL)*/
};
typedef uint8_t lv_label_align_t;


LV_CLASS_DECLARE_START(lv_label, lv_obj);

#define _lv_label_constructor   void (*constructor)(struct _lv_obj_t * obj, struct _lv_obj_t * parent, const struct _lv_obj_t * copy)

#if LV_USE_ANIMATION
#define LV_LABEL_ANIM_DATA uint32_t anim_speed;
#else
#define LV_LABEL_ANIM_DATA
#endif

#if LV_LABEL_LONG_TXT_HINT
#define LV_LABEL_HINT_DATA lv_draw_label_hint_t hint;
#else
#define LV_LABEL_HINT_DATA
#endif

#if LV_LABEL_TEXT_SEL
#define LV_LABEL_TEXT_SEL_DATA  uint32_t sel_start; uint32_t sel_end;
#else
#define LV_LABEL_TEXT_SEL_DATA
#endif


#define _lv_label_data             \
  _lv_obj_data                     \
    char * text;                   \
    union {                        \
        char * tmp_ptr; /* Pointer to the allocated memory containing the character replaced by dots*/ \
        char tmp[LV_LABEL_DOT_NUM + 1]; /* Directly store the characters if <=4 characters */          \
    } dot;                                                                                             \
    uint32_t dot_end;  /*The real text length, used in dot mode*/               \
    LV_LABEL_ANIM_DATA /*Speed of scroll and roll animation in px/sec unit*/  \
    LV_LABEL_HINT_DATA /*Used to buffer info about large text*/ \
    LV_LABEL_TEXT_SEL_DATA                                        \
    lv_point_t offset; /*Text draw position offset*/            \
    lv_label_long_mode_t long_mode : 3; /*Determinate what to do with the long texts*/  \
    uint8_t static_txt : 1;             /*Flag to indicate the text is static*/         \
    uint8_t align : 2;                  /*Align type from 'lv_label_align_t'*/          \
    uint8_t recolor : 1;                /*Enable in-line letter re-coloring*/           \
    uint8_t expand : 1;                 /*Ignore real width (used by the library with LV_LABEL_LONG_SROLL)*/    \
    uint8_t dot_tmp_alloc : 1; /*1: dot_tmp has been allocated;.0: dot_tmp directly holds up to 4 bytes of characters */               \

#define _lv_label_class_dsc        \
  _lv_obj_class_dsc

LV_CLASS_DECLARE_END(lv_label, lv_obj);

extern lv_label_class_t lv_label;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a label objects
 * @param parent pointer to an object, it will be the parent of the new label
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other label to copy.
 * @return pointer to the created button
 */
lv_obj_t * lv_label_create(lv_obj_t * parent, const lv_obj_t * copy);

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
 * Set a new formatted text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param fmt `printf`-like format
 */
void lv_label_set_text_fmt(lv_obj_t * label, const char * fmt, ...);

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
 *                  In LV_LONG_BREAK/LONG/ROLL the size of the label should be set AFTER this
 * function
 */
void lv_label_set_long_mode(lv_obj_t * label, lv_label_long_mode_t long_mode);

/**
 * Set the align of the label (left or center)
 * @param label pointer to a label object
 * @param align 'LV_LABEL_ALIGN_LEFT' or 'LV_LABEL_ALIGN_LEFT'
 */
void lv_label_set_align(lv_obj_t * label, lv_label_align_t align);

/**
 * Enable the recoloring by in-line commands
 * @param label pointer to a label object
 * @param en true: enable recoloring, false: disable
 */
void lv_label_set_recolor(lv_obj_t * label, bool en);

/**
 * Set the label's animation speed in LV_LABEL_LONG_SROLL/SROLL_CIRC modes
 * @param label pointer to a label object
 * @param anim_speed speed of animation in px/sec unit
 */
void lv_label_set_anim_speed(lv_obj_t * label, uint16_t anim_speed);

/**
 * @brief Set the selection start index.
 * @param label pointer to a label object.
 * @param index index to set. `LV_LABEL_TXT_SEL_OFF` to select nothing.
 */
void lv_label_set_text_sel_start(lv_obj_t * label, uint32_t index);

/**
 * @brief Set the selection end index.
 * @param label pointer to a label object.
 * @param index index to set. `LV_LABEL_TXT_SEL_OFF` to select nothing.
 */
void lv_label_set_text_sel_end(lv_obj_t * label, uint32_t index);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a label
 * @param label pointer to a label object
 * @return the text of the label
 */
char * lv_label_get_text(const lv_obj_t * label);

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
 * Get the label's animation speed in LV_LABEL_LONG_ROLL and SCROLL modes
 * @param label pointer to a label object
 * @return speed of animation in px/sec unit
 */
uint16_t lv_label_get_anim_speed(const lv_obj_t * label);

/**
 * Get the relative x and y coordinates of a letter
 * @param label pointer to a label object
 * @param index index of the letter [0 ... text length]. Expressed in character index, not byte
 * index (different in UTF-8)
 * @param pos store the result here (E.g. index = 0 gives 0;0 coordinates)
 */
void lv_label_get_letter_pos(const lv_obj_t * label, uint32_t index, lv_point_t * pos);

/**
 * Get the index of letter on a relative point of a label
 * @param label pointer to label object
 * @param pos pointer to point with coordinates on a the label
 * @return the index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter)
 * Expressed in character index and not byte index (different in UTF-8)
 */
uint32_t lv_label_get_letter_on(const lv_obj_t * label, lv_point_t * pos);

/**
 * Check if a character is drawn under a point.
 * @param label Label object
 * @param pos Point to check for character under
 * @return whether a character is drawn under the point
 */
bool lv_label_is_char_under_pos(const lv_obj_t * label, lv_point_t * pos);

/**
 * @brief Get the selection start index.
 * @param label pointer to a label object.
 * @return selection start index. `LV_LABEL_TXT_SEL_OFF` if nothing is selected.
 */
uint32_t lv_label_get_text_sel_start(const lv_obj_t * label);

/**
 * @brief Get the selection end index.
 * @param label pointer to a label object.
 * @return selection end index. `LV_LABEL_TXT_SEL_OFF` if nothing is selected.
 */
uint32_t lv_label_get_text_sel_end(const lv_obj_t * label);

/*=====================
 * Other functions
 *====================*/

/**
 * Insert a text to the label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different
 * in UTF-8) 0: before first char. LV_LABEL_POS_LAST: after last char.
 * @param txt pointer to the text to insert
 */
void lv_label_ins_text(lv_obj_t * label, uint32_t pos, const char * txt);

/**
 * Delete characters from a label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different
 * in UTF-8) 0: before first char.
 * @param cnt number of characters to cut
 */
void lv_label_cut_text(lv_obj_t * label, uint32_t pos, uint32_t cnt);

/**
 * Refresh the label with its text stored in its extended data
 * @param label pointer to a label object
 */
void lv_label_refr_text(lv_obj_t * label);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LABEL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LABEL_H*/
