/**
 * @file lv_draw_dave2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_dave2d.h"
#include "../../sw/lv_draw_sw_private.h"
#if LV_USE_DRAW_DAVE2D
#include "../../lv_draw_buf_private.h"
#include "../../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_begin(lv_layer_t * layer, const lv_area_t * clip_area);
static void draw_end(lv_layer_t * layer, const lv_area_t * clip_area);
#if defined(RENESAS_CORTEX_M85) || defined(_RENESAS_RZA_)
    #if (BSP_CFG_DCACHE_ENABLED) || defined(_RENESAS_RZA_)
        static void _dave2d_buf_invalidate_cache_cb(const lv_draw_buf_t * draw_buf, const lv_area_t * area);
    #endif
#endif

static bool _dave2d_grad_supported(const lv_grad_dsc_t * grad);
static bool _dave2d_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc);
static bool _dave2d_can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc);
static bool _dave2d_can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc);
static bool _dave2d_can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool _dave2d_can_draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc);
static bool _dave2d_can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc);
static bool _dave2d_can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer,
                                      const lv_draw_triangle_dsc_t * dsc);
static int32_t _dave2d_delete(lv_draw_unit_t * draw_unit);

static d2_s32 lv_dave2d_init(void);
static void lv_dave2d_release(bool hw_bound);
static void lv_draw_buf_dave2d_init_handlers(void);
static bool lv_draw_dave2d_image_color_format_supported(lv_color_format_t color_format);

static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);
static void draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);
static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
static void draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);
static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc);
static void draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_triangle_dsc_t * dsc);

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_draw_unit_draw_cbs_t draw_cbs = {
    .draw_fill_cb = draw_fill,
    .can_draw_fill_cb = _dave2d_can_draw_fill,
    .draw_border_cb = draw_border,
    .can_draw_border_cb = _dave2d_can_draw_border,
    .draw_label_cb = draw_label,
    .can_draw_label_cb = _dave2d_can_draw_label,
    .draw_image_cb = draw_image,
    .can_draw_image_cb = _dave2d_can_draw_image,
    .draw_arc_cb = draw_arc,
    .can_draw_arc_cb = _dave2d_can_draw_arc,
    .draw_line_cb = draw_line,
    .can_draw_line_cb = _dave2d_can_draw_line,
    .draw_triangle_cb = draw_triangle,
    .can_draw_triangle_cb = _dave2d_can_draw_triangle,
};

static d2_device * _d2_handle;

/* Main render buffer, used to carry the block of dave commands for any shape */
static d2_renderbuffer * _renderbuffer;

/* Label dedicated render buffer, used to carry only label related dave commands */
static d2_renderbuffer * _label_renderbuffer;

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
    draw_dave2d_unit->base_unit.delete_cb = _dave2d_delete;
    draw_dave2d_unit->base_unit.name = "DAVE2D";
    draw_dave2d_unit->base_unit.draw_cbs = &draw_cbs;
    draw_dave2d_unit->base_unit.fallback = lv_draw_sw_get_draw_unit();

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

/* Gradients are drawn only if they have one color */
static bool _dave2d_grad_supported(const lv_grad_dsc_t * grad)
{
    if(grad->dir == LV_GRAD_DIR_NONE) return true;

    lv_color_t first = grad->stops[0].color;
    lv_color_t last = grad->stops[grad->stops_count - 1].color;
    return first.blue == last.blue && first.red == last.red && first.green == last.green;
}

static bool _dave2d_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    return lv_draw_dave2d_is_dest_cf_supported(layer->color_format) && _dave2d_grad_supported(&dsc->grad);
}

static bool _dave2d_can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return lv_draw_dave2d_is_dest_cf_supported(layer->color_format);
}

static bool _dave2d_can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return lv_draw_dave2d_is_dest_cf_supported(layer->color_format);
}

static bool _dave2d_can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    return lv_draw_dave2d_is_dest_cf_supported(layer->color_format) &&
           lv_draw_dave2d_image_color_format_supported(dsc->header.cf);
}

static bool _dave2d_can_draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return lv_draw_dave2d_is_dest_cf_supported(layer->color_format);
}

static bool _dave2d_can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return lv_draw_dave2d_is_dest_cf_supported(layer->color_format);
}

static bool _dave2d_can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer,
                                      const lv_draw_triangle_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    return lv_draw_dave2d_is_dest_cf_supported(layer->color_format) && _dave2d_grad_supported(&dsc->grad);
}


static int32_t _dave2d_delete(lv_draw_unit_t * draw_unit)
{
    LV_UNUSED(draw_unit);

    if(_d2_handle != NULL) {
        lv_dave2d_release(true);
    }

#if LV_USE_OS
    lv_mutex_delete(&xd2Semaphore);
#endif

    return 0;
}

/**
 * Invalidate the CPU cache of the clip area (where the D-cache is on) and select the main render buffer
 */
static void draw_begin(lv_layer_t * layer, const lv_area_t * clip_area)
{
#if defined(RENESAS_CORTEX_M85) || defined(_RENESAS_RZA_)
#if (BSP_CFG_DCACHE_ENABLED) || defined(_RENESAS_RZA_)
    lv_area_t buf_area = *clip_area;
    lv_area_move(&buf_area, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_draw_buf_invalidate_cache(layer->draw_buf, &buf_area);
#else
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
#endif
#else
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
#endif

    d2_selectrenderbuffer(_d2_handle, _renderbuffer);
}

/**
 * Render the commands and wait for the GPU, then invalidate the CPU cache of the clip area again
 */
static void draw_end(lv_layer_t * layer, const lv_area_t * clip_area)
{
    d2_executerenderbuffer(_d2_handle, _renderbuffer, 0);
    d2_flushframe(_d2_handle);

#if defined(RENESAS_CORTEX_M85) || defined(_RENESAS_RZA_)
#if (BSP_CFG_DCACHE_ENABLED) || defined(_RENESAS_RZA_)
    lv_area_t buf_area = *clip_area;
    lv_area_move(&buf_area, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_draw_buf_invalidate_cache(layer->draw_buf, &buf_area);
#else
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
#endif
#else
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
#endif
}

static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin(layer, clip_area);
    lv_draw_dave2d_fill(draw_unit, layer, clip_area, dsc, coords);
    draw_end(layer, clip_area);
}

static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin(layer, clip_area);
    lv_draw_dave2d_border(draw_unit, layer, clip_area, dsc, coords);
    draw_end(layer, clip_area);
}

static void draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin(layer, clip_area);
    lv_draw_dave2d_label(draw_unit, layer, clip_area, dsc, coords);
    draw_end(layer, clip_area);
}

static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin(layer, clip_area);
    lv_draw_dave2d_image(draw_unit, layer, clip_area, dsc, coords);
    draw_end(layer, clip_area);
}

static void draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin(layer, clip_area);
    lv_draw_dave2d_arc(draw_unit, layer, clip_area, dsc, coords);
    draw_end(layer, clip_area);
}

static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc)
{
    draw_begin(layer, clip_area);
    lv_draw_line_iterate(layer, clip_area, dsc, lv_draw_dave2d_line, draw_unit);
    draw_end(layer, clip_area);
}

static void draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_triangle_dsc_t * dsc)
{
    draw_begin(layer, clip_area);
    lv_draw_dave2d_triangle(draw_unit, layer, clip_area, dsc);
    draw_end(layer, clip_area);
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
        lv_dave2d_release(false);
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
        lv_dave2d_release(true);
        return result;
    }

    _renderbuffer = d2_newrenderbuffer(_d2_handle, 250, 25);
    if(!_renderbuffer) {
        LV_LOG_ERROR("NO renderbuffer");
        lv_dave2d_release(true);

        return D2_NOMEMORY;
    }

    _label_renderbuffer = d2_newrenderbuffer(_d2_handle, 250, 25);
    if(!_label_renderbuffer) {
        LV_LOG_ERROR("NO renderbuffer");
        lv_dave2d_release(true);

        return D2_NOMEMORY;
    }

    result = d2_selectrenderbuffer(_d2_handle, _renderbuffer);
    if(D2_OK != result) {
        LV_LOG_ERROR("Could NOT d2_selectrenderbuffer");
        lv_dave2d_release(true);
    }

    return result;
}

/* Free the render buffers and close the device opened by `lv_dave2d_init`, so it can run again.
 * `hw_bound`: `d2_inithw` succeeded and needs `d2_deinithw` */
static void lv_dave2d_release(bool hw_bound)
{
    if(_label_renderbuffer != NULL) {
        d2_freerenderbuffer(_d2_handle, _label_renderbuffer);
        _label_renderbuffer = NULL;
    }
    if(_renderbuffer != NULL) {
        d2_freerenderbuffer(_d2_handle, _renderbuffer);
        _renderbuffer = NULL;
    }
    if(hw_bound) {
        d2_deinithw(_d2_handle);
    }
    d2_closedevice(_d2_handle);
    _d2_handle = NULL;
}

#endif /*LV_USE_DRAW_DAVE2D*/
