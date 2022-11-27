/**
 * @file lv_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "sw/lv_draw_sw.h"
#include "../core/lv_disp_priv.h"
#include "../core/lv_refr.h"

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

lv_draw_task_t * lv_draw_add_task(lv_draw_ctx_t * draw_ctx, const lv_area_t * coords)
{
    lv_draw_task_t * new_task = lv_malloc(sizeof(lv_draw_task_t));
    lv_memzero(new_task, sizeof(*new_task));

#if DRAW_LOG
    printf("Add  (%p, %p): %d, %d, %d, %d\n", new_task, new_task->draw_dsc, coords->x1, coords->y1,
           lv_area_get_width(coords), lv_area_get_height(coords));
#endif

    new_task->area = *coords;
    new_task->clip_area = draw_ctx->clip_area;
    new_task->state = LV_DRAW_TASK_STATE_QUEUED;

    /*Find the tail*/
    if(draw_ctx->draw_task_head == NULL) {
        draw_ctx->draw_task_head = new_task;
    }
    else {
        lv_draw_task_t * tail = draw_ctx->draw_task_head;
        while(tail->next) tail = tail->next;

        tail->next = new_task;
    }

    return new_task;
}

void lv_draw_dispatch(lv_draw_ctx_t * draw_ctx)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
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

            if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
                lv_draw_img_dsc_t * draw_img_dsc = t->draw_dsc;
                lv_draw_ctx_t * draw_ctx_drawn = (lv_draw_ctx_t *)draw_img_dsc->src;
                /*If it was layer drawing free the draw_ctx too*/
                lv_free(draw_ctx_drawn->buf);
                disp->draw_ctx_deinit(disp, draw_ctx_drawn);

                /*Remove the draw_ctx from  the display's*/
                lv_draw_ctx_t * c = disp->draw_ctx_head;
                while(c) {
                    if(c->next == draw_ctx_drawn) {
                        c->next = draw_ctx_drawn->next;
                        break;
                    }
                    c = c->next;
                }

                lv_free(draw_ctx_drawn);
            }

            lv_free(t->draw_dsc);
            lv_free(t);
        }
        else {
            t_prev = t;
        }
        t = t_next;
    }

    /*This draw_ctx is ready, enable blending its buffer*/
    if(draw_ctx->all_tasks_added && draw_ctx->draw_task_head == NULL) {
        /*If there is no parent then this is the root draw_ctx and there is nothing to do when its empty*/
        if(draw_ctx->parent == NULL) return;

        /*Find a draw task with LAYER in the draw_ctx where the is the src is this draw_ctx*/
        lv_draw_task_t * t = draw_ctx->parent->draw_task_head;
        while(t) {
            if(t->type == LV_DRAW_TASK_TYPE_LAYER && t->state == LV_DRAW_TASK_STATE_WAITING) {
                lv_draw_img_dsc_t * draw_dsc = t->draw_dsc;
                if(draw_dsc->src == draw_ctx) {
                    t->state = LV_DRAW_TASK_STATE_QUEUED;
                    lv_draw_dispatch_request(draw_ctx->parent);
                    break;
                }
            }
            t = t->next;
        }
    }
    /*Assign draw tasks to the draw_units*/
    else {
        /*Find a draw unit which is not busy and can take at least one task*/
        /*Let all draw units to pick draw tasks*/
        lv_draw_unit_t * u = disp->draw_unit_head;
        while(u) {
            int32_t taken_cnt = u->dispatch(u, draw_ctx);
            if(taken_cnt < 0) return;
            u = u->next;
        }
    }
}

void lv_draw_dispatch_if_requested(void)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_draw_ctx_t * draw_ctx = disp->draw_ctx_head;
    while(draw_ctx) {
        if(draw_ctx->dispatch_req) {
            lv_draw_dispatch(draw_ctx);
        }
        draw_ctx = draw_ctx->next;
    }
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

