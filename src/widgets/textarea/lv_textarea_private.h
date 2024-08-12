/**
 * @file lv_textarea_private.h
 *
 */

#ifndef LV_TEXTAREA_PRIVATE_H
#define LV_TEXTAREA_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_textarea.h"

#if LV_USE_TEXTAREA != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Data of text area */
struct _lv_textarea_t {
    lv_obj_t obj;
    lv_obj_t * label;            /**< Label of the text area */
    char * placeholder_txt;      /**< Place holder label. only visible if text is an empty string */
    char * pwd_tmp;              /**< Used to store the original text in password mode */
    char * pwd_bullet;           /**< Replacement characters displayed in password mode */
    const char * accepted_chars; /**< Only these characters will be accepted. NULL: accept all */
    uint32_t max_length;         /**< The max. number of characters. 0: no limit */
    uint32_t pwd_show_time;      /**< Time to show characters in password mode before change them to '*' */
    struct {
        int32_t valid_x;         /**< Used when stepping up/down to a shorter line.
                                  *(Used by the library) */
        uint32_t pos;            /**< The current cursor position
                                  *(0: before 1st letter; 1: before 2nd letter ...) */
        lv_area_t area;          /**< Cursor area relative to the Text Area */
        uint32_t txt_byte_pos;   /**< Byte index of the letter after (on) the cursor */
        uint8_t show : 1;        /**< Cursor is visible now or not (Handled by the library) */
        uint8_t click_pos : 1;   /**< 1: Enable positioning the cursor by clicking the text area */
    } cursor;
#if LV_LABEL_TEXT_SELECTION
    uint32_t sel_start;           /**< Temporary values for text selection */
    uint32_t sel_end;
    uint8_t text_sel_in_prog : 1; /**< User is in process of selecting */
    uint8_t text_sel_en : 1;      /**< Text can be selected on this text area */
#endif
    uint8_t pwd_mode : 1;         /**< Replace characters with '*' */
    uint8_t one_line : 1;         /**< One line mode (ignore line breaks) */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_TEXTAREA != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEXTAREA_PRIVATE_H*/
