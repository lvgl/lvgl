/**
 * @file lv_draw_dma2d_copy.c
 *
 */

#include "lv_draw_esp_dma2d_private.h"

#if LV_USE_ESP_DMA2D

#include <soc/dma2d_channel.h>
#include <esp_cache.h>
#include <esp_heap_caps.h>

static void LV_ATTRIBUTE_FAST_MEM fill_dma_desc_2d(dma2d_descriptor_t * desc,
                                                   void * buf,
                                                   uint32_t pic_w, uint32_t pic_h,
                                                   uint32_t block_w, uint32_t block_h,
                                                   uint32_t x, uint32_t y,
                                                   color_space_pixel_format_t fmt)
{
    desc->vb_size = block_h;
    desc->hb_length = block_w;
    desc->err_eof = 0;
    desc->dma2d_en = 1;
    desc->suc_eof = 1;
    desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    desc->va_size = pic_h;
    desc->ha_length = pic_w;
    desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(fmt);
    desc->y = y;
    desc->x = x;
    desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    desc->buffer = buf;
    desc->next = NULL;
}

static bool LV_ATTRIBUTE_FAST_MEM dma2d_copy_done_cb(dma2d_channel_handle_t dma2d_chan, dma2d_event_data_t * event_data,
                                                     void * user_data)
{
    LV_UNUSED(dma2d_chan);
    LV_UNUSED(event_data);
    lv_draw_dma2d_unit_t * u = (lv_draw_dma2d_unit_t *)user_data;
    BaseType_t x_higher_priority_task_woken = pdFALSE;
    xSemaphoreGiveFromISR(u->done_sem, &x_higher_priority_task_woken);
    return (x_higher_priority_task_woken == pdTRUE);
}

static bool LV_ATTRIBUTE_FAST_MEM dma2d_copy_job_picked_cb(uint32_t num_chans,
                                                           const dma2d_trans_channel_info_t * dma2d_chans, void * user_data)
{
    lv_draw_dma2d_unit_t * u = (lv_draw_dma2d_unit_t *)user_data;
    dma2d_channel_handle_t tx_chan = NULL;
    dma2d_channel_handle_t rx_chan = NULL;

    for(uint32_t i = 0; i < num_chans; i++) {
        if(dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_TX) tx_chan = dma2d_chans[i].chan;
        if(dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_RX) rx_chan = dma2d_chans[i].chan;
    }

    if(tx_chan == NULL || rx_chan == NULL) return false;

    dma2d_trigger_t trig_periph = {
        .periph = DMA2D_TRIG_PERIPH_M2M,
        .periph_sel_id = SOC_DMA2D_TRIG_PERIPH_M2M_TX,
    };
    dma2d_connect(tx_chan, &trig_periph);
    trig_periph.periph_sel_id = SOC_DMA2D_TRIG_PERIPH_M2M_RX;
    dma2d_connect(rx_chan, &trig_periph);

    dma2d_set_transfer_ability(tx_chan, &u->transfer_ability);
    dma2d_set_transfer_ability(rx_chan, &u->transfer_ability);

    if(u->use_tx_csc) {
        dma2d_configure_color_space_conversion(tx_chan, &u->tx_csc_cfg);
    }

    dma2d_register_rx_event_callbacks(rx_chan, &u->rx_cbs, u);
    dma2d_set_desc_addr(tx_chan, (intptr_t)u->tx_desc);
    dma2d_set_desc_addr(rx_chan, (intptr_t)u->rx_desc);
    dma2d_start(tx_chan);
    dma2d_start(rx_chan);

    return false;
}

esp_err_t LV_ATTRIBUTE_FAST_MEM lv_draw_esp_dma2d_blit(lv_draw_dma2d_unit_t * u,
                                                       const void * src_buf, uint32_t src_pic_w, uint32_t src_pic_h,
                                                       uint32_t src_x, uint32_t src_y,
                                                       void * dst_buf, uint32_t dst_pic_w, uint32_t dst_pic_h,
                                                       uint32_t dst_x, uint32_t dst_y,
                                                       uint32_t block_w, uint32_t block_h,
                                                       lv_color_format_t src_cf, lv_color_format_t dst_cf)
{
    if(u == NULL || src_buf == NULL || dst_buf == NULL || block_w == 0 || block_h == 0) return ESP_ERR_INVALID_ARG;

    lv_draw_dma2d_cf_info_t src_info;
    lv_draw_dma2d_cf_info_t dst_info;
    if(!lv_draw_esp_dma2d_cf_info(src_cf, &src_info)) return ESP_ERR_NOT_SUPPORTED;
    if(!lv_draw_esp_dma2d_cf_info(dst_cf, &dst_info)) return ESP_ERR_NOT_SUPPORTED;

    u->use_tx_csc = false;
    if(src_cf != dst_cf) {
        dma2d_csc_tx_option_t csc_option;
        if(!lv_draw_esp_dma2d_tx_csc_option(src_cf, dst_cf, &csc_option)) return ESP_ERR_NOT_SUPPORTED;

        u->tx_csc_cfg = (dma2d_csc_config_t) {
            .tx_csc_option = csc_option,
            .pre_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0,
        };
        u->use_tx_csc = true;
    }

    fill_dma_desc_2d(u->tx_desc, (void *)src_buf, src_pic_w, src_pic_h, block_w, block_h, src_x, src_y,
                     src_info.pixel_format);
    fill_dma_desc_2d(u->rx_desc, dst_buf, dst_pic_w, dst_pic_h, block_w, block_h, dst_x, dst_y, dst_info.pixel_format);

    esp_cache_msync(u->tx_desc, u->dma_desc_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    esp_cache_msync(u->rx_desc, u->dma_desc_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);

    (void)xSemaphoreTake(u->done_sem, 0);

    u->trans_cfg = (dma2d_trans_config_t) {
        .tx_channel_num = 1,
        .rx_channel_num = 1,
        .channel_flags = DMA2D_CHANNEL_FUNCTION_FLAG_SIBLING |
                         (u->use_tx_csc ? DMA2D_CHANNEL_FUNCTION_FLAG_TX_CSC : 0),
                         .specified_tx_channel_mask = 0,
                         .specified_rx_channel_mask = 0,
                         .on_job_picked = dma2d_copy_job_picked_cb,
                         .user_config = u,
    };
    u->rx_cbs = (dma2d_rx_event_callbacks_t) {
        .on_recv_eof = dma2d_copy_done_cb,
    };

    esp_err_t ret = dma2d_enqueue(u->dma2d_pool, &u->trans_cfg, u->dma2d_trans);
    if(ret != ESP_OK) return ret;

    if(xSemaphoreTake(u->done_sem, pdMS_TO_TICKS(1000)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}

#endif /* LV_USE_ESP_DMA2D */
