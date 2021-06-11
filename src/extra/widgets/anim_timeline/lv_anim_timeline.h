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
#include "../../../lvgl.h"
#if LV_USE_ANIM_TIMELINE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of anim_timeline*/
typedef struct {
    uint32_t start_time;
    lv_obj_t * obj;
    lv_anim_exec_xcb_t exec_cb;
    int32_t start;
    int32_t end;
    uint16_t duration;
    lv_anim_path_cb_t path_cb;
    bool early_apply;
#if LV_USE_USER_DATA
    void * user_data; /**< Custom user data*/
#endif
} lv_anim_timeline_t;

/**********************
* GLOBAL PROTOTYPES
**********************/

/**
 * Start animation according to the timeline
 * @param anim_timeline  timeline array address
 * @param playback       whether to play in reverse
 * @return timeline total time spent
 */
uint32_t lv_anim_timeline_start(const lv_anim_timeline_t * anim_timeline, bool playback);

/**
 * Set the progress of the timeline
 * @param anim_timeline  timeline array address
 * @param progress       set value 0~65535 to map 0~100% animation progress
 */
void lv_anim_timeline_set_progress(const lv_anim_timeline_t * anim_timeline, uint16_t progress);

/**
 * Get the time used to play the timeline
 * @param anim_timeline  timeline array address
 * @return timeline playback takes time
 */
uint32_t lv_anim_timeline_get_playtime(const lv_anim_timeline_t * anim_timeline);

/**
 * Delete timeline
 * @param anim_timeline  timeline array address
 */
void lv_anim_timeline_del(const lv_anim_timeline_t * anim_timeline);

/**********************
 *      MACROS
 **********************/

#define LV_ANIM_TIMELINE_END {0, NULL}

#endif /*LV_USE_ANIM_TIMELINE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ANIM_TIMELINE_H*/
