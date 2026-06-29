/**
 * @file lv_draw_dma2d.c
 *
 */

/*********************
*      INCLUDES
*********************/

#include "lv_draw_esp_dma2d_private.h"
#include "lv_draw_esp_dma2d.h"

#if LV_USE_ESP_DMA2D

#include <esp_heap_caps.h>

/*********************
*      DEFINES
*********************/

#define DMA2D_DESC_ALIGN 64U

/**********************
*  STATIC PROTOTYPES
**********************/

static int32_t dma2d_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t dma2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t dma2d_delete(lv_draw_unit_t * draw_unit);
static void dma2d_execute_drawing(lv_draw_dma2d_unit_t * u);

static bool image_task_basic_supported(const lv_draw_image_dsc_t * dsc);
static dma2d_data_burst_length_t burst_length_from_config(void);

/**********************
*   GLOBAL FUNCTIONS
**********************/

void LV_ATTRIBUTE_FAST_MEM lv_draw_esp_dma2d_init(void)
{
    lv_draw_dma2d_unit_t * u = lv_draw_create_unit(sizeof(lv_draw_dma2d_unit_t));
    LV_ASSERT_NULL(u);
    if(u == NULL) return;

    u->base_unit.evaluate_cb = dma2d_evaluate;
    u->base_unit.dispatch_cb = dma2d_dispatch;
    u->base_unit.delete_cb = dma2d_delete;
    u->base_unit.name = "ESP_DMA2D";

    u->done_sem = xSemaphoreCreateBinaryStatic(&u->done_sem_buffer);
    LV_ASSERT_NULL(u->done_sem);
    if(u->done_sem == NULL) return;

    dma2d_pool_config_t pool_cfg = {
        .pool_id = 0,
        .intr_priority = 0,
    };
    esp_err_t ret = dma2d_acquire_pool(&pool_cfg, &u->dma2d_pool);
    LV_ASSERT(ret == ESP_OK);
    if(ret != ESP_OK) return;

    u->tx_desc = heap_caps_aligned_calloc(DMA2D_DESC_ALIGN, 1, sizeof(dma2d_descriptor_t),
                                          MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    u->rx_desc = heap_caps_aligned_calloc(DMA2D_DESC_ALIGN, 1, sizeof(dma2d_descriptor_t),
                                          MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    u->dma2d_trans = heap_caps_calloc(1, SIZEOF_DMA2D_TRANS_T, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    u->dma_desc_size = DMA2D_DESC_ALIGN;
    LV_ASSERT_MALLOC(u->tx_desc);
    LV_ASSERT_MALLOC(u->rx_desc);
    LV_ASSERT_MALLOC(u->dma2d_trans);

    if(u->tx_desc == NULL || u->rx_desc == NULL || u->dma2d_trans == NULL) {
        return;
    }

    u->transfer_ability = (dma2d_transfer_ability_t) {
        .data_burst_length = burst_length_from_config(),
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_esp_dma2d_deinit(void)
{
    /* No global deinit required */
}

bool LV_ATTRIBUTE_FAST_MEM lv_draw_esp_dma2d_task_supported(const lv_draw_task_t * t, bool * is_convert)
{
    if(t == NULL) return false;
    const lv_draw_image_dsc_t * dsc = t->draw_dsc;
    if(dsc == NULL) return false;
    if(!image_task_basic_supported(dsc)) return false;

    lv_color_format_t dst_cf = dsc->base.layer->color_format;
    lv_color_format_t src_cf = dsc->header.cf;

    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        const lv_layer_t * src_layer = (const lv_layer_t *)dsc->src;
        if(src_layer == NULL || src_layer->draw_buf == NULL) return false;
        src_cf = src_layer->draw_buf->header.cf;
    }
    else if(t->type != LV_DRAW_TASK_TYPE_IMAGE) {
        return false;
    }

    lv_draw_dma2d_cf_info_t src_info;
    lv_draw_dma2d_cf_info_t dst_info;
    if(!lv_draw_esp_dma2d_cf_info(src_cf, &src_info)) return false;
    if(!lv_draw_esp_dma2d_cf_info(dst_cf, &dst_info)) return false;

    bool convert = (src_cf != dst_cf);
    if(convert) {
        dma2d_csc_tx_option_t csc_option;
        if(!lv_draw_esp_dma2d_tx_csc_option(src_cf, dst_cf, &csc_option)) return false;
    }

    if(is_convert) *is_convert = convert;
    return true;
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_esp_dma2d_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                                   const lv_area_t * coords)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;
    if(layer_to_draw == NULL || layer_to_draw->draw_buf == NULL) return;

    lv_draw_image_dsc_t new_draw_dsc = *draw_dsc;
    new_draw_dsc.src = layer_to_draw->draw_buf;
    lv_draw_esp_dma2d_image(t, &new_draw_dsc, coords);
}

/**********************
*   STATIC FUNCTIONS
**********************/

static bool LV_ATTRIBUTE_FAST_MEM image_task_basic_supported(const lv_draw_image_dsc_t * dsc)
{
    if(dsc->clip_radius != 0) return false;
    if(dsc->bitmap_mask_src != NULL) return false;
    if(dsc->sup != NULL) return false;
    if(dsc->tile != 0) return false;
    if(dsc->blend_mode != LV_BLEND_MODE_NORMAL) return false;
    if(dsc->recolor_opa > LV_OPA_MIN) return false;
    if(dsc->opa < (lv_opa_t)LV_OPA_MAX) return false;
    if(dsc->skew_y != 0 || dsc->skew_x != 0) return false;
    if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) return false;
    if(dsc->rotation != 0) return false;
    return true;
}

static dma2d_data_burst_length_t LV_ATTRIBUTE_FAST_MEM burst_length_from_config(void)
{
    switch(LV_ESP_DMA2D_BURST_LENGTH) {
        case 8:
            return DMA2D_DATA_BURST_LENGTH_8;
        case 16:
            return DMA2D_DATA_BURST_LENGTH_16;
        case 32:
            return DMA2D_DATA_BURST_LENGTH_32;
        case 64:
            return DMA2D_DATA_BURST_LENGTH_64;
        case 128:
            return DMA2D_DATA_BURST_LENGTH_128;
        default:
            return DMA2D_DATA_BURST_LENGTH_64;
    }
}

static int32_t LV_ATTRIBUTE_FAST_MEM dma2d_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * t)
{
    LV_UNUSED(draw_unit);
    bool is_convert = false;
    if(!lv_draw_esp_dma2d_task_supported(t, &is_convert)) return 0;

    if(t->preference_score > DRAW_UNIT_ESP_DMA2D_PREF_SCORE) {
        t->preference_score = DRAW_UNIT_ESP_DMA2D_PREF_SCORE;
        t->preferred_draw_unit_id = DRAW_UNIT_ID_ESP_DMA2D;
    }
    return 1;
}

static int32_t LV_ATTRIBUTE_FAST_MEM dma2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_dma2d_unit_t * u = (lv_draw_dma2d_unit_t *)draw_unit;
    if(u->task_act) return LV_DRAW_UNIT_IDLE;

    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_ESP_DMA2D);
    if(t == NULL || t->preferred_draw_unit_id != DRAW_UNIT_ID_ESP_DMA2D) return LV_DRAW_UNIT_IDLE;
    if(lv_draw_layer_alloc_buf(layer) == NULL) return LV_DRAW_UNIT_IDLE;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    u->task_act = t;
    u->task_act->draw_unit = draw_unit;

    dma2d_execute_drawing(u);

    u->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
    u->task_act = NULL;
    lv_draw_dispatch_request();

    return 1;
}

static int32_t LV_ATTRIBUTE_FAST_MEM dma2d_delete(lv_draw_unit_t * draw_unit)
{
    lv_draw_dma2d_unit_t * u = (lv_draw_dma2d_unit_t *)draw_unit;
    if(u->tx_desc) free(u->tx_desc);
    if(u->rx_desc) free(u->rx_desc);
    if(u->dma2d_trans) free(u->dma2d_trans);
    if(u->dma2d_pool) dma2d_release_pool(u->dma2d_pool);
    if(u->done_sem) vSemaphoreDelete(u->done_sem);
    return 0;
}

static void LV_ATTRIBUTE_FAST_MEM dma2d_execute_drawing(lv_draw_dma2d_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;
    lv_layer_t * layer = t->target_layer;
    lv_draw_buf_t * buf = layer->draw_buf;
    lv_area_t area;

    if(!lv_area_intersect(&area, &t->area, &t->clip_area)) return;
    lv_draw_buf_invalidate_cache(buf, &area);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_esp_dma2d_image(t, (lv_draw_image_dsc_t *)t->draw_dsc, &area);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_esp_dma2d_layer(t, (lv_draw_image_dsc_t *)t->draw_dsc, &area);
            break;
        default:
            break;
    }

    lv_draw_buf_invalidate_cache(buf, &area);
}

#endif /* LV_USE_ESP_DMA2D */
