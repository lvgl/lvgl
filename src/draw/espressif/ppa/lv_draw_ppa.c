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
static bool ppa_isr(ppa_client_handle_t ppa_client, ppa_event_data_t * event_data, void * user_data);

#if LV_PPA_NONBLOCKING_OPS
    static void ppa_thread(void * arg);
#endif

static bool g_ppa_complete = true;

/**********************
*   GLOBAL FUNCTIONS
**********************/

void lv_draw_ppa_init(void)
{
    esp_err_t res;
    ppa_client_config_t cfg = {0};
    ppa_event_callbacks_t ppa_cbs = {
        .on_trans_done = ppa_isr,

    };

    /* Create draw unit */
    lv_draw_buf_ppa_init_handlers();
    lv_draw_ppa_unit_t * draw_ppa_unit = lv_draw_create_unit(sizeof(lv_draw_ppa_unit_t));
    draw_ppa_unit->base_unit.evaluate_cb = ppa_evaluate;
    draw_ppa_unit->base_unit.dispatch_cb  = ppa_dispatch;
    draw_ppa_unit->base_unit.delete_cb    = ppa_delete;
    draw_ppa_unit->base_unit.name         = "ESP_PPA";

    /* Register SRM client */
    cfg.oper_type = PPA_OPERATION_SRM;
    cfg.max_pending_trans_num = 8;
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
    cfg.data_burst_length = PPA_DATA_BURST_LENGTH_32;

    res = ppa_register_client(&cfg, &draw_ppa_unit->blend_client);
    LV_ASSERT(res == ESP_OK);

    ppa_client_register_event_callbacks(draw_ppa_unit->srm_client, &ppa_cbs);
    ppa_client_register_event_callbacks(draw_ppa_unit->fill_client, &ppa_cbs);
    ppa_client_register_event_callbacks(draw_ppa_unit->blend_client, &ppa_cbs);

#if LV_PPA_NONBLOCKING_OPS
    lv_result_t lv_res = lv_thread_init(&draw_ppa_unit->thread, "ppa_thread", LV_DRAW_THREAD_PRIO, ppa_thread, 8192,
                                        draw_ppa_unit);
    LV_ASSERT(lv_res == LV_RESULT_OK);
#endif

}

void lv_draw_ppa_deinit(void)
{
    /* No global deinit required */
}

/**********************
*   STATIC FUNCTIONS
**********************/

static bool ppa_isr(ppa_client_handle_t ppa_client, ppa_event_data_t * event_data, void * user_data)
{
    g_ppa_complete = true;

#if LV_PPA_NONBLOCKING_OPS
    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *)user_data;
    lv_thread_sync_signal_isr(&u->interrupt_signal);
#endif

    return false;
}

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
        if(!g_ppa_complete) {
            return LV_DRAW_UNIT_IDLE;
        }
        else {
            u->task_act->state = LV_DRAW_TASK_STATE_READY;
            u->task_act = NULL;
        }
    }

    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_PPA);
    if(!t || t->preferred_draw_unit_id != DRAW_UNIT_ID_PPA) return LV_DRAW_UNIT_IDLE;
    if(!lv_draw_layer_alloc_buf(layer)) return LV_DRAW_UNIT_IDLE;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    u->task_act = t;
    u->task_act->draw_unit = draw_unit;

    ppa_execute_drawing(u);

#if !LV_PPA_NONBLOCKING_OPS
    lv_draw_dispatch_request();
#endif

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
    lv_area_t draw_area;

    if(!lv_area_intersect(&area, &t->area, &t->clip_area)) return;

    lv_area_move(&draw_area, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_draw_buf_invalidate_cache(buf, &draw_area);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            g_ppa_complete = false;
            lv_draw_ppa_fill(t, (lv_draw_fill_dsc_t *)t->draw_dsc, &area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            g_ppa_complete = false;
            lv_draw_ppa_img(t, (lv_draw_image_dsc_t *)t->draw_dsc, &area);
            break;
        default:
            break;
    }
}

#if LV_PPA_NONBLOCKING_OPS
static void ppa_thread(void * arg)
{
    lv_draw_ppa_unit_t * u = arg;
    lv_thread_sync_init(&u->interrupt_signal);

    while(1) {
        do {
            lv_thread_sync_wait(&u->interrupt_signal);
        } while(u->task_act != NULL);

        u->task_act->state = LV_DRAW_TASK_STATE_READY;
        u->task_act = NULL;
        lv_draw_dispatch_request();
    }
}
#endif

#endif /*LV_USE_PPA*/
