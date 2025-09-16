/**
 * @file lv_anim_timeline_private.h
 *
 */

#ifndef LV_ANIM_TIMELINE_PRIVATE_H
#define LV_ANIM_TIMELINE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim_timeline.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_anim_timeline_dsc_t;

/*Data of anim_timeline*/
struct _lv_anim_timeline_t {
    /** Dynamically allocated anim dsc array*/
    struct _lv_anim_timeline_dsc_t * anim_dsc;

    /** The length of anim dsc array*/
    uint32_t anim_dsc_cnt;

    /** Current time of the animation*/
    uint32_t act_time;

    /** Reverse playback*/
    bool reverse;

    /** Delay before starting the animation from any ends*/
    uint32_t delay;

    /** Repeat count*/
    uint32_t repeat_count;

    /** Wait before repeat*/
    uint32_t repeat_delay;

    /** For any custom data*/
    void * user_data;
};

/**********************
* GLOBAL PROTOTYPES
**********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ANIM_TIMELINE_PRIVATE_H*/
