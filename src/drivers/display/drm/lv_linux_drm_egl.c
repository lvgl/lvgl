/**
 * @file lv_linux_drm_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_linux_drm.h"
#if LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL
#define CLOCK_USE_MONOTONIC 0
#include "lv_linux_drm_egl_private.h"
#include <stdlib.h>
#include "../../../core/lv_refr.h"
#include "../../../stdlib/lv_string.h"
#include "../../../core/lv_global.h"
#include "../../../display/lv_display_private.h"
#include "../../../indev/lv_indev.h"
#include "../../../lv_init.h"
#include "../../../misc/lv_area_private.h"
#include "../../opengles/egl_adapter/common/opengl_headers.h"
#include "../../opengles/lv_opengles_driver.h"
#include "../../opengles/lv_opengles_texture.h"

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
static void lv_drm_egl_quit(void);

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

static void display_resolution_change_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_drm_egl_t * window = lv_event_get_user_data(e);
    LV_ASSERT_NULL(window);
    LV_LOG("RESOLUTION CHANGE EVENT RECIEVED\n");
    //return;

    lv_display_rotation_t rotation = lv_display_get_rotation(disp);
    int32_t hor_res = lv_egl_adapter_interface_width(window->egl_adapter_interface);
    int32_t ver_res = lv_egl_adapter_interface_height(window->egl_adapter_interface);

    //LV_LOG("Removing previous egl_texture..\n");
    lv_drm_use_egl_texture_remove(window->window_texture);

    lv_display_remove_event_cb_with_user_data(window->display_texture, display_resolution_change_event_cb, window);

    //LV_LOG("Creating new window texture\n");
    /* add the texture to the window */
    window->window_texture = lv_drm_egl_add_texture(window,
                                                    lv_opengles_texture_get_texture_id(window->display_texture), ver_res, hor_res);

    //lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);
    switch(rotation) {
        case LV_DISPLAY_ROTATION_0:
            break;
        case LV_DISPLAY_ROTATION_90:
            break;
        case LV_DISPLAY_ROTATION_180:
            break;
        case LV_DISPLAY_ROTATION_270:
            break;
    }
    lv_display_set_resolution(disp, hor_res, ver_res);
    //lv_display_set_resolution(disp, ver_res, hor_res);
    lv_opengles_texture_reshape(disp, hor_res, ver_res);
    //lv_opengles_texture_reshape(disp, ver_res, hor_res);
    //lv_display_set_rotation(disp, rotation);

    //lv_display_set_resolution(disp, hor_res, ver_res);
    //lv_display_set_rotation(disp, rotation);
    lv_display_add_event_cb(window->display_texture, display_resolution_change_event_cb, LV_EVENT_RESOLUTION_CHANGED,
                            window);


#if 0
    LV_LOG("Deleting previous display..\n");
    //lv_display_remove_event_cb_with_user_data(window->display_texture, display_resolution_change_event_cb, window);
    lv_display_delete(window->display_texture);

    lv_display_t * new_display;
    switch(rotation) {
        case LV_DISPLAY_ROTATION_0:
            LV_LOG("Creating display with default rotation and size: %dx%d\n", hor_res, ver_res);
            new_display = lv_opengles_texture_create(hor_res, ver_res);
            break;
        case LV_DISPLAY_ROTATION_90:
            LV_LOG("Creating display with 90 rotation and size: %dx%d\n", ver_res, hor_res);
            new_display = lv_opengles_texture_create(ver_res, hor_res);
            break;
        case LV_DISPLAY_ROTATION_180:
            LV_LOG("Creating display with 180 rotation and size: %dx%d\n", hor_res, ver_res);
            new_display = lv_opengles_texture_create(hor_res, ver_res);
            break;
        case LV_DISPLAY_ROTATION_270:
            LV_LOG("Creating display with 270 rotation and size: %dx%d\n", ver_res, hor_res);
            new_display = lv_opengles_texture_create(ver_res, hor_res);
            break;
    }
    LV_LOG("Removing previous egl_texture..\n");
    lv_drm_use_egl_texture_remove(window->window_texture);
    LV_LOG("Setting new display..\n");
    window->display_texture = new_display;
    lv_display_set_default(window->display_texture);
    lv_display_set_rotation(window->display_texture, rotation);

    //LV_LOG("Clearing linked-list of egl textures...\n");
    //lv_ll_clear(&window->textures);

    LV_LOG("Replacing display desc with egl_adapter_interface...\n");
    lv_opengles_texture_t * display_desc = (lv_opengles_texture_t *)lv_display_get_driver_data(window->display_texture);
    window->egl_adapter_interface->display_texture_desc.texture_id = display_desc->texture_id;
    window->egl_adapter_interface->display_texture_desc.fb1 = display_desc->fb1;
    lv_display_set_driver_data(window->display_texture, window->egl_adapter_interface);

    LV_LOG("Creating new window texture\n");
    /* add the texture to the window */
    window->window_texture = lv_drm_egl_add_texture(window,
                                                    lv_opengles_texture_get_texture_id(window->display_texture), hor_res, ver_res);

    //  MK TEMP - and then re-register the display turn event
#endif
    LV_LOG("OUT\n");
}

lv_drm_egl_t * lv_drm_egl_create_ex(lv_display_t * display, bool use_mouse_indev, bool h_flip, bool v_flip)
{
    if(lv_drm_egl_init() != 0) {
        LV_LOG_ERROR("Failed to init lv_drm_egl_ output");
        return NULL;
    }

    lv_drm_egl_t * window = lv_ll_ins_tail(&lv_drm_egl_window_ll);
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

    lv_opengles_texture_init_display(display, hor_res, ver_res);
    lv_opengles_texture_reshape(display, hor_res, ver_res);
    window->display_texture = display;
    //window->display_texture = lv_opengles_texture_create(ver_res, hor_res);  // rotated test, 90 degrees
    lv_display_set_default(window->display_texture);

    lv_opengles_texture_t * display_desc = (lv_opengles_texture_t *)lv_display_get_driver_data(window->display_texture);
    window->egl_adapter_interface->display_texture_desc.texture_id = display_desc->texture_id;

    lv_display_set_driver_data(window->display_texture, window->egl_adapter_interface);

    /* add the texture to the window */
    window->window_texture = lv_drm_egl_add_texture(window,
                                                    lv_opengles_texture_get_texture_id(window->display_texture), hor_res, ver_res);


    /* register resolution changed event callback for display rotation */
    lv_display_add_event_cb(window->display_texture, display_resolution_change_event_cb, LV_EVENT_RESOLUTION_CHANGED,
                            window);

    return window;
}

lv_display_t * lv_drm_egl_get_display(lv_drm_egl_t * window, lv_display_t * placeholder_display)
{
    if(window == NULL) {
        window = lv_drm_egl_create(placeholder_display);
    }
    return (window) ? window->display_texture : NULL;
}

lv_drm_egl_t * lv_drm_egl_create(lv_display_t * placeholder_display)
{
    bool use_mouse_indev = false;
    return lv_drm_egl_create_ex(placeholder_display, use_mouse_indev, false, false);
}

void lv_drm_egl_delete(lv_drm_egl_t * window)
{
    // FREE window->display_texture
    // FREE window->window_texture

    lv_egl_adapter_interface_destroy((void **)&window->egl_adapter_interface);
    lv_ll_clear(&window->textures);
    lv_ll_remove(&lv_drm_egl_window_ll, window);
    lv_free(window);

    if(lv_ll_is_empty(&lv_drm_egl_window_ll)) {
        lv_drm_egl_quit();
    }
}

void lv_drm_egl_set_flip(lv_drm_egl_t * window, bool h_flip, bool v_flip)
{
    window->h_flip = h_flip;
    window->v_flip = v_flip;
}

lv_drm_use_egl_texture_t * lv_drm_egl_add_texture(lv_drm_egl_t * window,
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

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int lv_drm_egl_init(void)
{
    if(lv_drm_egl_inited) {
        return 0;
    }

    lv_ll_init(&lv_drm_egl_window_ll, sizeof(lv_drm_egl_t));
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

static void lv_drm_egl_quit(void)
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

    lv_drm_egl_t * window;

    /* delete windows that are ready to close */
    window = lv_ll_get_head(&lv_drm_egl_window_ll);
    while(window) {
        lv_drm_egl_t * window_to_delete = window->closing ? window : NULL;
        window = lv_ll_get_next(&lv_drm_egl_window_ll, window);
        if(window_to_delete) {
            lv_drm_egl_delete(window_to_delete);
        }
    }

    /* render each window */
    LV_LL_READ(&lv_drm_egl_window_ll, window) {
        //lv_opengles_viewport(0, 0, window->hor_res, window->ver_res);
        //lv_opengles_viewport(0, 0, 480-1, 800-1);
        //lv_opengles_viewport(0, 0, 800-1, 480-1);
        lv_opengles_viewport(0, 0, lv_display_get_physical_horizontal_resolution(window->display_texture),
                             lv_display_get_physical_vertical_resolution(window->display_texture));
        lv_egl_adapter_interface_clear();

        /* render each texture in the window */
        lv_drm_use_egl_texture_t * texture;
        LV_LL_READ(&window->textures, texture) {
            /* if the added texture is an LVGL opengles texture display, refresh it before rendering it */
            lv_display_t * texture_disp = lv_opengles_texture_get_from_texture_id(texture->texture_id);
            bool is_disp = false;
            if(texture_disp != NULL) {
                lv_refr_now(texture_disp);
                is_disp = true;
            }
            //lv_area_set(&texture->area, 0, 0,480 - 1, 800 - 1);
            //lv_area_set(&texture->area, 0, 0,800 - 1, 480 - 1);
            lv_area_t clip_area = texture->area;
            if(is_disp) {
#if LV_USE_DRAW_OPENGLES
                lv_opengles_render_display_texture(texture->texture_id, &texture->area, texture->opa,
                                                   &clip_area, window->h_flip, !window->v_flip);
#else
                lv_opengles_render_display_texture(texture->texture_id, &texture->area, texture->opa,
                                                   &clip_area, window->h_flip, window->v_flip);
#endif
            }
            else {
                /* It's never not the display texture so far in my tests - mk*/
                lv_opengles_render_texture(texture->texture_id, &texture->area, texture->opa, window->hor_res, window->ver_res,
                                           &clip_area, window->h_flip, window->v_flip);
                LV_LOG("*********** IT WASN'T DISP ***********\n");
            }

        }

        /* Swap front and back buffers */
        lv_egl_adapter_interface_update(window->egl_adapter_interface);
    }
}


lv_display_t * lv_linux_drm_create(void)
{
    //return lv_drm_egl_get_display(NULL);
    lv_display_t * placeholder = lv_display_create(64, 64);
#if LV_USE_DRAW_OPENGLES
    //lv_display_delete_refr_timer(placeholder);
#endif
    return placeholder;
}

void lv_linux_drm_set_file(lv_display_t * disp, const char * file, int64_t connector_id)
{
    LV_UNUSED(disp);
    LV_UNUSED(file);
    LV_UNUSED(connector_id);

    if((lv_egl_adapter_interface_t *)lv_display_get_user_data(disp) == NULL) {
        lv_drm_egl_get_display(NULL, disp);
        //lv_display_t * new_display = lv_drm_egl_get_display(NULL, disp);
    }
    //LV_LOG_INFO(file);
}

#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL*/
