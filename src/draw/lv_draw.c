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

int r = 0;

void lv_draw_dispatch(lv_draw_ctx_t * draw_ctx)
{
    //    printf("Dispatch start\n");
    r = 1;
    draw_ctx->dispatch_req = 0;
    /*Remove the finished tasks first*/
    lv_draw_task_t * t_prev = NULL;
    lv_draw_task_t * t = draw_ctx->draw_task_head;
    while(t) {
        lv_draw_task_t * t_next = t->next;
        if(t->state == LV_DRAW_TASK_STATE_READY) {
            printf("Remov(%p, %p)\n", t, t->draw_dsc);
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

    while(t) {
        if(t == t->next) {
            printf("List error\n");
        }
        t = t->next;
    }

    /*Find a draw unit which is not busy and can take at least one task*/
    /*Let all draw units to pick draw tasks*/
    lv_draw_unit_t * u = draw_ctx->draw_unit_head;
    while(u) {
        u->dispatch(u, draw_ctx);
        u = u->next;
    }

    r = 0;
    //    printf("Dispatch end\n");

}

void lv_draw_dispatch_if_requested(lv_draw_ctx_t * draw_ctx)
{
    //    if(draw_ctx->dispatch_req)
    lv_draw_dispatch(draw_ctx);
}

void lv_draw_dispatch_request(lv_draw_ctx_t * draw_ctx)
{
    draw_ctx->dispatch_req = 1;
    //   printf("Disp req.\n");
    if(r) {
        printf("Disp req. concurrent: %d\n", r);
    }
}

lv_draw_task_t * lv_draw_get_next_available_task(lv_draw_ctx_t * draw_ctx, lv_draw_task_t * t_prev)
{
    lv_draw_task_t * t = t_prev ? t_prev->next : draw_ctx->draw_task_head;

    while(t) {
        if(t->state == LV_DRAW_TASK_STATE_QUEUED && is_independent(draw_ctx, t)) {
            return t;
        }

        t = t->next;
    }

    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool is_independent(lv_draw_ctx_t * draw_ctx, lv_draw_task_t * t_end)
{
    lv_draw_task_t * t = draw_ctx->draw_task_head;

    /*If t_end is outside of the older tasks then it's independent*/
    while(t && t != t_end) {
        lv_area_t res;
        if(_lv_area_intersect(&res, &t_end->area, &t->area)) return false;
        t = t->next;
    }

    return true;
}

