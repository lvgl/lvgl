/**
 * @file lv_wl_shm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland.h"
#if LV_USE_WAYLAND

#include "lv_wayland_private.h"
#include <string.h>
#include <src/display/lv_display.h>
#include <src/drivers/wayland/lv_wayland_smm.h>
#include <src/misc/lv_types.h>
#include <wayland-cursor.h>

/*********************
 *      DEFINES
 *********************/

#define SHM_FORMAT_UNKNOWN 0xFFFFFF

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
/*
 *  shm_format
 *  @description called by the compositor to advertise the supported
 *  color formats for SHM buffers, there is a call per supported format
 */
static void shm_format(void * data, struct wl_shm * wl_shm, uint32_t format);
static void handle_wl_buffer_release(void * data, struct wl_buffer * wl_buffer);
static bool sme_new_pool(void * ctx, smm_pool_t * pool);
static void sme_expand_pool(void * ctx, smm_pool_t * pool);
static void sme_free_pool(void * ctx, smm_pool_t * pool);
static bool sme_new_buffer(void * ctx, smm_buffer_t * buf);
static bool sme_init_buffer(void * ctx, smm_buffer_t * buf);
static void sme_free_buffer(void * ctx, smm_buffer_t * buf);

/**********************
 *  STATIC VARIABLES
 **********************/

static const struct smm_events sme_events = {NULL,           sme_new_pool,    sme_expand_pool, sme_free_pool,
           sme_new_buffer, sme_init_buffer, sme_free_buffer
};

static const struct wl_shm_listener shm_listener = {.format = shm_format};

static const struct wl_buffer_listener wl_buffer_listener = {
    .release = handle_wl_buffer_release,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

void lv_wayland_shm_initalize_context(shm_ctx_t * context)
{
    memset(context, 0, sizeof(*context));
    context->format = SHM_FORMAT_UNKNOWN;
    smm_init(&sme_events);
    smm_setctx(context);
}

void lv_wayland_shm_set_interface(shm_ctx_t * context, struct wl_registry * registry, uint32_t name,
                                  const char * interface, uint32_t version)
{
    LV_UNUSED(version);
    LV_UNUSED(interface);
    context->handler = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    wl_shm_add_listener(context->handler, &shm_listener, context);
}

lv_result_t lv_wayland_shm_is_ready(shm_ctx_t * context)
{
    return (context->handler && context->format != SHM_FORMAT_UNKNOWN) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

/* TODO: Move all cursor functions to a lv_wl_cursor file*/
struct wl_cursor_theme * lv_wayland_shm_load_cursor_theme(shm_ctx_t * context)
{
    return wl_cursor_theme_load(NULL, 32, context->handler);
}

void lv_wayland_shm_deinit(shm_ctx_t * context)
{
    smm_deinit();
    wl_shm_destroy(context->handler);
}

struct graphic_object * lv_wayland_shm_on_graphical_object_creation(shm_ctx_t * context, struct graphic_object * obj)
{
    LV_UNUSED(context);
    obj->pending_buffer = NULL;
    obj->buffer_group   = smm_create();
    if(obj->buffer_group == NULL) {
        LV_LOG_ERROR("Failed to create buffer group for graphic object");
        lv_free(obj);
        return NULL;
    }
    SMM_TAG(obj->buffer_group, TAG_LOCAL, obj);
    return obj;
}
void lv_wayland_shm_on_graphical_object_destruction(shm_ctx_t * context, struct graphic_object * obj)
{
    LV_UNUSED(context);
    smm_destroy(obj->buffer_group);
}

lv_result_t lv_wayland_shm_resize_window(shm_ctx_t * context, struct window * window, int32_t width, int32_t height)
{
    const uint8_t bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);

    /* Update size for newly allocated buffers */
    smm_resize(window->body->buffer_group, ((width * bpp) * height) * 2);

    window->body->width  = width;
    window->body->height = height;

    /* Pre-allocate two buffers for the window body here */
    struct smm_buffer_t * body_buf1 = smm_acquire(window->body->buffer_group);
    struct smm_buffer_t * body_buf2 = smm_acquire(window->body->buffer_group);

    if(smm_map(body_buf2) == NULL) {
        LV_LOG_ERROR("Cannot pre-allocate backing buffers for window body");
        wl_surface_destroy(window->body->surface);
        return LV_RESULT_INVALID;
    }

    /* Moves the buffers to the the unused list of the group */
    smm_release(body_buf1);
    smm_release(body_buf2);

    LV_LOG_TRACE("resize window:%dx%d body:%dx%d frame: %d", window->width, window->height, window->body->width,
                 window->body->height, window->frame_counter);

    width  = window->body->width;
    height = window->body->height;

    if(window->lv_disp != NULL) {
        /* Resize draw buffer */
        const uint32_t stride = lv_draw_buf_width_to_stride(width, lv_display_get_color_format(window->lv_disp));
        context->lv_draw_buf  = lv_draw_buf_reshape(context->lv_draw_buf, lv_display_get_color_format(window->lv_disp),
                                                    width, height / LVGL_DRAW_BUFFER_DIV, stride);
    }

    return LV_RESULT_OK;
}
lv_result_t lv_wayland_shm_create_draw_buffers(shm_ctx_t * context, struct window * window)
{

    const uint32_t stride = lv_draw_buf_width_to_stride(window->width, lv_display_get_color_format(window->lv_disp));

    context->lv_draw_buf = lv_draw_buf_create(window->width, window->height / LVGL_DRAW_BUFFER_DIV,
                                              lv_display_get_color_format(window->lv_disp), stride);
    return LV_RESULT_OK;
}

lv_result_t lv_wayland_shm_set_draw_buffers(shm_ctx_t * context, lv_display_t * display)
{
    if(LV_WAYLAND_BUF_COUNT != 1) {
        LV_LOG_ERROR("Wayland without dmabuf only supports 1 drawbuffer for now.");
        return LV_RESULT_INVALID;
    }
    lv_display_set_draw_buffers(display, context->lv_draw_buf, NULL);
    return LV_RESULT_OK;
}

void lv_wayland_shm_delete_draw_buffers(shm_ctx_t * context, struct window * window)
{
    LV_UNUSED(window);
    lv_draw_buf_destroy(context->lv_draw_buf);
}
void lv_wayland_shm_flush_partial_mode(lv_display_t * disp, const lv_area_t * area, unsigned char * color_p)
{
    struct window * window    = lv_display_get_user_data(disp);
    uint32_t format           = window->wl_ctx->shm_ctx.format;
    smm_buffer_t * buf        = window->body->pending_buffer;
    int32_t src_width         = lv_area_get_width(area);
    int32_t src_height        = lv_area_get_height(area);
    uint8_t bpp               = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);
    lv_display_rotation_t rot = lv_display_get_rotation(disp);
    int32_t w                 = lv_display_get_horizontal_resolution(disp);
    int32_t h                 = lv_display_get_vertical_resolution(disp);

    /* TODO actually test what happens if the rotation is 90 or 270 or 180 ? */
    int32_t hres = (rot == LV_DISPLAY_ROTATION_0) ? w : h;
    int32_t vres = (rot == LV_DISPLAY_ROTATION_0) ? h : w;

    /* If window has been / is being closed, or is not visible, skip flush */
    if(window->closed || window->shall_close) {
        goto skip;
    }
    /* Skip if the area is out the screen */
    else if((area->x2 < 0) || (area->y2 < 0) || (area->x1 > hres - 1) || (area->y1 > vres - 1)) {
        goto skip;
    }

    /* Acquire and map a buffer to attach/commit to surface */
    if(buf == NULL) {
        buf = smm_acquire(window->body->buffer_group);
        if(buf == NULL) {
            LV_LOG_ERROR("cannot acquire a window body buffer");
            goto skip;
        }

        window->body->pending_buffer = buf;
        SMM_TAG(buf, TAG_BUFFER_DAMAGE, window->dmg_cache.cache + window->dmg_cache.end);
    }

    void * buf_base = smm_map(buf);
    if(buf_base == NULL) {
        LV_LOG_ERROR("cannot map in window body buffer");
        goto skip;
    }

    /* Modify specified area in buffer */
    for(int32_t y = 0; y < src_height; ++y) {
        if(format == WL_SHM_FORMAT_ARGB8888) {
            for(int32_t x = 0; x < src_width; ++x) {
                lv_color_premultiply((lv_color32_t *)color_p + x);
            }
        }
        memcpy(((char *)buf_base) + ((((area->y1 + y) * hres) + area->x1) * bpp), color_p, src_width * bpp);
        color_p += src_width * bpp;
    }

    /* Mark surface damage */
    wl_surface_damage(window->body->surface, area->x1, area->y1, src_width, src_height);

    lv_wayland_cache_add_area(window, buf, area);

    if(lv_display_flush_is_last(disp)) {
        /* Finally, attach buffer and commit to surface */
        struct wl_buffer * wl_buf = SMM_BUFFER_PROPERTIES(buf)->tag[TAG_LOCAL];
        wl_surface_attach(window->body->surface, wl_buf, 0, 0);
        wl_surface_commit(window->body->surface);
        window->body->pending_buffer = NULL;

        struct wl_callback * cb = wl_surface_frame(window->body->surface);
        wl_callback_add_listener(cb, lv_wayland_window_get_wl_surface_frame_listener(), window->body);
        LV_LOG_TRACE("last flush frame: %d", window->frame_counter);

        window->flush_pending = true;
        /* Return early here, the lv_display_flush_ready will get called in the frame_listener callback */
        return;
    }
    lv_display_flush_ready(disp);
    return;
skip:
    if(buf != NULL) {
        /* Cleanup any intermediate state (in the event that this flush being
         * skipped is in the middle of a flush sequence)
         */
        lv_wayland_cache_clear(window);
        SMM_TAG(buf, TAG_BUFFER_DAMAGE, NULL);
        smm_release(buf);
        window->body->pending_buffer = NULL;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void shm_format(void * data, struct wl_shm * wl_shm, uint32_t format)
{
    shm_ctx_t * shm_ctx = (shm_ctx_t *)data;

    LV_UNUSED(wl_shm);

    LV_LOG_TRACE("Supported color space fourcc.h code: %08X", format);

    if(LV_COLOR_DEPTH == 32 && format == WL_SHM_FORMAT_ARGB8888) {
        LV_LOG_TRACE("Found WL_SHM_FORMAT_ARGB8888");

        /* Wayland compositors MUST support ARGB8888 */
        shm_ctx->format = format;

    }
    else if(LV_COLOR_DEPTH == 32 && format == WL_SHM_FORMAT_XRGB8888 && shm_ctx->format != WL_SHM_FORMAT_ARGB8888) {

        LV_LOG_TRACE("Found WL_SHM_FORMAT_XRGB8888");
        /* Select XRGB only if the compositor doesn't support transprancy */
        shm_ctx->format = format;

    }
    else if(LV_COLOR_DEPTH == 16 && format == WL_SHM_FORMAT_RGB565) {

        shm_ctx->format = format;
    }
}

static void handle_wl_buffer_release(void * data, struct wl_buffer * wl_buffer)
{
    const struct smm_buffer_properties * props;
    struct graphic_object * obj;
    struct window * window;
    smm_buffer_t * buf;
    /* window is unused when LV_LOG level is not set to trace */
    LV_UNUSED(window);
    LV_UNUSED(wl_buffer);

    buf    = (smm_buffer_t *)data;
    props  = SMM_BUFFER_PROPERTIES(buf);
    obj    = SMM_GROUP_PROPERTIES(props->group)->tag[TAG_LOCAL];
    window = obj->window;

    LV_LOG_TRACE("releasing buffer %p wl_buffer %p w:%d h:%d frame: %d", (smm_buffer_t *)data, (void *)wl_buffer,
                 obj->width, obj->height, window->frame_counter);
    smm_release((smm_buffer_t *)data);
}

static bool sme_new_pool(void * ctx, smm_pool_t * pool)
{
    struct wl_shm_pool * wl_pool;
    shm_ctx_t * shm_ctx                      = ctx;
    const struct smm_pool_properties * props = SMM_POOL_PROPERTIES(pool);

    LV_UNUSED(ctx);

    wl_pool = wl_shm_create_pool(shm_ctx->handler, props->fd, props->size);

    SMM_TAG(pool, TAG_LOCAL, wl_pool);
    return (wl_pool == NULL);
}

static void sme_expand_pool(void * ctx, smm_pool_t * pool)
{
    const struct smm_pool_properties * props = SMM_POOL_PROPERTIES(pool);

    LV_UNUSED(ctx);

    wl_shm_pool_resize(props->tag[TAG_LOCAL], props->size);
}

static void sme_free_pool(void * ctx, smm_pool_t * pool)
{
    struct wl_shm_pool * wl_pool = SMM_POOL_PROPERTIES(pool)->tag[TAG_LOCAL];

    LV_UNUSED(ctx);

    wl_shm_pool_destroy(wl_pool);
}

static bool sme_new_buffer(void * ctx, smm_buffer_t * buf)
{

    struct wl_buffer * wl_buf;
    bool fail_alloc                            = true;
    const struct smm_buffer_properties * props = SMM_BUFFER_PROPERTIES(buf);
    struct wl_shm_pool * wl_pool               = SMM_POOL_PROPERTIES(props->pool)->tag[TAG_LOCAL];
    shm_ctx_t * shm_ctx                        = (shm_ctx_t *)ctx;
    struct graphic_object * obj                = SMM_GROUP_PROPERTIES(props->group)->tag[TAG_LOCAL];
    uint8_t bpp;

    LV_LOG_TRACE("create new buffer of width %d height %d", obj->width, obj->height);

    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);
    wl_buf =
        wl_shm_pool_create_buffer(wl_pool, props->offset, obj->width, obj->height, obj->width * bpp, shm_ctx->format);

    if(wl_buf != NULL) {
        wl_buffer_add_listener(wl_buf, &wl_buffer_listener, buf);
        SMM_TAG(buf, TAG_LOCAL, wl_buf);
        SMM_TAG(buf, TAG_BUFFER_DAMAGE, NULL);
        fail_alloc = false;
    }

    return fail_alloc;
}

static bool sme_init_buffer(void * ctx, smm_buffer_t * buf)
{
    smm_buffer_t * src;
    void * src_base;
    bool fail_init                             = true;
    bool dmg_missing                           = false;
    void * buf_base                            = smm_map(buf);
    const struct smm_buffer_properties * props = SMM_BUFFER_PROPERTIES(buf);
    struct graphic_object * obj                = SMM_GROUP_PROPERTIES(props->group)->tag[TAG_LOCAL];
    uint8_t bpp;

    LV_UNUSED(ctx);

    if(buf_base == NULL) {
        LV_LOG_ERROR("cannot map in buffer to initialize");
        goto done;
    }

    /* Determine if all subsequent buffers damage is recorded */
    for(src = smm_next(buf); src != NULL; src = smm_next(src)) {
        if(SMM_BUFFER_PROPERTIES(src)->tag[TAG_BUFFER_DAMAGE] == NULL) {
            dmg_missing = true;
            break;
        }
    }

    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);

    if((smm_next(buf) == NULL) || dmg_missing) {
        /* Missing subsequent buffer damage, initialize by copying the most
         * recently acquired buffers data
         */
        src = smm_latest(props->group);
        if((src != NULL) && (src != buf)) {
            /* Map and copy latest buffer data */
            src_base = smm_map(src);
            if(src_base == NULL) {
                LV_LOG_ERROR("cannot map most recent buffer to copy");
                goto done;
            }

            memcpy(buf_base, src_base, (obj->width * bpp) * obj->height);
        }
    }
    else {
        /* All subsequent buffers damage is recorded, initialize by applying
         * their damage to this buffer
         */
        for(src = smm_next(buf); src != NULL; src = smm_next(src)) {
            src_base = smm_map(src);
            if(src_base == NULL) {
                LV_LOG_ERROR("cannot map source buffer to copy from");
                goto done;
            }

            lv_wayland_cache_apply_areas(obj->window, buf_base, src_base, src);
        }

        /* Purge out-of-date cached damage (up to and including next buffer) */
        src = smm_next(buf);
        if(src == NULL) {
            lv_wayland_cache_purge(obj->window, src);
        }
    }

    fail_init = false;
done:
    return fail_init;
}

static void sme_free_buffer(void * ctx, smm_buffer_t * buf)
{
    struct wl_buffer * wl_buf = SMM_BUFFER_PROPERTIES(buf)->tag[TAG_LOCAL];

    LV_UNUSED(ctx);

    wl_buffer_destroy(wl_buf);
}

#endif /* LV_USE_WAYLAND */
