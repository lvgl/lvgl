/**
 * @file lv_snapshot.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"
#if LV_USE_SNAPSHOT

#include "../../draw/lv_draw_private.h"
#include "../../core/lv_obj_draw_private.h"
#include LV_STDBOOL_INCLUDE
#include "../../core/lv_refr_private.h"
#include "../../display/lv_display_private.h"
#include "../../core/lv_obj_private.h"

/*********************
 *      DEFINES
 *********************/

/*Consecutive `lv_draw_dispatch()` calls tolerated without the pending task
 *count changing. A layer allocation that fails leaves its draw task queued
 *("Try later" in `lv_draw_layer_alloc_buf()`), and without an OS the loop below
 *would retry it forever: a fragmented heap turns a snapshot into a livelock
 *that hangs the whole application. 512 is roughly 100 ms of failing retries on
 *a low-end target, far above any legitimate stall.*/
#define LV_SNAPSHOT_MAX_STALLED_DISPATCHES 512U

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static uint32_t snapshot_pending_task_count(const lv_layer_t * layer);
static void snapshot_discard_layer_tasks(lv_layer_t * layer);

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
    LV_CHECK_OBJ(obj, &lv_obj_class, return NULL);

    lv_obj_update_layout(obj);
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
    w += ext_size * 2;
    h += ext_size * 2;
    if(w == 0 || h == 0) return NULL;

    return lv_draw_buf_create(w, h, cf, LV_STRIDE_AUTO);
}

lv_result_t lv_snapshot_reshape_draw_buf(lv_obj_t * obj, lv_draw_buf_t * draw_buf)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return LV_RESULT_INVALID);
    LV_CHECK_ARG(draw_buf != NULL, return LV_RESULT_INVALID);

    lv_obj_update_layout(obj);
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
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
        case LV_COLOR_FORMAT_ARGB8565:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_ARGB2222:
        case LV_COLOR_FORMAT_ARGB4444:
        case LV_COLOR_FORMAT_ARGB1555:
            break;
        default:
            LV_LOG_WARN("Not supported color format");
            return LV_RESULT_INVALID;
    }

    res = lv_snapshot_reshape_draw_buf(obj, draw_buf);
    if(res != LV_RESULT_OK) return res;

    lv_area_t snapshot_area;
    int32_t w = draw_buf->header.w;
    int32_t h = draw_buf->header.h;
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
    lv_obj_get_coords(obj, &snapshot_area);
    lv_area_increase(&snapshot_area, ext_size, ext_size);

    lv_obj_t * top_obj = lv_refr_get_top_obj(&snapshot_area, obj);
    if(top_obj == NULL) {
        /* Clear draw buffer when no top object*/
        lv_draw_buf_clear(draw_buf, NULL);
        top_obj = obj;
    }

    lv_layer_t layer;
    lv_layer_init(&layer);

    layer.draw_buf = draw_buf;
    layer.buf_area.x1 = snapshot_area.x1;
    layer.buf_area.y1 = snapshot_area.y1;
    layer.buf_area.x2 = snapshot_area.x1 + w - 1;
    layer.buf_area.y2 = snapshot_area.y1 + h - 1;
    layer.color_format = cf;
    layer._clip_area = snapshot_area;
    layer.phy_clip_area = snapshot_area;

    lv_draw_unit_send_event(NULL, LV_EVENT_CHILD_CREATED, &layer);

    lv_display_t * disp_old = lv_refr_get_disp_refreshing();
    lv_display_t * disp_new = lv_obj_get_display(obj);
    lv_layer_t * layer_old = disp_new->layer_head;
    disp_new->layer_head = &layer;

    lv_refr_set_disp_refreshing(disp_new);

    if(top_obj == obj) {
        lv_obj_redraw(&layer, top_obj);
    }
    else {
        lv_obj_refr(&layer, top_obj);

        lv_obj_t * parent = lv_obj_get_parent(top_obj);
        lv_obj_t * border_p = top_obj;

        /*Do until not reach the screen*/
        while(parent != NULL && border_p != obj) {
            bool go = false;
            uint32_t i;
            uint32_t child_cnt = lv_obj_get_child_count(parent);
            for(i = 0; i < child_cnt; i++) {
                lv_obj_t * child = parent->spec_attr->children[i];
                if(!go) {
                    if(child == border_p) go = true;
                }
                else {
                    /*Refresh the objects*/
                    lv_obj_refr(&layer, child);
                }
            }

            /*Call the post draw draw function of the parents of the to object*/
            lv_obj_send_event(parent, LV_EVENT_DRAW_POST_BEGIN, (void *)&layer);
            lv_obj_send_event(parent, LV_EVENT_DRAW_POST, (void *)&layer);
            lv_obj_send_event(parent, LV_EVENT_DRAW_POST_END, (void *)&layer);

            /*The new border will be the last parents,
            *so the 'younger' brothers of parent will be refreshed*/
            border_p = parent;
            /*Go a level deeper*/
            parent = lv_obj_get_parent(parent);
        }
    }

    layer.all_tasks_added = true;

    uint32_t stalled_dispatches = 0;
    uint32_t last_task_count = snapshot_pending_task_count(&layer);

    while(layer.draw_task_head) {
#if LV_USE_OS
        /*Without an OS this busy-spins on `_draw_info.dispatch_req`, so the
         *stall counter below would never be reached.*/
        lv_draw_dispatch_wait_for_request();
#endif
        lv_draw_dispatch();

        uint32_t task_count = snapshot_pending_task_count(&layer);
        if(task_count != last_task_count) {
            last_task_count = task_count;
            stalled_dispatches = 0;
        }
        else if(++stalled_dispatches > LV_SNAPSHOT_MAX_STALLED_DISPATCHES) {
            LV_LOG_WARN("Snapshot draw queue stalled, aborting");
            lv_draw_wait_for_finish();
            snapshot_discard_layer_tasks(&layer);
            disp_new->layer_head = layer_old;
            lv_refr_set_disp_refreshing(disp_old);

            /*Balance the CHILD_CREATED sent for the top layer above; sub-layers
             *were already balanced by lv_draw_cleanup_task().*/
            lv_draw_unit_send_event(NULL, LV_EVENT_CHILD_DELETED, &layer);
            return LV_RESULT_INVALID;
        }
    }

    disp_new->layer_head = layer_old;
    lv_refr_set_disp_refreshing(disp_old);

    lv_draw_unit_send_event(NULL, LV_EVENT_SCREEN_LOAD_START, &layer);
    lv_draw_unit_send_event(NULL, LV_EVENT_CHILD_DELETED, &layer);

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

void lv_snapshot_free(lv_image_dsc_t * dsc)
{
    if(dsc == NULL) return;

    LV_LOG_DEPRECATED("use lv_draw_buf_destroy directly");
    lv_draw_buf_destroy((lv_draw_buf_t *)dsc);
}

lv_result_t lv_snapshot_take_to_buf(lv_obj_t * obj, lv_color_format_t cf, lv_image_dsc_t * dsc,
                                    void * buf,
                                    uint32_t buf_size)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return LV_RESULT_INVALID);
    LV_CHECK_ARG(dsc != NULL, return LV_RESULT_INVALID);
    LV_CHECK_ARG(buf != NULL, return LV_RESULT_INVALID);

    lv_draw_buf_t draw_buf;
    LV_LOG_DEPRECATED("use lv_snapshot_take_to_draw_buf instead.");
    lv_draw_buf_init(&draw_buf, 1, 1, cf, buf_size, buf, buf_size);
    lv_result_t res = lv_snapshot_take_to_draw_buf(obj, cf, &draw_buf);
    if(res == LV_RESULT_OK) {
        lv_memcpy((void *)dsc, &draw_buf, sizeof(lv_image_dsc_t));
    }
    return res;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Count the draw tasks still pending for a snapshot, across every layer the
 * redraw created.
 *
 * Counting only the top layer would miss all progress made inside a sub-layer:
 * a task leaves the top layer's list only once it is `READY`, so rendering a
 * sub-layer keeps the top count constant and would look like a stall.
 * @param  layer   the snapshot's top layer, which is the head of the chain
 * @return         number of pending draw tasks
 */
static uint32_t snapshot_pending_task_count(const lv_layer_t * layer)
{
    uint32_t count = 0;

    while(layer) {
        const lv_draw_task_t * task = layer->draw_task_head;
        while(task) {
            count++;
            task = task->next;
        }
        layer = layer->next;
    }

    return count;
}

/**
 * Release every draw task a stalled snapshot left queued, freeing the sub-layers
 * they created along the way.
 *
 * `lv_draw_dispatch_layer()` only cleans up tasks that reached
 * `LV_DRAW_TASK_STATE_READY`, and a sub-layer is freed only when its parent's
 * LAYER task is cleaned up, so abandoning the redraw would otherwise leak both
 * the queued tasks and every sub-layer allocated for them - on the one path that
 * runs when memory is already short.  `lv_draw_cleanup_task()` frees a LAYER
 * task's sub-layer but not the tasks still queued inside it, so those are
 * released first by recursing into the sub-layer.
 * @param  layer   layer whose queued tasks are released.  Its own draw buffer
 *                 is left untouched: for the snapshot's top layer it belongs to
 *                 the caller, and a sub-layer's buffer is freed by the parent
 *                 LAYER task that owns it.
 */
static void snapshot_discard_layer_tasks(lv_layer_t * layer)
{
    lv_draw_task_t * task = layer->draw_task_head;

    while(task) {
        lv_draw_task_t * task_next = task->next;

        if(task->type == LV_DRAW_TASK_TYPE_LAYER) {
            lv_draw_image_dsc_t * image_dsc = task->draw_dsc;
            lv_layer_t * sub_layer = (lv_layer_t *)image_dsc->src;
            if(sub_layer) snapshot_discard_layer_tasks(sub_layer);
        }

        lv_draw_cleanup_task(task);

        task = task_next;
    }

    layer->draw_task_head = NULL;
}

#endif /*LV_USE_SNAPSHOT*/
