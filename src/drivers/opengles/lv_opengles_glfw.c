/**
 * @file lv_opengles_glfw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_opengles_glfw.h"
#if LV_USE_GLFW

#include "lv_opengles_window.h"
#include "lv_opengles_driver.h"
#include "lv_opengles_texture.h"
#include "lv_opengles_private.h"
#include "lv_opengles_debug.h"

#include "../../core/lv_refr.h"
#include "../../stdlib/lv_sprintf.h"
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
    GLFWwindow * window;
    int32_t hor_res;
    int32_t ver_res;
    bool h_flip;
    bool v_flip;
    lv_ll_t textures;
    lv_point_t mouse_last_point;
    lv_indev_state_t mouse_last_state;
    uint8_t use_indev : 1;
    uint8_t closing : 1;
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
    lv_indev_t * indev;
    lv_point_t indev_last_point;
    lv_indev_state_t indev_last_state;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void window_update_handler(lv_timer_t * t);
static uint32_t lv_glfw_tick_count_callback(void);
static lv_opengles_window_t * lv_glfw_get_lv_window_from_window(GLFWwindow * window);
static void glfw_error_cb(int error, const char * description);
static int lv_glfw_init(void);
static int lv_glew_init(void);
static void lv_glfw_timer_init(void);
static void lv_glfw_window_config(GLFWwindow * window, bool use_mouse_indev);
static void lv_glfw_window_quit(void);
static void window_close_callback(GLFWwindow * window);
static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);
static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods);
static void mouse_move_callback(GLFWwindow * window, double xpos, double ypos);
static void proc_mouse(lv_opengles_window_t * window);
static void indev_read_cb(lv_indev_t * indev, lv_indev_data_t * data);
static void framebuffer_size_callback(GLFWwindow * window, int width, int height);
static void window_display_delete_cb(lv_event_t * e);
static void window_display_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
#if !LV_USE_DRAW_OPENGLES
    static void ensure_init_window_display_texture(void);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static bool glfw_inited;
static bool glew_inited;
static lv_timer_t * update_handler_timer;
static lv_ll_t glfw_window_ll;
#if !LV_USE_DRAW_OPENGLES
    static unsigned int window_display_texture;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_opengles_window_t * lv_opengles_glfw_window_create_ex(int32_t hor_res, int32_t ver_res, bool use_mouse_indev,
                                                         bool h_flip, bool v_flip,  const char * title)
{
    LV_ASSERT_NULL(title);
    if(lv_glfw_init() != 0) {
        LV_LOG_ERROR("Failed to init glfw");
        return NULL;
    }

    lv_opengles_window_t * window = lv_ll_ins_tail(&glfw_window_ll);
    LV_ASSERT_MALLOC(window);
    if(window == NULL) return NULL;
    if(window == NULL) {
        LV_LOG_ERROR("Failed to create glfw window");
        return NULL;
    }
    lv_memzero(window, sizeof(*window));

    /* Create window with graphics context */
    lv_opengles_window_t * existing_window = lv_ll_get_head(&glfw_window_ll);
    window->window = glfwCreateWindow(hor_res, ver_res, title, NULL,
                                      existing_window ? existing_window->window : NULL);
    if(window->window == NULL) {
        LV_LOG_ERROR("glfwCreateWindow fail");
        lv_ll_remove(&glfw_window_ll, window);
        lv_free(window);
        return NULL;
    }

    glfwSetWindowTitle(window->window, title);

    window->h_flip = h_flip;
    window->v_flip = v_flip;
    window->hor_res = hor_res;
    window->ver_res = ver_res;

    lv_ll_init(&window->textures, sizeof(lv_opengles_window_texture_t));
    window->use_indev = use_mouse_indev;
#if LV_USE_DRAW_OPENGLES
    window->direct_render_invalidated = 1;
#endif

    glfwSetWindowUserPointer(window->window, window);
    lv_glfw_timer_init();
    lv_glfw_window_config(window->window, use_mouse_indev);
    lv_glew_init();
    glfwMakeContextCurrent(window->window);
    lv_opengles_init();

    return window;
}

lv_opengles_window_t * lv_opengles_glfw_window_create(int32_t hor_res, int32_t ver_res, bool use_mouse_indev)
{
    return lv_opengles_glfw_window_create_ex(hor_res, ver_res, use_mouse_indev, false, false, "LVGL Simulator");
}

void lv_opengles_glfw_window_set_title(lv_opengles_window_t * window, const char * new_title)
{
    glfwSetWindowTitle(window->window, new_title);
}

void lv_opengles_window_delete(lv_opengles_window_t * window)
{
    glfwDestroyWindow(window->window);

    lv_opengles_window_texture_t * texture;
    while((texture = lv_ll_get_head(&window->textures))) {
        if(texture->texture_id) {
            lv_opengles_window_texture_remove(texture);
        }
        else {
            lv_display_delete(texture->disp);
        }
    }

    lv_ll_remove(&glfw_window_ll, window);
    lv_free(window);

    if(lv_ll_is_empty(&glfw_window_ll)) {
        lv_glfw_window_quit();
#if !LV_USE_DRAW_OPENGLES
        if(window_display_texture) {
            GL_CALL(glDeleteTextures(1, &window_display_texture));
            window_display_texture = 0;
        }
#endif
    }
}

void * lv_opengles_glfw_window_get_glfw_window(lv_opengles_window_t * window)
{
    return (void *)(window->window);
}

void lv_opengles_glfw_window_set_flip(lv_opengles_window_t * window, bool h_flip, bool v_flip)
{
    window->h_flip = h_flip;
    window->v_flip = v_flip;
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

    if(window->use_indev && texture->disp) {
        lv_indev_t * indev = lv_indev_create();
        if(indev == NULL) {
            lv_ll_remove(&window->textures, texture);
            lv_free(texture);
            return NULL;
        }
        texture->indev = indev;
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, indev_read_cb);
        lv_indev_set_driver_data(indev, texture);
        lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);
        lv_indev_set_display(indev, texture->disp);
    }

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
    if(texture->indev != NULL) {
        lv_indev_delete(texture->indev);
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

    if(window->use_indev) {
        lv_indev_t * indev = lv_indev_create();
        if(indev == NULL) {
            lv_display_delete(disp);
            lv_ll_remove(&window->textures, dsc);
            lv_free(dsc);
            return NULL;
        }
        dsc->indev = indev;
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, indev_read_cb);
        lv_indev_set_driver_data(indev, dsc);
        lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);
        lv_indev_set_display(indev, disp);
    }

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
    return texture->indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int lv_glfw_init(void)
{
    if(glfw_inited) {
        return 0;
    }

    glfwSetErrorCallback(glfw_error_cb);

    int ret = glfwInit();
    if(ret == 0) {
        LV_LOG_ERROR("glfwInit fail.");
        return 1;
    }

    lv_ll_init(&glfw_window_ll, sizeof(lv_opengles_window_t));

    glfw_inited = true;
    return 0;
}

static int lv_glew_init(void)
{
    if(glew_inited) {
        return 0;
    }

    GLenum ret = glewInit();
    if(ret != GLEW_OK) {
        LV_LOG_ERROR("glewInit fail: %d.", ret);
        return ret;
    }

    LV_LOG_INFO("GL version: %s", glGetString(GL_VERSION));
    LV_LOG_INFO("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glew_inited = true;

    return 0;
}

static void lv_glfw_timer_init(void)
{
    if(update_handler_timer == NULL) {
        update_handler_timer = lv_timer_create(window_update_handler, LV_DEF_REFR_PERIOD, NULL);
        lv_tick_set_cb(lv_glfw_tick_count_callback);
    }
}

static void lv_glfw_window_config(GLFWwindow * window, bool use_mouse_indev)
{
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(use_mouse_indev) {
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, mouse_move_callback);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwSetWindowCloseCallback(window, window_close_callback);
}

static void lv_glfw_window_quit(void)
{
    lv_timer_delete(update_handler_timer);
    update_handler_timer = NULL;

    glfwTerminate();
    glfw_inited = false;

    lv_deinit();

    exit(0);
}

static void window_update_handler(lv_timer_t * t)
{
    LV_UNUSED(t);

    lv_opengles_window_t * window;

    glfwPollEvents();

    /* delete windows that are ready to close */
    window = lv_ll_get_head(&glfw_window_ll);
    while(window) {
        lv_opengles_window_t * window_to_delete = window->closing ? window : NULL;
        window = lv_ll_get_next(&glfw_window_ll, window);
        if(window_to_delete) {
            glfwSetWindowShouldClose(window_to_delete->window, GLFW_TRUE);
            lv_opengles_window_delete(window_to_delete);
        }
    }

    /* render each window */
    LV_LL_READ(&glfw_window_ll, window) {
        glfwMakeContextCurrent(window->window);
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
                                           &texture->area, window->h_flip, window->v_flip);
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
                                           &texture->area, window->h_flip, texture->disp == NULL ? window->v_flip : !window->v_flip);
#else
                lv_opengles_render_texture(texture->texture_id, &texture->area, texture->opa, window->hor_res, window->ver_res,
                                           &texture->area, window->h_flip, window->v_flip);
#endif
            }
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window->window);
    }
}

static void glfw_error_cb(int error, const char * description)
{
    LV_LOG_ERROR("GLFW Error %d: %s", error, description);
}

static lv_opengles_window_t * lv_glfw_get_lv_window_from_window(GLFWwindow * window)
{
    return glfwGetWindowUserPointer(window);
}

static void window_close_callback(GLFWwindow * window)
{
    lv_opengles_window_t * lv_window = lv_glfw_get_lv_window_from_window(window);
    lv_window->closing = 1;
}

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    LV_UNUSED(scancode);
    LV_UNUSED(mods);
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        lv_opengles_window_t * lv_window = lv_glfw_get_lv_window_from_window(window);
        lv_window->closing = 1;
    }
}

static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
    LV_UNUSED(mods);
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        lv_opengles_window_t * lv_window = lv_glfw_get_lv_window_from_window(window);
        lv_window->mouse_last_state = action == GLFW_PRESS ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
        proc_mouse(lv_window);
    }
}

static void mouse_move_callback(GLFWwindow * window, double xpos, double ypos)
{
    lv_opengles_window_t * lv_window = lv_glfw_get_lv_window_from_window(window);
    lv_window->mouse_last_point.x = (int32_t)xpos;
    lv_window->mouse_last_point.y = (int32_t)ypos;
    proc_mouse(lv_window);
}

static void proc_mouse(lv_opengles_window_t * window)
{
    /* mouse activity will affect the topmost LVGL display texture */
    lv_opengles_window_texture_t * texture;
    LV_LL_READ_BACK(&window->textures, texture) {
        if(lv_area_is_point_on(&texture->area, &window->mouse_last_point, 0)) {
            /* adjust the mouse pointer coordinates so that they are relative to the texture */
            if(window->h_flip) {
                texture->indev_last_point.x = texture->area.x2 - window->mouse_last_point.x;
            }
            else {
                texture->indev_last_point.x  = window->mouse_last_point.x - texture->area.x1;
            }
            if(window->v_flip) {
                texture->indev_last_point.y = (texture->area.y2 - window->mouse_last_point.y);
            }
            else {
                texture->indev_last_point.y = (window->mouse_last_point.y - texture->area.y1);
            }
            texture->indev_last_state = window->mouse_last_state;
            lv_indev_read(texture->indev);
            break;
        }
    }
}

static void indev_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_opengles_window_texture_t * texture = lv_indev_get_driver_data(indev);
    data->point = texture->indev_last_point;
    data->state = texture->indev_last_state;
}

static void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    lv_opengles_window_t * lv_window = lv_glfw_get_lv_window_from_window(window);
    lv_window->hor_res = width;
    lv_window->ver_res = height;
}

static uint32_t lv_glfw_tick_count_callback(void)
{
    double tick = glfwGetTime() * 1000.0;
    return (uint32_t)tick;
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

#endif /*LV_USE_GLFW*/
