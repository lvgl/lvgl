/**
 * @file lv_meter.h
 *
 */

#ifndef LV_METER_H
#define LV_METER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_METER != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_METER_INDICATOR_TYPE_SCALE = 0x01,
    LV_METER_INDICATOR_TYPE_NEEDLE = 0x02,
    LV_METER_INDICATOR_TYPE_ARC = 0x04,
}lv_meter_indicator_type_t;

typedef enum {
    LV_METER_SEGMENT_TYPE_SCALE = 0x1,
    LV_METER_SEGMENT_TYPE_ARC = 0x2,
}lv_meter_segment_type_t;

typedef struct {
    int32_t start_value;
    int32_t end_value;
    lv_meter_indicator_type_t type;
    lv_coord_t r_mod;
    lv_color_t color;
    lv_opa_t opa;
    lv_coord_t width;

    lv_color_t grad_color;
    const void * img_src;
    lv_opa_t recolor_opa;
    lv_point_t img_pivot;
    lv_point_t arc_ofs;
    uint8_t scale_color_local:1;
}lv_meter_indicator_t;

/*Data of line meter*/
typedef struct {
    lv_obj_t obj;
    lv_ll_t indic_ll;
    uint16_t scale_angle; /*Angle of the scale in deg. (0..360)*/
    uint16_t angle_ofs;
    uint16_t line_cnt;     /*Count of lines */
    uint16_t marker_nth;  /*Every Nth line should be marker */
    int32_t min_value;
    int32_t max_value;
} lv_meter_t;

extern const lv_obj_class_t lv_meter;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a line meter objects
 * @param par pointer to an object, it will be the parent of the new line meter
 * @param copy pointer to a line meter object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created line meter
 */
lv_obj_t * lv_meter_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the line meter
 * @param lmeter pointer to a line meter object
 * @param value new value
 */
lv_meter_indicator_t * lv_meter_add_indicator(lv_obj_t * obj);

void lv_meter_set_value(lv_obj_t * meter, lv_meter_indicator_t * indic, int32_t value);

/**
 * Set minimum and the maximum values of a line meter
 * @param lmeter pointer to he line meter object
 * @param min minimum value
 * @param max maximum value
 */
void lv_meter_set_range(lv_obj_t * meter, int32_t min, int32_t max);

/**
 * Set the scale settings of a line meter
 * @param lmeter pointer to a line meter object
 * @param angle angle of the scale (0..360)
 * @param line_cnt number of lines
 */
void lv_meter_set_scale(lv_obj_t * meter, uint16_t angle, uint16_t line_cnt, uint16_t nth_marker);

/**
 * Set the set an offset for the line meter's angles to rotate it.
 * @param lmeter pointer to a line meter object
 * @param angle angle offset (0..360), rotates clockwise
 */
void lv_meter_set_angle_offset(lv_obj_t * meter, uint16_t angle);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the value of the line meter
 */
int32_t lv_meter_get_value(const lv_obj_t * obj, const lv_meter_indicator_t * indic);

/**
 * Get the minimum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the minimum value of the line meter
 */
int32_t lv_meter_get_min_value(const lv_obj_t * meter);

/**
 * Get the maximum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the maximum value of the line meter
 */
int32_t lv_meter_get_max_value(const lv_obj_t * meter);

/**
 * Get the scale number of a line meter
 * @param lmeter pointer to a line meter object
 * @return number of the scale units
 */
uint16_t lv_meter_get_line_count(const lv_obj_t * meter);

/**
 * Get the scale angle of a line meter
 * @param lmeter pointer to a line meter object
 * @return angle of the scale
 */
uint16_t lv_meter_get_scale_angle(const lv_obj_t * meter);

/**
 * Get the offset for the line meter.
 * @param lmeter pointer to a line meter object
 * @return angle offset (0..360)
 */
uint16_t lv_meter_get_angle_offset(lv_obj_t * meter);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_METER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_METER_H*/
