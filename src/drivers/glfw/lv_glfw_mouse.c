/**
 * @file lv_glfw_mouse.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_glfw_mouse.h"
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
static void opengles_mouse_read(lv_indev_t * indev, lv_indev_data_t * data);
static void release_indev_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_glfw_mouse_create(void)
{
    lv_glfw_mouse_t * dsc = lv_malloc_zeroed(sizeof(lv_glfw_mouse_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    lv_indev_t * indev = lv_indev_create();
    LV_ASSERT_MALLOC(indev);
    if(indev == NULL) {
        lv_free(dsc);
        return NULL;
    }

    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, opengles_mouse_read);
    lv_indev_set_driver_data(indev, dsc);

    lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);
    lv_indev_add_event_cb(indev, release_indev_cb, LV_EVENT_DELETE, indev);

    return indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void opengles_mouse_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_glfw_mouse_t * dsc = lv_indev_get_driver_data(indev);

    /*Store the collected data*/
    data->point.x = dsc->last_x;
    data->point.y = dsc->last_y;
    data->state = dsc->left_button_down ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    data->enc_diff = dsc->diff;
    dsc->diff = 0;
}

static void release_indev_cb(lv_event_t * e)
{
    lv_indev_t * indev = (lv_indev_t *) lv_event_get_user_data(e);
    lv_glfw_mouse_t * dsc = lv_indev_get_driver_data(indev);
    if(dsc) {
        lv_indev_set_driver_data(indev, NULL);
        lv_indev_set_read_cb(indev, NULL);
        lv_free(dsc);
        LV_LOG_INFO("done");
    }
}

#endif /* LV_USE_OPENGLES */
