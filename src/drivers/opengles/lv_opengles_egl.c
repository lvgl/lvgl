/**
 * @file lv_opengles_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_opengles_egl.h"
#if LV_USE_OPENGLES && LV_USE_OPENGLES_API == LV_OPENGLES_API_EGL

#include "lv_opengles_window.h"
#include "lv_opengles_driver.h"
#include "lv_opengles_texture.h"
#include "lv_opengles_private.h"

#include "../../core/lv_refr.h"
#include "../../stdlib/lv_string.h"
#include "../../core/lv_global.h"
#include "../../display/lv_display_private.h"
#include "../../indev/lv_indev.h"
#include "../../lv_init.h"
#include "../../misc/lv_area_private.h"

#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_opengles_window_t {
    EGLSurface surface;
    int32_t hor_res;
    int32_t ver_res;
    lv_ll_t textures;
    lv_opengles_egl_window_cb_t pre;
    lv_opengles_egl_window_cb_t post1;
    lv_opengles_egl_window_cb_t post2;
};

struct _lv_opengles_window_texture_t {
    lv_opengles_window_t * window;
    unsigned int texture_id;
    lv_area_t area;
    lv_opa_t opa;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t lv_egl_init(void);
static void lv_egl_timer_init(void);
static void window_update_handler(lv_timer_t * t);

/**********************
 *  STATIC VARIABLES
 **********************/

static bool egl_inited;
static lv_timer_t * update_handler_timer;
static lv_ll_t egl_window_ll;
static EGLDisplay egl_display;
static EGLContext egl_config;
static EGLContext egl_context;
static void * backend_device;

static EGLint const attribute_list[] = {
    EGL_RED_SIZE, 1,
    EGL_GREEN_SIZE, 1,
    EGL_BLUE_SIZE, 1,
    EGL_NONE
};

static EGLint const context_attribute_list[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_opengles_window_t * lv_opengles_egl_window_create(int32_t hor_res, int32_t ver_res, void * native_window_handle,
                                                     void * device,
                                                     lv_opengles_egl_window_cb_t pre,
                                                     lv_opengles_egl_window_cb_t post1,
                                                     lv_opengles_egl_window_cb_t post2){
    backend_device = device;
    if(lv_egl_init() == LV_RESULT_INVALID) {
        return NULL;
    }

    lv_opengles_window_t * window = lv_ll_ins_tail(&egl_window_ll);
    LV_ASSERT_MALLOC(window);
    if(window == NULL) return NULL;
    lv_memzero(window, sizeof(*window));

    /* Create window with graphics context */
    window->surface = eglCreateWindowSurface(egl_display, egl_config, (EGLNativeWindowType)(uintptr_t)native_window_handle,
                                             NULL);
    if(window->surface == EGL_NO_SURFACE) {
        LV_LOG_ERROR("eglCreateWindowSurface failed.");
        lv_ll_remove(&egl_window_ll, window);
        lv_free(window);
        return NULL;
    }

    window->hor_res = hor_res;
    window->ver_res = ver_res;
    lv_ll_init(&window->textures, sizeof(lv_opengles_window_texture_t));

    window->pre = pre;
    window->post1 = post1;
    window->post2 = post2;

    lv_egl_timer_init();
    eglMakeCurrent(egl_display, window->surface, window->surface, egl_context);
    lv_opengles_init();

    return window;
}

void * lv_opengles_egl_window_get_surface(lv_opengles_window_t * window)
{
    return (void *)(uintptr_t)window->surface;
}

void * lv_opengles_egl_window_get_display(lv_opengles_window_t * window)
{
    LV_UNUSED(window);
    return (void *)(uintptr_t)egl_display;
}

void lv_opengles_window_delete(lv_opengles_window_t * window)
{
    eglDestroySurface(egl_display, window->surface);
    lv_ll_clear(&window->textures);
    lv_ll_remove(&egl_window_ll, window);
    lv_free(window);
}

lv_opengles_window_texture_t * lv_opengles_window_add_texture(lv_opengles_window_t * window, unsigned int texture_id,
                                                              int32_t w, int32_t h)
{
    lv_opengles_window_texture_t * texture = lv_ll_ins_tail(&window->textures);
    LV_ASSERT_MALLOC(texture);
    if(texture == NULL) return NULL;
    lv_memzero(texture, sizeof(*texture));
    texture->window = window;
    texture->texture_id = texture_id;
    lv_area_set(&texture->area, 0, 0, w - 1, h - 1);
    texture->opa = LV_OPA_COVER;

    return texture;
}

void lv_opengles_window_texture_remove(lv_opengles_window_texture_t * texture)
{
    lv_ll_remove(&texture->window->textures, texture);
    lv_free(texture);
}

void lv_opengles_window_texture_set_x(lv_opengles_window_texture_t * texture, int32_t x)
{
    lv_area_set_pos(&texture->area, x, texture->area.y1);
}

void lv_opengles_window_texture_set_y(lv_opengles_window_texture_t * texture, int32_t y)
{
    lv_area_set_pos(&texture->area, texture->area.x1, y);
}

void lv_opengles_window_texture_set_opa(lv_opengles_window_texture_t * texture, lv_opa_t opa)
{
    texture->opa = opa;
}

lv_indev_t * lv_opengles_window_texture_get_mouse_indev(lv_opengles_window_texture_t * texture)
{
    LV_LOG_WARN("EGL does not create indevs. Returning NULL.");
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t lv_egl_init(void)
{
    if(egl_inited) {
        return LV_RESULT_OK;
    }

    /* get an EGL display connection */
    if (backend_device) {
        egl_display = eglGetDisplay(backend_device);
    } else {
        egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    }

    if(egl_display == EGL_NO_DISPLAY) {
        LV_LOG_ERROR("eglGetDisplay failed.");
        return LV_RESULT_INVALID;
    }

    /* initialize the EGL display connection */
    EGLBoolean res = eglInitialize(egl_display, NULL, NULL);
    if(res == EGL_FALSE) {
        LV_LOG_ERROR("eglInitialize failed.");
        return LV_RESULT_INVALID;
    }

    /* get an appropriate EGL frame buffer configuration */
    EGLint num_config;
    res = eglChooseConfig(egl_display, attribute_list, &egl_config, 1, &num_config);
    if(res == EGL_FALSE) {
        LV_LOG_ERROR("eglChooseConfig failed.");
        eglTerminate(egl_display);
        return LV_RESULT_INVALID;
    }

    /* create an EGL rendering context */
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attribute_list);
    if(egl_context == EGL_NO_CONTEXT) {
        LV_LOG_ERROR("eglCreateContext failed.");
        eglTerminate(egl_display);
        return LV_RESULT_INVALID;
    }

    lv_ll_init(&egl_window_ll, sizeof(lv_opengles_window_t));

    egl_inited = true;
    return LV_RESULT_OK;
}

static void lv_egl_timer_init(void)
{
    if(update_handler_timer == NULL) {
        update_handler_timer = lv_timer_create(window_update_handler, LV_DEF_REFR_PERIOD, NULL);
    }
}

static void window_update_handler(lv_timer_t * t)
{
    LV_UNUSED(t);
    int count = 0;
    lv_opengles_window_t * window;
    if(window->pre) window->pre(window);

    /* render each window */
    LV_LL_READ(&egl_window_ll, window) {

        eglMakeCurrent(egl_display, window->surface, window->surface, egl_context);
        lv_opengles_viewport(0, 0, window->hor_res, window->ver_res);
        lv_opengles_render_clear();

        /* render each texture in the window */
        lv_opengles_window_texture_t * texture;
        LV_LL_READ(&window->textures, texture) {
            /* if the added texture is an LVGL opengles texture display, refresh it before rendering it */
            lv_display_t * texture_disp = lv_opengles_texture_get_from_texture_id(texture->texture_id);
            if(texture_disp != NULL) {
                lv_refr_now(texture_disp);
            }

            lv_area_t clip_area = texture->area;
#if LV_USE_DRAW_OPENGLES
            lv_opengles_render_texture(texture->texture_id, &texture->area, texture->opa, window->hor_res, window->ver_res,
                                       &clip_area, texture_disp == NULL);
#else
            lv_opengles_render_texture(texture->texture_id, &texture->area, texture->opa, window->hor_res, window->ver_res,
                                       &clip_area, true);
#endif
        }

        if(window->post1) window->post1(window);

        /* Swap front and back buffers */
        eglSwapBuffers(egl_display, window->surface);

        if(window->post2) window->post2(window);

    }
}

#endif /*LV_USE_OPENGLES && LV_USE_OPENGLES_API == LV_OPENGLES_API_EGL*/
