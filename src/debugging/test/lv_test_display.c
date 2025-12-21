/**
 * @file lv_test_display.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_display.h"
#if LV_USE_TEST

#include "../../core/lv_global.h"
#include "../../drivers/opengles/lv_opengles_debug.h"
#include "../../drivers/opengles/lv_opengles_texture.h"
#include "../../drivers/opengles/lv_opengles_private.h"
#include "../../drivers/opengles/lv_opengles_driver.h"
#include <stdlib.h>
#include "../../lv_init.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static lv_draw_buf_t create_test_draw_buf(int32_t hor_res, int32_t ver_res, lv_color_format_t cf);
static void buf_changed_event_cb(lv_event_t * e);
static void delete_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#define _state LV_GLOBAL_DEFAULT()->test_state

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_test_display_create(int32_t hor_res, int32_t ver_res)
{
    const lv_color_format_t cf = LV_COLOR_FORMAT_ARGB8888;
    _state.draw_buf = create_test_draw_buf(hor_res, ver_res, cf);

#if LV_USE_DRAW_OPENGLES
    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    _state.window = glfwCreateWindow(hor_res, ver_res, "", NULL, NULL);
    glfwMakeContextCurrent(_state.window);

    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        glfwDestroyWindow(_state.window);
        lv_draw_buf_destroy(&_state.draw_buf);
        return NULL;
    }
    lv_opengles_init();
    lv_display_t * disp = lv_opengles_texture_create(hor_res, ver_res);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_FULL);
#else
    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_DIRECT);
#endif

    lv_display_set_draw_buffers(disp, &_state.draw_buf, NULL);
    lv_display_set_color_format(disp, cf);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_add_event_cb(disp, buf_changed_event_cb, LV_EVENT_COLOR_FORMAT_CHANGED, NULL);
    lv_display_add_event_cb(disp, buf_changed_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(disp, delete_event_cb, LV_EVENT_DELETE, NULL);

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_draw_buf_t create_test_draw_buf(int32_t hor_res, int32_t ver_res, lv_color_format_t cf)
{
    lv_draw_buf_t res;
    size_t buf_size = lv_draw_buf_width_to_stride(hor_res, cf) * ver_res;
    uint8_t * buf = lv_malloc(buf_size);
    LV_ASSERT_MALLOC(buf);

    lv_draw_buf_init(&res, hor_res, ver_res, cf, LV_STRIDE_AUTO, lv_draw_buf_align(buf, cf), buf_size);
    res.unaligned_data = buf;
    return res;
}

static void buf_changed_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_color_format_t cf = lv_display_get_color_format(disp);
    int32_t hor_res = lv_display_get_original_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_original_vertical_resolution(disp);

    lv_free(_state.draw_buf.unaligned_data);

    size_t buf_size = 4 * (hor_res + LV_DRAW_BUF_STRIDE_ALIGN - 1) * ver_res + LV_DRAW_BUF_ALIGN;
    uint8_t * buf = lv_malloc(buf_size);
    LV_ASSERT_MALLOC(buf);

    lv_draw_buf_init(&_state.draw_buf, hor_res, ver_res, cf, LV_STRIDE_AUTO, lv_draw_buf_align(buf, cf), buf_size);
    _state.draw_buf.unaligned_data = buf;
    lv_display_set_draw_buffers(disp, &_state.draw_buf, NULL);
}

static void delete_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_display_t * disp = lv_event_get_target(e);
    lv_draw_buf_t * draw_buf = lv_display_get_buf_active(disp);
    lv_free(draw_buf->unaligned_data);
    draw_buf->unaligned_data = NULL;
#if LV_USE_DRAW_OPENGLES
    if(_state.window) {
        glfwDestroyWindow(_state.window);
        _state.window = NULL;
    }
    lv_deinit();
    glfwTerminate();
#endif
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LV_UNUSED(area);
    LV_UNUSED(color_p);
    lv_display_flush_ready(disp);
#if LV_USE_DRAW_OPENGLES
    if(!lv_display_flush_is_last(disp)) {
        return;
    }
    const lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t width = _state.draw_buf.header.w;
    uint32_t height = _state.draw_buf.header.h;
    uint8_t * data = _state.draw_buf.data;

    lv_opengles_render_params_t params  = {
        .h_flip = false,
        .v_flip = false,
        .rb_swap = false
    };
    lv_opengles_render_display(disp, &params);

    if(cf == LV_COLOR_FORMAT_XRGB8888 || cf == LV_COLOR_FORMAT_ARGB8888) {
        GL_CALL(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data));
    }
    else if(cf == LV_COLOR_FORMAT_RGB888) {
        GL_CALL(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data));
    }
    else if(cf == LV_COLOR_FORMAT_RGB565) {
        GL_CALL(glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data));
    }
#endif
}

#endif /*LV_USE_TEST*/
