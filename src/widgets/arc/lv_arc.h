/**
 * @file lv_arc.h
 *
 */

#ifndef LV_ARC_H
#define LV_ARC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_ARC != 0

#include "../../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_ARC_MODE_NORMAL,
    LV_ARC_MODE_SYMMETRICAL,
    LV_ARC_MODE_REVERSE
} lv_arc_mode_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_arc_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an arc object
 * @param parent    pointer to an object, it will be the parent of the new arc
 * @return          pointer to the created arc
 */
lv_obj_t * lv_arc_create(lv_obj_t * parent);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the start angle of an arc. 0 deg: right, 90 bottom, etc.
 * @param obj       pointer to an arc object
 * @param start     the start angle. (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
void lv_arc_set_start_angle(lv_obj_t * obj, lv_value_precise_t start);

/**
 * Set the end angle of an arc. 0 deg: right, 90 bottom, etc.
 * @param obj       pointer to an arc object
 * @param end       the end angle  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
void lv_arc_set_end_angle(lv_obj_t * obj, lv_value_precise_t end);

/**
 * Set the start and end angles
 * @param obj       pointer to an arc object
 * @param start     the start angle  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 * @param end       the end angle  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
void lv_arc_set_angles(lv_obj_t * obj, lv_value_precise_t start, lv_value_precise_t end);

/**
 * Set the start angle of an arc background. 0 deg: right, 90 bottom, etc.
 * @param obj       pointer to an arc object
 * @param start     the start angle  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
void lv_arc_set_bg_start_angle(lv_obj_t * obj, lv_value_precise_t start);

/**
 * Set the start angle of an arc background. 0 deg: right, 90 bottom etc.
 * @param obj       pointer to an arc object
 * @param end       the end angle  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
void lv_arc_set_bg_end_angle(lv_obj_t * obj, lv_value_precise_t end);

/**
 * Set the start and end angles of the arc background
 * @param obj       pointer to an arc object
 * @param start     the start angle  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 * @param end       the end angle  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
void lv_arc_set_bg_angles(lv_obj_t * obj, lv_value_precise_t start, lv_value_precise_t end);

/**
 * Set the rotation for the whole arc
 * @param obj           pointer to an arc object
 * @param rotation      rotation angle
 */
void lv_arc_set_rotation(lv_obj_t * obj, int32_t rotation);

/**
 * Set the type of arc.
 * @param obj       pointer to arc object
 * @param type      arc's mode
 */
void lv_arc_set_mode(lv_obj_t * obj, lv_arc_mode_t type);

/**
 * Set a new value on the arc
 * @param obj       pointer to an arc object
 * @param value     new value
 */
void lv_arc_set_value(lv_obj_t * obj, int32_t value);

/**
 * Set minimum and the maximum values of an arc
 * @param obj       pointer to the arc object
 * @param min       minimum value
 * @param max       maximum value
 */
void lv_arc_set_range(lv_obj_t * obj, int32_t min, int32_t max);

/**
 * Set a change rate to limit the speed how fast the arc should reach the pressed point.
 * @param obj       pointer to an arc object
 * @param rate      the change rate
 */
void lv_arc_set_change_rate(lv_obj_t * obj, uint32_t rate);

/**
 * Set an offset angle for the knob
 * @param obj       pointer to an arc object
 * @param offset    knob offset from main arc in degrees
 */
void lv_arc_set_knob_offset(lv_obj_t * obj, int32_t offset);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the start angle of an arc.
 * @param obj       pointer to an arc object
 * @return          the start angle [0..360]  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
lv_value_precise_t lv_arc_get_angle_start(lv_obj_t * obj);

/**
 * Get the end angle of an arc.
 * @param obj       pointer to an arc object
 * @return          the end angle [0..360]  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
lv_value_precise_t lv_arc_get_angle_end(lv_obj_t * obj);

/**
 * Get the start angle of an arc background.
 * @param obj       pointer to an arc object
 * @return          the  start angle [0..360]  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
lv_value_precise_t lv_arc_get_bg_angle_start(lv_obj_t * obj);

/**
 * Get the end angle of an arc background.
 * @param obj       pointer to an arc object
 * @return          the end angle [0..360]  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
lv_value_precise_t lv_arc_get_bg_angle_end(lv_obj_t * obj);

/**
 * Get the value of an arc
 * @param obj       pointer to an arc object
 * @return          the value of the arc
 */
int32_t lv_arc_get_value(const lv_obj_t * obj);

/**
 * Get the minimum value of an arc
 * @param obj       pointer to an arc object
 * @return          the minimum value of the arc
 */
int32_t lv_arc_get_min_value(const lv_obj_t * obj);

/**
 * Get the maximum value of an arc
 * @param obj       pointer to an arc object
 * @return          the maximum value of the arc
 */
int32_t lv_arc_get_max_value(const lv_obj_t * obj);

/**
 * Get whether the arc is type or not.
 * @param obj       pointer to an arc object
 * @return          arc's mode
 */
lv_arc_mode_t lv_arc_get_mode(const lv_obj_t * obj);

/**
 * Get the rotation for the whole arc
 * @param obj       pointer to an arc object
 * @return          arc's current rotation
 */
int32_t lv_arc_get_rotation(const lv_obj_t * obj);

/**
 * Get the current knob angle offset
 * @param obj       pointer to an arc object
 * @return          arc's current knob offset
 */
int32_t lv_arc_get_knob_offset(const lv_obj_t * obj);

/*=====================
 * Other functions
 *====================*/

/**
 * Align an object to the current position of the arc (knob)
 * @param obj           pointer to an arc object
 * @param obj_to_align  pointer to an object to align
 * @param r_offset      consider the radius larger with this value (< 0: for smaller radius)
 */
void lv_arc_align_obj_to_angle(const lv_obj_t * obj, lv_obj_t * obj_to_align, int32_t r_offset);

/**
 * Rotate an object to the current position of the arc (knob)
 * @param obj            pointer to an arc object
 * @param obj_to_rotate  pointer to an object to rotate
 * @param r_offset       consider the radius larger with this value (< 0: for smaller radius)
 */
void lv_arc_rotate_obj_to_angle(const lv_obj_t * obj, lv_obj_t * obj_to_rotate, int32_t r_offset);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ARC*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ARC_H*/
