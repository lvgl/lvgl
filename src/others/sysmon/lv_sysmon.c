/**
 * @file lv_sysmon.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_sysmon_private.h"
#include "../../misc/lv_timer_private.h"

#if LV_USE_SYSMON

#include "../../core/lv_global.h"
#include "../../misc/lv_async.h"
#include "../../stdlib/lv_string.h"
#include "../../widgets/label/lv_label.h"
#include "../../display/lv_display_private.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_SYSMON_REFR_PERIOD_DEF
    #define LV_SYSMON_REFR_PERIOD_DEF 300 /* ms */
#endif

#if LV_USE_MEM_MONITOR
    #define sysmon_mem LV_GLOBAL_DEFAULT()->sysmon_mem
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if LV_USE_PERF_MONITOR
    static void perf_update_timer_cb(lv_timer_t * t);
    static void perf_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
    static void perf_monitor_disp_event_cb(lv_event_t * e);
#endif

#if LV_USE_MEM_MONITOR
    static void mem_update_timer_cb(lv_timer_t * t);
    static void mem_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
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

void lv_sysmon_builtin_init(void)
{

#if LV_USE_MEM_MONITOR
    static lv_mem_monitor_t mem_info;
    lv_subject_init_pointer(&sysmon_mem.subject, &mem_info);
    sysmon_mem.timer = lv_timer_create(mem_update_timer_cb, LV_SYSMON_REFR_PERIOD_DEF, &mem_info);
#endif
}

void lv_sysmon_builtin_deinit(void)
{
#if LV_USE_MEM_MONITOR
    lv_timer_delete(sysmon_mem.timer);
#endif
}

lv_obj_t * lv_sysmon_create(lv_display_t * disp)
{
    LV_LOG_INFO("begin");
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) {
        LV_LOG_WARN("There is no default display");
        return NULL;
    }

    lv_obj_t * label = lv_label_create(lv_display_get_layer_sys(disp));
    lv_obj_set_style_bg_opa(label, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(label, lv_color_black(), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_pad_all(label, 3, 0);
    lv_label_set_text(label, "?");
    return label;
}

#if LV_USE_PERF_MONITOR

void lv_sysmon_show_performance(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) {
        LV_LOG_WARN("There is no default display");
        return;
    }

    disp->perf_label = lv_sysmon_create(disp);
    if(disp->perf_label == NULL) {
        LV_LOG_WARN("Couldn't create sysmon");
        return;
    }

    lv_subject_init_pointer(&disp->perf_sysmon_backend.subject, &disp->perf_sysmon_info);
    lv_obj_align(disp->perf_label, LV_USE_PERF_MONITOR_POS, 0, 0);
    lv_subject_add_observer_obj(&disp->perf_sysmon_backend.subject, perf_observer_cb, disp->perf_label, NULL);
    disp->perf_sysmon_backend.timer = lv_timer_create(perf_update_timer_cb, LV_SYSMON_REFR_PERIOD_DEF, disp);
    lv_display_add_event_cb(disp, perf_monitor_disp_event_cb, LV_EVENT_ALL, NULL);

#if LV_USE_PERF_MONITOR_LOG_MODE
    lv_obj_add_flag(disp->perf_label, LV_OBJ_FLAG_HIDDEN);
#else
    lv_obj_remove_flag(disp->perf_label, LV_OBJ_FLAG_HIDDEN);
#endif
}

void lv_sysmon_hide_performance(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) {
        LV_LOG_WARN("There is no default display");
        return;
    }

    lv_obj_add_flag(disp->perf_label, LV_OBJ_FLAG_HIDDEN);
}

#endif

#if LV_USE_MEM_MONITOR

void lv_sysmon_show_memory(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) {
        LV_LOG_WARN("There is no default display");
        return;
    }

    disp->mem_label = lv_sysmon_create(disp);
    if(disp->mem_label == NULL) {
        LV_LOG_WARN("Couldn't create sysmon");
        return;
    }

    lv_obj_align(disp->mem_label, LV_USE_MEM_MONITOR_POS, 0, 0);
    lv_subject_add_observer_obj(&sysmon_mem.subject, mem_observer_cb, disp->mem_label, NULL);

    lv_obj_remove_flag(disp->mem_label, LV_OBJ_FLAG_HIDDEN);
}

void lv_sysmon_hide_memory(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) {
        LV_LOG_WARN("There is no default display");
        return;
    }

    lv_obj_add_flag(disp->mem_label, LV_OBJ_FLAG_HIDDEN);
}

#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_USE_PERF_MONITOR

static void perf_monitor_disp_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_sysmon_perf_info_t * info = &disp->perf_sysmon_info;

    switch(code) {
        case LV_EVENT_REFR_START:
            info->measured.refr_interval_sum += lv_tick_elaps(info->measured.refr_start);
            info->measured.refr_start = lv_tick_get();
            break;
        case LV_EVENT_REFR_READY:
            info->measured.refr_elaps_sum += lv_tick_elaps(info->measured.refr_start);
            info->measured.refr_cnt++;
            break;
        case LV_EVENT_RENDER_START:
            info->measured.render_in_progress = 1;
            info->measured.render_start = lv_tick_get();
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
        case LV_EVENT_DELETE:
            lv_timer_delete(disp->perf_sysmon_backend.timer);
            lv_subject_deinit(&disp->perf_sysmon_backend.subject);
            break;
        default:
            break;
    }
}

static void perf_update_timer_cb(lv_timer_t * t)
{
    lv_display_t * disp = lv_timer_get_user_data(t);

    uint32_t LV_SYSMON_GET_IDLE(void);

    lv_sysmon_perf_info_t * info = &disp->perf_sysmon_info;
    info->calculated.run_cnt++;

    uint32_t time_since_last_report = lv_tick_elaps(info->measured.last_report_timestamp);
    lv_timer_t * disp_refr_timer = lv_display_get_refr_timer(NULL);
    uint32_t disp_refr_period = disp_refr_timer->period;

    info->calculated.fps = info->measured.refr_interval_sum ? (1000 * info->measured.refr_cnt / time_since_last_report) : 0;
    info->calculated.fps = LV_MIN(info->calculated.fps,
                                  1000 / disp_refr_period);   /*Limit due to possible off-by-one error*/

    info->calculated.cpu = 100 - LV_SYSMON_GET_IDLE();
    info->calculated.refr_avg_time = info->measured.refr_cnt ? (info->measured.refr_elaps_sum / info->measured.refr_cnt) :
                                     0;

    info->calculated.flush_avg_time = info->measured.render_cnt ?
                                      ((info->measured.flush_in_render_elaps_sum + info->measured.flush_not_in_render_elaps_sum)
                                       / info->measured.render_cnt) : 0;
    /*Flush time was measured in rendering time so subtract it*/
    info->calculated.render_avg_time = info->measured.render_cnt ? ((info->measured.render_elaps_sum -
                                                                     info->measured.flush_in_render_elaps_sum) /
                                                                    info->measured.render_cnt) : 0;

    info->calculated.cpu_avg_total = ((info->calculated.cpu_avg_total * (info->calculated.run_cnt - 1)) +
                                      info->calculated.cpu) / info->calculated.run_cnt;
    info->calculated.fps_avg_total = ((info->calculated.fps_avg_total * (info->calculated.run_cnt - 1)) +
                                      info->calculated.fps) / info->calculated.run_cnt;

    lv_subject_set_pointer(&disp->perf_sysmon_backend.subject, info);

    lv_sysmon_perf_info_t prev_info = *info;
    lv_memzero(info, sizeof(lv_sysmon_perf_info_t));
    info->measured.refr_start = prev_info.measured.refr_start;
    info->calculated.cpu_avg_total = prev_info.calculated.cpu_avg_total;
    info->calculated.fps_avg_total = prev_info.calculated.fps_avg_total;
    info->calculated.run_cnt = prev_info.calculated.run_cnt;

    info->measured.last_report_timestamp = lv_tick_get();
}

static void perf_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    const lv_sysmon_perf_info_t * perf = lv_subject_get_pointer(subject);

#if LV_USE_PERF_MONITOR_LOG_MODE
    LV_UNUSED(observer);
    LV_LOG("sysmon: "
           "%" LV_PRIu32 " FPS (refr_cnt: %" LV_PRIu32 " | redraw_cnt: %" LV_PRIu32"), "
           "refr %" LV_PRIu32 "ms (render %" LV_PRIu32 "ms | flush %" LV_PRIu32 "ms), "
           "CPU %" LV_PRIu32 "%%\n",
           perf->calculated.fps, perf->measured.refr_cnt, perf->measured.render_cnt,
           perf->calculated.refr_avg_time, perf->calculated.render_avg_time, perf->calculated.flush_avg_time,
           perf->calculated.cpu);
#else
    lv_obj_t * label = lv_observer_get_target(observer);
    lv_label_set_text_fmt(
        label,
        "%" LV_PRIu32" FPS, %" LV_PRIu32 "%% CPU\n"
        "%" LV_PRIu32" ms (%" LV_PRIu32" | %" LV_PRIu32")",
        perf->calculated.fps, perf->calculated.cpu,
        perf->calculated.render_avg_time + perf->calculated.flush_avg_time,
        perf->calculated.render_avg_time, perf->calculated.flush_avg_time
    );
#endif /*LV_USE_PERF_MONITOR_LOG_MODE*/
}

#endif

#if LV_USE_MEM_MONITOR

static void mem_update_timer_cb(lv_timer_t * t)
{
    lv_mem_monitor_t * mem_mon = lv_timer_get_user_data(t);
    lv_mem_monitor(mem_mon);
    lv_subject_set_pointer(&sysmon_mem.subject, mem_mon);
}

static void mem_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * label = lv_observer_get_target(observer);
    const lv_mem_monitor_t * mon = lv_subject_get_pointer(subject);

    size_t used_size = mon->total_size - mon->free_size;;
    size_t used_kb = used_size / 1024;
    size_t used_kb_tenth = (used_size - (used_kb * 1024)) / 102;
    size_t max_used_kb = mon->max_used / 1024;
    size_t max_used_kb_tenth = (mon->max_used - (max_used_kb * 1024)) / 102;
    lv_label_set_text_fmt(label,
                          "%zu.%zu kB (%d%%)\n"
                          "%zu.%zu kB max, %d%% frag.",
                          used_kb, used_kb_tenth, mon->used_pct,
                          max_used_kb, max_used_kb_tenth,
                          mon->frag_pct);
}

#endif

#endif /*LV_USE_SYSMON*/
