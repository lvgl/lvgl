/**
 * @file lv_opengles_window.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_glfw_window.h"
#include "lv_glfw_mouse_private.h"
#if LV_USE_OPENGLES
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include "../../core/lv_refr.h"
#include "../../stdlib/lv_string.h"
#include "../../core/lv_global.h"
#include "../../display/lv_display_private.h"
#include "../../lv_init.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lv_opengles_driver.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    GLFWwindow * window;

    uint8_t * fb1;
    uint8_t * fb2;
    uint8_t * fb_act;
    uint8_t * buf1;
    uint8_t * buf2;
    uint8_t * rotated_buf;
    size_t rotated_buf_size;

    uint8_t zoom;
    uint8_t ignore_size_chg;
} lv_glfw_window_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static int window_create(lv_display_t * disp);
static void window_update(lv_display_t * disp);
static void texture_resize(lv_display_t * disp);
static void window_update_handler(lv_timer_t * t);
static void window_event_handler(lv_timer_t * t);
static void release_disp_cb(lv_event_t * e);
static void res_chg_event_cb(lv_event_t * e);
static uint32_t lv_glfw_tick_count_callback(void);
static lv_display_t * _lv_glfw_get_disp_from_window(GLFWwindow * window);
static void glfw_error_cb(int error, const char * description);
static int lv_glfw_init(void);
static int lv_glew_init(void);
static void lv_glfw_window_config(GLFWwindow * window);
static void window_close_callback(GLFWwindow * window);
static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);
static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods);
static void mouse_move_cllback(GLFWwindow * window, double xpos, double ypos);
static void framebuffer_size_callback(GLFWwindow * window, int width, int height);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static bool inited = false;
static GLFWwindow * window_closed = NULL;
static lv_timer_t * update_handler_timer;
static lv_timer_t * event_handler_timer;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static void lv_glfw_timer_init()
{
    if(!inited) {
        update_handler_timer = lv_timer_create(window_update_handler, 5, NULL);
        event_handler_timer = lv_timer_create(window_event_handler, 5, NULL);

        lv_tick_set_cb(lv_glfw_tick_count_callback);

        inited = true;
    }
}

static void lv_glfw_window_config(GLFWwindow * window)
{
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_move_cllback);

    glfwSetKeyCallback(window, key_callback);

    glfwSetWindowCloseCallback(window, window_close_callback);
}

lv_display_t * lv_glfw_window_create(int32_t hor_res, int32_t ver_res)
{
    if(lv_glfw_init() != 0) {
        return NULL;
    }

    lv_glfw_window_t * dsc = lv_malloc_zeroed(sizeof(lv_glfw_window_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    dsc->zoom = 1;

    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }

    lv_glfw_timer_init();

    /* Create window with graphics context */
    dsc->window = glfwCreateWindow(hor_res * dsc->zoom, ver_res * dsc->zoom, "LVGL Simulator", NULL, NULL);
    if(dsc->window == NULL) {
        LV_LOG_ERROR("glfwCreateWindow fail.\n");
        return NULL;
    }

    lv_glfw_window_config(dsc->window);

    lv_glew_init();

    lv_display_add_event_cb(disp, release_disp_cb, LV_EVENT_DELETE, disp);
    lv_display_set_driver_data(disp, dsc);

    int ret = window_create(disp);
    if(ret != 0) {
        lv_display_send_event(disp, LV_EVENT_DELETE, NULL);
        return NULL;
    }

    lv_display_set_flush_cb(disp, flush_cb);
    uint32_t stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp),
                                                  lv_display_get_color_format(disp));
    lv_display_set_buffers(disp, dsc->fb1, dsc->fb2, stride * disp->ver_res,
                           LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_add_event_cb(disp, res_chg_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

    lv_opengles_init(dsc->fb1, hor_res, ver_res);

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int lv_glfw_init(void)
{
    static bool glfw_inited = false;
    if(glfw_inited) {
        return 0;
    }

    glfwSetErrorCallback(glfw_error_cb);

    int ret = glfwInit();
    if(ret == 0) {
        LV_LOG_ERROR("glfwInit fail.\n");
        return 1;
    }

    glfw_inited = true;
    return 0;
}

static int lv_glew_init(void)
{
    static bool glew_inited = false;
    if(glew_inited) {
        return 0;
    }

    GLenum ret = glewInit();
    if(ret != GLEW_OK) {
        LV_LOG_ERROR("glewInit fail: %d.\n", ret);
        return ret;
    }

    LV_LOG_INFO("GL version: %s\n", glGetString(GL_VERSION));
    LV_LOG_INFO("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glew_inited = true;

    return 0;
}

static void lv_glfw_window_quit()
{
    int working_window = 0;
    lv_display_t * disp = lv_display_get_next(NULL);
    while(disp) {
        lv_glfw_window_t * dsc = lv_display_get_driver_data(disp);
        if(dsc != NULL) {
            working_window++;
        }

        disp = lv_display_get_next(disp);
    }

    if(working_window > 0) {
        return;
    }

    lv_timer_delete(update_handler_timer);
    update_handler_timer = NULL;

    glfwTerminate();

    lv_deinit();

    inited = false;
    exit(0);
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);
    if(lv_display_flush_is_last(disp)) {
        window_update(disp);
    }

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_display_flush_ready(disp);
}

/**
 * Handler for glfw events
 */
static void window_event_handler(lv_timer_t * t)
{
    LV_UNUSED(t);
    if(window_closed == NULL) {
        return;
    }

    lv_display_t * disp = lv_display_get_next(NULL);
    while(disp) {
        lv_glfw_window_t * dsc = lv_display_get_driver_data(disp);
        if(dsc == NULL) {
            disp = lv_display_get_next(disp);
            continue;
        }
        if(dsc->window == window_closed) {
            glfwSetWindowShouldClose(dsc->window, GLFW_TRUE);
            lv_display_send_event(disp, LV_EVENT_DELETE, NULL);
            break;
        }

        disp = lv_display_get_next(disp);
    }

    window_closed = NULL;
}

/**
 * Handler to update texture
 */
static void window_update_handler(lv_timer_t * t)
{
    LV_UNUSED(t);
    lv_display_t * disp = lv_display_get_next(NULL);
    while(disp) {
        lv_glfw_window_t * dsc = lv_display_get_driver_data(disp);
        if(dsc == NULL) {
            disp = lv_display_get_next(disp);
            continue;
        }

        glfwMakeContextCurrent(dsc->window);

        if(!glfwWindowShouldClose(dsc->window)) {
            lv_opengles_update(dsc->fb1, disp->hor_res, disp->ver_res);

            /* Swap front and back buffers */
            glfwSwapBuffers(dsc->window);

            glfwPollEvents();
        }

        disp = lv_display_get_next(disp);
    }
}

static void glfw_error_cb(int error, const char * description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static lv_display_t * _lv_glfw_get_disp_from_window(GLFWwindow * window)
{
    if(window == NULL) return NULL;

    lv_display_t * disp = lv_display_get_next(NULL);
    while(disp) {
        lv_glfw_window_t * dsc = lv_display_get_driver_data(disp);
        if(dsc != NULL && dsc->window == window) {
            return disp;
        }
        disp = lv_display_get_next(disp);
    }
    return NULL;
}

static void window_close_callback(GLFWwindow * window)
{
    window_closed = window;
}

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    LV_UNUSED(scancode);
    LV_UNUSED(mods);
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        window_closed = window;
    }
}

static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
    LV_UNUSED(mods);
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        lv_display_t * disp = _lv_glfw_get_disp_from_window(window);
        if(disp == NULL) {
            return;
        }
        if(action == GLFW_PRESS) {
            lv_glfw_mouse_btn_handler(disp, 1);
        }
        else if(action == GLFW_RELEASE) {
            lv_glfw_mouse_btn_handler(disp, 0);
        }
    }
}

static void mouse_move_cllback(GLFWwindow * window, double xpos, double ypos)
{
    lv_display_t * disp = _lv_glfw_get_disp_from_window(window);
    if(disp == NULL) {
        return;
    }
    lv_glfw_mouse_move_handler(disp, (int)xpos, (int)ypos);
}

static void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    lv_display_t * disp = _lv_glfw_get_disp_from_window(window);
    if(disp == NULL) {
        return;
    }
    lv_glfw_window_t * dsc = lv_display_get_driver_data(disp);
    dsc->ignore_size_chg = 1;
    lv_display_set_resolution(disp, width / dsc->zoom, height / dsc->zoom);
    dsc->ignore_size_chg = 0;
    lv_refr_now(disp);
}

static void texture_resize(lv_display_t * disp)
{
    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);
    uint32_t stride = lv_draw_buf_width_to_stride(hor_res, lv_display_get_color_format(disp));
    lv_glfw_window_t * dsc = lv_display_get_driver_data(disp);

    dsc->fb1 = realloc(dsc->fb1, stride * ver_res);
    lv_memzero(dsc->fb1, stride * ver_res);

    lv_display_set_buffers(disp, dsc->fb1, dsc->fb2, stride * ver_res, LV_DISPLAY_RENDER_MODE_DIRECT);
}

static int window_create(lv_display_t * disp)
{
    lv_glfw_window_t * dsc = lv_display_get_driver_data(disp);

    int32_t hor_res = disp->hor_res;
    int32_t ver_res = disp->ver_res;

    texture_resize(disp);

    uint32_t px_size = lv_color_format_get_size(lv_display_get_color_format(disp));
    lv_memset(dsc->fb1, 0xff, hor_res * ver_res * px_size);

    return 0;
}

static void window_update(lv_display_t * disp)
{
    lv_glfw_window_t * dsc = lv_display_get_driver_data(disp);
    if(dsc->fb_act) {
        LV_LOG_INFO("current pixel: %d\n", ((uint16_t *)(dsc->fb_act))[0]);
    }
    if(dsc->fb1) {
        LV_LOG_INFO("fb1 pixel: %d\n", ((uint16_t *)(dsc->fb1))[0]);
    }
}

static void res_chg_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_current_target(e);
    texture_resize(disp);
}

static void release_disp_cb(lv_event_t * e)
{
    lv_display_t * disp = (lv_display_t *) lv_event_get_user_data(e);
    lv_glfw_window_t * dsc = lv_display_get_driver_data(disp);
    if(dsc == NULL) {
        return;
    }

    glfwDestroyWindow(dsc->window);

    if(dsc->fb1) {
        free(dsc->fb1);
        dsc->fb1 = NULL;
    }
    if(dsc->fb2) {
        free(dsc->fb2);
        dsc->fb2 = NULL;
    }
    if(dsc->buf1) {
        free(dsc->buf1);
        dsc->buf1 = NULL;
    }
    if(dsc->buf2) {
        free(dsc->buf2);
        dsc->buf2 = NULL;
    }

    lv_free(dsc);
    lv_display_set_driver_data(disp, NULL);

    lv_glfw_window_quit();
}

static uint32_t lv_glfw_tick_count_callback(void)
{
    int milliseconds = (int)(glfwGetTime() * 1000);
    return milliseconds;
}

#endif /*LV_USE_OPENGLES*/
