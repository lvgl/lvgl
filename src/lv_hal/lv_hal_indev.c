/**
 * @file hal_indev.c
 *
 * @description Input device HAL interface
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_misc/lv_debug.h"
#include "../lv_hal/lv_hal_indev.h"
#include "../lv_core/lv_indev.h"
#include "../lv_misc/lv_mem.h"
#include "../lv_misc/lv_gc.h"
#include "lv_hal_disp.h"

#if defined(LV_GC_INCLUDE)
    #include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
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
    _lv_memset_00(driver, sizeof(lv_indev_drv_t));

    driver->type                 = LV_INDEV_TYPE_NONE;
    driver->drag_limit           = LV_INDEV_DEF_DRAG_LIMIT;
    driver->drag_throw           = LV_INDEV_DEF_DRAG_THROW;
    driver->long_press_time      = LV_INDEV_DEF_LONG_PRESS_TIME;
    driver->long_press_rep_time  = LV_INDEV_DEF_LONG_PRESS_REP_TIME;
    driver->gesture_limit        = LV_INDEV_DEF_GESTURE_LIMIT;
    driver->gesture_min_velocity = LV_INDEV_DEF_GESTURE_MIN_VELOCITY;
}

/**
 * Register an initialized input device driver.
 * @param driver pointer to an initialized 'lv_indev_drv_t' variable (can be local variable)
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
    if(!indev) {
        LV_ASSERT_MEM(indev);
        return NULL;
    }

    _lv_memset_00(indev, sizeof(lv_indev_t));
    _lv_memcpy(&indev->driver, driver, sizeof(lv_indev_drv_t));

    indev->proc.reset_query = 1;
    indev->cursor           = NULL;
    indev->group            = NULL;
    indev->btn_points       = NULL;

    indev->driver.read_task = lv_task_create(_lv_indev_read_task, LV_INDEV_DEF_READ_PERIOD, LV_TASK_PRIO_HIGH, indev);

    return indev;
}

/**
 * Update the driver in run time.
 * @param indev pointer to a input device. (return value of `lv_indev_drv_register`)
 * @param new_drv pointer to the new driver
 */
void lv_indev_drv_update(lv_indev_t * indev, lv_indev_drv_t * new_drv)
{
    memcpy(&indev->driver, new_drv, sizeof(lv_indev_drv_t));
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
 * @return false: no more data; true: there more data to read (buffered)
 */
bool _lv_indev_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    bool cont = false;

    _lv_memset_00(data, sizeof(lv_indev_data_t));

    /* For touchpad sometimes users don't the last pressed coordinate on release.
     * So be sure a coordinates are initialized to the last point */
    if(indev->driver.type == LV_INDEV_TYPE_POINTER) {
        data->point.x = indev->proc.types.pointer.act_point.x;
        data->point.y = indev->proc.types.pointer.act_point.y;
    }
    /*Similarly set at least the last key in case of the  the user doesn't set it  on release*/
    else if(indev->driver.type == LV_INDEV_TYPE_KEYPAD) {
        data->key = indev->proc.types.keypad.last_key;
    }
    /*For compatibility assume that used button was enter (encoder push) */
    else if(indev->driver.type == LV_INDEV_TYPE_ENCODER) {
        data->key = LV_KEY_ENTER;
        data->enc_diff = 0;
    }

    if(indev->driver.read_cb) {
        LV_LOG_TRACE("idnev read started");
        cont = indev->driver.read_cb(&indev->driver, data);
        LV_LOG_TRACE("idnev read finished");
    }
    else {
        LV_LOG_WARN("indev function registered");
    }

    return cont;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
