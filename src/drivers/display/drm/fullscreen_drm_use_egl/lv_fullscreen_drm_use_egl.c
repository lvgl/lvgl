/**
 * @file lv_fullscreen_drm_use_egl.c
 *
 */

/* CREDIT: This file is a chopped down version of the
 * lv_glfw_window.c/h files from LVGL (in src/drivers/glfw),
 * with minimal changes to test out the lv_egl_adapter
 * module.
 */

/*********************
 *      DEFINES
 *********************/
#define CLOCK_USE_MONOTONIC 0

/*********************
 *      INCLUDES
 *********************/
#include "lv_fullscreen_drm_use_egl_private.h"
#if LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL
#include <stdlib.h>
#include "../../../../core/lv_refr.h"
#include "../../../../stdlib/lv_string.h"
#include "../../../../core/lv_global.h"
#include "../../../../display/lv_display_private.h"
#include "../../../../indev/lv_indev.h"
#include "../../../../lv_init.h"
#include "../../../../misc/lv_area_private.h"
#include "../../../opengles/egl_adapter/common/opengl_headers.h"
#include "../../../opengles/lv_opengles_driver.h"
#include "../../../opengles/lv_opengles_texture.h"

#if CLOCK_USE_MONOTONIC
    #include <time.h>
#else
    #include <sys/time.h>
#endif /* CLOCK_USE_MONOTONIC */
#include <stdint.h>

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void window_update_handler(lv_timer_t * t);
static uint32_t lv_drm_egl_tick_count_callback(void);
static int lv_drm_egl_init(void);
static void lv_drm_egl_timer_init(void);
static void lv_fullscreen_drm_use_egl_quit(void);
// static void lv_drm_egl_error_cb(int error, const char * description);
// static void indev_read_cb(lv_indev_t * indev, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool lv_drm_egl_inited;
static lv_timer_t * update_handler_timer;
static lv_ll_t lv_drm_egl_window_ll;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#define CLOCK_USE_MONOTONIC 0
#if CLOCK_USE_MONOTONIC
static uint32_t lv_drm_egl_tick_count_callback(void)
{
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    uint64_t ms = (uint64_t)ts.tv_sec * 1000ull + (uint64_t)ts.tv_nsec / 1000000ull;
    return (uint32_t)ms;
}
#else
/*
 * fallback for systems without clock_gettime (older libc), using gettimeofday:
 */
static uint32_t lv_drm_egl_tick_count_callback(void)
{
    struct timeval tv;
    if(gettimeofday(&tv, NULL) != 0) return 0;
    uint64_t ms = (uint64_t)tv.tv_sec * 1000ull + (uint64_t)tv.tv_usec / 1000ull;
    return (uint32_t)ms;
}
#endif

lv_fullscreen_drm_use_egl_t * lv_fullscreen_drm_use_egl_create_ex(bool use_mouse_indev, bool h_flip, bool v_flip)
{
    if(lv_drm_egl_init() != 0) {
        LV_LOG_ERROR("Failed to init lv_drm_egl_ output");
        return NULL;
    }

    lv_fullscreen_drm_use_egl_t * window = lv_ll_ins_tail(&lv_drm_egl_window_ll);
    LV_ASSERT_MALLOC(window);
    if(window == NULL) {
        LV_LOG_ERROR("Failed to create lv_drm_egl_ output");
        return NULL;
    }
    lv_memzero(window, sizeof(*window));


    window->egl_adapter_interface = lv_egl_adapter_interface_auto();
    int32_t hor_res = lv_egl_adapter_interface_width(window->egl_adapter_interface);
    int32_t ver_res = lv_egl_adapter_interface_height(window->egl_adapter_interface);
    if(window->egl_adapter_interface == NULL) {
        LV_LOG_ERROR("lv_drm_egl_ output CreateWindow fail");
        lv_ll_remove(&lv_drm_egl_window_ll, window);
        lv_free(window);
        return NULL;
    }

    window->h_flip = h_flip;
    window->v_flip = v_flip;
    window->hor_res = hor_res;
    window->ver_res = ver_res;

    lv_ll_init(&window->textures, sizeof(lv_drm_use_egl_texture_t));
    lv_drm_egl_timer_init();
    lv_opengles_init();

    window->display_texture = lv_opengles_texture_create(hor_res, ver_res);
    lv_display_set_default(window->display_texture);
    /* add the texture to the window */
    window->window_texture = lv_fullscreen_drm_use_egl_add_texture(window,
                                                                   lv_opengles_texture_get_texture_id(window->display_texture), hor_res, ver_res);

    return window;
}

lv_display_t * lv_fullscreen_drm_use_egl_get_display(lv_fullscreen_drm_use_egl_t * window)
{
    if(window == NULL) {
        window = lv_fullscreen_drm_use_egl_create();
    }
    return (window) ? window->display_texture : NULL;
}

lv_fullscreen_drm_use_egl_t * lv_fullscreen_drm_use_egl_create(void)
{
    bool use_mouse_indev = false;
    return lv_fullscreen_drm_use_egl_create_ex(use_mouse_indev, false, false);
}

void lv_fullscreen_drm_use_egl_delete(lv_fullscreen_drm_use_egl_t * window)
{
    // FREE window->display_texture
    // FREE window->window_texture

    lv_egl_adapter_interface_destroy((void **)&window->egl_adapter_interface);
    lv_ll_clear(&window->textures);
    lv_ll_remove(&lv_drm_egl_window_ll, window);
    lv_free(window);

    if(lv_ll_is_empty(&lv_drm_egl_window_ll)) {
        lv_fullscreen_drm_use_egl_quit();
    }
}

void * lv_fullscreen_drm_use_egl_get_drm_window(lv_fullscreen_drm_use_egl_t * window)
{
    return (void *)(window->egl_adapter_interface);
}

void lv_fullscreen_drm_use_egl_set_flip(lv_fullscreen_drm_use_egl_t * window, bool h_flip, bool v_flip)
{
    window->h_flip = h_flip;
    window->v_flip = v_flip;
}

lv_drm_use_egl_texture_t * lv_fullscreen_drm_use_egl_add_texture(lv_fullscreen_drm_use_egl_t * window,
                                                                 unsigned int texture_id,
                                                                 int32_t w,
                                                                 int32_t h)
{
    lv_drm_use_egl_texture_t * texture = lv_ll_ins_tail(&window->textures);
    LV_ASSERT_MALLOC(texture);
    if(texture == NULL) return NULL;
    lv_memzero(texture, sizeof(*texture));
    texture->window = window;
    texture->texture_id = texture_id;
    lv_area_set(&texture->area, 0, 0, w - 1, h - 1);
    texture->opa = LV_OPA_COVER;
    return texture;
}

void lv_drm_use_egl_texture_remove(lv_drm_use_egl_texture_t * texture)
{
    if(texture->indev != NULL) {
        lv_indev_delete(texture->indev);
    }
    lv_ll_remove(&texture->window->textures, texture);
    lv_free(texture);
}

void lv_drm_use_egl_texture_set_x(lv_drm_use_egl_texture_t * texture, int32_t x)
{
    lv_area_set_pos(&texture->area, x, texture->area.y1);
}

void lv_drm_use_egl_texture_set_y(lv_drm_use_egl_texture_t * texture, int32_t y)
{
    lv_area_set_pos(&texture->area, texture->area.x1, y);
}

void lv_drm_use_egl_texture_set_opa(lv_drm_use_egl_texture_t * texture, lv_opa_t opa)
{
    texture->opa = opa;
}

lv_indev_t * lv_drm_use_egl_texture_get_mouse_indev(lv_drm_use_egl_texture_t * texture)
{
    return texture->indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int lv_drm_egl_init(void)
{
    if(lv_drm_egl_inited) {
        return 0;
    }

    lv_ll_init(&lv_drm_egl_window_ll, sizeof(lv_fullscreen_drm_use_egl_t));
    lv_drm_egl_inited = true;
    return 0;
}

static void lv_drm_egl_timer_init(void)
{
    if(update_handler_timer == NULL) {
        update_handler_timer = lv_timer_create(window_update_handler, LV_DEF_REFR_PERIOD, NULL);
        lv_tick_set_cb(lv_drm_egl_tick_count_callback);
    }
}

static void lv_fullscreen_drm_use_egl_quit(void)
{
    lv_timer_delete(update_handler_timer);
    update_handler_timer = NULL;
    lv_drm_egl_inited = false;
    lv_deinit();
    exit(0);
}

static void window_update_handler(lv_timer_t * t)
{
    LV_UNUSED(t);

    lv_fullscreen_drm_use_egl_t * window;

    /* delete windows that are ready to close */
    window = lv_ll_get_head(&lv_drm_egl_window_ll);
    while(window) {
        lv_fullscreen_drm_use_egl_t * window_to_delete = window->closing ? window : NULL;
        window = lv_ll_get_next(&lv_drm_egl_window_ll, window);
        if(window_to_delete) {
            lv_fullscreen_drm_use_egl_delete(window_to_delete);
        }
    }

    /* render each window */
    LV_LL_READ(&lv_drm_egl_window_ll, window) {
        lv_opengles_viewport(0, 0, window->hor_res, window->ver_res);
        lv_egl_adapter_interface_clear();

        /* render each texture in the window */
        lv_drm_use_egl_texture_t * texture;
        LV_LL_READ(&window->textures, texture) {
            /* if the added texture is an LVGL opengles texture display, refresh it before rendering it */
            lv_display_t * texture_disp = lv_opengles_texture_get_from_texture_id(texture->texture_id);
            if(texture_disp != NULL) {
                lv_refr_now(texture_disp);
            }
            lv_area_t clip_area = texture->area;
#if LV_USE_DRAW_OPENGLES
            lv_opengles_render_texture(texture->texture_id, &texture->area, texture->opa, window->hor_res, window->ver_res,
                                       &clip_area, window->h_flip, texture_disp == NULL ? window->v_flip : !window->v_flip);
#else
            lv_opengles_render_texture(texture->texture_id, &texture->area, texture->opa, window->hor_res, window->ver_res,
                                       &clip_area, window->h_flip, window->v_flip);
#endif
        }

        /* Swap front and back buffers */
        lv_egl_adapter_interface_update(window->egl_adapter_interface);
    }
}
/*
static void lv_drm_egl_error_cb(int error, const char * description)
{
    LV_LOG_ERROR("MK DRM Error %d: %s", error, description);
}

static void indev_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_drm_use_egl_texture_t * texture = lv_indev_get_driver_data(indev);
    data->point = texture->indev_last_point;
    data->state = texture->indev_last_state;
}
*/
#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL*/
