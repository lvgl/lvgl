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
#define DRAW_UNIT_ID_DAVE2D     4

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_USE_OS
    static void _dave2d_render_thread_cb(void * ptr);
#endif

static void execute_drawing(lv_draw_dave2d_unit_t * u);

#if defined(RENESAS_CORTEX_M85)
    #if (BSP_CFG_DCACHE_ENABLED)
        static void _dave2d_buf_invalidate_cache_cb(const lv_draw_buf_t * draw_buf, const lv_area_t * area);
    #endif
#endif

static int32_t _dave2d_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);

static int32_t lv_draw_dave2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

static d2_s32 lv_dave2d_init(void);

static void lv_draw_buf_dave2d_init_handlers(void);

void dave2d_execute_dlist_and_flush(void);

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

d2_device * _d2_handle;
d2_renderbuffer * _renderbuffer;
d2_renderbuffer * _blit_renderbuffer;

lv_ll_t  _ll_Dave2D_Tasks;

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
    lv_ll_init(&_ll_Dave2D_Tasks, 4);

#if LV_USE_OS
    lv_thread_init(&draw_dave2d_unit->thread, "dave2d", LV_DRAW_THREAD_PRIO, _dave2d_render_thread_cb, 8 * 1024,
                   draw_dave2d_unit);
#endif

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_draw_buf_dave2d_init_handlers(void)
{

#if defined(RENESAS_CORTEX_M85)
#if (BSP_CFG_DCACHE_ENABLED)
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    handlers->invalidate_cache_cb = _dave2d_buf_invalidate_cache_cb;
#endif
#endif
}

#if defined(RENESAS_CORTEX_M85)
#if (BSP_CFG_DCACHE_ENABLED)
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
        SCB_CleanInvalidateDCache_by_Addr(address, bytes_to_flush_per_line);
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
                if((dsc->header.cf >= LV_COLOR_FORMAT_PROPRIETARY_START) || (dsc->header.cf == LV_COLOR_FORMAT_RGB888) ||
                   (dsc->header.cf == LV_COLOR_FORMAT_RGB565A8)) {
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

#define DAVE2D_REFERRING_WATERMARK  10

static int32_t lv_draw_dave2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_dave2d_unit_t * draw_dave2d_unit = (lv_draw_dave2d_unit_t *) draw_unit;
#if  (0 == D2_RENDER_EACH_OPERATION)
    static uint32_t ref_count = 0;
#endif

    /*Return immediately if it's busy with draw task*/
    if(draw_dave2d_unit->task_act) return 0;

    lv_draw_task_t * t = NULL;
    t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_DAVE2D);
    while(t && t->preferred_draw_unit_id != DRAW_UNIT_ID_DAVE2D) {
        t->state = LV_DRAW_TASK_STATE_FINISHED;
        t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_DAVE2D);
    }

    if(t == NULL) {
#if  (0 == D2_RENDER_EACH_OPERATION)
        if(false == lv_ll_is_empty(&_ll_Dave2D_Tasks)) {
            ref_count = 0;
            dave2d_execute_dlist_and_flush();
        }
#endif
        return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
    }

    /* Return if target buffer format is not supported. */
    if(!lv_draw_dave2d_is_dest_cf_supported(layer->color_format)) {
        return LV_DRAW_UNIT_IDLE; /*Couldn't start rendering*/
    }

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL) {
        return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
    }

#if  (0 == D2_RENDER_EACH_OPERATION)
    ref_count += lv_draw_get_dependent_count(t);

    if(DAVE2D_REFERRING_WATERMARK < ref_count) {
        ref_count = 0;
        dave2d_execute_dlist_and_flush();
    }

    lv_draw_task_t ** p_new_list_entry;
    p_new_list_entry = lv_ll_ins_tail(&_ll_Dave2D_Tasks);
    *p_new_list_entry = t;
#endif

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_dave2d_unit->task_act = t;

#if LV_USE_OS
    /*Let the render thread work*/
    lv_thread_sync_signal(&draw_dave2d_unit->sync);
#else
    execute_drawing(draw_dave2d_unit);
#if  (D2_RENDER_EACH_OPERATION)
    draw_dave2d_unit->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
#endif
    draw_dave2d_unit->task_act = NULL;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();

#endif
    return 1;
}

#if LV_USE_OS
static void _dave2d_render_thread_cb(void * ptr)
{
    lv_draw_dave2d_unit_t * u = ptr;

    lv_thread_sync_init(&u->sync);

    while(1) {
        while(u->task_act == NULL) {
            lv_thread_sync_wait(&u->sync);
        }

        execute_drawing(u);

        /*Cleanup*/
#if  (D2_RENDER_EACH_OPERATION)
        u->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
#endif
        u->task_act = NULL;

        /*The draw unit is free now. Request a new dispatching as it can get a new task*/
        lv_draw_dispatch_request();
    }
}
#endif

static void execute_drawing(lv_draw_dave2d_unit_t * u)
{
    /*Render the draw task*/
    lv_draw_task_t * t = u->task_act;

    /* remember draw unit for access to unit's context */
    t->draw_unit = (lv_draw_unit_t *)u;

#if defined(RENESAS_CORTEX_M85)
#if (BSP_CFG_DCACHE_ENABLED)
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
    result = d2_setdlistblocksize(_d2_handle, 25);
    if(D2_OK != result) {
        LV_LOG_ERROR("Could NOT d2_setdlistblocksize");
        d2_closedevice(_d2_handle);
        return result;
    }

    _blit_renderbuffer = d2_newrenderbuffer(_d2_handle, 20, 20);
    if(!_blit_renderbuffer) {
        LV_LOG_ERROR("NO renderbuffer");
        d2_closedevice(_d2_handle);

        return D2_NOMEMORY;
    }

    _renderbuffer = d2_newrenderbuffer(_d2_handle, 20, 20);
    if(!_renderbuffer) {
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
#if LV_USE_OS
    lv_result_t  status;

    status = lv_mutex_lock(&xd2Semaphore);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

    d2_s32     result;
    lv_draw_task_t ** p_list_entry;
    lv_draw_task_t * p_list_entry1;

    // Execute render operations
    result = d2_executerenderbuffer(_d2_handle, _renderbuffer, 0);
    LV_ASSERT(D2_OK == result);

    result = d2_flushframe(_d2_handle);
    LV_ASSERT(D2_OK == result);

    result = d2_selectrenderbuffer(_d2_handle, _renderbuffer);
    LV_ASSERT(D2_OK == result);

    while(false == lv_ll_is_empty(&_ll_Dave2D_Tasks)) {
        p_list_entry = lv_ll_get_tail(&_ll_Dave2D_Tasks);
        p_list_entry1 = *p_list_entry;
        p_list_entry1->state = LV_DRAW_TASK_STATE_FINISHED;
        lv_ll_remove(&_ll_Dave2D_Tasks, p_list_entry);
        lv_free(p_list_entry);
    }

#if LV_USE_OS
    status = lv_mutex_unlock(&xd2Semaphore);
    LV_ASSERT(LV_RESULT_OK == status);
#endif
}

#endif /*LV_USE_DRAW_DAVE2D*/
