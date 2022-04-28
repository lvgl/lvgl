//
// Created by Mariotaku on 2021/10/14.
//

#include "lv_drv_sdl_indev.h"
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
static void sdl_mousewheel_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint32_t keycode_to_ctrl_key(SDL_Keycode sdl_key);

/**********************
 *  STATIC VARIABLES
 **********************/

typedef struct _lv_drv_sdl_mouse_priv_t {
    bool left_button_down;
    int16_t last_x;
    int16_t last_y;
} _lv_drv_sdl_mouse_priv_t;

typedef struct _lv_drv_sdl_mousewheel_priv_t {
    int16_t diff;
    lv_indev_state_t state;
} _lv_drv_sdl_mousewheel_priv_t;

static char buf[KEYBOARD_BUFFER_SIZE];

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


void lv_drv_sdl_mouse_init(lv_drv_sdl_mouse_dsc_t * dsc)
{
    lv_memset_00(dsc, sizeof(lv_drv_sdl_mouse_dsc_t));
    return;
}

lv_indev_t * lv_drv_sdl_mouse_create(lv_drv_sdl_mouse_dsc_t * dsc)
{
    dsc->_priv = lv_mem_alloc(sizeof(lv_drv_sdl_mouse_dsc_t));
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

void lv_drv_sdl_mousewheel_init(lv_drv_sdl_mousewheel_dsc_t * dsc)
{
    lv_memset_00(dsc, sizeof(lv_drv_sdl_mouse_dsc_t));
    return;
}

lv_indev_t * lv_drv_sdl_mousewheel_create(lv_drv_sdl_mousewheel_dsc_t * dsc)
{
    dsc->_priv = lv_mem_alloc(sizeof(lv_drv_sdl_mousewheel_dsc_t));
    LV_ASSERT_MALLOC(dsc->_priv);

    lv_indev_drv_t * indev_drv = lv_mem_alloc(sizeof(lv_indev_drv_t));
    LV_ASSERT_MALLOC(indev_drv);

    if(dsc->_priv == NULL || indev_drv == NULL) {
        lv_mem_free(dsc->_priv);
        lv_mem_free(indev_drv);
        return NULL;
    }

    lv_memset_00(dsc->_priv, sizeof(_lv_drv_sdl_mousewheel_priv_t));

    lv_indev_drv_init(indev_drv);
    indev_drv->type = LV_INDEV_TYPE_ENCODER;
    indev_drv->read_cb = sdl_mousewheel_read;
    indev_drv->user_data = dsc;
    lv_indev_t * indev = lv_indev_drv_register(indev_drv);

    return indev;
}


/**
 * Get input from the keyboard.
 * @param indev_drv pointer to the related input device driver
 * @param data store the red data here
 */
void sdl_keyboard_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    (void) indev_drv;      /*Unused*/

    static bool dummy_read = false;
    const size_t len = strlen(buf);

    /*Send a release manually*/
    if(dummy_read) {
        dummy_read = false;
        data->state = LV_INDEV_STATE_RELEASED;
        data->continue_reading = len > 0;
    }
    /*Send the pressed character*/
    else if(len > 0) {
        dummy_read = true;
        data->state = LV_INDEV_STATE_PRESSED;
        data->key = buf[0];
        memmove(buf, buf + 1, len);
        data->continue_reading = true;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void sdl_mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    lv_drv_sdl_mouse_dsc_t * dsc = indev_drv->user_data;

    /*Store the collected data*/
    data->point.x = dsc->_priv->last_x;
    data->point.y = dsc->_priv->last_y;
    data->state = dsc->_priv->left_button_down ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

static void sdl_mousewheel_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    lv_drv_sdl_mousewheel_dsc_t * dsc = indev_drv->user_data;

    data->state = dsc->_priv->state;
    data->enc_diff = dsc->_priv->diff;
    dsc->_priv->diff = 0;
}


void _lv_sdl_mouse_handler(SDL_Event * event, lv_disp_t * disp)
{
    /*Find a suitable indev*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(indev->driver->disp == disp && lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
            break;
        }
        indev = lv_indev_get_next(indev);
    }

    if(indev == NULL) return;
    lv_drv_sdl_mouse_dsc_t * indev_dsc = indev->driver->user_data;
    lv_drv_sdl_disp_dsc_t * disp_dsc = indev->driver->disp->driver->user_data;

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


/**
 * It is called periodically from the SDL thread to check mouse wheel state
 * @param event describes the event
 */
void _lv_sdl_mousewheel_handler(SDL_Event * event, lv_disp_t * disp)
{
    /*Find a suitable indev*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(indev->driver->disp == disp && lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) {
            break;
        }
        indev = lv_indev_get_next(indev);
    }

    if(indev == NULL) return;
    lv_drv_sdl_mousewheel_dsc_t * indev_dsc = indev->driver->user_data;
    lv_drv_sdl_disp_dsc_t * disp_dsc = indev->driver->disp->driver->user_data;

    switch(event->type) {
        case SDL_MOUSEWHEEL:
            // Scroll down (y = -1) means positive encoder turn,
            // so invert it
#ifdef __EMSCRIPTEN__
            /*Escripten scales it wrong*/
            if(event->wheel.y < 0) indev_dsc->_priv->diff++;
            if(event->wheel.y > 0) indev_dsc->_priv->diff--;
#else
            indev_dsc->_priv->diff = -event->wheel.y;
#endif
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event->button.button == SDL_BUTTON_MIDDLE) {
                indev_dsc->_priv->state = LV_INDEV_STATE_PRESSED;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if(event->button.button == SDL_BUTTON_MIDDLE) {
                indev_dsc->_priv->state = LV_INDEV_STATE_RELEASED;
            }
            break;
        default:
            break;
    }
}

/**
 * Called periodically from the SDL thread, store text input or control characters in the buffer.
 * @param event describes the event
 */
void keyboard_handler(SDL_Event * event)
{
    /* We only care about SDL_KEYDOWN and SDL_TEXTINPUT events */
    switch(event->type) {
        case SDL_KEYDOWN: {                     /*Button press*/
                const uint32_t ctrl_key = keycode_to_ctrl_key(event->key.keysym.sym);
                if(ctrl_key == '\0')
                    return;
                const size_t len = strlen(buf);
                if(len < KEYBOARD_BUFFER_SIZE - 1) {
                    buf[len] = ctrl_key;
                    buf[len + 1] = '\0';
                }
                break;
            }
        case SDL_TEXTINPUT: {                   /*Text input*/
                const size_t len = strlen(buf) + strlen(event->text.text);
                if(len < KEYBOARD_BUFFER_SIZE - 1)
                    strcat(buf, event->text.text);
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


