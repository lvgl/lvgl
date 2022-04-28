//
// Created by Mariotaku on 2021/10/14.
//

#include <lvgl/src/dev/sdl/lv_dev_sdl_mouse.h>
#include "../../../src/hal/lv_hal_indev.h"
#include "../../../src/core/lv_group.h"

#include LV_PLATFORM_SDL_INCLUDE_PATH
/*********************
 *      DEFINES
 *********************/

#ifndef KEYBOARD_BUFFER_SIZE
    #define KEYBOARD_BUFFER_SIZE 32
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void sdl_mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/

typedef struct _lv_dev_sdl_mouse_priv_t {
    int16_t last_x;
    int16_t last_y;
    bool left_button_down;
} _lv_drv_sdl_mouse_priv_t;

typedef struct _lv_dev_sdl_mousewheel_priv_t {
    int16_t diff;
    lv_indev_state_t state;
} _lv_drv_sdl_mousewheel_priv_t;

static char buf[KEYBOARD_BUFFER_SIZE];

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


void lv_dev_sdl_mouse_init(lv_dev_sdl_mouse_dsc_t * dsc)
{
    lv_memset_00(dsc, sizeof(lv_dev_sdl_mouse_dsc_t));
    return;
}

lv_indev_t * lv_dev_sdl_mouse_create(lv_dev_sdl_mouse_dsc_t * dsc)
{
    dsc->_priv = lv_mem_alloc(sizeof(lv_dev_sdl_mouse_dsc_t));
    LV_ASSERT_MALLOC(dsc->_priv);

    lv_indev_drv_t * indev_drv = lv_mem_alloc(sizeof(lv_indev_drv_t));
    LV_ASSERT_MALLOC(indev_drv);

    if(dsc->_priv == NULL || indev_drv == NULL) {
        lv_mem_free(dsc->_priv);
        lv_mem_free(indev_drv);
        return NULL;
    }

    lv_memset_00(dsc->_priv, sizeof(_lv_drv_sdl_mouse_priv_t));

    lv_indev_drv_init(indev_drv);
    indev_drv->type = LV_INDEV_TYPE_POINTER;
    indev_drv->read_cb = sdl_mouse_read;
    indev_drv->user_data = dsc;
    lv_indev_t * indev = lv_indev_drv_register(indev_drv);

    return indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void sdl_mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    lv_dev_sdl_mouse_dsc_t * dsc = indev_drv->user_data;

    /*Store the collected data*/
    data->point.x = dsc->_priv->last_x;
    data->point.y = dsc->_priv->last_y;
    data->state = dsc->_priv->left_button_down ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}


void _lv_sdl_mouse_handler(SDL_Event * event)
{
    uint32_t win_id = UINT32_MAX;
    switch(event->type) {
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            win_id = event->button.windowID;
            break;
        case SDL_MOUSEMOTION:
            win_id = event->motion.windowID;
            break;

        case SDL_FINGERUP:
        case SDL_FINGERDOWN:
        case SDL_FINGERMOTION:
            win_id = event->tfinger.windowID;
            break;
        default:
            return;
    }

    lv_disp_t * disp = lv_dev_sdl_get_from_win_id(win_id);

    /*Find a suitable indev*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(indev->driver->disp == disp && lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
            break;
        }
        indev = lv_indev_get_next(indev);
    }

    if(indev == NULL) return;
    lv_dev_sdl_mouse_dsc_t * indev_dsc = indev->driver->user_data;
    lv_dev_sdl_window_dsc_t * disp_dsc = indev->driver->disp->driver->user_data;

    switch(event->type) {
        case SDL_MOUSEBUTTONUP:
            if(event->button.button == SDL_BUTTON_LEFT)
                indev_dsc->_priv->left_button_down = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event->button.button == SDL_BUTTON_LEFT) {
                indev_dsc->_priv->left_button_down = true;
                indev_dsc->_priv->last_x = event->motion.x / disp_dsc->zoom;
                indev_dsc->_priv->last_y = event->motion.y / disp_dsc->zoom;
            }
            break;
        case SDL_MOUSEMOTION:
            indev_dsc->_priv->last_x = event->motion.x / disp_dsc->zoom;
            indev_dsc->_priv->last_y = event->motion.y / disp_dsc->zoom;
            break;

        case SDL_FINGERUP:
            indev_dsc->_priv->left_button_down = false;
            indev_dsc->_priv->last_x = disp_dsc->hor_res * event->tfinger.x / disp_dsc->zoom;
            indev_dsc->_priv->last_y = disp_dsc->ver_res * event->tfinger.y / disp_dsc->zoom;
            break;
        case SDL_FINGERDOWN:
            indev_dsc->_priv->left_button_down = true;
            indev_dsc->_priv->last_x = disp_dsc->hor_res * event->tfinger.x / disp_dsc->zoom;
            indev_dsc->_priv->last_y = disp_dsc->ver_res * event->tfinger.y / disp_dsc->zoom;
            break;
        case SDL_FINGERMOTION:
            indev_dsc->_priv->last_x = disp_dsc->hor_res * event->tfinger.x / disp_dsc->zoom;
            indev_dsc->_priv->last_y = disp_dsc->ver_res * event->tfinger.y / disp_dsc->zoom;
            break;
    }
}
