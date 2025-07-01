/**
 * @file lv_draw_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_private.h"
#include "../lv_draw_private.h"
#if LV_USE_DRAW_SW

#include "../../core/lv_refr.h"
#include "../../display/lv_display_private.h"
#include "../../stdlib/lv_string.h"
#include "../../core/lv_global.h"
#include "../../misc/lv_area_private.h"

#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    #if LV_USE_THORVG_EXTERNAL
        #include <thorvg_capi.h>
    #else
        #include "../../libs/thorvg/thorvg_capi.h"
    #endif
#endif

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM
    #include "arm2d/lv_draw_sw_helium.h"
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
    #include LV_DRAW_SW_ASM_CUSTOM_INCLUDE
#endif

#if LV_DRAW_SW_DRAW_UNIT_CNT > 1 && LV_USE_OS == LV_OS_NONE
    #error "OS support is required when more than one SW rendering units are enabled"
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

static void execute_drawing(lv_draw_task_t * t);

static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t lv_draw_sw_delete(lv_draw_unit_t * draw_unit);
#if LV_USE_PARALLEL_DRAW_DEBUG
    static void parallel_debug_draw(lv_draw_task_t * t, uint32_t idx);
#endif

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

    lv_draw_sw_unit_t * draw_sw_unit = lv_draw_create_unit(sizeof(lv_draw_sw_unit_t));
    draw_sw_unit->base_unit.dispatch_cb = dispatch;
    draw_sw_unit->base_unit.evaluate_cb = evaluate;
    draw_sw_unit->base_unit.delete_cb = LV_USE_OS ? lv_draw_sw_delete : NULL;
#if LV_USE_DRAW_ARM2D_SYNC
    draw_sw_unit->base_unit.name = "SW_ARM2D";
#else
    draw_sw_unit->base_unit.name = "SW";
#endif

#if LV_USE_OS
    uint32_t i;
    for(i = 0; i < LV_DRAW_SW_DRAW_UNIT_CNT; i++) {
        lv_draw_sw_thread_dsc_t * thread_dsc = &draw_sw_unit->thread_dscs[i];
        thread_dsc->idx = i;
        thread_dsc->draw_unit = (void *) draw_sw_unit;
        lv_thread_init(&thread_dsc->thread, "swdraw", LV_DRAW_THREAD_PRIO, render_thread_cb,
                       LV_DRAW_THREAD_STACK_SIZE, thread_dsc);
    }
#endif

#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    if(LV_DRAW_SW_DRAW_UNIT_CNT > 1) {
        tvg_engine_init(TVG_ENGINE_SW, LV_DRAW_SW_DRAW_UNIT_CNT);
    }
    else {
        tvg_engine_init(TVG_ENGINE_SW, 0);
    }
#endif

    lv_ll_init(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, sizeof(lv_draw_sw_custom_blend_handler_t));
}

void lv_draw_sw_deinit(void)
{
#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
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

    uint32_t i;
    for(i = 0; i < LV_DRAW_SW_DRAW_UNIT_CNT; i++) {
        lv_draw_sw_thread_dsc_t * thread_dsc = &draw_sw_unit->thread_dscs[i];

        LV_LOG_INFO("cancel software rendering thread");
        thread_dsc->exit_status = true;

        if(thread_dsc->inited) {
            lv_thread_sync_signal(&thread_dsc->sync);
        }
        lv_thread_delete(&thread_dsc->thread);
    }

    return 0;
#else
    LV_UNUSED(draw_unit);
    return 0;
#endif
}

bool lv_draw_sw_register_blend_handler(lv_draw_sw_custom_blend_handler_t * handler)
{
    lv_draw_sw_custom_blend_handler_t * existing_handler = NULL;
    lv_draw_sw_custom_blend_handler_t * new_handler = NULL;

    // Check if a handler is already registered for the color format
    LV_LL_READ(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, existing_handler) {
        if(existing_handler->dest_cf == handler->dest_cf) {
            new_handler = existing_handler;
            break;
        }
    }

    if(new_handler == NULL) {
        new_handler = lv_ll_ins_head(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll);
        if(new_handler == NULL) {
            LV_ASSERT_MALLOC(new_handler);
            return false;
        }
    }

    lv_memcpy(new_handler, handler, sizeof(lv_draw_sw_custom_blend_handler_t));
    return true;
}

bool lv_draw_sw_unregister_blend_handler(lv_color_format_t dest_cf)
{
    lv_draw_sw_custom_blend_handler_t * handler;

    LV_LL_READ(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, handler) {
        if(handler->dest_cf == dest_cf) {
            lv_ll_remove(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, handler);
            lv_free(handler);
            return true;
        }
    }

    return false;
}

lv_draw_sw_blend_handler_t lv_draw_sw_get_blend_handler(lv_color_format_t dest_cf)
{
    lv_draw_sw_custom_blend_handler_t * handler;

    LV_LL_READ(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, handler) {
        if(handler->dest_cf == dest_cf) {
            return handler->handler;
        }
    }

    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    LV_UNUSED(draw_unit);

    switch(task->type) {
        case LV_DRAW_TASK_TYPE_IMAGE:
        case LV_DRAW_TASK_TYPE_LAYER: {
                lv_draw_image_dsc_t * draw_dsc = task->draw_dsc;

                /* not support skew */
                if(draw_dsc->skew_x != 0 || draw_dsc->skew_y != 0) {
                    return 0;
                }

                bool masked = draw_dsc->bitmap_mask_src != NULL;

                lv_color_format_t cf = draw_dsc->header.cf;
                if(masked && (cf == LV_COLOR_FORMAT_A8 || cf == LV_COLOR_FORMAT_RGB565A8)) {
                    return 0;
                }

                if(cf >= LV_COLOR_FORMAT_PROPRIETARY_START) {
                    return 0;
                }
            }
            break;
#if LV_USE_3DTEXTURE
        case LV_DRAW_TASK_TYPE_3D:
            return 0;
#endif
        default:
            break;
    }

    if(task->preference_score >= 100) {
        task->preference_score = 100;
        task->preferred_draw_unit_id = DRAW_UNIT_ID_SW;
    }

    return 0;
}

static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_sw_unit_t * draw_sw_unit = (lv_draw_sw_unit_t *) draw_unit;

#if LV_USE_OS
    uint32_t i;
    uint32_t taken_cnt = 0;
    /* All idle (couldn't take any tasks): return LV_DRAW_UNIT_IDLE;
     * All busy: return 0; as 0 tasks were taken
     * Otherwise return taken_cnt;
     */

    /*If at least one is busy, it's not all idle*/
    bool all_idle = true;
    for(i = 0; i < LV_DRAW_SW_DRAW_UNIT_CNT; i++) {
        if(draw_sw_unit->thread_dscs[i].task_act) {
            all_idle = false;
            break;
        }
    }

    lv_draw_task_t * t = NULL;
    for(i = 0; i < LV_DRAW_SW_DRAW_UNIT_CNT; i++) {
        lv_draw_sw_thread_dsc_t * thread_dsc = &draw_sw_unit->thread_dscs[i];

        /*Do nothing if busy*/
        if(thread_dsc->task_act) continue;

        /*Find an available task. Start from the previously taken task.*/
        t = lv_draw_get_next_available_task(layer, t, DRAW_UNIT_ID_SW);

        /*If there is not available task don't try other threads as there won't be available
         *tasks for then either*/
        if(t == NULL) {
            LV_PROFILER_DRAW_END;
            if(all_idle) return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
            else return taken_cnt;
        }

        /*Allocate a buffer if not done yet.*/
        void * buf = lv_draw_layer_alloc_buf(layer);
        /*Do not return is failed. The other thread might already have a buffer can do something. */
        if(buf == NULL) continue;

        /*Take the task*/
        all_idle = false;
        taken_cnt++;
        t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
        thread_dsc->task_act = t;

        /*Let the render thread work*/
        if(thread_dsc->inited) lv_thread_sync_signal(&thread_dsc->sync);
    }

    if(all_idle) return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
    else return taken_cnt;

#else
    /*Return immediately if it's busy with draw task*/
    if(draw_sw_unit->task_act) {
        LV_PROFILER_DRAW_END;
        return 0;
    }

    lv_draw_task_t * t = NULL;
    t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_SW);
    if(t == NULL) {
        LV_PROFILER_DRAW_END;
        return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
    }

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL) {
        LV_PROFILER_DRAW_END;
        return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_sw_unit->task_act = t;

    execute_drawing(t);
    draw_sw_unit->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
    draw_sw_unit->task_act = NULL;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();

    LV_PROFILER_DRAW_END;
    return 1;
#endif

}

#if LV_USE_OS
static void render_thread_cb(void * ptr)
{
    lv_draw_sw_thread_dsc_t * thread_dsc = ptr;

    lv_thread_sync_init(&thread_dsc->sync);
    thread_dsc->inited = true;

    while(1) {
        while(thread_dsc->task_act == NULL) {
            if(thread_dsc->exit_status) {
                break;
            }
            lv_thread_sync_wait(&thread_dsc->sync);
        }

        if(thread_dsc->exit_status) {
            LV_LOG_INFO("ready to exit software rendering thread");
            break;
        }

        execute_drawing(thread_dsc->task_act);
#if LV_USE_PARALLEL_DRAW_DEBUG
        parallel_debug_draw(thread_dsc->task_act, thread_dsc->idx);
#endif
        thread_dsc->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
        thread_dsc->task_act = NULL;

        /*The draw unit is free now. Request a new dispatching as it can get a new task*/
        lv_draw_dispatch_request();

    }

    thread_dsc->inited = false;
    lv_thread_sync_delete(&thread_dsc->sync);
    LV_LOG_INFO("exit software rendering thread");
}
#endif

static void execute_drawing(lv_draw_task_t * t)
{
    LV_PROFILER_DRAW_BEGIN;
    /*Render the draw task*/
    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_sw_fill(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_sw_border(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BOX_SHADOW:
            lv_draw_sw_box_shadow(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LETTER:
            lv_draw_sw_letter(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LABEL:
            lv_draw_sw_label(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_sw_image(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_ARC:
            lv_draw_sw_arc(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LINE:
            lv_draw_sw_line(t, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_TRIANGLE:
            lv_draw_sw_triangle(t, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_sw_layer(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
            lv_draw_sw_mask_rect(t, t->draw_dsc);
            break;
#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
        case LV_DRAW_TASK_TYPE_VECTOR:
            lv_draw_sw_vector(t, t->draw_dsc);
            break;
#endif
        default:
            break;
    }


    LV_PROFILER_DRAW_END;
}

#if LV_USE_PARALLEL_DRAW_DEBUG
static void parallel_debug_draw(lv_draw_task_t * t, uint32_t idx)
{
    /*Layers manage it for themselves*/
    if(t->type != LV_DRAW_TASK_TYPE_LAYER) {
        lv_area_t draw_area;
        if(!lv_area_intersect(&draw_area, &t->area, &t->clip_area)) return;

        lv_draw_fill_dsc_t fill_dsc;
        lv_draw_fill_dsc_init(&fill_dsc);
        fill_dsc.color = lv_palette_main(idx % LV_PALETTE_LAST);
        fill_dsc.opa = LV_OPA_10;
        lv_draw_sw_fill(t, &fill_dsc, &draw_area);

        lv_draw_border_dsc_t border_dsc;
        lv_draw_border_dsc_init(&border_dsc);
        border_dsc.color = lv_palette_main(idx % LV_PALETTE_LAST);
        border_dsc.opa = LV_OPA_60;
        border_dsc.width = 1;
        lv_draw_sw_border(t, &border_dsc, &draw_area);

        lv_point_t txt_size;
        lv_text_get_size(&txt_size, "W", LV_FONT_DEFAULT, 0, 0, 100, LV_TEXT_FLAG_NONE);

        lv_area_t txt_area;
        txt_area.x1 = draw_area.x1;
        txt_area.y1 = draw_area.y1;
        txt_area.x2 = draw_area.x1 + txt_size.x - 1;
        txt_area.y2 = draw_area.y1 + txt_size.y - 1;

        lv_draw_fill_dsc_init(&fill_dsc);
        fill_dsc.color = lv_color_white();
        lv_draw_sw_fill(t, &fill_dsc, &txt_area);

        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%d", idx);
        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lv_color_black();
        label_dsc.text = buf;
        lv_draw_sw_label(t, &label_dsc, &txt_area);
    }
}
#endif


#endif /*LV_USE_DRAW_SW*/
