/**
 * @file lv_draw_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_buf.h"

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

lv_draw_buf_t  * lv_draw_buf_malloc(lv_coord_t w, lv_coord_t h, lv_color_format_t color_format)
{
    lv_draw_buf_t * draw_buf = lv_malloc(sizeof(lv_draw_buf_t));
    draw_buf->width = w;
    draw_buf->height = h;
    draw_buf->color_format = color_format;
    draw_buf->buf = buf_alloc(NULL, w, h);

    return draw_buf;
}


lv_draw_buf_t  * lv_draw_buf_realloc(lv_draw_buf_t  * draw_buf, lv_coord_t w, lv_coord_t h,
                                     lv_color_format_t color_format)
{
    draw_buf->width = w;
    draw_buf->height = h;
    draw_buf->color_format = color_format;

    draw_buf->buf = buf_alloc(draw_buf->buf, w, h);
    return draw_buf;
}

void lv_draw_buf_free(lv_draw_buf_t  * draw_buf)
{
    lv_free(draw_buf->buf);
    lv_free(draw_buf);
}

void lv_draw_buf_invalidate_cache(lv_draw_buf_t  * buf)
{

}

uint32_t lv_draw_buf_get_stride(const lv_draw_buf_t * draw_buf)
{
    return (draw_buf->width + LV_DRAW_BUF_STRIDE - 1) & ~(LV_DRAW_BUF_STRIDE - 1);
}

void * lv_draw_buf_go_to_xy(lv_draw_buf_t * draw_buf, lv_coord_t x, lv_coord_t y)
{
    uint32_t px_size = lv_color_format_get_size(draw_buf->color_format);
    uint32_t stride = lv_draw_buf_get_stride(draw_buf);
    uint8_t * buf_tmp = draw_buf->buf;
    buf_tmp += stride * y * px_size;
    buf_tmp += x * px_size;

    return buf_tmp;
}


void lv_draw_buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * a)
{
    uint8_t px_size = lv_color_format_get_size(draw_buf->color_format);
    uint32_t stride = lv_draw_buf_get_stride(draw_buf);
    lv_memzero(draw_buf->buf, stride * draw_buf->height * draw_buf->color_format);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint8_t * buf_alloc(void * old_buf, lv_coord_t w, lv_coord_t h)
{
    w = (w / LV_DRAW_BUF_STRIDE) * LV_DRAW_BUF_STRIDE;
    uint8_t * buf;
    size_t s = w * h + LV_DRAW_BUF_ALIGN - 1;
    if(old_buf) buf = lv_realloc(old_buf, s);
    else  buf = lv_malloc(s);

    buf += LV_DRAW_BUF_ALIGN - 1;
    buf = (lv_uintptr_t) buf & ~(LV_DRAW_BUF_ALIGN - 1);
    return buf;
}
