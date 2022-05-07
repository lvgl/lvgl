/**
 * @file lv_anim.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim.h"

#include "../hal/lv_hal_tick.h"
#include "lv_assert.h"
#include "lv_timer.h"
#include "lv_math.h"
#include "lv_mem.h"
#include "lv_gc.h"

/*********************
 *      DEFINES
 *********************/
#define LV_ANIM_RESOLUTION 1024
#define LV_ANIM_RES_SHIFT 10

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_timer(lv_timer_t * param);
static void anim_mark_list_change(void);
static void anim_ready_handler(lv_anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t last_timer_run;
static bool anim_list_changed;
static bool anim_run_round;
static lv_timer_t * _lv_anim_tmr;

/**********************
 *      MACROS
 **********************/
#if LV_LOG_TRACE_ANIM
    #define TRACE_ANIM(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define TRACE_ANIM(...)
#endif


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_anim_core_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_anim_ll), sizeof(lv_anim_t));
    _lv_anim_tmr = lv_timer_create(anim_timer, LV_DISP_DEF_REFR_PERIOD, NULL);
    anim_mark_list_change(); /*Turn off the animation timer*/
    anim_list_changed = false;
}

void lv_anim_init(lv_anim_t * a)
{
    lv_memset_00(a, sizeof(lv_anim_t));
    a->time = 500;
    a->start_value = 0;
    a->end_value = 100;
    a->repeat_cnt = 1;
    a->path_cb = lv_anim_path_linear;
    a->early_apply = 1;
}

lv_anim_t * lv_anim_start(const lv_anim_t * a)
{
    TRACE_ANIM("begin");

    /*Do not let two animations for the same 'var' with the same 'exec_cb'*/
    if(a->exec_cb != NULL) lv_anim_del(a->var, a->exec_cb); /*exec_cb == NULL would delete all animations of var*/

    /*If the list is empty the anim timer was suspended and it's last run measure is invalid*/
    if(_lv_ll_is_empty(&LV_GC_ROOT(_lv_anim_ll))) {
        last_timer_run = lv_tick_get();
    }

    /*Add the new animation to the animation linked list*/
    lv_anim_t * new_anim = _lv_ll_ins_head(&LV_GC_ROOT(_lv_anim_ll));
    LV_ASSERT_MALLOC(new_anim);
    if(new_anim == NULL) return NULL;

    /*Initialize the animation descriptor*/
    lv_memcpy(new_anim, a, sizeof(lv_anim_t));
    if(a->var == a) new_anim->var = new_anim;
    new_anim->run_round = anim_run_round;

    /*Set the start value*/
    if(new_anim->early_apply) {
        if(new_anim->get_value_cb) {
            int32_t v_ofs = new_anim->get_value_cb(new_anim);
            new_anim->start_value += v_ofs;
            new_anim->end_value += v_ofs;
        }

        if(new_anim->exec_cb && new_anim->var) new_anim->exec_cb(new_anim->var, new_anim->start_value);
    }

    /*Creating an animation changed the linked list.
     *It's important if it happens in a ready callback. (see `anim_timer`)*/
    anim_mark_list_change();

    TRACE_ANIM("finished");
    return new_anim;
}

uint32_t lv_anim_get_playtime(lv_anim_t * a)
{
    uint32_t playtime = LV_ANIM_PLAYTIME_INFINITE;

    if(a->repeat_cnt == LV_ANIM_REPEAT_INFINITE)
        return playtime;

    playtime = a->time - a->act_time;
    if(a->playback_now == 0)
        playtime += a->playback_delay + a->playback_time;

    if(a->repeat_cnt <= 1)
        return playtime;

    playtime += (a->repeat_delay + a->time +
                 a->playback_delay + a->playback_time) *
                (a->repeat_cnt - 1);

    return playtime;
}

bool lv_anim_del(void * var, lv_anim_exec_xcb_t exec_cb)
{
    lv_anim_t * a;
    lv_anim_t * a_next;
    bool del = false;
    a        = _lv_ll_get_head(&LV_GC_ROOT(_lv_anim_ll));
    while(a != NULL) {
        /*'a' might be deleted, so get the next object while 'a' is valid*/
        a_next = _lv_ll_get_next(&LV_GC_ROOT(_lv_anim_ll), a);

        if((a->var == var || var == NULL) && (a->exec_cb == exec_cb || exec_cb == NULL)) {
            _lv_ll_remove(&LV_GC_ROOT(_lv_anim_ll), a);
            lv_mem_free(a);
            anim_mark_list_change(); /*Read by `anim_timer`. It need to know if a delete occurred in
                                       the linked list*/
            del = true;
        }

        a = a_next;
    }

    return del;
}

void lv_anim_del_all(void)
{
    _lv_ll_clear(&LV_GC_ROOT(_lv_anim_ll));
    anim_mark_list_change();
}

lv_anim_t * lv_anim_get(void * var, lv_anim_exec_xcb_t exec_cb)
{
    lv_anim_t * a;
    _LV_LL_READ(&LV_GC_ROOT(_lv_anim_ll), a) {
        if(a->var == var && (a->exec_cb == exec_cb || exec_cb == NULL)) {
            return a;
        }
    }

    return NULL;
}

struct _lv_timer_t * lv_anim_get_timer(void)
{
    return _lv_anim_tmr;
}

uint16_t lv_anim_count_running(void)
{
    uint16_t cnt = 0;
    lv_anim_t * a;
    _LV_LL_READ(&LV_GC_ROOT(_lv_anim_ll), a) cnt++;

    return cnt;
}

uint32_t lv_anim_speed_to_time(uint32_t speed, int32_t start, int32_t end)
{
    uint32_t d    = LV_ABS(start - end);
    uint32_t time = (d * 1000) / speed;

    if(time == 0) {
        time++;
    }

    return time;
}

void lv_anim_refr_now(void)
{
    anim_timer(NULL);
}

int32_t lv_anim_path_linear(const lv_anim_t * a)
{
    /*Calculate the current step*/
    int32_t step = lv_map(a->act_time, 0, a->time, 0, LV_ANIM_RESOLUTION);

    /*Get the new value which will be proportional to `step`
     *and the `start` and `end` values*/
    int32_t new_value;
    new_value = step * (a->end_value - a->start_value);
    new_value = new_value >> LV_ANIM_RES_SHIFT;
    new_value += a->start_value;

    return new_value;
}

int32_t lv_anim_path_ease_in(const lv_anim_t * a)
{
    /*Calculate the current step*/
    uint32_t t = lv_map(a->act_time, 0, a->time, 0, LV_BEZIER_VAL_MAX);
    int32_t step = lv_bezier3(t, 0, 50, 100, LV_BEZIER_VAL_MAX);

    int32_t new_value;
    new_value = step * (a->end_value - a->start_value);
    new_value = new_value >> LV_BEZIER_VAL_SHIFT;
    new_value += a->start_value;

    return new_value;
}

int32_t lv_anim_path_ease_out(const lv_anim_t * a)
{
    /*Calculate the current step*/
    uint32_t t = lv_map(a->act_time, 0, a->time, 0, LV_BEZIER_VAL_MAX);
    int32_t step = lv_bezier3(t, 0, 900, 950, LV_BEZIER_VAL_MAX);

    int32_t new_value;
    new_value = step * (a->end_value - a->start_value);
    new_value = new_value >> LV_BEZIER_VAL_SHIFT;
    new_value += a->start_value;

    return new_value;
}

int32_t lv_anim_path_ease_in_out(const lv_anim_t * a)
{
    /*Calculate the current step*/
    uint32_t t = lv_map(a->act_time, 0, a->time, 0, LV_BEZIER_VAL_MAX);
    int32_t step = lv_bezier3(t, 0, 50, 952, LV_BEZIER_VAL_MAX);

    int32_t new_value;
    new_value = step * (a->end_value - a->start_value);
    new_value = new_value >> LV_BEZIER_VAL_SHIFT;
    new_value += a->start_value;

    return new_value;
}

int32_t lv_anim_path_overshoot(const lv_anim_t * a)
{
    /*Calculate the current step*/
    uint32_t t = lv_map(a->act_time, 0, a->time, 0, LV_BEZIER_VAL_MAX);
    int32_t step = lv_bezier3(t, 0, 1000, 1300, LV_BEZIER_VAL_MAX);

    int32_t new_value;
    new_value = step * (a->end_value - a->start_value);
    new_value = new_value >> LV_BEZIER_VAL_SHIFT;
    new_value += a->start_value;

    return new_value;
}

int32_t lv_anim_path_bounce(const lv_anim_t * a)
{
    /*Calculate the current step*/
    int32_t t = lv_map(a->act_time, 0, a->time, 0, LV_BEZIER_VAL_MAX);
    int32_t diff = (a->end_value - a->start_value);

    /*3 bounces has 5 parts: 3 down and 2 up. One part is t / 5 long*/

    if(t < 408) {
        /*Go down*/
        t = (t * 2500) >> LV_BEZIER_VAL_SHIFT; /*[0..1024] range*/
    }
    else if(t >= 408 && t < 614) {
        /*First bounce back*/
        t -= 408;
        t    = t * 5; /*to [0..1024] range*/
        t    = LV_BEZIER_VAL_MAX - t;
        diff = diff / 20;
    }
    else if(t >= 614 && t < 819) {
        /*Fall back*/
        t -= 614;
        t    = t * 5; /*to [0..1024] range*/
        diff = diff / 20;
    }
    else if(t >= 819 && t < 921) {
        /*Second bounce back*/
        t -= 819;
        t    = t * 10; /*to [0..1024] range*/
        t    = LV_BEZIER_VAL_MAX - t;
        diff = diff / 40;
    }
    else if(t >= 921 && t <= LV_BEZIER_VAL_MAX) {
        /*Fall back*/
        t -= 921;
        t    = t * 10; /*to [0..1024] range*/
        diff = diff / 40;
    }

    if(t > LV_BEZIER_VAL_MAX) t = LV_BEZIER_VAL_MAX;
    if(t < 0) t = 0;
    int32_t step = lv_bezier3(t, LV_BEZIER_VAL_MAX, 800, 500, 0);

    int32_t new_value;
    new_value = step * diff;
    new_value = new_value >> LV_BEZIER_VAL_SHIFT;
    new_value = a->end_value - new_value;

    return new_value;
}

int32_t lv_anim_path_step(const lv_anim_t * a)
{
    if(a->act_time >= a->time)
        return a->end_value;
    else
        return a->start_value;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Periodically handle the animations.
 * @param param unused
 */
static void anim_timer(lv_timer_t * param)
{
    LV_UNUSED(param);

    uint32_t elaps = lv_tick_elaps(last_timer_run);

    /*Flip the run round*/
    anim_run_round = anim_run_round ? false : true;

    lv_anim_t * a = _lv_ll_get_head(&LV_GC_ROOT(_lv_anim_ll));

    while(a != NULL) {
        /*It can be set by `lv_anim_del()` typically in `end_cb`. If set then an animation delete
         * happened in `anim_ready_handler` which could make this linked list reading corrupt
         * because the list is changed meanwhile
         */
        anim_list_changed = false;

        if(a->run_round != anim_run_round) {
            a->run_round = anim_run_round; /*The list readying might be reset so need to know which anim has run already*/

            /*The animation will run now for the first time. Call `start_cb`*/
            int32_t new_act_time = a->act_time + elaps;
            if(!a->start_cb_called && a->act_time <= 0 && new_act_time >= 0) {
                if(a->early_apply == 0 && a->get_value_cb) {
                    int32_t v_ofs = a->get_value_cb(a);
                    a->start_value += v_ofs;
                    a->end_value += v_ofs;
                }
                if(a->start_cb) a->start_cb(a);
                a->start_cb_called = 1;
            }
            a->act_time += elaps;
            if(a->act_time >= 0) {
                if(a->act_time > a->time) a->act_time = a->time;

                int32_t new_value;
                new_value = a->path_cb(a);

                if(new_value != a->current_value) {
                    a->current_value = new_value;
                    /*Apply the calculated value*/
                    if(a->exec_cb) a->exec_cb(a->var, new_value);
                }

                /*If the time is elapsed the animation is ready*/
                if(a->act_time >= a->time) {
                    anim_ready_handler(a);
                }
            }
        }

        /*If the linked list changed due to anim. delete then it's not safe to continue
         *the reading of the list from here -> start from the head*/
        if(anim_list_changed)
            a = _lv_ll_get_head(&LV_GC_ROOT(_lv_anim_ll));
        else
            a = _lv_ll_get_next(&LV_GC_ROOT(_lv_anim_ll), a);
    }

    last_timer_run = lv_tick_get();
}

/**
 * Called when an animation is ready to do the necessary thinks
 * e.g. repeat, play back, delete etc.
 * @param a pointer to an animation descriptor
 */
static void anim_ready_handler(lv_anim_t * a)
{
    /*In the end of a forward anim decrement repeat cnt.*/
    if(a->playback_now == 0 && a->repeat_cnt > 0 && a->repeat_cnt != LV_ANIM_REPEAT_INFINITE) {
        a->repeat_cnt--;
    }

    /*Delete the animation if
     * - no repeat left and no play back (simple one shot animation)
     * - no repeat, play back is enabled and play back is ready*/
    if(a->repeat_cnt == 0 && (a->playback_time == 0 || a->playback_now == 1)) {

        /*Delete the animation from the list.
         * This way the `ready_cb` will see the animations like it's animation is ready deleted*/
        _lv_ll_remove(&LV_GC_ROOT(_lv_anim_ll), a);
        /*Flag that the list has changed*/
        anim_mark_list_change();

        /*Call the callback function at the end*/
        if(a->ready_cb != NULL) a->ready_cb(a);
        lv_mem_free(a);
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
            int32_t tmp    = a->start_value;
            a->start_value = a->end_value;
            a->end_value   = tmp;
            /*Swap the time and playback_time*/
            tmp = a->time;
            a->time = a->playback_time;
            a->playback_time = tmp;
        }
    }
}

static void anim_mark_list_change(void)
{
    anim_list_changed = true;
    if(_lv_ll_get_head(&LV_GC_ROOT(_lv_anim_ll)) == NULL)
        lv_timer_pause(_lv_anim_tmr);
    else
        lv_timer_resume(_lv_anim_tmr);
}
