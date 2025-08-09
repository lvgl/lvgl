/**
 * @file lv_arclabel.h
 *
 */

#ifndef LV_ARCLABEL_H
#define LV_ARCLABEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_ARCLABEL != 0

/*********************
 *      DEFINES
 *********************/

#define LV_ARCLABEL_DOT_NUM 3
#if LV_WIDGETS_HAS_DEFAULT_VALUE
#define LV_ARCLABEL_DEFAULT_TEXT "Arced Text"
#else
#define LV_ARCLABEL_DEFAULT_TEXT ""
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_ARCLABEL_DIR_CLOCKWISE = 0,
    LV_ARCLABEL_DIR_COUNTER_CLOCKWISE = 1
} lv_arclabel_dir_t;

typedef enum {
    LV_ARCLABEL_TEXT_ALIGN_DEFAULT = 0,
    LV_ARCLABEL_TEXT_ALIGN_LEADING = 1,
    LV_ARCLABEL_TEXT_ALIGN_CENTER = 2,
    LV_ARCLABEL_TEXT_ALIGN_TRAILING = 3,
} lv_arclabel_text_align_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_arclabel_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an arc label object
 * @param parent    pointer to an object, it will be the parent of the new arc label
 * @return          pointer to the created arc label
 */
lv_obj_t * lv_arclabel_create(lv_obj_t * parent);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of the arc label.
 *
 * This function sets the text displayed by an arc label object.
 *
 * @param obj       Pointer to the arc label object.
 * @param text      Pointer to a null-terminated string containing the new text for the label.
 */
void lv_arclabel_set_text(lv_obj_t * obj, const char * text);

/**
 * Set the formatted text of an arc label object.
 *
 * This function sets the text of an arc label object with support for
 * variable arguments formatting, similar to `printf`.
 *
 * @param obj       The arc label object to set the text for.
 * @param fmt       A format string that specifies how subsequent arguments are converted to text.
 * @param ...       Arguments following the format string that are used to replace format specifiers in the format string.
 */
void lv_arclabel_set_text_fmt(lv_obj_t * obj, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Sets a new static text for the arc label or refreshes it with the current text.
 * The 'text' must remain valid in memory; the arc label does not manage its lifecycle.
 *
 * @param obj       Pointer to the arc label object.
 * @param text      Pointer to the new text. If NULL, the label is refreshed with its current text.
 */
void lv_arclabel_set_text_static(lv_obj_t * obj, const char * text);

/**
 * Set the start angle of an arc. 0 deg: right, 90 bottom, etc.
 * @param obj       pointer to an arc label object
 * @param start     the start angle. (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
void lv_arclabel_set_angle_start(lv_obj_t * obj, lv_value_precise_t start);

/**
 * Set the end angle of an arc. 0 deg: right, 90 bottom, etc.
 * @param obj       pointer to an arc label object
 * @param size      the angle size (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
void lv_arclabel_set_angle_size(lv_obj_t * obj, lv_value_precise_t size);

/**
 * Set the rotation for the whole arc
 * @param obj       pointer to an arc label object
 * @param offset    rotation angle
 */
void lv_arclabel_set_offset(lv_obj_t * obj, int32_t offset);

/**
 * Set the type of arc.
 * @param obj       pointer to and arc label object
 * @param dir      arc label's direction
 */
void lv_arclabel_set_dir(lv_obj_t * obj, lv_arclabel_dir_t dir);

/**
 * Enable the recoloring by in-line commands
 * @param obj       pointer to an arc label object
 * @param en        true: enable recoloring, false: disable
 * Example: "This is a #ff0000 red# word"
 */
void lv_arclabel_set_recolor(lv_obj_t * obj, bool en);

/**
 * Set the radius for an arc label object.
 *
 * @param obj       pointer to the arc label object.
 * @param radius    The radius value to set for the label's curvature, in pixels.
 */
void lv_arclabel_set_radius(lv_obj_t * obj, uint32_t radius);

/**
 * Set the center offset x for an arc label object.
 * @param obj       pointer to an arc label object
 * @param x         the x offset
 */
void lv_arclabel_set_center_offset_x(lv_obj_t * obj, uint32_t x);

/**
 * Set the center offset y for an arc label object.
 * @param obj       pointer to an arc label object
 * @param y         the y offset
 */
void lv_arclabel_set_center_offset_y(lv_obj_t * obj, uint32_t y);

/**
 * Set the text vertical alignment for an arc label object.
 * @param obj       pointer to an arc label object
 * @param align     the vertical alignment
 */
void lv_arclabel_set_text_vertical_align(lv_obj_t * obj, lv_arclabel_text_align_t align);

/**
 * Set the text horizontal alignment for an arc label object.
 * @param obj       pointer to an arc label object
 * @param align     the horizontal alignment
 */
void lv_arclabel_set_text_horizontal_align(lv_obj_t * obj, lv_arclabel_text_align_t align);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the start angle of an arc label.
 * @param obj       pointer to an arc label object
 * @return          the start angle [0..360]  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
lv_value_precise_t lv_arclabel_get_angle_start(lv_obj_t * obj);

/**
 * Get the angle size of an arc label.
 * @param obj       pointer to an arc label object
 * @return          the end angle [0..360]  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
lv_value_precise_t lv_arclabel_get_angle_size(lv_obj_t * obj);

/**
 * Get whether the arc label is type or not.
 * @param obj       pointer to an arc label object
 * @return          arc label's direction
 */
lv_arclabel_dir_t lv_arclabel_get_dir(const lv_obj_t * obj);

/**
 * Enable the recoloring by in-line commands
 *
 * @see lv_arclabel_set_recolor
 *
 * @param obj       pointer to a label object
 * @return          true: enable recoloring, false: disable
 */
bool lv_arclabel_get_recolor(lv_obj_t * obj);

/**
 * Get the text of the arc label.
 * @param obj       pointer to an arc label object
 * @return          the radius of the arc label
 */
uint32_t lv_arclabel_get_radius(lv_obj_t * obj);

/**
 * Get the center offset x for an arc label object.
 * @param obj       pointer to an arc label object
 * @return          the x offset
 */
uint32_t lv_arclabel_get_center_offset_x(lv_obj_t * obj);

/**
 * Get the center offset y for an arc label object.
 * @param obj       pointer to an arc label object
 * @return          the y offset
 */
uint32_t lv_arclabel_get_center_offset_y(lv_obj_t * obj);

/**
 * Get the text vertical alignment for an arc label object.
 * @param obj       pointer to an arc label object
 * @return          the vertical alignment
 */
lv_arclabel_text_align_t lv_arclabel_get_text_vertical_align(lv_obj_t * obj);

/**
 * Get the text horizontal alignment for an arc label object.
 * @param obj       pointer to an arc label object
 * @return          the horizontal alignment
 */
lv_arclabel_text_align_t lv_arclabel_get_text_horizontal_align(lv_obj_t * obj);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ARCLABEL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ARCLABEL_H*/
