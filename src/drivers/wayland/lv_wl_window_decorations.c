/**
 * @file lv_wl_window_decorations.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland.h"

#if LV_WAYLAND_WINDOW_DECORATIONS

#include "lv_wayland_private.h"
#include <errno.h>
#include <unistd.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/*
 * Fills a buffer with a color
 * @description Used to draw the decorations, by writing directly to the SHM buffer,
 * most wayland compositors support the ARGB8888, XRGB8888, RGB565 formats
 *
 * For color depths usually not natively supported by wayland i.e RGB332, Grayscale
 * A conversion is performed to match the format of the SHM buffer read by the compositor.
 *
 * This function can also be used as a visual debugging aid to see how damage is applied
 *
 * @param pixels pointer to the buffer to fill
 * @param lv_color_t color the color that will be used for the fill
 * @param width width of the filled area
 * @param height height of the filled area
 *
 */
static void color_fill(void * pixels, lv_color_t color, uint32_t width, uint32_t height);
static void color_fill_XRGB8888(void * pixels, lv_color_t color, uint32_t width, uint32_t height);
static void color_fill_RGB565(void * pixels, lv_color_t color, uint32_t width, uint32_t height);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_wayland_window_decoration_attach(struct window * window, struct graphic_object * decoration,
                                         void * decoration_buffer, struct graphic_object * parent)
{
#if LV_WAYLAND_USE_DMABUF
    struct wl_buffer * wl_buf = ((struct buffer *)decoration_buffer)->buffer;
#else
    struct wl_buffer * wl_buf = SMM_BUFFER_PROPERTIES((smm_buffer_t *)decoration_buffer)->tag[TAG_LOCAL];
#endif


    int pos_x, pos_y;

    switch(decoration->type) {
        case OBJECT_TITLEBAR:
            pos_x = 0;
            pos_y = -TITLE_BAR_HEIGHT;
            break;
        case OBJECT_BUTTON_CLOSE:
            pos_x = parent->width - 1 * (BUTTON_MARGIN + BUTTON_SIZE);
            pos_y = -1 * (BUTTON_MARGIN + BUTTON_SIZE + (BORDER_SIZE / 2));
            break;
#if LV_WAYLAND_XDG_SHELL
        case OBJECT_BUTTON_MAXIMIZE:
            pos_x = parent->width - 2 * (BUTTON_MARGIN + BUTTON_SIZE);
            pos_y = -1 * (BUTTON_MARGIN + BUTTON_SIZE + (BORDER_SIZE / 2));
            break;
        case OBJECT_BUTTON_MINIMIZE:
            pos_x = parent->width - 3 * (BUTTON_MARGIN + BUTTON_SIZE);
            pos_y = -1 * (BUTTON_MARGIN + BUTTON_SIZE + (BORDER_SIZE / 2));
            break;
#endif
        case OBJECT_BORDER_TOP:
            pos_x = -BORDER_SIZE;
            pos_y = -(BORDER_SIZE + TITLE_BAR_HEIGHT);
            break;
        case OBJECT_BORDER_BOTTOM:
            pos_x = -BORDER_SIZE;
            pos_y = parent->height;
            break;
        case OBJECT_BORDER_LEFT:
            pos_x = -BORDER_SIZE;
            pos_y = -TITLE_BAR_HEIGHT;
            break;
        case OBJECT_BORDER_RIGHT:
            pos_x = parent->width;
            pos_y = -TITLE_BAR_HEIGHT;
            break;
        default:
            LV_ASSERT_MSG(0, "Invalid object type");
            return false;
    }

    /* Enable this, to make it function on weston 10.0.2 */
    /* It's not elegant but it forces weston to size the surfaces before */
    /* the conversion to a subsurface takes place */

    /* Likely related to this issue, some patches were merged into 10.0.0 */
    /* https://gitlab.freedesktop.org/wayland/weston/-/issues/446 */
    /* Moreover, it crashes on GNOME */

#if 0
    wl_surface_attach(decoration->surface, wl_buf, 0, 0);
    wl_surface_commit(decoration->surface);
#endif

    if(decoration->subsurface == NULL) {
        /* Create the subsurface only once */

        decoration->subsurface =
            wl_subcompositor_get_subsurface(window->wl_ctx->subcompositor, decoration->surface, parent->surface);
        if(!decoration->subsurface) {
            LV_LOG_ERROR("cannot get subsurface for decoration");
            goto err_destroy_surface;
        }
    }

    wl_subsurface_set_position(decoration->subsurface, pos_x, pos_y);
    wl_surface_attach(decoration->surface, wl_buf, 0, 0);
    wl_surface_commit(decoration->surface);

    return true;

err_destroy_surface:
    wl_surface_destroy(decoration->surface);
    decoration->surface = NULL;

    return false;
}

uint32_t lv_wayland_window_decoration_create_all(struct window * window)
{
    uint32_t created = 0;
    for(size_t i = 0; i < NUM_DECORATIONS; i++) {
        if(lv_wayland_window_decoration_create(window, window->decoration[i], window->body->width,
                                               window->body->height)) {
            created++;
            continue;
        }
        LV_LOG_ERROR("failed to create decoration %zu", i);
    }
    return created;
}

void lv_wayland_window_decoration_detach_all(struct window * window)
{
    for(int i = 0; i < NUM_DECORATIONS; i++) {
        lv_wayland_window_decoration_detach(window, window->decoration[i]);
    }
}

bool lv_wayland_window_decoration_create(struct window * window, struct graphic_object * decoration, int window_width,
                                         int window_height)
{
#if LV_WAYLAND_USE_DMABUF
    struct buffer * buf;
#else
    smm_buffer_t * buf;
#endif
    void * buf_base;
    int x, y;
    lv_color_t * pixel;
    uint8_t bpp;

    switch(decoration->type) {
        case OBJECT_TITLEBAR:
            decoration->width  = window_width;
            decoration->height = TITLE_BAR_HEIGHT;
            break;
        case OBJECT_BUTTON_CLOSE:
            decoration->width  = BUTTON_SIZE;
            decoration->height = BUTTON_SIZE;
            break;
#if LV_WAYLAND_XDG_SHELL
        case OBJECT_BUTTON_MAXIMIZE:
            decoration->width  = BUTTON_SIZE;
            decoration->height = BUTTON_SIZE;
            break;
        case OBJECT_BUTTON_MINIMIZE:
            decoration->width  = BUTTON_SIZE;
            decoration->height = BUTTON_SIZE;
            break;
#endif
        case OBJECT_BORDER_TOP:
            decoration->width  = window_width + 2 * (BORDER_SIZE);
            decoration->height = BORDER_SIZE;
            break;
        case OBJECT_BORDER_BOTTOM:
            decoration->width  = window_width + 2 * (BORDER_SIZE);
            decoration->height = BORDER_SIZE;
            break;
        case OBJECT_BORDER_LEFT:
            decoration->width  = BORDER_SIZE;
            decoration->height = window_height + TITLE_BAR_HEIGHT;
            break;
        case OBJECT_BORDER_RIGHT:
            decoration->width  = BORDER_SIZE;
            decoration->height = window_height + TITLE_BAR_HEIGHT;
            break;
        default:
            LV_ASSERT_MSG(0, "Invalid object type");
            return false;
    }
#if LV_WAYLAND_USE_DMABUF
    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);
    buf = dmabuf_acquire_pool_buffer(window, decoration);
    buf_base = mmap(0, (decoration->width * bpp) * decoration->height, PROT_READ | PROT_WRITE, MAP_SHARED,
                    buf->dmabuf_fds[0], 0);
    if(buf_base == MAP_FAILED) {
        destroy_decorators_buf(window, decoration);
        LV_LOG_ERROR("cannot map in allocated decoration buffer %d (%s)", errno, strerror(errno));
        return false;
    }
#else
    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);

    LV_LOG_TRACE("decoration window %dx%d", decoration->width, decoration->height);

    smm_resize(decoration->buffer_group, (decoration->width * bpp) * decoration->height);

    buf = smm_acquire(decoration->buffer_group);

    if(buf == NULL) {
        LV_LOG_ERROR("cannot allocate buffer for decoration");
        return false;
    }

    buf_base = smm_map(buf);
    if(buf_base == NULL) {
        LV_LOG_ERROR("cannot map in allocated decoration buffer");
        smm_release(buf);
        return false;
    }
#endif
    switch(decoration->type) {
        case OBJECT_TITLEBAR:
            color_fill(buf_base, lv_color_make(0x66, 0x66, 0x66), decoration->width, decoration->height);
            break;
        case OBJECT_BUTTON_CLOSE:
            color_fill(buf_base, lv_color_make(0xCC, 0xCC, 0xCC), decoration->width, decoration->height);
            for(y = 0; y < decoration->height; y++) {
                for(x = 0; x < decoration->width; x++) {
                    pixel = (lv_color_t *)((unsigned char *)buf_base + (y * (decoration->width * bpp)) + x * bpp);
                    if((x >= BUTTON_PADDING) && (x < decoration->width - BUTTON_PADDING)) {
                        if((x == y) || (x == decoration->width - 1 - y)) {
                            color_fill(pixel, lv_color_make(0x33, 0x33, 0x33), 1, 1);
                        }
                        else if((x == y - 1) || (x == decoration->width - y)) {
                            color_fill(pixel, lv_color_make(0x66, 0x66, 0x66), 1, 1);
                        }
                    }
                }
            }
            break;
#if LV_WAYLAND_XDG_SHELL
        case OBJECT_BUTTON_MAXIMIZE:
            color_fill(buf_base, lv_color_make(0xCC, 0xCC, 0xCC), decoration->width, decoration->height);
            for(y = 0; y < decoration->height; y++) {
                for(x = 0; x < decoration->width; x++) {
                    pixel = (lv_color_t *)((unsigned char *)buf_base + (y * (decoration->width * bpp)) + x * bpp);
                    if(((x == BUTTON_PADDING) && (y >= BUTTON_PADDING) && (y < decoration->height - BUTTON_PADDING)) ||
                       ((x == (decoration->width - BUTTON_PADDING)) && (y >= BUTTON_PADDING) &&
                        (y <= decoration->height - BUTTON_PADDING)) ||
                       ((y == BUTTON_PADDING) && (x >= BUTTON_PADDING) && (x < decoration->width - BUTTON_PADDING)) ||
                       ((y == (BUTTON_PADDING + 1)) && (x >= BUTTON_PADDING) &&
                        (x < decoration->width - BUTTON_PADDING)) ||
                       ((y == (decoration->height - BUTTON_PADDING)) && (x >= BUTTON_PADDING) &&
                        (x < decoration->width - BUTTON_PADDING))) {
                        color_fill(pixel, lv_color_make(0x33, 0x33, 0x33), 1, 1);
                    }
                }
            }
            break;
        case OBJECT_BUTTON_MINIMIZE:
            color_fill(buf_base, lv_color_make(0xCC, 0xCC, 0xCC), decoration->width, decoration->height);
            for(y = 0; y < decoration->height; y++) {
                for(x = 0; x < decoration->width; x++) {
                    pixel = (lv_color_t *)((unsigned char *)buf_base + (y * (decoration->width * bpp)) + x * bpp);
                    if((x >= BUTTON_PADDING) && (x < decoration->width - BUTTON_PADDING) &&
                       (y > decoration->height - (2 * BUTTON_PADDING)) && (y < decoration->height - BUTTON_PADDING)) {
                        color_fill(pixel, lv_color_make(0x33, 0x33, 0x33), 1, 1);
                    }
                }
            }
            break;
#endif
        case OBJECT_BORDER_TOP:
        /* fallthrough */
        case OBJECT_BORDER_BOTTOM:
        /* fallthrough */
        case OBJECT_BORDER_LEFT:
        /* fallthrough */
        case OBJECT_BORDER_RIGHT:
            color_fill(buf_base, lv_color_make(0x66, 0x66, 0x66), decoration->width, decoration->height);
            break;
        default:
            LV_ASSERT_MSG(0, "Invalid object type");
            return false;
    }

    bool ret = lv_wayland_window_decoration_attach(window, decoration, buf, window->body);

#if LV_WAYLAND_USE_DMABUF
    munmap(buf_base, (decoration->width * bpp) * decoration->height);
#endif

    return ret;
}

void lv_wayland_window_decoration_detach(struct window * window, struct graphic_object * decoration)
{

    LV_UNUSED(window);

    if(decoration->subsurface) {
        wl_subsurface_destroy(decoration->subsurface);
        decoration->subsurface = NULL;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void color_fill(void * pixels, lv_color_t color, uint32_t width, uint32_t height)
{

    switch(lv_wl_ctx.shm_ctx.format) {
        case WL_SHM_FORMAT_ARGB8888:
            color_fill_XRGB8888(pixels, color, width, height);
            break;
        case WL_SHM_FORMAT_RGB565:
            color_fill_RGB565(pixels, color, width, height);
            break;
        default:
            LV_ASSERT_MSG(0, "Unsupported WL_SHM_FORMAT");
            break;
    }
}

static void color_fill_XRGB8888(void * pixels, lv_color_t color, uint32_t width, uint32_t height)
{
    unsigned char * buf = pixels;
    unsigned char * buf_end;

    buf_end = (unsigned char *)((uint32_t *)buf + width * height);

    while(buf < buf_end) {
        *(buf++) = color.blue;
        *(buf++) = color.green;
        *(buf++) = color.red;
        *(buf++) = 0xFF;
    }
}

static void color_fill_RGB565(void * pixels, lv_color_t color, uint32_t width, uint32_t height)
{
    uint16_t * buf = pixels;
    uint16_t * buf_end;

    buf_end = (uint16_t *)buf + width * height;

    while(buf < buf_end) {
        *(buf++) = lv_color_to_u16(color);
    }
}

#endif /* LV_WAYLAND_WINDOW_DECORATIONS */
