/**
 * @file lv_line.h
 *
 */

#ifndef LV_LINE_H
#define LV_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_LINE != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of line*/
typedef struct {
    /*Inherited from 'base_obj' so no inherited ext.*/ /*Ext. of ancestor*/
    const lv_point_t * point_array;                    /*Pointer to an array with the points of the line*/
    uint16_t point_num;                                /*Number of points in 'point_array' */
    uint8_t auto_size : 1;                             /*1: set obj. width to x max and obj. height to y max */
    uint8_t y_inv : 1;                                 /*1: y == 0 will be on the bottom*/
} lv_line_ext_t;

/*Styles*/
enum {
    LV_LINE_PART_MAIN,
};
typedef uint8_t lv_line_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a line objects
 * @param par pointer to an object, it will be the parent of the new line
 * @return pointer to the created line
 */
lv_obj_t * lv_line_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set an array of points. The line object will connect these points.
 * @param line pointer to a line object
 * @param point_a an array of points. Only the address is saved,
 * so the array can NOT be a local variable which will be destroyed
 * @param point_num number of points in 'point_a'
 */
void lv_line_set_points(lv_obj_t * line, const lv_point_t point_a[], uint16_t point_num);

/**
 * Enable (or disable) the auto-size option. The size of the object will fit to its points.
 * (set width to x max and height to y max)
 * @param line pointer to a line object
 * @param en true: auto size is enabled, false: auto size is disabled
 */
void lv_line_set_auto_size(lv_obj_t * line, bool en);

/**
 * Enable (or disable) the y coordinate inversion.
 * If enabled then y will be subtracted from the height of the object,
 * therefore the y=0 coordinate will be on the bottom.
 * @param line pointer to a line object
 * @param en true: enable the y inversion, false:disable the y inversion
 */
void lv_line_set_y_invert(lv_obj_t * line, bool en);

#define lv_line_set_y_inv                                                                                              \
    lv_line_set_y_invert /*The name was inconsistent. In v.6.0 only `lv_line_set_y_invert`will                         \
work */

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the auto size attribute
 * @param line pointer to a line object
 * @return true: auto size is enabled, false: disabled
 */
bool lv_line_get_auto_size(const lv_obj_t * line);

/**
 * Get the y inversion attribute
 * @param line pointer to a line object
 * @return true: y inversion is enabled, false: disabled
 */
bool lv_line_get_y_invert(const lv_obj_t * line);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LINE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LINE_H*/
