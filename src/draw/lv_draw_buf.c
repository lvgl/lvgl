/**
 * @file lv_draw_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "lv_draw_buf.h"
#include "../stdlib/lv_string.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define handlers LV_GLOBAL_DEFAULT()->draw_buf_handlers

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void * buf_malloc(size_t size, lv_color_format_t color_format);
static void buf_free(void * buf);
static void * buf_align(void * buf, lv_color_format_t color_format);
static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format);
static void * buf_go_to_xy(const void * buf, uint32_t stride, lv_color_format_t color_format, int32_t x,
                           int32_t y);

static void buf_clear(void * buf, uint32_t w, uint32_t h, lv_color_format_t color_format, const lv_area_t * a);

static void buf_copy(void * dest_buf, uint32_t dest_w, uint32_t dest_h, const lv_area_t * dest_area_to_copy,
                     void * src_buf,  uint32_t src_w, uint32_t src_h, const lv_area_t * src_area_to_copy,
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

void _lv_draw_buf_init_handlers(void)
{
    lv_memzero(&handlers, sizeof(lv_draw_buf_handlers_t));
    handlers.buf_malloc_cb = buf_malloc;
    handlers.buf_free_cb = buf_free;
    handlers.align_pointer_cb = buf_align;
    handlers.invalidate_cache_cb = NULL;
    handlers.width_to_stride_cb = width_to_stride;
    handlers.go_to_xy_cb = buf_go_to_xy;
    handlers.buf_clear_cb = buf_clear;
    handlers.buf_copy_cb = buf_copy;
}

lv_draw_buf_handlers_t * lv_draw_buf_get_handlers(void)
{
    return &handlers;
}

uint32_t lv_draw_buf_width_to_stride(uint32_t w, lv_color_format_t color_format)
{
    if(handlers.width_to_stride_cb) return handlers.width_to_stride_cb(w, color_format);
    else return 0;
}

void * lv_draw_buf_malloc(size_t size_bytes, lv_color_format_t color_format)
{
    if(handlers.buf_malloc_cb) return handlers.buf_malloc_cb(size_bytes, color_format);
    else return NULL;
}

void lv_draw_buf_free(void * buf)
{
    if(handlers.buf_free_cb) handlers.buf_free_cb(buf);
}

void * lv_draw_buf_align(void * data, lv_color_format_t color_format)
{
    if(handlers.align_pointer_cb) return handlers.align_pointer_cb(data, color_format);
    else return NULL;
}

void lv_draw_buf_invalidate_cache(void * buf, uint32_t stride, lv_color_format_t color_format, const lv_area_t * area)
{
    if(handlers.invalidate_cache_cb) handlers.invalidate_cache_cb(buf, stride, color_format, area);
}

void * lv_draw_buf_go_to_xy(const void * buf, uint32_t stride, lv_color_format_t color_format, int32_t x,
                            int32_t y)
{
    if(handlers.go_to_xy_cb) return handlers.go_to_xy_cb(buf, stride, color_format, x, y);
    else return NULL;
}

void lv_draw_buf_clear(void * buf, uint32_t w, uint32_t h, lv_color_format_t color_format, const lv_area_t * a)
{
    if(handlers.buf_clear_cb) handlers.buf_clear_cb(buf, w, h, color_format, a);
}

void lv_draw_buf_copy(void * dest_buf, uint32_t dest_w, uint32_t dest_h, const lv_area_t * dest_area_to_copy,
                      void * src_buf,  uint32_t src_w, uint32_t src_h, const lv_area_t * src_area_to_copy,
                      lv_color_format_t color_format)
{
    if(handlers.buf_copy_cb) handlers.buf_copy_cb(dest_buf, dest_w, dest_h, dest_area_to_copy,
                                                      src_buf, src_w, src_h, src_area_to_copy,
                                                      color_format);
}

lv_draw_buf_t * lv_draw_buf_create(uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride)
{
    uint32_t size;
    lv_draw_buf_t * draw_buf = lv_malloc_zeroed(sizeof(lv_draw_buf_t));
    LV_ASSERT_MALLOC(draw_buf);
    if(draw_buf == NULL) return NULL;
    if(stride == 0) stride = lv_draw_buf_width_to_stride(w, cf);

    size = stride * h;
    if(cf == LV_COLOR_FORMAT_RGB565A8) {
        size += (stride / 2) * h; /*A8 mask*/
    }
    else if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        /*@todo we have to include palette right before image data*/
        size += LV_COLOR_INDEXED_PALETTE_SIZE(cf) * 4;
    }

    /*RLE decompression operates on pixel unit, thus add padding to make sure memory is enough*/
    uint8_t bpp = lv_color_format_get_bpp(cf);
    bpp = (bpp + 7) >> 3;
    size += bpp;

    void * buf = lv_draw_buf_malloc(size, cf);
    LV_ASSERT_MALLOC(buf);
    if(buf == NULL) {
        lv_free(draw_buf);
        return NULL;
    }

    draw_buf->header.w = w;
    draw_buf->header.h = h;
    draw_buf->header.cf = cf;
    draw_buf->header.flags = LV_IMAGE_FLAGS_MODIFIABLE;
    draw_buf->header.stride = stride;
    draw_buf->data = lv_draw_buf_align(buf, cf);
    draw_buf->_unaligned = buf;
    draw_buf->data_size = size;
    return draw_buf;
}

void lv_draw_buf_destroy(lv_draw_buf_t * buf)
{
    LV_ASSERT_NULL(buf);
    if(buf == NULL) return;
    if(buf->header.flags & LV_IMAGE_FLAGS_MODIFIABLE)
        lv_draw_buf_free(buf->_unaligned);

    lv_free(buf);
}

void * lv_draw_buf_goto_xy(lv_draw_buf_t * buf, uint32_t x, uint32_t y)
{
    LV_ASSERT_NULL(buf);
    if(buf == NULL) return NULL;

    uint8_t * data = buf->data;
    data += buf->header.stride * y;

    if(x == 0)
        return data;

    return data + x * lv_color_format_get_size(buf->header.cf);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * buf_malloc(size_t size_bytes, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);

    /*Allocate larger memory to be sure it can be aligned as needed*/
    size_bytes += LV_DRAW_BUF_ALIGN - 1;
    return lv_malloc(size_bytes);
}

static void buf_free(void * buf)
{
    lv_free(buf);
}

static void * buf_align(void * buf, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);

    uint8_t * buf_u8 = buf;
    if(buf_u8) {
        buf_u8 += LV_DRAW_BUF_ALIGN - 1;
        buf_u8 = (uint8_t *)((lv_uintptr_t) buf_u8 & ~(LV_DRAW_BUF_ALIGN - 1));
    }
    return buf_u8;
}

static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format)
{
    uint32_t width_byte;
    width_byte = w * lv_color_format_get_bpp(color_format);
    width_byte = (width_byte + 7) >> 3; /*Round up*/
    return (width_byte + LV_DRAW_BUF_STRIDE_ALIGN - 1) & ~(LV_DRAW_BUF_STRIDE_ALIGN - 1);
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

    LV_UNUSED(h);

    uint8_t px_size = lv_color_format_get_size(color_format);
    uint32_t stride = lv_draw_buf_width_to_stride(w, color_format);
    uint8_t * bufc =  buf;

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
                     void * src_buf,  uint32_t src_w, uint32_t src_h, const lv_area_t * src_area_to_copy,
                     lv_color_format_t color_format)
{
    LV_UNUSED(dest_h);
    LV_UNUSED(src_h);

    uint8_t px_size = lv_color_format_get_size(color_format);
    uint8_t * dest_bufc =  dest_buf;
    uint8_t * src_bufc =  src_buf;

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
