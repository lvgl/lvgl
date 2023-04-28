/**
 * @file lv_monitor.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_monitor.h"

#if LV_USE_MONITOR

#include "../../misc/lv_assert.h"
#include LV_COLOR_EXTERN_INCLUDE

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_monitor_class

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint32_t    elaps_sum;
    uint32_t    frame_cnt;
    lv_disp_t * disp;
} perf_info_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_monitor_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_monitor_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_monitor_timer_cb(lv_timer_t * timer);
static void monitor_async_cb(void * user_data);

#if LV_USE_PERF_MONITOR
    static void perf_monitor_init(void);
#endif

#if LV_USE_MEM_MONITOR && LV_USE_BUILTIN_MALLOC
    static void mem_monitor_init(void);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_monitor_class = {
    .base_class = &lv_label_class,
    .constructor_cb = lv_monitor_constructor,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .event_cb = lv_monitor_event,
    .instance_size = sizeof(lv_monitor_t),
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_monitor_create(void)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, lv_layer_sys());
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_monitor_set_refr_time(lv_obj_t * obj, uint32_t time)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_monitor_t * monitor = (lv_monitor_t *)obj;
    lv_timer_set_period(monitor->timer, time);
}

void _lv_monitor_builtin_init(void)
{
    lv_async_call(monitor_async_cb, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_monitor_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_monitor_t * monitor = (lv_monitor_t *)obj;
    monitor->timer = lv_timer_create(lv_monitor_timer_cb, 1000, obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_style_text_color(obj, lv_color_white(), 0);
    lv_obj_set_style_pad_top(obj, 3, 0);
    lv_obj_set_style_pad_bottom(obj, 3, 0);
    lv_obj_set_style_pad_left(obj, 3, 0);
    lv_obj_set_style_pad_right(obj, 3, 0);
    lv_label_set_text(obj, "?");
}

static void lv_monitor_timer_cb(lv_timer_t * timer)
{
    lv_obj_t * obj = lv_timer_get_user_data(timer);
    lv_obj_send_event(obj, LV_EVENT_REFRESH, NULL);
}

static void lv_monitor_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    lv_obj_event_base(MY_CLASS, e);
}

#if LV_USE_PERF_MONITOR

static void perf_monitor_refr_finish_cb(lv_event_t * e)
{
    lv_obj_t * monitor = lv_event_get_user_data(e);
    perf_info_t * info = lv_obj_get_user_data(monitor);
    info->elaps_sum += lv_tick_elaps(info->disp->last_render_start_time);
    info->frame_cnt++;
}

static void perf_monitor_event_cb(lv_event_t * e)
{
    lv_obj_t * monitor = lv_event_get_current_target_obj(e);
    perf_info_t * info = lv_obj_get_user_data(monitor);
    uint32_t cpu = 100 - lv_timer_get_idle();
    uint32_t avg_time = info->frame_cnt ? info->elaps_sum / info->frame_cnt : 0;

#if LV_USE_PERF_MONITOR_LOG_MDOE
    LV_LOG("Performance: %" LV_PRIu32" FPS / %" LV_PRIu32" ms / %" LV_PRIu32 "%% CPU\n",
           info->frame_cnt,
           avg_time,
           cpu);
#else
    lv_label_set_text_fmt(
        monitor,
        "%" LV_PRIu32" FPS / %" LV_PRIu32" ms\n%" LV_PRIu32 "%% CPU",
        info->frame_cnt,
        avg_time,
        cpu
    );
#endif /*LV_USE_PERF_MONITOR_LOG_MDOE*/
    info->elaps_sum = 0;
    info->frame_cnt = 0;
}

static void perf_monitor_init(void)
{
    perf_info_t * info = lv_malloc(sizeof(perf_info_t));
    LV_ASSERT_MALLOC(info);

    lv_memzero(info, sizeof(perf_info_t));
    info->disp = lv_disp_get_default();

    lv_obj_t * monitor = lv_monitor_create();
    lv_monitor_set_refr_time(monitor, 1000);
    lv_obj_align(monitor, LV_USE_PERF_MONITOR_POS, 0, 0);
    lv_obj_set_style_text_align(monitor, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_user_data(monitor, info);
    lv_obj_add_event(monitor, perf_monitor_event_cb, LV_EVENT_REFRESH, NULL);
    lv_disp_add_event(info->disp, perf_monitor_refr_finish_cb, LV_EVENT_REFR_FINISH, monitor);

#if LV_USE_PERF_MONITOR_LOG_MDOE
    /*Reduce rendering performance consumption*/
    lv_obj_add_flag(monitor, LV_OBJ_FLAG_HIDDEN);
#endif
}
#endif

#if LV_USE_MEM_MONITOR && LV_USE_BUILTIN_MALLOC
static void mem_monitor_event_cb(lv_event_t * e)
{
    lv_obj_t * monitor = lv_event_get_current_target_obj(e);
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    uint32_t used_size = mon.total_size - mon.free_size;;
    uint32_t used_kb = used_size / 1024;
    uint32_t used_kb_tenth = (used_size - (used_kb * 1024)) / 102;
    lv_label_set_text_fmt(monitor,
                          "%"LV_PRIu32 ".%"LV_PRIu32 " kB used (%d %%)\n"
                          "%d%% frag.",
                          used_kb, used_kb_tenth, mon.used_pct,
                          mon.frag_pct);
}

static void mem_monitor_init(void)
{
    lv_obj_t * monitor = lv_monitor_create();
    lv_obj_add_event(monitor, mem_monitor_event_cb, LV_EVENT_REFRESH, NULL);
    lv_obj_align(monitor, LV_USE_MEM_MONITOR_POS, 0, 0);
    lv_monitor_set_refr_time(monitor, 300);
}
#endif

static void monitor_async_cb(void * user_data)
{
    LV_UNUSED(user_data);
#if LV_USE_PERF_MONITOR
    perf_monitor_init();
#endif
#if LV_USE_MEM_MONITOR && LV_USE_BUILTIN_MALLOC
    mem_monitor_init();
#endif
}

#endif /*LV_USE_MONITOR*/
