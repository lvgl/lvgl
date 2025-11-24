/**
 * @file lv_draw_dave2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_dave2d.h"
#if LV_USE_DRAW_DAVE2D
#include "../../lv_draw_buf_private.h"
#include "../../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/
#define DRAW_UNIT_ID_DAVE2D         4
/* The amount of tasks exercising pressure to the currrent to get finished
 * This one is used as the main signal to start to render a block of tasks.
 */
#define DAVE2D_MAX_DRAW_PRESSURE    256

#if (DAVE2D_MAX_DRAW_PRESSURE < 256)
    #error "DRAW Pressure should be at least 256 otherwise the Dave engine may crash!"
#endif
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void execute_drawing(lv_draw_dave2d_unit_t * u);
#if defined(RENESAS_CORTEX_M85) || defined(_RENESAS_RZA_)
    #if (BSP_CFG_DCACHE_ENABLED) || defined(_RENESAS_RZA_)
        static void _dave2d_buf_invalidate_cache_cb(const lv_draw_buf_t * draw_buf, const lv_area_t * area);
    #endif
#endif

static int32_t _dave2d_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t _dave2d_wait_finish(lv_draw_unit_t * draw_unit);
static int32_t lv_draw_dave2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

static d2_s32 lv_dave2d_init(void);
static void lv_draw_buf_dave2d_init_handlers(void);
static bool lv_draw_dave2d_image_color_format_supported(lv_color_format_t color_format);
void dave2d_execute_dlist_and_flush(void);

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static d2_device * _d2_handle;

/* Main render buffer, used to carry the block of dave commands for any shape */
static d2_renderbuffer * _renderbuffer;

/* Label dedicated render buffer, used to carry only label related dave commands */
static d2_renderbuffer * _label_renderbuffer;

static lv_ll_t  draw_tasks_on_dlist;
static uint32_t draw_pressure = 0;

#if LV_USE_OS
    lv_mutex_t xd2Semaphore;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_dave2d_init(void)
{
    d2_s32 result = D2_OK;

    lv_draw_buf_dave2d_init_handlers();

    lv_draw_dave2d_unit_t * draw_dave2d_unit = lv_draw_create_unit(sizeof(lv_draw_dave2d_unit_t));
    draw_dave2d_unit->base_unit.dispatch_cb = lv_draw_dave2d_dispatch;
    draw_dave2d_unit->base_unit.evaluate_cb = _dave2d_evaluate;
    draw_dave2d_unit->base_unit.wait_for_finish_cb = _dave2d_wait_finish;
    draw_dave2d_unit->base_unit.name = "DAVE2D";
    draw_dave2d_unit->idx = DRAW_UNIT_ID_DAVE2D;

    result = lv_dave2d_init();
    LV_ASSERT(D2_OK == result);

#if LV_USE_OS
    lv_result_t res;
    res =  lv_mutex_init(&xd2Semaphore);
    LV_ASSERT(LV_RESULT_OK == res);
    draw_dave2d_unit->pd2Mutex    = &xd2Semaphore;
#endif

    draw_dave2d_unit->d2_handle = _d2_handle;
    draw_dave2d_unit->renderbuffer = _renderbuffer;
    draw_dave2d_unit->label_renderbuffer = _label_renderbuffer;
    lv_ll_init(&draw_tasks_on_dlist, sizeof(uintptr_t));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_draw_buf_dave2d_init_handlers(void)
{

#if defined(RENESAS_CORTEX_M85) || defined(_RENESAS_RZA_)
#if (BSP_CFG_DCACHE_ENABLED) || defined(_RENESAS_RZA_)
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    handlers->invalidate_cache_cb = _dave2d_buf_invalidate_cache_cb;
#endif
#endif
}

#if defined(RENESAS_CORTEX_M85) || defined(_RENESAS_RZA_)
#if (BSP_CFG_DCACHE_ENABLED) || defined(_RENESAS_RZA_)
static void _dave2d_buf_invalidate_cache_cb(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    const lv_image_header_t * header = &draw_buf->header;
    uint32_t stride = header->stride;
    lv_color_format_t cf = header->cf;

    uint8_t * address = draw_buf->data;
    int32_t i = 0;
    uint32_t bytes_per_pixel = lv_color_format_get_size(cf);
    int32_t width = lv_area_get_width(area);
    int32_t lines = lv_area_get_height(area);
    int32_t bytes_to_flush_per_line = (int32_t)width * (int32_t)bytes_per_pixel;

    /* Stride is in bytes, not pixels */
    address = address + (area->x1 * (int32_t)bytes_per_pixel) + (stride * (uint32_t)area->y1);

    for(i = 0; i < lines; i++) {
#if defined(RENESAS_CORTEX_M85)
        SCB_CleanInvalidateDCache_by_Addr(address, bytes_to_flush_per_line);
#else /* _RENESAS_RZA_ */
        R_BSP_CACHE_CleanInvalidateRange((uint64_t) address, (uint64_t) bytes_to_flush_per_line);
#endif
        address += stride;
    }
}
#endif
#endif

/**
 * @todo
 * LVGL needs to use hardware acceleration for buf_copy and do not affect GPU rendering.
 */
#if 0
static void _dave2d_buf_copy(void * dest_buf, uint32_t dest_w, uint32_t dest_h, const lv_area_t * dest_area,
                             void * src_buf,  uint32_t src_w, uint32_t src_h, const lv_area_t * src_area, lv_color_format_t color_format)
{
    d2_s32     result;

#if LV_USE_OS
    lv_result_t  status;

    status = lv_mutex_lock(&xd2Semaphore);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

    d2_u32 src_blend_mode = d2_getblendmodesrc(_d2_handle);
    d2_u32 dst_blend_mode = d2_getblendmodedst(_d2_handle);

    result = d2_selectrenderbuffer(_d2_handle, _blit_renderbuffer);
    LV_ASSERT(D2_OK == result);

    result = d2_setblendmode(_d2_handle, d2_bm_one, d2_bm_zero);
    LV_ASSERT(D2_OK == result);

    // Generate render operations
    result = d2_framebuffer(_d2_handle, (uint16_t *)dest_buf, DISPLAY_HSIZE_INPUT0, DISPLAY_BUFFER_STRIDE_PIXELS_INPUT0,
                            DISPLAY_VSIZE_INPUT0, lv_draw_dave2d_cf_fb_get());
    LV_ASSERT(D2_OK == result);

    result = d2_cliprect(_d2_handle, (d2_border)dest_area->x1, (d2_border)dest_area->y1, (d2_border)dest_area->x2,
                         (d2_border)dest_area->y2);
    LV_ASSERT(D2_OK == result);

    result = d2_setblitsrc(_d2_handle, (void *) src_buf, (d2_s32)src_w, (d2_s32)src_w, (d2_s32)src_h,
                           lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(color_format));
    LV_ASSERT(D2_OK == result);

    result = d2_blitcopy(_d2_handle, (d2_s32)src_w, (d2_s32)src_h, (d2_blitpos)src_area->x1, (d2_blitpos)src_area->y1,
                         D2_FIX4(dest_w), D2_FIX4(dest_h),
                         D2_FIX4(dest_area->x1), D2_FIX4(dest_area->y1), 0);
    LV_ASSERT(D2_OK == result);

    // Execute render operations
    result = d2_executerenderbuffer(_d2_handle, _blit_renderbuffer, 0);
    LV_ASSERT(D2_OK == result) ;

    result = d2_flushframe(_d2_handle);
    LV_ASSERT(D2_OK == result);

    result = d2_selectrenderbuffer(_d2_handle, _renderbuffer);
    LV_ASSERT(D2_OK == result);

    result = d2_setblendmode(_d2_handle, src_blend_mode, dst_blend_mode);
    LV_ASSERT(D2_OK != result);

#if LV_USE_OS
    status = lv_mutex_unlock(&xd2Semaphore);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

}
#endif

static bool lv_draw_dave2d_image_color_format_supported(lv_color_format_t color_format)
{
    bool supported = false;

    switch(color_format) {
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_ARGB1555:
        case LV_COLOR_FORMAT_ARGB4444:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            supported = true;
            break;
        default:
            break;
    }

    return supported;
}

#define USE_D2 (1)

static int32_t _dave2d_evaluate(lv_draw_unit_t * u, lv_draw_task_t * t)
{
    LV_UNUSED(u);
    int32_t ret = 0;

    lv_draw_dsc_base_t * draw_dsc_base = (lv_draw_dsc_base_t *) t->draw_dsc;

    if(!lv_draw_dave2d_is_dest_cf_supported(draw_dsc_base->layer->color_format))
        return 0;

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL: {
#if USE_D2
                lv_draw_fill_dsc_t * dsc = t->draw_dsc;
                if(dsc->grad.dir == LV_GRAD_DIR_NONE
                   || ((dsc->grad.dir != LV_GRAD_DIR_NONE)
                       && ((dsc->grad.stops[0].color.blue == dsc->grad.stops[dsc->grad.stops_count - 1].color.blue)
                           && (dsc->grad.stops[0].color.red   == dsc->grad.stops[dsc->grad.stops_count - 1].color.red)
                           && (dsc->grad.stops[0].color.green == dsc->grad.stops[dsc->grad.stops_count - 1].color.green)))) {

                    t->preferred_draw_unit_id = DRAW_UNIT_ID_DAVE2D;
                    t->preference_score = 0;

                }
                else
#endif
                {
                }
                ret =  0;
                break;
            }
        case LV_DRAW_TASK_TYPE_LAYER: {
                ret = 0;
                break;
            }

        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_image_dsc_t * dsc = t->draw_dsc;
                if(!lv_draw_dave2d_image_color_format_supported(dsc->header.cf)) {
                    ret = 0;
                    break;
                }
#if USE_D2
                t->preferred_draw_unit_id = DRAW_UNIT_ID_DAVE2D;
                t->preference_score = 0;
#endif
                ret = 0;
                break;
            }

        case LV_DRAW_TASK_TYPE_BORDER: {
#if USE_D2
                t->preferred_draw_unit_id = DRAW_UNIT_ID_DAVE2D;
                t->preference_score = 0;
#endif
                ret = 0;
                break;
            }

        case  LV_DRAW_TASK_TYPE_BOX_SHADOW: {
                ret = 0;
                break;
            }

        case  LV_DRAW_TASK_TYPE_LABEL: {
#if USE_D2
                t->preferred_draw_unit_id = DRAW_UNIT_ID_DAVE2D;
                t->preference_score = 0;
#endif
                ret = 0;
                break;
            }

        case LV_DRAW_TASK_TYPE_LINE: {
#if USE_D2
                t->preferred_draw_unit_id = DRAW_UNIT_ID_DAVE2D;
                t->preference_score = 0;
#endif
                ret = 0;
                break;
            }

        case  LV_DRAW_TASK_TYPE_ARC: {
#if USE_D2
                t->preferred_draw_unit_id = DRAW_UNIT_ID_DAVE2D;
                t->preference_score = 0;
#endif
                ret = 0;
                break;
            }

        case LV_DRAW_TASK_TYPE_TRIANGLE: {
#if USE_D2
                lv_draw_fill_dsc_t * dsc = t->draw_dsc;
                if(dsc->grad.dir == LV_GRAD_DIR_NONE
                   || ((dsc->grad.dir != LV_GRAD_DIR_NONE)
                       && ((dsc->grad.stops[0].color.blue == dsc->grad.stops[dsc->grad.stops_count - 1].color.blue)
                           && (dsc->grad.stops[0].color.red   == dsc->grad.stops[dsc->grad.stops_count - 1].color.red)
                           && (dsc->grad.stops[0].color.green == dsc->grad.stops[dsc->grad.stops_count - 1].color.green)))) {
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_DAVE2D;
                    t->preference_score = 0;
                }
                else {
                }
#endif
                ret = 0;
                break;
            }

        case  LV_DRAW_TASK_TYPE_MASK_RECTANGLE: {
#if 0//USE_D2
                t->preferred_draw_unit_id = DRAW_UNIT_ID_DAVE2D;
                t->preference_score = 0;
#endif
                ret = 0;
                break;
            }

        case LV_DRAW_TASK_TYPE_MASK_BITMAP: {
                ret = 0;
                break;
            }

        default:
            ret = 0;
            break;
    }

    return ret;
}


static int32_t lv_draw_dave2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_dave2d_unit_t * draw_dave2d_unit = (lv_draw_dave2d_unit_t *) draw_unit;
    uint32_t deps = 0;

    if(draw_dave2d_unit->task_act) {
        /* Return immediately if it's busy with draw task */
        return LV_DRAW_UNIT_IDLE;
    }

    lv_draw_task_t * t = NULL;
    t = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_DAVE2D);
    if(t == NULL) {
        /* No valid task, but there are tasks waiting to be rendered,
         * start to draw then immediatelly.
         */
        if(false == lv_ll_is_empty(&draw_tasks_on_dlist)) {
            draw_pressure = 0;
            dave2d_execute_dlist_and_flush();
        }
        return LV_DRAW_UNIT_IDLE;  /* This task is not for us. */
    }

    if((t->preferred_draw_unit_id != DRAW_UNIT_ID_DAVE2D)) return LV_DRAW_UNIT_IDLE;
    if(!lv_draw_dave2d_is_dest_cf_supported(layer->color_format)) return LV_DRAW_UNIT_IDLE;

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL) return LV_DRAW_UNIT_IDLE;

    deps = lv_draw_get_dependent_count(t);
    if(deps > 0 || draw_pressure > 0) {
        draw_pressure += deps;
        if(draw_pressure < DAVE2D_MAX_DRAW_PRESSURE) {
            /* No other tasks are pressuring to get the current block
             * of tasks including the latest one, just accumulate it
             * and tells the drawing pipeline to send a new one if there is any
             */
            lv_draw_task_t ** p_new_list_entry;
            p_new_list_entry = lv_ll_ins_tail(&draw_tasks_on_dlist);
            *p_new_list_entry = t;

            draw_dave2d_unit->task_act = t;
            draw_dave2d_unit->task_act->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
            execute_drawing(draw_dave2d_unit);

            draw_dave2d_unit->task_act = NULL;
            lv_draw_dispatch_request();

            return 1;
        }
        /* If the pressure for drawing value was maxed-out, it is time to render
         * return IDLE to force the drawing pipeline to wait while the Dave
         * draw the block of tasks in a single row
         */
        return LV_DRAW_UNIT_IDLE;
    }
    else {
        /* Handles a special case when there is no sufficient draw pressure
         * But the actual task did not carry any extra pressure to get drew
         * in this case, the drawing pipeline have a few set of tasks that
         * don't make sense to accumulate them, just do a run to completion
         * here, that is it, render the incoming task immediately.
         */
        draw_dave2d_unit->task_act = t;
        draw_dave2d_unit->task_act->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
        d2_selectrenderbuffer(_d2_handle, _renderbuffer);
        execute_drawing(draw_dave2d_unit);
        d2_executerenderbuffer(_d2_handle, _renderbuffer, 0);
        d2_flushframe(_d2_handle);

        draw_dave2d_unit->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
        draw_dave2d_unit->task_act = NULL;
        lv_draw_dispatch_request();

        return 1;
    }
}

static int32_t _dave2d_wait_finish(lv_draw_unit_t * draw_unit)
{
    /* If the drawing pipeline is waiting it means the pressure for drawing
     * has been maxed out, defer the block of tasks to be rendered by the
     * Dave and wait for its interrupt. (Dave2D driver is RTOS aware, no need for semaphores);
     */
    lv_draw_dave2d_unit_t * draw_dave2d_unit = (lv_draw_dave2d_unit_t *) draw_unit;

    if(!draw_pressure) {
        /* It reached here because Dave2D Draw Unit was not suitable to take a task
         * While there is nothing being rendered, prevent the dead lock
         * by flushing the GPU command buffer empty and just return.
         */
        return 0;
    }
    dave2d_execute_dlist_and_flush();
    draw_pressure = 0;

    return 0;
}

static void execute_drawing(lv_draw_dave2d_unit_t * u)
{
    /*Render the draw task*/
    lv_draw_task_t * t = u->task_act;

    /* remember draw unit for access to unit's context */
    t->draw_unit = (lv_draw_unit_t *)u;

#if defined(RENESAS_CORTEX_M85) || defined(_RENESAS_RZA_)
#if (BSP_CFG_DCACHE_ENABLED) || defined(_RENESAS_RZA_)
    lv_layer_t * layer = t->target_layer;
    lv_area_t clipped_area;
    int32_t x;
    int32_t y;

    lv_area_intersect(&clipped_area,  &t->area, &t->clip_area);

    x = 0 - t->target_layer->buf_area.x1;
    y = 0 - t->target_layer->buf_area.y1;

    lv_area_move(&clipped_area, x, y);

    /* Invalidate cache */
    lv_draw_buf_invalidate_cache(layer->draw_buf, &clipped_area);
#endif
#endif

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_dave2d_fill(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_dave2d_border(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BOX_SHADOW:
            //lv_draw_dave2d_box_shadow(t, t->draw_dsc, &t->area);
            break;
#if 0
        case LV_DRAW_TASK_TYPE_BG_IMG:
            //lv_draw_dave2d_bg_image(t, t->draw_dsc, &t->area);
            break;
#endif
        case LV_DRAW_TASK_TYPE_LABEL:
            lv_draw_dave2d_label(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_dave2d_image(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LINE:
            lv_draw_dave2d_line(t, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_ARC:
            lv_draw_dave2d_arc(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_TRIANGLE:
            lv_draw_dave2d_triangle(t, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            //lv_draw_dave2d_layer(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
            //lv_draw_dave2d_mask_rect(t, t->draw_dsc, &t->area);
            break;
        default:
            break;
    }

#if defined(RENESAS_CORTEX_M85) || defined(_RENESAS_RZA_)
#if (BSP_CFG_DCACHE_ENABLED) || defined(_RENESAS_RZA_)
    lv_draw_buf_invalidate_cache(layer->draw_buf, &clipped_area);
#endif
#endif
}

static d2_s32 lv_dave2d_init(void)
{
    d2_s32 result = D2_OK;

    if(_d2_handle != NULL) {
        return D2_NOMEMORY;
    }

    _d2_handle = d2_opendevice(0);
    if(_d2_handle == NULL) {
        return D2_NOMEMORY;
    }

    /* bind the hardware */
    result = d2_inithw(_d2_handle, 0);
    if(result != D2_OK) {
        LV_LOG_ERROR("Could NOT d2_inithw");
        d2_closedevice(_d2_handle);
        return result;
    }

    //
    // Set various D2 parameters
    //
    result = d2_setblendmode(_d2_handle, d2_bm_alpha, d2_bm_one_minus_alpha);
    result = d2_setalphamode(_d2_handle, d2_am_constant);
    result = d2_setalpha(_d2_handle, UINT8_MAX);
    result = d2_setantialiasing(_d2_handle, 1);
    result = d2_setlinecap(_d2_handle, d2_lc_butt);
    result = d2_setlinejoin(_d2_handle, d2_lj_miter);

    /* set blocksize for default displaylist */
    result = d2_setdlistblocksize(_d2_handle, 20);
    if(D2_OK != result) {
        LV_LOG_ERROR("Could NOT d2_setdlistblocksize");
        d2_closedevice(_d2_handle);
        return result;
    }

    _renderbuffer = d2_newrenderbuffer(_d2_handle, 250, 25);
    if(!_renderbuffer) {
        LV_LOG_ERROR("NO renderbuffer");
        d2_closedevice(_d2_handle);

        return D2_NOMEMORY;
    }

    _label_renderbuffer = d2_newrenderbuffer(_d2_handle, 250, 25);
    if(!_label_renderbuffer) {
        LV_LOG_ERROR("NO renderbuffer");
        d2_closedevice(_d2_handle);

        return D2_NOMEMORY;
    }

    result = d2_selectrenderbuffer(_d2_handle, _renderbuffer);
    if(D2_OK != result) {
        LV_LOG_ERROR("Could NOT d2_selectrenderbuffer");
        d2_closedevice(_d2_handle);
    }

    return result;
}

void dave2d_execute_dlist_and_flush(void)
{
    d2_s32 result;
    lv_draw_task_t ** p_list_entry;
    lv_draw_task_t * p_list_entry1;

#if LV_USE_OS
    lv_result_t  status;

    status = lv_mutex_lock(&xd2Semaphore);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

    result = d2_executerenderbuffer(_d2_handle, _renderbuffer, 0);
    LV_ASSERT(D2_OK == result);

    result = d2_flushframe(_d2_handle);
    LV_ASSERT(D2_OK == result);

    result = d2_selectrenderbuffer(_d2_handle, _renderbuffer);
    LV_ASSERT(D2_OK == result);

    while(false == lv_ll_is_empty(&draw_tasks_on_dlist)) {
        p_list_entry = lv_ll_get_tail(&draw_tasks_on_dlist);
        p_list_entry1 = *p_list_entry;
        p_list_entry1->state = LV_DRAW_TASK_STATE_FINISHED;
        lv_ll_remove(&draw_tasks_on_dlist, p_list_entry);
        lv_free(p_list_entry);
    }

#if LV_USE_OS
    status = lv_mutex_unlock(&xd2Semaphore);
    LV_ASSERT(LV_RESULT_OK == status);
#endif
}

#endif /*LV_USE_DRAW_DAVE2D*/
