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
#include "lv_conf.h"
#if USE_LV_LINE != 0

#include "lvgl/lv_obj/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of line*/
typedef struct
{
    /*Inherited from 'base_obj' so no inherited ext.*/  /*Ext. of ancestor*/
    const point_t * point_array;    /*Pointer to an array with the points of the line*/
    uint16_t  point_num;            /*Number of points in 'point_array' */
    uint8_t  auto_size  :1;         /*1: set obj. width to x max and obj. height to y max */
    uint8_t  y_inv      :1;         /*1: y == 0 will be on the bottom*/
    uint8_t  upscale    :1;         /*1: upscale coordinates with LV_DOWNSCALE*/
}lv_line_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a line objects
 * @param par pointer to an object, it will be the parent of the new line
 * @return pointer to the created line
 */
lv_obj_t * lv_line_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the line
 * @param line pointer to a line object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_line_signal(lv_obj_t * line, lv_signal_t sign, void * param);

/**
 * Set an array of points. The line object will connect these points.
 * @param line pointer to a line object
 * @param point_a an array of points. Only the address is saved,
 * so the array can NOT be a local variable which will be destroyed
 * @param point_num number of points in 'point_a'
 */
void lv_line_set_points(lv_obj_t * line, const point_t * point_a, uint16_t point_num);

/**
 * Enable (or disable) the auto-size option. The size of the object will fit to its points.
 * (set width to x max and height to y max)
 * @param line pointer to a line object
 * @param autosize true: auto size is enabled, false: auto size is disabled
 */
void lv_line_set_auto_size(lv_obj_t * line, bool autosize);

/**
 * Enable (or disable) the y coordinate inversion.
 * If enabled then y will be subtracted from the height of the object,
 * therefore the y=0 coordinate will be on the bottom.
 * @param line pointer to a line object
 * @param yinv true: enable the y inversion, false:disable the y inversion
 */
void lv_line_set_y_inv(lv_obj_t * line, bool yinv);

/**
 * Enable (or disable) the point coordinate upscaling (compensate LV_DOWNSCALE).
 * @param line pointer to a line object
 * @param unscale true: enable the point coordinate upscaling
 */
void lv_line_set_upscale(lv_obj_t * line, bool unscale);

/**
 * Get the auto size attribute
 * @param line pointer to a line object
 * @return true: auto size is enabled, false: disabled
 */
bool lv_line_get_auto_size(lv_obj_t * line);

/**
 * Get the y inversion attribute
 * @param line pointer to a line object
 * @return true: y inversion is enabled, false: disabled
 */
bool lv_line_get_y_inv(lv_obj_t * line);

/**
 * Get the point upscale enable attribute
 * @param obj pointer to a line object
 * @return true: point coordinate upscale is enabled, false: disabled
 */
bool lv_line_get_upscale(lv_obj_t * line);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_LINE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_LINE_H*/
