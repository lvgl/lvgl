/**
 * @file lv_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool is_independent(lv_draw_ctx_t * draw_ctx, lv_draw_task_t * t_end);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_init(void)
{
    /*Nothing to init now*/
}

void lv_draw_wait_for_finish(lv_draw_ctx_t * draw_ctx)
{
    //    if(draw_ctx->wait_for_finish) draw_ctx->wait_for_finish(draw_ctx);
}

void lv_draw_dispatch(lv_draw_ctx_t * draw_ctx)
{
    draw_ctx->dispatch_req = 0;
    /*Remove the finished tasks first*/
    lv_draw_task_t * t_prev = NULL;
    lv_draw_task_t * t = draw_ctx->draw_task_head;
    while(t) {
        lv_draw_task_t * t_next = t->next;
        if(t->state == LV_DRAW_TASK_STATE_READY) {
#if DRAW_LOG
            printf("Remov(%p)\n", t);
#endif
            if(t_prev) t_prev->next = t->next;
            else draw_ctx->draw_task_head = t_next;
            lv_free(t->draw_dsc);
            lv_free(t);
        }
        else {
            t_prev = t;
        }
        t = t_next;
    }

    /*Find a draw unit which is not busy and can take at least one task*/
    /*Let all draw units to pick draw tasks*/
    lv_draw_unit_t * u = draw_ctx->draw_unit_head;
    while(u) {
        int32_t taken_cnt = u->dispatch(u, draw_ctx);
        if(taken_cnt < 0) return;
        u = u->next;
    }
}

void lv_draw_dispatch_if_requested(lv_draw_ctx_t * draw_ctx)
{
    if(draw_ctx->dispatch_req)
        lv_draw_dispatch(draw_ctx);
}

void lv_draw_dispatch_request(lv_draw_ctx_t * draw_ctx)
{
    draw_ctx->dispatch_req = 1;
}

lv_draw_task_t * lv_draw_get_next_available_task(lv_draw_ctx_t * draw_ctx, lv_draw_task_t * t_prev)
{
    if(draw_ctx->draw_task_head) {
        lv_draw_task_t * t = draw_ctx->draw_task_head;
        if(t->state != LV_DRAW_TASK_STATE_QUEUED && t->area.x1 <= 0 && t->area.x2 >= 799 && t->area.y1 <= 0 &&
           t->area.y2 >= 479) {
            //            printf("screen early exit (t: %d)\n", lv_tick_get());
            return NULL;
        }
    }

    lv_draw_task_t * t = t_prev ? t_prev->next : draw_ctx->draw_task_head;
    int c = 0;
    while(t) {
        if(t->state == LV_DRAW_TASK_STATE_QUEUED && is_independent(draw_ctx, t)) {
            //            printf("available: %d\n", c);
            return t;
        }
        c++;
        t = t->next;
    }

    if(c > 20) {
        //        return NULL;
    }
    //    printf("not available (t: %d): %d\n", lv_tick_get(), c);
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool is_independent(lv_draw_ctx_t * draw_ctx, lv_draw_task_t * t_end)
{
    lv_draw_task_t * t = draw_ctx->draw_task_head;

    int c = 0;
    /*If t_end is outside of the older tasks then it's independent*/
    while(t && t != t_end) {
        c++;
        if(t->state != LV_DRAW_TASK_STATE_READY) {
            if(_lv_area_is_on(&t_end->area, &t->area)) {
                //                printf("ind false: %d\n", c);
                return false;
            }
        }
        t = t->next;
    }

    //    printf("ind true: %d\n", c);
    return true;
}

