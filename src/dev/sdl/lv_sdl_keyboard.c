/**
 * @file lv_sdl_keyboard.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_sdl_keyboard.h"
#if LV_USE_SDL

#include "../../hal/lv_hal_indev.h"
#include "../../core/lv_indev.h"
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
static void sdl_keyboard_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint32_t keycode_to_ctrl_key(SDL_Keycode sdl_key);

/**********************
 *  STATIC VARIABLES
 **********************/

typedef struct _lv_sdl_keyboard_priv_t {
    char buf[KEYBOARD_BUFFER_SIZE];
    bool dummy_read;
} _lv_sdl_keyboard_priv_t;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


lv_sdl_keyboard_t * lv_dev_sdl_keyboard_create(void)
{
    lv_sdl_keyboard_t * cfg = lv_malloc(sizeof(lv_sdl_keyboard_t));
    LV_ASSERT_MALLOC(cfg);
    if(cfg == NULL) return NULL;

    lv_memzero(cfg, sizeof(lv_sdl_keyboard_t));

    cfg->_priv = lv_malloc(sizeof(_lv_sdl_keyboard_priv_t));
    LV_ASSERT_MALLOC(cfg->_priv);

    if(cfg->_priv == NULL) {
        lv_free(cfg);
        return NULL;
    }

    lv_memzero(cfg->_priv, sizeof(_lv_sdl_keyboard_priv_t));
    return cfg;
}

lv_indev_t * lv_dev_sdl_keyboard_register(lv_sdl_keyboard_t * dev)
{
    lv_indev_drv_t * indev_drv = lv_malloc(sizeof(lv_indev_drv_t));
    LV_ASSERT_MALLOC(indev_drv);

    if(indev_drv == NULL) return NULL;

    lv_indev_drv_init(indev_drv);
    indev_drv->type = LV_INDEV_TYPE_KEYPAD;
    indev_drv->read_cb = sdl_keyboard_read;
    indev_drv->user_data = dev;
    lv_indev_t * indev = lv_indev_drv_register(indev_drv);

    return indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void sdl_keyboard_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    lv_sdl_keyboard_t * dev = indev_drv->user_data;

    const size_t len = strlen(dev->_priv->buf);

    /*Send a release manually*/
    if(dev->_priv->dummy_read) {
        dev->_priv->dummy_read = false;
        data->state = LV_INDEV_STATE_RELEASED;
        data->continue_reading = len > 0;
    }
    /*Send the pressed character*/
    else if(len > 0) {
        dev->_priv->dummy_read = true;
        data->state = LV_INDEV_STATE_PRESSED;
        data->key = dev->_priv->buf[0];
        memmove(dev->_priv->buf, dev->_priv->buf + 1, len);
        data->continue_reading = true;
    }
}

void _lv_sdl_keyboard_handler(SDL_Event * event)
{
    uint32_t win_id = UINT32_MAX;
    switch(event->type) {
        case SDL_KEYDOWN:
            win_id = event->key.windowID;
            break;
        case SDL_TEXTINPUT:
            win_id = event->text.windowID;
            break;
        default:
            return;
    }

    lv_disp_t * disp = _lv_sdl_get_disp_from_win_id(win_id);

    /*Find a suitable indev*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(indev->driver->disp == disp && lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
            break;
        }
        indev = lv_indev_get_next(indev);
    }

    if(indev == NULL) return;
    lv_sdl_keyboard_t * indev_dev = indev->driver->user_data;


    /* We only care about SDL_KEYDOWN and SDL_TEXTINPUT events */
    switch(event->type) {
        case SDL_KEYDOWN: {                     /*Button press*/
                const uint32_t ctrl_key = keycode_to_ctrl_key(event->key.keysym.sym);
                if(ctrl_key == '\0')
                    return;
                const size_t len = strlen(indev_dev->_priv->buf);
                if(len < KEYBOARD_BUFFER_SIZE - 1) {
                    indev_dev->_priv->buf[len] = ctrl_key;
                    indev_dev->_priv->buf[len + 1] = '\0';
                }
                break;
            }
        case SDL_TEXTINPUT: {                   /*Text input*/
                const size_t len = strlen(indev_dev->_priv->buf) + strlen(event->text.text);
                if(len < KEYBOARD_BUFFER_SIZE - 1)
                    strcat(indev_dev->_priv->buf, event->text.text);
            }
            break;
        default:
            break;

    }
}


/**
 * Convert a SDL key code to it's LV_KEY_* counterpart or return '\0' if it's not a control character.
 * @param sdl_key the key code
 * @return LV_KEY_* control character or '\0'
 */
static uint32_t keycode_to_ctrl_key(SDL_Keycode sdl_key)
{
    /*Remap some key to LV_KEY_... to manage groups*/
    switch(sdl_key) {
        case SDLK_RIGHT:
        case SDLK_KP_PLUS:
            return LV_KEY_RIGHT;

        case SDLK_LEFT:
        case SDLK_KP_MINUS:
            return LV_KEY_LEFT;

        case SDLK_UP:
            return LV_KEY_UP;

        case SDLK_DOWN:
            return LV_KEY_DOWN;

        case SDLK_ESCAPE:
            return LV_KEY_ESC;

        case SDLK_BACKSPACE:
            return LV_KEY_BACKSPACE;

        case SDLK_DELETE:
            return LV_KEY_DEL;

        case SDLK_KP_ENTER:
        case '\r':
            return LV_KEY_ENTER;

        case SDLK_TAB:
        case SDLK_PAGEDOWN:
            return LV_KEY_NEXT;

        case SDLK_PAGEUP:
            return LV_KEY_PREV;

        default:
            return '\0';
    }
}

#endif /*LV_USE_SDL*/
