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
#include "lv_hal.h"
#include "misc/gfx/area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Possible input device types*/
typedef enum {
    LV_INDEV_TYPE_TOUCH,       /*Touch pad*/
    LV_INDEV_TYPE_POINTER,     /*Mouse or similar pointer device*/
    LV_INDEV_TYPE_KEYPAD,      /*Keypad or keyboard*/
    LV_INDEV_TYPE_BUTTON,      /*Hardware button assigned to a point on the screen*/
} lv_hal_indev_type_t;

/*State for input devices*/
typedef enum {
    LV_INDEV_EVENT_RELEASED,
    LV_INDEV_EVENT_PRESSED
}lv_indev_event_t;

/*Data read from an input device.  */
typedef struct {
    union {
        point_t point;  /*For INDEV_TYPE_TOUCH, INDEV_TYPE_POINTER, INDEV_TYPE_BUTTON*/
        uint32_t key;   /*For INDEV_TYPE_BUTTON*/
    };
    lv_indev_event_t state;
}lv_indev_data_t;

/*Initialized by the user and registered by 'lv_hal_indev_drv_register'*/
typedef struct {
    const char * name;                              /*Input device name*/
    lv_hal_indev_type_t type;                       /*Input device type*/
    bool (*get_data)(lv_indev_data_t * data);       /*Function pointer to read data. Return 'true' if there is still data to be read (buffered)*/
}lv_indev_drv_t;

struct __LV_OBJ_T;

typedef struct _lv_indev_state_t
{
    lv_indev_event_t event;
    point_t act_point;
    point_t last_point;
    point_t vect;
    point_t vect_sum;
    struct __LV_OBJ_T * act_obj;
    struct __LV_OBJ_T * last_obj;
    uint32_t press_timestamp;
    uint32_t longpress_repeat_timestamp;

    /*Flags*/
    uint8_t drag_range_out      :1;
    uint8_t drag_in_prog        :1;
    uint8_t long_press_sent     :1;
    uint8_t wait_unil_release   :1;
    uint8_t reset_query         :1;
    uint8_t disabled            :1;
}lv_indev_state_t;


typedef struct _lv_indev_t {
    lv_indev_drv_t driver;
    lv_indev_state_t state;
    struct __LV_OBJ_T *cursor;
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
lv_indev_t * lv_indev_register(lv_indev_drv_t *driver);


/**
 * Ask data from an input device.
 * @param data input device data
 * @return false: no more data; true: there more data to read (buffered)
 */
bool lv_indev_read(lv_indev_t * indev, lv_indev_data_t *data);

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
