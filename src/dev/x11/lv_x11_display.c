/**
 * @file lv_x11_display.h
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

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    Display* display;  /* X11 Display */
    void*    inp_data; /* reserved pointer for input user data (see lv_x11_input.c) */
} x11_disp_user_hdr_t;

typedef struct
{
    /* header (containing X Display + input user data pointer - keep aligned with x11_input module!) */
    x11_disp_user_hdr_t hdr;
    /* X11 related information */
    Window          window;
    GC              gc;
    Visual*         visual;
    int             dplanes;
    XImage*         ximage;
    Atom            wmDeleteMessage;
    void*           xdata;
    bool            resize;
    /* LVGL related information */
    lv_timer_t*     timer;
    lv_color_t*     buffer[2];
    lv_area_t       flush_area;
    lv_x11_close_cb close_cb;
    /* systemtick by thread related information */
    pthread_t       thr_tick;
    bool            terminated;
} x11_data_t;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if   LV_COLOR_DEPTH == 32
    typedef lv_color32_t color_t;
    static inline lv_color32_t get_px(color_t p) { return (lv_color32_t)p; }
#elif LV_COLOR_DEPTH == 24
    typedef lv_color_t color_t;
    static inline lv_color32_t get_px(color_t p) { lv_color32_t out = { .red = p.red, .green = p.green, .blue = p.blue }; return out; }
#elif LV_COLOR_DEPTH == 16
    typedef lv_color16_t color_t;
    static inline lv_color32_t get_px(color_t p) { lv_color32_t out = { .red = p.red<<3, .green = p.green<<2, .blue = p.blue<<3 }; return out; }
#elif LV_COLOR_DEPTH == 8
    typedef uint8_t color_t;
    static inline lv_color32_t get_px(color_t p) { lv_color32_t out = { .red = p, .green = p, .blue = p }; return out; }
    #warning ("LV_COLOR_DEPTH=8 delivers black data only - open issue in lvgl?")
#else
    #error ("Unsupported LV_COLOR_DEPTH")
#endif

/*
 * Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *'lv_display_flush_ready()' has to be called when it's finished.
 */
static void x11_flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map)
{
    x11_data_t* xd = (x11_data_t*)lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(xd);

    static const lv_area_t inv_area = { .x1 = 0xFFFF,
                                        .x2 = 0,
                                        .y1 = 0xFFFF,
                                        .y2 = 0
                                      };

    /* build display update area until lv_disp_flush_is_last */
    #define MIN(A, B) ((A) < (B) ? (A) : (B))
    #define MAX(A, B) ((A) > (B) ? (A) : (B))
    xd->flush_area.x1 = MIN(xd->flush_area.x1, area->x1);
    xd->flush_area.x2 = MAX(xd->flush_area.x2, area->x2);
    xd->flush_area.y1 = MIN(xd->flush_area.y1, area->y1);
    xd->flush_area.y2 = MAX(xd->flush_area.y2, area->y2);

    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);

    LV_LOG_WARN("x1:%d, y1:%d, w:%d, h:%d", area->x1, area->y1, area->x2+1-area->x1, area->y2+1-area->y1);


    uint32_t      dst_offs;
    lv_color32_t* dst_data;
    color_t*      src_data = (color_t*)px_map + (LV_X11_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL ? 0 : hor_res*area->y1 + area->x1);
    for (int16_t y = area->y1; y <= area->y2; y++) {
        dst_offs = area->x1 + y * hor_res;
        dst_data = &((lv_color32_t*)(xd->xdata))[dst_offs];
        for (int16_t x = area->x1; x <= area->x2; x++, src_data++, dst_data++) {
            *dst_data = get_px(*src_data);
        }
        src_data += (LV_X11_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL ? 0 : hor_res - (area->x2 - area->x1 + 1));
    }

    if (lv_display_flush_is_last(disp)) {
        /* refresh collected display update area only */
        int16_t upd_w = xd->flush_area.x2 - xd->flush_area.x1 + 1;
        int16_t upd_h = xd->flush_area.y2 - xd->flush_area.y1 + 1;
        if (xd->resize) {
            XResizeWindow(xd->hdr.display, xd->window, hor_res, ver_res);
            xd->resize = false;
        }
        XPutImage(xd->hdr.display, xd->window, xd->gc, xd->ximage, xd->flush_area.x1, xd->flush_area.y1, xd->flush_area.x1, xd->flush_area.y1, upd_w, upd_h);
        /* invalidate collected area */
        xd->flush_area = inv_area;
    }
    /*Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp);
}

static void x11_hide_cursor(x11_data_t* xd)
{
    XColor black = { .red = 0, .green = 0, .blue = 0 };
    char empty_data[] = { 0 };

    Pixmap empty_bitmap = XCreateBitmapFromData(xd->hdr.display, xd->window, empty_data, 1, 1);
    Cursor inv_cursor = XCreatePixmapCursor(xd->hdr.display, empty_bitmap, empty_bitmap, &black, &black, 0, 0);
    XDefineCursor(xd->hdr.display, xd->window, inv_cursor);
    XFreeCursor(xd->hdr.display, inv_cursor);
    XFreePixmap(xd->hdr.display, empty_bitmap);
}

/**
 * X11 input event handler, only handling display related events (Expose)
 */
static int is_disp_event(Display* disp, XEvent* evt, XPointer arg)
{
    LV_UNUSED(disp);
    LV_UNUSED(arg);
    return (evt->type == Expose
         || evt->type == ResizeRequest
         || evt->type == ClientMessage);
}
static void event_handler(lv_timer_t* t)
{
    lv_display_t* disp = lv_timer_get_user_data(t);
    LV_ASSERT_NULL(disp);
    x11_data_t* xd = (x11_data_t*)lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(xd);


    /* handle all outstanding X events */
    XEvent myevent;
    while (XCheckIfEvent(xd->hdr.display, &myevent, is_disp_event, NULL)) {
        LV_LOG_TRACE("Display Event %d", myevent.type);
        switch(myevent.type)
        {
        case Expose:
            if(myevent.xexpose.count==0)
            {
                int32_t hor_res = lv_display_get_horizontal_resolution(disp);
                int32_t ver_res = lv_display_get_vertical_resolution(disp);
                XPutImage(xd->hdr.display, xd->window, xd->gc, xd->ximage, 0, 0, 0, 0, hor_res, ver_res);
            }
            break;
        case ResizeRequest:
            /* foce window redraw with size restore */
            xd->resize = true;
            lv_obj_invalidate(lv_screen_active());
            break;
        case ClientMessage:
            if (myevent.xclient.data.l[0] == (long)xd->wmDeleteMessage) {
                xd->terminated = true;
                void* ret = NULL;
                pthread_join(xd->thr_tick, &ret);
                if (NULL != xd->close_cb) {
                    xd->close_cb(disp);
                }
            }
            break;
        }
    }
}


static void* tick_thread(void *data)
{
    x11_data_t* xd = (x11_data_t*)data;
    LV_ASSERT_NULL(xd);

    while(!xd->terminated) {
        usleep(5000);
        lv_tick_inc(5);
    }
    return NULL;
}

static void x11_window_create(lv_display_t* disp, char const* title)
{
    x11_data_t* xd = (x11_data_t*)lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(xd);

    /* setup display/screen */
    xd->hdr.display = XOpenDisplay(NULL);
    int screen = DefaultScreen(xd->hdr.display);

    /* drawing contexts for an window */
    unsigned long col_fg = BlackPixel(xd->hdr.display, screen);
    unsigned long col_bg = WhitePixel(xd->hdr.display, screen);

    /* create window */
    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);
#if 1
    xd->window = XCreateSimpleWindow(xd->hdr.display, DefaultRootWindow(xd->hdr.display),
                                     0, 0, hor_res, ver_res, 0, col_fg, col_bg);
#else
    xd->window = XCreateWindow(xd->hdr.display, DefaultRootWindow(xd->hdr.display),
                                   0, 0, hor_res, ver_res, 0,
                                   DefaultDepth(xd->hdr.display,screen), CopyFromParent,
                                   XDefaultVisual(xd->hdr.display, screen), 0, NULL);
#endif
    /* window manager properties (yes, use of StdProp is obsolete) */
    XSetStandardProperties(xd->hdr.display, xd->window, title, NULL, None, NULL, 0, NULL);

    /* allow receiving mouse and keyboard events */
    XSelectInput(xd->hdr.display, xd->window, PointerMotionMask|ButtonPressMask|ButtonReleaseMask|KeyPressMask|KeyReleaseMask|ExposureMask|ResizeRedirectMask);
    xd->wmDeleteMessage = XInternAtom(xd->hdr.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(xd->hdr.display, xd->window, &xd->wmDeleteMessage, 1);

    /* graphics context */
    xd->gc = XCreateGC(xd->hdr.display, xd->window, 0, 0);

    x11_hide_cursor(xd);

    /* create cache XImage */
    xd->xdata = lv_malloc(hor_res * ver_res * sizeof(uint32_t));
    xd->visual = XDefaultVisual(xd->hdr.display, screen);
    xd->dplanes = DisplayPlanes(xd->hdr.display, screen);

    size_t sz_buffers = hor_res * ver_res * sizeof(lv_color32_t);
    xd->xdata = malloc(sz_buffers); /* use clib method here, x11 memory not part of device footprint */
    xd->ximage = XCreateImage(xd->hdr.display, xd->visual, xd->dplanes, ZPixmap, 0, xd->xdata,
                              hor_res, ver_res, lv_color_format_get_bpp(LV_COLOR_FORMAT_ARGB8888), 0);

    /* finally bring window on top of the other windows */
    XMapRaised(xd->hdr.display, xd->window);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_x11_window_create(char const* title, int32_t hor_res, int32_t ver_res)
{
    x11_data_t* xd = lv_malloc_zeroed(sizeof(x11_data_t));
    LV_ASSERT_MALLOC(xd);
    if (xd == NULL) return NULL;

    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if (disp == NULL) {
        lv_free(xd);
        return NULL;
    }
    lv_display_set_driver_data(disp, xd);
    lv_display_set_flush_cb(disp, x11_flush_cb);

    x11_window_create(disp, title);

    int sz_buffers = (hor_res * ver_res * (LV_COLOR_DEPTH + 7)/8);
    if (LV_X11_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        sz_buffers /= 10;
    }
    xd->buffer[0] = lv_malloc(sz_buffers);
    xd->buffer[1] = (LV_X11_DOUBLE_BUFFER ? lv_malloc(sz_buffers) : NULL);
    lv_display_set_draw_buffers(disp, xd->buffer[0], xd->buffer[1], sz_buffers, LV_X11_RENDER_MODE);

    xd->timer = lv_timer_create(event_handler, 5, disp);

    /* initialize Tick simulation */
    xd->terminated = false;
    pthread_create(&xd->thr_tick, NULL, tick_thread, xd);

    return disp;
}

void lv_x11_window_set_close_cb(lv_display_t* disp, lv_x11_close_cb close_cb)
{
    x11_data_t* xd = (x11_data_t*)lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(xd);
    xd->close_cb = close_cb;
}

#endif /*LV_USE_X11*/
