/**
 * @file lv_gauge.h
 *
 */

#ifndef LV_GAUGE_H
#define LV_GAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_GAUGE != 0

/*Testing of dependencies*/
#if LV_USE_LINEMETER == 0
#error "lv_gauge: lv_linemeter is required. Enable it in lv_conf.h (LV_USE_LINEMETER  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_linemeter.h"
#include "lv_label.h"
#include "lv_line.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_gauge_format_cb_t)(lv_obj_t * gauge, char * buf, int bufsize, int32_t value);

/*Data of gauge*/
typedef struct {
    lv_linemeter_ext_t lmeter; /*Ext. of ancestor*/
    /*New data for this type */
    int32_t * values;        /*Array of the set values (for needles) */
    const lv_color_t * needle_colors; /*Color of the needles (lv_color_t my_colors[needle_num])*/
    const void * needle_img;
    lv_point_t needle_img_pivot;
    lv_style_list_t style_needle;
    lv_style_list_t style_strong;
    uint8_t needle_count;             /*Number of needles*/
    uint8_t label_count;              /*Number of labels on the scale*/
    lv_gauge_format_cb_t format_cb;
} lv_gauge_ext_t;

/*Styles*/
enum {
    LV_GAUGE_PART_MAIN = LV_LINEMETER_PART_MAIN,
    LV_GAUGE_PART_MAJOR = _LV_LINEMETER_PART_VIRTUAL_LAST,
    LV_GAUGE_PART_NEEDLE,
    _LV_GAUGE_PART_VIRTUAL_LAST = _LV_LINEMETER_PART_VIRTUAL_LAST,
    _LV_GAUGE_PART_REAL_LAST = _LV_LINEMETER_PART_REAL_LAST,
};
typedef uint8_t lv_gauge_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a gauge objects
 * @param par pointer to an object, it will be the parent of the new gauge
 * @param copy pointer to a gauge object, if not NULL then the new object will be copied from it
 * @return pointer to the created gauge
 */
lv_obj_t * lv_gauge_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of needles
 * @param gauge pointer to gauge object
 * @param needle_cnt new count of needles
 * @param colors an array of colors for needles (with 'num' elements)
 */
void lv_gauge_set_needle_count(lv_obj_t * gauge, uint8_t needle_cnt, const lv_color_t colors[]);

/**
 * Set the value of a needle
 * @param gauge pointer to a gauge
 * @param needle_id the id of the needle
 * @param value the new value
 */
void lv_gauge_set_value(lv_obj_t * gauge, uint8_t needle_id, int32_t value);

/**
 * Set minimum and the maximum values of a gauge
 * @param gauge pointer to he gauge object
 * @param min minimum value
 * @param max maximum value
 */
static inline void lv_gauge_set_range(lv_obj_t * gauge, int32_t min, int32_t max)
{
    lv_linemeter_set_range(gauge, min, max);
}

/**
 * Set a critical value on the scale. After this value 'line.color' scale lines will be drawn
 * @param gauge pointer to a gauge object
 * @param value the critical value
 */
static inline void lv_gauge_set_critical_value(lv_obj_t * gauge, int32_t value)
{
    lv_linemeter_set_value(gauge, value);
}

/**
 * Set the scale settings of a gauge
 * @param gauge pointer to a gauge object
 * @param angle angle of the scale (0..360)
 * @param line_cnt count of scale lines.
 * To get a given "subdivision" lines between labels:
 * `line_cnt = (sub_div + 1) * (label_cnt - 1) + 1 `
 * @param label_cnt count of scale labels.
 */
void lv_gauge_set_scale(lv_obj_t * gauge, uint16_t angle, uint8_t line_cnt, uint8_t label_cnt);

/**
 * Set the set an offset for the gauge's angles to rotate it.
 * @param gauge pointer to a line meter object
 * @param angle angle offset (0..360), rotates clockwise
 */
static inline void lv_gauge_set_angle_offset(lv_obj_t * gauge, uint16_t angle)
{
    lv_linemeter_set_angle_offset(gauge, angle);
}

/**
 * Set an image to display as needle(s).
 * The needle image should be horizontal and pointing to the right (`--->`).
 * @param gauge pointer to a gauge object
 * @param img_src pointer to an `lv_img_dsc_t` variable or a path to an image
 *        (not an `lv_img` object)
 * @param pivot_x the X coordinate of rotation center of the image
 * @param pivot_y the Y coordinate of rotation center of the image
 */
void lv_gauge_set_needle_img(lv_obj_t * gauge, const void * img, lv_coord_t pivot_x, lv_coord_t pivot_y);

/**
 * Assign a function to format gauge values
 * @param gauge pointer to a gauge object
 * @param format_cb pointer to function of lv_gauge_format_cb_t
 */
void lv_gauge_set_formatter_cb(lv_obj_t * gauge, lv_gauge_format_cb_t format_cb);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a needle
 * @param gauge pointer to gauge object
 * @param needle the id of the needle
 * @return the value of the needle [min,max]
 */
int32_t lv_gauge_get_value(const lv_obj_t * gauge, uint8_t needle);

/**
 * Get the count of needles on a gauge
 * @param gauge pointer to gauge
 * @return count of needles
 */
uint8_t lv_gauge_get_needle_count(const lv_obj_t * gauge);

/**
 * Get the minimum value of a gauge
 * @param gauge pointer to a gauge object
 * @return the minimum value of the gauge
 */
static inline int32_t lv_gauge_get_min_value(const lv_obj_t * lmeter)
{
    return lv_linemeter_get_min_value(lmeter);
}

/**
 * Get the maximum value of a gauge
 * @param gauge pointer to a gauge object
 * @return the maximum value of the gauge
 */
static inline int32_t lv_gauge_get_max_value(const lv_obj_t * lmeter)
{
    return lv_linemeter_get_max_value(lmeter);
}

/**
 * Get a critical value on the scale.
 * @param gauge pointer to a gauge object
 * @return the critical value
 */
static inline int32_t lv_gauge_get_critical_value(const lv_obj_t * gauge)
{
    return lv_linemeter_get_value(gauge);
}

/**
 * Set the number of labels (and the thicker lines too)
 * @param gauge pointer to a gauge object
 * @return count of labels
 */
uint8_t lv_gauge_get_label_count(const lv_obj_t * gauge);

/**
 * Get the scale number of a gauge
 * @param gauge pointer to a gauge object
 * @return number of the scale units
 */
static inline uint16_t lv_gauge_get_line_count(const lv_obj_t * gauge)
{
    return lv_linemeter_get_line_count(gauge);
}

/**
 * Get the scale angle of a gauge
 * @param gauge pointer to a gauge object
 * @return angle of the scale
 */
static inline uint16_t lv_gauge_get_scale_angle(const lv_obj_t * gauge)
{
    return lv_linemeter_get_scale_angle(gauge);
}

/**
 * Get the offset for the gauge.
 * @param gauge pointer to a gauge object
 * @return angle offset (0..360)
 */
static inline uint16_t lv_gauge_get_angle_offset(lv_obj_t * gauge)
{
    return lv_linemeter_get_angle_offset(gauge);
}

/**
 * Get an image to display as needle(s).
 * @param gauge pointer to a gauge object
 * @return pointer to an `lv_img_dsc_t` variable or a path to an image
 *        (not an `lv_img` object). `NULL` if not used.
 */
const void * lv_gauge_get_needle_img(lv_obj_t * gauge);

/**
 * Get the X coordinate of the rotation center of the needle image
 * @param gauge pointer to a gauge object
 * @return the X coordinate of rotation center of the image
 */
lv_coord_t lv_gauge_get_needle_img_pivot_x(lv_obj_t * gauge);

/**
 * Get the Y coordinate of the rotation center of the needle image
 * @param gauge pointer to a gauge object
 * @return the X coordinate of rotation center of the image
 */
lv_coord_t lv_gauge_get_needle_img_pivot_y(lv_obj_t * gauge);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GAUGE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GAUGE_H*/
