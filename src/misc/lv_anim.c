/**
 * @file lv_anim.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim.h"

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
#define LV_ANIM_RESOLUTION 1024
#define LV_ANIM_RES_SHIFT 10
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
static uint32_t convert_speed_to_time(uint32_t speed, int32_t start, int32_t end);
static void resolve_time(lv_anim_t * a);
static bool remove_concurrent_anims(lv_anim_t * a_current);
static void anim_set_start_values(lv_anim_t * a);
static void anim_list_remove(lv_anim_t * a, bool call_completed, bool is_in_the_list);
static lv_anim_t * anim_list_allocate(void);
static void anim_list_add(lv_anim_t * a);
static void anim_init(lv_anim_t * a);
static void anim_toggle_playback_state(lv_anim_t * a);

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

void _lv_anim_core_init(void)
{
    _lv_ll_init(anim_ll_p, sizeof(lv_anim_t));
    state.timer = lv_timer_create(anim_timer, LV_DEF_REFR_PERIOD, NULL);
    anim_mark_list_change(); /*Turn off the animation timer*/
    state.anim_list_changed = false;
    state.anim_run_round = false;
}

void _lv_anim_core_deinit(void)
{
    lv_anim_delete_all();
}

void lv_anim_init(lv_anim_t * a)
{
    LV_LOG_WARN("`lv_anim_init()` is deprecated. Please use `lv_anim_create()` instead.");

    anim_init(a);
}

lv_anim_t * lv_anim_start(const lv_anim_t * a)
{
    LV_LOG_WARN("`lv_anim_start()` is deprecated. Please use `lv_anim_trigger()` instead.");

    LV_TRACE_ANIM("begin");

    lv_anim_t * new_anim = anim_list_allocate();
    if(new_anim == NULL) return NULL;

    /*Initialize the animation descriptor*/
    lv_memcpy(new_anim, a, sizeof(lv_anim_t));
    if(a->var == a) new_anim->var = new_anim;

    anim_set_start_values(new_anim);
    anim_list_add(new_anim);

    LV_TRACE_ANIM("finished");
    return new_anim;
}

lv_anim_t * lv_anim_create(void)
{
    lv_anim_t * new_anim = anim_list_allocate();
    if(new_anim == NULL) return NULL;

    anim_init(new_anim);

    return new_anim;
}

lv_anim_t * lv_anim_dup(const lv_anim_t * other)
{
    lv_anim_t * new_anim = anim_list_allocate();
    if(new_anim == NULL) return NULL;

    /* Initialize with the other animation */
    lv_memcpy(new_anim, other, sizeof(lv_anim_t));

    return new_anim;
}

void lv_anim_trigger(lv_anim_t * a)
{
    anim_set_start_values(a);
    anim_list_add(a);
}

void lv_anim_pause(lv_anim_t * a)
{
    if((a->pause_cb != NULL) && a->is_running) a->pause_cb(a);
    a->is_running = false;
}

void lv_anim_resume(lv_anim_t * a)
{
    if((a->resume_cb != NULL) && !a->is_running) a->resume_cb(a);
    a->is_running = true;
}

bool lv_anim_toggle_running(lv_anim_t * a)
{
    if(a->is_running) {
        if(a->pause_cb != NULL) a->pause_cb(a);
        a->is_running = false;
    }
    else {
        if(a->resume_cb != NULL) a->resume_cb(a);
        a->is_running = true;
    }

    return a->is_running;
}

void lv_anim_set_current_time(lv_anim_t * a, uint32_t current_time)
{
    int32_t value = 0;

    if(a->playback_now) anim_toggle_playback_state(a);

    if(current_time <= (uint32_t)a->duration) {
        a->current_time = current_time;
        value = a->path_cb(a);
    }
    else { // current_time > duration
        value = a->end_value;
    }

    if(a->exec_cb) a->exec_cb(a->var, value);
    if(a->custom_exec_cb) a->custom_exec_cb(a, value);
}

uint32_t lv_anim_get_playtime(const lv_anim_t * a)
{
    if(a->repeat_cnt == LV_ANIM_REPEAT_INFINITE) {
        return LV_ANIM_PLAYTIME_INFINITE;
    }

    uint32_t repeat_cnt = a->repeat_cnt;
    if(repeat_cnt < 1) repeat_cnt = 1;

    uint32_t playtime = a->repeat_delay + a->duration + a->playback_delay + a->playback_duration;
    playtime = playtime * a->repeat_cnt;
    return playtime;
}

bool lv_anim_delete(void * var, lv_anim_exec_xcb_t exec_cb)
{
    lv_anim_t * a;
    bool del_any = false;
    a        = _lv_ll_get_head(anim_ll_p);
    while(a != NULL) {
        bool del = false;
        if((a->var == var || var == NULL) && (a->exec_cb == exec_cb || exec_cb == NULL)) {
            anim_list_remove(a, false, true);
            del_any = true;
            del = true;
        }

        /*Always start from the head on delete, because we don't know
         *how `anim_ll_p` was changes in `a->deleted_cb` */
        a = del ? _lv_ll_get_head(anim_ll_p) : _lv_ll_get_next(anim_ll_p, a);
    }

    return del_any;
}

void lv_anim_delete_by_ptr(lv_anim_t * a)
{
    bool is_in_the_list = _lv_ll_get_next(anim_ll_p, a) || _lv_ll_get_prev(anim_ll_p, a);

    anim_list_remove(a, false, is_in_the_list);
}

void lv_anim_delete_all(void)
{
    _lv_ll_clear(anim_ll_p);
    anim_mark_list_change();
}

lv_anim_t * lv_anim_get(void * var, lv_anim_exec_xcb_t exec_cb)
{
    lv_anim_t * a;
    _LV_LL_READ(anim_ll_p, a) {
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
    _LV_LL_READ(anim_ll_p, a) cnt++;

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

    return 0x80000000 + (max_time << 20) + (min_time << 10) + speed;

}

uint32_t lv_anim_speed(uint32_t speed)
{
    return lv_anim_speed_clamped(speed, 0, 10000);
}

void lv_anim_refr_now(void)
{
    anim_timer(NULL);
}

int32_t lv_anim_path_linear(const lv_anim_t * a)
{
    /*Calculate the current step*/
    int32_t step = lv_map(a->current_time, 0, a->duration, 0, LV_ANIM_RESOLUTION);

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
    int32_t t = lv_map(a->current_time, 0, a->duration, 0, LV_BEZIER_VAL_MAX);
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
    if(a->current_time >= a->duration)
        return a->end_value;
    else
        return a->start_value;
}

int32_t lv_anim_path_cubic_bezier(const lv_anim_t * a, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    /*Calculate the current step*/
    uint32_t t = lv_map(a->current_time, 0, a->duration, 0, LV_BEZIER_VAL_MAX);
    int32_t step = lv_cubic_bezier(t, x1, y1, x2, y2);

    int32_t new_value;
    new_value = step * (a->end_value - a->start_value);
    new_value = new_value >> LV_BEZIER_VAL_SHIFT;
    new_value += a->start_value;

    return new_value;
}

int32_t lv_anim_path_custom_bezier3(const lv_anim_t * a)
{
    const struct _lv_anim_bezier3_para_t * para = &a->parameter.bezier3;
    return lv_anim_path_cubic_bezier(a, para->x1, para->y1, para->x2, para->y2);
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

    lv_anim_t * a = _lv_ll_get_head(anim_ll_p);

    while(a != NULL) {
        uint32_t elaps = lv_tick_elaps(a->last_timer_run);
        a->last_timer_run = lv_tick_get();

        /*It can be set by `lv_anim_delete()` typically in `end_cb`. If set then an animation delete
         * happened in `anim_completed_handler` which could make this linked list reading corrupt
         * because the list is changed meanwhile
         */
        state.anim_list_changed = false;

        if(!a->is_running)
            goto get_next_element;

        a->current_time += elaps;

        if(a->run_round != state.anim_run_round) {
            a->run_round = state.anim_run_round; /*The list readying might be reset so need to know which anim has run already*/

            /*The animation will run now for the first time. Call `start_cb`*/
            if(!a->start_cb_called && a->current_time >= 0) {

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

            if(a->current_time >= 0) {
                if(a->current_time > a->duration) a->current_time = a->duration;

                int32_t new_value;
                new_value = a->path_cb(a);

                if(new_value != a->current_value) {
                    a->current_value = new_value;
                    /*Apply the calculated value*/
                    if(a->exec_cb) a->exec_cb(a->var, new_value);
                    if(!state.anim_list_changed && a->custom_exec_cb) a->custom_exec_cb(a, new_value);
                }

                /*If the time is elapsed the animation is ready*/
                if(!state.anim_list_changed && a->current_time >= a->duration) {
                    anim_completed_handler(a);
                }
            }
        }

get_next_element:
        /*If the linked list changed due to anim. delete then it's not safe to continue
         *the reading of the list from here -> start from the head*/
        if(state.anim_list_changed)
            a = _lv_ll_get_head(anim_ll_p);
        else
            a = _lv_ll_get_next(anim_ll_p, a);
    }

}

/**
 * Called when an animation is completed to do the necessary things
 * e.g. repeat, play back, delete etc.
 * @param a pointer to an animation descriptor
 */
static void anim_completed_handler(lv_anim_t * a)
{
    /*In the end of a forward anim decrement repeat cnt.*/
    if(a->playback_now == 0 && a->repeat_cnt > 0 && a->repeat_cnt != LV_ANIM_REPEAT_INFINITE) {
        a->repeat_cnt--;
    }

    /*Delete the animation if
     * - no repeat left and no play back (simple one shot animation)
     * - no repeat, play back is enabled and play back is ready*/
    if(a->repeat_cnt == 0 && (a->playback_duration == 0 || a->playback_now == 1)) {
        anim_list_remove(a, true, true);
    }
    /*If the animation is not deleted then restart it*/
    else {
        a->current_time = -(int32_t)(a->repeat_delay); /*Restart the animation*/
        /*Swap the start and end values in play back mode*/
        if(a->playback_duration != 0) {
            /*If now turning back use the 'playback_pause*/
            if(a->playback_now == 0) a->current_time = -(int32_t)(a->playback_delay);
            anim_toggle_playback_state(a);
        }
    }
}

static void anim_mark_list_change(void)
{
    state.anim_list_changed = true;
    if(_lv_ll_get_head(anim_ll_p) == NULL)
        lv_timer_pause(state.timer);
    else
        lv_timer_resume(state.timer);
}

static uint32_t convert_speed_to_time(uint32_t speed_or_time, int32_t start, int32_t end)
{
    /*It was a simple time*/
    if((speed_or_time & 0x80000000) == 0) return speed_or_time;

    uint32_t d    = LV_ABS(start - end);
    uint32_t speed = speed_or_time & 0x3FF;
    uint32_t time = (d * 100) / speed; /*Speed is in 10 units per sec*/
    uint32_t max_time = (speed_or_time >> 20) & 0x3FF;
    uint32_t min_time = (speed_or_time >> 10) & 0x3FF;

    return LV_CLAMP(min_time * 10, time, max_time * 10);
}

static void resolve_time(lv_anim_t * a)
{
    a->duration = convert_speed_to_time(a->duration, a->start_value, a->end_value);
    a->playback_duration = convert_speed_to_time(a->playback_duration, a->start_value, a->end_value);
    a->playback_delay = convert_speed_to_time(a->playback_delay, a->start_value, a->end_value);
    a->repeat_delay = convert_speed_to_time(a->repeat_delay, a->start_value, a->end_value);
}

/**
 * Remove animations which are animating the same var with the same exec_cb
 * and they are already running or they have early_apply
 * @param a_current     the current animation, use its var and exec_cb as reference to know what to remove
 * @return              true: at least one animation was delete
 */
static bool remove_concurrent_anims(lv_anim_t * a_current)
{
    if(a_current->exec_cb == NULL && a_current->custom_exec_cb == NULL) return false;

    lv_anim_t * a;
    bool del_any = false;
    a = _lv_ll_get_head(anim_ll_p);
    while(a != NULL) {
        bool del = false;
        /*We can't test for custom_exec_cb equality because in the MicroPython binding
         *a wrapper callback is used here an the real callback data is stored in the `user_data`.
         *Therefore equality check would remove all animations.*/
        if(a != a_current &&
           (a->current_time >= 0 || a->early_apply) &&
           (a->var == a_current->var) &&
           ((a->exec_cb && a->exec_cb == a_current->exec_cb)
            /*|| (a->custom_exec_cb && a->custom_exec_cb == a_current->custom_exec_cb)*/)) {
            anim_list_remove(a, false, true);
            del_any = true;
            del = true;
        }

        /*Always start from the head on delete, because we don't know
         *how `anim_ll_p` was changes in `a->deleted_cb` */
        a = del ? _lv_ll_get_head(anim_ll_p) : _lv_ll_get_next(anim_ll_p, a);
    }

    return del_any;
}

static void anim_set_start_values(lv_anim_t * a)
{
    a->run_round = state.anim_run_round;
    a->last_timer_run = lv_tick_get();

    if(a->early_apply) {
        if(a->get_value_cb) {
            int32_t v_ofs = a->get_value_cb(a);
            a->start_value += v_ofs;
            a->end_value += v_ofs;

        }

        resolve_time(a);

        /*Do not let two animations for the same 'var' with the same 'exec_cb'*/
        if(a->exec_cb || a->custom_exec_cb) remove_concurrent_anims(a);

        if(a->exec_cb) {
            a->exec_cb(a->var, a->start_value);
        }
        if(a->custom_exec_cb) {
            a->custom_exec_cb(a, a->start_value);
        }
    }
}

static void anim_list_remove(lv_anim_t * a, bool call_completed, bool is_in_the_list)
{
    /* Might not be part of the list (was never triggered)*/
    if(is_in_the_list) {
        /*Delete the animation from the list first.
        * This way the callback will see the animation like it's animation is already deleted*/
        _lv_ll_remove(anim_ll_p, a);

        /*Flag that the list has changed*/
        anim_mark_list_change();
    }

    /*Call the callback function at the end*/
    if(call_completed && a->completed_cb) a->completed_cb(a);
    if(a->deleted_cb) a->deleted_cb(a);

    lv_free(a);
}

static lv_anim_t * anim_list_allocate(void)
{
    lv_anim_t * new_anim = _lv_ll_node_allocate(anim_ll_p);
    LV_ASSERT_MALLOC(new_anim);

    return new_anim;
}

static void anim_list_add(lv_anim_t * a)
{
    _lv_ll_node_ins_head(anim_ll_p, a);

    /*Creating an animation changed the linked list.
     *It's important if it happens in a ready callback. (see `anim_timer`)*/
    anim_mark_list_change();
}

static void anim_init(lv_anim_t * a)
{
    lv_memzero(a, sizeof(lv_anim_t));
    a->duration = 500;
    a->start_value = 0;
    a->end_value = 100;
    a->repeat_cnt = 1;
    a->path_cb = lv_anim_path_linear;
    a->early_apply = 1;
    a->is_running = 1;
}

static void anim_toggle_playback_state(lv_anim_t * a)
{
    a->playback_now = a->playback_now == 0 ? 1 : 0;
    /*Swap the start and end values*/
    int32_t tmp    = a->start_value;
    a->start_value = a->end_value;
    a->end_value   = tmp;
    /*Swap the time and playback_duration*/
    tmp = a->duration;
    a->duration = a->playback_duration;
    a->playback_duration = tmp;
}
