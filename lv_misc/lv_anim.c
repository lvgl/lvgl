/**
 * @file anim.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim.h"

#if USE_LV_ANIMATION
#include <stddef.h>
#include <string.h>
#include "../lv_hal/lv_hal_tick.h"
#include "lv_task.h"
#include "lv_math.h"
#include "lv_gc.h"

#if defined(LV_GC_INCLUDE)
#   include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */


/*********************
 *      DEFINES
 *********************/
#define LV_ANIM_RESOLUTION      1024
#define LV_ANIM_RES_SHIFT       10

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_task(void * param);
static bool anim_ready_handler(lv_anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t last_task_run;
static bool anim_list_changed;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init. the animation module
 */
void lv_anim_init(void)
{
    lv_ll_init(&LV_GC_ROOT(_lv_anim_ll), sizeof(lv_anim_t));
    last_task_run = lv_tick_get();
    lv_task_create(anim_task, LV_REFR_PERIOD, LV_TASK_PRIO_MID, NULL);
}

/**
 * Create an animation
 * @param anim_p an initialized 'anim_t' variable. Not required after call.
 */
void lv_anim_create(lv_anim_t * anim_p)
{
    LV_LOG_TRACE("animation create started")
    /* Do not let two animations for the  same 'var' with the same 'fp'*/
    if(anim_p->fp != NULL) lv_anim_del(anim_p->var, anim_p->fp);       /*fp == NULL would delete all animations of var*/

    /*Add the new animation to the animation linked list*/
    lv_anim_t * new_anim = lv_ll_ins_head(&LV_GC_ROOT(_lv_anim_ll));
    lv_mem_assert(new_anim);
    if(new_anim == NULL) return;

    /*Initialize the animation descriptor*/
    anim_p->playback_now = 0;
    memcpy(new_anim, anim_p, sizeof(lv_anim_t));

    /*Set the start value*/
    if(new_anim->fp != NULL) new_anim->fp(new_anim->var, new_anim->start);

    /* Creating an animation changed the linked list.
     * It's important if it happens in a ready callback. (see `anim_task`)*/
    anim_list_changed = true;

    LV_LOG_TRACE("animation created")
}

/**
 * Delete an animation for a variable with a given animator function
 * @param var pointer to variable
 * @param fp a function pointer which is animating 'var',
 *           or NULL to delete all animations of 'var'
 * @return true: at least 1 animation is deleted, false: no animation is deleted
 */
bool lv_anim_del(void * var, lv_anim_fp_t fp)
{
    lv_anim_t * a;
    lv_anim_t * a_next;
    bool del = false;
    a = lv_ll_get_head(&LV_GC_ROOT(_lv_anim_ll));
    while(a != NULL) {
        /*'a' might be deleted, so get the next object while 'a' is valid*/
        a_next = lv_ll_get_next(&LV_GC_ROOT(_lv_anim_ll), a);

        if(a->var == var && (a->fp == fp || fp == NULL)) {
            lv_ll_rem(&LV_GC_ROOT(_lv_anim_ll), a);
            lv_mem_free(a);
            anim_list_changed = true;       /*Read by `anim_task`. It need to know if a delete occurred in the linked list*/
            del = true;
        }

        a = a_next;
    }

    return del;
}

/**
 * Get the number of currently running animations
 * @return the number of running animations
 */
uint16_t lv_anim_count_running(void)
{
    uint16_t cnt = 0;
    lv_anim_t * a;
    LL_READ(LV_GC_ROOT(_lv_anim_ll), a) cnt++;

    return cnt++;
}

/**
 * Calculate the time of an animation with a given speed and the start and end values
 * @param speed speed of animation in unit/sec
 * @param start start value of the animation
 * @param end end value of the animation
 * @return the required time [ms] for the animation with the given parameters
 */
uint16_t lv_anim_speed_to_time(uint16_t speed, int32_t start, int32_t end)
{
    int32_t d = LV_MATH_ABS((int32_t) start - end);
    uint32_t time = (int32_t)((int32_t)(d * 1000) / speed);

    if(time > UINT16_MAX) time = UINT16_MAX;

    if(time == 0) {
        time++;
    }

    return time;
}

/**
 * Calculate the current value of an animation applying linear characteristic
 * @param a pointer to an animation
 * @return the current value to set
 */
int32_t lv_anim_path_linear(const lv_anim_t * a)
{
    /*Calculate the current step*/
    uint16_t step;
    if(a->time == a->act_time) step = LV_ANIM_RESOLUTION; /*Use the last value if the time fully elapsed*/
    else step = (a->act_time * LV_ANIM_RESOLUTION) / a->time;

    /* Get the new value which will be proportional to `step`
     * and the `start` and `end` values*/
    int32_t new_value;
    new_value = (int32_t) step * (a->end - a->start);
    new_value = new_value >> LV_ANIM_RES_SHIFT;
    new_value += a->start;

    return new_value;
}

/**
 * Calculate the current value of an animation slowing down the start phase
 * @param a pointer to an animation
 * @return the current value to set
 */
int32_t lv_anim_path_ease_in(const lv_anim_t * a)
{
    /*Calculate the current step*/
    uint32_t t;
    if(a->time == a->act_time) t = 1024;
    else t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = lv_bezier3(t, 0, 1, 1, 1024);

    int32_t new_value;
    new_value = (int32_t) step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;


    return new_value;
}

/**
 * Calculate the current value of an animation slowing down the end phase
 * @param a pointer to an animation
 * @return the current value to set
 */
int32_t lv_anim_path_ease_out(const lv_anim_t * a)
{
    /*Calculate the current step*/

    uint32_t t;
    if(a->time == a->act_time) t = 1024;
    else t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = lv_bezier3(t, 0, 1023, 1023, 1024);

    int32_t new_value;
    new_value = (int32_t) step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;


    return new_value;
}

/**
 * Calculate the current value of an animation applying an "S" characteristic (cosine)
 * @param a pointer to an animation
 * @return the current value to set
 */
int32_t lv_anim_path_ease_in_out(const lv_anim_t * a)
{
    /*Calculate the current step*/

    uint32_t t;
    if(a->time == a->act_time) t = 1024;
    else t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = lv_bezier3(t, 0, 100, 924, 1024);

    int32_t new_value;
    new_value = (int32_t) step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;


    return new_value;
}

/**
 * Calculate the current value of an animation with overshoot at the end
 * @param a pointer to an animation
 * @return the current value to set
 */
int32_t lv_anim_path_overshoot(const lv_anim_t * a)
{
    /*Calculate the current step*/

    uint32_t t;
    if(a->time == a->act_time) t = 1024;
    else t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = lv_bezier3(t, 0, 600, 1300, 1024);

    int32_t new_value;
    new_value = (int32_t) step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;


    return new_value;
}

/**
 * Calculate the current value of an animation with 3 bounces
 * @param a pointer to an animation
 * @return the current value to set
 */
int32_t lv_anim_path_bounce(const lv_anim_t * a)
{
    /*Calculate the current step*/
    uint32_t t;
    if(a->time == a->act_time) t = 1024;
    else t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t diff = (a->end - a->start);

    /*3 bounces has 5 parts: 3 down and 2 up. One part is t / 5 long*/

    if(t < 408){
        /*Go down*/
        t = (t * 2500) >> 10;      /*[0..1024] range*/
    }
    else if(t >= 408 && t < 614) {
        /*First bounce back*/
        t -= 408;
        t = t * 5;      /*to [0..1024] range*/
        t = 1024 - t;
        diff = diff / 6;
    }
    else if(t >= 614 && t < 819) {
        /*Fall back*/
        t -= 614;
        t = t * 5;      /*to [0..1024] range*/
        diff = diff / 6;
    }
    else if(t >= 819 && t < 921) {
        /*Second bounce back*/
        t -= 819;
        t = t * 10;      /*to [0..1024] range*/
        t = 1024 - t;
        diff = diff / 16;
    }
    else if(t >= 921 && t <= 1024) {
        /*Fall back*/
        t -= 921;
        t = t * 10;      /*to [0..1024] range*/
        diff = diff / 16;
    }

    if(t > 1024) t = 1024;

    int32_t step = lv_bezier3(t, 1024, 1024, 800, 0);

    int32_t new_value;

    new_value = (int32_t) step * diff;
    new_value = new_value >> 10;
    new_value = a->end - new_value;


    return new_value;
}

/**
 * Calculate the current value of an animation applying step characteristic.
 * (Set end value on the end of the animation)
 * @param a pointer to an animation
 * @return the current value to set
 */
int32_t lv_anim_path_step(const lv_anim_t * a)
{
    if(a->act_time >= a->time) return a->end;
    else return a->start;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Periodically handle the animations.
 * @param param unused
 */
static void anim_task(void * param)
{
    (void)param;

    lv_anim_t * a;
    LL_READ(LV_GC_ROOT(_lv_anim_ll), a) {
        a->has_run = 0;
    }

    uint32_t elaps = lv_tick_elaps(last_task_run);
    a = lv_ll_get_head(&LV_GC_ROOT(_lv_anim_ll));

    while(a != NULL) {
        /*It can be set by `lv_anim_del()` typically in `end_cb`. If set then an animation delete happened in `anim_ready_handler`
         * which could make this linked list reading corrupt because the list is changed meanwhile
         */
        anim_list_changed = false;

        if(!a->has_run) {
            a->has_run = 1;         /*The list readying might be reseted so need to know which anim has run already*/
            a->act_time += elaps;
            if(a->act_time >= 0) {
                if(a->act_time > a->time) a->act_time = a->time;

                int32_t new_value;
                new_value = a->path(a);

                if(a->fp != NULL) a->fp(a->var, new_value); /*Apply the calculated value*/

                /*If the time is elapsed the animation is ready*/
                if(a->act_time >= a->time) {
                    anim_ready_handler(a);
                }
            }
        }

        /* If the linked list changed due to anim. delete then it's not safe to continue
         * the reading of the list from here -> start from the head*/
        if(anim_list_changed) a = lv_ll_get_head(&LV_GC_ROOT(_lv_anim_ll));
        else a = lv_ll_get_next(&LV_GC_ROOT(_lv_anim_ll), a);
    }

    last_task_run = lv_tick_get();
}

/**
 * Called when an animation is ready to do the necessary thinks
 * e.g. repeat, play back, delete etc.
 * @param a pointer to an animation descriptor
 * @return true: animation delete occurred nnd the `LV_GC_ROOT(_lv_anim_ll)` has changed
 * */
static bool anim_ready_handler(lv_anim_t * a)
{

    /*Delete the animation if
     * - no repeat and no play back (simple one shot animation)
     * - no repeat, play back is enabled and play back is ready */
    if((a->repeat == 0 && a->playback == 0) ||
            (a->repeat == 0 && a->playback == 1 && a->playback_now == 1)) {
        void (*cb)(void *) = a->end_cb;
        void * p = a->var;
        lv_ll_rem(&LV_GC_ROOT(_lv_anim_ll), a);
        lv_mem_free(a);
        anim_list_changed = true;

        /* Call the callback function at the end*/
        /* Check if an animation is deleted in the cb function
         * if yes then the caller function has to know this*/
        if(cb != NULL) cb(p);
    }
    /*If the animation is not deleted then restart it*/
    else {
        a->act_time = - a->repeat_pause;    /*Restart the animation*/
        /*Swap the start and end values in play back mode*/
        if(a->playback != 0) {
            /*If now turning back use the 'playback_pause*/
            if(a->playback_now == 0) a->act_time = - a->playback_pause;

            /*Toggle the play back state*/
            a->playback_now = a->playback_now == 0 ? 1 : 0;
            /*Swap the start and end values*/
            int32_t tmp;
            tmp = a->start;
            a->start = a->end;
            a->end = tmp;
        }
    }

    return anim_list_changed;
}
#endif
