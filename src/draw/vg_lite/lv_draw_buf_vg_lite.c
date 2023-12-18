/**
 * @file lv_draw_buf_vg_lite.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_utils.h"
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

static void * buf_malloc(size_t size, lv_color_format_t color_format);
static void buf_free(void * buf);
static void * buf_align(void * buf, lv_color_format_t color_format);
static void invalidate_cache(void * buf, uint32_t stride, lv_color_format_t color_format, const lv_area_t * area);
static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format);
static void * buf_go_to_xy(const void * buf, uint32_t stride, lv_color_format_t color_format, int32_t x,
                           int32_t y);
static void buf_clear(void * buf, uint32_t w, uint32_t h, lv_color_format_t color_format, const lv_area_t * a);
static void buf_copy(void * dest_buf, uint32_t dest_w, uint32_t dest_h, const lv_area_t * dest_area_to_copy,
                     void * src_buf, uint32_t src_w, uint32_t src_h, const lv_area_t * src_area_to_copy,
                     lv_color_format_t color_format);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_buf_vg_lite_init_handlers(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();

    handlers->buf_malloc_cb = buf_malloc;
    handlers->buf_free_cb = buf_free;
    handlers->align_pointer_cb = buf_align;
    handlers->invalidate_cache_cb = invalidate_cache;
    handlers->width_to_stride_cb = width_to_stride;
    handlers->go_to_xy_cb = buf_go_to_xy;
    handlers->buf_clear_cb = buf_clear;
    handlers->buf_copy_cb = buf_copy;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * buf_malloc(size_t size_bytes, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);
    size_bytes = LV_VG_LITE_ALIGN(size_bytes, LV_VG_LITE_BUF_ALIGN);
    return aligned_alloc(LV_VG_LITE_BUF_ALIGN, size_bytes);
}

static void buf_free(void * buf)
{
    free(buf);
}

static void * buf_align(void * buf, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);
    return (void *)LV_VG_LITE_ALIGN((lv_uintptr_t)buf, LV_VG_LITE_BUF_ALIGN);
}

static void invalidate_cache(void * buf, uint32_t stride, lv_color_format_t color_format, const lv_area_t * area)
{
}

static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format)
{
    return lv_vg_lite_width_to_stride(w, lv_vg_lite_vg_fmt(color_format));
}

static void * buf_go_to_xy(const void * buf, uint32_t stride, lv_color_format_t color_format, int32_t x,
                           int32_t y)
{
    const uint8_t * buf_tmp = buf;
    buf_tmp += stride * y;
    buf_tmp += x * lv_color_format_get_size(color_format);

    return (void *)buf_tmp;
}

static void buf_clear(void * buf, uint32_t w, uint32_t h, lv_color_format_t color_format, const lv_area_t * a)
{
#if 0
    if(LV_VG_LITE_IS_ALIGNED(buf, LV_VG_LITE_BUF_ALIGN)) {
        /* finish outstanding buffers */
        LV_VG_LITE_CHECK_ERROR(vg_lite_finish());

        vg_lite_buffer_t dest_buf;
        LV_ASSERT(lv_vg_lite_buffer_init(&dest_buf, buf, w, h, lv_vg_lite_vg_fmt(color_format), false));
        LV_VG_LITE_ASSERT_DEST_BUFFER(&dest_buf);

        vg_lite_rectangle_t rect;
        lv_vg_lite_rect(&rect, a);
        LV_VG_LITE_CHECK_ERROR(vg_lite_clear(&dest_buf, &rect, 0));
        LV_VG_LITE_CHECK_ERROR(vg_lite_finish());
        return;
    }
#endif

    uint8_t px_size = lv_color_format_get_size(color_format);
    uint32_t stride = lv_draw_buf_width_to_stride(w, color_format);
    uint8_t * bufc = buf;

    /*Got the first pixel of each buffer*/
    bufc += stride * a->y1;
    bufc += a->x1 * px_size;

    uint32_t line_length = lv_area_get_width(a) * px_size;
    int32_t y;
    for(y = a->y1; y <= a->y2; y++) {
        lv_memzero(bufc, line_length);
        bufc += stride;
    }
}

static void buf_copy(void * dest_buf, uint32_t dest_w, uint32_t dest_h, const lv_area_t * dest_area_to_copy,
                     void * src_buf, uint32_t src_w, uint32_t src_h, const lv_area_t * src_area_to_copy,
                     lv_color_format_t color_format)
{
#if 0
    if(LV_VG_LITE_IS_ALIGNED(dest_buf, LV_VG_LITE_BUF_ALIGN)
       && LV_VG_LITE_IS_ALIGNED(src_buf, LV_VG_LITE_BUF_ALIGN)) {
        vg_lite_buffer_t dest;
        LV_ASSERT(lv_vg_lite_buffer_init(&dest, dest_buf, dest_w, dest_h, lv_vg_lite_vg_fmt(color_format), false));
        LV_VG_LITE_ASSERT_DEST_BUFFER(&dest);

        vg_lite_buffer_t src;
        LV_ASSERT(lv_vg_lite_buffer_init(&src, src_buf, src_w, src_h, lv_vg_lite_vg_fmt(color_format), false));
        LV_VG_LITE_ASSERT_SRC_BUFFER(&src);

        vg_lite_rectangle_t src_rect;
        lv_vg_lite_rect(&src_rect, src_area_to_copy);

        vg_lite_matrix_t matrix;
        vg_lite_identity(&matrix);

        LV_VG_LITE_CHECK_ERROR(vg_lite_blit_rect(&dest, &src,
                                                 &src_rect,
                                                 &matrix,
                                                 VG_LITE_BLEND_NONE, 0,
                                                 VG_LITE_FILTER_POINT));
        LV_VG_LITE_CHECK_ERROR(vg_lite_finish());
        return;
    }
#endif

    uint8_t px_size = lv_color_format_get_size(color_format);
    uint8_t * dest_bufc = dest_buf;
    uint8_t * src_bufc = src_buf;

    uint32_t dest_stride = lv_draw_buf_width_to_stride(dest_w, color_format);
    uint32_t src_stride = lv_draw_buf_width_to_stride(src_w, color_format);

    /*Got the first pixel of each buffer*/
    dest_bufc += dest_stride * dest_area_to_copy->y1;
    dest_bufc += dest_area_to_copy->x1 * px_size;

    src_bufc += src_stride * src_area_to_copy->y1;
    src_bufc += src_area_to_copy->x1 * px_size;

    uint32_t line_length = lv_area_get_width(dest_area_to_copy) * px_size;
    int32_t y;
    for(y = dest_area_to_copy->y1; y <= dest_area_to_copy->y2; y++) {
        lv_memcpy(dest_bufc, src_bufc, line_length);
        dest_bufc += dest_stride;
        src_bufc += src_stride;
    }
}

#endif /*LV_USE_DRAW_VG_LITE*/
