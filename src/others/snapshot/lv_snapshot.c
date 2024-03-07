/**
 * @file lv_snapshot.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_snapshot.h"
#if LV_USE_SNAPSHOT

#include <stdbool.h>
#include "../../display/lv_display.h"
#include "../../core/lv_refr.h"
#include "../../display/lv_display_private.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a draw buffer for object to store the snapshot image.
 */
lv_draw_buf_t * lv_snapshot_create_draw_buf(lv_obj_t * obj, lv_color_format_t cf)
{
    lv_obj_update_layout(obj);
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    int32_t ext_size = _lv_obj_get_ext_draw_size(obj);
    w += ext_size * 2;
    h += ext_size * 2;
    if(w == 0 || h == 0) return NULL;

    return lv_draw_buf_create(w, h, cf, LV_STRIDE_AUTO);
}

lv_result_t lv_snapshot_reshape_draw_buf(lv_obj_t * obj, lv_draw_buf_t * draw_buf)
{
    lv_obj_update_layout(obj);
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    int32_t ext_size = _lv_obj_get_ext_draw_size(obj);
    w += ext_size * 2;
    h += ext_size * 2;
    if(w == 0 || h == 0) return LV_RESULT_INVALID;

    draw_buf = lv_draw_buf_reshape(draw_buf, LV_COLOR_FORMAT_UNKNOWN, w, h, LV_STRIDE_AUTO);
    return draw_buf == NULL ? LV_RESULT_INVALID : LV_RESULT_OK;
}

lv_result_t lv_snapshot_take_to_draw_buf(lv_obj_t * obj, lv_color_format_t cf, lv_draw_buf_t * draw_buf)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(draw_buf);
    lv_result_t res;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_ARGB8888:
            break;
        default:
            LV_LOG_WARN("Not supported color format");
            return LV_RESULT_INVALID;
    }

    res = lv_snapshot_reshape_draw_buf(obj, draw_buf);
    if(res != LV_RESULT_OK) return res;

    /* clear draw buffer*/
    lv_draw_buf_clear(draw_buf, NULL);

    lv_area_t snapshot_area;
    int32_t w = draw_buf->header.w;
    int32_t h = draw_buf->header.h;
    int32_t ext_size = _lv_obj_get_ext_draw_size(obj);
    lv_obj_get_coords(obj, &snapshot_area);
    lv_area_increase(&snapshot_area, ext_size, ext_size);

    lv_layer_t layer;
    lv_memzero(&layer, sizeof(layer));

    layer.draw_buf = draw_buf;
    layer.buf_area.x1 = snapshot_area.x1;
    layer.buf_area.y1 = snapshot_area.y1;
    layer.buf_area.x2 = snapshot_area.x1 + w - 1;
    layer.buf_area.y2 = snapshot_area.y1 + h - 1;
    layer.color_format = cf;
    layer._clip_area = snapshot_area;

    lv_display_t * disp_old = _lv_refr_get_disp_refreshing();
    lv_display_t * disp_new = lv_obj_get_display(obj);
    lv_layer_t * layer_old = disp_new->layer_head;
    disp_new->layer_head = &layer;

    _lv_refr_set_disp_refreshing(disp_new);
    lv_obj_redraw(&layer, obj);

    while(layer.draw_task_head) {
        lv_draw_dispatch_wait_for_request();
        lv_draw_dispatch_layer(NULL, &layer);
    }

    disp_new->layer_head = layer_old;
    _lv_refr_set_disp_refreshing(disp_old);

    return LV_RESULT_OK;
}

lv_draw_buf_t * lv_snapshot_take(lv_obj_t * obj, lv_color_format_t cf)
{
    LV_ASSERT_NULL(obj);
    lv_draw_buf_t * draw_buf = lv_snapshot_create_draw_buf(obj, cf);
    if(draw_buf == NULL) return NULL;

    if(lv_snapshot_take_to_draw_buf(obj, cf, draw_buf) != LV_RESULT_OK) {
        lv_draw_buf_destroy(draw_buf);
        return NULL;
    }

    return draw_buf;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_SNAPSHOT*/
