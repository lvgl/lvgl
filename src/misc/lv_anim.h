/**
 * @file lv_anim.h
 *
 */

#ifndef LV_ANIM_H
#define LV_ANIM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

#define LV_ANIM_REPEAT_INFINITE      0xFFFF
#define LV_ANIM_PLAYTIME_INFINITE    0xFFFFFFFF

LV_EXPORT_CONST_INT(LV_ANIM_REPEAT_INFINITE);
LV_EXPORT_CONST_INT(LV_ANIM_PLAYTIME_INFINITE);

/**********************
 *      TYPEDEFS
 **********************/

/** Can be used to indicate if animations are enabled or disabled in a case*/
typedef enum {
    LV_ANIM_OFF,
    LV_ANIM_ON,
} lv_anim_enable_t;

struct _lv_anim_t;

/** Get the current value during an animation*/
typedef int32_t (*lv_anim_path_cb_t)(const struct _lv_anim_t *);

/** Generic prototype of "animator" functions.
 * First parameter is the variable to animate.
 * Second parameter is the value to set.
 * Compatible with `lv_xxx_set_yyy(obj, value)` functions
 * The `x` in `_xcb_t` means it's not a fully generic prototype because
 * it doesn't receive `lv_anim_t *` as its first argument*/
typedef void (*lv_anim_exec_xcb_t)(void *, int32_t);

/** Same as `lv_anim_exec_xcb_t` but receives `lv_anim_t *` as the first parameter.
 * It's more consistent but less convenient. Might be used by binding generator functions.*/
typedef void (*lv_anim_custom_exec_cb_t)(struct _lv_anim_t *, int32_t);

/** Callback to call when the animation is ready*/
typedef void (*lv_anim_ready_cb_t)(struct _lv_anim_t *);

/** Callback to call when the animation really stars (considering `delay`)*/
typedef void (*lv_anim_start_cb_t)(struct _lv_anim_t *);

/** Callback used when the animation values are relative to get the current value*/
typedef int32_t (*lv_anim_get_value_cb_t)(struct _lv_anim_t *);

/** Describes an animation*/
typedef struct _lv_anim_t {
    void * var;                          /**<Variable to animate*/
    lv_anim_exec_xcb_t exec_cb;          /**< Function to execute to animate*/
    lv_anim_start_cb_t start_cb;         /**< Call it when the animation is starts (considering `delay`)*/
    lv_anim_ready_cb_t ready_cb;         /**< Call it when the animation is ready*/
    lv_anim_get_value_cb_t get_value_cb; /**< Get the current value in relative mode*/
#if LV_USE_USER_DATA
    void * user_data; /**< Custom user data*/
#endif
    lv_anim_path_cb_t path_cb;         /**< Describe the path (curve) of animations*/
    int32_t start_value;               /**< Start value*/
    int32_t current_value;             /**< Current value*/
    int32_t end_value;                 /**< End value*/
    int32_t time;                /**< Animation time in ms*/
    int32_t act_time;            /**< Current time in animation. Set to negative to make delay.*/
    uint32_t playback_delay;     /**< Wait before play back*/
    uint32_t playback_time;      /**< Duration of playback animation*/
    uint32_t repeat_delay;       /**< Wait before repeat*/
    uint16_t repeat_cnt;         /**< Repeat count for the animation*/
    uint8_t early_apply  : 1;    /**< 1: Apply start value immediately even is there is `delay`*/

    /*Animation system use these - user shouldn't set*/
    uint8_t playback_now : 1; /**< Play back is in progress*/
    uint8_t run_round : 1;    /**< Indicates the animation has run in this round*/
    uint8_t start_cb_called : 1;    /**< Indicates that the `start_cb` was already called*/
} lv_anim_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init. the animation module
 */
void _lv_anim_core_init(void);

/**
 * Initialize an animation variable.
 * E.g.:
 * lv_anim_t a;
 * lv_anim_init(&a);
 * lv_anim_set_...(&a);
 * lv_anim_start(&a);
 * @param a     pointer to an `lv_anim_t` variable to initialize
 */
void lv_anim_init(lv_anim_t * a);

/**
 * Set a variable to animate
 * @param a     pointer to an initialized `lv_anim_t` variable
 * @param var   pointer to a variable to animate
 */
static inline void lv_anim_set_var(lv_anim_t * a, void * var)
{
    a->var = var;
}

/**
 * Set a function to animate `var`
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param exec_cb   a function to execute during animation
 *                  LVGL's built-in functions can be used.
 *                  E.g. lv_obj_set_x
 */
static inline void lv_anim_set_exec_cb(lv_anim_t * a, lv_anim_exec_xcb_t exec_cb)
{
    a->exec_cb = exec_cb;
}

/**
 * Set the duration of an animation
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param duration  duration of the animation in milliseconds
 */
static inline void lv_anim_set_time(lv_anim_t * a, uint32_t duration)
{
    a->time = duration;
}

/**
 * Set a delay before starting the animation
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param delay     delay before the animation in milliseconds
 */
static inline void lv_anim_set_delay(lv_anim_t * a, uint32_t delay)
{
    a->act_time = -(int32_t)(delay);
}

/**
 * Set the start and end values of an animation
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param start     the start value
 * @param end       the end value
 */
static inline void lv_anim_set_values(lv_anim_t * a, int32_t start, int32_t end)
{
    a->start_value = start;
    a->current_value = start;
    a->end_value = end;
}

/**
 * Similar to `lv_anim_set_exec_cb` but `lv_anim_custom_exec_cb_t` receives
 * `lv_anim_t * ` as its first parameter instead of `void *`.
 * This function might be used when LVGL is bound to other languages because
 * it's more consistent to have `lv_anim_t *` as first parameter.
 * The variable to animate can be stored in the animation's `user_data`
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param exec_cb   a function to execute.
 */
static inline void lv_anim_set_custom_exec_cb(lv_anim_t * a, lv_anim_custom_exec_cb_t exec_cb)
{
    a->var     = a;
    a->exec_cb = (lv_anim_exec_xcb_t)exec_cb;
}

/**
 * Set the path (curve) of the animation.
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param path_cb a function to set the current value of the animation.
 */
static inline void lv_anim_set_path_cb(lv_anim_t * a, lv_anim_path_cb_t path_cb)
{
    a->path_cb = path_cb;
}

/**
 * Set a function call when the animation really starts (considering `delay`)
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param start_cb  a function call when the animation starts
 */
static inline void lv_anim_set_start_cb(lv_anim_t * a, lv_anim_start_cb_t start_cb)
{
    a->start_cb = start_cb;
}

/**
 * Set a function to use the current value of the variable and make start and end value
 * relative to the returned current value.
 * @param a             pointer to an initialized `lv_anim_t` variable
 * @param get_value_cb  a function call when the animation starts
 */
static inline void lv_anim_set_get_value_cb(lv_anim_t * a, lv_anim_get_value_cb_t get_value_cb)
{
    a->get_value_cb = get_value_cb;
}

/**
 * Set a function call when the animation is ready
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param ready_cb  a function call when the animation is ready
 */
static inline void lv_anim_set_ready_cb(lv_anim_t * a, lv_anim_ready_cb_t ready_cb)
{
    a->ready_cb = ready_cb;
}

/**
 * Make the animation to play back to when the forward direction is ready
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param time      the duration of the playback animation in milliseconds. 0: disable playback
 */
static inline void lv_anim_set_playback_time(lv_anim_t * a, uint32_t time)
{
    a->playback_time = time;
}

/**
 * Make the animation to play back to when the forward direction is ready
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param delay     delay in milliseconds before starting the playback animation.
 */
static inline void lv_anim_set_playback_delay(lv_anim_t * a, uint32_t delay)
{
    a->playback_delay = delay;
}

/**
 * Make the animation repeat itself.
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param cnt       repeat count or `LV_ANIM_REPEAT_INFINITE` for infinite repetition. 0: to disable repetition.
 */
static inline void lv_anim_set_repeat_count(lv_anim_t * a, uint16_t cnt)
{
    a->repeat_cnt = cnt;
}

/**
 * Set a delay before repeating the animation.
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param delay     delay in milliseconds before repeating the animation.
 */
static inline void lv_anim_set_repeat_delay(lv_anim_t * a, uint32_t delay)
{
    a->repeat_delay = delay;
}

/**
 * Set a whether the animation's should be applied immediately or only when the delay expired.
 * @param a         pointer to an initialized `lv_anim_t` variable
 * @param en        true: apply the start value immediately in `lv_anim_start`;
 *                  false: apply the start value only when `delay` ms is elapsed and the animations really starts
 */
static inline void lv_anim_set_early_apply(lv_anim_t * a, bool en)
{
    a->early_apply = en;
}

/**
 * Set the custom user data field of the animation.
 * @param a           pointer to an initialized `lv_anim_t` variable
 * @param user_data   pointer to the new user_data.
 */
#if LV_USE_USER_DATA
static inline void lv_anim_set_user_data(lv_anim_t * a, void * user_data)
{
    a->user_data = user_data;
}
#endif

/**
 * Create an animation
 * @param a         an initialized 'anim_t' variable. Not required after call.
 * @return          pointer to the created animation (different from the `a` parameter)
 */
lv_anim_t * lv_anim_start(const lv_anim_t * a);

/**
 * Get a delay before starting the animation
 * @param a pointer to an initialized `lv_anim_t` variable
 * @return delay before the animation in milliseconds
 */
static inline uint32_t lv_anim_get_delay(lv_anim_t * a)
{
    return -a->act_time;
}

/**
 * Get the time used to play the animation.
 * @param a pointer to an animation.
 * @return the play time in milliseconds.
 */
uint32_t lv_anim_get_playtime(lv_anim_t * a);

/**
 * Get the user_data field of the animation
 * @param   a pointer to an initialized `lv_anim_t` variable
 * @return  the pointer to the custom user_data of the animation
 */
#if LV_USE_USER_DATA
static inline void * lv_anim_get_user_data(lv_anim_t * a)
{
    return a->user_data;
}
#endif

/**
 * Delete an animation of a variable with a given animator function
 * @param var       pointer to variable
 * @param exec_cb   a function pointer which is animating 'var',
 *                  or NULL to ignore it and delete all the animations of 'var
 * @return          true: at least 1 animation is deleted, false: no animation is deleted
 */
bool lv_anim_del(void * var, lv_anim_exec_xcb_t exec_cb);

/**
 * Delete all the animations
 */
void lv_anim_del_all(void);

/**
 * Get the animation of a variable and its `exec_cb`.
 * @param var       pointer to variable
 * @param exec_cb   a function pointer which is animating 'var', or NULL to return first matching 'var'
 * @return          pointer to the animation.
 */
lv_anim_t * lv_anim_get(void * var, lv_anim_exec_xcb_t exec_cb);

/**
 * Delete an animation by getting the animated variable from `a`.
 * Only animations with `exec_cb` will be deleted.
 * This function exists because it's logical that all anim. functions receives an
 * `lv_anim_t` as their first parameter. It's not practical in C but might make
 * the API more consequent and makes easier to generate bindings.
 * @param a         pointer to an animation.
 * @param exec_cb   a function pointer which is animating 'var',
 *                  or NULL to ignore it and delete all the animations of 'var
 * @return          true: at least 1 animation is deleted, false: no animation is deleted
 */
static inline bool lv_anim_custom_del(lv_anim_t * a, lv_anim_custom_exec_cb_t exec_cb)
{
    return lv_anim_del(a ? a->var : NULL, (lv_anim_exec_xcb_t)exec_cb);
}

/**
 * Get the animation of a variable and its `exec_cb`.
 * This function exists because it's logical that all anim. functions receives an
 * `lv_anim_t` as their first parameter. It's not practical in C but might make
 * the API more consequent and makes easier to generate bindings.
 * @param a         pointer to an animation.
 * @param exec_cb   a function pointer which is animating 'var', or NULL to return first matching 'var'
 * @return          pointer to the animation.
 */
static inline lv_anim_t * lv_anim_custom_get(lv_anim_t * a, lv_anim_custom_exec_cb_t exec_cb)
{
    return lv_anim_get(a ? a->var : NULL, (lv_anim_exec_xcb_t)exec_cb);
}

/**
 * Get the number of currently running animations
 * @return      the number of running animations
 */
uint16_t lv_anim_count_running(void);

/**
 * Calculate the time of an animation with a given speed and the start and end values
 * @param speed speed of animation in unit/sec
 * @param start     start value of the animation
 * @param end       end value of the animation
 * @return          the required time [ms] for the animation with the given parameters
 */
uint32_t lv_anim_speed_to_time(uint32_t speed, int32_t start, int32_t end);

/**
 * Manually refresh the state of the animations.
 * Useful to make the animations running in a blocking process where
 * `lv_timer_handler` can't run for a while.
 * Shouldn't be used directly because it is called in `lv_refr_now()`.
 */
void lv_anim_refr_now(void);

/**
 * Calculate the current value of an animation applying linear characteristic
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_linear(const lv_anim_t * a);

/**
 * Calculate the current value of an animation slowing down the start phase
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_ease_in(const lv_anim_t * a);

/**
 * Calculate the current value of an animation slowing down the end phase
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_ease_out(const lv_anim_t * a);

/**
 * Calculate the current value of an animation applying an "S" characteristic (cosine)
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_ease_in_out(const lv_anim_t * a);

/**
 * Calculate the current value of an animation with overshoot at the end
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_overshoot(const lv_anim_t * a);

/**
 * Calculate the current value of an animation with 3 bounces
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_bounce(const lv_anim_t * a);

/**
 * Calculate the current value of an animation applying step characteristic.
 * (Set end value on the end of the animation)
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_step(const lv_anim_t * a);

/**********************
 *   GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ANIM_H*/
