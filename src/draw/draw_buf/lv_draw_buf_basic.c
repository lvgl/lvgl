/**
 * @file lv_draw_buf_basic.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_types.h"
#include "lv_draw_buf.h"
#if LV_USE_DRAW_BUF == LV_DRAW_BUF_BASIC

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
static uint8_t * buf_alloc(void * old_buf, lv_coord_t w, lv_coord_t h);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_buf_init(lv_draw_buf_t * draw_buf, lv_coord_t w, lv_coord_t h, lv_color_format_t color_format)
{
    draw_buf->width = w;
    draw_buf->height = h;
    draw_buf->color_format = color_format;
    draw_buf->buf = NULL;
}

void lv_draw_buf_malloc(lv_draw_buf_t * draw_buf)
{
    if(draw_buf->width != 0 ||
       draw_buf->height != 0) draw_buf->buf = buf_alloc(NULL, lv_draw_buf_get_stride(draw_buf), draw_buf->height);
    else draw_buf->buf = NULL;
}

void lv_draw_buf_realloc(lv_draw_buf_t  * draw_buf, lv_coord_t w, lv_coord_t h,
                         lv_color_format_t color_format)
{
    draw_buf->width = w;
    draw_buf->height = h;
    draw_buf->color_format = color_format;
    draw_buf->buf = buf_alloc(draw_buf->buf, lv_draw_buf_get_stride(draw_buf), h);
}

void lv_draw_buf_free(lv_draw_buf_t * draw_buf)
{
    lv_free(draw_buf->buf);
}

void * lv_draw_buf_get_buf(lv_draw_buf_t * draw_buf)
{
    uint8_t * buf = draw_buf->buf;
    buf += LV_DRAW_BUF_ALIGN - 1;
    buf = (uint8_t *)((lv_uintptr_t) buf & ~(LV_DRAW_BUF_ALIGN - 1));
    return buf;
}

void lv_draw_buf_invalidate_cache(lv_draw_buf_t  * draw_buf)
{
    LV_UNUSED(draw_buf);
}

uint32_t lv_draw_buf_width_to_stride(uint32_t w, lv_color_format_t color_format)
{
    uint32_t width_byte =  w * lv_color_format_get_size(color_format);
    return (width_byte + LV_DRAW_BUF_STRIDE_ALIGN - 1) & ~(LV_DRAW_BUF_STRIDE_ALIGN - 1);
}

uint32_t lv_draw_buf_get_stride(const lv_draw_buf_t * draw_buf)
{
    return lv_draw_buf_width_to_stride(draw_buf->width, draw_buf->color_format);
}

void * lv_draw_buf_go_to_xy(lv_draw_buf_t * draw_buf, lv_coord_t x, lv_coord_t y)
{
    uint32_t px_size = lv_color_format_get_size(draw_buf->color_format);
    uint32_t stride = lv_draw_buf_get_stride(draw_buf);
    uint8_t * buf_tmp = lv_draw_buf_get_buf(draw_buf);
    buf_tmp += stride * y;
    buf_tmp += x * px_size;

    return buf_tmp;
}

void lv_draw_buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * a)
{
    //TODO clear the area
    LV_UNUSED(a);
    uint32_t stride = lv_draw_buf_get_stride(draw_buf);
    uint8_t * buf = lv_draw_buf_get_buf(draw_buf);
    lv_memzero(buf, stride * draw_buf->height);
}

void lv_draw_buf_copy(void * dest_buf, uint32_t dest_stride, const lv_area_t * dest_area,
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

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint8_t * buf_alloc(void * old_buf, lv_coord_t w, lv_coord_t h)
{

    uint8_t * buf;
    size_t s = w * h + LV_DRAW_BUF_ALIGN - 1;
    if(old_buf) buf = lv_realloc(old_buf, s);
    else  buf = lv_malloc(s);

    return buf;
}


#endif /*LV_USE DRAW_BUF == LV_DRAW_BUF_BASIC*/
