/**
 * @file lv_opengles_mouse.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_opengles_mouse.h"
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

typedef struct {
    int16_t last_x;
    int16_t last_y;
    bool left_button_down;
    int32_t diff;
} lv_opengles_mouse_t;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_opengles_mouse_create(void)
{
    lv_opengles_mouse_t * dsc = lv_malloc_zeroed(sizeof(lv_opengles_mouse_t));
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
    lv_opengles_mouse_t * dsc = lv_indev_get_driver_data(indev);

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
    lv_opengles_mouse_t * dsc = lv_indev_get_driver_data(indev);
    if(dsc) {
        lv_indev_set_driver_data(indev, NULL);
        lv_indev_set_read_cb(indev, NULL);
        lv_free(dsc);
        LV_LOG_INFO("done");
    }
}

void _lv_opengles_mouse_move_handler(lv_display_t * disp, int x, int y)
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
    lv_opengles_mouse_t * indev_dev = lv_indev_get_driver_data(indev);
    if(indev_dev == NULL) return;

    indev_dev->last_x = x;
    indev_dev->last_y = y;

    lv_indev_read(indev);
}

void _lv_opengles_mouse_btn_handler(lv_display_t * disp, int btn_down)
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
    lv_opengles_mouse_t * indev_dev = lv_indev_get_driver_data(indev);
    if(indev_dev == NULL) return;

    indev_dev->left_button_down = btn_down;

    lv_indev_read(indev);
}

#endif /* LV_USE_OPENGLES */
