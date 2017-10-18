/**
 * @file hal_indev.h
 *
 * @description Input Device HAL interface layer header file
 * 
 */

#ifndef HAL_INDEV_H
#define HAL_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include <stdint.h>
#include "misc/gfx/area.h"
#include <lvgl/lv_hal/lv_hal.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Possible input device types*/
typedef enum {
    LV_HAL_INDEV_TYPE_TOUCH,       /*Touch pad*/
    LV_HAL_INDEV_TYPE_POINTER,     /*Mouse or similar pointer device*/
    LV_HAL_INDEV_TYPE_KEYPAD,      /*Keypad or keyboard*/
    LV_HAL_INDEV_TYPE_BUTTON,      /*Hardware button assigned to a point on the screen*/
} lv_hal_indev_type_t;

/*State for input devices*/
typedef enum {
    LV_HAL_INDEV_STATE_PRESS,
    LV_HAL_INDEV_STATE_RELEASE
}lv_hal_indev_state_t;

/*Data read from an input device.  */
typedef struct {
    union {
        point_t point;  /*For INDEV_TYPE_TOUCH, INDEV_TYPE_POINTER, INDEV_TYPE_BUTTON*/
        uint32_t key;   /*For INDEV_TYPE_BUTTON*/
    };
    lv_hal_indev_state_t state;
}lv_hal_indev_data_t;

/*Initialized by the user and registered by 'lv_hal_indev_drv_register'*/
typedef struct {
    const char * name;                              /*Input device name*/
    lv_hal_indev_type_t type;                       /*Input device type*/
    bool (*get_data)(lv_hal_indev_data_t * data);   /*Function pointer to read data. Return 'true' if there is still data to be read (buffered)*/
}lv_hal_indev_drv_t;

struct __LV_OBJ_T;

typedef struct _lv_indev_state_t
{
    bool pressed;
    point_t act_point;
    point_t last_point;
    point_t vect;
    point_t vect_sum;
    struct __LV_OBJ_T * act_obj;
    struct __LV_OBJ_T * last_obj;
    uint32_t press_time_stamp;
    uint32_t lpr_rep_time_stamp;

    /*Flags*/
    uint8_t drag_range_out  :1;
    uint8_t drag_in_prog    :1;
    uint8_t long_press_sent :1;
    uint8_t wait_release    :1;
    uint8_t reset_qry       :1;
    uint8_t disable :1;
}lv_indev_state_t;


typedef struct _lv_indev_t {
    lv_hal_indev_drv_t drv;
    lv_indev_state_t state;
    struct __LV_OBJ_T * cursor;
    struct _lv_indev_t *next;
} lv_indev_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register Input Device driver
 *
 * @param driver Input Device driver structure
 * @return 0 on success, -ve on error
 */
lv_indev_t * lv_indev_drv_register(lv_hal_indev_drv_t *driver);


/**
 * Ask data fro man input device.
 * @param data input device data
 * @return false: no more data; true: there more data to read (buffered)
 */
bool lv_indev_get(lv_indev_t * indev, lv_hal_indev_data_t *data);

/**
 * Get the next input device.
 * @param indev pointer to the current input device. NULL to initialize.
 * @return the next input devise or NULL if no more. Give the first input device when the parameter is NULL
 */
lv_indev_t * lv_indev_next(lv_indev_t * indev);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
