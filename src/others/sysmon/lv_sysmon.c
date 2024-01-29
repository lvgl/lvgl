/**
 * @file lv_sysmon.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_sysmon.h"

#if LV_USE_SYSMON

#include "../../core/lv_global.h"
#include "../../misc/lv_async.h"
#include "../../stdlib/lv_string.h"
#include "../../widgets/label/lv_label.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_sysmon_class)

#define SYSMON_REFR_PERIOD_DEF 300 /* ms */

#if defined(LV_USE_PERF_MONITOR) && LV_USE_PERF_MONITOR
    #define sysmon_perf LV_GLOBAL_DEFAULT()->sysmon_perf
    #define _USE_PERF_MONITOR   1
#else
    #define _USE_PERF_MONITOR   0
#endif

#if defined(LV_USE_MEM_MONITOR) && LV_USE_MEM_MONITOR
    #define sysmon_mem LV_GLOBAL_DEFAULT()->sysmon_mem
    #define _USE_MEM_MONITOR   1
#else
    #define _USE_MEM_MONITOR   0
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if _USE_PERF_MONITOR
    static void perf_update_timer_cb(lv_timer_t * t);
    static void perf_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif

#if _USE_MEM_MONITOR
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

void _lv_sysmon_builtin_init(void)
{
#if _USE_PERF_MONITOR
    static lv_sysmon_perf_info_t perf_info;
    lv_subject_init_pointer(&sysmon_perf.subject, &perf_info);
    sysmon_perf.timer = lv_timer_create(perf_update_timer_cb, SYSMON_REFR_PERIOD_DEF, &perf_info);
#endif

#if _USE_MEM_MONITOR
    static lv_mem_monitor_t mem_info;
    lv_subject_init_pointer(&sysmon_mem.subject, &mem_info);
    sysmon_mem.timer = lv_timer_create(mem_update_timer_cb, SYSMON_REFR_PERIOD_DEF, &mem_info);
#endif
}

void _lv_sysmon_builtin_deinit(void)
{
#if _USE_PERF_MONITOR
    lv_timer_delete(sysmon_perf.timer);
#endif

#if _USE_MEM_MONITOR
    lv_timer_delete(sysmon_mem.timer);
#endif
}

lv_obj_t * lv_sysmon_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_style_bg_opa(label, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(label, lv_color_black(), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_pad_all(label, 3, 0);
    lv_label_set_text(label, "?");
    return label;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if _USE_PERF_MONITOR

static void perf_monitor_disp_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_sysmon_perf_info_t * info = (lv_sysmon_perf_info_t *)lv_subject_get_pointer(&sysmon_perf.subject);

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
            info->measured.render_start = lv_tick_get();
            break;
        case LV_EVENT_RENDER_READY:
            info->measured.render_elaps_sum += lv_tick_elaps(info->measured.render_start);
            info->measured.render_cnt++;
            break;
        case LV_EVENT_FLUSH_START:
            info->measured.flush_start = lv_tick_get();
            break;
        case LV_EVENT_FLUSH_FINISH:
            info->measured.flush_elaps_sum += lv_tick_elaps(info->measured.flush_start);
            info->measured.flush_cnt++;
            break;
        default:
            break;
    }
}

static void perf_update_timer_cb(lv_timer_t * t)
{
    /*Wait for a display*/
    if(!sysmon_perf.inited && lv_display_get_default()) {
        lv_display_add_event_cb(lv_display_get_default(), perf_monitor_disp_event_cb, LV_EVENT_ALL, NULL);

        lv_obj_t * obj1 = lv_sysmon_create(lv_layer_sys());
        lv_obj_align(obj1, LV_USE_PERF_MONITOR_POS, 0, 0);
        lv_subject_add_observer_obj(&sysmon_perf.subject, perf_observer_cb, obj1, NULL);
#if LV_USE_PERF_MONITOR_LOG_MODE
        lv_obj_add_flag(obj1, LV_OBJ_FLAG_HIDDEN);
#endif
        sysmon_perf.inited = true;
    }

    if(!sysmon_perf.inited) return;

    uint32_t LV_SYSMON_GET_IDLE(void);

    lv_sysmon_perf_info_t * info = lv_timer_get_user_data(t);
    info->calculated.run_cnt++;

    info->calculated.fps = info->measured.refr_interval_sum ? (1000 * info->measured.refr_cnt /
                                                               info->measured.refr_interval_sum) : 0;
    info->calculated.cpu = 100 - LV_SYSMON_GET_IDLE();
    info->calculated.refr_avg_time = info->measured.refr_cnt ? (info->measured.refr_elaps_sum / info->measured.refr_cnt) :
                                     0;
    info->calculated.flush_avg_time = info->measured.flush_cnt ? (info->measured.flush_elaps_sum / info->measured.flush_cnt)
                                      : 0;
    /*Flush time was measured in rendering time so subtract it*/
    info->calculated.render_avg_time = info->measured.render_cnt ? ((info->measured.render_elaps_sum -
                                                                     info->measured.flush_elaps_sum) /
                                                                    info->measured.render_cnt) : 0;

    info->calculated.cpu_avg_total = ((info->calculated.cpu_avg_total * (info->calculated.run_cnt - 1)) +
                                      info->calculated.cpu) / info->calculated.run_cnt;
    info->calculated.fps_avg_total = ((info->calculated.fps_avg_total * (info->calculated.run_cnt - 1)) +
                                      info->calculated.fps) / info->calculated.run_cnt;

    lv_subject_set_pointer(&sysmon_perf.subject, info);

    lv_sysmon_perf_info_t prev_info = *info;
    lv_memzero(info, sizeof(lv_sysmon_perf_info_t));
    info->measured.refr_start = prev_info.measured.refr_start;
    info->calculated.cpu_avg_total = prev_info.calculated.cpu_avg_total;
    info->calculated.fps_avg_total = prev_info.calculated.fps_avg_total;
    info->calculated.run_cnt = prev_info.calculated.run_cnt;
}

static void perf_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * label = lv_observer_get_target(observer);
    const lv_sysmon_perf_info_t * perf = lv_subject_get_pointer(subject);

#if LV_USE_PERF_MONITOR_LOG_MODE
    LV_UNUSED(label);
    LV_LOG("sysmon: "
           "%" LV_PRIu32 " FPS (refr_cnt: %" LV_PRIu32 " | redraw_cnt: %" LV_PRIu32 " | flush_cnt: %" LV_PRIu32 "), "
           "refr %" LV_PRIu32 "ms (render %" LV_PRIu32 "ms | flush %" LV_PRIu32 "ms), "
           "CPU %" LV_PRIu32 "%%\n",
           perf->calculated.fps, perf->measured.refr_cnt, perf->measured.render_cnt, perf->measured.flush_cnt,
           perf->calculated.refr_avg_time, perf->calculated.render_real_avg_time, perf->calculated.flush_avg_time,
           perf->calculated.cpu);
#else
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

#if _USE_MEM_MONITOR

static void mem_update_timer_cb(lv_timer_t * t)
{
    /*Wait for a display*/
    if(!sysmon_mem.inited && lv_display_get_default()) {
        lv_obj_t * obj2 = lv_sysmon_create(lv_layer_sys());
        lv_obj_align(obj2, LV_USE_MEM_MONITOR_POS, 0, 0);
        lv_subject_add_observer_obj(&sysmon_mem.subject, mem_observer_cb, obj2, NULL);
        sysmon_mem.inited = true;
    }

    if(!sysmon_mem.inited) return;

    lv_mem_monitor_t * mem_mon = lv_timer_get_user_data(t);
    lv_mem_monitor(mem_mon);
    lv_subject_set_pointer(&sysmon_mem.subject, mem_mon);
}

static void mem_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * label = lv_observer_get_target(observer);
    const lv_mem_monitor_t * mon = lv_subject_get_pointer(subject);

    uint32_t used_size = mon->total_size - mon->free_size;;
    uint32_t used_kb = used_size / 1024;
    uint32_t used_kb_tenth = (used_size - (used_kb * 1024)) / 102;
    lv_label_set_text_fmt(label,
                          "%"LV_PRIu32 ".%"LV_PRIu32 " kB, %d%%\n"
                          "%d%% frag.",
                          used_kb, used_kb_tenth, mon->used_pct,
                          mon->frag_pct);
}

#endif

#endif /*LV_USE_SYSMON*/
