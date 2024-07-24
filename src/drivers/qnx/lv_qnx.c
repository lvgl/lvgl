/**
 * @file lv_qnx.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_qnx.h"
#if LV_USE_QNX
#include <stdbool.h>
#include "../../core/lv_refr.h"
#include "../../stdlib/lv_string.h"
#include "../../core/lv_global.h"
#include "../../display/lv_display_private.h"
#include "../../lv_init.h"
#include <stdlib.h>
#include <time.h>
#include <screen/screen.h>
#include <sys/keycodes.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    screen_window_t     window;
    screen_buffer_t     buffers[LV_QNX_BUF_COUNT];
    int                 bufidx;
    bool                managed;
} lv_qnx_window_t;

typedef struct {
    int                 pos[2];
    int                 buttons;
} lv_qnx_pointer_t;

typedef struct {
    int                 key;
    int                 flags;
} lv_qnx_keyboard_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static uint32_t get_ticks(void);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static bool window_create(lv_display_t * disp);
static bool init_display_from_window(lv_display_t * disp);
static void get_pointer(lv_indev_t * indev, lv_indev_data_t * data);
static void get_key(lv_indev_t * indev, lv_indev_data_t * data);
static bool handle_pointer_event(screen_event_t event);
static bool handle_keyboard_event(screen_event_t event);
static void release_disp_cb(lv_event_t * e);
static void refresh_cb(lv_timer_t * timer);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

static screen_context_t context;
static bool inited = false;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_indev_t * pointer_indev;
static lv_indev_t * keyboard_indev;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_qnx_window_create(int32_t hor_res, int32_t ver_res)
{
    if(!inited) {
        if(screen_create_context(&context,
                                 SCREEN_APPLICATION_CONTEXT) != 0) {
            LV_LOG_ERROR("screen_create_context: %s", strerror(errno));
            return NULL;
        }

        lv_tick_set_cb(get_ticks);
        inited = true;
    }

    lv_qnx_window_t * dsc = lv_malloc_zeroed(sizeof(lv_qnx_window_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }
    lv_display_add_event_cb(disp, release_disp_cb, LV_EVENT_DELETE, disp);
    lv_display_set_driver_data(disp, dsc);
    if(!window_create(disp)) {
        lv_free(dsc);
        return NULL;
    }

    lv_display_set_flush_cb(disp, flush_cb);

    if(!init_display_from_window(disp)) {
        screen_destroy_window(dsc->window);
        lv_free(dsc);
        return NULL;
    }

    /*Replace the default refresh timer handler, so that we can run it on
     *demand instead of constantly.*/
    lv_timer_t * refr_timer = lv_display_get_refr_timer(disp);
    lv_timer_set_cb(refr_timer, refresh_cb);

    return disp;
}

void lv_qnx_window_set_title(lv_display_t * disp, const char * title)
{
    lv_qnx_window_t * dsc = lv_display_get_driver_data(disp);
    if(!dsc->managed) {
        /*Can't set title if there is no window manager*/
        return;
    }

    screen_event_t event;
    screen_create_event(&event);

    char title_buf[64];
    snprintf(title_buf, sizeof(title_buf), "Title=%s", title);

    int type = SCREEN_EVENT_MANAGER;
    screen_set_event_property_iv(event, SCREEN_PROPERTY_TYPE, &type);
    screen_set_event_property_cv(event, SCREEN_PROPERTY_USER_DATA,
                                 sizeof(title_buf), title_buf);
    screen_set_event_property_pv(event, SCREEN_PROPERTY_WINDOW,
                                 (void **)&dsc->window);
    screen_set_event_property_pv(event, SCREEN_PROPERTY_CONTEXT,
                                 (void **)&context);

    screen_inject_event(NULL, event);
}

bool lv_qnx_add_pointer_device(lv_display_t * disp)
{
    if(pointer_indev != NULL) {
        return false;
    }

    lv_qnx_pointer_t * dsc = lv_malloc_zeroed(sizeof(lv_qnx_pointer_t));
    if(dsc == NULL) {
        return false;
    }

    pointer_indev = lv_indev_create();
    if(pointer_indev == NULL) {
        lv_free(dsc);
        return false;
    }

    lv_indev_set_type(pointer_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(pointer_indev, get_pointer);
    lv_indev_set_driver_data(pointer_indev, dsc);
    lv_indev_set_mode(pointer_indev, LV_INDEV_MODE_EVENT);
    return true;
}

bool lv_qnx_add_keyboard_device(lv_display_t * disp)
{
    if(keyboard_indev != NULL) {
        return false;
    }

    lv_qnx_keyboard_t * dsc = lv_malloc_zeroed(sizeof(lv_qnx_keyboard_t));
    if(dsc == NULL) {
        return false;
    }

    keyboard_indev = lv_indev_create();
    if(keyboard_indev == NULL) {
        lv_free(dsc);
        return false;
    }

    lv_indev_set_type(keyboard_indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(keyboard_indev, get_key);
    lv_indev_set_driver_data(keyboard_indev, dsc);
    lv_indev_set_mode(keyboard_indev, LV_INDEV_MODE_EVENT);
    return true;
}

int lv_qnx_event_loop(lv_display_t * disp)
{
    lv_refr_now(disp);

    /*Run the event loop*/
    screen_event_t  event;
    if(screen_create_event(&event) != 0) {
        LV_LOG_ERROR("screen_create_event: %s", strerror(errno));
        return EXIT_FAILURE;
    }

    uint64_t timeout_ns = 0;
    for(;;) {
        /*Wait for an event, timing out after 16ms if animations are running*/
        if(screen_get_event(context, event, timeout_ns) != 0) {
            LV_LOG_ERROR("screen_get_event: %s", strerror(errno));
            return EXIT_FAILURE;
        }

        /*Get the event's type*/
        int type;
        if(screen_get_event_property_iv(event, SCREEN_PROPERTY_TYPE, &type)
           != 0) {
            LV_LOG_ERROR("screen_get_event_property_iv(TYPE): %s", strerror(errno));
            return EXIT_FAILURE;
        }

        if(type == SCREEN_EVENT_POINTER) {
            if(!handle_pointer_event(event)) {
                return EXIT_FAILURE;
            }
        }
        else if(type == SCREEN_EVENT_KEYBOARD) {
            if(!handle_keyboard_event(event)) {
                return EXIT_FAILURE;
            }
        }
        else if(type == SCREEN_EVENT_MANAGER) {
            /*Only sub-type supported is closing the window*/
            break;
        }

        /*Calculate the next timeout*/
        uint32_t timeout_ms = lv_timer_handler();
        if(timeout_ms == LV_NO_TIMER_READY) {
            timeout_ns = -1ULL;
        }
        else {
            timeout_ns = (uint64_t)timeout_ms * 1000000UL;
        }
    }

    return EXIT_SUCCESS;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t get_ticks(void)
{
    uint64_t const ns = clock_gettime_mon_ns();
    return (uint32_t)(ns / 1000000UL);
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_qnx_window_t * dsc = lv_display_get_driver_data(disp);
    if(screen_post_window(dsc->window, dsc->buffers[dsc->bufidx], 0, NULL, 0)
       != 0) {
        LV_LOG_ERROR("screen_post_window: %s", strerror(errno));
    }

#if (LV_QNX_BUF_COUNT > 1)
    dsc->bufidx = 1 - dsc->bufidx;
#endif

    lv_display_flush_ready(disp);
}

static bool window_create(lv_display_t * disp)
{
    /*Create a window*/
    lv_qnx_window_t * dsc = lv_display_get_driver_data(disp);
    if(screen_create_window(&dsc->window, context) != 0) {
        LV_LOG_ERROR("screen_create_window: %s", strerror(errno));
        return false;
    }

    /*Set window properties*/
    int rect[] = { 0, 0, disp->hor_res, disp->ver_res };
    if(screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_POSITION,
                                     &rect[0]) != 0) {
        LV_LOG_ERROR("screen_window_set_property_iv(POSITION): %s", strerror(errno));
        return false;
    }

    if(screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_SIZE,
                                     &rect[2]) != 0) {
        LV_LOG_ERROR("screen_window_set_property_iv(SIZE): %s", strerror(errno));
        return false;
    }

    if(screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_SOURCE_SIZE,
                                     &rect[2]) != 0) {
        LV_LOG_ERROR("screen_window_set_property_iv(SOURCE_SIZE): %s", strerror(errno));
        return NULL;
    }

    int usage = SCREEN_USAGE_WRITE;
    if(screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_USAGE,
                                     &usage) != 0) {
        LV_LOG_ERROR("screen_window_set_property_iv(USAGE): %s", strerror(errno));
        return NULL;
    }

    int format = SCREEN_FORMAT_RGBA8888;
    if(screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_FORMAT,
                                     &format) != 0) {
        LV_LOG_ERROR("screen_window_set_property_iv(USAGE): %s", strerror(errno));
        return NULL;
    }

    /*Initialize window buffers*/
    if(screen_create_window_buffers(dsc->window, LV_QNX_BUF_COUNT) != 0) {
        LV_LOG_ERROR("screen_create_window_buffers: %s", strerror(errno));
        return false;
    }

    if(screen_get_window_property_pv(dsc->window, SCREEN_PROPERTY_BUFFERS,
                                     (void **)&dsc->buffers) != 0) {
        LV_LOG_ERROR("screen_get_window_property_pv(BUFFERS): %s", strerror(errno));
        return false;
    }

    /*Connect to the window manager. Can legitimately fail if one is not running*/
    if(screen_manage_window(dsc->window, "Frame=Y") == 0) {
        dsc->managed = true;
    }
    else {
        dsc->managed = false;
    }

    int visible = 1;
    if(screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_VISIBLE,
                                     &visible) != 0) {
        LV_LOG_ERROR("screen_set_window_property_iv(VISIBLE): %s", strerror(errno));
        return false;
    }

    return true;
}

static bool init_display_from_window(lv_display_t * disp)
{
    lv_qnx_window_t * dsc = lv_display_get_driver_data(disp);

    int bufsize;
    if(screen_get_buffer_property_iv(dsc->buffers[0], SCREEN_PROPERTY_SIZE,
                                     &bufsize) == -1) {
        LV_LOG_ERROR("screen_get_buffer_property_iv(SIZE): %s", strerror(errno));
        return false;
    }

    void * ptr1 = NULL;
    if(screen_get_buffer_property_pv(dsc->buffers[0], SCREEN_PROPERTY_POINTER,
                                     &ptr1) == -1) {
        LV_LOG_ERROR("screen_get_buffer_property_pv(POINTER): %s", strerror(errno));
        return false;
    }

    void * ptr2 = NULL;
#if (LV_QNX_BUF_COUNT > 1)
    if(screen_get_buffer_property_pv(dsc->buffers[1], SCREEN_PROPERTY_POINTER,
                                     &ptr2) == -1) {
        LV_LOG_ERROR("screen_get_buffer_property_pv(POINTER): %s", strerror(errno));
        return false;
    }
#endif

    lv_display_set_buffers(disp, ptr1, ptr2, bufsize, LV_DISPLAY_RENDER_MODE_FULL);
    return true;
}

static void release_disp_cb(lv_event_t * e)
{
    lv_display_t * disp = (lv_display_t *) lv_event_get_user_data(e);
    lv_qnx_window_t * dsc = lv_display_get_driver_data(disp);

    if(dsc->window != NULL) {
        screen_destroy_window(dsc->window);
    }

    lv_free(dsc);
    lv_display_set_driver_data(disp, NULL);
}

static void get_pointer(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_qnx_pointer_t * dsc = lv_indev_get_driver_data(indev);

    data->point.x = dsc->pos[0];
    data->point.y = dsc->pos[1];
    if((dsc->buttons & SCREEN_LEFT_MOUSE_BUTTON) != 0) {
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static bool handle_pointer_event(screen_event_t event)
{
    if(pointer_indev == NULL) return true;

    lv_qnx_pointer_t * dsc = lv_indev_get_driver_data(pointer_indev);

    if(screen_get_event_property_iv(event, SCREEN_PROPERTY_SOURCE_POSITION,
                                    dsc->pos)
       != 0) {
        LV_LOG_ERROR("screen_get_event_property_iv(SOURCE_POSITION): %s", strerror(errno));
        return false;
    }

    if(screen_get_event_property_iv(event, SCREEN_PROPERTY_BUTTONS,
                                    &dsc->buttons)
       != 0) {
        LV_LOG_ERROR("screen_get_event_property_iv(BUTTONS): %s", strerror(errno));
        return false;
    }

    lv_indev_read(pointer_indev);
    return true;
}

static void get_key(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_qnx_keyboard_t * dsc = lv_indev_get_driver_data(indev);

    if((dsc->flags & KEY_DOWN) != 0) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->key = dsc->key;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static bool handle_keyboard_event(screen_event_t event)
{
    if(keyboard_indev == NULL) return true;

    lv_qnx_keyboard_t * dsc = lv_indev_get_driver_data(keyboard_indev);

    /*Get event data*/
    if(screen_get_event_property_iv(event, SCREEN_PROPERTY_FLAGS,
                                    &dsc->flags)
       != 0) {
        LV_LOG_ERROR("screen_get_event_property_iv(FLAGS): %s", strerror(errno));
        return false;
    }

    if(screen_get_event_property_iv(event, SCREEN_PROPERTY_SYM,
                                    &dsc->key)
       != 0) {
        LV_LOG_ERROR("screen_get_event_property_iv(SYM): %s", strerror(errno));
        return false;
    }

    /*Translate special keys*/
    switch(dsc->key) {
        case KEYCODE_UP:
            dsc->key = LV_KEY_UP;
            break;

        case KEYCODE_DOWN:
            dsc->key = LV_KEY_DOWN;
            break;

        case KEYCODE_LEFT:
            dsc->key = LV_KEY_LEFT;
            break;

        case KEYCODE_RIGHT:
            dsc->key = LV_KEY_RIGHT;
            break;

        case KEYCODE_RETURN:
            dsc->key = LV_KEY_ENTER;
            break;

        case KEYCODE_BACKSPACE:
            dsc->key = LV_KEY_BACKSPACE;
            break;

        case KEYCODE_HOME:
            dsc->key = LV_KEY_HOME;
            break;

        case KEYCODE_END:
            dsc->key = LV_KEY_END;
            break;

        case KEYCODE_DELETE:
            dsc->key = LV_KEY_DEL;
            break;

        default:
            /*Ignore other non-ASCII keys, including modifiers*/
            if(dsc->key > 0xff) return true;
    }

    lv_indev_read(keyboard_indev);
    return true;
}

static void refresh_cb(lv_timer_t * timer)
{
    /*Refresh the window on timeout, but disable the timer. Any callback can
     *re-enable it.*/
    lv_display_t * disp = timer->user_data;
    lv_refr_now(disp);
    lv_timer_pause(timer);
}

#endif /*LV_USE_QNX*/
