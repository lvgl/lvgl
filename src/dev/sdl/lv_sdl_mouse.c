/**
 * @file lv_sdl_mouse.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_sdl_mouse.h"
#if LV_USE_SDL

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
static void sdl_mouse_read(lv_indev_t * indev, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/

typedef struct {
    int16_t last_x;
    int16_t last_y;
    bool left_button_down;
} lv_sdl_mouse_t;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_sdl_mouse_create(void)
{
    lv_sdl_mouse_t * dsc = lv_malloc(sizeof(lv_sdl_mouse_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;
    lv_memzero(dsc, sizeof(lv_sdl_mouse_t));

    lv_indev_t * indev = lv_indev_create();
    LV_ASSERT_MALLOC(indev);
    if(indev == NULL) {
        lv_free(dsc);
        return NULL;
    }

    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, sdl_mouse_read);
    lv_indev_set_driver_data(indev, dsc);

    return indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void sdl_mouse_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_sdl_mouse_t * dsc = lv_indev_get_driver_data(indev);

    /*Store the collected data*/
    data->point.x = dsc->last_x;
    data->point.y = dsc->last_y;
    data->state = dsc->left_button_down ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
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
#if SDL_VERSION_ATLEAST(2,0,12)
            win_id = event->tfinger.windowID;
#endif
            break;
        default:
            return;
    }

    lv_disp_t * disp = _lv_sdl_get_disp_from_win_id(win_id);

    /*Find a suitable indev*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(lv_indev_get_disp(indev) == disp && lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
            break;
        }
        indev = lv_indev_get_next(indev);
    }

    if(indev == NULL) return;
    lv_sdl_mouse_t * indev_dev = lv_indev_get_driver_data(indev);

    lv_coord_t hor_res = lv_disp_get_hor_res(disp);
    lv_coord_t ver_res = lv_disp_get_ver_res(disp);
    uint8_t zoom = lv_sdl_window_get_zoom(disp);

    switch(event->type) {
        case SDL_MOUSEBUTTONUP:
            if(event->button.button == SDL_BUTTON_LEFT)
                indev_dev->left_button_down = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event->button.button == SDL_BUTTON_LEFT) {
                indev_dev->left_button_down = true;
                indev_dev->last_x = event->motion.x / zoom;
                indev_dev->last_y = event->motion.y / zoom;
            }
            break;
        case SDL_MOUSEMOTION:
            indev_dev->last_x = event->motion.x / zoom;
            indev_dev->last_y = event->motion.y / zoom;
            break;

        case SDL_FINGERUP:
            indev_dev->left_button_down = false;
            indev_dev->last_x = hor_res * (lv_coord_t)event->tfinger.x / zoom;
            indev_dev->last_y = ver_res * (lv_coord_t)event->tfinger.y / zoom;
            break;
        case SDL_FINGERDOWN:
            indev_dev->left_button_down = true;
            indev_dev->last_x = hor_res * (lv_coord_t)event->tfinger.x / zoom;
            indev_dev->last_y = ver_res * (lv_coord_t)event->tfinger.y / zoom;
            break;
        case SDL_FINGERMOTION:
            indev_dev->last_x = hor_res * (lv_coord_t)event->tfinger.x / zoom;
            indev_dev->last_y = ver_res * (lv_coord_t)event->tfinger.y / zoom;
            break;
    }
}

#endif /*LV_USE_SDL*/
