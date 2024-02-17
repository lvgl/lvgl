/**
 * @file lv_x11_input.c
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
#define MIN(A, B) ((A) < (B) ? (A) : (B))

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _x11_inp_data {
    /* LVGL related information */
    lv_group_t * inp_group;      /**< input group for X input elements */
    lv_indev_t * keyboard;       /**< keyboard input device object */
    lv_indev_t * mousepointer;   /**< mouse input device object */
    lv_indev_t * mousewheel;     /**< encoder input device object */
    lv_timer_t * timer;          /**< timer object for @ref x11_event_handler */
    /* user input related information */
    char         kb_buffer[32];   /**< keyboard buffer for X keyboard inpputs */
    lv_point_t   mouse_pos;       /**< current reported mouse position */
    bool         left_mouse_btn;  /**< current state of left mouse button */
    bool         right_mouse_btn; /**< current state of right mouse button */
    bool         wheel_mouse_btn; /**< current state of wheel (=middle) mouse button */
    int16_t      wheel_cnt;       /**< mouse wheel increments */
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
 * X11 input event handler, predicated to fetch and handle only input related events
 * (MotionNotify, ButtonPress/Release, KeyPress/Release)
 */
static int is_inp_event(Display * disp, XEvent * event, XPointer arg)
{
    LV_UNUSED(disp);
    LV_UNUSED(arg);
    return !(event->type == Expose
             || (event->type >= DestroyNotify && event->type <= CirculateNotify) /* events from StructureNotifyMask */
             ||  event->type == ClientMessage);
}
static void x11_inp_event_handler(lv_timer_t * t)
{
    lv_display_t * disp = lv_timer_get_user_data(t);
    _x11_user_hdr_t * disp_hdr = lv_display_get_driver_data(disp);
    x11_inp_data_t * xd = disp_hdr->inp_data;

    /* handle all outstanding X events */
    XEvent event;
    while(XCheckIfEvent(disp_hdr->display, &event, is_inp_event, NULL)) {
        LV_LOG_TRACE("Input Event %d", event.type);
        switch(event.type) {
            case MotionNotify:
                xd->mouse_pos.x = event.xmotion.x;
                xd->mouse_pos.y = event.xmotion.y;
                break;
            case ButtonPress:
                switch(event.xbutton.button) {
                    case Button1:
                        xd->left_mouse_btn = true;
                        break;
                    case Button2:
                        xd->wheel_mouse_btn = true;
                        break;
                    case Button3:
                        xd->right_mouse_btn = true;
                        break;
                    case Button4: /* Scrolled up */
                        xd->wheel_cnt--;
                        break;
                    case Button5: /* Scrolled down */
                        xd->wheel_cnt++;
                        break;
                    default:
                        LV_LOG_WARN("unhandled button press : %d", event.xbutton.button);
                }
                break;
            case ButtonRelease:
                switch(event.xbutton.button) {
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
            case KeyPress: {
                    size_t len = strlen(xd->kb_buffer);
                    if(len < (sizeof(xd->kb_buffer) - 2 /* space for 1 char + '\0' */)) {
                        KeySym key;
                        int n = XLookupString(&event.xkey, &xd->kb_buffer[len], sizeof(xd->kb_buffer) - (len + 1), &key, NULL);
                        xd->kb_buffer[len + n] = '\0';
                    }
                }
                break;
            case KeyRelease:
                break;
            default:
                LV_LOG_WARN("unhandled x11 event: %d", event.type);
        }
    }
}

/**
 * event callbed by lvgl display if display has been closed (@ref lv_display_delete has been called)
 * @param[in] e  event data, containing lv_display_t object
 */
static void x11_inp_delete_evt_cb(lv_event_t * e)
{
    x11_inp_data_t * xd = (x11_inp_data_t *)lv_event_get_user_data(e);

    lv_timer_delete(xd->timer);
    lv_free(xd);
}

/**
 * create the local data/timers for the X11 input functionality.
 * extracts the user data information from lv_display_t object and initializes the input user object on 1st use.
 * @param[in] disp   the created X11 display object from @lv_x11_window_create
 * @return           pointer to the local user data object @x11_inp_data_t
 */
static x11_inp_data_t * x11_input_get_user_data(lv_display_t * disp)
{
    _x11_user_hdr_t * disp_hdr = lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(disp_hdr);
    x11_inp_data_t ** inp_data = &disp_hdr->inp_data;

    /* create input data set if initial call */
    if(NULL == *inp_data) {
        *inp_data = lv_malloc_zeroed(sizeof(x11_inp_data_t));
        LV_ASSERT_MALLOC(*inp_data);
        if(NULL != *inp_data) {
            /* initialize timer callback for X11 kb/mouse input event reading */
            (*inp_data)->timer = lv_timer_create(x11_inp_event_handler, 1, disp);
            lv_display_add_event_cb(disp, x11_inp_delete_evt_cb, LV_EVENT_DELETE, *inp_data);
        }
    }
    return *inp_data;
}

static void x11_keyboard_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_display_t * disp = lv_indev_get_driver_data(indev);
    x11_inp_data_t * xd = x11_input_get_user_data(disp);

    size_t len = strlen(xd->kb_buffer);
    if(len > 0) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->key   = xd->kb_buffer[0];
        memmove(xd->kb_buffer, xd->kb_buffer + 1, len);
        data->continue_reading = (len > 0);
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void x11_mouse_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_display_t * disp = lv_indev_get_driver_data(indev);
    x11_inp_data_t * xd = x11_input_get_user_data(disp);

    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);

    xd->mouse_pos.x = MIN(xd->mouse_pos.x, hor_res - 1);
    xd->mouse_pos.y = MIN(xd->mouse_pos.y, ver_res - 1);

    data->point = xd->mouse_pos;
    data->state = xd->left_mouse_btn ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

static void x11_mousewheel_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_display_t * disp = lv_indev_get_driver_data(indev);
    x11_inp_data_t * xd = x11_input_get_user_data(disp);

    data->state    = xd->wheel_mouse_btn ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    data->enc_diff = xd->wheel_cnt;
    xd->wheel_cnt  = 0;
}

static lv_indev_t * lv_x11_keyboard_create(lv_display_t * disp)
{
    lv_indev_t * indev = lv_indev_create();
    LV_ASSERT_OBJ(indev, MY_CLASS);
    if(NULL != indev) {
        lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
        lv_indev_set_read_cb(indev, x11_keyboard_read_cb);
        lv_indev_set_driver_data(indev, disp);
    }
    return indev;
}

static lv_indev_t * lv_x11_mouse_create(lv_display_t * disp, lv_image_dsc_t const * symb)
{
    lv_indev_t * indev = lv_indev_create();
    LV_ASSERT_OBJ(indev, MY_CLASS);
    if(NULL != indev) {
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, x11_mouse_read_cb);
        lv_indev_set_driver_data(indev, disp);

        /* optional mouse cursor symbol */
        if(NULL != symb) {
            lv_obj_t * mouse_cursor = lv_image_create(lv_screen_active());
            lv_image_set_src(mouse_cursor, symb);
            lv_indev_set_cursor(indev, mouse_cursor);
        }
    }
    return indev;
}

static lv_indev_t * lv_x11_mousewheel_create(lv_display_t * disp)
{
    lv_indev_t * indev = lv_indev_create();
    LV_ASSERT_OBJ(indev, MY_CLASS);
    if(NULL != indev) {
        lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
        lv_indev_set_read_cb(indev, x11_mousewheel_read_cb);
        lv_indev_set_driver_data(indev, disp);
    }
    return indev;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_x11_inputs_create(lv_display_t * disp, lv_image_dsc_t const * mouse_img)
{
    x11_inp_data_t * xd = x11_input_get_user_data(disp);
    LV_ASSERT_NULL(xd);

    xd->inp_group = lv_group_create();
    lv_group_set_default(xd->inp_group);

    xd->mousepointer = lv_x11_mouse_create(disp, mouse_img);
    lv_indev_set_group(xd->mousepointer, xd->inp_group);

    xd->mousewheel = lv_x11_mousewheel_create(disp);
    lv_indev_set_group(xd->mousewheel, xd->inp_group);

    xd->keyboard = lv_x11_keyboard_create(disp);
    lv_indev_set_group(xd->keyboard, xd->inp_group);
}

#endif /*LV_USE_X11*/
