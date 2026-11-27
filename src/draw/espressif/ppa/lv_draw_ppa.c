/**
 * @file lv_draw_ppa.c
 *
 */

/*********************
*      INCLUDES
*********************/

#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"
#include "../../sw/lv_draw_sw_private.h"

#if LV_USE_PPA

/*********************
*      DEFINES
*********************/


/**********************
*  STATIC PROTOTYPES
**********************/

static bool ppa_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc);
static bool ppa_can_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static int32_t ppa_delete(lv_draw_unit_t * draw_unit);
static void ppa_draw_begin(lv_layer_t * layer, const lv_area_t * clip_area, lv_area_t * buf_area);
static void ppa_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
static void ppa_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                         const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

/**********************
*  STATIC VARIABLES
**********************/

static const lv_draw_unit_draw_cbs_t draw_cbs = {
    .draw_fill_cb = ppa_draw_fill,
    .can_draw_fill_cb = ppa_can_draw_fill,
    .draw_image_cb = ppa_draw_img,
    .can_draw_image_cb = ppa_can_draw_img,
};

static lv_draw_ppa_unit_t * ppa_unit;

/**********************
*   GLOBAL FUNCTIONS
**********************/

void lv_draw_ppa_init(void)
{
    esp_err_t res;
    ppa_client_config_t cfg = {0};

    /* Create draw unit */
    lv_draw_buf_ppa_init_handlers();
    lv_draw_ppa_unit_t * draw_ppa_unit = lv_draw_create_unit(sizeof(lv_draw_ppa_unit_t));
    draw_ppa_unit->base_unit.delete_cb    = ppa_delete;
    draw_ppa_unit->base_unit.name         = "ESP_PPA";
    draw_ppa_unit->base_unit.draw_cbs     = &draw_cbs;
    draw_ppa_unit->base_unit.fallback     = lv_draw_sw_get_draw_unit();

    /* Register SRM client */
    cfg.oper_type = PPA_OPERATION_SRM;
    cfg.max_pending_trans_num = 1;
#if (LV_PPA_BURST_LENGTH == 128)
    cfg.data_burst_length = PPA_DATA_BURST_LENGTH_128;
#elif (LV_PPA_BURST_LENGTH == 64)
    cfg.data_burst_length = PPA_DATA_BURST_LENGTH_64;
#elif (LV_PPA_BURST_LENGTH == 32)
    cfg.data_burst_length = PPA_DATA_BURST_LENGTH_32;
#elif (LV_PPA_BURST_LENGTH == 16)
    cfg.data_burst_length = PPA_DATA_BURST_LENGTH_16;
#elif (LV_PPA_BURST_LENGTH == 8)
    cfg.data_burst_length = PPA_DATA_BURST_LENGTH_8;
#else
#error "Invalid burst length selection for PPA"
#endif

    res = ppa_register_client(&cfg, &draw_ppa_unit->srm_client);
    LV_ASSERT(res == ESP_OK);

    /* Register Fill client */
    cfg.oper_type = PPA_OPERATION_FILL;
    res = ppa_register_client(&cfg, &draw_ppa_unit->fill_client);
    LV_ASSERT(res == ESP_OK);

    /* Register Blend client */
    cfg.oper_type = PPA_OPERATION_BLEND;

    res = ppa_register_client(&cfg, &draw_ppa_unit->blend_client);
    LV_ASSERT(res == ESP_OK);

    ppa_unit = draw_ppa_unit;
}

void lv_draw_ppa_deinit(void)
{
    /* No global deinit required */
}

ppa_client_handle_t lv_draw_ppa_get_handle(void)
{
    return ppa_unit ? ppa_unit->srm_client : NULL;
}

/**********************
*   STATIC FUNCTIONS
**********************/
static bool ppa_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    if(!ppa_dest_cf_supported(layer->color_format)) return false;
    if((dsc->radius != 0 || dsc->grad.dir != LV_GRAD_DIR_NONE)) return false;
    return dsc->opa > (lv_opa_t)LV_OPA_MAX;
}

static bool ppa_can_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
#if LV_USE_PPA_IMG
    if(!ppa_dest_cf_supported(layer->color_format)) return false;
    return dsc->header.cf < LV_COLOR_FORMAT_PROPRIETARY_START
           && dsc->clip_radius == 0
           && dsc->bitmap_mask_src == NULL
           && dsc->sup == NULL
           && dsc->tile == 0
           && dsc->blend_mode == LV_BLEND_MODE_NORMAL
           && dsc->recolor_opa <= LV_OPA_MIN
           && dsc->opa >= (lv_opa_t)LV_OPA_MAX
           && dsc->skew_y == 0
           && dsc->skew_x == 0
           && dsc->scale_x == 256
           && dsc->scale_y == 256
           && dsc->rotation == 0
           && lv_image_src_get_type(dsc->src) == LV_IMAGE_SRC_VARIABLE
           && (dsc->header.cf == LV_COLOR_FORMAT_RGB888
               || dsc->header.cf == LV_COLOR_FORMAT_RGB565)
           && (layer->color_format == LV_COLOR_FORMAT_RGB888
               || layer->color_format == LV_COLOR_FORMAT_RGB565);
#else
    LV_UNUSED(layer);
    LV_UNUSED(dsc);
    return false;
#endif
}

static int32_t ppa_delete(lv_draw_unit_t * draw_unit)
{
    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *)draw_unit;
    ppa_unregister_client(u->srm_client);
    ppa_unregister_client(u->fill_client);
    ppa_unregister_client(u->blend_client);
    ppa_unit = NULL;
    return 0;
}

/**
 * Write the CPU cache of the clip area back before the PPA draws, and return that area relative to the buffer
 */
static void ppa_draw_begin(lv_layer_t * layer, const lv_area_t * clip_area, lv_area_t * buf_area)
{
    *buf_area = *clip_area;
    lv_area_move(buf_area, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_draw_buf_flush_cache(layer->draw_buf, buf_area);
}

static void ppa_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    lv_area_t buf_area;
    ppa_draw_begin(layer, clip_area, &buf_area);
    lv_draw_ppa_fill(draw_unit, layer, clip_area, dsc, coords);
    lv_draw_buf_invalidate_cache(layer->draw_buf, &buf_area);
}

static void ppa_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                         const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *)draw_unit;
    lv_area_t buf_area;
    ppa_draw_begin(layer, clip_area, &buf_area);

    u->img_sw_fallback = false;
    lv_draw_ppa_img(draw_unit, layer, clip_area, dsc, coords);

    /* The software fallback wrote through the cache, so its lines must survive */
    if(!u->img_sw_fallback) lv_draw_buf_invalidate_cache(layer->draw_buf, &buf_area);
}

#endif /*LV_USE_PPA*/
