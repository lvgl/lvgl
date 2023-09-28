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
static void buf_init(lv_draw_buf_t * draw_buf, lv_coord_t w, lv_coord_t h, lv_color_format_t color_format);
static void * buf_malloc(size_t size, lv_color_format_t color_format);
static void buf_free(void * buf);
static void * align_buf(void * buf, lv_color_format_t color_format);
static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format);
static void * go_to_xy(lv_draw_buf_t * draw_buf, lv_coord_t x, lv_coord_t y);
static void buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * a);
static void buf_copy(void * dest_buf, uint32_t dest_stride, const lv_area_t * dest_area,
                     void * src_buf, uint32_t src_stride, const lv_area_t * src_area, lv_color_format_t color_format);

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
    handlers.init_cb = buf_init;
    handlers.buf_malloc_cb = buf_malloc;
    handlers.buf_free_cb = buf_free;
    handlers.align_pointer_cb = align_buf;
    handlers.invalidate_cache_cb = NULL;
    handlers.width_to_stride_cb = width_to_stride;
    handlers.go_to_xy_cb = go_to_xy;
    handlers.buf_clear_cb = buf_clear;
    handlers.buf_copy_cb = buf_copy;
}

lv_draw_buf_handlers_t * lv_draw_buf_get_handlers(void)
{
    return &handlers;
}

void lv_draw_buf_init(lv_draw_buf_t * draw_buf, lv_coord_t w, lv_coord_t h, lv_color_format_t color_format)
{
    lv_memzero(draw_buf, sizeof(lv_draw_buf_t));
    if(handlers.init_cb) handlers.init_cb(draw_buf, w, h, color_format);
}

void lv_draw_buf_init_alloc(lv_draw_buf_t * draw_buf, lv_coord_t w, lv_coord_t h, lv_color_format_t color_format)
{
    lv_draw_buf_init(draw_buf, w, h, color_format);

    size_t s = lv_draw_buf_get_stride(draw_buf) * h;
    draw_buf->buf = lv_draw_buf_malloc(s, color_format);
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

void * lv_draw_buf_align_buf(void * data, lv_color_format_t color_format)
{
    if(handlers.align_pointer_cb) return handlers.align_pointer_cb(data, color_format);
    else return NULL;
}

void lv_draw_buf_invalidate_cache(lv_draw_buf_t  * draw_buf, const char * area)
{
    if(handlers.invalidate_cache_cb) handlers.invalidate_cache_cb(draw_buf, area);
}

void * lv_draw_buf_go_to_xy(lv_draw_buf_t * draw_buf, lv_coord_t x, lv_coord_t y)
{
    if(handlers.go_to_xy_cb) return handlers.go_to_xy_cb(draw_buf, x, y);
    else return NULL;
}

void lv_draw_buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * a)
{
    if(handlers.buf_clear_cb) handlers.buf_clear_cb(draw_buf, a);
}

void lv_draw_buf_copy(void * dest_buf, uint32_t dest_stride, const lv_area_t * dest_area,
                      void * src_buf, uint32_t src_stride, const lv_area_t * src_area, lv_color_format_t color_format)
{
    if(handlers.buf_copy_cb) handlers.buf_copy_cb(dest_buf, dest_stride, dest_area, src_buf, src_stride, src_area,
                                                      color_format);
}

void * lv_draw_buf_get_buf(lv_draw_buf_t * draw_buf)
{
    if(draw_buf->buf) return lv_draw_buf_align_buf(draw_buf->buf, draw_buf->color_format);
    else return NULL;
}


uint32_t lv_draw_buf_get_stride(const lv_draw_buf_t * draw_buf)
{
    return lv_draw_buf_width_to_stride(draw_buf->width, draw_buf->color_format);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/


static void buf_init(lv_draw_buf_t * draw_buf, lv_coord_t w, lv_coord_t h, lv_color_format_t color_format)
{
    draw_buf->width = w;
    draw_buf->height = h;
    draw_buf->color_format = color_format;
    draw_buf->buf = NULL;
}

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

static void * align_buf(void * buf, lv_color_format_t color_format)
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
    width_byte =  w * lv_color_format_get_size(color_format);
    return (width_byte + LV_DRAW_BUF_STRIDE_ALIGN - 1) & ~(LV_DRAW_BUF_STRIDE_ALIGN - 1);
}

static void * go_to_xy(lv_draw_buf_t * draw_buf, lv_coord_t x, lv_coord_t y)
{
    uint32_t px_size = lv_color_format_get_size(draw_buf->color_format);
    uint32_t stride = lv_draw_buf_get_stride(draw_buf);
    uint8_t * buf_tmp = lv_draw_buf_get_buf(draw_buf);
    buf_tmp += stride * y;
    buf_tmp += x * px_size;

    return buf_tmp;
}

static void buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * a)
{
    //TODO clear the area
    LV_UNUSED(a);
    uint32_t stride = lv_draw_buf_get_stride(draw_buf);
    uint8_t * buf = lv_draw_buf_get_buf(draw_buf);
    lv_memzero(buf, stride * draw_buf->height);
}

static void buf_copy(void * dest_buf, uint32_t dest_stride, const lv_area_t * dest_area,
                     void * src_buf, uint32_t src_stride, const lv_area_t * src_area, lv_color_format_t color_format)
{
    uint8_t px_size = lv_color_format_get_size(color_format);
    uint8_t * dest_bufc =  dest_buf;
    uint8_t * src_bufc =  src_buf;

    /*Got the first pixel of each buffer*/
    dest_bufc += dest_stride * dest_area->y1;
    dest_bufc += dest_area->x1 * px_size;

    src_bufc += src_stride * src_area->y1;
    src_bufc += src_area->x1 * px_size;

    uint32_t line_length = lv_area_get_width(dest_area) * px_size;
    lv_coord_t y;
    for(y = dest_area->y1; y <= dest_area->y2; y++) {
        lv_memcpy(dest_bufc, src_bufc, line_length);
        dest_bufc += dest_stride;
        src_bufc += src_stride;
    }
}

