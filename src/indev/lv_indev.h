/**
 * @file lv_indev.h
 *
 */

#ifndef LV_INDEV_H
#define LV_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_group.h"
#include "../misc/lv_area.h"
#include "../misc/lv_timer.h"
#include "../misc/lv_event.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

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

typedef enum {
    LV_INDEV_MODE_NONE = 0,
    LV_INDEV_MODE_TIMER,
    LV_INDEV_MODE_EVENT,
} lv_indev_mode_t;

/** Data structure passed to an input driver to fill*/
typedef struct {
    lv_point_t point; /**< For LV_INDEV_TYPE_POINTER the currently pressed point*/
    uint32_t key;     /**< For LV_INDEV_TYPE_KEYPAD the currently pressed key*/
    uint32_t btn_id;  /**< For LV_INDEV_TYPE_BUTTON the currently pressed button*/
    int16_t enc_diff; /**< For LV_INDEV_TYPE_ENCODER number of steps since the previous read*/

    lv_indev_state_t state; /**< LV_INDEV_STATE_REL or LV_INDEV_STATE_PR*/
    bool continue_reading;  /**< If set to true, the read callback is invoked again*/
} lv_indev_data_t;

typedef void (*lv_indev_read_cb_t)(lv_indev_t * indev, lv_indev_data_t * data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_indev_t * lv_indev_create(void);

/**
 * Remove the provided input device. Make sure not to use the provided input device afterwards anymore.
 * @param indev pointer to delete
 */
void lv_indev_delete(lv_indev_t * indev);

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
 */
void lv_indev_read(lv_indev_t * indev);

/**
 * Called periodically to read the input devices
 * @param timer pointer to a timer to read
 */
void lv_indev_read_timer_cb(lv_timer_t * timer);

/**
 * Enable or disable one or all input devices (default enabled)
 * @param indev pointer to an input device or NULL to enable/disable all of them
 * @param en true to enable, false to disable
 */
void lv_indev_enable(lv_indev_t * indev, bool en);

/**
 * Get the currently processed input device. Can be used in action functions too.
 * @return pointer to the currently processed input device or NULL if no input device processing
 * right now
 */
lv_indev_t * lv_indev_active(void);

/**
 * Set the type of an input device
 * @param indev pointer to an input device
 * @param indev_type the type of the input device from `lv_indev_type_t` (`LV_INDEV_TYPE_...`)
 */
void lv_indev_set_type(lv_indev_t * indev, lv_indev_type_t indev_type);

void lv_indev_set_read_cb(lv_indev_t * indev,  lv_indev_read_cb_t read_cb);

void lv_indev_set_user_data(lv_indev_t * indev, void * user_data);

void lv_indev_set_driver_data(lv_indev_t * indev, void * driver_data);

/**
 * Get the type of an input device
 * @param indev pointer to an input device
 * @return the type of the input device from `lv_hal_indev_type_t` (`LV_INDEV_TYPE_...`)
 */
lv_indev_type_t lv_indev_get_type(const lv_indev_t * indev);

lv_indev_read_cb_t lv_indev_get_read_cb(lv_indev_t * indev);

lv_indev_state_t lv_indev_get_state(const lv_indev_t * indev);

lv_group_t * lv_indev_get_group(const lv_indev_t * indev);

void lv_indev_set_display(lv_indev_t * indev, struct _lv_display_t * disp);

lv_display_t * lv_indev_get_display(const lv_indev_t * indev);

void * lv_indev_get_user_data(const lv_indev_t * indev);

void * lv_indev_get_driver_data(const lv_indev_t * indev);

/**
 * Reset one or all input devices
 * @param indev pointer to an input device to reset or NULL to reset all of them
 * @param obj pointer to an object which triggers the reset.
 */
void lv_indev_reset(lv_indev_t * indev, lv_obj_t * obj);

/**
 * Reset the long press state of an input device
 * @param indev pointer to an input device
 */
void lv_indev_reset_long_press(lv_indev_t * indev);

/**
 * Set a cursor for a pointer input device (for LV_INPUT_TYPE_POINTER and LV_INPUT_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @param cur_obj pointer to an object to be used as cursor
 */
void lv_indev_set_cursor(lv_indev_t * indev, lv_obj_t * cur_obj);

/**
 * Set a destination group for a keypad input device (for LV_INDEV_TYPE_KEYPAD)
 * @param indev pointer to an input device
 * @param group pointer to a group
 */
void lv_indev_set_group(lv_indev_t * indev, lv_group_t * group);

/**
 * Set the an array of points for LV_INDEV_TYPE_BUTTON.
 * These points will be assigned to the buttons to press a specific point on the screen
 * @param indev pointer to an input device
 * @param points array of points
 */
void lv_indev_set_button_points(lv_indev_t * indev, const lv_point_t points[]);

/**
 * Get the last point of an input device (for LV_INDEV_TYPE_POINTER and LV_INDEV_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @param point pointer to a point to store the result
 */
void lv_indev_get_point(const lv_indev_t * indev, lv_point_t * point);

/**
* Get the current gesture direct
* @param indev pointer to an input device
* @return current gesture direct
*/
lv_dir_t lv_indev_get_gesture_dir(const lv_indev_t * indev);

/**
 * Get the last pressed key of an input device (for LV_INDEV_TYPE_KEYPAD)
 * @param indev pointer to an input device
 * @return the last pressed key (0 on error)
 */
uint32_t lv_indev_get_key(const lv_indev_t * indev);

/**
 * Check the current scroll direction of an input device (for LV_INDEV_TYPE_POINTER and
 * LV_INDEV_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @return LV_DIR_NONE: no scrolling now
 *         LV_DIR_HOR/VER
 */
lv_dir_t lv_indev_get_scroll_dir(const lv_indev_t * indev);

/**
 * Get the currently scrolled object (for LV_INDEV_TYPE_POINTER and
 * LV_INDEV_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @return pointer to the currently scrolled object or NULL if no scrolling by this indev
 */
lv_obj_t * lv_indev_get_scroll_obj(const lv_indev_t * indev);

/**
 * Get the movement vector of an input device (for LV_INDEV_TYPE_POINTER and
 * LV_INDEV_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @param point pointer to a point to store the types.pointer.vector
 */
void lv_indev_get_vect(const lv_indev_t * indev, lv_point_t * point);

/**
 * Do nothing until the next release
 * @param indev pointer to an input device
 */
void lv_indev_wait_release(lv_indev_t * indev);

/**
 * Gets a pointer to the currently active object in the currently processed input device.
 * @return pointer to currently active object or NULL if no active object
 */
lv_obj_t * lv_indev_get_active_obj(void);

/**
 * Get a pointer to the indev read timer to
 * modify its parameters with `lv_timer_...` functions.
 * @param indev pointer to an input device
 * @return pointer to the indev read refresher timer. (NULL on error)
 */
lv_timer_t * lv_indev_get_read_timer(lv_indev_t * indev);

/**
* Set the input device's event model: event-driven mode or timer mode.
* @param indev pointer to an input device
* @param mode the mode of input device
*/
void lv_indev_set_mode(lv_indev_t * indev, lv_indev_mode_t mode);

/**
 * Get the input device's running mode.
 * @param indev pointer to an input device
 * @return the running mode for the specified input device.
 */
lv_indev_mode_t lv_indev_get_mode(lv_indev_t * indev);

/**
 * Search the most top, clickable object by a point
 * @param obj pointer to a start object, typically the screen
 * @param point pointer to a point for searching the most top child
 * @return pointer to the found object or NULL if there was no suitable object
 */
lv_obj_t * lv_indev_search_obj(lv_obj_t * obj, lv_point_t * point);

/**
 * Add an event handler to the indev
 * @param indev          pointer to an indev
 * @param event_cb      an event callback
 * @param filter        event code to react or `LV_EVENT_ALL`
 * @param user_data     optional user_data
 */
void lv_indev_add_event_cb(lv_indev_t * indev, lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data);

/**
 * Get the number of event attached to an indev
 * @param indev          pointer to an indev
 * @return              number of events
 */
uint32_t lv_indev_get_event_count(lv_indev_t * indev);

/**
 * Get an event descriptor for an event
 * @param indev          pointer to an indev
 * @param index         the index of the event
 * @return              the event descriptor
 */
lv_event_dsc_t * lv_indev_get_event_dsc(lv_indev_t * indev, uint32_t index);

/**
 * Remove an event
 * @param indev         pointer to an indev
 * @param index         the index of the event to remove
 * @return              true: and event was removed; false: no event was removed
 */
bool lv_indev_remove_event(lv_indev_t * indev, uint32_t index);

/**
 * Remove an event_cb with user_data
 * @param indev         pointer to a indev
 * @param event_cb      the event_cb of the event to remove
 * @param user_data     user_data
 * @return              the count of the event removed
 */
uint32_t lv_indev_remove_event_cb_with_user_data(lv_indev_t * indev, lv_event_cb_t event_cb, void * user_data);

/**
 * Send an event to an indev
 * @param indev         pointer to an indev
 * @param code          an event code. LV_EVENT_...
 * @param param         optional param
 * @return              LV_RESULT_OK: indev wasn't deleted in the event.
 */
lv_result_t lv_indev_send_event(lv_indev_t * indev, lv_event_code_t code, void * param);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_INDEV_H*/
