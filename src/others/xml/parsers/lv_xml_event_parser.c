/**
 * @file lv_xml_event_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_event_parser.h"
#if LV_USE_XML

#include "../../../lvgl.h"
#include "../../../lvgl_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_event_code_t trigger_text_to_enum_value(const char * txt);
static void free_user_data_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_event_call_function_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);


    const char * trigger = lv_xml_get_value_of(attrs, "trigger");
    lv_event_code_t code = LV_EVENT_CLICKED;
    if(trigger) code = trigger_text_to_enum_value(trigger);
    if(code == LV_EVENT_LAST)  {
        LV_LOG_WARN("Couldn't add call function event because \"%s\" trigger is invalid.", trigger);
        return NULL;
    }

    const char * cb_txt = lv_xml_get_value_of(attrs, "callback");
    if(cb_txt == NULL) {
        LV_LOG_WARN("callback is mandatory for event-call_function");
        return NULL;
    }

    lv_obj_t * obj = lv_xml_state_get_parent(state);
    lv_event_cb_t cb = lv_xml_get_event_cb(&state->scope, cb_txt);
    if(cb == NULL) {
        LV_LOG_WARN("Couldn't add call function event because \"%s\" callback is not found.", cb_txt);
        /*Don't return NULL.
         *When the component is isolated e.g. in the editor the callback is not registered */
        return obj;
    }

    const char * user_data_xml = lv_xml_get_value_of(attrs, "user_data");
    char * user_data = NULL;
    if(user_data_xml) user_data = lv_strdup(user_data_xml);

    lv_obj_add_event_cb(obj, cb, code, user_data);
    if(user_data) lv_obj_add_event_cb(obj, free_user_data_event_cb, LV_EVENT_DELETE, user_data);

    return obj;
}

void lv_xml_event_call_function_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);
    /*Nothing to apply*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void free_user_data_event_cb(lv_event_t * e)
{
    char * user_data = lv_event_get_user_data(e);
    lv_free(user_data);
}

static lv_event_code_t trigger_text_to_enum_value(const char * txt)
{

    if(lv_streq("all", txt)) return LV_EVENT_ALL;
    if(lv_streq("pressed", txt)) return LV_EVENT_PRESSED;
    if(lv_streq("pressing", txt)) return LV_EVENT_PRESSING;
    if(lv_streq("press_lost", txt)) return LV_EVENT_PRESS_LOST;
    if(lv_streq("short_clicked", txt)) return LV_EVENT_SHORT_CLICKED;
    if(lv_streq("single_clicked", txt)) return LV_EVENT_SINGLE_CLICKED;
    if(lv_streq("double_clicked", txt)) return LV_EVENT_DOUBLE_CLICKED;
    if(lv_streq("triple_clicked", txt)) return LV_EVENT_TRIPLE_CLICKED;
    if(lv_streq("long_pressed", txt)) return LV_EVENT_LONG_PRESSED;
    if(lv_streq("long_pressed_repeat", txt)) return LV_EVENT_LONG_PRESSED_REPEAT;
    if(lv_streq("clicked", txt)) return LV_EVENT_CLICKED;
    if(lv_streq("released", txt)) return LV_EVENT_RELEASED;
    if(lv_streq("scroll_begin", txt)) return LV_EVENT_SCROLL_BEGIN;
    if(lv_streq("scroll_throw_begin", txt)) return LV_EVENT_SCROLL_THROW_BEGIN;
    if(lv_streq("scroll_end", txt)) return LV_EVENT_SCROLL_END;
    if(lv_streq("scroll", txt)) return LV_EVENT_SCROLL;
    if(lv_streq("gesture", txt)) return LV_EVENT_GESTURE;
    if(lv_streq("key", txt)) return LV_EVENT_KEY;
    if(lv_streq("rotary", txt)) return LV_EVENT_ROTARY;
    if(lv_streq("focused", txt)) return LV_EVENT_FOCUSED;
    if(lv_streq("defocused", txt)) return LV_EVENT_DEFOCUSED;
    if(lv_streq("leave", txt)) return LV_EVENT_LEAVE;
    if(lv_streq("hit_test", txt)) return LV_EVENT_HIT_TEST;
    if(lv_streq("indev_reset", txt)) return LV_EVENT_INDEV_RESET;
    if(lv_streq("hover_over", txt)) return LV_EVENT_HOVER_OVER;
    if(lv_streq("hover_leave", txt)) return LV_EVENT_HOVER_LEAVE;
    if(lv_streq("cover_check", txt)) return LV_EVENT_COVER_CHECK;
    if(lv_streq("refr_ext_draw_size", txt)) return LV_EVENT_REFR_EXT_DRAW_SIZE;
    if(lv_streq("draw_main_begin", txt)) return LV_EVENT_DRAW_MAIN_BEGIN;
    if(lv_streq("draw_main", txt)) return LV_EVENT_DRAW_MAIN;
    if(lv_streq("draw_main_end", txt)) return LV_EVENT_DRAW_MAIN_END;
    if(lv_streq("draw_post_begin", txt)) return LV_EVENT_DRAW_POST_BEGIN;
    if(lv_streq("draw_post", txt)) return LV_EVENT_DRAW_POST;
    if(lv_streq("draw_post_end", txt)) return LV_EVENT_DRAW_POST_END;
    if(lv_streq("draw_task_added", txt)) return LV_EVENT_DRAW_TASK_ADDED;
    if(lv_streq("value_changed", txt)) return LV_EVENT_VALUE_CHANGED;
    if(lv_streq("insert", txt)) return LV_EVENT_INSERT;
    if(lv_streq("refresh", txt)) return LV_EVENT_REFRESH;
    if(lv_streq("ready", txt)) return LV_EVENT_READY;
    if(lv_streq("cancel", txt)) return LV_EVENT_CANCEL;
    if(lv_streq("create", txt)) return LV_EVENT_CREATE;
    if(lv_streq("delete", txt)) return LV_EVENT_DELETE;
    if(lv_streq("child_changed", txt)) return LV_EVENT_CHILD_CHANGED;
    if(lv_streq("child_created", txt)) return LV_EVENT_CHILD_CREATED;
    if(lv_streq("child_deleted", txt)) return LV_EVENT_CHILD_DELETED;
    if(lv_streq("screen_unload_start", txt)) return LV_EVENT_SCREEN_UNLOAD_START;
    if(lv_streq("screen_load_start", txt)) return LV_EVENT_SCREEN_LOAD_START;
    if(lv_streq("screen_loaded", txt)) return LV_EVENT_SCREEN_LOADED;
    if(lv_streq("screen_unloaded", txt)) return LV_EVENT_SCREEN_UNLOADED;
    if(lv_streq("size_changed", txt)) return LV_EVENT_SIZE_CHANGED;
    if(lv_streq("style_changed", txt)) return LV_EVENT_STYLE_CHANGED;
    if(lv_streq("layout_changed", txt)) return LV_EVENT_LAYOUT_CHANGED;
    if(lv_streq("get_self_size", txt)) return LV_EVENT_GET_SELF_SIZE;
    if(lv_streq("invalidate_area", txt)) return LV_EVENT_INVALIDATE_AREA;
    if(lv_streq("resolution_changed", txt)) return LV_EVENT_RESOLUTION_CHANGED;
    if(lv_streq("color_format_changed", txt)) return LV_EVENT_COLOR_FORMAT_CHANGED;
    if(lv_streq("refr_request", txt)) return LV_EVENT_REFR_REQUEST;
    if(lv_streq("refr_start", txt)) return LV_EVENT_REFR_START;
    if(lv_streq("refr_ready", txt)) return LV_EVENT_REFR_READY;
    if(lv_streq("render_start", txt)) return LV_EVENT_RENDER_START;
    if(lv_streq("render_ready", txt)) return LV_EVENT_RENDER_READY;
    if(lv_streq("flush_start", txt)) return LV_EVENT_FLUSH_START;
    if(lv_streq("flush_finish", txt)) return LV_EVENT_FLUSH_FINISH;
    if(lv_streq("flush_wait_start", txt)) return LV_EVENT_FLUSH_WAIT_START;
    if(lv_streq("flush_wait_finish", txt)) return LV_EVENT_FLUSH_WAIT_FINISH;
    if(lv_streq("vsync", txt)) return LV_EVENT_VSYNC;

    LV_LOG_WARN("%s is an unknown value for event's trigger", txt);
    return LV_EVENT_LAST; /*Indicate error*/
}

#endif /* LV_USE_XML */
