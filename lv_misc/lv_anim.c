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

/*********************
 *      DEFINES
 *********************/
#define LV_ANIM_RESOLUTION             1024
#define LV_ANIM_RES_SHIFT       10

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_task (void * param);
static bool anim_ready_handler(lv_anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_ll_t anim_ll;
static uint32_t last_task_run;

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
    lv_ll_init(&anim_ll, sizeof(lv_anim_t));
    last_task_run = lv_tick_get();
    lv_task_create(anim_task, LV_REFR_PERIOD, LV_TASK_PRIO_MID, NULL);
}

/**
 * Create an animation
 * @param anim_p an initialized 'anim_t' variable. Not required after call.
 */
void lv_anim_create(lv_anim_t * anim_p)
{
    /* Do not let two animations for the  same 'var' with the same 'fp'*/
    if(anim_p->fp != NULL) lv_anim_del(anim_p->var, anim_p->fp);       /*fp == NULL would delete all animations of var*/

    /*Add the new animation to the animation linked list*/
    lv_anim_t * new_anim = lv_ll_ins_head(&anim_ll);
    lv_mem_assert(new_anim);

    /*Initialize the animation descriptor*/
    anim_p->playback_now = 0;
    memcpy(new_anim, anim_p, sizeof(lv_anim_t));

    /*Set the start value*/
    if(new_anim->fp != NULL) new_anim->fp(new_anim->var, new_anim->start);
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
    bool del = false;
    lv_anim_t * a;
    lv_anim_t * a_next;
    a = lv_ll_get_head(&anim_ll);
    while(a != NULL) {
        /*'a' might be deleted, so get the next object while 'a' is valid*/
        a_next = lv_ll_get_next(&anim_ll, a);

        if(a->var == var && (a->fp == fp || fp == NULL)) {
            lv_ll_rem(&anim_ll, a);
            lv_mem_free(a);
            del = true;
        }

        a = a_next;
    }

    return del;
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
    uint16_t time = (int32_t)((int32_t)(d * 1000) / speed);

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
int32_t lv_anim_path_linear(const lv_anim_t *a)
{
    /*Calculate the current step*/

    uint16_t step;
    if(a->time == a->act_time) step = LV_ANIM_RESOLUTION; /*Use the last value id the time fully elapsed*/
    else step = (a->act_time * LV_ANIM_RESOLUTION) / a->time;


    /* Get the new value which will be proportional to the current element of 'path_p'
     * and the 'start' and 'end' values*/
    int32_t new_value;
    new_value =  (int32_t) step * (a->end - a->start);
    new_value = new_value >> LV_ANIM_RES_SHIFT;
    new_value += a->start;

    return new_value;
}

/**
 * Calculate the current value of an animation applying step characteristic.
 * (Set end value on the end of the animation)
 * @param a pointer to an animation
 * @return the current value to set
 */
int32_t lv_anim_path_step(const lv_anim_t *a)
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
static void anim_task (void * param)
{
    (void)param;

    volatile uint32_t elaps;
    elaps = lv_tick_elaps(last_task_run);

    lv_anim_t * a;
    lv_anim_t * a_next;
    a = lv_ll_get_head(&anim_ll);
    while(a != NULL) {
        /*'a' might be deleted, so get the next object while 'a' is valid*/
        a_next = lv_ll_get_next(&anim_ll, a);

        a->act_time += elaps;
        if(a->act_time >= 0) {
            if(a->act_time > a->time) a->act_time = a->time;

            int32_t new_value;
            new_value = a->path(a);

            if(a->fp != NULL) a->fp(a->var, new_value); /*Apply the calculated value*/

            /*If the time is elapsed the animation is ready*/
            if(a->act_time >= a->time) {
                bool invalid;
                invalid = anim_ready_handler(a);
                if(invalid != false) {
                    a_next = lv_ll_get_head(&anim_ll);  /*a_next might be invalid if animation delete occurred*/
                }
            }
        }

        a = a_next;
    }

    last_task_run = lv_tick_get();
}

/**
 * Called when an animation is ready to do the necessary thinks
 * e.g. repeat, play back, delete etc.
 * @param a pointer to an animation descriptor
 * @return true: animation delete occurred
 * */
static bool anim_ready_handler(lv_anim_t * a)
{
    bool invalid = false;

    /*Delete the animation if
     * - no repeat and no play back (simple one shot animation)
     * - no repeat, play back is enabled and play back is ready */
    if((a->repeat == 0 && a->playback == 0) ||
            (a->repeat == 0 && a->playback == 1 && a->playback_now == 1)) {
        void (*cb) (void *) = a->end_cb;
        void * p = a->var;
        lv_ll_rem(&anim_ll, a);
        lv_mem_free(a);

        /*Call the callback function at the end*/
        /* Check if an animation is deleted in the cb function
         * if yes then the caller function has to know this*/
        if(cb != NULL) cb(p);
        invalid = true;
    }
    /*If the animation is not deleted then restart it*/
    else {
        a->act_time = - a->repeat_pause;    /*Restart the animation*/
        /*Swap the start and end values in play back mode*/
        if(a->playback != 0) {
            /*If now turning back use the 'playback_pause*/
            if(a->playback_now == 0) a->act_time = - a->playback_pause;

            /*Toggle the play back state*/
            a->playback_now = a->playback_now == 0 ? 1: 0;
            /*Swap the start and end values*/
            int32_t tmp;
            tmp = a->start;
            a->start = a->end;
            a->end = tmp;
        }
    }

    return invalid;
}
#endif
