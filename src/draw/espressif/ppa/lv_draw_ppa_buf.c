/**
 * @file lv_draw_ppa_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"

#if LV_USE_PPA
#include LV_STDINT_INCLUDE
#include "../../lv_draw_buf_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 *********************/
static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);
static void flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);
static void cache_msync_rows(const lv_draw_buf_t * draw_buf, const lv_area_t * area, uint32_t dir);

/**********************
 *   GLOBAL FUNCTIONS
 *********************/
void lv_draw_buf_ppa_init_handlers(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    handlers->invalidate_cache_cb = invalidate_cache;
    handlers->flush_cache_cb = flush_cache;
}

/**********************
 *   STATIC FUNCTIONS
 *********************/

static void cache_msync_rows(const lv_draw_buf_t * draw_buf, const lv_area_t * area, uint32_t dir)
{
    size_t line = esp_cache_get_line_size_by_addr(draw_buf->data);
    if(line == 0) return; /* Not cached */

    size_t start = 0;
    size_t end = draw_buf->data_size;

    /* Whole rows, and the area is relative to the buffer */
    if(area) {
        uint32_t stride = draw_buf->header.stride;
        int32_t y1 = area->y1 < 0 ? 0 : area->y1;
        int32_t y2 = LV_MIN(area->y2, (int32_t)draw_buf->header.h - 1);
        if(stride == 0 || y2 < y1) return;

        start = (size_t)y1 * stride;
        end = (size_t)(y2 + 1) * stride;
    }

    /* M2C is refused unless the range is whole cache lines */
    start &= ~(line - 1);
    end = (end + line - 1) & ~(line - 1);
    if(end > draw_buf->data_size) end = draw_buf->data_size & ~(line - 1);
    if(start >= end) return;

    esp_cache_msync(draw_buf->data + start, end - start, dir | ESP_CACHE_MSYNC_FLAG_TYPE_DATA);
}

static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    cache_msync_rows(draw_buf, area, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
}

static void flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    cache_msync_rows(draw_buf, area, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
}
#endif /* LV_USE_PPA */
