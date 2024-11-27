/**
 * @file lv_nuttx_libuv.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_nuttx_libuv.h"

#include "../../lvgl.h"
#include "../../lvgl_private.h"

#if LV_USE_NUTTX
#include <stdlib.h>

#if LV_USE_NUTTX_LIBUV
#include <uv.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    int fd;
    bool polling;
    uv_poll_t fb_poll;
    uv_poll_t vsync_poll;
} lv_nuttx_uv_fb_ctx_t;

typedef struct {
    int fd;
    uv_poll_t input_poll;
    lv_indev_t * indev;
} lv_nuttx_uv_input_ctx_t;

typedef struct {
    uv_timer_t uv_timer;
    lv_nuttx_uv_fb_ctx_t fb_ctx;
    lv_nuttx_uv_input_ctx_t input_ctx;
    int32_t ref_count;
} lv_nuttx_uv_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_nuttx_uv_timer_cb(uv_timer_t * handle);
static int  lv_nuttx_uv_timer_init(lv_nuttx_uv_t * uv_info, lv_nuttx_uv_ctx_t * uv_ctx);
static void lv_nuttx_uv_timer_deinit(lv_nuttx_uv_ctx_t * uv_ctx);

static void lv_nuttx_uv_vsync_poll_cb(uv_poll_t * handle, int status, int events);
static void lv_nuttx_uv_disp_poll_cb(uv_poll_t * handle, int status, int events);
static void lv_nuttx_uv_disp_refr_req_cb(lv_event_t * e);
static int  lv_nuttx_uv_fb_init(lv_nuttx_uv_t * uv_info, lv_nuttx_uv_ctx_t * uv_ctx);
static void lv_nuttx_uv_fb_deinit(lv_nuttx_uv_ctx_t * uv_ctx);

static void lv_nuttx_uv_input_poll_cb(uv_poll_t * handle, int status, int events);
static int lv_nuttx_uv_input_init(lv_nuttx_uv_t * uv_info, lv_nuttx_uv_ctx_t * uv_ctx);
static void lv_nuttx_uv_input_deinit(lv_nuttx_uv_ctx_t * uv_ctx);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_nuttx_uv_init(lv_nuttx_uv_t * uv_info)
{
    lv_nuttx_uv_ctx_t * uv_ctx;
    int ret;

    uv_ctx = lv_malloc_zeroed(sizeof(lv_nuttx_uv_ctx_t));
    LV_ASSERT_MALLOC(uv_ctx);
    if(uv_ctx == NULL) return NULL;

    if((ret = lv_nuttx_uv_timer_init(uv_info, uv_ctx)) < 0) {
        LV_LOG_ERROR("lv_nuttx_uv_timer_init fail : %d", ret);
        goto err_out;
    }

    if((ret = lv_nuttx_uv_fb_init(uv_info, uv_ctx)) < 0) {
        LV_LOG_ERROR("lv_nuttx_uv_fb_init fail : %d", ret);
        goto err_out;
    }

    if((ret = lv_nuttx_uv_input_init(uv_info, uv_ctx)) < 0) {
        LV_LOG_ERROR("lv_nuttx_uv_input_init fail : %d", ret);
        goto err_out;
    }

    return uv_ctx;

err_out:
    lv_free(uv_ctx);
    return NULL;
}

void lv_nuttx_uv_deinit(void ** data)
{
    lv_nuttx_uv_ctx_t * uv_ctx = *data;

    if(uv_ctx == NULL) return;
    lv_nuttx_uv_input_deinit(uv_ctx);
    lv_nuttx_uv_fb_deinit(uv_ctx);
    lv_nuttx_uv_timer_deinit(uv_ctx);
    *data = NULL;
    LV_LOG_USER("Done");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_nuttx_uv_timer_cb(uv_timer_t * handle)
{
    uint32_t sleep_ms;

    sleep_ms = lv_timer_handler();

    if(sleep_ms == LV_NO_TIMER_READY) {
        uv_timer_stop(handle);
        return;
    }

    /* Prevent busy loops. */

    if(sleep_ms == 0) {
        sleep_ms = 1;
    }

    LV_LOG_TRACE("sleep_ms = %" LV_PRIu32, sleep_ms);
    uv_timer_start(handle, lv_nuttx_uv_timer_cb, sleep_ms, 0);
}

static void lv_nuttx_uv_timer_resume(void * data)
{
    uv_timer_t * timer = (uv_timer_t *)data;
    if(timer)
        uv_timer_start(timer, lv_nuttx_uv_timer_cb, 0, 0);
}

static int lv_nuttx_uv_timer_init(lv_nuttx_uv_t * uv_info, lv_nuttx_uv_ctx_t * uv_ctx)
{
    uv_loop_t * loop = uv_info->loop;

    LV_ASSERT_NULL(uv_ctx);
    LV_ASSERT_NULL(loop);

    uv_ctx->uv_timer.data = uv_ctx;
    uv_timer_init(loop, &uv_ctx->uv_timer);
    uv_ctx->ref_count++;
    uv_timer_start(&uv_ctx->uv_timer, lv_nuttx_uv_timer_cb, 1, 1);

    lv_timer_handler_set_resume_cb(lv_nuttx_uv_timer_resume, &uv_ctx->uv_timer);
    return 0;
}

static void lv_nuttx_uv_deinit_cb(uv_handle_t * handle)
{
    lv_nuttx_uv_ctx_t * uv_ctx = handle->data;
    if(--uv_ctx->ref_count <= 0) {
        LV_LOG_USER("Done");
        lv_free(uv_ctx);
    }
}

static void lv_nuttx_uv_timer_deinit(lv_nuttx_uv_ctx_t * uv_ctx)
{
    lv_timer_handler_set_resume_cb(NULL, NULL);
    uv_close((uv_handle_t *)&uv_ctx->uv_timer, lv_nuttx_uv_deinit_cb);
    LV_LOG_USER("Done");
}

static void lv_nuttx_uv_vsync_poll_cb(uv_poll_t * handle, int status, int events)
{
    LV_UNUSED(handle);
    LV_UNUSED(status);
    LV_UNUSED(events);

    lv_display_t * d;
    d = lv_display_get_next(NULL);
    while(d) {
        lv_display_send_event(d, LV_EVENT_VSYNC, NULL);
        d = lv_display_get_next(d);
    }
}

static void lv_nuttx_uv_disp_poll_cb(uv_poll_t * handle, int status, int events)
{
    lv_nuttx_uv_fb_ctx_t * fb_ctx = &((lv_nuttx_uv_ctx_t *)(handle->data))->fb_ctx;

    LV_UNUSED(status);
    LV_UNUSED(events);
    uv_poll_stop(handle);
    lv_display_refr_timer(NULL);
    fb_ctx->polling = false;
}

static void lv_nuttx_uv_disp_refr_req_cb(lv_event_t * e)
{
    lv_nuttx_uv_fb_ctx_t * fb_ctx = lv_event_get_user_data(e);

    if(fb_ctx->polling) {
        return;
    }
    fb_ctx->polling = true;
    uv_poll_start(&fb_ctx->fb_poll, UV_WRITABLE, lv_nuttx_uv_disp_poll_cb);
}

static int lv_nuttx_uv_fb_init(lv_nuttx_uv_t * uv_info, lv_nuttx_uv_ctx_t * uv_ctx)
{
    uv_loop_t * loop = uv_info->loop;
    lv_display_t * disp = uv_info->disp;

    LV_ASSERT_NULL(uv_ctx);
    LV_ASSERT_NULL(disp);
    LV_ASSERT_NULL(loop);

    lv_nuttx_uv_fb_ctx_t * fb_ctx = &uv_ctx->fb_ctx;
    fb_ctx->fd = *(int *)lv_display_get_driver_data(disp);

    if(fb_ctx->fd <= 0) {
        LV_LOG_USER("skip uv fb init.");
        return 0;
    }

    if(!disp->refr_timer) {
        LV_LOG_ERROR("disp->refr_timer is NULL");
        return -EINVAL;
    }

    /* Remove default refr timer. */

    lv_timer_delete(disp->refr_timer);
    disp->refr_timer = NULL;

    fb_ctx->fb_poll.data = uv_ctx;
    uv_poll_init(loop, &fb_ctx->fb_poll, fb_ctx->fd);
    uv_ctx->ref_count++;
    uv_poll_start(&fb_ctx->fb_poll, UV_WRITABLE, lv_nuttx_uv_disp_poll_cb);

    fb_ctx->vsync_poll.data = uv_ctx;
    uv_poll_init(loop, &fb_ctx->vsync_poll, fb_ctx->fd);
    uv_ctx->ref_count++;
    uv_poll_start(&fb_ctx->vsync_poll, UV_PRIORITIZED, lv_nuttx_uv_vsync_poll_cb);

    LV_LOG_USER("lvgl fb loop start OK");

    /* Register for the invalidate area event */

    lv_event_add(&disp->event_list, lv_nuttx_uv_disp_refr_req_cb, LV_EVENT_REFR_REQUEST, fb_ctx);

    return 0;
}

static void lv_nuttx_uv_fb_deinit(lv_nuttx_uv_ctx_t * uv_ctx)
{
    /* should remove event */
    lv_nuttx_uv_fb_ctx_t * fb_ctx = &uv_ctx->fb_ctx;
    if(fb_ctx->fd > 0) {
        uv_close((uv_handle_t *)&fb_ctx->fb_poll, lv_nuttx_uv_deinit_cb);
        uv_close((uv_handle_t *)&fb_ctx->vsync_poll, lv_nuttx_uv_deinit_cb);
    }
    LV_LOG_USER("Done");
}

static void lv_nuttx_uv_input_poll_cb(uv_poll_t * handle, int status, int events)
{
    lv_indev_t * indev = ((lv_nuttx_uv_ctx_t *)(handle->data))->input_ctx.indev;

    if(status < 0) {
        LV_LOG_WARN("input poll error: %s ", uv_strerror(status));
        return;
    }

    if(events & UV_READABLE) {
        lv_indev_read(indev);
    }
}

static int lv_nuttx_uv_input_init(lv_nuttx_uv_t * uv_info, lv_nuttx_uv_ctx_t * uv_ctx)
{
    uv_loop_t * loop = uv_info->loop;
    lv_indev_t * indev = uv_info->indev;

    if(indev == NULL) {
        LV_LOG_USER("skip uv input init.");
        return 0;
    }

    LV_ASSERT_NULL(uv_ctx);
    LV_ASSERT_NULL(loop);

    if(lv_indev_get_mode(indev) == LV_INDEV_MODE_EVENT) {
        LV_LOG_ERROR("input device has been running in event-driven mode");
        return -EINVAL;
    }

    lv_nuttx_uv_input_ctx_t * input_ctx = &uv_ctx->input_ctx;
    input_ctx->fd = *(int *)lv_indev_get_driver_data(indev);
    if(input_ctx->fd <= 0) {
        LV_LOG_ERROR("can't get valid input fd");
        return 0;
    }

    input_ctx->indev = indev;
    lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);

    input_ctx->input_poll.data = uv_ctx;
    uv_poll_init(loop, &input_ctx->input_poll, input_ctx->fd);
    uv_ctx->ref_count++;
    uv_poll_start(&input_ctx->input_poll, UV_READABLE, lv_nuttx_uv_input_poll_cb);

    LV_LOG_USER("lvgl input loop start OK");

    return 0;
}

static void lv_nuttx_uv_input_deinit(lv_nuttx_uv_ctx_t * uv_ctx)
{
    lv_nuttx_uv_input_ctx_t * input_ctx = &uv_ctx->input_ctx;
    if(input_ctx->fd > 0) {
        uv_close((uv_handle_t *)&input_ctx->input_poll, lv_nuttx_uv_deinit_cb);
    }
    LV_LOG_USER("Done");
}

#endif /*LV_USE_NUTTX_LIBUV*/

#endif /*LV_USE_NUTTX*/
