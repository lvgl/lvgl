/**
 * @file lv_draw_buf.h
 *
 */

#ifndef LV_DRAW_BUF_H
#define LV_DRAW_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    void * buf;
    lv_coord_t width;
    lv_coord_t height;
    lv_color_format_t color_format;
} lv_draw_buf_t;

void lv_draw_buf_init(lv_draw_buf_t * draw_buf, lv_coord_t w, lv_coord_t h, lv_color_format_t color_format);
void lv_draw_buf_malloc(lv_draw_buf_t * draw_buf);
void lv_draw_buf_realloc(lv_draw_buf_t  * buf, lv_coord_t w, lv_coord_t h, lv_color_format_t color_format);
void lv_draw_buf_free(lv_draw_buf_t  * buf);
void lv_draw_buf_invalidate_cache(lv_draw_buf_t  * buf);

uint32_t lv_draw_buf_get_stride(const lv_draw_buf_t * draw_buf);
void * lv_draw_buf_go_to_xy(lv_draw_buf_t * draw_buf, lv_coord_t x, lv_coord_t y);
void lv_draw_buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * a);

void lv_draw_buf_copy(void * dest_buf, uint32_t dest_stride, const lv_area_t * dest_area,
                      void * src_buf, uint32_t src_stride, const lv_area_t * src_area, lv_color_format_t color_format);

static inline uint32_t lv_draw_buf_width_to_stride(uint32_t w, lv_color_format_t color_format)
{
    uint32_t width_byte =  w * lv_color_format_get_size(color_format);
    return (width_byte + LV_DRAW_BUF_STRIDE_ALIGN - 1) & ~(LV_DRAW_BUF_STRIDE_ALIGN - 1);
}

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BUF_H*/
