/**
 * @file lv_glfw_window.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_glfw_window_private.h"
#if LV_USE_OPENGLES
#include <stdlib.h>
#include "../../core/lv_refr.h"
#include "../../stdlib/lv_string.h"
#include "../../core/lv_global.h"
#include "../../display/lv_display_private.h"
#include "../../indev/lv_indev.h"
#include "../../lv_init.h"
#include "../../misc/lv_area_private.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lv_opengles_driver.h"
#include "lv_opengles_texture.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void window_update_handler(lv_timer_t * t);
static uint32_t lv_glfw_tick_count_callback(void);
static lv_glfw_window_t * lv_glfw_get_lv_window_from_window(GLFWwindow * window);
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
static void proc_mouse(lv_glfw_window_t * window);
static void indev_read_cb(lv_indev_t * indev, lv_indev_data_t * data);
static void framebuffer_size_callback(GLFWwindow * window, int width, int height);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool glfw_inited;
static bool glew_inited;
static lv_timer_t * update_handler_timer;
static lv_ll_t glfw_window_ll;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_glfw_window_t * lv_glfw_window_create(int32_t hor_res, int32_t ver_res, bool use_mouse_indev)
{
    if(lv_glfw_init() != 0) {
        return NULL;
    }

    lv_glfw_window_t * window = lv_ll_ins_tail(&glfw_window_ll);
    LV_ASSERT_MALLOC(window);
    if(window == NULL) return NULL;
    lv_memzero(window, sizeof(*window));

    /* Create window with graphics context */
    lv_glfw_window_t * existing_window = lv_ll_get_head(&glfw_window_ll);
    window->window = glfwCreateWindow(hor_res, ver_res, "LVGL Simulator", NULL,
                                      existing_window ? existing_window->window : NULL);
    if(window->window == NULL) {
        LV_LOG_ERROR("glfwCreateWindow fail.");
        lv_ll_remove(&glfw_window_ll, window);
        lv_free(window);
        return NULL;
    }

    window->hor_res = hor_res;
    window->ver_res = ver_res;
    lv_ll_init(&window->textures, sizeof(lv_glfw_texture_t));
    window->use_indev = use_mouse_indev;

    glfwSetWindowUserPointer(window->window, window);
    lv_glfw_timer_init();
    lv_glfw_window_config(window->window, use_mouse_indev);
    lv_glew_init();
    glfwMakeContextCurrent(window->window);
    lv_opengles_init();

    return window;
}

void lv_glfw_window_delete(lv_glfw_window_t * window)
{
    glfwDestroyWindow(window->window);
    if(window->use_indev) {
        lv_glfw_texture_t * texture;
        LV_LL_READ(&window->textures, texture) {
            lv_indev_delete(texture->indev);
        }
    }
    lv_ll_clear(&window->textures);
    lv_ll_remove(&glfw_window_ll, window);
    lv_free(window);

    if(lv_ll_is_empty(&glfw_window_ll)) {
        lv_glfw_window_quit();
    }
}

lv_glfw_texture_t * lv_glfw_window_add_texture(lv_glfw_window_t * window, unsigned int texture_id, int32_t w, int32_t h)
{
    lv_glfw_texture_t * texture = lv_ll_ins_tail(&window->textures);
    LV_ASSERT_MALLOC(texture);
    if(texture == NULL) return NULL;
    lv_memzero(texture, sizeof(*texture));
    texture->window = window;
    texture->texture_id = texture_id;
    lv_area_set(&texture->area, 0, 0, w - 1, h - 1);
    texture->opa = LV_OPA_COVER;

    if(window->use_indev) {
        lv_display_t * texture_disp = lv_opengles_texture_get_from_texture_id(texture_id);
        if(texture_disp != NULL) {
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
            lv_indev_set_display(indev, texture_disp);
        }
    }

    return texture;
}

void lv_glfw_texture_remove(lv_glfw_texture_t * texture)
{
    if(texture->indev != NULL) {
        lv_indev_delete(texture->indev);
    }
    lv_ll_remove(&texture->window->textures, texture);
    lv_free(texture);
}

void lv_glfw_texture_set_x(lv_glfw_texture_t * texture, int32_t x)
{
    lv_area_set_pos(&texture->area, x, texture->area.y1);
}

void lv_glfw_texture_set_y(lv_glfw_texture_t * texture, int32_t y)
{
    lv_area_set_pos(&texture->area, texture->area.x1, y);
}

void lv_glfw_texture_set_opa(lv_glfw_texture_t * texture, lv_opa_t opa)
{
    texture->opa = opa;
}

lv_indev_t * lv_glfw_texture_get_mouse_indev(lv_glfw_texture_t * texture)
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

    lv_ll_init(&glfw_window_ll, sizeof(lv_glfw_window_t));

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

    lv_glfw_window_t * window;

    glfwPollEvents();

    /* delete windows that are ready to close */
    window = lv_ll_get_head(&glfw_window_ll);
    while(window) {
        lv_glfw_window_t * window_to_delete = window->closing ? window : NULL;
        window = lv_ll_get_next(&glfw_window_ll, window);
        if(window_to_delete) {
            glfwSetWindowShouldClose(window_to_delete->window, GLFW_TRUE);
            lv_glfw_window_delete(window_to_delete);
        }
    }

    /* render each window */
    LV_LL_READ(&glfw_window_ll, window) {
        glfwMakeContextCurrent(window->window);
        lv_opengles_viewport(0, 0, window->hor_res, window->ver_res);
        lv_opengles_render_clear();

        /* render each texture in the window */
        lv_glfw_texture_t * texture;
        LV_LL_READ(&window->textures, texture) {
            /* if the added texture is an LVGL opengles texture display, refresh it before rendering it */
            lv_display_t * texture_disp = lv_opengles_texture_get_from_texture_id(texture->texture_id);
            if(texture_disp != NULL) {
                lv_refr_now(texture_disp);
            }

            lv_opengles_render_texture(texture->texture_id, &texture->area, texture->opa, window->hor_res, window->ver_res);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window->window);
    }
}

static void glfw_error_cb(int error, const char * description)
{
    LV_LOG_ERROR("GLFW Error %d: %s", error, description);
}

static lv_glfw_window_t * lv_glfw_get_lv_window_from_window(GLFWwindow * window)
{
    return glfwGetWindowUserPointer(window);
}

static void window_close_callback(GLFWwindow * window)
{
    lv_glfw_window_t * lv_window = lv_glfw_get_lv_window_from_window(window);
    lv_window->closing = 1;
}

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    LV_UNUSED(scancode);
    LV_UNUSED(mods);
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        lv_glfw_window_t * lv_window = lv_glfw_get_lv_window_from_window(window);
        lv_window->closing = 1;
    }
}

static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
    LV_UNUSED(mods);
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        lv_glfw_window_t * lv_window = lv_glfw_get_lv_window_from_window(window);
        lv_window->mouse_last_state = action == GLFW_PRESS ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
        proc_mouse(lv_window);
    }
}

static void mouse_move_callback(GLFWwindow * window, double xpos, double ypos)
{
    lv_glfw_window_t * lv_window = lv_glfw_get_lv_window_from_window(window);
    lv_window->mouse_last_point.x = (int32_t)xpos;
    lv_window->mouse_last_point.y = (int32_t)ypos;
    proc_mouse(lv_window);
}

static void proc_mouse(lv_glfw_window_t * window)
{
    /* mouse activity will affect the topmost LVGL display texture */
    lv_glfw_texture_t * texture;
    LV_LL_READ_BACK(&window->textures, texture) {
        if(lv_area_is_point_on(&texture->area, &window->mouse_last_point, 0)) {
            /* adjust the mouse pointer coordinates so that they are relative to the texture */
            texture->indev_last_point.x = window->mouse_last_point.x - texture->area.x1;
            texture->indev_last_point.y = window->mouse_last_point.y - texture->area.y1;
            texture->indev_last_state = window->mouse_last_state;
            lv_indev_read(texture->indev);
            break;
        }
    }
}

static void indev_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_glfw_texture_t * texture = lv_indev_get_driver_data(indev);
    data->point = texture->indev_last_point;
    data->state = texture->indev_last_state;
}

static void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    lv_glfw_window_t * lv_window = lv_glfw_get_lv_window_from_window(window);
    lv_window->hor_res = width;
    lv_window->ver_res = height;
}

static uint32_t lv_glfw_tick_count_callback(void)
{
    double tick = glfwGetTime() * 1000.0;
    return (uint32_t)tick;
}

#endif /*LV_USE_OPENGLES*/
