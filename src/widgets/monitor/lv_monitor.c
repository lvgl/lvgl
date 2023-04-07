/**
 * @file lv_monitor.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_monitor.h"

#include "../../misc/lv_assert.h"
#include LV_COLOR_EXTERN_INCLUDE

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_monitor_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_monitor_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_monitor_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_monitor_timer_cb(lv_timer_t * timer);

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
    lv_obj_event_base(MY_CLASS, e);
}
