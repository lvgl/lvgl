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
#include "../lv_conf_internal.h"

#if LV_USE_ARC != 0

#include "../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_ARC_MODE_NORMAL,
    LV_ARC_MODE_SYMMETRICAL,
    LV_ARC_MODE_REVERSE
};
typedef uint8_t lv_arc_mode_t;

typedef struct {
    lv_obj_t obj;
    uint16_t rotation;
    uint16_t indic_angle_start;
    uint16_t indic_angle_end;
    uint16_t bg_angle_start;
    uint16_t bg_angle_end;
    int16_t value;              /*Current value of the arc*/
    int16_t min_value;          /*Minimum value of the arc*/
    int16_t max_value;          /*Maximum value of the arc*/
    uint16_t dragging    : 1;
    uint16_t type        : 2;
    uint16_t min_close   : 1;   /*1: the last pressed angle was closer to minimum end*/
    uint16_t chg_rate;          /*Drag angle rate of change of the arc (degrees/sec)*/
    uint32_t last_tick;         /*Last dragging event timestamp of the arc*/
    int16_t last_angle;         /*Last dragging angle of the arc*/
} lv_arc_t;

extern const lv_obj_class_t lv_arc_class;

/**
 * `type` field in `lv_obj_draw_part_dsc_t` if `class_p = lv_arc_class`
 * Used in `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END`
 */
typedef enum {
    LV_ARC_DRAW_PART_BACKGROUND,    /**< The background arc*/
    LV_ARC_DRAW_PART_FOREGROUND,    /**< The foreground arc*/
    LV_ARC_DRAW_PART_KNOB,          /**< The knob*/
} lv_arc_draw_part_type_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an arc object
 * @param parent pointer to an object, it will be the parent of the new arc
 * @return pointer to the created arc
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
 * @param arc   pointer to an arc object
 * @param start the start angle
 */
void lv_arc_set_start_angle(lv_obj_t * arc, uint16_t start);

/**
 * Set the end angle of an arc. 0 deg: right, 90 bottom, etc.
 * @param arc   pointer to an arc object
 * @param end   the end angle
 */
void lv_arc_set_end_angle(lv_obj_t * arc, uint16_t end);

/**
 * Set the start and end angles
 * @param arc   pointer to an arc object
 * @param start the start angle
 * @param end   the end angle
 */
void lv_arc_set_angles(lv_obj_t * arc, uint16_t start, uint16_t end);

/**
 * Set the start angle of an arc background. 0 deg: right, 90 bottom, etc.
 * @param arc   pointer to an arc object
 * @param start the start angle
 */
void lv_arc_set_bg_start_angle(lv_obj_t * arc, uint16_t start);

/**
 * Set the start angle of an arc background. 0 deg: right, 90 bottom etc.
 * @param arc   pointer to an arc object
 * @param end   the end angle
 */
void lv_arc_set_bg_end_angle(lv_obj_t * arc, uint16_t end);

/**
 * Set the start and end angles of the arc background
 * @param arc   pointer to an arc object
 * @param start the start angle
 * @param end   the end angle
 */
void lv_arc_set_bg_angles(lv_obj_t * arc, uint16_t start, uint16_t end);

/**
 * Set the rotation for the whole arc
 * @param arc       pointer to an arc object
 * @param rotation  rotation angle
 */
void lv_arc_set_rotation(lv_obj_t * arc, uint16_t rotation);

/**
 * Set the type of arc.
 * @param arc   pointer to arc object
 * @param mode  arc's mode
 */
void lv_arc_set_mode(lv_obj_t * arc, lv_arc_mode_t type);

/**
 * Set a new value on the arc
 * @param arc   pointer to an arc object
 * @param value new value
 */
void lv_arc_set_value(lv_obj_t * arc, int16_t value);

/**
 * Set minimum and the maximum values of an arc
 * @param arc   pointer to the arc object
 * @param min   minimum value
 * @param max   maximum value
 */
void lv_arc_set_range(lv_obj_t * arc, int16_t min, int16_t max);

/**
 * Set a change rate to limit the speed how fast the arc should reach the pressed point.
 * @param arc       pointer to an arc object
 * @param rate      the change rate
 */
void lv_arc_set_change_rate(lv_obj_t * arc, uint16_t rate);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the start angle of an arc.
 * @param arc   pointer to an arc object
 * @return      the start angle [0..360]
 */
uint16_t lv_arc_get_angle_start(lv_obj_t * obj);

/**
 * Get the end angle of an arc.
 * @param arc   pointer to an arc object
 * @return      the end angle [0..360]
 */
uint16_t lv_arc_get_angle_end(lv_obj_t * obj);

/**
 * Get the start angle of an arc background.
 * @param arc   pointer to an arc object
 * @return the  start angle [0..360]
 */
uint16_t lv_arc_get_bg_angle_start(lv_obj_t * obj);

/**
 * Get the end angle of an arc background.
 * @param arc   pointer to an arc object
 * @return      the end angle [0..360]
 */
uint16_t lv_arc_get_bg_angle_end(lv_obj_t * obj);

/**
 * Get the value of an arc
 * @param arc       pointer to an arc object
 * @return the      value of the arc
 */
int16_t lv_arc_get_value(const lv_obj_t * obj);

/**
 * Get the minimum value of an arc
 * @param arc   pointer to an arc object
 * @return      the minimum value of the arc
 */
int16_t lv_arc_get_min_value(const lv_obj_t * obj);

/**
 * Get the maximum value of an arc
 * @param arc   pointer to an arc object
 * @return      the maximum value of the arc
 */
int16_t lv_arc_get_max_value(const lv_obj_t * obj);

/**
 * Get whether the arc is type or not.
 * @param arc       pointer to an arc object
 * @return          arc's mode
 */
lv_arc_mode_t lv_arc_get_mode(const lv_obj_t * obj);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ARC*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ARC_H*/
