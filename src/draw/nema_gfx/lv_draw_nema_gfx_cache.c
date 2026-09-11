/**
 * @file lv_draw_nema_gfx_cache.c
 * Copyright (c) 2026 STMicroelectronics.
 */

#include "lv_draw_nema_gfx.h"

#if LV_NEMA_USE_CACHE

/*********************
 *      INCLUDES
 *********************/
#include LV_NEMA_CACHE_HAL_INCLUDE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void __invalidate_flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area,
                                     bool flush);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_nema_gfx_invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    __invalidate_flush_cache(draw_buf, area, false);
}

void lv_draw_nema_gfx_flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    __invalidate_flush_cache(draw_buf, area, true);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void __invalidate_flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area,
                                     bool flush)
{
    const lv_image_header_t * header = &draw_buf->header;
    uint32_t stride = header->stride;
    lv_color_format_t cf = header->cf;

    uint32_t bpp = lv_color_format_get_bpp(cf);
    int32_t lines = lv_area_get_height(area);

    if(lines <= 0 || bpp == 0U || stride == 0U) {
        return;
    }

    uint64_t start_bit = (uint64_t)(uint32_t)area->x1 * (uint64_t)bpp;
    uint64_t end_bit = (uint64_t)((uint32_t)area->x2 + 1U) * (uint64_t)bpp;
    uint32_t start_byte = (uint32_t)(start_bit >> 3);
    uint32_t end_byte = (uint32_t)((end_bit + 7U) >> 3);
    int32_t bytes_to_flush_per_line = (int32_t)(end_byte - start_byte);
    uint8_t * address = draw_buf->data + start_byte + (stride * (uint32_t)area->y1);
    int32_t i = 0;

    if(bytes_to_flush_per_line <= 0) {
        return;
    }

    for(i = 0; i < lines; i++) {
        if(SCB->CCR & SCB_CCR_DC_Msk) {
            if(flush) {
                SCB_CleanDCache_by_Addr(address, bytes_to_flush_per_line);
            }
            else {
                SCB_InvalidateDCache_by_Addr(address, bytes_to_flush_per_line);
            }
        }
        address += stride;
    }
}

#endif
