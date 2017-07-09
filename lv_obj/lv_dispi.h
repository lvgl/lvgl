/**
 * @file lv_dispi.h
 * 
 */

#ifndef LV_DISPI_H
#define LV_DISPI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    bool pressed;
    point_t act_point;
    point_t last_point;
    point_t vect;     
    point_t vect_sum;
    lv_obj_t * act_obj;
    lv_obj_t * last_obj;
    uint32_t press_time_stamp;
    uint32_t lpr_rep_time_stamp;
    
    /*Flags*/
    uint8_t drag_range_out :1;
    uint8_t drag_in_prog :1;
    uint8_t long_press_sent :1;
    uint8_t wait_release :1;
}lv_dispi_t;


typedef enum
{
    LV_ACTION_RES_INV = 0,      /*Typically indicates that the object is deleted (become invalid) in the action function*/
	LV_ACTION_RES_OK,           /*The object is valid (no deleted) after the action*/
}lv_action_res_t;

typedef lv_action_res_t ( * lv_action_t) (struct __LV_OBJ_T * obj, lv_dispi_t * dispi);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the display input subsystem
 */
void lv_dispi_init(void);

/**
 * Get an array with all the display inputs. Contains (INDEV_NUM elements)
 * @return pointer to a an lv_dispi_t array.
 */
lv_dispi_t * lv_dispi_get_array(void);

/**
 * Reset all display inputs
 */
void lv_dispi_reset(void);

/**
 * Reset the long press state of a display input
 * @param dispi pointer to a display input
 */
void lv_dispi_reset_lpr(lv_dispi_t * dispi);

/**
 * Get the last point on display input
 * @param dispi pointer to a display input
 * @param point pointer to a point to store the result
 */
void lv_dispi_get_point(lv_dispi_t * dispi, point_t * point);

/**
 * Check if there is dragging on display input or not
 * @param dispi pointer to a display input
 * @return true: drag is in progress
 */
bool lv_dispi_is_dragging(lv_dispi_t * dispi);

/**
 * Get the vector of dragging on a display input
 * @param dispi pointer to a display input
 * @param point pointer to a point to store the vector
 */
void lv_dispi_get_vect(lv_dispi_t * dispi, point_t * point);

/**
 * Do nothing until the next release
 * @param dispi pointer to a display input
 */
void lv_dispi_wait_release(lv_dispi_t * dispi);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_DISPI_H*/
