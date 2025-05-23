/**
 * @file lv_draw_buf_ppa.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"

#if LV_USE_PPA
#include <stdint.h>
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
static void _invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

/**********************
 *   GLOBAL FUNCTIONS
 *********************/

void lv_draw_buf_ppa_init_handlers(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    lv_draw_buf_handlers_t * image_handlers = lv_draw_buf_get_image_handlers();

    handlers->invalidate_cache_cb       = _invalidate_cache;
    image_handlers->invalidate_cache_cb = _invalidate_cache;
}

/**********************
 *   STATIC FUNCTIONS
 *********************/

static void _invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    const lv_image_header_t * header = &draw_buf->header;
    uint32_t stride = header->stride;
    lv_color_format_t cf = header->cf;

    if(area->y1 == 0) {
        uint32_t size = stride * lv_area_get_height(area);
        esp_cache_msync(PPA_PTR_ALIGN_UP((void *)draw_buf->data, 64), size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        return;
    }

    const uint8_t * buf_u8 = draw_buf->data;
    uint8_t align_bytes = 64;
    uint8_t bits_per_pixel = lv_color_format_get_bpp(cf);

    uint16_t align_pixels = (align_bytes * 8) / bits_per_pixel;
    uint16_t offset_x = 0;

    if(area->x1 >= (int32_t)(area->x1 % align_pixels)) {
        uint16_t shift_x = (uint16_t)(area->x1 - (area->x1 % align_pixels));
        offset_x = (uint16_t)(area->x1 - shift_x);
        buf_u8 += (shift_x * bits_per_pixel) / 8;
    }

    if(area->y1 > 0) {
        buf_u8 += (uint32_t)area->y1 * stride;
    }

    uint16_t line_pixels = (uint16_t)(offset_x + lv_area_get_width(area));
    uint16_t line_size = (uint16_t)((line_pixels * bits_per_pixel) / 8);
    uint16_t area_height = lv_area_get_height(area);

    for(uint16_t y = 0; y < area_height; y++) {
        const void * line_addr = PPA_PTR_ALIGN_UP(buf_u8 + (uint32_t)y * stride, 64);
        esp_cache_msync((void *)line_addr, line_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    }
}
#endif /* LV_USE_PPA */
