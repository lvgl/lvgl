/**
 * @file hal_indev.c
 *
 * @description Input device HAL interface
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_hal/lv_hal_indev.h"
#include "../lv_misc/lv_mem.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_indev_t *indev_list = NULL;

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
void lv_indev_drv_init(lv_indev_drv_t *driver)
{
    driver->read = NULL;
    driver->type = LV_INDEV_TYPE_NONE;
    driver->user_data = NULL;
}

/**
 * Register an initialized input device driver.
 * @param driver pointer to an initialized 'lv_indev_drv_t' variable (can be local variable)
 * @return pointer to the new input device or NULL on error
 */
lv_indev_t * lv_indev_drv_register(lv_indev_drv_t *driver)
{
    lv_indev_t *node;

    node = lv_mem_alloc(sizeof(lv_indev_t));
    if (!node) return NULL;

    memset(node, 0, sizeof(lv_indev_t));
    memcpy(&node->driver, driver, sizeof(lv_indev_drv_t));

    node->next = NULL;
    node->proc.reset_query = 1;
    node->cursor = NULL;
    node->group = NULL;
    node->btn_points = NULL;

    if (indev_list == NULL) {
        indev_list = node;
    } else {
        lv_indev_t *last = indev_list;
        while (last->next)
            last = last->next;

        last->next = node;
    }

    return node;
}

/**
 * Get the next input device.
 * @param indev pointer to the current input device. NULL to initialize.
 * @return the next input devise or NULL if no more. Give the first input device when the parameter is NULL
 */
lv_indev_t * lv_indev_next(lv_indev_t * indev)
{

    if(indev == NULL) {
        return indev_list;
    } else {
        if(indev->next == NULL) return NULL;
        else return indev->next;
    }
}

/**
 * Read data from an input device.
 * @param indev pointer to an input device
 * @param data input device will write its data here
 * @return false: no more data; true: there more data to read (buffered)
 */
bool lv_indev_read(lv_indev_t * indev, lv_indev_data_t *data)
{
    bool cont = false;

    if(indev->driver.read) {
        data->user_data = indev->driver.user_data;
        cont = indev->driver.read(data);
    } else {
        memset(data, 0, sizeof(lv_indev_data_t));
    }

    return cont;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
