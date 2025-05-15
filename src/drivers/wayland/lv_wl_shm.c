/**
 * @file lv_wl_shm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland.h"
#include <string.h>
#if LV_USE_WAYLAND

#include "lv_wayland_private.h"

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

const struct wl_shm_listener * lv_wayland_shm_get_listener(void)
{
    return &shm_listener;
}

const struct smm_events * lv_wayland_sme_get_events(void)
{
    return &sme_events;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void shm_format(void * data, struct wl_shm * wl_shm, uint32_t format)
{
    struct application * app = data;

    LV_UNUSED(wl_shm);

    LV_LOG_WARN("Supported color space fourcc.h code: %08X", format);

    if(LV_COLOR_DEPTH == 32 && format == WL_SHM_FORMAT_ARGB8888) {
        LV_LOG_WARN("Found WL_SHM_FORMAT_ARGB8888");

        /* Wayland compositors MUST support ARGB8888 */
        app->shm_format = format;

    }
    else if(LV_COLOR_DEPTH == 32 && format == WL_SHM_FORMAT_XRGB8888 && app->shm_format != WL_SHM_FORMAT_ARGB8888) {

        LV_LOG_WARN("Found WL_SHM_FORMAT_XRGB8888");
        /* Select XRGB only if the compositor doesn't support transprancy */
        app->shm_format = format;

    }
    else if(LV_COLOR_DEPTH == 16 && format == WL_SHM_FORMAT_RGB565) {

        app->shm_format = format;
    }
}

static void handle_wl_buffer_release(void * data, struct wl_buffer * wl_buffer)
{
    const struct smm_buffer_properties * props;
    struct graphic_object * obj;
    struct window * window;
    smm_buffer_t * buf;

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
    struct application * app                 = ctx;
    const struct smm_pool_properties * props = SMM_POOL_PROPERTIES(pool);

    LV_UNUSED(ctx);

    wl_pool = wl_shm_create_pool(app->shm, props->fd, props->size);

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
    struct application * app                   = ctx;
    struct graphic_object * obj                = SMM_GROUP_PROPERTIES(props->group)->tag[TAG_LOCAL];
    uint8_t bpp;

    LV_LOG_TRACE("create new buffer of width %d height %d", obj->width, obj->height);

    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);
    wl_buf =
        wl_shm_pool_create_buffer(wl_pool, props->offset, obj->width, obj->height, obj->width * bpp, app->shm_format);

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
