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
#include "misc/mem/linked_list.h"

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
 * Register Input Device driver
 *
 * @param driver Input Device driver structure
 * @return pointer to the new input device
 */
lv_indev_t * lv_indev_drv_register(lv_hal_indev_drv_t *driver)
{
    lv_indev_t *node;

    node = dm_alloc(sizeof(lv_indev_t));
    if (!node) return NULL;

    memcpy(&node->drv, driver, sizeof(lv_hal_indev_drv_t));

    node->next = NULL;

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
        if(indev_list->next == NULL) return NULL;
        else return indev_list->next;
    }
}

/**
 * Read data from an input device.
 * @param indev pointer to an input device
 * @param data input device will write its data here
 * @return false: no more data; true: there more data to read (buffered)
 */
bool lv_indev_get(lv_indev_t * indev, lv_hal_indev_data_t *data)
{
    bool cont = false;

    if(indev->drv.get_data) {
        cont = indev->drv.get_data(data);
    } else {
        memset(data, 0, sizeof(lv_hal_indev_data_t));
    }

    return cont;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
