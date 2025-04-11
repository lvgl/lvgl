/**
 * @file lv_sysmon_perf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_sysmon_private.h"

#if LV_USE_PERF_MONITOR

#include "../../misc/lv_circle_buf.h"
#include "../../misc/lv_types.h"
#include "../../display/lv_display_private.h"
#include "../../misc/lv_timer_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_sysmon_perf_t {
    lv_display_t * disp;
    lv_sysmon_perf_data_t data;
    lv_sysmon_perf_info_t * current_scroll;
    const char * tag;
    bool running;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_sysmon_perf_display_cb(lv_event_t * e);
static void lv_sysmon_perf_update_global(lv_display_t * disp, lv_event_code_t code);
static void * lv_sysmon_perf_circle_buf_append(lv_circle_buf_t * circle_buf, void * data);
static void lv_sysmon_perf_init_info(lv_sysmon_perf_info_t * info, lv_display_t * disp);
static void lv_sysmon_perf_update_info(lv_sysmon_perf_info_t * info, lv_display_t * disp, lv_event_code_t code);
static void lv_sysmon_perf_calculate_info(lv_sysmon_perf_info_t * info, lv_display_t * disp, bool overall);
static void lv_sysmon_perf_update_scrolls(lv_sysmon_perf_t * perf, lv_event_code_t code);
static void lv_sysmon_perf_update_events(lv_sysmon_perf_t * perf, lv_event_code_t code);
#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
    static uint16_t lv_sysmon_perf_gen_tid(const char * tag);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_sysmon_perf_t * lv_sysmon_perf_create(lv_display_t * disp, const char * tag, size_t max_events, size_t max_scrolls)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) {
        LV_LOG_WARN("There is no default display");
        return NULL;
    }

    if(!disp->perf_sysmon_backend.inited) {
        lv_ll_init(&disp->perf_sysmon_backend.instances_ll, sizeof(lv_sysmon_perf_t));
        lv_display_add_event_cb(disp, lv_sysmon_perf_display_cb, LV_EVENT_ALL, NULL);
        disp->perf_sysmon_backend.inited = true;
    }

    lv_sysmon_perf_t * perf = lv_ll_ins_tail(&disp->perf_sysmon_backend.instances_ll);
    if(!perf) {
        return NULL;
    }

    lv_memzero(perf, sizeof(lv_sysmon_perf_t));
    perf->disp = disp;
    perf->tag = tag;
    if(!perf->tag) {
        perf->tag = "default_perf";
    }
    if(max_events > 0) {
        perf->data.events = lv_circle_buf_create(max_events, sizeof(lv_sysmon_event_data_t));
    }
    if(max_scrolls > 0) {
        perf->data.scrolls = lv_circle_buf_create(max_scrolls, sizeof(lv_sysmon_perf_info_t));
    }

    return perf;
}

void lv_sysmon_perf_destroy(lv_sysmon_perf_t * perf)
{
    if(!perf) {
        return;
    }

    if(perf->data.events) {
        lv_circle_buf_destroy(perf->data.events);
    }
    if(perf->data.scrolls) {
        lv_circle_buf_destroy(perf->data.scrolls);
    }
    lv_ll_remove(&perf->disp->perf_sysmon_backend.instances_ll, perf);
    lv_free(perf);
}

lv_result_t lv_sysmon_perf_start(lv_sysmon_perf_t * perf)
{
    if(!perf || perf->running) {
        return LV_RESULT_INVALID;
    }

    lv_sysmon_perf_reset_data(perf, LV_SYSMON_PERF_TYPE_ALL);
    perf->running = true;

    return LV_RESULT_OK;
}

void lv_sysmon_perf_reset_data(lv_sysmon_perf_t * perf, lv_sysmon_perf_type_t types)
{
    if(!perf) {
        return;
    }

    if(types & LV_SYSMON_PERF_TYPE_OVERALL) {
        lv_sysmon_perf_init_info(&perf->data.overall, perf->disp);
    }

    if(perf->data.events && (types & LV_SYSMON_PERF_TYPE_EVENTS)) {
        lv_circle_buf_reset(perf->data.events);
    }
    if(perf->data.scrolls && (types & LV_SYSMON_PERF_TYPE_SCROLLS)) {
        lv_circle_buf_reset(perf->data.scrolls);
        if(perf->disp->perf_sysmon_backend.scrolling) {
            perf->current_scroll = lv_sysmon_perf_circle_buf_append(perf->data.scrolls, &perf->data.overall);
        }
        else {
            perf->current_scroll = NULL;
        }
    }
}

const lv_sysmon_perf_data_t * lv_sysmon_perf_get_data(lv_sysmon_perf_t * perf)
{
    if(!perf) {
        return NULL;
    }

    if(perf->running) {
        lv_sysmon_perf_calculate_info(&perf->data.overall, perf->disp, true);
        if(perf->current_scroll) {
            lv_sysmon_perf_calculate_info(perf->current_scroll, perf->disp, false);
        }
    }

    return &perf->data;
}

const lv_sysmon_perf_data_t * lv_sysmon_perf_stop(lv_sysmon_perf_t * perf)
{
    if(!perf || !perf->running) {
        return NULL;
    }

    const lv_sysmon_perf_data_t * data = lv_sysmon_perf_get_data(perf);
    perf->running = false;
    return data;
}

void lv_sysmon_perf_generate_trace(lv_sysmon_perf_t * perf)
{
#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
    if(!perf || !perf->data.events) {
        return;
    }

    uint16_t tid = lv_sysmon_perf_gen_tid(perf->tag);
    lv_sysmon_event_data_t event;

    while(lv_circle_buf_read(perf->data.events, &event) == LV_RESULT_OK) {
        switch(event.type) {
            case LV_SYSMON_EVENT_TYPE_REFR_BEGIN:
                lv_profiler_builtin_write_custom("lv_perf_refresh", perf->tag, 'B', event.timestamp, tid, 0);
                break;
            case LV_SYSMON_EVENT_TYPE_REFR_END:
                lv_profiler_builtin_write_custom("lv_perf_refresh", perf->tag, 'E', event.timestamp, tid, 0);
                break;
            case LV_SYSMON_EVENT_TYPE_RENDER_BEGIN:
                lv_profiler_builtin_write_custom("lv_perf_render", perf->tag, 'B', event.timestamp, tid, 0);
                break;
            case LV_SYSMON_EVENT_TYPE_RENDER_END:
                lv_profiler_builtin_write_custom("lv_perf_render", perf->tag, 'E', event.timestamp, tid, 0);
                break;
            case LV_SYSMON_EVENT_TYPE_SCROLL_BEGIN:
                lv_profiler_builtin_write_custom("lv_perf_scroll", perf->tag, 'B', event.timestamp, tid, 0);
                break;
            case LV_SYSMON_EVENT_TYPE_SCROLL_END:
                lv_profiler_builtin_write_custom("lv_perf_scroll", perf->tag, 'E', event.timestamp, tid, 0);
                break;
            default:
                break;
        }
    }
#else
    LV_UNUSED(perf);
#endif
}

void lv_sysmon_perf_event(lv_display_t * disp, lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_sysmon_perf_t * perf;

    if(disp == NULL) {
        disp = lv_display_get_default();
    }

    if(disp == NULL) {
        return;
    }

    lv_sysmon_perf_update_global(disp, code);

    LV_LL_READ(&disp->perf_sysmon_backend.instances_ll, perf) {
        if(perf->running) {
            lv_sysmon_perf_update_info(&perf->data.overall, disp, code);
            lv_sysmon_perf_update_scrolls(perf, code);
            lv_sysmon_perf_update_events(perf, code);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_sysmon_perf_display_cb(lv_event_t * e)
{
    lv_sysmon_perf_event(lv_event_get_current_target(e), e);
}

static void lv_sysmon_perf_update_global(lv_display_t * disp, lv_event_code_t code)
{
    switch(code) {
        case LV_EVENT_REFR_START:
            disp->perf_sysmon_backend.last_refr_start = lv_tick_get();
            break;
        case LV_EVENT_REFR_READY:
            disp->perf_sysmon_backend.last_refr_start = 0;
            break;
        case LV_EVENT_RENDER_START:
            disp->perf_sysmon_backend.last_render_start = lv_tick_get();
            break;
        case LV_EVENT_RENDER_READY:
            disp->perf_sysmon_backend.last_render_start = 0;
            break;
        case LV_EVENT_SCROLL_BEGIN:
            disp->perf_sysmon_backend.scrolling = true;
            break;
        case LV_EVENT_SCROLL_END:
            disp->perf_sysmon_backend.scrolling = false;
            break;
        default:
            break;
    }
}

static void * lv_sysmon_perf_circle_buf_append(lv_circle_buf_t * circle_buf, void * data)
{
    if(lv_circle_buf_is_full(circle_buf)) {
        lv_circle_buf_skip(circle_buf);
    }
    LV_ASSERT(lv_circle_buf_write(circle_buf, data) == LV_RESULT_OK);
    return lv_circle_buf_latest(circle_buf);
}

static void lv_sysmon_perf_init_info(lv_sysmon_perf_info_t * info, lv_display_t * disp)
{
    lv_memzero(info, offsetof(lv_sysmon_perf_info_t, calculated.cpu_avg_total));
    info->measured.perf_start = lv_tick_get();
    info->measured.refr_start = disp->perf_sysmon_backend.last_refr_start;
}

static void lv_sysmon_perf_update_info(lv_sysmon_perf_info_t * info, lv_display_t * disp, lv_event_code_t code)
{
    switch(code) {
        case LV_EVENT_REFR_START:
            info->measured.refr_interval_sum += lv_tick_elaps(info->measured.refr_start);
            info->measured.refr_start = disp->perf_sysmon_backend.last_refr_start;
            break;
        case LV_EVENT_REFR_READY:
            info->measured.refr_elaps_sum += lv_tick_elaps(info->measured.refr_start);
            info->measured.refr_cnt++;
            break;
        case LV_EVENT_RENDER_START:
            info->measured.render_in_progress = 1;
            info->measured.render_start = disp->perf_sysmon_backend.last_render_start;
            break;
        case LV_EVENT_RENDER_READY:
            info->measured.render_in_progress = 0;
            info->measured.render_elaps_sum += lv_tick_elaps(info->measured.render_start);
            info->measured.render_cnt++;
            break;
        case LV_EVENT_FLUSH_START:
        case LV_EVENT_FLUSH_WAIT_START:
            if(info->measured.render_in_progress) {
                info->measured.flush_in_render_start = lv_tick_get();
            }
            else {
                info->measured.flush_not_in_render_start = lv_tick_get();
            }
            break;
        case LV_EVENT_FLUSH_FINISH:
        case LV_EVENT_FLUSH_WAIT_FINISH:
            if(info->measured.render_in_progress) {
                info->measured.flush_in_render_elaps_sum += lv_tick_elaps(info->measured.flush_in_render_start);
            }
            else {
                info->measured.flush_not_in_render_elaps_sum += lv_tick_elaps(info->measured.flush_not_in_render_start);
            }
            break;
        default:
            break;
    }
}

static void lv_sysmon_perf_calculate_info(lv_sysmon_perf_info_t * info, lv_display_t * disp, bool overall)
{
    uint32_t LV_SYSMON_GET_IDLE(void);

    lv_timer_t * disp_refr_timer = lv_display_get_refr_timer(disp);
    lv_value_precise_t disp_refr_period = disp_refr_timer ? disp_refr_timer->period : LV_DEF_REFR_PERIOD;

    info->calculated.duration = lv_tick_elaps(info->measured.perf_start);
    info->calculated.fps = info->calculated.duration ? ((lv_value_precise_t)1000 * info->measured.refr_cnt /
                                                        info->calculated.duration) : 0;
    info->calculated.fps = LV_MIN(info->calculated.fps,
                                  (lv_value_precise_t)1000 / disp_refr_period);   /*Limit due to possible off-by-one error*/

    info->calculated.cpu = 100 - LV_SYSMON_GET_IDLE();
#if LV_SYSMON_PROC_IDLE_AVAILABLE
    uint32_t LV_SYSMON_GET_PROC_IDLE(void);
    info->calculated.cpu_proc = 100 - LV_SYSMON_GET_PROC_IDLE();
#endif /*LV_SYSMON_PROC_IDLE_AVAILABLE*/
    info->calculated.refr_avg_time = info->measured.refr_cnt ? ((lv_value_precise_t)info->measured.refr_elaps_sum /
                                                                info->measured.refr_cnt) : 0;

    info->calculated.flush_avg_time = info->measured.render_cnt ?
                                      ((lv_value_precise_t)(info->measured.flush_in_render_elaps_sum + info->measured.flush_not_in_render_elaps_sum)
                                       / info->measured.render_cnt) : 0;
    /*Flush time was measured in rendering time so subtract it*/
    info->calculated.render_avg_time = info->measured.render_cnt ? ((lv_value_precise_t)(info->measured.render_elaps_sum -
                                                                                         info->measured.flush_in_render_elaps_sum) /
                                                                    info->measured.render_cnt) : 0;

    if(!overall) {
        return;
    }

    info->calculated.run_cnt++;
    info->calculated.cpu_avg_total = ((info->calculated.cpu_avg_total * (info->calculated.run_cnt - 1)) +
                                      info->calculated.cpu) / info->calculated.run_cnt;
    info->calculated.fps_avg_total = ((info->calculated.fps_avg_total * (info->calculated.run_cnt - 1)) +
                                      info->calculated.fps) / info->calculated.run_cnt;
}

static void lv_sysmon_perf_update_scrolls(lv_sysmon_perf_t * perf, lv_event_code_t code)
{
    if(!perf->data.scrolls) {
        return;
    }

    switch(code) {
        case LV_EVENT_SCROLL_BEGIN:
            if(!perf->current_scroll) {
                perf->current_scroll = lv_sysmon_perf_circle_buf_append(perf->data.scrolls, NULL);
                LV_ASSERT_NULL(perf->current_scroll);
                lv_sysmon_perf_init_info(perf->current_scroll, perf->disp);
            }
            break;
        case LV_EVENT_SCROLL_END:
            if(perf->current_scroll) {
                lv_sysmon_perf_calculate_info(perf->current_scroll, perf->disp, false);
                perf->current_scroll = NULL;
            }
            break;
        default:
            if(perf->current_scroll) {
                lv_sysmon_perf_update_info(perf->current_scroll, perf->disp, code);
            }
            break;
    }
}

static uint64_t lv_sysmon_perf_get_timestamp(void)
{
#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
    return lv_profiler_builtin_tick_get();
#else
    return lv_tick_get();
#endif
}

static void lv_sysmon_perf_update_events(lv_sysmon_perf_t * perf, lv_event_code_t code)
{
    if(!perf->data.events) {
        return;
    }

    lv_sysmon_event_data_t event = {LV_SYSMON_EVENT_TYPE_INVALID};

    switch(code) {
        case LV_EVENT_REFR_START:
            event.type = LV_SYSMON_EVENT_TYPE_REFR_BEGIN;
            break;
        case LV_EVENT_REFR_READY:
            event.type = LV_SYSMON_EVENT_TYPE_REFR_END;
            break;
        case LV_EVENT_RENDER_START:
            event.type = LV_SYSMON_EVENT_TYPE_RENDER_BEGIN;
            break;
        case LV_EVENT_RENDER_READY:
            event.type = LV_SYSMON_EVENT_TYPE_RENDER_END;
            break;
        case LV_EVENT_SCROLL_BEGIN:
            event.type = LV_SYSMON_EVENT_TYPE_SCROLL_BEGIN;
            break;
        case LV_EVENT_SCROLL_END:
            event.type = LV_SYSMON_EVENT_TYPE_SCROLL_END;
            break;
        default:
            break;
    }

    if(event.type != LV_SYSMON_EVENT_TYPE_INVALID) {
        event.timestamp = lv_sysmon_perf_get_timestamp();
        lv_sysmon_perf_circle_buf_append(perf->data.events, &event);
    }
}

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
static uint16_t lv_sysmon_perf_gen_tid(const char * tag)
{
    /* Simple hash function to generate a constant tid from a tag, to make the trace more readable */
    uint16_t tid = 0;
    while(*tag) {
        tid = (tid << 5) + tid + *tag; /* 33 * tid + *tag, a simple but effective hash */
        tid ^= (tid >> 3);             /* Add some bit mixing */
        tag++;
    }
    return tid;
}
#endif

#endif /*LV_USE_PERF_MONITOR*/
