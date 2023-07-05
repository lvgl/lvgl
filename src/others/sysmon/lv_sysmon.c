/**
 * @file lv_sysmon.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_sysmon.h"

#if LV_USE_SYSMON

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_sysmon_class

#define SYSMON_REFR_PERIOD_DEF 300 /* ms */

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint32_t    refr_start;
    uint32_t    refr_interval_sum;
    uint32_t    refr_elaps_sum;
    uint32_t    refr_cnt;
    uint32_t    render_start;
    uint32_t    render_elaps_sum;
    uint32_t    render_cnt;
    uint32_t    flush_start;
    uint32_t    flush_elaps_sum;
    uint32_t    flush_cnt;
} perf_info_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_sysmon_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_sysmon_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_sysmon_timer_cb(lv_timer_t * timer);
static void sysmon_async_cb(void * user_data);

#if LV_USE_PERF_MONITOR
    static void perf_monitor_init(void);
#endif

#if LV_USE_MEM_MONITOR && LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    static void mem_monitor_init(void);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_sysmon_class = {
    .base_class = &lv_label_class,
    .constructor_cb = lv_sysmon_constructor,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .event_cb = lv_sysmon_event,
    .instance_size = sizeof(lv_sysmon_t),
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_sysmon_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_sysmon_set_refr_period(lv_obj_t * obj, uint32_t period)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_sysmon_t * sysmon = (lv_sysmon_t *)obj;
    lv_timer_set_period(sysmon->timer, period);
}

void _lv_sysmon_builtin_init(void)
{
    lv_async_call(sysmon_async_cb, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_sysmon_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_sysmon_t * sysmon = (lv_sysmon_t *)obj;
    sysmon->timer = lv_timer_create(lv_sysmon_timer_cb, SYSMON_REFR_PERIOD_DEF, obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_style_text_color(obj, lv_color_white(), 0);
    lv_obj_set_style_pad_all(obj, 3, 0);
    lv_label_set_text(obj, "?");
}

static void lv_sysmon_timer_cb(lv_timer_t * timer)
{
    lv_obj_t * obj = lv_timer_get_user_data(timer);
    lv_obj_send_event(obj, LV_EVENT_REFRESH, NULL);
}

static void lv_sysmon_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    lv_obj_event_base(MY_CLASS, e);
}

#if LV_USE_PERF_MONITOR

static void perf_monitor_disp_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * sysmon = lv_event_get_user_data(e);
    perf_info_t * info = lv_obj_get_user_data(sysmon);
    switch(code) {
        case LV_EVENT_REFR_START:
            info->refr_interval_sum += lv_tick_elaps(info->refr_start);
            info->refr_start = lv_tick_get();
            break;
        case LV_EVENT_REFR_FINISH:
            info->refr_elaps_sum += lv_tick_elaps(info->refr_start);
            info->refr_cnt++;
            break;
        case LV_EVENT_RENDER_START:
            info->render_start = lv_tick_get();
            break;
        case LV_EVENT_RENDER_READY:
            info->render_elaps_sum += lv_tick_elaps(info->render_start);
            info->render_cnt++;
            break;
        case LV_EVENT_FLUSH_START:
            info->flush_start = lv_tick_get();
            break;
        case LV_EVENT_FLUSH_FINISH:
            info->flush_elaps_sum += lv_tick_elaps(info->flush_start);
            info->flush_cnt++;
            break;
        default:
            break;
    }
}

static void perf_monitor_event_cb(lv_event_t * e)
{
    lv_obj_t * sysmon = lv_event_get_current_target_obj(e);
    perf_info_t * info = lv_obj_get_user_data(sysmon);

    uint32_t fps = info->refr_interval_sum ? (1000 * info->refr_cnt / info->refr_interval_sum) : 0;
    uint32_t cpu = 100 - lv_timer_get_idle();
    uint32_t refr_avg_time = info->refr_cnt ? (info->refr_elaps_sum / info->refr_cnt) : 0;
    uint32_t render_avg_time = info->render_cnt ? (info->render_elaps_sum / info->render_cnt) : 0;
    uint32_t flush_avg_time = info->flush_cnt ? (info->flush_elaps_sum / info->flush_cnt) : 0;
    uint32_t render_real_avg_time = render_avg_time - flush_avg_time;

#if LV_USE_PERF_MONITOR_LOG_MODE
    /*Avoid warning*/
    LV_UNUSED(fps);
    LV_UNUSED(cpu);
    LV_UNUSED(refr_avg_time);
    LV_UNUSED(render_real_avg_time);
    LV_UNUSED(flush_avg_time);

    LV_LOG("sysmon: "
           "%" LV_PRIu32 " FPS (refr_cnt: %" LV_PRIu32 " | redraw_cnt: %" LV_PRIu32 " | flush_cnt: %" LV_PRIu32 "), "
           "refr %" LV_PRIu32 "ms (render %" LV_PRIu32 "ms | flush %" LV_PRIu32 "ms), "
           "CPU %" LV_PRIu32 "%%\n",
           fps, info->refr_cnt, info->render_cnt, info->flush_cnt,
           refr_avg_time, render_real_avg_time, flush_avg_time,
           cpu);
#else
    lv_label_set_text_fmt(
        sysmon,
        "%" LV_PRIu32" FPS, %" LV_PRIu32 "%% CPU\n"
        "%" LV_PRIu32" ms (%" LV_PRIu32" | %" LV_PRIu32")",
        fps, cpu,
        refr_avg_time, render_real_avg_time, flush_avg_time
    );
#endif /*LV_USE_PERF_MONITOR_LOG_MODE*/

    /*Save the refresh start time of the next period*/
    uint32_t refr_start = info->refr_start;

    /*Reset the counters*/
    lv_memzero(info, sizeof(perf_info_t));

    /*Restore the refresh start time*/
    info->refr_start = refr_start;
}

static void perf_monitor_init(void)
{
    static perf_info_t info = { 0 };
    lv_disp_t * disp = lv_disp_get_default();

    lv_obj_t * sysmon = lv_sysmon_create(lv_layer_sys());
    lv_obj_align(sysmon, LV_USE_PERF_MONITOR_POS, 0, 0);
    lv_obj_set_style_text_align(sysmon, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_user_data(sysmon, &info);
    lv_obj_add_event(sysmon, perf_monitor_event_cb, LV_EVENT_REFRESH, NULL);
    lv_disp_add_event(disp, perf_monitor_disp_event_cb, LV_EVENT_ALL, sysmon);

#if LV_USE_PERF_MONITOR_LOG_MODE
    /*Reduce rendering performance consumption*/
    lv_obj_add_flag(sysmon, LV_OBJ_FLAG_HIDDEN);
#endif
}
#endif

#if LV_USE_MEM_MONITOR && LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
static void mem_monitor_event_cb(lv_event_t * e)
{
    lv_obj_t * sysmon = lv_event_get_current_target_obj(e);
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    uint32_t used_size = mon.total_size - mon.free_size;;
    uint32_t used_kb = used_size / 1024;
    uint32_t used_kb_tenth = (used_size - (used_kb * 1024)) / 102;
    lv_label_set_text_fmt(sysmon,
                          "%"LV_PRIu32 ".%"LV_PRIu32 " kB, %d%%\n"
                          "%d%% frag.",
                          used_kb, used_kb_tenth, mon.used_pct,
                          mon.frag_pct);
}

static void mem_monitor_init(void)
{
    lv_obj_t * sysmon = lv_sysmon_create(lv_layer_sys());
    lv_obj_add_event(sysmon, mem_monitor_event_cb, LV_EVENT_REFRESH, NULL);
    lv_obj_align(sysmon, LV_USE_MEM_MONITOR_POS, 0, 0);
}
#endif

static void sysmon_async_cb(void * user_data)
{
    LV_UNUSED(user_data);
#if LV_USE_PERF_MONITOR
    perf_monitor_init();
#endif
#if LV_USE_MEM_MONITOR && LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    mem_monitor_init();
#endif
}

#endif /*LV_USE_SYSMON*/
