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
    draw_ppa_unit->base_unit.evaluate_cb = ppa_evaluate;
    draw_ppa_unit->base_unit.dispatch_cb  = ppa_dispatch;
    draw_ppa_unit->base_unit.delete_cb    = ppa_delete;
    draw_ppa_unit->base_unit.name         = "ESP_PPA";

    /* Register SRM client */
    cfg.oper_type = PPA_OPERATION_SRM;
    cfg.max_pending_trans_num = 1;
    cfg.data_burst_length = PPA_DATA_BURST_LENGTH_128;

    res = ppa_register_client(&cfg, &draw_ppa_unit->srm_client);
    LV_ASSERT(res == ESP_OK);

    /* Register Fill client */
    cfg.oper_type = PPA_OPERATION_FILL;
    cfg.data_burst_length = PPA_DATA_BURST_LENGTH_128;
    res = ppa_register_client(&cfg, &draw_ppa_unit->fill_client);
    LV_ASSERT(res == ESP_OK);

    /* Register Blend client */
    cfg.oper_type = PPA_OPERATION_BLEND;
    cfg.data_burst_length = PPA_DATA_BURST_LENGTH_128;

    res = ppa_register_client(&cfg, &draw_ppa_unit->blend_client);
    LV_ASSERT(res == ESP_OK);
}

void lv_draw_ppa_deinit(void)
{
    /* No global deinit required */
}

/**********************
*   STATIC FUNCTIONS
**********************/
static int32_t ppa_evaluate(lv_draw_unit_t * u, lv_draw_task_t * t)
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

#if LV_USE_PPA_IMG
        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_image_dsc_t * dsc = t->draw_dsc;
                if(!(dsc->header.cf < LV_COLOR_FORMAT_PROPRIETARY_START
                     && dsc->clip_radius == 0
                     && dsc->bitmap_mask_src == NULL
                     && dsc->sup == NULL
                     && dsc->tile == 0
                     && dsc->blend_mode == LV_BLEND_MODE_NORMAL
                     && dsc->recolor_opa <= LV_OPA_MIN
                     && dsc->opa <= (lv_opa_t)LV_OPA_MAX
                     && dsc->skew_y == 0
                     && dsc->skew_x == 0
                     && dsc->scale_x == 256
                     && dsc->scale_y == 256
                     && dsc->rotation == 0
                     && lv_image_src_get_type(dsc->src) == LV_IMAGE_SRC_VARIABLE
                     && (dsc->header.cf == LV_COLOR_FORMAT_RGB888
                         || dsc->header.cf == LV_COLOR_FORMAT_RGB565)
                     && (dsc->base.layer->color_format == LV_COLOR_FORMAT_RGB888
                         || dsc->base.layer->color_format == LV_COLOR_FORMAT_RGB565))) {
                    return 0;
                }

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

static int32_t ppa_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
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

static int32_t ppa_delete(lv_draw_unit_t * draw_unit)
{
    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *)draw_unit;
    ppa_unregister_client(u->srm_client);
    ppa_unregister_client(u->fill_client);
    ppa_unregister_client(u->blend_client);
    return 0;
}

static void ppa_execute_drawing(lv_draw_ppa_unit_t * u)
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
        default:
            break;
    }
}

#endif /*LV_USE_PPA*/
