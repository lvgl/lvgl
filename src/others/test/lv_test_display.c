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
#include "../../lvgl_private.h"
#include "../../drivers/opengles/lv_opengles_glfw.c"
#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void dummy_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);

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
#if LV_USE_GLFW
    LV_UNUSED(dummy_flush_cb);
    LV_UNUSED(buf_changed_event_cb);
    LV_UNUSED(delete_event_cb);

    /* create a window and initialize OpenGL */
    lv_opengles_window_t * window = lv_opengles_glfw_window_create(hor_res, ver_res, true);

    /* create a display that flushes to a texture */
    lv_display_t * disp = lv_opengles_texture_create(hor_res, ver_res);
    lv_display_set_default(disp);

    /* add the texture to the window */
    unsigned int texture_id = lv_opengles_texture_get_texture_id(disp);
    lv_opengles_window_texture_t * window_texture = lv_opengles_window_add_texture(window, texture_id, hor_res, ver_res);

    /* get the mouse indev of the window texture */
    lv_indev_t * mouse = lv_opengles_window_texture_get_mouse_indev(window_texture);
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_display(mouse, disp);

    lv_draw_buf_t * draw_buf = lv_display_get_buf_active(disp);
    LV_ASSERT_NULL(draw_buf);
    _state.draw_buf = *draw_buf;

    /**
     * Remove the timestamp callback set internally by lv_glfw to
     * avoid affecting testcases that are strongly related to timestamps
     */
    lv_tick_set_cb(NULL);
#else
    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_XRGB8888);

    size_t buf_size = 4 * (hor_res + LV_DRAW_BUF_STRIDE_ALIGN - 1) * ver_res + LV_DRAW_BUF_ALIGN;
    uint8_t * buf = malloc(buf_size);
    LV_ASSERT_MALLOC(buf);

    lv_draw_buf_init(&_state.draw_buf, hor_res, ver_res, LV_COLOR_FORMAT_XRGB8888, LV_STRIDE_AUTO, lv_draw_buf_align(buf,
                                                                                                                     LV_COLOR_FORMAT_XRGB8888), buf_size);
    _state.draw_buf.unaligned_data = buf;
    lv_display_set_draw_buffers(disp, &_state.draw_buf, NULL);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_DIRECT);

    lv_display_set_flush_cb(disp, dummy_flush_cb);

    lv_display_add_event_cb(disp, buf_changed_event_cb, LV_EVENT_COLOR_FORMAT_CHANGED, NULL);
    lv_display_add_event_cb(disp, buf_changed_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(disp, delete_event_cb, LV_EVENT_DELETE, NULL);
#endif /* LV_USE_DRAW_OPENGLES */

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void buf_changed_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_color_format_t cf = lv_display_get_color_format(disp);
    int32_t hor_res = lv_display_get_original_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_original_vertical_resolution(disp);

    free(_state.draw_buf.unaligned_data);

    size_t buf_size = 4 * (hor_res + LV_DRAW_BUF_STRIDE_ALIGN - 1) * ver_res + LV_DRAW_BUF_ALIGN;
    uint8_t * buf = malloc(buf_size);
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
    free(draw_buf->unaligned_data);

}

static void dummy_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LV_UNUSED(area);
    LV_UNUSED(color_p);
    lv_display_flush_ready(disp);
}

#endif /*LV_USE_TEST*/
