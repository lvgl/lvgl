/**
 * @file lv_linemeter.h
 *
 */

#ifndef LV_LINEMETER_H
#define LV_LINEMETER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_LMETER != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of line meter*/
typedef struct
{
    /*No inherited ext.*/ /*Ext. of ancestor*/
    /*New data for this type */
    uint16_t scale_angle; /*Angle of the scale in deg. (0..360)*/
    uint16_t angle_ofs;
    uint16_t line_cnt;     /*Count of lines */
    int16_t cur_value;
    int16_t min_value;
    int16_t max_value;
} lv_linemeter_ext_t;

/*Styles*/
enum {
    LV_LINEMETER_PART_MAIN,
    _LV_LINEMETER_PART_VIRTUAL_LAST,
    _LV_LINEMETER_PART_REAL_LAST = _LV_OBJ_PART_REAL_LAST,
};
typedef uint8_t lv_linemeter_part_t;

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
lv_obj_t * lv_linemeter_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the line meter
 * @param lmeter pointer to a line meter object
 * @param value new value
 */
void lv_linemeter_set_value(lv_obj_t * lmeter, int16_t value);

/**
 * Set minimum and the maximum values of a line meter
 * @param lmeter pointer to he line meter object
 * @param min minimum value
 * @param max maximum value
 */
void lv_linemeter_set_range(lv_obj_t * lmeter, int16_t min, int16_t max);

/**
 * Set the scale settings of a line meter
 * @param lmeter pointer to a line meter object
 * @param angle angle of the scale (0..360)
 * @param line_cnt number of lines
 */
void lv_linemeter_set_scale(lv_obj_t * lmeter, uint16_t angle, uint16_t line_cnt);

/**
 * Set the set an offset for the line meter's angles to rotate it.
 * @param lmeter pointer to a line meter object
 * @param angle angle offset (0..360), rotates clockwise
 */
void lv_linemeter_set_angle_offset(lv_obj_t * lmeter, uint16_t angle);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the value of the line meter
 */
int16_t lv_linemeter_get_value(const lv_obj_t * lmeter);

/**
 * Get the minimum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the minimum value of the line meter
 */
int16_t lv_linemeter_get_min_value(const lv_obj_t * lmeter);

/**
 * Get the maximum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the maximum value of the line meter
 */
int16_t lv_linemeter_get_max_value(const lv_obj_t * lmeter);

/**
 * Get the scale number of a line meter
 * @param lmeter pointer to a line meter object
 * @return number of the scale units
 */
uint16_t lv_linemeter_get_line_count(const lv_obj_t * lmeter);

/**
 * Get the scale angle of a line meter
 * @param lmeter pointer to a line meter object
 * @return angle of the scale
 */
uint16_t lv_linemeter_get_scale_angle(const lv_obj_t * lmeter);

/**
 * get the set an offset for the line meter.
 * @param lmeter pointer to a line meter object
 * @return angle offset (0..360)
 */
uint16_t lv_linemeter_get_angle_offset(lv_obj_t * lmeter);


void lv_linemeter_draw_scale(lv_obj_t * lmeter, const lv_area_t * clip_area, uint8_t part);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LMETER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LINEMETER_H*/
