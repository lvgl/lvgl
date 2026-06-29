/**
 * @file lv_draw_dma2d_convert.c
 *
 */

#include "lv_draw_esp_dma2d_private.h"

#if LV_USE_ESP_DMA2D

bool LV_ATTRIBUTE_FAST_MEM lv_draw_esp_dma2d_cf_info(lv_color_format_t cf, lv_draw_dma2d_cf_info_t * info)
{
    if(info == NULL) return false;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
            info->pixel_format.color_space = COLOR_SPACE_RGB;
            info->pixel_format.pixel_format = COLOR_PIXEL_RGB565;
            info->bytes_per_pixel = 2;
            return true;
        case LV_COLOR_FORMAT_RGB888:
            info->pixel_format.color_space = COLOR_SPACE_RGB;
            info->pixel_format.pixel_format = COLOR_PIXEL_RGB888;
            info->bytes_per_pixel = 3;
            return true;
        default:
            return false;
    }
}

bool LV_ATTRIBUTE_FAST_MEM lv_draw_esp_dma2d_tx_csc_option(lv_color_format_t src_cf, lv_color_format_t dst_cf,
                                                           dma2d_csc_tx_option_t * option)
{
    if(option == NULL) return false;

    if(src_cf == LV_COLOR_FORMAT_RGB565 && dst_cf == LV_COLOR_FORMAT_RGB888) {
        *option = DMA2D_CSC_TX_RGB565_TO_RGB888;
        return true;
    }

    if(src_cf == LV_COLOR_FORMAT_RGB888 && dst_cf == LV_COLOR_FORMAT_RGB565) {
        *option = DMA2D_CSC_TX_RGB888_TO_RGB565;
        return true;
    }

    return false;
}

#endif /* LV_USE_ESP_DMA2D */
