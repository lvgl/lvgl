/**
 * @file lv_draw_esp_dma2d_private.h
 *
 */

#ifndef LV_DRAW_ESP_DMA2D_PRIVATE_H
#define LV_DRAW_ESP_DMA2D_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl_public.h"

#if LV_USE_ESP_DMA2D

#include "../../lv_draw_private.h"
#include "../../../display/lv_display_private.h"
#include "../../../misc/lv_area_private.h"
#include "../../../draw/lv_draw_image_private.h"
#include "../../../draw/lv_image_decoder_private.h"

#include <esp_private/dma2d.h>
#include <hal/color_types.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_ESP_DMA2D         81
#define DRAW_UNIT_ESP_DMA2D_PREF_SCORE 60

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    color_space_pixel_format_t pixel_format;
    uint32_t bytes_per_pixel;
} lv_draw_dma2d_cf_info_t;

typedef struct lv_draw_dma2d_unit {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
    dma2d_pool_handle_t dma2d_pool;
    dma2d_trans_t * dma2d_trans;
    dma2d_descriptor_t * tx_desc;
    dma2d_descriptor_t * rx_desc;
    size_t dma_desc_size;
    SemaphoreHandle_t done_sem;
    StaticSemaphore_t done_sem_buffer;
    dma2d_transfer_ability_t transfer_ability;
    dma2d_trans_config_t trans_cfg;
    dma2d_rx_event_callbacks_t rx_cbs;
    dma2d_csc_config_t tx_csc_cfg;
    bool use_tx_csc;
} lv_draw_dma2d_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

bool lv_draw_esp_dma2d_task_supported(const lv_draw_task_t * t, bool * is_convert);
void lv_draw_esp_dma2d_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords);
void lv_draw_esp_dma2d_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords);

bool lv_draw_esp_dma2d_cf_info(lv_color_format_t cf, lv_draw_dma2d_cf_info_t * info);
bool lv_draw_esp_dma2d_tx_csc_option(lv_color_format_t src_cf, lv_color_format_t dst_cf,
                                     dma2d_csc_tx_option_t * option);
esp_err_t lv_draw_esp_dma2d_blit(lv_draw_dma2d_unit_t * u,
                                 const void * src_buf, uint32_t src_pic_w, uint32_t src_pic_h,
                                 uint32_t src_x, uint32_t src_y,
                                 void * dst_buf, uint32_t dst_pic_w, uint32_t dst_pic_h,
                                 uint32_t dst_x, uint32_t dst_y,
                                 uint32_t block_w, uint32_t block_h,
                                 lv_color_format_t src_cf, lv_color_format_t dst_cf);

#endif /* LV_USE_ESP_DMA2D */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_ESP_DMA2D_PRIVATE_H*/
