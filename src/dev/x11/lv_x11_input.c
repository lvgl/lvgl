/**
 * @file lv_sdl_window.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_x11.h"

#if LV_USE_X11

#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../../stdlib/lv_string.h"
#include "../../widgets/image/lv_image.h"
#include "../../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/
#define X11_KEYBOARD_BUFFER_SIZE 64

/**********************
 *      TYPEDEFS
 **********************/
struct lv_x11_inp_data;

typedef struct {
    Display* display;
    struct lv_x11_inp_data* inp_data;
} x11_disp_user_hdr_t;

typedef struct lv_x11_inp_data {
    /* X11 Display */
    Display*    display;
    /* LVGL related information */
    lv_timer_t* inp_timer;
    /* user input related information */
    char        kb_buffer[X11_KEYBOARD_BUFFER_SIZE];
    lv_point_t  mouse_pos;
    bool        left_mouse_btn;
    bool        right_mouse_btn;
    bool        wheel_mouse_btn;
    int16_t     wheel_cnt;
} x11_inp_data_t;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * X11 input event handler, only handling input related events (MotionNotify, ButtonPress/Release, KeyPress/Release)
 */
static int is_inp_event(Display* disp, XEvent* evt, XPointer arg) { LV_UNUSED(disp); LV_UNUSED(arg); return !(evt->type == Expose || evt->type == ResizeRequest || evt->type == ClientMessage); }
static void lv_x11_inp_event_handler(lv_timer_t* t)
{
    x11_inp_data_t* xd = (x11_inp_data_t*)lv_timer_get_user_data(t);

    XEvent myevent;
    KeySym mykey;
    int n;

    /* handle all outstanding X events */
    while (XCheckIfEvent(xd->display, &myevent, is_inp_event, NULL)) {
        LV_LOG_TRACE("Input Event %d", myevent.type);
        switch(myevent.type) {
        case MotionNotify:
            xd->mouse_pos.x = myevent.xmotion.x;
            xd->mouse_pos.y = myevent.xmotion.y;
            break;
        case ButtonPress:
            switch (myevent.xbutton.button) {
            case Button1:
                xd->left_mouse_btn = true;
                break;
            case Button2:
                xd->wheel_mouse_btn = true;
                break;
            case Button3:
                xd->right_mouse_btn = true;
                break;
            case Button4:
                xd->wheel_cnt--; // Scrolled up
                break;
            case Button5:
                xd->wheel_cnt++; // Scrolled down
                break;
            default:
                LV_LOG_WARN("unhandled button press : %d", myevent.xbutton.button);
            }
            break;
        case ButtonRelease:
            switch (myevent.xbutton.button) {
            case Button1:
                xd->left_mouse_btn = false;
                break;
            case Button2:
                xd->wheel_mouse_btn = false;
                break;
            case Button3:
                xd->right_mouse_btn = false;
                break;
            }
            break;
        case KeyPress:
            n = XLookupString(&myevent.xkey, &xd->kb_buffer[0], sizeof(xd->kb_buffer), &mykey, NULL);
            xd->kb_buffer[n] = '\0';
            break;
        case KeyRelease:
            break;
        default:
            LV_LOG_WARN("unhandled x11 event: %d", myevent.type);
        }
    }
}

/**
 * create the local data/timers for the X11 input functionality. *
 * @param disp     the created X11 display object from @lv_x11_window_create
 * @return         pointer to the local user data object @x11_inp_data_t
 */
static x11_inp_data_t* lv_x11_input_locals(lv_display_t* disp) {
    /* get local input data set */
    x11_disp_user_hdr_t* disp_hdr = lv_display_get_driver_data(disp);
    x11_inp_data_t** inp_data = &disp_hdr->inp_data;
    if (NULL == *inp_data) {
        /* initial call -> create input data set */
        *inp_data = lv_malloc_zeroed(sizeof(x11_inp_data_t));
        LV_ASSERT_MALLOC(*inp_data);
        if (NULL != *inp_data) {
            /* keep X11 display information in local input data */
            (*inp_data)->display = disp_hdr->display;
            /* initialize timer callback for X11 kb/mouse input event reading */
            (*inp_data)->inp_timer = lv_timer_create(lv_x11_inp_event_handler, 1, *inp_data);
        }
    }
    return *inp_data;
}


static void lv_x11_keyboard_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{
    x11_inp_data_t* xd = (x11_inp_data_t*)lv_indev_get_driver_data(indev);
    LV_ASSERT_NULL(xd);

    size_t len = strlen(xd->kb_buffer);
    if (len > 0) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->key   = xd->kb_buffer[0];
        memmove(xd->kb_buffer, xd->kb_buffer + 1, len);
        data->continue_reading = (len > 0);
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void lv_x11_mouse_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{
    x11_inp_data_t* xd = (x11_inp_data_t*)lv_indev_get_driver_data(indev);
    LV_ASSERT_NULL(xd);

    data->point = xd->mouse_pos;
    data->state = xd->left_mouse_btn ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

static void lv_x11_mousewheel_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{
    x11_inp_data_t* xd = (x11_inp_data_t*)lv_indev_get_driver_data(indev);
    LV_ASSERT_NULL(xd);

    data->state    = xd->wheel_mouse_btn ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    data->enc_diff = xd->wheel_cnt;
    xd->wheel_cnt  = 0;
}


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t* lv_x11_keyboard_create(lv_display_t* disp)
{
    lv_indev_t* indev = NULL;
    x11_inp_data_t* inp_user_data = lv_x11_input_locals(disp);
    if (inp_user_data != NULL) {
        indev = lv_indev_create();
        LV_ASSERT_OBJ(indev, MY_CLASS);
        if (indev != NULL) {
            lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
            lv_indev_set_read_cb(indev, lv_x11_keyboard_read_cb);
            lv_indev_set_driver_data(indev, inp_user_data);
        }
    }
    return indev;
}

lv_indev_t* lv_x11_mouse_create(lv_display_t* disp, lv_image_dsc_t const* symb)
{
    lv_indev_t* indev = NULL;
    x11_inp_data_t* inp_user_data = lv_x11_input_locals(disp);
    if (inp_user_data != NULL) {
        indev = lv_indev_create();
        LV_ASSERT_OBJ(indev, MY_CLASS);
        if (indev != NULL) {
            lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
            lv_indev_set_read_cb(indev, lv_x11_mouse_read_cb);
            lv_indev_set_driver_data(indev, inp_user_data);

            /* optional mouse cursor symbol */
            if (NULL != symb) {
                lv_obj_t* mouse_cursor = lv_image_create(lv_screen_active());
                lv_image_set_src(mouse_cursor, symb);
                lv_indev_set_cursor(indev, mouse_cursor);
            }
        }
    }
    return indev;
}

lv_indev_t* lv_x11_mousewheel_create(lv_display_t* disp)
{
    lv_indev_t* indev = NULL;
    x11_inp_data_t* inp_user_data = lv_x11_input_locals(disp);
    if (inp_user_data != NULL) {
        indev = lv_indev_create();
        LV_ASSERT_OBJ(indev, MY_CLASS);
        if (indev != NULL) {
            lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
            lv_indev_set_read_cb(indev, lv_x11_mousewheel_read_cb);
            lv_indev_set_driver_data(indev, inp_user_data);
        }
    }
    return indev;
}

#endif /*LV_USE_X11*/
