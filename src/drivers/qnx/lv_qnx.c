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
} lv_qnx_window_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static inline int qnx_render_mode(void);
static uint32_t get_ticks(void);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static bool window_create(lv_display_t * disp);
static bool init_display_from_window(lv_display_t * disp);
static void get_pointer(lv_indev_t *indev_drv, lv_indev_data_t *data);
static void release_disp_cb(lv_event_t * e);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/
lv_display_t * _lv_qnx_get_disp_from_win_id(uint32_t win_id);

static screen_context_t context;
static bool inited = false;

/**********************
 *  STATIC VARIABLES
 **********************/
static int mouse_pos[2];
static int mouse_buttons;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_qnx_window_create(int32_t hor_res, int32_t ver_res)
{
    if(!inited) {
        if (screen_create_context(&context,
                                  SCREEN_APPLICATION_CONTEXT) != 0) {
            perror("screen_create_context");
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
    if (!window_create(disp)) {
        lv_free(dsc);
        return NULL;
    }

    lv_display_set_flush_cb(disp, flush_cb);

    if (!init_display_from_window(disp)) {
        screen_destroy_window(dsc->window);
        lv_free(dsc);
        return NULL;
    }

    return disp;
}

#if 0
lv_display_t * _lv_qnx_get_disp_from_win_id(uint32_t win_id)
{
    lv_display_t * disp = lv_display_get_next(NULL);
    if(win_id == UINT32_MAX) return disp;

    while(disp) {
        lv_qnx_window_t * dsc = lv_display_get_driver_data(disp);
        if(QNX_GetWindowID(dsc->window) == win_id) {
            return disp;
        }
        disp = lv_display_get_next(disp);
    }
    return NULL;
}
#endif

void lv_qnx_window_set_title(lv_display_t * disp, const char * title)
{
    lv_qnx_window_t * dsc = lv_display_get_driver_data(disp);
    screen_event_t event;
    screen_create_event(&event);

    char title_buf[64];
    snprintf(title_buf, sizeof(title), "Title=%s", title);
    screen_set_event_property_iv(event, SCREEN_PROPERTY_TYPE,
                                 (const int[]){ SCREEN_EVENT_MANAGER });
    screen_set_event_property_cv(event, SCREEN_PROPERTY_USER_DATA,
                                 sizeof(title_buf), title_buf);
    screen_set_event_property_pv(event, SCREEN_PROPERTY_WINDOW,
                                 (void **)&dsc->window);
    screen_set_event_property_pv(event, SCREEN_PROPERTY_CONTEXT,
                                 (void **)&context);

    screen_inject_event(NULL, event);
}

bool lv_qnx_inputs_create(lv_display_t * disp)
{
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, get_pointer);
    return true;
}

int lv_qnx_event_loop(lv_display_t * disp)
{
    // Run the event loop.
    screen_event_t  event;
    if (screen_create_event(&event) != 0) {
        perror("screen_create_event");
        return EXIT_FAILURE;
    }

    for (;;) {
        // Wait for an event, timing out after 10ms.
        if (screen_get_event(context, event, 10000000L) != 0) {
            perror("screen_get_event");
            return EXIT_FAILURE;
        }

        // Get the event's type.
        int type;
        if (screen_get_event_property_iv(event, SCREEN_PROPERTY_TYPE, &type)
            != 0) {
            perror("screen_get_event_property_iv(TYPE)");
            return EXIT_FAILURE;
        }

        if (type == SCREEN_EVENT_POINTER) {
            // Handle a mouse pointer event.
            if (screen_get_event_property_iv(event,
                                             SCREEN_PROPERTY_SOURCE_POSITION,
                                             mouse_pos)
                != 0) {
                perror("screen_get_event_property_iv(SOURCE_POSITION)");
                return EXIT_FAILURE;
            }

            if (screen_get_event_property_iv(event, SCREEN_PROPERTY_BUTTONS,
                                             &mouse_buttons)
                != 0) {
                perror("screen_get_event_property_iv(BUTTONS)");
                return EXIT_FAILURE;
            }
        }

        lv_timer_handler();
    }

    return EXIT_SUCCESS;
}

void lv_qnx_quit()
{
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline int qnx_render_mode(void)
{
    return LV_DISPLAY_RENDER_MODE_DIRECT;
}

static uint32_t
get_ticks(void)
{
    uint64_t const ns = clock_gettime_mon_ns();
    return (uint32_t)(ns / 1000000UL);
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_qnx_window_t * dsc = lv_display_get_driver_data(disp);
    if (screen_post_window(dsc->window, dsc->buffers[dsc->bufidx], 0, NULL, 0)
        != 0) {
        perror("screen_post_window");
    }

#if (LV_QNX_BUF_COUNT > 1)
    dsc->bufidx = 1 - dsc->bufidx;
#endif

    lv_display_flush_ready(disp);
}

static bool window_create(lv_display_t * disp)
{
    lv_qnx_window_t * dsc = lv_display_get_driver_data(disp);
    if (screen_create_window(&dsc->window, context) != 0) {
        perror("screen_create_window");
        return false;
    }

    int rect[] = { 0, 0, disp->hor_res, disp->ver_res };

    if (screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_POSITION,
                                      &rect[0]) != 0) {
        perror("screen_window_set_property_iv(POSITION)");
        return false;
    }

    if (screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_SIZE,
                                      &rect[2]) != 0) {
        perror("screen_window_set_property_iv(SIZE)");
        return false;
    }

    if (screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_SOURCE_SIZE,
                                      &rect[2]) != 0) {
        perror("screen_window_set_property_iv(SOURCE_SIZE)");
        return NULL;
    }

    if (screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_USAGE,
                                      (int[]){ SCREEN_USAGE_WRITE }) != 0) {
        perror("screen_window_set_property_iv(USAGE)");
        return NULL;
    }

    if (screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_FORMAT,
                                      (int[]){ SCREEN_FORMAT_RGBA8888 }) != 0) {
        perror("screen_window_set_property_iv(USAGE)");
        return NULL;
    }

    // Initialize window buffers.
    if (screen_create_window_buffers(dsc->window, LV_QNX_BUF_COUNT) != 0) {
        perror("screen_create_window_buffers");
        return false;
    }

    if (screen_get_window_property_pv(dsc->window, SCREEN_PROPERTY_BUFFERS,
                                      (void **)&dsc->buffers) != 0) {
        perror("screen_get_window_property_pv(BUFFERS)");
        return false;
    }

    if (screen_set_window_property_iv(dsc->window, SCREEN_PROPERTY_VISIBLE,
                                      (int[]){ 1 })
        != 0) {
        perror("screen_set_window_property_iv(VISIBLE)");
        return false;
    }

    return true;
}

static bool init_display_from_window(lv_display_t *disp)
{
    lv_qnx_window_t * dsc = lv_display_get_driver_data(disp);

    int bufsize;
    if (screen_get_buffer_property_iv(dsc->buffers[0], SCREEN_PROPERTY_SIZE,
                                      &bufsize) == -1) {
        perror("screen_get_buffer_property_iv(SIZE)");
        return false;
    }

    void *ptr1 = NULL;
    if (screen_get_buffer_property_pv(dsc->buffers[0], SCREEN_PROPERTY_POINTER,
                                      &ptr1) == -1) {
        perror("screen_get_buffer_property_pv(POINTER)");
        return false;
    }

    void *ptr2 = NULL;
#if (LV_QNX_BUF_COUNT > 1)
    if (screen_get_buffer_property_pv(dsc->buffers[1], SCREEN_PROPERTY_POINTER,
                                      &ptr2) == -1) {
        perror("screen_get_buffer_property_pv(POINTER)");
        return false;
    }
#endif

    lv_display_set_buffers(disp, ptr1, ptr2, bufsize, LV_DISPLAY_RENDER_MODE_DIRECT);
    return true;
}

static void release_disp_cb(lv_event_t * e)
{
}

static void get_pointer(lv_indev_t *indev_drv, lv_indev_data_t *data)
{
    data->point.x = mouse_pos[0];
    data->point.y = mouse_pos[1];
    if ((mouse_buttons & SCREEN_LEFT_MOUSE_BUTTON) != 0) {
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

#endif /*LV_USE_QNX*/
