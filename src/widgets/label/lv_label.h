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
#include "../../lv_conf_internal.h"

#if LV_USE_LABEL != 0

#include <stdarg.h>
#include "../../core/lv_obj.h"
#include "../../font/lv_font.h"
#include "../../font/lv_symbol_def.h"
#include "../../misc/lv_txt.h"
#include "../../draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LABEL_WAIT_CHAR_COUNT        3
#define LV_LABEL_DOT_NUM 3
#define LV_LABEL_POS_LAST 0xFFFF
#define LV_LABEL_TEXT_SELECTION_OFF LV_DRAW_LABEL_NO_TXT_SEL

LV_EXPORT_CONST_INT(LV_LABEL_DOT_NUM);
LV_EXPORT_CONST_INT(LV_LABEL_POS_LAST);
LV_EXPORT_CONST_INT(LV_LABEL_TEXT_SELECTION_OFF);

/**********************
 *      TYPEDEFS
 **********************/

/** Long mode behaviors. Used in 'lv_label_ext_t'*/
enum {
    LV_LABEL_LONG_WRAP,             /**< Keep the object width, wrap the too long lines and expand the object height*/
    LV_LABEL_LONG_DOT,              /**< Keep the size and write dots at the end if the text is too long*/
    LV_LABEL_LONG_SCROLL,           /**< Keep the size and roll the text back and forth*/
    LV_LABEL_LONG_SCROLL_CIRCULAR,  /**< Keep the size and roll the text circularly*/
    LV_LABEL_LONG_CLIP,             /**< Keep the size and clip the text out of it*/
};
typedef uint8_t lv_label_long_mode_t;

typedef struct {
    lv_obj_t obj;
    char * text;
    union {
        char * tmp_ptr; /*Pointer to the allocated memory containing the character replaced by dots*/
        char tmp[LV_LABEL_DOT_NUM + 1]; /*Directly store the characters if <=4 characters*/
    } dot;
    uint32_t dot_end;  /*The real text length, used in dot mode*/

#if LV_LABEL_LONG_TXT_HINT
    lv_draw_label_hint_t hint;
#endif

#if LV_LABEL_TEXT_SELECTION
    uint32_t sel_start;
    uint32_t sel_end;
#endif

    lv_point_t size_cache; /*Text size cache*/
    lv_point_t offset; /*Text draw position offset*/
    lv_label_long_mode_t long_mode : 3; /*Determine what to do with the long texts*/
    uint8_t static_txt : 1;             /*Flag to indicate the text is static*/
    uint8_t recolor : 1;                /*Enable in-line letter re-coloring*/
    uint8_t expand : 1;                 /*Ignore real width (used by the library with LV_LABEL_LONG_SCROLL)*/
    uint8_t dot_tmp_alloc : 1;          /*1: dot is allocated, 0: dot directly holds up to 4 chars*/
    uint8_t invalid_size_cache : 1;     /*1: Recalculate size and update cache*/
} lv_label_t;

extern const lv_obj_class_t lv_label_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a label object
 * @param parent    pointer to an object, it will be the parent of the new label.
 * @return          pointer to the created button
 */
lv_obj_t * lv_label_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new text for a label. Memory will be allocated to store the text by the label.
 * @param obj           pointer to a label object
 * @param text          '\0' terminated character string. NULL to refresh with the current text.
 */
void lv_label_set_text(lv_obj_t * obj, const char * text);

/**
 * Set a new formatted text for a label. Memory will be allocated to store the text by the label.
 * @param obj           pointer to a label object
 * @param fmt           `printf`-like format
 * @example lv_label_set_text_fmt(label1, "%d user", user_num);
 */
void lv_label_set_text_fmt(lv_obj_t * obj, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Set a static text. It will not be saved by the label so the 'text' variable
 * has to be 'alive' while the label exists.
 * @param obj           pointer to a label object
 * @param text          pointer to a text. NULL to refresh with the current text.
 */
void lv_label_set_text_static(lv_obj_t * obj, const char * text);

/**
 * Set the behavior of the label with longer text then the object size
 * @param obj           pointer to a label object
 * @param long_mode     the new mode from 'lv_label_long_mode' enum.
 *                      In LV_LONG_WRAP/DOT/SCROLL/SCROLL_CIRC the size of the label should be set AFTER this function
 */
void lv_label_set_long_mode(lv_obj_t * obj, lv_label_long_mode_t long_mode);

/**
 * Enable the recoloring by in-line commands
 * @param obj           pointer to a label object
 * @param en            true: enable recoloring, false: disable
 * @example "This is a #ff0000 red# word"
 */
void lv_label_set_recolor(lv_obj_t * obj, bool en);

/**
 * Set where text selection should start
 * @param obj       pointer to a label object
 * @param index     character index from where selection should start. `LV_LABEL_TEXT_SELECTION_OFF` for no selection
 */
void lv_label_set_text_sel_start(lv_obj_t * obj, uint32_t index);

/**
 * Set where text selection should end
 * @param obj       pointer to a label object
 * @param index     character index where selection should end. `LV_LABEL_TEXT_SELECTION_OFF` for no selection
 */
void lv_label_set_text_sel_end(lv_obj_t * obj, uint32_t index);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a label
 * @param obj       pointer to a label object
 * @return          the text of the label
 */
char * lv_label_get_text(const lv_obj_t * obj);

/**
 * Get the long mode of a label
 * @param obj       pointer to a label object
 * @return          the current long mode
 */
lv_label_long_mode_t lv_label_get_long_mode(const lv_obj_t * obj);

/**
 * Get the recoloring attribute
 * @param obj       pointer to a label object
 * @return          true: recoloring is enabled, false: disable
 */
bool lv_label_get_recolor(const lv_obj_t * obj);

/**
 * Get the relative x and y coordinates of a letter
 * @param obj       pointer to a label object
 * @param index     index of the character [0 ... text length - 1].
 *                  Expressed in character index, not byte index (different in UTF-8)
 * @param pos       store the result here (E.g. index = 0 gives 0;0 coordinates if the text if aligned to the left)
 */
void lv_label_get_letter_pos(const lv_obj_t * obj, uint32_t char_id, lv_point_t * pos);

/**
 * Get the index of letter on a relative point of a label.
 * @param obj       pointer to label object
 * @param pos       pointer to point with coordinates on a the label
 * @return          The index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter if aligned to the left)
 *                  Expressed in character index and not byte index (different in UTF-8)
 */
uint32_t lv_label_get_letter_on(const lv_obj_t * obj, lv_point_t * pos_in);

/**
 * Check if a character is drawn under a point.
 * @param obj       pointer to a label object
 * @param pos       Point to check for character under
 * @return          whether a character is drawn under the point
 */
bool lv_label_is_char_under_pos(const lv_obj_t * obj, lv_point_t * pos);

/**
 * @brief Get the selection start index.
 * @param obj       pointer to a label object.
 * @return          selection start index. `LV_LABEL_TEXT_SELECTION_OFF` if nothing is selected.
 */
uint32_t lv_label_get_text_selection_start(const lv_obj_t * obj);

/**
 * @brief Get the selection end index.
 * @param obj       pointer to a label object.
 * @return          selection end index. `LV_LABEL_TXT_SEL_OFF` if nothing is selected.
 */
uint32_t lv_label_get_text_selection_end(const lv_obj_t * obj);

/*=====================
 * Other functions
 *====================*/

/**
 * Insert a text to a label. The label text can not be static.
 * @param obj       pointer to a label object
 * @param pos       character index to insert. Expressed in character index and not byte index.
 *                  0: before first char. LV_LABEL_POS_LAST: after last char.
 * @param txt       pointer to the text to insert
 */
void lv_label_ins_text(lv_obj_t * obj, uint32_t pos, const char * txt);

/**
 * Delete characters from a label. The label text can not be static.
 * @param obj       pointer to a label object
 * @param pos       character index from where to cut. Expressed in character index and not byte index.
 *                  0: start in from of the first character
 * @param cnt       number of characters to cut
 */
void lv_label_cut_text(lv_obj_t * obj, uint32_t pos, uint32_t cnt);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LABEL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LABEL_H*/
