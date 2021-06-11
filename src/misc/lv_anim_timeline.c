/**
 * @file lv_anim_timeline.c
 *
 */

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

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_anim_timeline_add(const lv_anim_timeline_t * anim_timeline, bool playback, uint32_t playtime);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint32_t lv_anim_timeline_start(const lv_anim_timeline_t * anim_timeline, bool playback)
{
    const uint32_t playtime = lv_anim_timeline_get_playtime(anim_timeline);
    uint32_t i = 0;
    while(anim_timeline[i].start_time >= 0) {
        lv_anim_timeline_add(&(anim_timeline[i]), playback, playtime);
        i++;
    }
    return playtime;
}

void lv_anim_timeline_set_progress(const lv_anim_timeline_t * anim_timeline, uint16_t progress)
{
    const uint32_t playtime = lv_anim_timeline_get_playtime(anim_timeline);
    const uint32_t act_time = progress * playtime / 0xFFFF;

    uint32_t i = 0;
    while(anim_timeline[i].start_time >= 0) {
        const lv_anim_timeline_t * at = &anim_timeline[i];
        int32_t value;
        if(act_time < at->start_time) {
            value = at->start_value;
        }
        else if(act_time < (at->start_time + at->duration)) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_values(&a, at->start_value, at->end_value);
            lv_anim_set_time(&a, at->duration);

            a.act_time = act_time - at->start_time;

            value = at->path_cb(&a);
        }
        else {
            value = at->end_value;
        }
        at->exec_cb(at->var, value);
        i++;
    }
}

uint32_t lv_anim_timeline_get_playtime(const lv_anim_timeline_t * anim_timeline)
{
    uint32_t playtime = 0;
    uint32_t i = 0;
    while(anim_timeline[i].start_time >= 0) {
        uint32_t end = anim_timeline[i].start_time + anim_timeline[i].duration;
        if(end > playtime) {
            playtime = end;
        }
        i++;
    }
    return playtime;
}

void lv_anim_timeline_del(const lv_anim_timeline_t * anim_timeline)
{
    uint32_t i = 0;
    while(anim_timeline[i].start_time >= 0) {
        void * var = anim_timeline[i].var;
        lv_anim_exec_xcb_t exec_xcb = anim_timeline[i].exec_cb;
        lv_anim_del(var, exec_xcb);
        i++;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_anim_timeline_add(const lv_anim_timeline_t * anim_timeline, bool playback, uint32_t playtime)
{
    void * var = anim_timeline->var;
    int32_t delay = anim_timeline->start_time;

    const lv_anim_exec_xcb_t exec_cb = anim_timeline->exec_cb;
    const uint32_t duration = anim_timeline->duration;

    int32_t start = anim_timeline->start_value;
    int32_t end = anim_timeline->end_value;

    if(playback) {
        int32_t temp = start;
        start = end;
        end = temp;
        delay = playtime - (delay + duration);
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_early_apply(&a, anim_timeline->early_apply);
    lv_anim_set_var(&a, var);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_time(&a, duration);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_path_cb(&a, anim_timeline->path_cb);

#if LV_USE_USER_DATA
    a.user_data = anim_timeline->user_data;
#endif

    lv_anim_start(&a);
}
