/**
 * @file lv_label_private.h
 *
 */

#ifndef LV_LABEL_PRIVATE_H
#define LV_LABEL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../draw/lv_draw_label_private.h"
#include "../../core/lv_obj_private.h"
#include "../../lvgl_public.h"

#if LV_USE_LABEL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_label_t {
    lv_obj_t obj;
    char * text;
#if LV_USE_TRANSLATION
    char * translation_tag;
#endif /*LV_USE_TRANSLATION*/
    char dot[LV_LABEL_DOT_NUM + 1]; /**< Bytes that have been replaced with dots */
    uint32_t dot_begin;  /**< Offset where bytes have been replaced with dots */
    int32_t max_lines;

#if LV_LABEL_LONG_TXT_HINT
    lv_draw_label_hint_t hint;
#endif

#if LV_LABEL_TEXT_SELECTION
    uint32_t sel_start;
    uint32_t sel_end;
#endif

    int32_t text_size_width;            /**< Width `text_size` was measured at */
    lv_point_t offset;                  /**< Text draw position offset */
    lv_label_long_mode_t long_mode : 4; /**< Determine what to do with the long texts */
    uint8_t static_txt : 1;             /**< Flag to indicate the text is static */
    uint8_t recolor : 1;                /**< Enable in-line letter re-coloring*/
    uint8_t expand : 1;                 /**< Ignore real width (used by the library with LV_LABEL_LONG_MODE_SCROLL) */
    uint8_t text_flow_invalid : 1;
    uint8_t self_size_invalid : 1;      /**< 1: `text_size` needs to be measured again */
    lv_point_t text_size;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LABEL != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LABEL_PRIVATE_H*/
