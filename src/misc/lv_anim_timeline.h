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

/**********************
 *      TYPEDEFS
 **********************/

/*Data of anim_timeline*/
typedef struct {
    int32_t start_time;         /**< Start time on the timeline*/
    void * var;                 /**< Variable to animate*/
    lv_anim_exec_xcb_t exec_cb; /**< Function to execute to animate*/
    int32_t start_value;        /**< Start value*/
    int32_t end_value;          /**< End value*/
    uint32_t duration;          /**< Animation time in ms*/
    lv_anim_path_cb_t path_cb;  /**< Describe the path (curve) of animations*/
    bool early_apply;           /**< Apply start value immediately even is there is `delay`*/
#if LV_USE_USER_DATA
    void * user_data;           /**< Custom user data*/
#endif
} lv_anim_timeline_t;

extern const lv_anim_timeline_t LV_ANIM_TIMELINE_END;

/**********************
* GLOBAL PROTOTYPES
**********************/

/**
 * Start animation according to the timeline
 * @param anim_timeline  pointer to timeline array
 * @param reverse        whether to play in reverse
 * @return total time spent in timeline
 */
uint32_t lv_anim_timeline_start(const lv_anim_timeline_t * anim_timeline, bool reverse);

/**
 * Set the progress of the timeline
 * @param anim_timeline  pointer to timeline array
 * @param progress       set value 0~65535 to map 0~100% animation progress
 */
void lv_anim_timeline_set_progress(const lv_anim_timeline_t * anim_timeline, uint16_t progress);

/**
 * Get the time used to play the timeline
 * @param anim_timeline  pointer to timeline array
 * @return total time spent in timeline
 */
uint32_t lv_anim_timeline_get_playtime(const lv_anim_timeline_t * anim_timeline);

/**
 * Delete timeline
 * @param anim_timeline  pointer to timeline array
 */
void lv_anim_timeline_del(const lv_anim_timeline_t * anim_timeline);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ANIM_TIMELINE_H*/
