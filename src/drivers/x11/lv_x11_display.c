/**
 * @file lv_x11_display.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_x11.h"

#if LV_USE_X11

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../../core/lv_obj_pos.h"

/*********************
 *      DEFINES
 *********************/
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

#if LV_X11_RENDER_MODE_PARTIAL
    #define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL
#elif defined LV_X11_RENDER_MODE_DIRECT
    #define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_DIRECT
#elif defined LV_X11_RENDER_MODE_FULL
    #define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_FULL
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /* header (containing X Display + input user data pointer - keep aligned with x11_input module!) */
    _x11_user_hdr_t hdr;
    /* X11 related information */
    Window          window;          /**< X11 window object */
    GC              gc;              /**< X11 graphics context object */
    Visual     *    visual;          /**< X11 visual */
    int             dplanes;         /**< X11 display depth */
    XImage     *    ximage;          /**< X11 XImage cache object for updating window content */
    Atom            wmDeleteMessage; /**< X11 atom to window object */
    void      *     xdata;           /**< allocated data for XImage */
    /* LVGL related information */
    lv_timer_t   *  timer;           /**< timer object for @ref x11_event_handler */
    lv_color_t   *  buffer[2];       /**< (double) lv display buffers, depending on @ref LV_X11_RENDER_MODE */
    lv_area_t       flush_area;      /**< integrated area for a display update */
    /* systemtick by thread related information */
    pthread_t       thr_tick;        /**< pthread for SysTick simulation */
    bool            terminated;      /**< flag to germinate SysTick simulation thread */
} x11_disp_data_t;

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_X11_DIRECT_EXIT
    static unsigned int count_windows = 0;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if   LV_COLOR_DEPTH == 32
typedef lv_color32_t color_t;
static inline lv_color32_t get_px(color_t p)
{
    return (lv_color32_t)p;
}
#elif LV_COLOR_DEPTH == 24
typedef lv_color_t color_t;
static inline lv_color32_t get_px(color_t p)
{
    lv_color32_t out = { .red = p.red, .green = p.green, .blue = p.blue };
    return out;
}
#elif LV_COLOR_DEPTH == 16
typedef lv_color16_t color_t;
static inline lv_color32_t get_px(color_t p)
{
    lv_color32_t out = { .red = p.red << 3, .green = p.green << 2, .blue = p.blue << 3 };
    return out;
}
#elif LV_COLOR_DEPTH == 8
typedef uint8_t color_t;
static inline lv_color32_t get_px(color_t p)
{
    lv_color32_t out = { .red = p, .green = p, .blue = p };
    return out;
}
#warning ("LV_COLOR_DEPTH=8 delivers black data only - open issue in lvgl?")
#else
#error ("Unsupported LV_COLOR_DEPTH")
#endif

/**
 * Flush the content of the internal buffer the specific area on the display.
 * @param[in] disp    the created X11 display object from @lv_x11_window_create
 * @param[in] area    area to be updated
 * @param[in] px_map  contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 * @note              @ref lv_display_flush_ready has to be called when it's finished.
 */
static void x11_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    x11_disp_data_t * xd = lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(xd);

    static const lv_area_t inv_area = { .x1 = 0xFFFF,
                                        .x2 = 0,
                                        .y1 = 0xFFFF,
                                        .y2 = 0
                                      };

    /* build display update area until lv_disp_flush_is_last */
    xd->flush_area.x1 = MIN(xd->flush_area.x1, area->x1);
    xd->flush_area.x2 = MAX(xd->flush_area.x2, area->x2);
    xd->flush_area.y1 = MIN(xd->flush_area.y1, area->y1);
    xd->flush_area.y2 = MAX(xd->flush_area.y2, area->y2);

    int32_t hor_res = lv_display_get_horizontal_resolution(disp);

    uint32_t      dst_offs;
    lv_color32_t * dst_data;
    color_t   *   src_data = (color_t *)px_map + (LV_X11_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL ? 0 : hor_res *
                                                  area->y1 + area->x1);
    for(int16_t y = area->y1; y <= area->y2; y++) {
        dst_offs = area->x1 + y * hor_res;
        dst_data = &((lv_color32_t *)(xd->xdata))[dst_offs];
        for(int16_t x = area->x1; x <= area->x2; x++, src_data++, dst_data++) {
            *dst_data = get_px(*src_data);
        }
        src_data += (LV_X11_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL ? 0 : hor_res - (area->x2 - area->x1 + 1));
    }

    if(lv_display_flush_is_last(disp)) {
        LV_LOG_TRACE("(%d/%d), %dx%d)", xd->flush_area.x1, xd->flush_area.y1, xd->flush_area.x2 + 1 - xd->flush_area.x1,
                     xd->flush_area.y2 + 1 - xd->flush_area.y1);

        /* refresh collected display update area only */
        int16_t upd_w = xd->flush_area.x2 - xd->flush_area.x1 + 1;
        int16_t upd_h = xd->flush_area.y2 - xd->flush_area.y1 + 1;
        XPutImage(xd->hdr.display, xd->window, xd->gc, xd->ximage, xd->flush_area.x1, xd->flush_area.y1, xd->flush_area.x1,
                  xd->flush_area.y1, upd_w, upd_h);

        /* invalidate collected area */
        xd->flush_area = inv_area;
    }
    /* Inform the graphics library that you are ready with the flushing */
    lv_display_flush_ready(disp);
}

/**
 * event callbed by lvgl display if resolution has been changed (@ref lv_display_set_resolution has been called)
 * @param[in] e  event data, containing lv_display_t object
 */
static void x11_resolution_evt_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_user_data(e);
    x11_disp_data_t * xd = lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(xd);

    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);

    if(LV_X11_RENDER_MODE != LV_DISPLAY_RENDER_MODE_PARTIAL) {
        /* update lvgl full-screen display draw buffers for new display size */
        int sz_buffers = (hor_res * ver_res * (LV_COLOR_DEPTH + 7) / 8);
        xd->buffer[0] = lv_realloc(xd->buffer[0], sz_buffers);
        xd->buffer[1] = (LV_X11_DOUBLE_BUFFER ?  lv_realloc(xd->buffer[1], sz_buffers) : NULL);
        lv_display_set_buffers(disp, xd->buffer[0], xd->buffer[1], sz_buffers, LV_X11_RENDER_MODE);
    }

    /* re-create cache image with new size */
    XDestroyImage(xd->ximage);
    size_t sz_buffers = hor_res * ver_res * sizeof(lv_color32_t);
    xd->xdata = malloc(sz_buffers); /* use clib method here, x11 memory not part of device footprint */
    xd->ximage = XCreateImage(xd->hdr.display, xd->visual, xd->dplanes, ZPixmap, 0, xd->xdata,
                              hor_res, ver_res, lv_color_format_get_bpp(LV_COLOR_FORMAT_ARGB8888), 0);
}

/**
 * event callbed by lvgl display if display has been closed (@ref lv_display_delete has been called)
 * @param[in] e  event data, containing lv_display_t object
 */
static void x11_disp_delete_evt_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_user_data(e);
    x11_disp_data_t * xd = lv_display_get_driver_data(disp);

    lv_timer_delete(xd->timer);

    lv_display_set_buffers(disp, NULL, NULL, 0, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_free(xd->buffer[0]);
    if(LV_X11_DOUBLE_BUFFER) {
        lv_free(xd->buffer[1]);
    }

    XDestroyImage(xd->ximage);
    XFreeGC(xd->hdr.display, xd->gc);
    XUnmapWindow(xd->hdr.display, xd->window);
    XDestroyWindow(xd->hdr.display, xd->window);
    XFlush(xd->hdr.display);

    lv_free(xd);
#if LV_X11_DIRECT_EXIT
    if(0 == --count_windows) {
        exit(0);
    }
#endif
}

static void x11_hide_cursor(lv_display_t * disp)
{
    x11_disp_data_t * xd = lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(xd);

    XColor black = { .red = 0, .green = 0, .blue = 0 };
    char empty_data[] = { 0 };

    Pixmap empty_bitmap = XCreateBitmapFromData(xd->hdr.display, xd->window, empty_data, 1, 1);
    Cursor inv_cursor = XCreatePixmapCursor(xd->hdr.display, empty_bitmap, empty_bitmap, &black, &black, 0, 0);
    XDefineCursor(xd->hdr.display, xd->window, inv_cursor);
    XFreeCursor(xd->hdr.display, inv_cursor);
    XFreePixmap(xd->hdr.display, empty_bitmap);
}

/**
 * X11 input event handler, predicated to fetch and handle only display related events
 * (Window changes)
 */
static int is_disp_event(Display * disp, XEvent * event, XPointer arg)
{
    LV_UNUSED(disp);
    LV_UNUSED(arg);
    return (event->type == Expose
            || (event->type >= DestroyNotify && event->type <= CirculateNotify) /* events from StructureNotifyMask */
            ||  event->type == ClientMessage);
}
static void x11_event_handler(lv_timer_t * t)
{
    lv_display_t * disp = lv_timer_get_user_data(t);
    x11_disp_data_t * xd = lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(xd);

    /* handle all outstanding X events */
    XEvent event;
    while(XCheckIfEvent(xd->hdr.display, &event, is_disp_event, NULL)) {
        LV_LOG_TRACE("Display Event %d", event.type);
        switch(event.type) {
            case Expose:
                if(event.xexpose.count == 0) {
                    XPutImage(xd->hdr.display, xd->window, xd->gc, xd->ximage, 0, 0, 0, 0, event.xexpose.width, event.xexpose.height);
                }
                break;
            case ConfigureNotify:
                if(event.xconfigure.width  != lv_display_get_horizontal_resolution(disp)
                   ||  event.xconfigure.height != lv_display_get_vertical_resolution(disp)) {
                    lv_display_set_resolution(disp, event.xconfigure.width, event.xconfigure.height);
                }
                break;
            case ClientMessage:
                if(event.xclient.data.l[0] == (long)xd->wmDeleteMessage) {
                    xd->terminated = true;
                    void * ret = NULL;
                    pthread_join(xd->thr_tick, &ret);
                    lv_display_delete(disp);
                    return;
                }
                break;
            case MapNotify:
            case ReparentNotify:
                /*suppress unhandled warning*/
                break;
            default:
                LV_LOG_WARN("unhandled x11 event: %d", event.type);
        }
    }
}

static void * x11_tick_thread(void * data)
{
    x11_disp_data_t * xd = data;
    LV_ASSERT_NULL(xd);

    while(!xd->terminated) {
        usleep(5000);
        lv_tick_inc(5);
    }
    return NULL;
}

static void x11_window_create(lv_display_t * disp, char const * title)
{
    x11_disp_data_t * xd = lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(xd);

    /* setup display/screen */
    xd->hdr.display = XOpenDisplay(NULL);
    int screen = XDefaultScreen(xd->hdr.display);
    xd->visual = XDefaultVisual(xd->hdr.display, screen);

    /* create window */
    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);
#if 0
    /* drawing contexts for an window */
    unsigned long col_fg = BlackPixel(xd->hdr.display, screen);
    unsigned long col_bg = WhitePixel(xd->hdr.display, screen);

    xd->window = XCreateSimpleWindow(xd->hdr.display, DefaultRootWindow(xd->hdr.display),
                                     0, 0, hor_res, ver_res, 0, col_fg, col_bg);
#else
    xd->window = XCreateWindow(xd->hdr.display, XDefaultRootWindow(xd->hdr.display),
                               0, 0, hor_res, ver_res, 0,
                               XDefaultDepth(xd->hdr.display, screen), InputOutput,
                               xd->visual, 0, NULL);
#endif
    /* window manager properties (yes, use of StdProp is obsolete) */
    XSetStandardProperties(xd->hdr.display, xd->window, title, NULL, None, NULL, 0, NULL);
    xd->gc = XCreateGC(xd->hdr.display, xd->window, 0, 0);

    /* allow receiving mouse, keyboard and window change/close events */
    XSelectInput(xd->hdr.display, xd->window,
                 PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | ExposureMask |
                 StructureNotifyMask);
    xd->wmDeleteMessage = XInternAtom(xd->hdr.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(xd->hdr.display, xd->window, &xd->wmDeleteMessage, 1);

    x11_hide_cursor(disp);

    /* create cache XImage */
    size_t sz_buffers = hor_res * ver_res * sizeof(lv_color32_t);
    xd->dplanes = XDisplayPlanes(xd->hdr.display, screen);
    xd->xdata = malloc(sz_buffers); /* use clib method here, x11 memory not part of device footprint */
    xd->ximage = XCreateImage(xd->hdr.display, xd->visual, xd->dplanes, ZPixmap, 0, xd->xdata,
                              hor_res, ver_res, lv_color_format_get_bpp(LV_COLOR_FORMAT_ARGB8888), 0);

    /* finally bring window on top of the other windows */
    XMapRaised(xd->hdr.display, xd->window);

#if LV_X11_DIRECT_EXIT
    count_windows++;
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_x11_window_create(char const * title, int32_t hor_res, int32_t ver_res)
{
    x11_disp_data_t * xd = lv_malloc_zeroed(sizeof(x11_disp_data_t));
    LV_ASSERT_MALLOC(xd);
    if(NULL == xd) return NULL;

    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(NULL == disp) {
        lv_free(xd);
        return NULL;
    }
    lv_display_set_driver_data(disp, xd);
    lv_display_set_flush_cb(disp, x11_flush_cb);
    lv_display_add_event_cb(disp, x11_resolution_evt_cb, LV_EVENT_RESOLUTION_CHANGED, disp);
    lv_display_add_event_cb(disp, x11_disp_delete_evt_cb, LV_EVENT_DELETE, disp);

    x11_window_create(disp, title);

    int sz_buffers = (hor_res * ver_res * (LV_COLOR_DEPTH + 7) / 8);
    if(LV_X11_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        sz_buffers /= 10;
    }
    xd->buffer[0] = lv_malloc(sz_buffers);
    xd->buffer[1] = (LV_X11_DOUBLE_BUFFER ? lv_malloc(sz_buffers) : NULL);
    lv_display_set_buffers(disp, xd->buffer[0], xd->buffer[1], sz_buffers, LV_X11_RENDER_MODE);

    xd->timer = lv_timer_create(x11_event_handler, 5, disp);

    /* initialize Tick simulation */
    xd->terminated = false;
    pthread_create(&xd->thr_tick, NULL, x11_tick_thread, xd);

    return disp;
}

#endif /*LV_USE_X11*/
