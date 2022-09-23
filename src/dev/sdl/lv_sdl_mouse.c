/**
 * @file lv_sdl_mouse.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_sdl_mouse.h"
#if LV_USE_SDL

#include "../../hal/lv_hal_indev.h"
#include "../../core/lv_group.h"

#include LV_SDL_INCLUDE_PATH
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

typedef struct _lv_sdl_mouse_priv_t {
    int16_t last_x;
    int16_t last_y;
    bool left_button_down;
} _lv_sdl_mouse_priv_t;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_sdl_mouse_t * lv_sdl_mouse_create(void)
{
    lv_sdl_mouse_t * cfg = lv_malloc(sizeof(lv_sdl_mouse_t));
    LV_ASSERT_MALLOC(cfg);
    if(cfg == NULL) return NULL;

    lv_memzero(cfg, sizeof(lv_sdl_mouse_t));

    cfg->_priv = lv_malloc(sizeof(_lv_sdl_mouse_priv_t));
    LV_ASSERT_MALLOC(cfg->_priv);

    if(cfg->_priv == NULL) {
        lv_free(cfg);
        return NULL;
    }

    lv_memzero(cfg->_priv, sizeof(_lv_sdl_mouse_priv_t));
    return cfg;
}

lv_indev_t * lv_sdl_mouse_register(lv_sdl_mouse_t * cfg)
{
    lv_indev_drv_t * indev_drv = lv_malloc(sizeof(lv_indev_drv_t));
    LV_ASSERT_MALLOC(indev_drv);

    if(indev_drv == NULL) return NULL;

    lv_indev_drv_init(indev_drv);
    indev_drv->type = LV_INDEV_TYPE_POINTER;
    indev_drv->read_cb = sdl_mouse_read;
    indev_drv->user_data = cfg;
    lv_indev_t * indev = lv_indev_drv_register(indev_drv);

    return indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void sdl_mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    lv_sdl_mouse_t * dev = indev_drv->user_data;

    /*Store the collected data*/
    data->point.x = dev->_priv->last_x;
    data->point.y = dev->_priv->last_y;
    data->state = dev->_priv->left_button_down ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
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

    lv_disp_t * disp = _lv_sdl_get_disp_from_win_id(win_id);

    /*Find a suitable indev*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(indev->driver->disp == disp && lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
            break;
        }
        indev = lv_indev_get_next(indev);
    }

    if(indev == NULL) return;
    lv_sdl_mouse_t * indev_dev = indev->driver->user_data;
    lv_sdl_window_t * disp_dev = indev->driver->disp->driver->user_data;

    switch(event->type) {
        case SDL_MOUSEBUTTONUP:
            if(event->button.button == SDL_BUTTON_LEFT)
                indev_dev->_priv->left_button_down = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event->button.button == SDL_BUTTON_LEFT) {
                indev_dev->_priv->left_button_down = true;
                indev_dev->_priv->last_x = event->motion.x / disp_dev->zoom;
                indev_dev->_priv->last_y = event->motion.y / disp_dev->zoom;
            }
            break;
        case SDL_MOUSEMOTION:
            indev_dev->_priv->last_x = event->motion.x / disp_dev->zoom;
            indev_dev->_priv->last_y = event->motion.y / disp_dev->zoom;
            break;

        case SDL_FINGERUP:
            indev_dev->_priv->left_button_down = false;
            indev_dev->_priv->last_x = disp_dev->hor_res * event->tfinger.x / disp_dev->zoom;
            indev_dev->_priv->last_y = disp_dev->ver_res * event->tfinger.y / disp_dev->zoom;
            break;
        case SDL_FINGERDOWN:
            indev_dev->_priv->left_button_down = true;
            indev_dev->_priv->last_x = disp_dev->hor_res * event->tfinger.x / disp_dev->zoom;
            indev_dev->_priv->last_y = disp_dev->ver_res * event->tfinger.y / disp_dev->zoom;
            break;
        case SDL_FINGERMOTION:
            indev_dev->_priv->last_x = disp_dev->hor_res * event->tfinger.x / disp_dev->zoom;
            indev_dev->_priv->last_y = disp_dev->ver_res * event->tfinger.y / disp_dev->zoom;
            break;
    }
}

#endif /*LV_USE_SDL*/
