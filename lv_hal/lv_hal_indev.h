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
#include "../lv_misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Possible input device types*/
typedef enum {
    LV_INDEV_TYPE_NONE,        /*Show uninitialized state*/
    LV_INDEV_TYPE_POINTER,     /*Touch pad, mouse, external button*/
    LV_INDEV_TYPE_KEYPAD,      /*Keypad or keyboard*/
} lv_hal_indev_type_t;

/*States for input devices*/
typedef enum {
    LV_INDEV_STATE_REL,
    LV_INDEV_STATE_PR
}lv_indev_state_t;

/*Data type when an input device is read */
typedef struct {
    union {
        lv_point_t point;      /*For INDEV_TYPE_POINTER*/
        uint32_t key;          /*For INDEV_TYPE_KEYPAD*/
    };
    lv_indev_state_t state; /*LV_INDEV_EVENT_REL or LV_INDEV_EVENT_PR*/
}lv_indev_data_t;

/*Initialized by the user and registered by 'lv_indev_add()'*/
typedef struct {
    lv_hal_indev_type_t type;                       /*Input device type*/
    bool (*read)(lv_indev_data_t *data);        /*Function pointer to read data. Return 'true' if there is still data to be read (buffered)*/
}lv_indev_drv_t;

struct _lv_obj_t;

typedef struct _lv_indev_state_t {
    lv_indev_state_t state;
    union {
        struct {    /*Pointer data*/
            lv_point_t act_point;
            lv_point_t last_point;
            lv_point_t vect;
            lv_point_t drag_sum;				/*Count the dragged pixels to check LV_INDEV_DRAG_LIMIT*/
            struct _lv_obj_t * act_obj;
            struct _lv_obj_t * last_obj;

            /*Flags*/
            uint8_t drag_range_out      :1;
            uint8_t drag_in_prog        :1;
            uint8_t wait_unil_release   :1;
        };
        struct {    /*Keypad data*/
            lv_indev_state_t last_state;
        };
    };

    uint32_t pr_timestamp;          /*Pressed time stamp*/
    uint32_t longpr_rep_timestamp;  /*Long press repeat time stamp*/

    /*Flags*/
    uint8_t long_pr_sent        :1;
    uint8_t reset_query         :1;
    uint8_t disabled            :1;
}lv_indev_proc_t;


struct _lv_obj_t;
struct _lv_group_t;

typedef struct _lv_indev_t {
    lv_indev_drv_t driver;
    lv_indev_proc_t proc;
    uint32_t last_activity_time;
    union {
        struct _lv_obj_t *cursor;
        struct _lv_group_t *group;      /*Keypad destination group*/
    };
    struct _lv_indev_t *next;
} lv_indev_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize an input device driver with default values.
 * It is used to surly have known values in the fields ant not memory junk.
 * After it you can set the fields.
 * @param driver pointer to driver variable to initialize
 */
void lv_indev_drv_init(lv_indev_drv_t *driver);

/**
 * Register an initialized input device driver.
 * @param driver pointer to an initialized 'lv_indev_drv_t' variable (can be local variable)
 * @return pointer to the new input device or NULL on error
 */
lv_indev_t * lv_indev_drv_register(lv_indev_drv_t *driver);

/**
 * Get the next input device.
 * @param indev pointer to the current input device. NULL to initialize.
 * @return the next input devise or NULL if no more. Gives the first input device when the parameter is NULL
 */
lv_indev_t * lv_indev_next(lv_indev_t * indev);

/**
 * Read data from an input device.
 * @param indev pointer to an input device
 * @param data input device will write its data here
 * @return false: no more data; true: there more data to read (buffered)
 */
bool lv_indev_read(lv_indev_t * indev, lv_indev_data_t *data);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
