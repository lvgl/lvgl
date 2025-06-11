/**
 * @file lv_wl_pointer_axis.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wl_pointer_axis.h"

#if LV_USE_WAYLAND

#include "lv_wayland_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void pointeraxis_read(lv_indev_t * drv, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_wayland_pointer_axis_create(void)
{

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(indev, pointeraxis_read);

    return indev;
}

lv_indev_t * lv_wayland_get_pointeraxis(lv_display_t * display)
{
    struct window * window = lv_display_get_user_data(display);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_pointeraxis;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void pointeraxis_read(lv_indev_t * drv, lv_indev_data_t * data)
{
    struct window * window = lv_display_get_user_data(lv_indev_get_display(drv));

    if(!window || window->closed) {
        return;
    }

    data->state    = window->body->input.pointer.wheel_button;
    data->enc_diff = window->body->input.pointer.wheel_diff;

    window->body->input.pointer.wheel_diff = 0;
}

#endif /* LV_USE_WAYLAND */
