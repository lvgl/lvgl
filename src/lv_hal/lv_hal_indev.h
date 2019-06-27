/**
 * @file lv_hal_indev.h
 *
 * @description Input Device HAL interface layer header file
 *
 */

#ifndef LV_HAL_INDEV_H
#define LV_HAL_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#include <stdbool.h>
#include <stdint.h>
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_task.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;
struct _disp_t;
struct _lv_indev_t;
struct _lv_indev_drv_t;

/*Possible input device types*/
enum {
    LV_INDEV_TYPE_NONE,    /*Show uninitialized state*/
    LV_INDEV_TYPE_POINTER, /*Touch pad, mouse, external button*/
    LV_INDEV_TYPE_KEYPAD,  /*Keypad or keyboard*/
    LV_INDEV_TYPE_BUTTON,  /*External (hardware button) which is assigned to a specific point of the
                              screen*/
    LV_INDEV_TYPE_ENCODER, /*Encoder with only Left, Right turn and a Button*/
};
typedef uint8_t lv_indev_type_t;

/*States for input devices*/
enum { LV_INDEV_STATE_REL = 0, LV_INDEV_STATE_PR };
typedef uint8_t lv_indev_state_t;

/*Data type when an input device is read */
typedef struct
{
    lv_point_t point; /*For LV_INDEV_TYPE_POINTER the currently pressed point*/
    uint32_t key;     /*For LV_INDEV_TYPE_KEYPAD the currently pressed key*/
    uint32_t btn_id;  /*For LV_INDEV_TYPE_BUTTON the currently pressed button*/
    int16_t enc_diff; /*For LV_INDEV_TYPE_ENCODER number of steps since the previous read*/

    lv_indev_state_t state; /*LV_INDEV_STATE_REL or LV_INDEV_STATE_PR*/
} lv_indev_data_t;

/*Initialized by the user and registered by 'lv_indev_add()'*/
typedef struct _lv_indev_drv_t
{

    /*Input device type*/
    lv_indev_type_t type;

    /*Function pointer to read input device data.
     * Return 'true' if there is still data to be read (buffered)*/
    bool (*read_cb)(struct _lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

    /* Called when an action happened on the input device.
     * The second parameter is the event from `lv_event_t`*/
    void (*feedback_cb)(struct _lv_indev_drv_t *, uint8_t);

#if LV_USE_USER_DATA
    lv_indev_drv_user_data_t user_data;
#endif

    /*Pointer to the assigned display*/
    struct _disp_t * disp;

    /*Task to read the periodically read the input device*/
    lv_task_t * read_task;

    /*Number of pixels to slide before actually drag the object*/
    uint8_t drag_limit;

    /*Drag throw slow-down in [%]. Greater value means faster slow-down */
    uint8_t drag_throw;

    /*Long press time in milliseconds*/
    uint16_t long_press_time;

    /*Repeated trigger period in long press [ms] */
    uint16_t long_press_rep_time;
} lv_indev_drv_t;

/*Run time data of input devices*/
typedef struct _lv_indev_proc_t
{
    lv_indev_state_t state;
    union
    {
        struct
        { /*Pointer and button data*/
            lv_point_t act_point;
            lv_point_t last_point;
            lv_point_t vect;
            lv_point_t drag_sum; /*Count the dragged pixels to check LV_INDEV_DEF_DRAG_LIMIT*/
            lv_point_t drag_throw_vect;
            struct _lv_obj_t * act_obj;      /*The object being pressed*/
            struct _lv_obj_t * last_obj;     /*The last obejct which was pressed (used by dragthrow and
                                                other post-release event)*/
            struct _lv_obj_t * last_pressed; /*The lastly pressed object*/

            /*Flags*/
            uint8_t drag_limit_out : 1;
            uint8_t drag_in_prog : 1;
        } pointer;
        struct
        { /*Keypad data*/
            lv_indev_state_t last_state;
            uint32_t last_key;
        } keypad;
    } types;

    uint32_t pr_timestamp;         /*Pressed time stamp*/
    uint32_t longpr_rep_timestamp; /*Long press repeat time stamp*/

    /*Flags*/
    uint8_t long_pr_sent : 1;
    uint8_t reset_query : 1;
    uint8_t disabled : 1;
    uint8_t wait_until_release : 1;
} lv_indev_proc_t;

struct _lv_obj_t;
struct _lv_group_t;

/*The main input device descriptor with driver, runtime data ('proc') and some additional
 * information*/
typedef struct _lv_indev_t
{
    lv_indev_drv_t driver;
    lv_indev_proc_t proc;
    struct _lv_obj_t * cursor;     /*Cursor for LV_INPUT_TYPE_POINTER*/
    struct _lv_group_t * group;    /*Keypad destination group*/
    const lv_point_t * btn_points; /*Array points assigned to the button ()screen will be pressed
                                      here by the buttons*/
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
void lv_indev_drv_init(lv_indev_drv_t * driver);

/**
 * Register an initialized input device driver.
 * @param driver pointer to an initialized 'lv_indev_drv_t' variable (can be local variable)
 * @return pointer to the new input device or NULL on error
 */
lv_indev_t * lv_indev_drv_register(lv_indev_drv_t * driver);

/**
 * Update the driver in run time.
 * @param indev pointer to a input device. (return value of `lv_indev_drv_register`)
 * @param new_drv pointer to the new driver
 */
void lv_indev_drv_update(lv_indev_t * indev, lv_indev_drv_t * new_drv);

/**
 * Get the next input device.
 * @param indev pointer to the current input device. NULL to initialize.
 * @return the next input devise or NULL if no more. Give the first input device when the parameter
 * is NULL
 */
lv_indev_t * lv_indev_get_next(lv_indev_t * indev);

/**
 * Read data from an input device.
 * @param indev pointer to an input device
 * @param data input device will write its data here
 * @return false: no more data; true: there more data to read (buffered)
 */
bool lv_indev_read(lv_indev_t * indev, lv_indev_data_t * data);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
