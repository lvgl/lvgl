/**
 * @file anim.h
 *
 */

#ifndef ANIM_H
#define ANIM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Can be used to indicate if animations are enabled or disabled in a case*/
enum {
    LV_ANIM_OFF,
    LV_ANIM_ON,
};

typedef uint8_t lv_anim_enable_t;

/*Type of the animated value*/
typedef lv_coord_t lv_anim_value_t;

#if LV_USE_ANIMATION

struct _lv_anim_t;

/* Generic prototype of "animator" functions.
 * First parameter is the variable to animate.
 * Second parameter is the value to set.
 * Compatible with `lv_xxx_set_yyy(obj, value)` functions
 * The `x` in `_xcb_t` means its not a fully generic prototype because
 * it doesn't receive `lv_anim_t *` as its first argument*/
typedef void (*lv_anim_exec_xcb_t)(void *, lv_anim_value_t);

/* Same as `lv_anim_exec_cb_t` but receives `lv_anim_t *` as the first parameter.
 * It's more consistent but less convenient. Might be used by binding generator functions.*/
typedef void (*lv_anim_custom_exec_cb_t)(struct _lv_anim_t *, lv_anim_value_t);

/*Get the current value during an animation*/
typedef lv_anim_value_t (*lv_anim_path_cb_t)(const struct _lv_anim_t *);

/*Callback to call when the animation is ready*/
typedef void (*lv_anim_ready_cb_t)(struct _lv_anim_t *);

/*Describe an animation*/
typedef struct _lv_anim_t
{
    void * var;                  /*Variable to animate*/
    lv_anim_exec_xcb_t exec_cb;  /*Function to execute to animate*/
    lv_anim_path_cb_t path_cb;   /*An array with the steps of animations*/
    lv_anim_ready_cb_t ready_cb; /*Call it when the animation is ready*/
    int32_t start;               /*Start value*/
    int32_t end;                 /*End value*/
    uint16_t time;               /*Animation time in ms*/
    int16_t act_time;            /*Current time in animation. Set to negative to make delay.*/
    uint16_t playback_pause;     /*Wait before play back*/
    uint16_t repeat_pause;       /*Wait before repeat*/
#if LV_USE_USER_DATA
    lv_anim_user_data_t user_data; /*Custom user data*/
#endif

    uint8_t playback : 1; /*When the animation is ready play it back*/
    uint8_t repeat : 1;   /*Repeat the animation infinitely*/
    /*Animation system use these - user shouldn't set*/
    uint8_t playback_now : 1; /*Play back is in progress*/
    uint32_t has_run : 1;     /*Indicates the animation has run in this round*/
} lv_anim_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init. the animation module
 */
void lv_anim_core_init(void);

/**
 * Initialize an animation variable.
 * E.g.:
 * lv_anim_t a;
 * lv_anim_init(&a);
 * lv_anim_set_...(&a);
 * lv_anim_create(&a);
 * @param a pointer to an `lv_anim_t` variable to initialize
 */
void lv_anim_init(lv_anim_t * a);

/**
 * Set a variable to animate function to execute on `var`
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param var pointer to a variable to animate
 * @param exec_cb a function to execute.
 *                LittelvGL's built-in functions can be used.
 *                E.g. lv_obj_set_x
 */
static inline void lv_anim_set_exec_cb(lv_anim_t * a, void * var, lv_anim_exec_xcb_t exec_cb)
{
    a->var     = var;
    a->exec_cb = exec_cb;
}

/**
 * Set the duration and delay of an animation
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param duration duration of the animation in milliseconds
 * @param delay delay before the animation in milliseconds
 */
static inline void lv_anim_set_time(lv_anim_t * a, uint16_t duration, uint16_t delay)
{
    a->time     = duration;
    a->act_time = -delay;
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
    a->end   = end;
}

/**
 * Similar to `lv_anim_set_var_and_cb` but `lv_anim_custom_exec_cb_t` receives
 * `lv_anim_t * ` as its first parameter instead of `void *`.
 * This function might be used when LittlevGL is binded to other languages because
 * it's more consistent to have `lv_anim_t *` as first parameter.
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
 * @param path_cb a function the get the current value of the animation.
 *                The built in functions starts with `lv_anim_path_...`
 */
static inline void lv_anim_set_path_cb(lv_anim_t * a, lv_anim_path_cb_t path_cb)
{
    a->path_cb = path_cb;
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
 * @param wait_time time in milliseconds to wait before starting the back direction
 */
static inline void lv_anim_set_playback(lv_anim_t * a, uint16_t wait_time)
{
    a->playback       = 1;
    a->playback_pause = wait_time;
}

/**
 * Disable playback. (Disabled after `lv_anim_init()`)
 * @param a pointer to an initialized `lv_anim_t` variable
 */
static inline void lv_anim_clear_playback(lv_anim_t * a)
{
    a->playback = 0;
}

/**
 * Make the animation to start again when ready.
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param wait_time time in milliseconds to wait before starting the animation again
 */
static inline void lv_anim_set_repeat(lv_anim_t * a, uint16_t wait_time)
{
    a->repeat       = 1;
    a->repeat_pause = wait_time;
}

/**
 * Disable repeat. (Disabled after `lv_anim_init()`)
 * @param a pointer to an initialized `lv_anim_t` variable
 */
static inline void lv_anim_clear_repeat(lv_anim_t * a)
{
    a->repeat = 0;
}

/**
 * Set a user specific data for the animation
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param user_data the user data
 */
static inline void lv_anim_set_user_data(lv_anim_t * a, lv_anim_user_data_t user_data)
{
    memcpy(&a->user_data, &user_data, sizeof(user_data));
}

/**
 * Get the user data
 * @param a pointer to an initialized `lv_anim_t` variable
 * @return the user data
 */
static inline lv_anim_user_data_t lv_anim_get_user_data(lv_anim_t * a)
{
    return a->user_data;
}

/**
 * Get pointer to the user data
 * @param a pointer to an initialized `lv_anim_t` variable
 * @return pointer to the user data
 */
static inline lv_anim_user_data_t * lv_anim_get_user_data_ptr(lv_anim_t * a)
{
    return &a->user_data;
}

/**
 * Create an animation
 * @param a an initialized 'anim_t' variable. Not required after call.
 */
void lv_anim_create(lv_anim_t * a);

/**
 * Delete an animation of a variable with a given animator function
 * @param var pointer to variable
 * @param exec_cb a function pointer which is animating 'var',
 *           or NULL to ignore it and delete all the animations of 'var
 * @return true: at least 1 animation is deleted, false: no animation is deleted
 */
bool lv_anim_del(void * var, lv_anim_exec_xcb_t exec_cb);

/**
 * Delete an aniamation by getting the animated variable from `a`.
 * Only animations with `exec_cb` will be deleted.
 * This function exist becasue it's logical that all anim functions receives an
 * `lv_anim_t` as their first parameter. It's not practical in C but might makes
 * the API more conequent and makes easier to genrate bindings.
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
uint16_t lv_anim_speed_to_time(uint16_t speed, lv_anim_value_t start, lv_anim_value_t end);

/**
 * Calculate the current value of an animation applying linear characteristic
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_linear(const lv_anim_t * a);

/**
 * Calculate the current value of an animation slowing down the start phase
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_ease_in(const lv_anim_t * a);

/**
 * Calculate the current value of an animation slowing down the end phase
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_ease_out(const lv_anim_t * a);

/**
 * Calculate the current value of an animation applying an "S" characteristic (cosine)
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_ease_in_out(const lv_anim_t * a);

/**
 * Calculate the current value of an animation with overshoot at the end
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_overshoot(const lv_anim_t * a);

/**
 * Calculate the current value of an animation with 3 bounces
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_bounce(const lv_anim_t * a);

/**
 * Calculate the current value of an animation applying step characteristic.
 * (Set end value on the end of the animation)
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t lv_anim_path_step(const lv_anim_t * a);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ANIMATION == 0*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_ANIM_H*/
