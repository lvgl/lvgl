/**
 * @file lv_hal_indev.c
 *
 * @description Input device HAL interface
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_assert.h"
#include "../hal/lv_hal_indev.h"
#include "../core/lv_indev.h"
#include "../misc/lv_mem.h"
#include "../misc/lv_gc.h"
#include "lv_hal_disp.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_LOG_TRACE_INDEV
    #define INDEV_TRACE(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define INDEV_TRACE(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize an input device driver with default values.
 * It is used to surly have known values in the fields ant not memory junk.
 * After it you can set the fields.
 * @param driver pointer to driver variable to initialize
 */
void lv_indev_drv_init(lv_indev_drv_t * driver)
{
    lv_memzero(driver, sizeof(lv_indev_drv_t));

    driver->type                 = LV_INDEV_TYPE_NONE;
    driver->scroll_limit         = LV_INDEV_DEF_SCROLL_LIMIT;
    driver->scroll_throw         = LV_INDEV_DEF_SCROLL_THROW;
    driver->long_press_time      = LV_INDEV_DEF_LONG_PRESS_TIME;
    driver->long_press_repeat_time  = LV_INDEV_DEF_LONG_PRESS_REP_TIME;
    driver->gesture_limit        = LV_INDEV_DEF_GESTURE_LIMIT;
    driver->gesture_min_velocity = LV_INDEV_DEF_GESTURE_MIN_VELOCITY;
}

/**
 * Register an initialized input device driver.
 * @param driver pointer to an initialized 'lv_indev_drv_t' variable.
 * Only pointer is saved, so the driver should be static or dynamically allocated.
 * @return pointer to the new input device or NULL on error
 */
lv_indev_t * lv_indev_drv_register(lv_indev_drv_t * driver)
{
    if(driver->disp == NULL) driver->disp = lv_disp_get_default();

    if(driver->disp == NULL) {
        LV_LOG_WARN("lv_indev_drv_register: no display registered hence can't attach the indev to "
                    "a display");
        return NULL;
    }

    lv_indev_t * indev = _lv_ll_ins_head(&LV_GC_ROOT(_lv_indev_ll));
    LV_ASSERT_MALLOC(indev);
    if(!indev) {
        return NULL;
    }

    lv_memzero(indev, sizeof(lv_indev_t));
    indev->driver = driver;

    indev->proc.reset_query  = 1;
    indev->driver->read_timer = lv_timer_create(lv_indev_read_timer_cb, LV_DEF_REFR_PERIOD, indev);

    return indev;
}

/**
 * Update the driver in run time.
 * @param indev pointer to a input device. (return value of `lv_indev_drv_register`)
 * @param new_drv pointer to the new driver
 */
void lv_indev_drv_update(lv_indev_t * indev, lv_indev_drv_t * new_drv)
{
    LV_ASSERT_NULL(indev);
    LV_ASSERT_NULL(indev->driver);
    LV_ASSERT_NULL(indev->driver->read_timer);
    lv_timer_del(indev->driver->read_timer);

    LV_ASSERT_NULL(new_drv);
    if(new_drv->disp == NULL) {
        new_drv->disp = lv_disp_get_default();
    }
    if(new_drv->disp == NULL) {
        LV_LOG_WARN("lv_indev_drv_register: no display registered hence can't attach the indev to "
                    "a display");
        indev->proc.disabled = true;
        return;
    }

    indev->driver = new_drv;
    indev->driver->read_timer = lv_timer_create(lv_indev_read_timer_cb, LV_DEF_REFR_PERIOD, indev);
    indev->proc.reset_query   = 1;
}

/**
* Remove the provided input device. Make sure not to use the provided input device afterwards anymore.
* @param indev pointer to delete
*/
void lv_indev_delete(lv_indev_t * indev)
{
    LV_ASSERT_NULL(indev);
    LV_ASSERT_NULL(indev->driver);
    LV_ASSERT_NULL(indev->driver->read_timer);
    /*Clean up the read timer first*/
    lv_timer_del(indev->driver->read_timer);
    /*Remove the input device from the list*/
    _lv_ll_remove(&LV_GC_ROOT(_lv_indev_ll), indev);
    /*Free the memory of the input device*/
    lv_free(indev);
}

/**
 * Get the next input device.
 * @param indev pointer to the current input device. NULL to initialize.
 * @return the next input devise or NULL if no more. Give the first input device when the parameter
 * is NULL
 */
lv_indev_t * lv_indev_get_next(lv_indev_t * indev)
{
    if(indev == NULL)
        return _lv_ll_get_head(&LV_GC_ROOT(_lv_indev_ll));
    else
        return _lv_ll_get_next(&LV_GC_ROOT(_lv_indev_ll), indev);
}

/**
 * Read data from an input device.
 * @param indev pointer to an input device
 * @param data input device will write its data here
 */
void _lv_indev_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_memzero(data, sizeof(lv_indev_data_t));

    /* For touchpad sometimes users don't set the last pressed coordinate on release.
     * So be sure a coordinates are initialized to the last point */
    if(indev->driver->type == LV_INDEV_TYPE_POINTER) {
        data->point.x = indev->proc.types.pointer.last_raw_point.x;
        data->point.y = indev->proc.types.pointer.last_raw_point.y;
    }
    /*Similarly set at least the last key in case of the user doesn't set it on release*/
    else if(indev->driver->type == LV_INDEV_TYPE_KEYPAD) {
        data->key = indev->proc.types.keypad.last_key;
    }
    /*For compatibility assume that used button was enter (encoder push)*/
    else if(indev->driver->type == LV_INDEV_TYPE_ENCODER) {
        data->key = LV_KEY_ENTER;
    }

    if(indev->driver->read_cb) {
        INDEV_TRACE("calling indev_read_cb");
        indev->driver->read_cb(indev->driver, data);
    }
    else {
        LV_LOG_WARN("indev_read_cb is not registered");
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
