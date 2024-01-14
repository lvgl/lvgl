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

uint32_t lv_snapshot_buf_size_needed(lv_obj_t * obj, lv_color_format_t cf)
{
    LV_ASSERT_NULL(obj);
    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_ARGB8888:
            break;
        default:
            LV_LOG_WARN("Not supported color format");
            return 0;
    }

    lv_obj_update_layout(obj);

    /*Width and height determine snapshot image size.*/
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    int32_t ext_size = _lv_obj_get_ext_draw_size(obj);
    w += ext_size * 2;
    h += ext_size * 2;

    return lv_draw_buf_width_to_stride(w, cf) * h;
}

lv_result_t lv_snapshot_take_to_buf(lv_obj_t * obj, lv_color_format_t cf, lv_image_dsc_t * dsc, void * buf,
                                    uint32_t buf_size)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(dsc);
    LV_ASSERT_NULL(buf);

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

    uint32_t buf_size_needed = lv_snapshot_buf_size_needed(obj, cf);
    if(buf_size_needed == 0 || buf_size < buf_size_needed) return LV_RESULT_INVALID;

    LV_ASSERT_MSG(buf == lv_draw_buf_align(buf, cf), "Buffer is not aligned");

    /*Width and height determine snapshot image size.*/
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    int32_t ext_size = _lv_obj_get_ext_draw_size(obj);
    w += ext_size * 2;
    h += ext_size * 2;

    lv_area_t snapshot_area;
    lv_obj_get_coords(obj, &snapshot_area);
    lv_area_increase(&snapshot_area, ext_size, ext_size);

    lv_memzero(buf, buf_size);
    dsc->data = buf;
    dsc->data_size = buf_size_needed;
    /*Keep header flags unchanged, because we don't know if it's allocated or not.*/
    dsc->header.w = w;
    dsc->header.h = h;
    dsc->header.cf = cf;
    dsc->header.magic = LV_IMAGE_HEADER_MAGIC;

    lv_layer_t layer;
    lv_memzero(&layer, sizeof(layer));

    lv_draw_buf_t draw_buf;
    lv_draw_buf_from_image(&draw_buf, dsc);

    layer.draw_buf = &draw_buf;
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

lv_image_dsc_t * lv_snapshot_take(lv_obj_t * obj, lv_color_format_t cf)
{
    LV_ASSERT_NULL(obj);
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    int32_t ext_size = _lv_obj_get_ext_draw_size(obj);
    w += ext_size * 2;
    h += ext_size * 2;
    if(w == 0 || h == 0) return NULL;

    lv_draw_buf_t * draw_buf = lv_draw_buf_create(w, h, cf, LV_STRIDE_AUTO);
    if(draw_buf == NULL) return NULL;

    if(lv_snapshot_take_to_buf(obj, cf, (lv_image_dsc_t *)draw_buf, draw_buf->data, draw_buf->data_size) != LV_RESULT_OK) {
        lv_draw_buf_destroy(draw_buf);
        return NULL;
    }

    return (lv_image_dsc_t *)draw_buf;
}

void lv_snapshot_free(lv_image_dsc_t * dsc)
{
    if(!dsc)
        return;

    lv_draw_buf_destroy((lv_draw_buf_t *)dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_SNAPSHOT*/
