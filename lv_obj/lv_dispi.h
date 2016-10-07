/**
 * @file lv_dispi.h
 * 
 */

#ifndef LV_DISPI_H
#define LV_DISPI_H

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
    
    /*Flags*/
    uint8_t drag_in_prog :1;
    uint8_t long_press_sent :1;
}lv_dispi_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_dispi_init(void);
void lv_dispi_reset(void);
bool lv_dispi_is_dragging(lv_dispi_t * dispi_p);
void lv_dispi_get_point(lv_dispi_t * dispi_p, point_t * point_p);
void lv_dispi_get_vect(lv_dispi_t * dispi_p, point_t * point_p);

/**********************
 *      MACROS
 **********************/

#endif