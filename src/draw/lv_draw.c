/**
 * @file lv_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "sw/lv_draw_sw.h"
#include "../core/lv_disp_private.h"
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
static bool is_independent(lv_draw_ctx_t * draw_ctx, lv_draw_task_t * t_check);

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_OS
    static lv_thread_sync_t sync;
#else
    static int dispatch_req = 0;
#endif

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
#if LV_USE_OS
    lv_thread_sync_init(&sync);
#endif
}

lv_draw_task_t * lv_draw_add_task(lv_draw_ctx_t * draw_ctx, const lv_area_t * coords)
{
    lv_draw_task_t * new_task = lv_malloc(sizeof(lv_draw_task_t));
    lv_memzero(new_task, sizeof(*new_task));

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

void lv_draw_dispatch(void)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_draw_ctx_t * draw_ctx = disp->draw_ctx_head;
    while(draw_ctx) {
        /*Remove the finished tasks first*/
        lv_draw_task_t * t_prev = NULL;
        lv_draw_task_t * t = draw_ctx->draw_task_head;
        while(t) {
            lv_draw_task_t * t_next = t->next;
            if(t->state == LV_DRAW_TASK_STATE_READY) {
                if(t_prev) t_prev->next = t->next;
                else draw_ctx->draw_task_head = t_next;

                /*If it was layer drawing free the draw_ctx too*/
                if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
                    lv_draw_img_dsc_t * draw_img_dsc = t->draw_dsc;
                    lv_draw_ctx_t * draw_ctx_drawn = (lv_draw_ctx_t *)draw_img_dsc->src;

                    /*Remove the draw_ctx from  the display's*/
                    lv_draw_ctx_t * c = disp->draw_ctx_head;
                    while(c) {
                        if(c->next == draw_ctx_drawn) {
                            c->next = draw_ctx_drawn->next;
                            break;
                        }
                        c = c->next;
                    }

                    lv_free(draw_ctx_drawn->buf);
                    disp->draw_ctx_deinit(disp, draw_ctx_drawn);
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
        if(draw_ctx->parent && draw_ctx->all_tasks_added && draw_ctx->draw_task_head == NULL) {
            /*Find a draw task with TYPE_LAYER in the draw_ctx where the is the src is this draw_ctx*/
            lv_draw_task_t * t = draw_ctx->parent->draw_task_head;
            while(t) {
                if(t->type == LV_DRAW_TASK_TYPE_LAYER && t->state == LV_DRAW_TASK_STATE_WAITING) {
                    lv_draw_img_dsc_t * draw_dsc = t->draw_dsc;
                    if(draw_dsc->src == draw_ctx) {
                        t->state = LV_DRAW_TASK_STATE_QUEUED;
                        lv_draw_dispatch_request();
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
                if(taken_cnt < 0) break;
                u = u->next;
            }
        }
        draw_ctx = draw_ctx->next;
    }
}

void lv_draw_dispatch_wait_for_request(void)
{
#if LV_USE_OS
    lv_thread_sync_wait(&sync);
#else
    while(!dispatch_req);
    dispatch_req = 0;
#endif
}

void lv_draw_dispatch_request(void)
{
#if LV_USE_OS
    lv_thread_sync_signal(&sync);
#else
    dispatch_req = 1;
#endif
}

lv_draw_task_t * lv_draw_get_next_available_task(lv_draw_ctx_t * draw_ctx, lv_draw_task_t * t_prev)
{
    /*If the first task is screen sized, there cannot be independent areas*/
    if(draw_ctx->draw_task_head) {
        lv_coord_t hor_res = lv_disp_get_hor_res(_lv_refr_get_disp_refreshing());
        lv_coord_t ver_res = lv_disp_get_ver_res(_lv_refr_get_disp_refreshing());
        lv_draw_task_t * t = draw_ctx->draw_task_head;
        if(t->state != LV_DRAW_TASK_STATE_QUEUED &&
           t->area.x1 <= 0 && t->area.x2 >= hor_res - 1 &&
           t->area.y1 <= 0 && t->area.y2 >= ver_res - 1) {
            return NULL;
        }
    }

    lv_draw_task_t * t = t_prev ? t_prev->next : draw_ctx->draw_task_head;
    while(t) {
        /*Find a queued and independent task*/
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

/**
 * Check if there are older draw task overlapping the area of `t_check`
 * @param draw_ctx      the draw ctx to search in
 * @param t_check       check this task if it overlaps with the older ones
 * @return              true: `t_check` is not overlapping with older tasks so it's independent
 */
static bool is_independent(lv_draw_ctx_t * draw_ctx, lv_draw_task_t * t_check)
{
    lv_draw_task_t * t = draw_ctx->draw_task_head;

    /*If t_check is outside of the older tasks then it's independent*/
    while(t && t != t_check) {
        if(t->state != LV_DRAW_TASK_STATE_READY) {
            lv_area_t a;
            if(_lv_area_intersect(&a, &t->area, &t_check->area)) {
                return false;
            }
        }
        t = t->next;
    }

    return true;
}

