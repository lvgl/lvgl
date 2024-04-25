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

typedef struct _lv_anim_timeline_t lv_anim_timeline_t;

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
 * @return total time spent in animation timeline.
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
 * Get the time used to play the animation timeline.
 * @param at    pointer to the animation timeline.
 * @return total time spent in animation timeline.
 */
uint32_t lv_anim_timeline_get_playtime(lv_anim_timeline_t * at);

/**
 * Get whether the animation timeline is played in reverse.
 * @param at    pointer to the animation timeline.
 * @return return true if it is reverse playback.
 */
bool lv_anim_timeline_get_reverse(lv_anim_timeline_t * at);

/**
 * Get the progress of the animation timeline.
 * @param at    pointer to the animation timeline.
 * @return return value 0~65535 to map 0~100% animation progress.
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

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ANIM_TIMELINE_H*/
