/**
 * @file lv_draw_ppa.c
 *
 */

/*********************
*      INCLUDES
*********************/

#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"

#if LV_USE_PPA

/*********************
*      DEFINES
*********************/

#define DRAW_UNIT_ID_PPA         80
#define DRAW_UNIT_PPA_PREF_SCORE 70

/**********************
*  STATIC PROTOTYPES
**********************/

static int32_t ppa_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t ppa_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t ppa_delete(lv_draw_unit_t * draw_unit);
static void  ppa_execute_drawing(lv_draw_ppa_unit_t * u);
static bool ppa_rotation_supported(int32_t rotation);

/**********************
*   GLOBAL FUNCTIONS
**********************/

void LV_ATTRIBUTE_FAST_MEM lv_draw_ppa_init(void)
{
    esp_err_t res;
    ppa_client_config_t cfg = {0};

    /* Create draw unit */
    lv_draw_buf_ppa_init_handlers();
    lv_draw_ppa_unit_t * draw_ppa_unit = lv_draw_create_unit(sizeof(lv_draw_ppa_unit_t));
    draw_ppa_unit->base_unit.evaluate_cb = ppa_evaluate;
    draw_ppa_unit->base_unit.dispatch_cb  = ppa_dispatch;
    draw_ppa_unit->base_unit.delete_cb    = ppa_delete;
    draw_ppa_unit->base_unit.name         = "ESP_PPA";

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
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_ppa_deinit(void)
{
    /* No global deinit required */
}

/**********************
*   STATIC FUNCTIONS
**********************/
static int32_t LV_ATTRIBUTE_FAST_MEM ppa_evaluate(lv_draw_unit_t * u, lv_draw_task_t * t)
{
    LV_UNUSED(u);
    const lv_draw_dsc_base_t * base = (lv_draw_dsc_base_t *)t->draw_dsc;

    if(!ppa_dest_cf_supported(base->layer->color_format)) return 0;

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL: {
                const lv_draw_fill_dsc_t * dsc = (lv_draw_fill_dsc_t *)t->draw_dsc;
                if((dsc->radius != 0 || dsc->grad.dir != LV_GRAD_DIR_NONE)) return 0;
                if(dsc->opa <= (lv_opa_t)LV_OPA_MAX) return 0;

                if(t->preference_score > DRAW_UNIT_PPA_PREF_SCORE) {
                    t->preference_score = DRAW_UNIT_PPA_PREF_SCORE;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_PPA;
                }
                return 1;
            }

        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_image_dsc_t * dsc = t->draw_dsc;
                bool common_ok = dsc->header.cf < LV_COLOR_FORMAT_PROPRIETARY_START
                                 && dsc->clip_radius == 0
                                 && dsc->bitmap_mask_src == NULL
                                 && dsc->sup == NULL
                                 && dsc->tile == 0
                                 && dsc->blend_mode == LV_BLEND_MODE_NORMAL
                                 && dsc->recolor_opa <= LV_OPA_MIN
                                 && dsc->opa >= (lv_opa_t)LV_OPA_MAX
                                 && dsc->skew_y == 0
                                 && dsc->skew_x == 0
                                 && lv_image_src_get_type(dsc->src) == LV_IMAGE_SRC_VARIABLE;
                if(!common_ok) return 0;

                bool is_identity = (dsc->scale_x == LV_SCALE_NONE && dsc->scale_y == LV_SCALE_NONE && dsc->rotation == 0);
                bool clip_is_full = lv_area_is_equal(&t->area, &t->clip_area);

                bool ppa_ok = false;
#if LV_USE_PPA_IMG
                if(is_identity && ppa_src_cf_supported(dsc->header.cf) && ppa_dest_cf_supported(dsc->base.layer->color_format)) {
                    ppa_ok = true;
                }
#endif
#if LV_USE_PPA_TRANSFORM
                if(!ppa_ok && clip_is_full
                   && ppa_rotation_supported(dsc->rotation)
                   && ppa_srm_src_cf_supported(dsc->header.cf)
                   && ppa_srm_dest_cf_supported(dsc->base.layer->color_format)) {
                    ppa_ok = true;
                }
#endif
                if(!ppa_ok) return 0;

                if(t->preference_score > DRAW_UNIT_PPA_PREF_SCORE) {
                    t->preference_score = DRAW_UNIT_PPA_PREF_SCORE;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_PPA;
                }
                return 1;
            }
#if LV_USE_PPA_TRANSFORM
        case LV_DRAW_TASK_TYPE_LAYER: {
                lv_draw_image_dsc_t * dsc = t->draw_dsc;
                lv_layer_t * src_layer = (lv_layer_t *)dsc->src;
                if(src_layer == NULL || src_layer->draw_buf == NULL) return 0;

                bool common_ok = dsc->clip_radius == 0
                                 && dsc->bitmap_mask_src == NULL
                                 && dsc->sup == NULL
                                 && dsc->tile == 0
                                 && dsc->blend_mode == LV_BLEND_MODE_NORMAL
                                 && dsc->recolor_opa <= LV_OPA_MIN
                                 && dsc->opa >= (lv_opa_t)LV_OPA_MAX
                                 && dsc->skew_y == 0
                                 && dsc->skew_x == 0
                                 && lv_area_is_equal(&t->area, &t->clip_area)
                                 && ppa_rotation_supported(dsc->rotation)
                                 && ppa_srm_src_cf_supported(src_layer->draw_buf->header.cf)
                                 && ppa_srm_dest_cf_supported(dsc->base.layer->color_format);
                if(!common_ok) return 0;

                if(t->preference_score > DRAW_UNIT_PPA_PREF_SCORE) {
                    t->preference_score = DRAW_UNIT_PPA_PREF_SCORE;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_PPA;
                }
                return 1;
            }
#endif
        default:
            return 0;
    }
}

static int32_t LV_ATTRIBUTE_FAST_MEM ppa_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *)draw_unit;
    if(u->task_act) {
        return LV_DRAW_UNIT_IDLE;
    }

    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_PPA);
    if(!t || t->preferred_draw_unit_id != DRAW_UNIT_ID_PPA) return LV_DRAW_UNIT_IDLE;
    if(lv_draw_layer_alloc_buf(layer) == NULL) return LV_DRAW_UNIT_IDLE;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    u->task_act = t;
    u->task_act->draw_unit = draw_unit;

    ppa_execute_drawing(u);

    u->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
    u->task_act = NULL;
    lv_draw_dispatch_request();

    return 1;
}

static int32_t LV_ATTRIBUTE_FAST_MEM ppa_delete(lv_draw_unit_t * draw_unit)
{
    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *)draw_unit;
    ppa_unregister_client(u->srm_client);
    ppa_unregister_client(u->fill_client);
    ppa_unregister_client(u->blend_client);
    return 0;
}

static void LV_ATTRIBUTE_FAST_MEM ppa_execute_drawing(lv_draw_ppa_unit_t * u)
{
    lv_draw_task_t * t         = u->task_act;
    lv_layer_t * layer         = t->target_layer;
    lv_draw_buf_t * buf        = layer->draw_buf;
    lv_area_t area;

    if(!lv_area_intersect(&area, &t->area, &t->clip_area)) return;
    lv_draw_buf_invalidate_cache(buf, &area);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_ppa_fill(t, (lv_draw_fill_dsc_t *)t->draw_dsc, &area);
            lv_draw_buf_invalidate_cache(buf, &area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_ppa_img(t, (lv_draw_image_dsc_t *)t->draw_dsc, &area);
            lv_draw_buf_invalidate_cache(buf, &area);
            break;
#if LV_USE_PPA_TRANSFORM
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_ppa_layer(t, (lv_draw_image_dsc_t *)t->draw_dsc, &area);
            lv_draw_buf_invalidate_cache(buf, &area);
            break;
#endif
        default:
            break;
    }
}

static bool LV_ATTRIBUTE_FAST_MEM ppa_rotation_supported(int32_t rotation)
{
    int32_t r = rotation % 3600;
    if(r < 0) r += 3600;
    return (r == 0 || r == 900 || r == 1800 || r == 2700);
}

#endif /*LV_USE_PPA*/
