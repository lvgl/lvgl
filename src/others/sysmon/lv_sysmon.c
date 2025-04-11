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

#if LV_USE_PERF_MONITOR && !LV_PERF_MONITOR_SERVICE_ONLY
    static void perf_update_timer_cb(lv_timer_t * t);
    static void perf_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
    static void perf_monitor_disp_event_cb(lv_event_t * e);
    static void perf_dump_info(lv_display_t * disp);
    static void perf_control(lv_display_t * disp, bool start);
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

#if LV_USE_PERF_MONITOR && !LV_PERF_MONITOR_SERVICE_ONLY

void lv_sysmon_show_performance(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) {
        LV_LOG_WARN("There is no default display");
        return;
    }

    if(disp->perf_label == NULL) {
        disp->perf_label = lv_sysmon_create(disp);
        if(disp->perf_label == NULL) {
            LV_LOG_WARN("Couldn't create sysmon");
            return;
        }

        disp->perf_sysmon_backend.instance = lv_sysmon_perf_create(disp, "lv_sysmon_builtin", 0, 0);
        if(disp->perf_sysmon_backend.instance == NULL) {
            LV_LOG_WARN("Couldn't create sysmon");
            return;
        }
        lv_sysmon_perf_start(disp->perf_sysmon_backend.instance);

        const lv_sysmon_perf_data_t * data = lv_sysmon_perf_get_data(disp->perf_sysmon_backend.instance);
        lv_subject_init_pointer(&disp->perf_sysmon_backend.subject, (void *)&data->overall);
        lv_obj_align(disp->perf_label, LV_USE_PERF_MONITOR_POS, 0, 0);
        lv_subject_add_observer_obj(&disp->perf_sysmon_backend.subject, perf_observer_cb, disp->perf_label, NULL);
        disp->perf_sysmon_backend.timer = lv_timer_create(perf_update_timer_cb, LV_SYSMON_REFR_PERIOD_DEF, disp);
        lv_display_add_event_cb(disp, perf_monitor_disp_event_cb, LV_EVENT_ALL, NULL);
    }

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

void lv_sysmon_performance_dump(lv_display_t * disp)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) {
        LV_LOG_WARN("There is no default display");
        return;
    }
    perf_dump_info(disp);
}

void lv_sysmon_performance_resume(lv_display_t * disp)
{
    perf_control(disp, true);
}

void lv_sysmon_performance_pause(lv_display_t * disp)
{
    perf_control(disp, false);
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

    if(disp->mem_label == NULL) {
        disp->mem_label = lv_sysmon_create(disp);
        if(disp->mem_label == NULL) {
            LV_LOG_WARN("Couldn't create sysmon");
            return;
        }

        lv_obj_align(disp->mem_label, LV_USE_MEM_MONITOR_POS, 0, 0);
        lv_subject_add_observer_obj(&sysmon_mem.subject, mem_observer_cb, disp->mem_label, NULL);
    }

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

#if LV_USE_PERF_MONITOR && !LV_PERF_MONITOR_SERVICE_ONLY

static void perf_monitor_disp_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    switch(code) {
        case LV_EVENT_DELETE:
            lv_timer_delete(disp->perf_sysmon_backend.timer);
            lv_subject_deinit(&disp->perf_sysmon_backend.subject);
            lv_sysmon_perf_destroy(disp->perf_sysmon_backend.instance);
            break;
        default:
            break;
    }
}

static void perf_dump_info(lv_display_t * disp)
{
    const lv_sysmon_perf_data_t * data = lv_sysmon_perf_get_data(disp->perf_sysmon_backend.instance);
    lv_subject_set_pointer(&disp->perf_sysmon_backend.subject, (void *)&data->overall);
    lv_sysmon_perf_reset_data(disp->perf_sysmon_backend.instance, LV_SYSMON_PERF_TYPE_OVERALL);
}

static void perf_update_timer_cb(lv_timer_t * t)
{
    lv_display_t * disp = lv_timer_get_user_data(t);

    perf_dump_info(disp);
}

static void perf_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    const lv_sysmon_perf_info_t * perf = lv_subject_get_pointer(subject);

#if LV_USE_PERF_MONITOR_LOG_MODE
    LV_UNUSED(observer);
    LV_LOG("sysmon: "
           "%" LV_PRFv32(".2f") " FPS (refr_cnt: %" LV_PRIu32 " | redraw_cnt: %" LV_PRIu32"), "
           "refr %" LV_PRFv32(".2f") "ms (render %" LV_PRFv32(".2f") "ms | flush %" LV_PRFv32(".2f") "ms), "
           "CPU %" LV_PRIu32 "%%\n",
           perf->calculated.fps, perf->measured.refr_cnt, perf->measured.render_cnt,
           perf->calculated.refr_avg_time, perf->calculated.render_avg_time, perf->calculated.flush_avg_time,
           perf->calculated.cpu);
#else
    lv_obj_t * label = lv_observer_get_target(observer);
#if LV_SYSMON_PROC_IDLE_AVAILABLE
    lv_label_set_text_fmt(
        label,
        "%" LV_PRIu32" FPS, %" LV_PRIu32 "%% CPU, %" LV_PRIu32 "%% Self\n"
        "%" LV_PRIu32" ms (%" LV_PRIu32" | %" LV_PRIu32")",
        perf->calculated.fps, perf->calculated.cpu, perf->calculated.cpu_proc,
        perf->calculated.render_avg_time + perf->calculated.flush_avg_time,
        perf->calculated.render_avg_time, perf->calculated.flush_avg_time
    );
#else
    lv_label_set_text_fmt(
        label,
        "%" LV_PRFv32(".2f")" FPS, %" LV_PRIu32 "%% CPU\n"
        "%" LV_PRFv32(".2f")" ms (%" LV_PRFv32(".2f")" | %" LV_PRFv32(".2f")")",
        perf->calculated.fps, perf->calculated.cpu,
        perf->calculated.render_avg_time + perf->calculated.flush_avg_time,
        perf->calculated.render_avg_time, perf->calculated.flush_avg_time
    );
#endif /*LV_SYSMON_PROC_IDLE_AVAILABLE*/
#endif /*LV_USE_PERF_MONITOR_LOG_MODE*/
}

static void perf_control(lv_display_t * disp, bool start)
{
    if(disp == NULL) disp = lv_display_get_default();
    if(disp == NULL) {
        LV_LOG_WARN("There is no default display");
        return;
    }

    if(disp->perf_sysmon_backend.timer == NULL) return;

    if(start) {
        lv_timer_resume(disp->perf_sysmon_backend.timer);
    }
    else {
        lv_timer_pause(disp->perf_sysmon_backend.timer);
    }
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
