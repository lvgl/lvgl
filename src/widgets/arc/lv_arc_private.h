/**
 * @file lv_arc_private.h
 *
 */

#ifndef LV_ARC_PRIVATE_H
#define LV_ARC_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj_private.h"
#include "lv_arc.h"

#if LV_USE_ARC != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_arc_t {
    lv_obj_t obj;
    int32_t rotation;
    lv_value_precise_t indic_angle_start;
    lv_value_precise_t indic_angle_end;
    lv_value_precise_t bg_angle_start;
    lv_value_precise_t bg_angle_end;
    int32_t value;              /**< Current value of the arc */
    int32_t min_value;          /**< Minimum value of the arc */
    int32_t max_value;          /**< Maximum value of the arc */
    uint32_t dragging    : 1;
    uint32_t type        : 2;
    uint32_t min_close   : 1;   /**< 1: the last pressed angle was closer to minimum end */
    uint32_t in_out      : 1;   /**< 1: The click was within the background arc angles. 0: Click outside */
    uint32_t chg_rate;          /**< Drag angle rate of change of the arc (degrees/sec) */
    uint32_t last_tick;         /**< Last dragging event timestamp of the arc */
    lv_value_precise_t last_angle;  /**< Last dragging angle of the arc */
    int16_t knob_offset;        /**< knob offset from the main arc */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_ARC != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ARC_PRIVATE_H*/
