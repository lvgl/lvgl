/**
 * @file lv_anim.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim_private.h"

#include "../core/lv_global.h"
#include "../tick/lv_tick.h"
#include "lv_assert.h"
#include "lv_timer.h"
#include "lv_math.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**Perform linear animations in max 1024 steps. Used in `path_cb`s*/
#define LV_ANIM_RESOLUTION 1024

/**log2(LV_ANIM_RESOLUTION)*/
#define LV_ANIM_RES_SHIFT 10

/**In an anim. time this bit indicates that the value is speed, and not time*/
#define LV_ANIM_SPEED_MASK 0x80000000

#define state LV_GLOBAL_DEFAULT()->anim_state
#define anim_ll_p &(state.anim_ll)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_timer(lv_timer_t * param);
static void anim_mark_list_change(void);
static void anim_completed_handler(lv_anim_t * a);
static int32_t lv_anim_path_cubic_bezier(const lv_anim_t * a, int32_t x1,
                                         int32_t y1, int32_t x2, int32_t y2);
static void lv_anim_pause_for_internal(lv_anim_t * a, uint32_t ms);
static void resolve_time(lv_anim_t * a);
static bool remove_concurrent_anims(const lv_anim_t * a_current);
static void remove_anim(void * a);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_USE_LOG && LV_LOG_TRACE_ANIM
    #define LV_TRACE_ANIM(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define LV_TRACE_ANIM(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_anim_core_init(void)
{
    lv_ll_init(anim_ll_p, sizeof(lv_anim_t));
    state.timer = lv_timer_create(anim_timer, LV_DEF_REFR_PERIOD, NULL);
    anim_mark_list_change(); /*Turn off the animation timer*/
    state.anim_list_changed = false;
    state.anim_run_round = false;
}

void lv_anim_core_deinit(void)
{
    lv_anim_delete_all();
}

void lv_anim_init(lv_anim_t * a)
{
    lv_memzero(a, sizeof(lv_anim_t));
    a->duration = 500;
    a->start_value = 0;
    a->end_value = 100;
    a->repeat_cnt = 1;
    a->path_cb = lv_anim_path_linear;
    a->early_apply = 1;
}

lv_anim_t * lv_anim_start(const lv_anim_t * a)
{
    LV_TRACE_ANIM("begin");

    /*Do not let two animations for the same 'var' with the same 'exec_cb'*/
    if(a->early_apply && (a->exec_cb || a->custom_exec_cb)) {
        remove_concurrent_anims(a);
    }

    /*Add the new animation to the animation linked list*/
    lv_anim_t * new_anim = lv_ll_ins_head(anim_ll_p);
    LV_ASSERT_MALLOC(new_anim);
    if(new_anim == NULL) return NULL;

    /*Initialize the animation descriptor*/
    lv_memcpy(new_anim, a, sizeof(lv_anim_t));
    if(a->var == a) new_anim->var = new_anim;
    new_anim->run_round = state.anim_run_round;
    new_anim->last_timer_run = lv_tick_get();
    new_anim->is_paused = false;

    /*Set the start value*/
    if(new_anim->early_apply) {
        if(new_anim->get_value_cb) {
            int32_t v_ofs = new_anim->get_value_cb(new_anim);
            new_anim->start_value += v_ofs;
            new_anim->end_value += v_ofs;

        }

        resolve_time(new_anim);

        new_anim->current_value = new_anim->path_cb(new_anim);
        if(new_anim->exec_cb) {
            new_anim->exec_cb(new_anim->var, new_anim->current_value);
        }
        if(new_anim->custom_exec_cb) {
            new_anim->custom_exec_cb(new_anim, new_anim->current_value);
        }
    }

    /*Creating an animation changed the linked list.
     *It's important if it happens in a ready callback. (see `anim_timer`)*/
    anim_mark_list_change();

    LV_TRACE_ANIM("finished");
    return new_anim;
}

uint32_t lv_anim_get_playtime(const lv_anim_t * a)
{
    if(a->repeat_cnt == LV_ANIM_REPEAT_INFINITE) {
        return LV_ANIM_PLAYTIME_INFINITE;
    }

    uint32_t repeat_cnt = a->repeat_cnt;
    if(repeat_cnt < 1) repeat_cnt = 1;

    uint32_t playtime = a->repeat_delay + a->duration + a->reverse_delay + a->reverse_duration;
    playtime = playtime * repeat_cnt;
    return playtime;
}

bool lv_anim_delete(void * var, lv_anim_exec_xcb_t exec_cb)
{
    lv_anim_t * a;
    bool del_any = false;
    a        = lv_ll_get_head(anim_ll_p);
    while(a != NULL) {
        bool del = false;
        if((a->var == var || var == NULL) && (a->exec_cb == exec_cb || exec_cb == NULL)) {
            remove_anim(a);
            anim_mark_list_change(); /*Read by `anim_timer`. It need to know if a delete occurred in
                                       the linked list*/
            del_any = true;
            del = true;
        }

        /*Always start from the head on delete, because we don't know
         *how `anim_ll_p` was changes in `a->deleted_cb` */
        a = del ? lv_ll_get_head(anim_ll_p) : lv_ll_get_next(anim_ll_p, a);
    }

    return del_any;
}

void lv_anim_delete_all(void)
{
    lv_ll_clear_custom(anim_ll_p, remove_anim);
    anim_mark_list_change();
}

lv_anim_t * lv_anim_get(void * var, lv_anim_exec_xcb_t exec_cb)
{
    lv_anim_t * a;
    LV_LL_READ(anim_ll_p, a) {
        if(a->var == var && (a->exec_cb == exec_cb || exec_cb == NULL)) {
            return a;
        }
    }

    return NULL;
}

lv_timer_t * lv_anim_get_timer(void)
{
    return state.timer;
}

uint16_t lv_anim_count_running(void)
{
    uint16_t cnt = 0;
    lv_anim_t * a;
    LV_LL_READ(anim_ll_p, a) cnt++;

    return cnt;
}

uint32_t lv_anim_speed_clamped(uint32_t speed, uint32_t min_time, uint32_t max_time)
{

    if(speed > 10000) {
        LV_LOG_WARN("speed is truncated to 10000 (was %"LV_PRIu32")", speed);
        speed = 10230;
    }
    if(min_time > 10000) {
        LV_LOG_WARN("min_time is truncated to 10000 (was %"LV_PRIu32")", min_time);
        min_time = 10230;
    }
    if(max_time > 10000) {
        LV_LOG_WARN("max_time is truncated to 10000 (was %"LV_PRIu32")", max_time);
        max_time = 10230;
    }

    /*Lower the resolution to fit the 0.1023 range*/
    speed = (speed + 5) / 10;
    min_time = (min_time + 5) / 10;
    max_time = (max_time + 5) / 10;

    return LV_ANIM_SPEED_MASK + (max_time << 20) + (min_time << 10) + speed;

}

uint32_t lv_anim_speed(uint32_t speed)
{
    return lv_anim_speed_clamped(speed, 0, 10000);
}

uint32_t lv_anim_speed_to_time(uint32_t speed, int32_t start, int32_t end)
{
    uint32_t d = LV_ABS(start - end);
    uint32_t time = (d * 1000) / speed;

    time = time == 0 ? 1 : time;

    return time;
}

void lv_anim_refr_now(void)
{
    anim_timer(NULL);
}

int32_t lv_anim_path_linear(const lv_anim_t * a)
{
    /*Calculate the current step*/
    int32_t step = lv_map(a->act_time, 0, a->duration, 0, LV_ANIM_RESOLUTION);

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
    return lv_anim_path_cubic_bezier(a, LV_BEZIER_VAL_FLOAT(0.42), LV_BEZIER_VAL_FLOAT(0),
                                     LV_BEZIER_VAL_FLOAT(1), LV_BEZIER_VAL_FLOAT(1));
}

int32_t lv_anim_path_ease_out(const lv_anim_t * a)
{
    return lv_anim_path_cubic_bezier(a, LV_BEZIER_VAL_FLOAT(0), LV_BEZIER_VAL_FLOAT(0),
                                     LV_BEZIER_VAL_FLOAT(0.58), LV_BEZIER_VAL_FLOAT(1));
}

int32_t lv_anim_path_ease_in_out(const lv_anim_t * a)
{
    return lv_anim_path_cubic_bezier(a, LV_BEZIER_VAL_FLOAT(0.42), LV_BEZIER_VAL_FLOAT(0),
                                     LV_BEZIER_VAL_FLOAT(0.58), LV_BEZIER_VAL_FLOAT(1));
}

int32_t lv_anim_path_overshoot(const lv_anim_t * a)
{
    return lv_anim_path_cubic_bezier(a, 341, 0, 683, 1300);
}

int32_t lv_anim_path_bounce(const lv_anim_t * a)
{
    /*Calculate the current step*/
    int32_t t = lv_map(a->act_time, 0, a->duration, 0, LV_BEZIER_VAL_MAX);
    int32_t diff = (a->end_value - a->start_value);

    /*3 bounces has 5 parts: 3 down and 2 up. One part is t / 5 long*/

    if(t < 408) {
        /*Go down*/
        t = (t * 2500) >> LV_BEZIER_VAL_SHIFT; /*[0..1024] range*/
        t = LV_BEZIER_VAL_MAX - t;
    }
    else if(t >= 408 && t < 614) {
        /*First bounce back*/
        t -= 408;
        t    = t * 5; /*to [0..1024] range*/
        diff = diff / 20;
    }
    else if(t >= 614 && t < 819) {
        /*Fall back*/
        t -= 614;
        t    = t * 5; /*to [0..1024] range*/
        t    = LV_BEZIER_VAL_MAX - t;
        diff = diff / 20;
    }
    else if(t >= 819 && t < 921) {
        /*Second bounce back*/
        t -= 819;
        t    = t * 10; /*to [0..1024] range*/
        diff = diff / 40;
    }
    else if(t >= 921 && t <= LV_BEZIER_VAL_MAX) {
        /*Fall back*/
        t -= 921;
        t    = t * 10; /*to [0..1024] range*/
        t    = LV_BEZIER_VAL_MAX - t;
        diff = diff / 40;
    }

    if(t > LV_BEZIER_VAL_MAX) t = LV_BEZIER_VAL_MAX;
    if(t < 0) t = 0;
    int32_t step = lv_bezier3(t, 0, 500, 800, LV_BEZIER_VAL_MAX);

    int32_t new_value;
    new_value = step * diff;
    new_value = new_value >> LV_BEZIER_VAL_SHIFT;
    new_value = a->end_value - new_value;

    return new_value;
}

int32_t lv_anim_path_step(const lv_anim_t * a)
{
    if(a->act_time >= a->duration)
        return a->end_value;
    else
        return a->start_value;
}

int32_t lv_anim_path_custom_bezier3(const lv_anim_t * a)
{
    const lv_anim_bezier3_para_t * para = &a->parameter.bezier3;
    return lv_anim_path_cubic_bezier(a, para->x1, para->y1, para->x2, para->y2);
}

void lv_anim_set_var(lv_anim_t * a, void * var)
{
    a->var = var;
}

void lv_anim_set_exec_cb(lv_anim_t * a, lv_anim_exec_xcb_t exec_cb)
{
    a->exec_cb = exec_cb;
}

void lv_anim_set_duration(lv_anim_t * a, uint32_t duration)
{
    a->duration = duration;
}

void lv_anim_set_delay(lv_anim_t * a, uint32_t delay)
{
    a->act_time = -(int32_t)(delay);
}

void lv_anim_set_values(lv_anim_t * a, int32_t start, int32_t end)
{
    a->start_value = start;
    a->current_value = INT32_MIN;
    a->end_value = end;
}

void lv_anim_set_custom_exec_cb(lv_anim_t * a, lv_anim_custom_exec_cb_t exec_cb)
{
    a->custom_exec_cb = exec_cb;
}

void lv_anim_set_path_cb(lv_anim_t * a, lv_anim_path_cb_t path_cb)
{
    a->path_cb = path_cb;
}

void lv_anim_set_start_cb(lv_anim_t * a, lv_anim_start_cb_t start_cb)
{
    a->start_cb = start_cb;
}

void lv_anim_set_get_value_cb(lv_anim_t * a, lv_anim_get_value_cb_t get_value_cb)
{
    a->get_value_cb = get_value_cb;
}

void lv_anim_set_completed_cb(lv_anim_t * a, lv_anim_completed_cb_t completed_cb)
{
    a->completed_cb = completed_cb;
}

void lv_anim_set_deleted_cb(lv_anim_t * a, lv_anim_deleted_cb_t deleted_cb)
{
    a->deleted_cb = deleted_cb;
}

void lv_anim_set_reverse_duration(lv_anim_t * a, uint32_t duration)
{
    a->reverse_duration = duration;
}

void lv_anim_set_reverse_time(lv_anim_t * a, uint32_t duration)
{
    lv_anim_set_reverse_duration(a, duration);
}

void lv_anim_set_reverse_delay(lv_anim_t * a, uint32_t delay)
{
    a->reverse_delay = delay;
}

void lv_anim_set_repeat_count(lv_anim_t * a, uint32_t cnt)
{
    a->repeat_cnt = cnt;
}

void lv_anim_set_repeat_delay(lv_anim_t * a, uint32_t delay)
{
    a->repeat_delay = delay;
}

void lv_anim_set_early_apply(lv_anim_t * a, bool en)
{
    a->early_apply = en;
}

void lv_anim_set_user_data(lv_anim_t * a, void * user_data)
{
    a->user_data = user_data;
}

void lv_anim_set_bezier3_param(lv_anim_t * a, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    lv_anim_bezier3_para_t * para = &a->parameter.bezier3;

    para->x1 = x1;
    para->x2 = x2;
    para->y1 = y1;
    para->y2 = y2;
}

uint32_t lv_anim_get_delay(const lv_anim_t * a)
{
    return -a->act_time;
}

uint32_t lv_anim_get_time(const lv_anim_t * a)
{
    return a->duration;
}

uint32_t lv_anim_get_repeat_count(const lv_anim_t * a)
{
    return a->repeat_cnt;
}

void * lv_anim_get_user_data(const lv_anim_t * a)
{
    return a->user_data;
}

bool lv_anim_custom_delete(lv_anim_t * a, lv_anim_custom_exec_cb_t exec_cb)
{
    return lv_anim_delete(a ? a->var : NULL, (lv_anim_exec_xcb_t)exec_cb);
}

lv_anim_t * lv_anim_custom_get(lv_anim_t * a, lv_anim_custom_exec_cb_t exec_cb)
{
    return lv_anim_get(a ? a->var : NULL, (lv_anim_exec_xcb_t)exec_cb);
}

uint32_t lv_anim_resolve_speed(uint32_t speed_or_time, int32_t start, int32_t end)
{
    /*It was a simple time*/
    if((speed_or_time & LV_ANIM_SPEED_MASK) == 0) return speed_or_time;

    uint32_t d    = LV_ABS(start - end);
    uint32_t speed = speed_or_time & 0x3FF;
    uint32_t time = (d * 100) / speed; /*Speed is in 10 units per sec*/
    uint32_t max_time = (speed_or_time >> 20) & 0x3FF;
    uint32_t min_time = (speed_or_time >> 10) & 0x3FF;

    return LV_CLAMP(min_time * 10, time, max_time * 10);
}

bool lv_anim_is_paused(lv_anim_t * a)
{
    LV_ASSERT_NULL(a);
    return a->is_paused;
}

void lv_anim_pause(lv_anim_t * a)
{
    LV_ASSERT_NULL(a);
    lv_anim_pause_for_internal(a, LV_ANIM_PAUSE_FOREVER);
}

void lv_anim_pause_for(lv_anim_t * a, uint32_t ms)
{
    LV_ASSERT_NULL(a);
    lv_anim_pause_for_internal(a, ms);
}

void lv_anim_resume(lv_anim_t * a)
{
    LV_ASSERT_NULL(a);
    a->is_paused = false;
    a->pause_duration = 0;
    a->run_round = state.anim_run_round;
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

    /*Flip the run round*/
    state.anim_run_round = state.anim_run_round ? false : true;

    lv_anim_t * a = lv_ll_get_head(anim_ll_p);
    while(a != NULL) {
        uint32_t elaps = lv_tick_elaps(a->last_timer_run);

        if(a->is_paused) {
            const uint32_t time_paused = lv_tick_elaps(a->pause_time);
            const bool is_pause_over = a->pause_duration != LV_ANIM_PAUSE_FOREVER && time_paused >= a->pause_duration;

            if(is_pause_over) {
                const uint32_t pause_overrun = time_paused - a->pause_duration;
                a->is_paused = false;
                a->act_time += pause_overrun;
                a->run_round = !state.anim_run_round;
            }
        }
        else {
            a->act_time += elaps;
        }
        a->last_timer_run = lv_tick_get();

        /*It can be set by `lv_anim_delete()` typically in `end_cb`. If set then an animation delete
         * happened in `anim_completed_handler` which could make this linked list reading corrupt
         * because the list is changed meanwhile
         */
        state.anim_list_changed = false;

        if(!a->is_paused && a->run_round != state.anim_run_round) {
            a->run_round = state.anim_run_round; /*The list readying might be reset so need to know which anim has run already*/
            /*The animation will run now for the first time. Call `start_cb`*/
            if(!a->start_cb_called && a->act_time >= 0) {

                if(a->early_apply == 0 && a->get_value_cb) {
                    int32_t v_ofs = a->get_value_cb(a);
                    a->start_value += v_ofs;
                    a->end_value += v_ofs;
                }

                resolve_time(a);

                if(a->start_cb) a->start_cb(a);
                a->start_cb_called = 1;

                /*Do not let two animations for the same 'var' with the same 'exec_cb'*/
                remove_concurrent_anims(a);
            }

            if(a->act_time >= 0) {
                int32_t act_time_original = a->act_time; /*The unclipped version is used later to correctly repeat the animation*/
                if(a->act_time > a->duration) a->act_time = a->duration;

                int32_t act_time_before_exec = a->act_time;
                int32_t new_value;
                new_value = a->path_cb(a);

                if(new_value != a->current_value) {
                    a->current_value = new_value;
                    /*Apply the calculated value*/
                    if(a->exec_cb) a->exec_cb(a->var, new_value);
                    if(!state.anim_list_changed && a->custom_exec_cb) a->custom_exec_cb(a, new_value);
                }

                if(!state.anim_list_changed) {
                    /*Restore the original time to see is there is over time.
                     *Restore only if it wasn't changed in the `exec_cb` for some special reasons.*/
                    if(a->act_time == act_time_before_exec) a->act_time = act_time_original;

                    /*If the time is elapsed the animation is ready*/
                    if(a->act_time >= a->duration) {
                        anim_completed_handler(a);
                    }
                }
            }
        }

        /*If the linked list changed due to anim. delete then it's not safe to continue
         *the reading of the list from here -> start from the head*/
        if(state.anim_list_changed)
            a = lv_ll_get_head(anim_ll_p);
        else
            a = lv_ll_get_next(anim_ll_p, a);
    }

}

/**
 * Called when an animation is completed to do the necessary things
 * e.g. repeat, play in reverse, delete etc.
 * @param a pointer to an animation descriptor
 */
static void anim_completed_handler(lv_anim_t * a)
{
    /*In the end of a forward anim decrement repeat cnt.*/
    if(a->reverse_play_in_progress == 0 && a->repeat_cnt > 0 && a->repeat_cnt != LV_ANIM_REPEAT_INFINITE) {
        a->repeat_cnt--;
    }

    /*Delete animation if
     * - no repeat left and no reverse play scheduled (simple one shot animation); or
     * - no repeat, reverse play enabled (reverse_duration != 0) and reverse play is completed. */
    if(a->repeat_cnt == 0 && (a->reverse_duration == 0 || a->reverse_play_in_progress == 1)) {

        /*Delete the animation from the list.
         * This way the `completed_cb` will see the animations like it's animation is already deleted*/
        lv_ll_remove(anim_ll_p, a);
        /*Flag that the list has changed*/
        anim_mark_list_change();

        /*Call the callback function at the end*/
        if(a->completed_cb != NULL) a->completed_cb(a);
        if(a->deleted_cb != NULL) a->deleted_cb(a);
        lv_free(a);
    }
    /*If the animation is not deleted then restart it*/
    else {
        /*Restart the animation. If the time is over a little compensate it.*/
        int32_t over_time = 0;
        if(a->act_time > a->duration) over_time = a->act_time - a->duration;
        a->act_time = over_time - (int32_t)(a->repeat_delay);
        /*Swap start and end values in reverse-play mode*/
        if(a->reverse_duration != 0) {
            /*If now now playing in reverse, use the 'reverse_delay'.*/
            if(a->reverse_play_in_progress == 0) a->act_time = -(int32_t)(a->reverse_delay);

            /*Toggle reverse-play state*/
            a->reverse_play_in_progress = a->reverse_play_in_progress == 0 ? 1 : 0;
            /*Swap the start and end values*/
            int32_t tmp    = a->start_value;
            a->start_value = a->end_value;
            a->end_value   = tmp;
            /*Swap the time and reverse_duration*/
            tmp = a->duration;
            a->duration = a->reverse_duration;
            a->reverse_duration = tmp;
        }
    }
}

static void anim_mark_list_change(void)
{
    state.anim_list_changed = true;
    if(lv_ll_get_head(anim_ll_p) == NULL)
        lv_timer_pause(state.timer);
    else
        lv_timer_resume(state.timer);
}

static int32_t lv_anim_path_cubic_bezier(const lv_anim_t * a, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    /*Calculate the current step*/
    uint32_t t = lv_map(a->act_time, 0, a->duration, 0, LV_BEZIER_VAL_MAX);
    int32_t step = lv_cubic_bezier(t, x1, y1, x2, y2);

    int32_t new_value;
    new_value = step * (a->end_value - a->start_value);
    new_value = new_value >> LV_BEZIER_VAL_SHIFT;
    new_value += a->start_value;

    return new_value;
}

static void lv_anim_pause_for_internal(lv_anim_t * a, uint32_t ms)
{

    a->is_paused = true;
    a->pause_time = lv_tick_get();
    a->pause_duration = ms;
}

static void resolve_time(lv_anim_t * a)
{
    a->duration = lv_anim_resolve_speed(a->duration, a->start_value, a->end_value);
    a->reverse_duration = lv_anim_resolve_speed(a->reverse_duration, a->start_value, a->end_value);
    a->reverse_delay = lv_anim_resolve_speed(a->reverse_delay, a->start_value, a->end_value);
    a->repeat_delay = lv_anim_resolve_speed(a->repeat_delay, a->start_value, a->end_value);
}

/**
 * Remove animations which are animating the same var with the same exec_cb
 * and they are already running or they have early_apply
 * @param a_current     the current animation, use its var and exec_cb as reference to know what to remove
 * @return              true: at least one animation was delete
 */
static bool remove_concurrent_anims(const lv_anim_t * a_current)
{
    if(a_current->exec_cb == NULL && a_current->custom_exec_cb == NULL) return false;

    lv_anim_t * a;
    bool del_any = false;
    a = lv_ll_get_head(anim_ll_p);
    while(a != NULL) {
        bool del = false;
        /*We can't test for custom_exec_cb equality because in the MicroPython binding
         *a wrapper callback is used here an the real callback data is stored in the `user_data`.
         *Therefore equality check would remove all animations.*/
        if(a != a_current &&
           (a->act_time >= 0 || a->early_apply) &&
           (a->var == a_current->var) &&
           ((a->exec_cb && a->exec_cb == a_current->exec_cb)
            /*|| (a->custom_exec_cb && a->custom_exec_cb == a_current->custom_exec_cb)*/)) {
            lv_ll_remove(anim_ll_p, a);
            if(a->deleted_cb != NULL) a->deleted_cb(a);
            lv_free(a);
            /*Read by `anim_timer`. It need to know if a delete occurred in the linked list*/
            anim_mark_list_change();

            del_any = true;
            del = true;
        }

        /*Always start from the head on delete, because we don't know
         *how `anim_ll_p` was changes in `a->deleted_cb` */
        a = del ? lv_ll_get_head(anim_ll_p) : lv_ll_get_next(anim_ll_p, a);
    }

    return del_any;
}

static void remove_anim(void * a)
{
    lv_anim_t * anim = a;
    lv_ll_remove(anim_ll_p, a);
    if(anim->deleted_cb != NULL) anim->deleted_cb(anim);
    lv_free(a);
}
