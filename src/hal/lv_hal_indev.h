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
#include "../lv_conf_internal.h"

#include <stdbool.h>
#include <stdint.h>
#include "../misc/lv_area.h"
#include "../misc/lv_timer.h"

/*********************
 *      DEFINES
 *********************/

/*Drag threshold in pixels*/
#define LV_INDEV_DEF_SCROLL_LIMIT         10

/*Drag throw slow-down in [%]. Greater value -> faster slow-down*/
#define LV_INDEV_DEF_SCROLL_THROW         10

/*Long press time in milliseconds.
 *Time to send `LV_EVENT_LONG_PRESSSED`)*/
#define LV_INDEV_DEF_LONG_PRESS_TIME      400

/*Repeated trigger period in long press [ms]
 *Time between `LV_EVENT_LONG_PRESSED_REPEAT*/
#define LV_INDEV_DEF_LONG_PRESS_REP_TIME  100


/*Gesture threshold in pixels*/
#define LV_INDEV_DEF_GESTURE_LIMIT        50

/*Gesture min velocity at release before swipe (pixels)*/
#define LV_INDEV_DEF_GESTURE_MIN_VELOCITY 3


/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;
struct _lv_disp_t;
struct _lv_group_t;
struct _lv_indev_t;
struct _lv_indev_drv_t;

/** Possible input device types*/
typedef enum {
    LV_INDEV_TYPE_NONE,    /**< Uninitialized state*/
    LV_INDEV_TYPE_POINTER, /**< Touch pad, mouse, external button*/
    LV_INDEV_TYPE_KEYPAD,  /**< Keypad or keyboard*/
    LV_INDEV_TYPE_BUTTON,  /**< External (hardware button) which is assigned to a specific point of the screen*/
    LV_INDEV_TYPE_ENCODER, /**< Encoder with only Left, Right turn and a Button*/
} lv_indev_type_t;

/** States for input devices*/
typedef enum {
    LV_INDEV_STATE_RELEASED = 0,
    LV_INDEV_STATE_PRESSED
} lv_indev_state_t;

/** Data structure passed to an input driver to fill*/
typedef struct {
    lv_point_t point; /**< For LV_INDEV_TYPE_POINTER the currently pressed point*/
    uint32_t key;     /**< For LV_INDEV_TYPE_KEYPAD the currently pressed key*/
    uint32_t btn_id;  /**< For LV_INDEV_TYPE_BUTTON the currently pressed button*/
    int16_t enc_diff; /**< For LV_INDEV_TYPE_ENCODER number of steps since the previous read*/

    lv_indev_state_t state; /**< LV_INDEV_STATE_REL or LV_INDEV_STATE_PR*/
    bool continue_reading;  /**< If set to true, the read callback is invoked again*/
} lv_indev_data_t;

/** Initialized by the user and registered by 'lv_indev_add()'*/
typedef struct _lv_indev_drv_t {

    /**< Input device type*/
    lv_indev_type_t type;

    /**< Function pointer to read input device data.*/
    void (*read_cb)(struct _lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

    /** Called when an action happened on the input device.
     * The second parameter is the event from `lv_event_t`*/
    void (*feedback_cb)(struct _lv_indev_drv_t *, uint8_t);

#if LV_USE_USER_DATA
    void * user_data;
#endif

    /**< Pointer to the assigned display*/
    struct _lv_disp_t * disp;

    /**< Timer to periodically read the input device*/
    lv_timer_t * read_timer;

    /**< Number of pixels to slide before actually drag the object*/
    uint8_t scroll_limit;

    /**< Drag throw slow-down in [%]. Greater value means faster slow-down*/
    uint8_t scroll_throw;

    /**< At least this difference should be between two points to evaluate as gesture*/
    uint8_t gesture_min_velocity;

    /**< At least this difference should be to send a gesture*/
    uint8_t gesture_limit;

    /**< Long press time in milliseconds*/
    uint16_t long_press_time;

    /**< Repeated trigger period in long press [ms]*/
    uint16_t long_press_repeat_time;
} lv_indev_drv_t;

/** Run time data of input devices
 * Internally used by the library, you should not need to touch it.
 */
typedef struct _lv_indev_proc_t {
    lv_indev_state_t state; /**< Current state of the input device.*/
    /*Flags*/
    uint8_t long_pr_sent : 1;
    uint8_t reset_query : 1;
    uint8_t disabled : 1;
    uint8_t wait_until_release : 1;

    union {
        struct {
            /*Pointer and button data*/
            lv_point_t act_point; /**< Current point of input device.*/
            lv_point_t last_point; /**< Last point of input device.*/
            lv_point_t last_raw_point; /**< Last point read from read_cb. */
            lv_point_t vect; /**< Difference between `act_point` and `last_point`.*/
            lv_point_t scroll_sum; /*Count the dragged pixels to check LV_INDEV_DEF_SCROLL_LIMIT*/
            lv_point_t scroll_throw_vect;
            lv_point_t scroll_throw_vect_ori;
            struct _lv_obj_t * act_obj;      /*The object being pressed*/
            struct _lv_obj_t * last_obj;     /*The last object which was pressed*/
            struct _lv_obj_t * scroll_obj;   /*The object being scrolled*/
            struct _lv_obj_t * last_pressed; /*The lastly pressed object*/
            lv_area_t scroll_area;

            lv_point_t gesture_sum; /*Count the gesture pixels to check LV_INDEV_DEF_GESTURE_LIMIT*/
            /*Flags*/
            lv_dir_t scroll_dir : 4;
            lv_dir_t gesture_dir : 4;
            uint8_t gesture_sent : 1;
        } pointer;
        struct {
            /*Keypad data*/
            lv_indev_state_t last_state;
            uint32_t last_key;
        } keypad;
    } types;

    uint32_t pr_timestamp;         /**< Pressed time stamp*/
    uint32_t longpr_rep_timestamp; /**< Long press repeat time stamp*/
} _lv_indev_proc_t;

/** The main input device descriptor with driver, runtime data ('proc') and some additional
 * information*/
typedef struct _lv_indev_t {
    struct _lv_indev_drv_t * driver;
    _lv_indev_proc_t proc;
    struct _lv_obj_t * cursor;     /**< Cursor for LV_INPUT_TYPE_POINTER*/
    struct _lv_group_t * group;    /**< Keypad destination group*/
    const lv_point_t * btn_points; /**< Array points assigned to the button ()screen will be pressed
                                      here by the buttons*/
} lv_indev_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize an input device driver with default values.
 * It is used to surely have known values in the fields and not memory junk.
 * After it you can set the fields.
 * @param driver pointer to driver variable to initialize
 */
void lv_indev_drv_init(struct _lv_indev_drv_t * driver);

/**
 * Register an initialized input device driver.
 * @param driver pointer to an initialized 'lv_indev_drv_t' variable (can be local variable)
 * @return pointer to the new input device or NULL on error
 */
lv_indev_t * lv_indev_drv_register(struct _lv_indev_drv_t * driver);

/**
 * Update the driver in run time.
 * @param indev pointer to an input device. (return value of `lv_indev_drv_register`)
 * @param new_drv pointer to the new driver
 */
void lv_indev_drv_update(lv_indev_t * indev, struct _lv_indev_drv_t * new_drv);

/**
 * Get the next input device.
 * @param indev pointer to the current input device. NULL to initialize.
 * @return the next input device or NULL if there are no more. Provide the first input device when
 * the parameter is NULL
 */
lv_indev_t * lv_indev_get_next(lv_indev_t * indev);

/**
 * Read data from an input device.
 * @param indev pointer to an input device
 * @param data input device will write its data here
 */
void _lv_indev_read(lv_indev_t * indev, lv_indev_data_t * data);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
