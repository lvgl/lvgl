/**
 * @file lv_anim_timeline.h
 *
 */

#ifndef LV_ANIM_TIMELINE_H
#define LV_ANIM_TIMELINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim.h"

/*********************
 *      DEFINES
 *********************/

#define LV_ANIM_TIMELINE_PROGRESS_MAX 0xFFFF

/**********************
 *      TYPEDEFS
 **********************/

/*Data of anim_timeline_dsc*/
typedef struct _lv_anim_timeline_dsc_t {
    lv_anim_t anim;
    uint32_t start_time;
    uint8_t is_started : 1;
    uint8_t is_completed : 1;
} lv_anim_timeline_dsc_t;


/**********************
* GLOBAL PROTOTYPES
**********************/

/**
 * Create an animation timeline.
 * @return pointer to the animation timeline.
 */
lv_anim_timeline_t * lv_anim_timeline_create(void);

/**
 * Delete animation timeline.
 * @param at    pointer to the animation timeline.
 */
void lv_anim_timeline_delete(lv_anim_timeline_t * at);

/**
 * Add animation to the animation timeline.
 * @param at            pointer to the animation timeline.
 * @param start_time    the time the animation started on the timeline, note that start_time will override the value of delay.
 * @param a             pointer to an animation.
 */
void lv_anim_timeline_add(lv_anim_timeline_t * at, uint32_t start_time, const lv_anim_t * a);

/**
 * Start the animation timeline.
 * @param at    pointer to the animation timeline.
 * @return      total time spent in animation timeline.
 */
uint32_t lv_anim_timeline_start(lv_anim_timeline_t * at);

/**
 * Pause the animation timeline.
 * @param at    pointer to the animation timeline.
 */
void lv_anim_timeline_pause(lv_anim_timeline_t * at);

/**
 * Set the playback direction of the animation timeline.
 * @param at        pointer to the animation timeline.
 * @param reverse   whether to play in reverse.
 */
void lv_anim_timeline_set_reverse(lv_anim_timeline_t * at, bool reverse);

/**
 * Set the time to wait before starting the animation.
 * Applies only when playing from the very start, or reverse from the very end.
 * @param at        pointer to an animation timeline
 * @param delay     the delay time in milliseconds
 */
void lv_anim_timeline_set_delay(lv_anim_timeline_t * at, uint32_t delay);

/**
 * Make the animation timeline repeat itself.
 * @param at        pointer to the animation timeline.
 * @param cnt       repeat count or `LV_ANIM_REPEAT_INFINITE` for infinite repetition. 0: to disable repetition.
 */
void lv_anim_timeline_set_repeat_count(lv_anim_timeline_t * at, uint32_t cnt);

/**
 * Set a delay before repeating the animation timeline.
 * @param at        pointer to the animation timeline.
 * @param delay     delay in milliseconds before repeating the animation timeline.
 */
void lv_anim_timeline_set_repeat_delay(lv_anim_timeline_t * at, uint32_t delay);

/**
 * Set the progress of the animation timeline.
 * @param at        pointer to the animation timeline.
 * @param progress  set value 0~65535 to map 0~100% animation progress.
 */
void lv_anim_timeline_set_progress(lv_anim_timeline_t * at, uint16_t progress);

/**
 * Set the user_data of a an animation timeline
 * @param at        pointer to the animation timeline.
 * @param user_data pointer to any data. Only the pointer will be saved.
 */
void lv_anim_timeline_set_user_data(lv_anim_timeline_t * at, void * user_data);

/**
 * Get the time used to play the animation timeline.
 * @param at        pointer to the animation timeline.
 * @return total    time spent in animation timeline.
 */
uint32_t lv_anim_timeline_get_playtime(lv_anim_timeline_t * at);

/**
 * Get whether the animation timeline is played in reverse.
 * @param at        pointer to the animation timeline.
 * @return return   true if it is reverse playback.
 */
bool lv_anim_timeline_get_reverse(lv_anim_timeline_t * at);

/**
 * Get the wait time when  playing from the very start, or reverse from the very end.
 * @param at    pointer to an animation timeline
 * @return      the remaining time in milliseconds
 */
uint32_t lv_anim_timeline_get_delay(lv_anim_timeline_t * at);

/**
 * Get the progress of the animation timeline.
 * @param at    pointer to the animation timeline.
 * @return      return value 0~65535 to map 0~100% animation progress.
 */
uint16_t lv_anim_timeline_get_progress(lv_anim_timeline_t * at);

/**
 * Get repeat count of the animation timeline.
 * @param at    pointer to the animation timeline.
 */
uint32_t lv_anim_timeline_get_repeat_count(lv_anim_timeline_t * at);

/**
 * Get repeat delay of the animation timeline.
 * @param at    pointer to the animation timeline.
 */
uint32_t lv_anim_timeline_get_repeat_delay(lv_anim_timeline_t * at);

/**
 * Get the user_data of a an animation timeline
 * @param at    pointer to the animation timeline.
 */
void * lv_anim_timeline_get_user_data(lv_anim_timeline_t * at);

/**
 * Merge (add) all animations of a timeline to another
 * @param dest      merge animation into this timeline
 * @param src       merge the animations of this timeline
 * @param delay     add the animations with this extra delay
 */
void lv_anim_timeline_merge(lv_anim_timeline_t * dest, const lv_anim_timeline_t * src, int32_t delay);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ANIM_TIMELINE_H*/
