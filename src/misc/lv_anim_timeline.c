/**
 * @file lv_anim_timeline.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
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
} lv_anim_timeline_dsc_t;

/*Data of anim_timeline*/
struct _lv_anim_timeline_t {
    lv_anim_timeline_dsc_t * anim_dsc;  /**< Dynamically allocated anim dsc array*/
    uint32_t anim_dsc_cnt;              /**< The length of anim dsc array*/
    uint16_t progress;                  /**< The progress of the animation timeline, 0~65535 to map 0~100% animation progress*/
    bool reverse;                       /**< Reverse playback*/
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_timeline_exec_cb(void * var, int32_t v);

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
    uint32_t progress = at->progress;
    uint32_t remain = at->reverse ? progress : LV_ANIM_TIMELINE_PROGRESS_MAX - progress;
    uint32_t duration = remain * playtime / LV_ANIM_TIMELINE_PROGRESS_MAX;
    int32_t start = progress;
    int32_t end = at->reverse ? 0 : LV_ANIM_TIMELINE_PROGRESS_MAX;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, at);
    lv_anim_set_exec_cb(&a, anim_timeline_exec_cb);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_time(&a, duration);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
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

void lv_anim_timeline_set_progress(lv_anim_timeline_t * at, uint16_t progress)
{
    LV_ASSERT_NULL(at);

    uint32_t playtime = lv_anim_timeline_get_playtime(at);
    uint32_t act_time = progress * playtime / LV_ANIM_TIMELINE_PROGRESS_MAX;

    at->progress = progress;

    for(uint32_t i = 0; i < at->anim_dsc_cnt; i++) {
        lv_anim_t * a = &(at->anim_dsc[i].anim);

        if(a->exec_cb == NULL && a->custom_exec_cb == NULL) {
            continue;
        }

        uint32_t start_time = at->anim_dsc[i].start_time;
        int32_t value = 0;
        if(act_time < start_time && a->early_apply) {
            value = a->start_value;
            if(a->exec_cb) a->exec_cb(a->var, value);
            if(a->custom_exec_cb) a->custom_exec_cb(a, value);
        }
        else if(act_time >= start_time && act_time <= (start_time + a->duration)) {
            a->act_time = act_time - start_time;
            value = a->path_cb(a);
            if(a->exec_cb) a->exec_cb(a->var, value);
            if(a->custom_exec_cb) a->custom_exec_cb(a, value);
        }
        else if(act_time > start_time + a->duration) {
            value = a->end_value;
            if(a->exec_cb) a->exec_cb(a->var, value);
            if(a->custom_exec_cb) a->custom_exec_cb(a, value);
        }
    }
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

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void anim_timeline_exec_cb(void * var, int32_t v)
{
    lv_anim_timeline_t * at = var;
    lv_anim_timeline_set_progress(at, v);
}
