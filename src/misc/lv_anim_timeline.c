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
static void lv_anim_timeline_add(const lv_anim_timeline_t * anim_timeline, bool reverse, uint32_t playtime);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_anim_timeline_t LV_ANIM_TIMELINE_END = {
    .start_time = -1
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint32_t lv_anim_timeline_start(const lv_anim_timeline_t anim_timeline[], bool reverse)
{
    const uint32_t playtime = lv_anim_timeline_get_playtime(anim_timeline);
    uint32_t i = 0;
    while(anim_timeline[i].start_time >= 0) {
        lv_anim_timeline_add(&(anim_timeline[i]), reverse, playtime);
        i++;
    }
    return playtime;
}

void lv_anim_timeline_set_progress(const lv_anim_timeline_t anim_timeline[], uint16_t progress)
{
    const uint32_t playtime = lv_anim_timeline_get_playtime(anim_timeline);
    const uint32_t act_time = progress * playtime / 0xFFFF;

    lv_anim_t a;
    lv_anim_init(&a);

    uint32_t i = 0;
    while(anim_timeline[i].start_time >= 0) {
        const lv_anim_timeline_t * at = &anim_timeline[i];
        int32_t value = 0;

        lv_anim_set_var(&a, at->var);

#if LV_USE_USER_DATA
        a.user_data = at->user_data;
#endif

#if LV_ANIM_TIMELINE_CUSTOM_EXEC
        lv_anim_set_custom_exec_cb(&a, at->exec_cb);
#else
        lv_anim_set_exec_cb(&a, at->exec_cb);
#endif

        if(act_time < (uint32_t)at->start_time) {
            value = at->start_value;
        }
        else if(act_time < (at->start_time + at->duration)) {
            lv_anim_set_values(&a, at->start_value, at->end_value);
            lv_anim_set_time(&a, at->duration);
            lv_anim_set_path_cb(&a, at->path_cb);

            a.act_time = act_time - at->start_time;

            value = a.path_cb(&a);
        }
        else {
            value = at->end_value;
        }

        a.exec_cb(a.var, value);
        i++;
    }
}

uint32_t lv_anim_timeline_get_playtime(const lv_anim_timeline_t anim_timeline[])
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

void lv_anim_timeline_del(const lv_anim_timeline_t anim_timeline[])
{
    uint32_t i = 0;
    while(anim_timeline[i].start_time >= 0) {
        void * var = anim_timeline[i].var;
        lv_anim_timeline_exec_cb_t exec_cb = anim_timeline[i].exec_cb;
#if LV_ANIM_TIMELINE_CUSTOM_EXEC
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, var);

#if LV_USE_USER_DATA
        a.user_data = anim_timeline[i].user_data;
#endif

        lv_anim_custom_del(&a, exec_cb);

#else
        lv_anim_del(var, exec_cb);
#endif
        i++;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_anim_timeline_add(const lv_anim_timeline_t * anim_timeline, bool reverse, uint32_t playtime)
{
    void * var = anim_timeline->var;
    int32_t delay = anim_timeline->start_time;

    const lv_anim_timeline_exec_cb_t exec_cb = anim_timeline->exec_cb;
    const uint32_t duration = anim_timeline->duration;

    int32_t start = anim_timeline->start_value;
    int32_t end = anim_timeline->end_value;

    if(reverse) {
        int32_t temp = start;
        start = end;
        end = temp;
        delay = playtime - (delay + duration);
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, var);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_time(&a, duration);
    lv_anim_set_path_cb(&a, anim_timeline->path_cb);
    lv_anim_set_early_apply(&a, anim_timeline->early_apply);

#if LV_ANIM_TIMELINE_CUSTOM_EXEC
    lv_anim_set_custom_exec_cb(&a, exec_cb);
#else
    lv_anim_set_exec_cb(&a, exec_cb);
#endif

#if LV_USE_USER_DATA
    a.user_data = anim_timeline->user_data;
#endif

    lv_anim_start(&a);
}
