/**
 * @file anim.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim.h"

#if LV_USE_ANIMATION
#include <stddef.h>
#include <string.h>
#include "../lv_misc/lv_debug.h"
#include "../lv_hal/lv_hal_tick.h"
#include "lv_task.h"
#include "lv_math.h"
#include "lv_gc.h"

#if defined(LV_GC_INCLUDE)
    #include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/
#define LV_ANIM_RESOLUTION 1024
#define LV_ANIM_RES_SHIFT 10
#define LV_ANIM_TASK_PRIO LV_TASK_PRIO_HIGH

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_task(lv_task_t * param);
static void anim_mark_list_change(void);
static bool anim_ready_handler(lv_anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t last_task_run;
static bool anim_list_changed;
static lv_task_t * _lv_anim_task;
const lv_anim_path_t lv_anim_path_def = {.cb = lv_anim_path_linear};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init. the animation module
 */
void _lv_anim_core_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_anim_ll), sizeof(lv_anim_t));
    last_task_run = lv_tick_get();
    _lv_anim_task = lv_task_create(anim_task, LV_DISP_DEF_REFR_PERIOD, LV_ANIM_TASK_PRIO, NULL);
    anim_mark_list_change(); /*Turn off the animation task*/
    anim_list_changed = false; /*The list has not actually changed*/
}

/**
 * Initialize an animation variable.
 * E.g.:
 * lv_anim_t a;
 * lv_anim_init(&a);
 * lv_anim_set_...(&a);
 * @param a pointer to an `lv_anim_t` variable to initialize
 */
void lv_anim_init(lv_anim_t * a)
{
    _lv_memset_00(a, sizeof(lv_anim_t));
    a->time    = 500;
    a->start   = 0;
    a->end     = 100;
    _lv_memcpy_small(&a->path, &lv_anim_path_def, sizeof(lv_anim_path_cb_t));
    a->repeat_cnt = 1;
    a->early_apply = 1;
}
/**
 * Create an animation
 * @param a an initialized 'anim_t' variable. Not required after call.
 */
void lv_anim_start(lv_anim_t * a)
{
    LV_LOG_TRACE("animation create started")
    /* Do not let two animations for the  same 'var' with the same 'fp'*/
    if(a->exec_cb != NULL) lv_anim_del(a->var, a->exec_cb); /*fp == NULL would delete all animations of var*/

    /*If the list is empty the anim task was suspended and it's last run measure is invalid*/
    if(_lv_ll_is_empty(&LV_GC_ROOT(_lv_anim_ll))) {
        last_task_run = lv_tick_get() - 1;
    }

    /*Add the new animation to the animation linked list*/
    lv_anim_t * new_anim = _lv_ll_ins_head(&LV_GC_ROOT(_lv_anim_ll));
    LV_ASSERT_MEM(new_anim);
    if(new_anim == NULL) return;

    /*Initialize the animation descriptor*/
    a->time_orig = a->time;
    _lv_memcpy(new_anim, a, sizeof(lv_anim_t));

    /*Set the start value*/
    if(new_anim->early_apply) {
        if(new_anim->exec_cb && new_anim->var) new_anim->exec_cb(new_anim->var, new_anim->start);
    }

    /* Creating an animation changed the linked list.
     * It's important if it happens in a ready callback. (see `anim_task`)*/
    anim_mark_list_change();

    LV_LOG_TRACE("animation created")
}

/**
 * Delete an animation of a variable with a given animator function
 * @param var pointer to variable
 * @param exec_cb a function pointer which is animating 'var',
 *           or NULL to delete all the animations of 'var'
 * @return true: at least 1 animation is deleted, false: no animation is deleted
 */
bool lv_anim_del(void * var, lv_anim_exec_xcb_t exec_cb)
{
    lv_anim_t * a;
    lv_anim_t * a_next;
    bool del = false;
    a        = _lv_ll_get_head(&LV_GC_ROOT(_lv_anim_ll));
    while(a != NULL) {
        /*'a' might be deleted, so get the next object while 'a' is valid*/
        a_next = _lv_ll_get_next(&LV_GC_ROOT(_lv_anim_ll), a);

        if(a->var == var && (a->exec_cb == exec_cb || exec_cb == NULL)) {
            _lv_ll_remove(&LV_GC_ROOT(_lv_anim_ll), a);
            lv_mem_free(a);
            anim_mark_list_change(); /*Read by `anim_task`. It need to know if a delete occurred in
                                         the linked list*/
            del = true;
        }

        a = a_next;
    }

    return del;
}

/**
 * Get the animation of a variable and its `exec_cb`.
 * @param var pointer to variable
 * @param exec_cb a function pointer which is animating 'var',
 *           or NULL to delete all the animations of 'var'
 * @return pointer to the animation.
 */
lv_anim_t * lv_anim_get(void * var, lv_anim_exec_xcb_t exec_cb)
{
    lv_anim_t * a;
    _LV_LL_READ(LV_GC_ROOT(_lv_anim_ll), a) {
        if(a->var == var && a->exec_cb == exec_cb) {
            return a;
        }
    }

    return NULL;
}

/**
 * Get the number of currently running animations
 * @return the number of running animations
 */
uint16_t lv_anim_count_running(void)
{
    uint16_t cnt = 0;
    lv_anim_t * a;
    _LV_LL_READ(LV_GC_ROOT(_lv_anim_ll), a) cnt++;

    return cnt;
}

/**
 * Calculate the time of an animation with a given speed and the start and end values
 * @param speed speed of animation in unit/sec
 * @param start start value of the animation
 * @param end end value of the animation
 * @return the required time [ms] for the animation with the given parameters
 */
uint16_t lv_anim_speed_to_time(uint16_t speed, lv_anim_value_t start, lv_anim_value_t end)
{
    int32_t d     = LV_MATH_ABS((int32_t)start - end);
    uint32_t time = (int32_t)((int32_t)(d * 1000) / speed);

    if(time > UINT16_MAX) time = UINT16_MAX;

    if(time == 0) {
        time++;
    }

    return time;
}

/**
 * Manually refresh the state of the animations.
 * Useful to make the animations running in a blocking process where
 * `lv_task_handler` can't run for a while.
 * Shouldn't be used directly because it is called in `lv_refr_now()`.
 */
void lv_anim_refr_now(void)
{
    anim_task(NULL);
}

/**
 * Calculate the current value of an animation applying linear characteristic
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_linear(const lv_anim_path_t * path, const lv_anim_t * a)
{
    LV_UNUSED(path);

    /*Calculate the current step*/
    uint32_t step;
    if(a->time == a->act_time) {
        step = LV_ANIM_RESOLUTION; /*Use the last value if the time fully elapsed*/
    }
    else {
        step = ((int32_t)a->act_time * LV_ANIM_RESOLUTION) / a->time;
    }

    /* Get the new value which will be proportional to `step`
     * and the `start` and `end` values*/
    int32_t new_value;
    new_value = (int32_t)step * (a->end - a->start);
    new_value = new_value >> LV_ANIM_RES_SHIFT;
    new_value += a->start;

    return (lv_anim_value_t)new_value;
}

/**
 * Calculate the current value of an animation slowing down the start phase
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_ease_in(const lv_anim_path_t * path, const lv_anim_t * a)
{
    LV_UNUSED(path);

    /*Calculate the current step*/
    uint32_t t;
    if(a->time == a->act_time)
        t = 1024;
    else
        t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = _lv_bezier3(t, 0, 1, 1, 1024);

    int32_t new_value;
    new_value = (int32_t)step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;

    return (lv_anim_value_t)new_value;
}

/**
 * Calculate the current value of an animation slowing down the end phase
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_ease_out(const lv_anim_path_t * path, const lv_anim_t * a)
{
    LV_UNUSED(path);

    /*Calculate the current step*/

    uint32_t t;
    if(a->time == a->act_time)
        t = 1024;
    else
        t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = _lv_bezier3(t, 0, 1023, 1023, 1024);

    int32_t new_value;
    new_value = (int32_t)step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;

    return (lv_anim_value_t)new_value;
}

/**
 * Calculate the current value of an animation applying an "S" characteristic (cosine)
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_ease_in_out(const lv_anim_path_t * path, const lv_anim_t * a)
{
    LV_UNUSED(path);

    /*Calculate the current step*/

    uint32_t t;
    if(a->time == a->act_time)
        t = 1024;
    else
        t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = _lv_bezier3(t, 0, 100, 924, 1024);

    int32_t new_value;
    new_value = (int32_t)step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;

    return (lv_anim_value_t)new_value;
}

/**
 * Calculate the current value of an animation with overshoot at the end
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_overshoot(const lv_anim_path_t * path, const lv_anim_t * a)
{
    LV_UNUSED(path);

    /*Calculate the current step*/

    uint32_t t;
    if(a->time == a->act_time)
        t = 1024;
    else
        t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = _lv_bezier3(t, 0, 1000, 2000, 1024);

    int32_t new_value;
    new_value = (int32_t)step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;

    return (lv_anim_value_t)new_value;
}


/**
 * Calculate the current value of an animation with 3 bounces
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_bounce(const lv_anim_path_t * path, const lv_anim_t * a)
{
    LV_UNUSED(path);

    /*Calculate the current step*/
    uint32_t t;
    if(a->time == a->act_time)
        t = 1024;
    else
        t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t diff = (a->end - a->start);

    /*3 bounces has 5 parts: 3 down and 2 up. One part is t / 5 long*/

    if(t < 408) {
        /*Go down*/
        t = (t * 2500) >> 10; /*[0..1024] range*/
    }
    else if(t >= 408 && t < 614) {
        /*First bounce back*/
        t -= 408;
        t    = t * 5; /*to [0..1024] range*/
        t    = 1024 - t;
        diff = diff / 6;
    }
    else if(t >= 614 && t < 819) {
        /*Fall back*/
        t -= 614;
        t    = t * 5; /*to [0..1024] range*/
        diff = diff / 6;
    }
    else if(t >= 819 && t < 921) {
        /*Second bounce back*/
        t -= 819;
        t    = t * 10; /*to [0..1024] range*/
        t    = 1024 - t;
        diff = diff / 16;
    }
    else if(t >= 921 && t <= 1024) {
        /*Fall back*/
        t -= 921;
        t    = t * 10; /*to [0..1024] range*/
        diff = diff / 16;
    }

    if(t > 1024) t = 1024;

    int32_t step = _lv_bezier3(t, 1024, 1024, 800, 0);

    int32_t new_value;
    new_value = (int32_t)step * diff;
    new_value = new_value >> 10;
    new_value = a->end - new_value;

    return (lv_anim_value_t)new_value;
}

/**
 * Calculate the current value of an animation applying step characteristic.
 * (Set end value on the end of the animation)
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_step(const lv_anim_path_t * path, const lv_anim_t * a)
{
    LV_UNUSED(path);

    if(a->act_time >= a->time)
        return a->end;
    else
        return a->start;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Periodically handle the animations.
 * @param param unused
 */
static void anim_task(lv_task_t * param)
{
    (void)param;

    lv_anim_t * a;
    _LV_LL_READ(LV_GC_ROOT(_lv_anim_ll), a) {
        a->has_run = 0;
    }

    uint32_t elaps = lv_tick_elaps(last_task_run);

    a = _lv_ll_get_head(&LV_GC_ROOT(_lv_anim_ll));

    while(a != NULL) {
        /*It can be set by `lv_anim_del()` typically in `end_cb`. If set then an animation delete
         * happened in `anim_ready_handler` which could make this linked list reading corrupt
         * because the list is changed meanwhile
         */
        anim_list_changed = false;

        if(!a->has_run) {
            a->has_run = 1; /*The list readying might be reseted so need to know which anim has run already*/

            /*The animation will run now for the first time. Call `start_cb`*/
            int32_t new_act_time = a->act_time + elaps;
            if(a->act_time <= 0 && new_act_time >= 0) {
                if(a->start_cb) a->start_cb(a);
            }
            a->act_time += elaps;
            if(a->act_time >= 0) {
                if(a->act_time > a->time) a->act_time = a->time;

                int32_t new_value;
                if(a->path.cb) new_value = a->path.cb(&a->path, a);
                else new_value = lv_anim_path_linear(&a->path, a);

                if(new_value != a->current) {
                    a->current = new_value;
                    /*Apply the calculated value*/
                    if(a->exec_cb) a->exec_cb(a->var, new_value);
                }

                /*If the time is elapsed the animation is ready*/
                if(a->act_time >= a->time) {
                    anim_ready_handler(a);
                }
            }
        }

        /* If the linked list changed due to anim. delete then it's not safe to continue
         * the reading of the list from here -> start from the head*/
        if(anim_list_changed)
            a = _lv_ll_get_head(&LV_GC_ROOT(_lv_anim_ll));
        else
            a = _lv_ll_get_next(&LV_GC_ROOT(_lv_anim_ll), a);
    }

    last_task_run = lv_tick_get();
}

/**
 * Called when an animation is ready to do the necessary thinks
 * e.g. repeat, play back, delete etc.
 * @param a pointer to an animation descriptor
 * @return true: animation delete occurred and the `LV_GC_ROOT(_lv_anim_ll)` has changed
 * */
static bool anim_ready_handler(lv_anim_t * a)
{
    /*In the end of a forward anim decrement repeat cnt.*/
    if(a->playback_now == 0 && a->repeat_cnt > 0 && a->repeat_cnt != LV_ANIM_REPEAT_INFINITE) {
        a->repeat_cnt--;
    }

    /*Delete the animation if
     * - no repeat left and no play back (simple one shot animation)
     * - no repeat, play back is enabled and play back is ready */
    if(a->repeat_cnt == 0 && ((a->playback_time == 0) || (a->playback_time && a->playback_now == 1))) {

        /*Create copy from the animation and delete the animation from the list.
         * This way the `ready_cb` will see the animations like it's animation is ready deleted*/
        lv_anim_t a_tmp;
        _lv_memcpy(&a_tmp, a, sizeof(lv_anim_t));
        _lv_ll_remove(&LV_GC_ROOT(_lv_anim_ll), a);
        lv_mem_free(a);
        /*Flag that the list has changed */
        anim_mark_list_change();

        /* Call the callback function at the end*/
        if(a_tmp.ready_cb != NULL) a_tmp.ready_cb(&a_tmp);
    }
    /*If the animation is not deleted then restart it*/
    else {
        a->act_time = -(int32_t)(a->repeat_delay); /*Restart the animation*/
        /*Swap the start and end values in play back mode*/
        if(a->playback_time != 0) {
            /*If now turning back use the 'playback_pause*/
            if(a->playback_now == 0) a->act_time = -(int32_t)(a->playback_delay);

            /*Toggle the play back state*/
            a->playback_now = a->playback_now == 0 ? 1 : 0;
            /*Swap the start and end values*/
            int32_t tmp;
            tmp      = a->start;
            a->start = a->end;
            a->end   = tmp;

            a->time = a->playback_now == 0 ? a->time_orig : a->playback_time;
        }
    }

    return anim_list_changed;
}
static void anim_mark_list_change(void)
{
    anim_list_changed = true;
    if(_lv_ll_get_head(&LV_GC_ROOT(_lv_anim_ll)) == NULL)
        lv_task_set_prio(_lv_anim_task, LV_TASK_PRIO_OFF);
    else
        lv_task_set_prio(_lv_anim_task, LV_ANIM_TASK_PRIO);
}
#endif
