/**
 * @file lv_anim_timeline.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim_private.h"
#include "lv_assert.h"
#include "lv_anim_timeline.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of anim_timeline_dsc*/
typedef struct {
    lv_anim_t anim;
    uint32_t start_time;
    uint8_t is_started : 1;
    uint8_t is_completed : 1;
} lv_anim_timeline_dsc_t;

/*Data of anim_timeline*/
struct _lv_anim_timeline_t {
    lv_anim_timeline_dsc_t * anim_dsc;  /**< Dynamically allocated anim dsc array*/
    uint32_t anim_dsc_cnt;              /**< The length of anim dsc array*/
    uint32_t act_time;                  /**< Current time of the animation*/
    bool reverse;                       /**< Reverse playback*/
    uint32_t repeat_count;              /**< Repeat count*/
    uint32_t repeat_delay;              /**< Wait before repeat*/
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_timeline_exec_cb(void * var, int32_t v);
static void anim_timeline_set_act_time(lv_anim_timeline_t * at, uint32_t act_time);
static int32_t anim_timeline_path_cb(const lv_anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_anim_timeline_t * lv_anim_timeline_create(void)
{
    lv_anim_timeline_t * at = lv_malloc_zeroed(sizeof(lv_anim_timeline_t));
    LV_ASSERT_MALLOC(at);
    return at;
}

void lv_anim_timeline_delete(lv_anim_timeline_t * at)
{
    LV_ASSERT_NULL(at);

    lv_anim_timeline_pause(at);

    lv_free(at->anim_dsc);
    lv_free(at);
}

void lv_anim_timeline_add(lv_anim_timeline_t * at, uint32_t start_time, const lv_anim_t * a)
{
    LV_ASSERT_NULL(at);

    at->anim_dsc_cnt++;
    at->anim_dsc = lv_realloc(at->anim_dsc, at->anim_dsc_cnt * sizeof(lv_anim_timeline_dsc_t));

    LV_ASSERT_MALLOC(at->anim_dsc);

    at->anim_dsc[at->anim_dsc_cnt - 1].anim = *a;
    at->anim_dsc[at->anim_dsc_cnt - 1].start_time = start_time;
}

uint32_t lv_anim_timeline_start(lv_anim_timeline_t * at)
{
    LV_ASSERT_NULL(at);

    uint32_t playtime = lv_anim_timeline_get_playtime(at);
    uint32_t repeat = at->repeat_count;
    uint32_t delay = at->repeat_delay;
    uint32_t start = at->act_time;
    uint32_t end = at->reverse ? 0 : playtime;
    uint32_t duration = end > start ? end - start : start - end;

    if((!at->reverse && at->act_time == 0) || (at->reverse && at->act_time == playtime)) {
        for(uint32_t i = 0; i < at->anim_dsc_cnt; i++) {
            at->anim_dsc[i].is_started   = 0;
            at->anim_dsc[i].is_completed = 0;
        }
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, at);
    lv_anim_set_exec_cb(&a, anim_timeline_exec_cb);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_duration(&a, duration);
    lv_anim_set_path_cb(&a, anim_timeline_path_cb);
    lv_anim_set_repeat_count(&a, repeat);
    lv_anim_set_repeat_delay(&a, delay);
    lv_anim_start(&a);
    return playtime;
}

void lv_anim_timeline_pause(lv_anim_timeline_t * at)
{
    LV_ASSERT_NULL(at);

    lv_anim_delete(at, anim_timeline_exec_cb);
}

void lv_anim_timeline_set_reverse(lv_anim_timeline_t * at, bool reverse)
{
    LV_ASSERT_NULL(at);
    at->reverse = reverse;
}

void lv_anim_timeline_set_repeat_count(lv_anim_timeline_t * at, uint32_t cnt)
{
    LV_ASSERT_NULL(at);
    at->repeat_count = cnt;
}

void lv_anim_timeline_set_repeat_delay(lv_anim_timeline_t * at, uint32_t delay)
{
    LV_ASSERT_NULL(at);
    at->repeat_delay = delay;
}

void lv_anim_timeline_set_progress(lv_anim_timeline_t * at, uint16_t progress)
{
    LV_ASSERT_NULL(at);

    uint32_t playtime = lv_anim_timeline_get_playtime(at);
    uint32_t act_time = lv_map(progress, 0, LV_ANIM_TIMELINE_PROGRESS_MAX, 0, playtime);
    anim_timeline_set_act_time(at, act_time);
}

uint32_t lv_anim_timeline_get_playtime(lv_anim_timeline_t * at)
{
    LV_ASSERT_NULL(at);

    uint32_t playtime = 0;
    for(uint32_t i = 0; i < at->anim_dsc_cnt; i++) {
        uint32_t end = lv_anim_get_playtime(&at->anim_dsc[i].anim);
        if(end == LV_ANIM_PLAYTIME_INFINITE)
            return end;
        end += at->anim_dsc[i].start_time;
        if(end > playtime) {
            playtime = end;
        }
    }

    return playtime;
}

bool lv_anim_timeline_get_reverse(lv_anim_timeline_t * at)
{
    LV_ASSERT_NULL(at);
    return at->reverse;
}

uint16_t lv_anim_timeline_get_progress(lv_anim_timeline_t * at)
{
    LV_ASSERT_NULL(at);
    uint32_t playtime = lv_anim_timeline_get_playtime(at);
    return lv_map(at->act_time, 0, playtime, 0, LV_ANIM_TIMELINE_PROGRESS_MAX);
}

uint32_t lv_anim_timeline_get_repeat_count(lv_anim_timeline_t * at)
{
    LV_ASSERT_NULL(at);
    return  at->repeat_count;
}

uint32_t lv_anim_timeline_get_repeat_delay(lv_anim_timeline_t * at)
{
    LV_ASSERT_NULL(at);
    return  at->repeat_delay;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void anim_timeline_set_act_time(lv_anim_timeline_t * at, uint32_t act_time)
{
    at->act_time = act_time;
    bool anim_timeline_is_started = (lv_anim_get(at, anim_timeline_exec_cb) != NULL);
    for(uint32_t i = 0; i < at->anim_dsc_cnt; i++) {
        lv_anim_timeline_dsc_t * anim_dsc = &(at->anim_dsc[i]);
        lv_anim_t * a = &(anim_dsc->anim);

        uint32_t start_time = anim_dsc->start_time;
        int32_t value = 0;

        if(act_time < start_time && a->early_apply) {
            if(anim_timeline_is_started) {
                if(at->reverse) {
                    if(!anim_dsc->is_started && a->start_cb)  a->start_cb(a);
                    anim_dsc->is_started = 1;
                }
                else {
                    anim_dsc->is_started = 0;
                }
            }

            value = a->start_value;
            if(a->exec_cb) a->exec_cb(a->var, value);
            if(a->custom_exec_cb) a->custom_exec_cb(a, value);

            if(anim_timeline_is_started) {
                if(at->reverse) {
                    if(!anim_dsc->is_completed && a->completed_cb) a->completed_cb(a);
                    anim_dsc->is_completed = 1;
                }
                else {
                    anim_dsc->is_completed = 0;
                }
            }
        }
        else if(act_time >= start_time && act_time <= (start_time + a->duration)) {
            if(anim_timeline_is_started) {
                if(!anim_dsc->is_started && a->start_cb) a->start_cb(a);
                anim_dsc->is_started = 1;
            }

            a->act_time = act_time - start_time;
            value = a->path_cb(a);
            if(a->exec_cb) a->exec_cb(a->var, value);
            if(a->custom_exec_cb) a->custom_exec_cb(a, value);

            if(anim_timeline_is_started) {
                if(at->reverse) {
                    if(act_time == start_time) {
                        if(!anim_dsc->is_completed && a->completed_cb) a->completed_cb(a);
                        anim_dsc->is_completed = 1;
                    }
                    else {
                        anim_dsc->is_completed = 0;
                    }
                }
                else {
                    if(act_time == (start_time + a->duration)) {
                        if(!anim_dsc->is_completed && a->completed_cb) a->completed_cb(a);
                        anim_dsc->is_completed = 1;
                    }
                    else {
                        anim_dsc->is_completed = 0;
                    }
                }
            }
        }
        else if(act_time > start_time + a->duration) {
            if(anim_timeline_is_started) {
                if(at->reverse) {
                    anim_dsc->is_started = 0;
                }
                else {
                    if(!anim_dsc->is_started && a->start_cb) a->start_cb(a);
                    anim_dsc->is_started = 1;
                }
            }

            value = a->end_value;
            if(a->exec_cb) a->exec_cb(a->var, value);
            if(a->custom_exec_cb) a->custom_exec_cb(a, value);

            if(anim_timeline_is_started) {
                if(at->reverse) {
                    anim_dsc->is_completed = 0;
                }
                else {
                    if(!anim_dsc->is_completed && a->completed_cb) a->completed_cb(a);
                    anim_dsc->is_completed = 1;
                }
            }
        }
    }
}

static int32_t anim_timeline_path_cb(const lv_anim_t * a)
{
    /* Directly map original timestamps to avoid loss of accuracy */
    return lv_map(a->act_time, 0, a->duration, a->start_value, a->end_value);
}

static void anim_timeline_exec_cb(void * var, int32_t v)
{
    lv_anim_timeline_t * at = var;
    anim_timeline_set_act_time(at, v);
}
