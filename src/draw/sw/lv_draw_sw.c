/**
 * @file lv_draw_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw.h"
#if LV_USE_DRAW_SW

#include "../../core/lv_refr.h"
#include "lv_draw_sw.h"
#include "../../display/lv_display_private.h"
#include "../../stdlib/lv_string.h"
#include "../../core/lv_global.h"

#if LV_USE_VECTOR_GRAPHIC && (LV_USE_THORVG_EXTERNAL || LV_USE_THORVG_INTERNAL)
    #if LV_USE_THORVG_EXTERNAL
        #include <thorvg_capi.h>
    #else
        #include "../../libs/thorvg/thorvg_capi.h"
    #endif
#endif
/*********************
 *      DEFINES
 *********************/
#define DRAW_UNIT_ID_SW     1

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_USE_OS
    static void render_thread_cb(void * ptr);
#endif

static void execute_drawing(lv_draw_sw_unit_t * u);

static int32_t lv_draw_sw_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t lv_draw_sw_delete(lv_draw_unit_t * draw_unit);

/**********************
 *  STATIC VARIABLES
 **********************/
#define _draw_info LV_GLOBAL_DEFAULT()->draw_info

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_init(void)
{

#if LV_DRAW_SW_COMPLEX == 1
    lv_draw_sw_mask_init();
#endif

    uint32_t i;
    for(i = 0; i < LV_DRAW_SW_DRAW_UNIT_CNT; i++) {
        lv_draw_sw_unit_t * draw_sw_unit = lv_draw_create_unit(sizeof(lv_draw_sw_unit_t));
        draw_sw_unit->base_unit.dispatch_cb = lv_draw_sw_dispatch;
        draw_sw_unit->idx = i;
        draw_sw_unit->base_unit.delete_cb = LV_USE_OS ? lv_draw_sw_delete : NULL;

#if LV_USE_OS
        lv_thread_init(&draw_sw_unit->thread, LV_THREAD_PRIO_HIGH, render_thread_cb, 8 * 1024, draw_sw_unit);
#endif
    }

#if LV_USE_VECTOR_GRAPHIC && (LV_USE_THORVG_EXTERNAL || LV_USE_THORVG_INTERNAL)
    tvg_engine_init(TVG_ENGINE_SW, 0);
#endif
}

void lv_draw_sw_deinit(void)
{
#if LV_USE_VECTOR_GRAPHIC && (LV_USE_THORVG_EXTERNAL || LV_USE_THORVG_INTERNAL)
    tvg_engine_term(TVG_ENGINE_SW);
#endif

#if LV_DRAW_SW_COMPLEX == 1
    lv_draw_sw_mask_deinit();
#endif
}

static int32_t lv_draw_sw_delete(lv_draw_unit_t * draw_unit)
{
#if LV_USE_OS
    lv_draw_sw_unit_t * draw_sw_unit = (lv_draw_sw_unit_t *) draw_unit;

    LV_LOG_INFO("cancel software rendering thread");
    draw_sw_unit->exit_status = true;

    if(draw_sw_unit->inited) {
        lv_thread_sync_signal(&draw_sw_unit->sync);
    }

    return lv_thread_delete(&draw_sw_unit->thread);
#else
    LV_UNUSED(draw_unit);
    return 0;
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t lv_draw_sw_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_sw_unit_t * draw_sw_unit = (lv_draw_sw_unit_t *) draw_unit;

    /*Return immediately if it's busy with draw task*/
    if(draw_sw_unit->task_act) return 0;

    lv_draw_task_t * t = NULL;
    t = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_SW);
    if(t == NULL) return -1;

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL) return -1;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_sw_unit->base_unit.target_layer = layer;
    draw_sw_unit->base_unit.clip_area = &t->clip_area;
    draw_sw_unit->task_act = t;

#if LV_USE_OS
    /*Let the render thread work*/
    if(draw_sw_unit->inited) lv_thread_sync_signal(&draw_sw_unit->sync);
#else
    execute_drawing(draw_sw_unit);

    draw_sw_unit->task_act->state = LV_DRAW_TASK_STATE_READY;
    draw_sw_unit->task_act = NULL;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();

#endif
    return 1;
}

#if LV_USE_OS
static void render_thread_cb(void * ptr)
{
    lv_draw_sw_unit_t * u = ptr;

    lv_thread_sync_init(&u->sync);
    u->inited = true;

    while(1) {
        while(u->task_act == NULL) {
            if(u->exit_status) {
                break;
            }
            lv_thread_sync_wait(&u->sync);
        }

        if(u->exit_status) {
            LV_LOG_INFO("ready to exit software rendering thread");
            break;
        }

        execute_drawing(u);

        /*Cleanup*/
        u->task_act->state = LV_DRAW_TASK_STATE_READY;
        u->task_act = NULL;

        /*The draw unit is free now. Request a new dispatching as it can get a new task*/
        lv_draw_dispatch_request();
    }

    u->inited = false;
    lv_thread_sync_delete(&u->sync);
    LV_LOG_INFO("exit software rendering thread");
}
#endif

static void execute_drawing(lv_draw_sw_unit_t * u)
{
    /*Render the draw task*/
    lv_draw_task_t * t = u->task_act;
    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_sw_fill((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_sw_border((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BOX_SHADOW:
            lv_draw_sw_box_shadow((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BG_IMG:
            lv_draw_sw_bg_image((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LABEL:
            lv_draw_sw_label((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_sw_image((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_ARC:
            lv_draw_sw_arc((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LINE:
            lv_draw_sw_line((lv_draw_unit_t *)u, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_TRIANGLE:
            lv_draw_sw_triangle((lv_draw_unit_t *)u, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_sw_layer((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
            lv_draw_sw_mask_rect((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
#if LV_USE_VECTOR_GRAPHIC
        case LV_DRAW_TASK_TYPE_VECTOR:
            lv_draw_sw_vector((lv_draw_unit_t *)u, t->draw_dsc);
            break;
#endif
        default:
            break;
    }

#if LV_USE_PARALLEL_DRAW_DEBUG
    /*Layers manage it for themselves*/
    if(t->type != LV_DRAW_TASK_TYPE_LAYER) {
        lv_area_t draw_area;
        if(!_lv_area_intersect(&draw_area, &t->area, u->base_unit.clip_area)) return;

        int32_t idx = 0;
        lv_draw_unit_t * draw_unit_tmp = _draw_info.unit_head;
        while(draw_unit_tmp != (lv_draw_unit_t *)u) {
            draw_unit_tmp = draw_unit_tmp->next;
            idx++;
        }
        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = lv_palette_main(idx % _LV_PALETTE_LAST);
        rect_dsc.border_color = rect_dsc.bg_color;
        rect_dsc.bg_opa = LV_OPA_10;
        rect_dsc.border_opa = LV_OPA_80;
        rect_dsc.border_width = 1;
        lv_draw_sw_fill((lv_draw_unit_t *)u, &rect_dsc, &draw_area);

        lv_point_t txt_size;
        lv_text_get_size(&txt_size, "W", LV_FONT_DEFAULT, 0, 0, 100, LV_TEXT_FLAG_NONE);

        lv_area_t txt_area;
        txt_area.x1 = draw_area.x1;
        txt_area.y1 = draw_area.y1;
        txt_area.x2 = draw_area.x1 + txt_size.x - 1;
        txt_area.y2 = draw_area.y1 + txt_size.y - 1;

        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = lv_color_white();
        lv_draw_sw_fill((lv_draw_unit_t *)u, &rect_dsc, &txt_area);

        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%d", idx);
        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lv_color_black();
        label_dsc.text = buf;
        lv_draw_sw_label((lv_draw_unit_t *)u, &label_dsc, &txt_area);
    }
#endif

}

void lv_draw_sw_rgb565_swap(void * buf, int32_t buf_size_px)
{
    uint32_t u32_cnt = buf_size_px / 2;
    uint16_t * buf16 = buf;
    uint32_t * buf32 = buf;

    while(u32_cnt >= 8) {
        buf32[0] = ((uint32_t)(buf32[0] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[0] & 0x00ff00ff) << 8);
        buf32[1] = ((uint32_t)(buf32[1] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[1] & 0x00ff00ff) << 8);
        buf32[2] = ((uint32_t)(buf32[2] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[2] & 0x00ff00ff) << 8);
        buf32[3] = ((uint32_t)(buf32[3] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[3] & 0x00ff00ff) << 8);
        buf32[4] = ((uint32_t)(buf32[4] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[4] & 0x00ff00ff) << 8);
        buf32[5] = ((uint32_t)(buf32[5] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[5] & 0x00ff00ff) << 8);
        buf32[6] = ((uint32_t)(buf32[6] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[6] & 0x00ff00ff) << 8);
        buf32[7] = ((uint32_t)(buf32[7] & 0xff00ff00) >> 8) + ((uint32_t)(buf32[7] & 0x00ff00ff) << 8);
        buf32 += 8;
        u32_cnt -= 8;
    }

    while(u32_cnt) {
        *buf32 = ((uint32_t)(*buf32 & 0xff00ff00) >> 8) + ((uint32_t)(*buf32 & 0x00ff00ff) << 8);
        buf32++;
        u32_cnt--;
    }

    if(buf_size_px & 0x1) {
        uint32_t e = buf_size_px - 1;
        buf16[e] = ((buf16[e] & 0xff00) >> 8) + ((buf16[e] & 0x00ff) << 8);
    }

    return;
}

#endif /*LV_USE_DRAW_SW*/
