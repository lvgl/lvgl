/**
 * @file lv_anim_timeline.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim_timeline.h"
#if LV_USE_ANIM_TIMELINE

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
    uint32_t playtime = lv_anim_timeline_get_playtime(anim_timeline);
    uint32_t i = 0;
    while(anim_timeline[i].obj != NULL) {
        lv_anim_timeline_add(&(anim_timeline[i]), playback, playtime);
        i++;
    }
    return playtime;
}

void lv_anim_timeline_set_progress(const lv_anim_timeline_t * anim_timeline, uint16_t progress)
{
    uint32_t playtime = lv_anim_timeline_get_playtime(anim_timeline);
    uint32_t time = progress * playtime / 0xFFFF;

    uint32_t i = 0;
    while(anim_timeline[i].obj != NULL) {
        const lv_anim_timeline_t * at = &anim_timeline[i];
        int32_t value;
        if(time < at->start_time) {
            value = at->start;
        }
        else if(time < (at->start_time + at->duration)) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_values(&a, at->start, at->end);
            lv_anim_set_time(&a, at->duration);

            a.act_time = time - at->start_time;

            value = at->path_cb(&a);
        }
        else {
            value = at->end;
        }
        at->exec_cb(at->obj, value);
        i++;
    }
}

uint32_t lv_anim_timeline_get_playtime(const lv_anim_timeline_t * anim_timeline)
{
    uint32_t playtime = 0;
    uint32_t i = 0;
    while(anim_timeline[i].obj != NULL) {
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
    while(anim_timeline[i].obj != NULL) {
        lv_obj_t * obj = anim_timeline[i].obj;
        lv_anim_exec_xcb_t exec_xcb = anim_timeline[i].exec_cb;
        lv_anim_del(obj, exec_xcb);
        i++;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_anim_timeline_add(const lv_anim_timeline_t * anim_timeline, bool playback, uint32_t playtime)
{
    lv_obj_t * obj = anim_timeline->obj;
    uint32_t delay = anim_timeline->start_time;

    lv_anim_exec_xcb_t exec_cb = anim_timeline->exec_cb;
    uint16_t time = anim_timeline->duration;

    int32_t start = anim_timeline->start;
    int32_t end = anim_timeline->end;

    if(playback) {
        int32_t temp = start;
        start = end;
        end = temp;

        delay = playtime - (delay + time);
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_early_apply(&a, anim_timeline->early_apply);
    lv_anim_set_var(&a, obj);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_time(&a, time);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_path_cb(&a, anim_timeline->path_cb);

#if LV_USE_USER_DATA
    a.user_data = anim_timeline->user_data;
#endif

    lv_anim_start(&a);
}

#endif /*LV_USE_ANIM_TIMELINE*/
