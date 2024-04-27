/**
 * @file lv_opengles_window.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_opengles_window.h"
#if LV_USE_OPENGLES
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include <pthread.h>
#include <unistd.h>
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
} lv_opengles_window_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void * opengles_tick_thread(void * data);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static int window_create(lv_display_t * disp);
static void window_update(lv_display_t * disp);
static void texture_resize(lv_display_t * disp);
static void window_update_handler(lv_timer_t * t);
static void window_event_handler(lv_timer_t * t);
static void release_disp_cb(lv_event_t * e);
static void res_chg_event_cb(lv_event_t * e);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/
static bool inited = false;
bool deiniting = false;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_timer_t * update_handler_timer;
static lv_timer_t * event_handler_timer;

#define lv_deinit_in_progress  LV_GLOBAL_DEFAULT()->deinit_in_progress

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_opengles_window_create(int32_t hor_res, int32_t ver_res)
{
    if(!inited) {
        update_handler_timer = lv_timer_create(window_update_handler, 5, NULL);
        event_handler_timer = lv_timer_create(window_event_handler, 5, NULL);

        inited = true;

        pthread_t thr_tick = 0;
        pthread_create(&thr_tick, NULL, opengles_tick_thread, NULL);
    }

    lv_opengles_window_t * dsc = lv_malloc_zeroed(sizeof(lv_opengles_window_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }

    lv_display_add_event_cb(disp, release_disp_cb, LV_EVENT_DELETE, disp);

    lv_display_set_driver_data(disp, dsc);

    window_create(disp);

    lv_display_set_flush_cb(disp, flush_cb);
    uint32_t stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp),
                                                  lv_display_get_color_format(disp));
    lv_display_set_buffers(disp, dsc->fb1, dsc->fb2, stride * disp->ver_res,
                           LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_add_event_cb(disp, res_chg_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

    return disp;
}

lv_display_t * lv_opengles_get_disp_from_window(GLFWwindow * window)
{
    return NULL;
}

void lv_opengles_window_set_title(lv_display_t * disp, const char * title)
{
    lv_opengles_window_t * dsc = lv_display_get_driver_data(disp);
    glfwSetWindowTitle(dsc->window, title);
}

void * lv_opengles_window_get_renderer(lv_display_t * disp)
{
    return NULL;
}

void lv_opengles_quit(lv_display_t * disp)
{
    lv_opengles_window_t * dsc = lv_display_get_driver_data(disp);
    if(inited) {
        lv_timer_delete(update_handler_timer);
        update_handler_timer = NULL;

        glfwDestroyWindow(dsc->window);
        glfwTerminate();

        inited = false;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
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
    if(deiniting == false) {
        return;
    }

    lv_display_t * disp = lv_display_get_default();
    if(disp == NULL) {
        return;
    }
    lv_opengles_window_t * dsc = lv_display_get_driver_data(disp);
    if(dsc == NULL) {
        return;
    }

    glfwSetWindowShouldClose(dsc->window, GLFW_TRUE);
    lv_display_send_event(disp, LV_EVENT_DELETE, NULL);
}

/**
 * Handler to update texture
 */
static void window_update_handler(lv_timer_t * t)
{
    //lv_display_t * disp = lv_timer_get_user_data(t);
    lv_display_t * disp = lv_display_get_default();
    if(disp == NULL) {
        return;
    }
    lv_opengles_window_t * dsc = lv_display_get_driver_data(disp);
    if(dsc == NULL) {
        return;
    }

    if(!glfwWindowShouldClose(dsc->window)) {
        lv_opengles_render_clear();
        lv_opengles_texture_update(dsc->fb1, disp->hor_res, disp->ver_res);

        lv_opengles_shader_bind();
        lv_opengles_shader_set_uniform4f("u_Color", 0.0f, 0.3f, 0.8f, 1.0f);
        lv_opengles_render_draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(dsc->window);

        glfwPollEvents();
    }
}

static void glfw_error_cb(int error, const char * description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void window_close_callback(GLFWwindow * window)
{
    deiniting = true;
}

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        deiniting = true;
    }
}

void _lv_opengles_mouse_btn_handler(lv_display_t * disp, int btn_down);

static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lv_display_t * disp = lv_display_get_default();
        if(action == GLFW_PRESS) {
            // LV_LOG_INFO("Mouse position: (%f, %f)\n", xpos, ypos);
            _lv_opengles_mouse_btn_handler(disp, 1);
        }
        else if(action == GLFW_RELEASE) {
            _lv_opengles_mouse_btn_handler(disp, 0);
        }
    }
}

void _lv_opengles_mouse_move_handler(lv_display_t * disp, int x, int y);

static void mouse_move_cllback(GLFWwindow * window, double xpos, double ypos)
{
    // LV_LOG_INFO("Mouse position: (%f, %f)\n", xpos, ypos);
    lv_display_t * disp = lv_display_get_default();
    _lv_opengles_mouse_move_handler(disp, (int)xpos, (int)ypos);
}

static void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
    lv_display_t * disp = lv_display_get_default();
    if(disp == NULL) {
        return;
    }
    lv_opengles_window_t * dsc = lv_display_get_driver_data(disp);
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
    lv_opengles_window_t * dsc = lv_display_get_driver_data(disp);

    dsc->fb1 = realloc(dsc->fb1, stride * ver_res);
    lv_memzero(dsc->fb1, stride * ver_res);

    lv_display_set_buffers(disp, dsc->fb1, dsc->fb2, stride * ver_res, LV_DISPLAY_RENDER_MODE_DIRECT);
}

static int window_create(lv_display_t * disp)
{
    lv_opengles_window_t * dsc = lv_display_get_driver_data(disp);
    dsc->zoom = 1;

    glfwSetErrorCallback(glfw_error_cb);
    if(!glfwInit())
        return 1;

    const char * glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    int32_t hor_res = disp->hor_res;
    int32_t ver_res = disp->ver_res;

    // Create window with graphics context
    dsc->window = glfwCreateWindow(hor_res * dsc->zoom, ver_res * dsc->zoom, "hellow world", NULL, NULL);
    if(dsc->window == NULL)
        return 2;

    glfwMakeContextCurrent(dsc->window);
    if(glewInit() != GLEW_OK) {
        LV_LOG_ERROR("glewInit fail\n");
        return 3;
    }

    LV_LOG_INFO("GL version: %s\n", glGetString(GL_VERSION));
    LV_LOG_INFO("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(dsc->window, framebuffer_size_callback);

    glfwSetMouseButtonCallback(dsc->window, mouse_button_callback);
    glfwSetCursorPosCallback(dsc->window, mouse_move_cllback);

    glfwSetKeyCallback(dsc->window, key_callback);

    glfwSetWindowCloseCallback(dsc->window, window_close_callback);

    texture_resize(disp);

    uint32_t px_size = lv_color_format_get_size(lv_display_get_color_format(disp));
    lv_memset(dsc->fb1, 0xff, hor_res * ver_res * px_size);

    float positions[] = {
        -1.0f,  1.0f,  0.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    //GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    lv_opengles_vertex_buffer_init(positions, 4 * 4 * sizeof(float));

    lv_opengles_vertex_array_init();
    lv_opengles_vertex_array_add_buffer();

    lv_opengles_index_buffer_init(indices, 6);

    lv_opengles_shader_init();
    lv_opengles_shader_bind();
    lv_opengles_shader_set_uniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    int slot = 0;
    lv_opengles_texture_init(dsc->fb1, hor_res, ver_res);
    lv_opengles_texture_bind(slot);

    lv_opengles_shader_set_uniform1i("u_Texture", slot);

    //unbound everything
    lv_opengles_vertex_array_unbind();
    lv_opengles_vertex_buffer_unbind();
    lv_opengles_index_buffer_unbind();
    lv_opengles_shader_unbind();

    return 0;
}

static void window_update(lv_display_t * disp)
{
    lv_opengles_window_t * dsc = lv_display_get_driver_data(disp);
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

    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);
    lv_opengles_window_t * dsc = lv_display_get_driver_data(disp);

    texture_resize(disp);
}

static void release_disp_cb(lv_event_t * e)
{
    lv_display_t * disp = (lv_display_t *) lv_event_get_user_data(e);
    lv_opengles_window_t * dsc = lv_display_get_driver_data(disp);

    lv_opengles_quit(disp);

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

    //lv_display_set_driver_data(disp, NULL);
    lv_deinit();
    lv_free(dsc);

    exit(0);
}

static void * opengles_tick_thread(void * data)
{
    while(inited) {
        usleep(5000);
        lv_tick_inc(5);
    }
    return NULL;
}

#endif /*LV_USE_OPENGLES*/
