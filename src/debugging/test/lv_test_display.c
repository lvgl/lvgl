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
#include <stdlib.h>
#include "../../drivers/opengles/lv_opengles_pbuffer.h"

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

static void refr_ready_event_cb(lv_event_t * e);

static lv_display_t * create_sw_display(int32_t hor_res, int32_t ver_res);
static lv_display_t * create_opengl_display(int32_t hor_res, int32_t ver_res);

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
    const lv_color_format_t cf = LV_COLOR_FORMAT_XRGB8888;
    _state.draw_buf = create_test_draw_buf(hor_res, ver_res, cf);

    lv_display_t * disp;
    if(LV_USE_DRAW_OPENGLES && LV_USE_EGL) {
        disp = create_opengl_display(hor_res, ver_res);
    }
    else {
        disp = create_sw_display(hor_res, ver_res);
    }

    if(!disp) {
        lv_free(_state.draw_buf.unaligned_data);
        return NULL;
    }

    lv_display_set_color_format(disp, cf);

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static lv_display_t * create_sw_display(int32_t hor_res, int32_t ver_res)
{
    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(!disp) {
        LV_LOG_ERROR("Failed to create display");
        return NULL;
    }

    lv_display_set_draw_buffers(disp, &_state.draw_buf, NULL);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_add_event_cb(disp, buf_changed_event_cb, LV_EVENT_COLOR_FORMAT_CHANGED, NULL);
    lv_display_add_event_cb(disp, buf_changed_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(disp, delete_event_cb, LV_EVENT_DELETE, NULL);
    return disp;

}
static lv_display_t * create_opengl_display(int32_t hor_res, int32_t ver_res)
{
    lv_display_t * disp = lv_opengles_pbuffer_create(hor_res, ver_res);
    if(!disp) {
        LV_LOG_ERROR("Failed to create pbuffer display");
        return NULL;
    }
    /* Listen for refresh complete events to copy data back to CPU */
    lv_display_add_event_cb(disp, refr_ready_event_cb, LV_EVENT_REFR_READY, NULL);
    return disp;
}

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
    lv_draw_buf_t * draw_buf = &_state.draw_buf;

    if(draw_buf->unaligned_data) {
        lv_free(draw_buf->unaligned_data);
        draw_buf->unaligned_data = NULL;
    }
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LV_UNUSED(area);
    LV_UNUSED(color_p);
    lv_display_flush_ready(disp);
}

#if LV_USE_DRAW_OPENGLES && LV_USE_EGL
static void refr_ready_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_result_t res = lv_opengles_pbuffer_read_to_draw_buf(disp, &_state.draw_buf);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to read pbuffer to draw buffer");
    }
}
#endif

#endif /*LV_USE_TEST*/
