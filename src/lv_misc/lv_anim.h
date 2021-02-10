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
#include <string.h>
#include "lv_mem.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Can be used to indicate if animations are enabled or disabled in a case*/
enum {
    LV_ANIM_OFF,
    LV_ANIM_ON,
};

typedef uint8_t lv_anim_enable_t;

/** Type of the animated value*/
typedef lv_coord_t lv_anim_value_t;

#if LV_USE_ANIMATION

#define LV_ANIM_REPEAT_INFINITE      0xFFFF

struct _lv_anim_t;
struct _lv_anim_path_t;
/** Get the current value during an animation*/
typedef lv_anim_value_t (*lv_anim_path_cb_t)(const struct _lv_anim_path_t *, const struct _lv_anim_t *);

typedef struct _lv_anim_path_t {
    lv_anim_path_cb_t cb;
    void * user_data;
} lv_anim_path_t;

/** Generic prototype of "animator" functions.
 * First parameter is the variable to animate.
 * Second parameter is the value to set.
 * Compatible with `lv_xxx_set_yyy(obj, value)` functions
 * The `x` in `_xcb_t` means its not a fully generic prototype because
 * it doesn't receive `lv_anim_t *` as its first argument*/
typedef void (*lv_anim_exec_xcb_t)(void *, lv_anim_value_t);

/** Same as `lv_anim_exec_xcb_t` but receives `lv_anim_t *` as the first parameter.
 * It's more consistent but less convenient. Might be used by binding generator functions.*/
typedef void (*lv_anim_custom_exec_cb_t)(struct _lv_anim_t *, lv_anim_value_t);

/** Callback to call when the animation is ready*/
typedef void (*lv_anim_ready_cb_t)(struct _lv_anim_t *);

/** Callback to call when the animation really stars (considering `delay`)*/
typedef void (*lv_anim_start_cb_t)(struct _lv_anim_t *);

/** Describes an animation*/
typedef struct _lv_anim_t {
    void * var;                  /**<Variable to animate*/
    lv_anim_exec_xcb_t exec_cb;  /**< Function to execute to animate*/
    lv_anim_start_cb_t start_cb; /**< Call it when the animation is starts (considering `delay`)*/
    lv_anim_ready_cb_t ready_cb; /**< Call it when the animation is ready*/
#if LV_USE_USER_DATA
    lv_anim_user_data_t user_data; /**< Custom user data*/
#endif
    lv_anim_path_t path;         /**< Describe the path (curve) of animations*/
    int32_t start;               /**< Start value*/
    int32_t current;             /**< Current value */
    int32_t end;                 /**< End value*/
    int32_t time;                /**< Animation time in ms*/
    int32_t act_time;            /**< Current time in animation. Set to negative to make delay.*/
    uint32_t playback_delay;     /**< Wait before play back*/
    uint32_t playback_time;      /**< Duration of playback animation*/
    uint32_t repeat_delay;       /**< Wait before repeat*/
    uint16_t repeat_cnt;         /**< Repeat count for the animation*/
    uint8_t early_apply  : 1;    /**< 1: Apply start value immediately even is there is `delay` */

    /*Animation system use these - user shouldn't set*/
    uint8_t playback_now : 1; /**< Play back is in progress*/
    uint8_t run_round : 1;    /**< Indicates the animation has run in this round*/
    uint32_t time_orig;
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
 * @param a pointer to an `lv_anim_t` variable to initialize
 */
void lv_anim_init(lv_anim_t * a);

/**
 * Set a variable to animate
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param var pointer to a variable to animate
 */
static inline void lv_anim_set_var(lv_anim_t * a, void * var)
{
    a->var = var;
}

/**
 * Set a function to animate `var`
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param exec_cb a function to execute during animation
 *                LittelvGL's built-in functions can be used.
 *                E.g. lv_obj_set_x
 */
static inline void lv_anim_set_exec_cb(lv_anim_t * a, lv_anim_exec_xcb_t exec_cb)
{
    a->exec_cb = exec_cb;
}

/**
 * Set the duration of an animation
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param duration duration of the animation in milliseconds
 */
static inline void lv_anim_set_time(lv_anim_t * a, uint32_t duration)
{
    a->time = duration;
}

/**
 * Set a delay before starting the animation
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param delay delay before the animation in milliseconds
 */
static inline void lv_anim_set_delay(lv_anim_t * a, uint32_t delay)
{
    a->act_time = -(int32_t)(delay);
}

/**
 * Set the start and end values of an animation
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param start the start value
 * @param end the end value
 */
static inline void lv_anim_set_values(lv_anim_t * a, lv_anim_value_t start, lv_anim_value_t end)
{
    a->start = start;
    a->current = start;
    a->end = end;
}

/**
 * Similar to `lv_anim_set_exec_cb` but `lv_anim_custom_exec_cb_t` receives
 * `lv_anim_t * ` as its first parameter instead of `void *`.
 * This function might be used when LVGL is binded to other languages because
 * it's more consistent to have `lv_anim_t *` as first parameter.
 * The variable to animate can be stored in the animation's `user_sata`
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param exec_cb a function to execute.
 */
static inline void lv_anim_set_custom_exec_cb(lv_anim_t * a, lv_anim_custom_exec_cb_t exec_cb)
{
    a->var     = a;
    a->exec_cb = (lv_anim_exec_xcb_t)exec_cb;
}

/**
 * Set the path (curve) of the animation.
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param path a function the get the current value of the animation.
 *             The built in functions starts with `lv_anim_path_...`
 */
static inline void lv_anim_set_path(lv_anim_t * a, const lv_anim_path_t * path)
{
    _lv_memcpy_small(&a->path, path, sizeof(lv_anim_path_t));
}

/**
 * Set a function call when the animation really starts (considering `delay`)
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param start_cb a function call when the animation starts
 */
static inline void lv_anim_set_start_cb(lv_anim_t * a, lv_anim_ready_cb_t start_cb)
{
    a->start_cb = start_cb;
}

/**
 * Set a function call when the animation is ready
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param ready_cb a function call when the animation is ready
 */
static inline void lv_anim_set_ready_cb(lv_anim_t * a, lv_anim_ready_cb_t ready_cb)
{
    a->ready_cb = ready_cb;
}

/**
 * Make the animation to play back to when the forward direction is ready
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param time the duration of the playback animation in in milliseconds. 0: disable playback
 */
static inline void lv_anim_set_playback_time(lv_anim_t * a, uint32_t time)
{
    a->playback_time = time;
}

/**
 * Make the animation to play back to when the forward direction is ready
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param delay delay in milliseconds before starting the playback animation.
 */
static inline void lv_anim_set_playback_delay(lv_anim_t * a, uint32_t delay)
{
    a->playback_delay = delay;
}

/**
 * Make the animation repeat itself.
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param cnt repeat count or `LV_ANIM_REPEAT_INFINITE` for infinite repetition. 0: to disable repetition.
 */
static inline void lv_anim_set_repeat_count(lv_anim_t * a, uint16_t cnt)
{
    a->repeat_cnt = cnt;
}

/**
 * Set a delay before repeating the animation.
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param delay delay in milliseconds before repeating the animation.
 */
static inline void lv_anim_set_repeat_delay(lv_anim_t * a, uint32_t delay)
{
    a->repeat_delay = delay;
}

/**
 * Create an animation
 * @param a an initialized 'anim_t' variable. Not required after call.
 */
void lv_anim_start(lv_anim_t * a);

/**
 * Initialize an animation path
 * @param path pointer to path
 */
static inline void lv_anim_path_init(lv_anim_path_t * path)
{
    _lv_memset_00(path, sizeof(lv_anim_path_t));
}

/**
 * Set a callback for a path
 * @param path pointer to an initialized path
 * @param cb the callback
 */
static inline void lv_anim_path_set_cb(lv_anim_path_t * path, lv_anim_path_cb_t cb)
{
    path->cb = cb;
}

/**
 * Set a user data for a path
 * @param path pointer to an initialized path
 * @param user_data pointer to the user data
 */
static inline void lv_anim_path_set_user_data(lv_anim_path_t * path, void * user_data)
{
    path->user_data = user_data;
}

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
 * Delete an animation of a variable with a given animator function
 * @param var pointer to variable
 * @param exec_cb a function pointer which is animating 'var',
 *           or NULL to ignore it and delete all the animations of 'var
 * @return true: at least 1 animation is deleted, false: no animation is deleted
 */
bool lv_anim_del(void * var, lv_anim_exec_xcb_t exec_cb);

/**
 * Get the animation of a variable and its `exec_cb`.
 * @param var pointer to variable
 * @param exec_cb a function pointer which is animating 'var',
 *           or NULL to delete all the animations of 'var'
 * @return pointer to the animation.
 */
lv_anim_t * lv_anim_get(void * var, lv_anim_exec_xcb_t exec_cb);

/**
 * Delete an animation by getting the animated variable from `a`.
 * Only animations with `exec_cb` will be deleted.
 * This function exists because it's logical that all anim. functions receives an
 * `lv_anim_t` as their first parameter. It's not practical in C but might make
 * the API more consequent and makes easier to generate bindings.
 * @param a pointer to an animation.
 * @param exec_cb a function pointer which is animating 'var',
 *           or NULL to ignore it and delete all the animations of 'var
 * @return true: at least 1 animation is deleted, false: no animation is deleted
 */
static inline bool lv_anim_custom_del(lv_anim_t * a, lv_anim_custom_exec_cb_t exec_cb)
{
    return lv_anim_del(a->var, (lv_anim_exec_xcb_t)exec_cb);
}

/**
 * Get the number of currently running animations
 * @return the number of running animations
 */
uint16_t lv_anim_count_running(void);

/**
 * Calculate the time of an animation with a given speed and the start and end values
 * @param speed speed of animation in unit/sec
 * @param start start value of the animation
 * @param end end value of the animation
 * @return the required time [ms] for the animation with the given parameters
 */
uint32_t lv_anim_speed_to_time(uint32_t speed, lv_anim_value_t start, lv_anim_value_t end);

/**
 * Manually refresh the state of the animations.
 * Useful to make the animations running in a blocking process where
 * `lv_task_handler` can't run for a while.
 * Shouldn't be used directly because it is called in `lv_refr_now()`.
 */
void lv_anim_refr_now(void);

/**
 * Calculate the current value of an animation applying linear characteristic
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_linear(const lv_anim_path_t * path, const lv_anim_t * a);

/**
 * Calculate the current value of an animation slowing down the start phase
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_ease_in(const lv_anim_path_t * path, const lv_anim_t * a);

/**
 * Calculate the current value of an animation slowing down the end phase
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_ease_out(const lv_anim_path_t * path, const lv_anim_t * a);

/**
 * Calculate the current value of an animation applying an "S" characteristic (cosine)
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_ease_in_out(const lv_anim_path_t * path, const lv_anim_t * a);

/**
 * Calculate the current value of an animation with overshoot at the end
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_overshoot(const lv_anim_path_t * path, const lv_anim_t * a);

/**
 * Calculate the current value of an animation with 3 bounces
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_bounce(const lv_anim_path_t * path, const lv_anim_t * a);

/**
 * Calculate the current value of an animation applying step characteristic.
 * (Set end value on the end of the animation)
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_step(const lv_anim_path_t * path, const lv_anim_t * a);

/**********************
 *   GLOBAL VARIABLES
 **********************/
extern const lv_anim_path_t lv_anim_path_def;

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ANIMATION == 0*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_ANIM_H*/
