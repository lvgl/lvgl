/**
 * @file lv_arclabel_private.h
 *
 */

#ifndef LV_ARCLABEL_PRIVATE_H
#define LV_ARCLABEL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj_private.h"

#if LV_USE_ARCLABEL != 0
#include "lv_arclabel.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_arclabel_t {
    lv_obj_t obj;

    char * text;
    char dot[LV_ARCLABEL_DOT_NUM + 1]; /**< Bytes that have been replaced with dots */
    uint32_t dot_begin;                 /**< Offset where bytes have been replaced with dots */

    /**
     * @brief The starting angle of the arc in degrees.
     *
     * This variable represents the beginning of the arc's angular range
     * in terms of degrees. It is used in conjunction with `angle_size` to define
     * the section of the circle that the arc covers. Values are normalized
     * to the range [0, 360), ensuring compatibility with circular representations.
     *
     * Modifying this value affects the visual rendering of the arc and its associated
     * indicators or labels, necessitating a subsequent invalidation or update of the
     * object's display to reflect the changes.
     *
     * @note When setting this value programmatically, ensure it does not exceed 360 degrees,
     * as it will be automatically adjusted to fit within the valid range.
     */
    lv_value_precise_t angle_start;
    lv_value_precise_t angle_size;
    uint32_t offset;
    uint32_t radius;
    lv_point_t center_offset;
    lv_arclabel_dir_t dir;
    lv_arclabel_text_align_t text_align_v; /**< Vertical text alignment */
    lv_arclabel_text_align_t text_align_h; /**< Horizontal text alignment */
    uint8_t static_txt : 1;             /**< Flag to indicate the text is static */
    uint8_t recolor    : 1;             /**< Enable in-line letter re-coloring */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_ARCLABEL != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ARCLABEL_PRIVATE_H*/
