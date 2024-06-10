/**
 * @file lv_glfw_mouse_private.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#include "lv_glfw_mouse_private.h"
#if LV_USE_OPENGLES

#include <stdint.h>
#include <stdbool.h>
#include "../../core/lv_group.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_glfw_mouse_move_handler(lv_display_t * disp, int x, int y)
{
    if(disp == NULL) {
        return;
    }

    /*Find a suitable indev*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(lv_indev_get_display(indev) == disp && lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
            break;
        }
        indev = lv_indev_get_next(indev);
    }

    if(indev == NULL) return;
    lv_glfw_mouse_t * indev_dev = lv_indev_get_driver_data(indev);
    if(indev_dev == NULL) return;

    indev_dev->last_x = x;
    indev_dev->last_y = y;

    lv_indev_read(indev);
}

void lv_glfw_mouse_btn_handler(lv_display_t * disp, int btn_down)
{
    if(disp == NULL) {
        return;
    }

    /*Find a suitable indev*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(lv_indev_get_display(indev) == disp && lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
            break;
        }
        indev = lv_indev_get_next(indev);
    }

    if(indev == NULL) return;
    lv_glfw_mouse_t * indev_dev = lv_indev_get_driver_data(indev);
    if(indev_dev == NULL) return;

    indev_dev->left_button_down = btn_down;

    lv_indev_read(indev);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_OPENGLES */
