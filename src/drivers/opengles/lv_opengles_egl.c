/**
 * @file lv_opengles_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_opengles_egl.h"
#if LV_USE_EGL

#include "lv_opengles_window.h"
#include "lv_opengles_driver.h"
#include "lv_opengles_texture.h"
#include "lv_opengles_private.h"
#include "lv_opengles_debug.h"

#include "../../core/lv_refr_private.h"
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
    void * user_data;
#if LV_USE_DRAW_OPENGLES
    uint8_t direct_render_invalidated: 1;
#endif
};

struct _lv_opengles_window_texture_t {
    lv_opengles_window_t * window;
    unsigned int texture_id; /* 0 if it's a window display */
    lv_display_t * disp; /* non-NULL if it's a display texture or a window display */
    uint8_t * fb; /* non-NULL if it's a window display and !DRAW_OPENGLES */
    lv_area_t area;
    lv_opa_t opa;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t lv_egl_init(void);
static void lv_egl_timer_init(void);
static void window_update_handler(lv_timer_t * t);
static void window_display_delete_cb(lv_event_t * e);
static void window_display_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
#if !LV_USE_DRAW_OPENGLES
    static void ensure_init_window_display_texture(void);
#endif

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
#if !LV_USE_DRAW_OPENGLES
    static unsigned int window_display_texture;
#endif

static EGLint const attribute_list[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
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
                                                     lv_opengles_egl_window_cb_t post2)
{
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

#if LV_USE_DRAW_OPENGLES
    window->direct_render_invalidated = 1;
#endif

    lv_egl_timer_init();

    EGLBoolean res = eglMakeCurrent(egl_display, window->surface, window->surface, egl_context);
    if(res == EGL_FALSE) {
        LV_LOG_ERROR("eglMakeCurrent failed.");
        return NULL;
    }

    res = eglSwapInterval(egl_display, 0);
    if(res == EGL_FALSE) {
        LV_LOG_ERROR("eglSwapInterval failed.");
        return NULL;
    }

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

void lv_opengles_egl_window_set_user_data(lv_opengles_window_t * window, void * user_data)
{
    window->user_data = user_data;
}

void * lv_opengles_egl_window_get_user_data(lv_opengles_window_t * window)
{
    return window->user_data;
}

void lv_opengles_window_delete(lv_opengles_window_t * window)
{
    EGLBoolean res = eglDestroySurface(egl_display, window->surface);
    if(res == EGL_FALSE) {
        LV_LOG_ERROR("eglDestroySurface failed.");
        return;
    }

    lv_opengles_window_texture_t * texture;
    while((texture = lv_ll_get_head(&window->textures))) {
        if(texture->texture_id) {
            lv_opengles_window_texture_remove(texture);
        }
        else {
            lv_display_delete(texture->disp);
        }
    }

    lv_ll_remove(&egl_window_ll, window);
    lv_free(window);

#if !LV_USE_DRAW_OPENGLES
    if(lv_ll_is_empty(&egl_window_ll)) {
        GL_CALL(glDeleteTextures(1, &window_display_texture));
        window_display_texture = 0;
    }
#endif
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
    texture->disp = lv_opengles_texture_get_from_texture_id(texture_id);
    lv_area_set(&texture->area, 0, 0, w - 1, h - 1);
    texture->opa = LV_OPA_COVER;

#if LV_USE_DRAW_OPENGLES
    window->direct_render_invalidated = 1;
#endif

    return texture;
}

void lv_opengles_window_texture_remove(lv_opengles_window_texture_t * texture)
{
    if(texture->texture_id == 0) {
        LV_LOG_WARN("window displays should be deleted with `lv_display_delete`");
        return;
    }

#if LV_USE_DRAW_OPENGLES
    texture->window->direct_render_invalidated = 1;
#endif

    lv_ll_remove(&texture->window->textures, texture);
    lv_free(texture);
}

lv_display_t * lv_opengles_window_display_create(lv_opengles_window_t * window, int32_t w, int32_t h)
{
    lv_display_t * disp = lv_display_create(w, h);
    if(disp == NULL) {
        return NULL;
    }

    lv_opengles_window_texture_t * dsc = lv_ll_ins_tail(&window->textures);
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) {
        lv_display_delete(disp);
        return NULL;
    }
    lv_memzero(dsc, sizeof(*dsc));
    dsc->window = window;
    dsc->disp = disp;
    lv_area_set(&dsc->area, 0, 0, w - 1, h - 1);
    dsc->opa = LV_OPA_COVER;

#if LV_USE_DRAW_OPENGLES
    static size_t LV_ATTRIBUTE_MEM_ALIGN dummy_buf;
    lv_display_set_buffers(disp, &dummy_buf, NULL, h * lv_draw_buf_width_to_stride(w, LV_COLOR_FORMAT_ARGB8888),
                           LV_DISPLAY_RENDER_MODE_FULL);
#else
    uint32_t stride = lv_draw_buf_width_to_stride(w, lv_display_get_color_format(disp));
    uint32_t buf_size = stride * h;
    dsc->fb = malloc(buf_size);
    LV_ASSERT_MALLOC(dsc->fb);
    if(dsc->fb == NULL) {
        lv_display_delete(disp);
        lv_ll_remove(&window->textures, dsc);
        lv_free(dsc);
        return NULL;
    }
    lv_display_set_buffers(disp, dsc->fb, NULL, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);
#endif

    lv_display_set_driver_data(disp, dsc);
    lv_display_delete_refr_timer(disp);
    lv_display_set_flush_cb(disp, window_display_flush_cb);
    lv_display_add_event_cb(disp, window_display_delete_cb, LV_EVENT_DELETE, disp);

#if LV_USE_DRAW_OPENGLES
    window->direct_render_invalidated = 1;
#endif

    return disp;
}

lv_opengles_window_texture_t * lv_opengles_window_display_get_window_texture(lv_display_t * window_display)
{
    return lv_display_get_driver_data(window_display);
}

void lv_opengles_window_texture_set_x(lv_opengles_window_texture_t * texture, int32_t x)
{
    lv_area_set_pos(&texture->area, x, texture->area.y1);

#if LV_USE_DRAW_OPENGLES
    texture->window->direct_render_invalidated = 1;
#endif
}

void lv_opengles_window_texture_set_y(lv_opengles_window_texture_t * texture, int32_t y)
{
    lv_area_set_pos(&texture->area, texture->area.x1, y);

#if LV_USE_DRAW_OPENGLES
    texture->window->direct_render_invalidated = 1;
#endif
}

void lv_opengles_window_texture_set_opa(lv_opengles_window_texture_t * texture, lv_opa_t opa)
{
    texture->opa = opa;

#if LV_USE_DRAW_OPENGLES
    texture->window->direct_render_invalidated = 1;
#endif
}

lv_indev_t * lv_opengles_window_texture_get_mouse_indev(lv_opengles_window_texture_t * texture)
{
    LV_UNUSED(texture);
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
    if(backend_device) {
        egl_display = eglGetDisplay(backend_device);
    }
    else {
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
    lv_opengles_window_t * window;

    /* render each window */
    LV_LL_READ(&egl_window_ll, window) {
        if(window->pre) window->pre(window);

        EGLBoolean res = eglMakeCurrent(egl_display, window->surface, window->surface, egl_context);
        if(res == EGL_FALSE) {
            LV_LOG_ERROR("eglMakeCurrent failed.");
            return;
        }

        lv_opengles_viewport(0, 0, window->hor_res, window->ver_res);

#if LV_USE_DRAW_OPENGLES
        lv_opengles_window_texture_t * textures_head;
        bool window_display_direct_render =
            !window->direct_render_invalidated
            && (textures_head = lv_ll_get_head(&window->textures))
            && textures_head->texture_id == 0 /* it's a window display */
            && lv_ll_get_next(&window->textures, textures_head) == NULL /* it's the only one */
            && textures_head->opa == LV_OPA_COVER
            && textures_head->area.x1 == 0
            && textures_head->area.y1 == 0
            && textures_head->area.x2 == window->hor_res - 1
            && textures_head->area.y2 == window->ver_res - 1
            ;
        window->direct_render_invalidated = 0;
        if(!window_display_direct_render) {
            lv_opengles_render_clear();
        }
#else
        lv_opengles_render_clear();
#endif

        /* render each texture in the window */
        lv_opengles_window_texture_t * texture;
        LV_LL_READ(&window->textures, texture) {
            if(texture->texture_id == 0) { /* it's a window display */
#if LV_USE_DRAW_OPENGLES
                lv_display_set_render_mode(texture->disp,
                                           window_display_direct_render ? LV_DISPLAY_RENDER_MODE_DIRECT : LV_DISPLAY_RENDER_MODE_FULL);
#endif

                lv_display_t * default_save = lv_display_get_default();
                lv_display_set_default(texture->disp);
                lv_display_refr_timer(NULL);
                lv_display_set_default(default_save);

#if !LV_USE_DRAW_OPENGLES
                ensure_init_window_display_texture();

                GL_CALL(glBindTexture(GL_TEXTURE_2D, window_display_texture));

                /* set the dimensions and format to complete the texture */
                /* Color depth: 8 (L8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888) */
#if LV_COLOR_DEPTH == 8
                GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, lv_area_get_width(&texture->area), lv_area_get_height(&texture->area), 0,
                                     GL_RED, GL_UNSIGNED_BYTE, texture->fb));
#elif LV_COLOR_DEPTH == 16
                GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, lv_area_get_width(&texture->area), lv_area_get_height(&texture->area),
                                     0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
                                     texture->fb));
#elif LV_COLOR_DEPTH == 24
                GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, lv_area_get_width(&texture->area), lv_area_get_height(&texture->area), 0,
                                     GL_BGR, GL_UNSIGNED_BYTE, texture->fb));
#elif LV_COLOR_DEPTH == 32
                GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lv_area_get_width(&texture->area), lv_area_get_height(&texture->area),
                                     0, GL_BGRA, GL_UNSIGNED_BYTE, texture->fb));
#else
#error("Unsupported color format")
#endif

                GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

                GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

                lv_opengles_render_texture(window_display_texture, &texture->area, texture->opa, window->hor_res, window->ver_res,
                                           &texture->area, false, false);
#endif
            }
            else {
                /* if the added texture is an LVGL opengles texture display, refresh it before rendering it */
                if(texture->disp != NULL) {
#if LV_USE_DRAW_OPENGLES
                    lv_display_t * default_save = lv_display_get_default();
                    lv_display_set_default(texture->disp);
                    lv_display_refr_timer(NULL);
                    lv_display_set_default(default_save);
#else
                    lv_refr_now(texture->disp);
#endif
                }

#if LV_USE_DRAW_OPENGLES
                lv_opengles_render_texture(texture->texture_id, &texture->area, texture->opa, window->hor_res, window->ver_res,
                                           &texture->area, false, texture->disp != NULL);
#else
                lv_opengles_render_texture(texture->texture_id, &texture->area, texture->opa, window->hor_res, window->ver_res,
                                           &texture->area, false, false);
#endif
            }
        }

        if(window->post1) window->post1(window);

        /* Swap front and back buffers */
        res = eglSwapBuffers(egl_display, window->surface);
        if(res == EGL_FALSE) {
            LV_LOG_ERROR("eglSwapBuffers failed.");
            return;
        }

        if(window->post2) window->post2(window);
    }
}

static void window_display_delete_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_opengles_window_texture_t * dsc = lv_display_get_driver_data(disp);
    free(dsc->fb);
    lv_opengles_window_texture_remove(dsc);
}

static void window_display_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);
    lv_display_flush_ready(disp);
}

#if !LV_USE_DRAW_OPENGLES
static void ensure_init_window_display_texture(void)
{
    if(window_display_texture) {
        return;
    }

    GL_CALL(glGenTextures(1, &window_display_texture));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, window_display_texture));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 20));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}
#endif

#endif /*LV_USE_EGL*/
