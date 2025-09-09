/**
 * @file lv_xml_obj_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_obj_parser.h"
#if LV_USE_XML

#include "../../../lvgl.h"
#include "../../../lvgl_private.h"

/*********************
 *      DEFINES
 *********************/
#define lv_event_xml_store_timeline LV_GLOBAL_DEFAULT()->lv_event_xml_store_timeline

/**********************
 *      TYPEDEFS
 **********************/

/*Duplication from lv_obj.c as lv_obj_add_screen_create_event needs to be
 * reimplemented here slightly differently */
typedef struct {
    lv_screen_load_anim_t anim_type;
    uint32_t duration;
    uint32_t delay;
    const char * screen_name;
} screen_load_anim_dsc_t;

typedef struct {
    const char * timeline_name;
    const char * target_name;
    uint32_t delay;
    bool reverse;
    lv_obj_t * base_obj; /**< Get the objs by name from here (the view) */
} play_anim_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_flag_t flag_to_enum(const char * txt);
static void apply_styles(lv_xml_parser_state_t * state, lv_obj_t * obj, const char * name, const char * value);
static void screen_create_on_trigger_event_cb(lv_event_t * e);
static void screen_load_on_trigger_event_cb(lv_event_t * e);
static void delete_on_screen_unloaded_event_cb(lv_event_t * e);
static void free_screen_create_user_data_on_delete_event_cb(lv_event_t * e);
static void play_anim_on_trigger_event_cb(lv_event_t * e);
static void free_play_anim_user_data_on_delete_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/* Expands to
   if(lv_streq(prop_name, "style_height")) lv_obj_set_style_height(obj, value, selector)
 */
#define SET_STYLE_IF(prop, value) if(lv_streq(prop_name, "style_" #prop)) lv_obj_set_style_##prop(obj, value, selector)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_obj_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_obj_create(lv_xml_state_get_parent(state));

    return item;
}

void lv_xml_obj_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];
        size_t name_len = lv_strlen(name);

#if LV_USE_OBJ_NAME
        if(lv_streq("name", name)) {
            lv_obj_set_name(item, value);
        }
#endif
        if(lv_streq("x", name)) lv_obj_set_x(item, lv_xml_to_size(value));
        else if(lv_streq("y", name)) lv_obj_set_y(item, lv_xml_to_size(value));
        else if(lv_streq("width", name)) lv_obj_set_width(item, lv_xml_to_size(value));
        else if(lv_streq("height", name)) lv_obj_set_height(item, lv_xml_to_size(value));
        else if(lv_streq("align", name)) lv_obj_set_align(item, lv_xml_align_to_enum(value));
        else if(lv_streq("flex_flow", name)) lv_obj_set_flex_flow(item, lv_xml_flex_flow_to_enum(value));
        else if(lv_streq("flex_grow", name)) lv_obj_set_flex_grow(item, lv_xml_atoi(value));
        else if(lv_streq("ext_click_area", name)) lv_obj_set_ext_click_area(item, lv_xml_atoi(value));
        else if(lv_streq("scroll_snap_x", name)) lv_obj_set_scroll_snap_x(item, lv_xml_scroll_snap_to_enum(value));
        else if(lv_streq("scroll_snap_y", name)) lv_obj_set_scroll_snap_y(item, lv_xml_scroll_snap_to_enum(value));

        else if(lv_streq("hidden", name))               lv_obj_set_flag(item, LV_OBJ_FLAG_HIDDEN, lv_xml_to_bool(value));
        else if(lv_streq("clickable", name))            lv_obj_set_flag(item, LV_OBJ_FLAG_CLICKABLE, lv_xml_to_bool(value));
        else if(lv_streq("click_focusable", name))      lv_obj_set_flag(item, LV_OBJ_FLAG_CLICK_FOCUSABLE,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("checkable", name))            lv_obj_set_flag(item, LV_OBJ_FLAG_CHECKABLE, lv_xml_to_bool(value));
        else if(lv_streq("scrollable", name))           lv_obj_set_flag(item, LV_OBJ_FLAG_SCROLLABLE, lv_xml_to_bool(value));
        else if(lv_streq("scroll_elastic", name))       lv_obj_set_flag(item, LV_OBJ_FLAG_SCROLL_ELASTIC,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("scroll_momentum", name))      lv_obj_set_flag(item, LV_OBJ_FLAG_SCROLL_MOMENTUM,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("scroll_one", name))           lv_obj_set_flag(item, LV_OBJ_FLAG_SCROLL_ONE, lv_xml_to_bool(value));
        else if(lv_streq("scroll_chain_hor", name))     lv_obj_set_flag(item, LV_OBJ_FLAG_SCROLL_CHAIN_HOR,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("scroll_chain_ver", name))     lv_obj_set_flag(item, LV_OBJ_FLAG_SCROLL_CHAIN_VER,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("scroll_chain", name))         lv_obj_set_flag(item, LV_OBJ_FLAG_SCROLL_CHAIN,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("scroll_on_focus", name))      lv_obj_set_flag(item, LV_OBJ_FLAG_SCROLL_ON_FOCUS,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("scroll_with_arrow", name))    lv_obj_set_flag(item, LV_OBJ_FLAG_SCROLL_WITH_ARROW,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("snappable", name))            lv_obj_set_flag(item, LV_OBJ_FLAG_SNAPPABLE, lv_xml_to_bool(value));
        else if(lv_streq("press_lock", name))           lv_obj_set_flag(item, LV_OBJ_FLAG_PRESS_LOCK, lv_xml_to_bool(value));
        else if(lv_streq("event_bubble", name))         lv_obj_set_flag(item, LV_OBJ_FLAG_EVENT_BUBBLE,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("event_trickle", name))        lv_obj_set_flag(item, LV_OBJ_FLAG_EVENT_TRICKLE,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("state_trickle", name))       lv_obj_set_flag(item, LV_OBJ_FLAG_STATE_TRICKLE,
                                                                           lv_xml_to_bool(value));
        else if(lv_streq("gesture_bubble", name))       lv_obj_set_flag(item, LV_OBJ_FLAG_GESTURE_BUBBLE,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("adv_hittest", name))          lv_obj_set_flag(item, LV_OBJ_FLAG_ADV_HITTEST,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("ignore_layout", name))        lv_obj_set_flag(item, LV_OBJ_FLAG_IGNORE_LAYOUT,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("floating", name))             lv_obj_set_flag(item, LV_OBJ_FLAG_FLOATING, lv_xml_to_bool(value));
        else if(lv_streq("send_draw_task_events", name))lv_obj_set_flag(item, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("overflow_visible", name))     lv_obj_set_flag(item, LV_OBJ_FLAG_OVERFLOW_VISIBLE,
                                                                            lv_xml_to_bool(value));
        else if(lv_streq("flex_in_new_track", name))    lv_obj_set_flag(item, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK,
                                                                            lv_xml_to_bool(value));

        else if(lv_streq("checked", name))  lv_obj_set_state(item, LV_STATE_CHECKED, lv_xml_to_bool(value));
        else if(lv_streq("focused", name))  lv_obj_set_state(item, LV_STATE_FOCUSED, lv_xml_to_bool(value));
        else if(lv_streq("focus_key", name)) lv_obj_set_state(item, LV_STATE_FOCUS_KEY, lv_xml_to_bool(value));
        else if(lv_streq("edited", name))   lv_obj_set_state(item, LV_STATE_EDITED, lv_xml_to_bool(value));
        else if(lv_streq("hovered", name))  lv_obj_set_state(item, LV_STATE_HOVERED, lv_xml_to_bool(value));
        else if(lv_streq("pressed", name))  lv_obj_set_state(item, LV_STATE_PRESSED, lv_xml_to_bool(value));
        else if(lv_streq("scrolled", name)) lv_obj_set_state(item, LV_STATE_SCROLLED, lv_xml_to_bool(value));
        else if(lv_streq("disabled", name)) lv_obj_set_state(item, LV_STATE_DISABLED, lv_xml_to_bool(value));

        else if(lv_streq("bind_checked", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                lv_obj_bind_checked(item, subject);
            }
            else {
                LV_LOG_WARN("Subject `%s` doesn't exist in lv_obj bind_checked", value);
            }
        }

        else if(name_len > 6 && lv_memcmp("style_", name, 6) == 0) {
            apply_styles(state, item, name, value);
        }
    }
}

void * lv_obj_xml_style_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_style_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * name = lv_xml_get_value_of(attrs, "name");
    if(name == NULL) {
        /*Silently ignore this issue.
         *The name set to NULL if there there was no default value when resolving params*/
        return;
    }
    lv_xml_style_t * xml_style = lv_xml_get_style_by_name(&state->scope, name);
    if(xml_style == NULL) {
        LV_LOG_WARN("`%s` style is not found", name);
        return;
    }

    const char * selector_str = lv_xml_get_value_of(attrs, "selector");
    lv_style_selector_t selector = lv_xml_style_selector_text_to_enum(selector_str);

    void * item = lv_xml_state_get_parent(state);
    lv_obj_add_style(item, &xml_style->style, selector);
}

void * lv_obj_xml_remove_style_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_remove_style_apply(lv_xml_parser_state_t * state, const char ** attrs)
{

    const char * style_str = lv_xml_get_value_of(attrs, "name");
    const char * selector_str = lv_xml_get_value_of(attrs, "selector");

    lv_style_t * style = NULL;
    if(style_str) {
        lv_xml_style_t * xml_style = lv_xml_get_style_by_name(&state->scope, style_str);
        if(xml_style == NULL) {
            LV_LOG_WARN("No style found with name `%s`", style_str);
            return;
        }
        style = &xml_style->style;
    }

    lv_style_selector_t selector = lv_xml_style_selector_text_to_enum(selector_str);

    void * item = lv_xml_state_get_item(state);
    lv_obj_remove_style(item, style, selector);
}

void * lv_obj_xml_remove_style_all_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_remove_style_all_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_item(state);
    lv_obj_remove_style_all(item);
}

void * lv_obj_xml_event_cb_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_event_cb_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * trigger_str = lv_xml_get_value_of(attrs, "trigger");
    lv_event_code_t code = LV_EVENT_CLICKED;
    if(trigger_str) code = lv_xml_trigger_text_to_enum_value(trigger_str);
    if(code == LV_EVENT_LAST)  {
        LV_LOG_WARN("Couldn't add call function event because `%s` trigger is invalid.", trigger_str);
        return;
    }

    const char * cb_str = lv_xml_get_value_of(attrs, "callback");
    if(cb_str == NULL) {
        LV_LOG_WARN("callback is mandatory for event-call_function");
        return;
    }

    lv_obj_t * obj = lv_xml_state_get_parent(state);
    lv_event_cb_t cb = lv_xml_get_event_cb(&state->scope, cb_str);
    if(cb == NULL) {
        LV_LOG_WARN("Couldn't add call function event because `%s` callback is not found.", cb_str);
        return;
    }

    const char * user_data_str = lv_xml_get_value_of(attrs, "user_data");
    char * user_data = NULL;
    if(user_data_str) user_data = lv_strdup(user_data_str);

    lv_obj_add_event_cb(obj, cb, code, user_data);
    if(user_data) lv_obj_add_event_cb(obj, lv_event_free_user_data_cb, LV_EVENT_DELETE, user_data);
}

void * lv_obj_xml_subject_set_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_subject_set_apply(lv_xml_parser_state_t * state, const char ** attrs)
{

    /*If the tag_name is */
    lv_subject_type_t subject_type = LV_SUBJECT_TYPE_NONE;
    if(lv_streq(state->tag_name, "lv_obj-subject_set_int_event") ||
       lv_streq(state->tag_name, "subject_set_int_event")) {
        subject_type = LV_SUBJECT_TYPE_INT;
    }
#if LV_USE_FLOAT
    else if(lv_streq(state->tag_name, "lv_obj-subject_set_float_event") ||
            lv_streq(state->tag_name, "subject_set_float_event")) {
        subject_type = LV_SUBJECT_TYPE_FLOAT;
    }
#endif
    else if(lv_streq(state->tag_name, "lv_obj-subject_set_string_event") ||
            lv_streq(state->tag_name, "subject_set_string_event")) {
        subject_type = LV_SUBJECT_TYPE_STRING;
    }
    else {
        LV_LOG_WARN("`%s` is not supported in <lv_obj-subject_set_event>", state->tag_name);
        return;
    }

    const char * subject_str =  lv_xml_get_value_of(attrs, "subject");
    const char * trigger_str =  lv_xml_get_value_of(attrs, "trigger");
    const char * value_str =  lv_xml_get_value_of(attrs, "value");

    if(subject_str == NULL) {
        LV_LOG_WARN("`subject` is missing in <lv_obj-subject_set_event>");
        return;
    }

    if(value_str == NULL) {
        LV_LOG_WARN("`value` is missing in <lv_obj-subject_set_event>");
        return;
    }

    lv_event_code_t trigger = LV_EVENT_CLICKED;
    if(trigger_str) trigger = lv_xml_trigger_text_to_enum_value(trigger_str);
    if(trigger == LV_EVENT_LAST)  {
        LV_LOG_WARN("Couldn't apply <subject_set_event> because `%s` trigger is invalid.", trigger_str);
        return;
    }

    lv_subject_t * subject = lv_xml_get_subject(&state->scope, subject_str);
    if(subject == NULL) {
        LV_LOG_WARN("Subject `%s` doesn't exist in <lv_obj-subject_set>", subject_str);
        return;
    }

    if(subject->type != subject_type) {
        LV_LOG_WARN("`%s` subject has incorrect type in <lv_obj-subject_set>", subject_str);
        return;
    }

    void * item = lv_xml_state_get_item(state);
    if(subject_type == LV_SUBJECT_TYPE_INT) {
        lv_obj_add_subject_set_int_event(item, subject, trigger, lv_xml_atoi(value_str));
    }
    else if(subject_type == LV_SUBJECT_TYPE_FLOAT) {
#if LV_USE_FLOAT
        lv_obj_add_subject_set_float_event(item, subject, trigger, lv_xml_atof(value_str));
#else
        LV_LOG_ERROR("Tried to add a subject of type float but LV_USE_FLOAT is not enabled");
#endif
    }
    else if(subject_type == LV_SUBJECT_TYPE_STRING) {
        lv_obj_add_subject_set_string_event(item, subject, trigger, value_str);
    }
}

void * lv_obj_xml_subject_increment_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_subject_increment_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * subject_str =  lv_xml_get_value_of(attrs, "subject");
    const char * trigger_str =  lv_xml_get_value_of(attrs, "trigger");
    const char * step_str =  lv_xml_get_value_of(attrs, "step");
    const char * min_str =  lv_xml_get_value_of(attrs, "min");
    const char * max_str =  lv_xml_get_value_of(attrs, "max");

    if(subject_str == NULL) {
        LV_LOG_WARN("`subject` is missing in <lv_obj-subject_increment>");
        return;
    }

    if(step_str == NULL) step_str = "1";

    lv_event_code_t trigger = LV_EVENT_CLICKED;
    if(trigger_str) trigger = lv_xml_trigger_text_to_enum_value(trigger_str);
    if(trigger == LV_EVENT_LAST)  {
        LV_LOG_WARN("Couldn't apply <subject_increment> because `%s` trigger is invalid.", trigger_str);
        return;
    }

    lv_subject_t * subject = lv_xml_get_subject(&state->scope, subject_str);
    if(subject == NULL) {
        LV_LOG_WARN("Subject `%s` doesn't exist in <lv_obj-subject_increment>", subject_str);
        return;
    }

    if(subject->type != LV_SUBJECT_TYPE_INT && subject->type != LV_SUBJECT_TYPE_FLOAT) {
        LV_LOG_WARN("`%s` subject should have integer type in <lv_obj-subject_increment>", subject_str);
        return;
    }

    void * item = lv_xml_state_get_item(state);

    int32_t step = lv_xml_atoi(step_str);
    int32_t min_v = min_str ? lv_xml_atoi(min_str) : INT32_MIN;
    int32_t max_v = max_str ? lv_xml_atoi(max_str) : INT32_MAX;
    lv_obj_add_subject_increment_event(item, subject, trigger, step, min_v, max_v);
}

void * lv_obj_xml_bind_style_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_bind_style_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * name = lv_xml_get_value_of(attrs, "name");
    if(name == NULL) {
        /*Silently ignore this issue.
         *The name set to NULL if there there was no default value when resolving params*/
        return;
    }
    lv_xml_style_t * xml_style = lv_xml_get_style_by_name(&state->scope, name);
    if(xml_style == NULL) {
        LV_LOG_WARN("`%s` style is not found", name);
        return;
    }
    const char * subject_str = lv_xml_get_value_of(attrs, "subject");

    if(subject_str == NULL) {
        LV_LOG_WARN("`subject` is missing in lv_obj bind_style");
        return;
    }

    lv_subject_t * subject = lv_xml_get_subject(&state->scope, subject_str);
    if(subject == NULL) {
        LV_LOG_WARN("Subject `%s` doesn't exist in lv_obj bind_style", subject_str);
        return;
    }

    const char * ref_value_str = lv_xml_get_value_of(attrs, "ref_value");
    if(ref_value_str == NULL) {
        LV_LOG_WARN("`ref_value` is missing in lv_obj bind_style");
        return;
    }

    int32_t ref_value = lv_xml_atoi(ref_value_str);

    const char * selector_str = lv_xml_get_value_of(attrs, "selector");
    lv_style_selector_t selector = lv_xml_style_selector_text_to_enum(selector_str);

    void * item = lv_xml_state_get_parent(state);
    lv_obj_bind_style(item, &xml_style->style, selector, subject, ref_value);
}

void * lv_obj_xml_bind_flag_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_bind_flag_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * op = state->tag_name;

    /*If starts with "lv_obj-" skip that part*/
    if(op[0] == 'l') op += 7;

    lv_observer_t * (*cb)(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value) = NULL;
    if(lv_streq(op, "bind_flag_if_eq")) cb = lv_obj_bind_flag_if_eq;
    else if(lv_streq(op, "bind_flag_if_not_eq")) cb = lv_obj_bind_flag_if_not_eq;
    else if(lv_streq(op, "bind_flag_if_gt")) cb = lv_obj_bind_flag_if_gt;
    else if(lv_streq(op, "bind_flag_if_ge")) cb = lv_obj_bind_flag_if_ge;
    else if(lv_streq(op, "bind_flag_if_lt")) cb = lv_obj_bind_flag_if_lt;
    else if(lv_streq(op, "bind_flag_if_le")) cb = lv_obj_bind_flag_if_le;
    else {
        LV_LOG_WARN("`%s` is not known", op);
        return;
    }

    const char * subject_str =  lv_xml_get_value_of(attrs, "subject");
    const char * flag_str =  lv_xml_get_value_of(attrs, "flag");
    const char * ref_value_str = lv_xml_get_value_of(attrs, "ref_value");

    if(subject_str == NULL) {
        LV_LOG_WARN("`subject` is missing in lv_obj bind_flag");
    }
    else if(flag_str == NULL) {
        LV_LOG_WARN("`flag` is missing in lv_obj bind_flag");
    }
    else if(ref_value_str == NULL) {
        LV_LOG_WARN("`ref_value` is missing in lv_obj bind_flag");
    }
    else {
        lv_subject_t * subject = lv_xml_get_subject(&state->scope, subject_str);
        if(subject == NULL) {
            LV_LOG_WARN("Subject `%s` doesn't exist in lv_obj bind_flag", subject_str);
        }
        else {
            lv_obj_flag_t flag = flag_to_enum(flag_str);
            int32_t ref_value = lv_xml_atoi(ref_value_str);
            void * item = lv_xml_state_get_item(state);
            cb(item, subject, flag, ref_value);
        }
    }
}

void * lv_obj_xml_bind_state_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_bind_state_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * op = state->tag_name;

    /*If starts with "lv_obj-" skip that part*/
    if(op[0] == 'l') op += 7;

    lv_observer_t * (*cb)(lv_obj_t * obj, lv_subject_t * subject, lv_state_t flag, int32_t ref_value) = NULL;
    if(lv_streq(op, "bind_state_if_eq")) cb = lv_obj_bind_state_if_eq;
    else if(lv_streq(op, "bind_state_if_not_eq")) cb = lv_obj_bind_state_if_not_eq;
    else if(lv_streq(op, "bind_state_if_gt")) cb = lv_obj_bind_state_if_gt;
    else if(lv_streq(op, "bind_state_if_ge")) cb = lv_obj_bind_state_if_ge;
    else if(lv_streq(op, "bind_state_if_lt")) cb = lv_obj_bind_state_if_lt;
    else if(lv_streq(op, "bind_state_if_le")) cb = lv_obj_bind_state_if_le;
    else {
        LV_LOG_WARN("`%s` is not known", op);
        return;
    }

    const char * subject_str =  lv_xml_get_value_of(attrs, "subject");
    const char * state_str =  lv_xml_get_value_of(attrs, "state");
    const char * ref_value_str = lv_xml_get_value_of(attrs, "ref_value");

    if(subject_str == NULL) {
        LV_LOG_WARN("`subject` is missing in lv_obj state_flag");
    }
    else if(state_str == NULL) {
        LV_LOG_WARN("`state` is missing in lv_obj state_flag");
    }
    else if(ref_value_str == NULL) {
        LV_LOG_WARN("`ref_value` is missing in lv_obj state_flag");
    }
    else {
        lv_subject_t * subject = lv_xml_get_subject(&state->scope, subject_str);
        if(subject == NULL) {
            LV_LOG_WARN("Subject `%s` doesn't exist in bind_state", subject_str);
        }
        else {
            lv_state_t s = lv_xml_state_to_enum(state_str);
            int32_t ref_value = lv_xml_atoi(ref_value_str);
            void * item = lv_xml_state_get_item(state);
            cb(item, subject, s, ref_value);
        }
    }
}

void * lv_obj_xml_screen_load_event_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_screen_load_event_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * screen_str = lv_xml_get_value_of(attrs, "screen");
    const char * duration_str = lv_xml_get_value_of(attrs, "duration");
    const char * delay_str = lv_xml_get_value_of(attrs, "delay");
    const char * anim_type_str = lv_xml_get_value_of(attrs, "anim_type");
    const char * trigger_str = lv_xml_get_value_of(attrs, "trigger");

    if(screen_str == NULL) {
        LV_LOG_WARN("`screen` is missing in <lv_obj-screen_load_event>");
        return;
    }

    if(duration_str == NULL) duration_str = "0";
    if(delay_str == NULL) delay_str = "0";
    if(anim_type_str == NULL) anim_type_str = "none";
    if(trigger_str == NULL) trigger_str = "clicked";

    lv_event_code_t trigger = lv_xml_trigger_text_to_enum_value(trigger_str);
    if(trigger == LV_EVENT_LAST)  {
        LV_LOG_WARN("Couldn't apply <screen_load_event> because `%s` trigger is invalid.", trigger_str);
        return;
    }

    int32_t duration = lv_xml_atoi(duration_str);
    int32_t delay = lv_xml_atoi(delay_str);
    lv_screen_load_anim_t anim_type = lv_xml_screen_load_anim_text_to_enum_value(anim_type_str);

    void * item = lv_xml_state_get_item(state);

    screen_load_anim_dsc_t * dsc = lv_malloc(sizeof(screen_load_anim_dsc_t));
    LV_ASSERT_MALLOC(dsc);
    lv_memzero(dsc, sizeof(screen_load_anim_dsc_t));
    dsc->anim_type = anim_type;
    dsc->duration = duration;
    dsc->delay = delay;
    dsc->screen_name = lv_strdup(screen_str);

    lv_obj_add_event_cb(item, screen_load_on_trigger_event_cb, trigger, dsc);
    lv_obj_add_event_cb(item, free_screen_create_user_data_on_delete_event_cb, LV_EVENT_DELETE, dsc);
}


void * lv_obj_xml_screen_create_event_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_screen_create_event_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * screen_str = lv_xml_get_value_of(attrs, "screen");
    const char * duration_str = lv_xml_get_value_of(attrs, "duration");
    const char * delay_str = lv_xml_get_value_of(attrs, "delay");
    const char * anim_type_str = lv_xml_get_value_of(attrs, "anim_type");
    const char * trigger_str = lv_xml_get_value_of(attrs, "trigger");

    if(screen_str == NULL) {
        LV_LOG_WARN("`screen` is missing in <lv_obj-screen_load_event>");
        return;
    }

    if(duration_str == NULL) duration_str = "0";
    if(delay_str == NULL) delay_str = "0";
    if(anim_type_str == NULL) anim_type_str = "none";
    if(trigger_str == NULL) trigger_str = "clicked";

    lv_event_code_t trigger = lv_xml_trigger_text_to_enum_value(trigger_str);
    if(trigger == LV_EVENT_LAST)  {
        LV_LOG_WARN("Couldn't apply <screen_load_event> because `%s` trigger is invalid.", trigger_str);
        return;
    }

    int32_t duration = lv_xml_atoi(duration_str);
    int32_t delay = lv_xml_atoi(delay_str);
    lv_screen_load_anim_t anim_type = lv_xml_screen_load_anim_text_to_enum_value(anim_type_str);

    screen_load_anim_dsc_t * dsc = lv_malloc(sizeof(screen_load_anim_dsc_t));
    LV_ASSERT_MALLOC(dsc);
    lv_memzero(dsc, sizeof(screen_load_anim_dsc_t));
    dsc->anim_type = anim_type;
    dsc->duration = duration;
    dsc->delay = delay;
    dsc->screen_name = lv_strdup(screen_str);

    void * item = lv_xml_state_get_item(state);
    lv_obj_add_event_cb(item, screen_create_on_trigger_event_cb, trigger, dsc);
    lv_obj_add_event_cb(item, free_screen_create_user_data_on_delete_event_cb, LV_EVENT_DELETE, dsc);
}

void * lv_obj_xml_play_timeline_event_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);
    return item;
}

void lv_obj_xml_play_timeline_event_apply(lv_xml_parser_state_t * state, const char ** attrs)
{

    if(state->view == NULL) {
        /*Shouldn't happen*/
        LV_LOG_WARN("view is not set, can't add the event");
        return;
    }

    const char * target_str = lv_xml_get_value_of(attrs, "target");
    const char * delay_str = lv_xml_get_value_of(attrs, "delay");
    const char * trigger_str = lv_xml_get_value_of(attrs, "trigger");
    const char * timeline_str = lv_xml_get_value_of(attrs, "timeline");
    const char * reverse_str = lv_xml_get_value_of(attrs, "reverse");

    if(target_str == NULL) {
        LV_LOG_WARN("`target` is missing in <lv_obj-play_animation_event>");
        return;
    }

    if(timeline_str == NULL) {
        LV_LOG_WARN("`timeline` is missing in <lv_obj-play_animation_event>");
        return;
    }

    if(delay_str == NULL) delay_str = "0";
    if(trigger_str == NULL) trigger_str = "clicked";
    if(reverse_str == NULL) reverse_str = "false";

    lv_event_code_t trigger = lv_xml_trigger_text_to_enum_value(trigger_str);
    if(trigger == LV_EVENT_LAST)  {
        LV_LOG_WARN("Couldn't apply <screen_load_event> because `%s` trigger is invalid.", trigger_str);
        return;
    }

    play_anim_dsc_t * dsc = lv_malloc(sizeof(play_anim_dsc_t));
    LV_ASSERT_MALLOC(dsc);
    lv_memzero(dsc, sizeof(play_anim_dsc_t));
    dsc->target_name = lv_strdup(target_str);
    dsc->timeline_name = lv_strdup(timeline_str);
    dsc->delay = lv_xml_atoi(delay_str);
    dsc->reverse = lv_xml_to_bool(reverse_str);
    dsc->base_obj = state->view;

    void * item = lv_xml_state_get_item(state);
    lv_obj_add_event_cb(item, play_anim_on_trigger_event_cb, trigger, dsc);
    lv_obj_add_event_cb(item, free_play_anim_user_data_on_delete_event_cb, LV_EVENT_DELETE, dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_flag_t flag_to_enum(const char * txt)
{
    if(lv_streq("hidden", txt)) return LV_OBJ_FLAG_HIDDEN;
    if(lv_streq("clickable", txt)) return LV_OBJ_FLAG_CLICKABLE;
    if(lv_streq("click_focusable", txt)) return LV_OBJ_FLAG_CLICK_FOCUSABLE;
    if(lv_streq("checkable", txt)) return LV_OBJ_FLAG_CHECKABLE;
    if(lv_streq("scrollable", txt)) return LV_OBJ_FLAG_SCROLLABLE;
    if(lv_streq("scroll_elastic", txt)) return LV_OBJ_FLAG_SCROLL_ELASTIC;
    if(lv_streq("scroll_momentum", txt)) return LV_OBJ_FLAG_SCROLL_MOMENTUM;
    if(lv_streq("scroll_one", txt)) return LV_OBJ_FLAG_SCROLL_ONE;
    if(lv_streq("scroll_chain_hor", txt)) return LV_OBJ_FLAG_SCROLL_CHAIN_HOR;
    if(lv_streq("scroll_chain_ver", txt)) return LV_OBJ_FLAG_SCROLL_CHAIN_VER;
    if(lv_streq("scroll_chain", txt)) return LV_OBJ_FLAG_SCROLL_CHAIN;
    if(lv_streq("scroll_on_focus", txt)) return LV_OBJ_FLAG_SCROLL_ON_FOCUS;
    if(lv_streq("scroll_with_arrow", txt)) return LV_OBJ_FLAG_SCROLL_WITH_ARROW;
    if(lv_streq("snappable", txt)) return LV_OBJ_FLAG_SNAPPABLE;
    if(lv_streq("press_lock", txt)) return LV_OBJ_FLAG_PRESS_LOCK;
    if(lv_streq("event_bubble", txt)) return LV_OBJ_FLAG_EVENT_BUBBLE;
    if(lv_streq("event_trickle", txt)) return LV_OBJ_FLAG_EVENT_TRICKLE;
    if(lv_streq("state_trickle", txt)) return LV_OBJ_FLAG_STATE_TRICKLE;
    if(lv_streq("gesture_bubble", txt)) return LV_OBJ_FLAG_GESTURE_BUBBLE;
    if(lv_streq("adv_hittest", txt)) return LV_OBJ_FLAG_ADV_HITTEST;
    if(lv_streq("ignore_layout", txt)) return LV_OBJ_FLAG_IGNORE_LAYOUT;
    if(lv_streq("floating", txt)) return LV_OBJ_FLAG_FLOATING;
    if(lv_streq("send_draw_task_evenTS", txt)) return LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS;
    if(lv_streq("overflow_visible", txt)) return LV_OBJ_FLAG_OVERFLOW_VISIBLE;
    if(lv_streq("flex_in_new_track", txt)) return LV_OBJ_FLAG_FLEX_IN_NEW_TRACK;
    if(lv_streq("layout_1", txt)) return LV_OBJ_FLAG_LAYOUT_1;
    if(lv_streq("layout_2", txt)) return LV_OBJ_FLAG_LAYOUT_2;
    if(lv_streq("widget_1", txt)) return LV_OBJ_FLAG_WIDGET_1;
    if(lv_streq("widget_2", txt)) return LV_OBJ_FLAG_WIDGET_2;
    if(lv_streq("user_1", txt)) return LV_OBJ_FLAG_USER_1;
    if(lv_streq("user_2", txt)) return LV_OBJ_FLAG_USER_2;
    if(lv_streq("user_3", txt)) return LV_OBJ_FLAG_USER_3;
    if(lv_streq("user_4", txt)) return LV_OBJ_FLAG_USER_4;

    LV_LOG_WARN("%s is an unknown value for flag", txt);
    return 0; /*Return 0 in lack of a better option. */
}


static void apply_styles(lv_xml_parser_state_t * state, lv_obj_t * obj, const char * name, const char * value)
{
    char name_local[512];
    lv_strlcpy(name_local, name, sizeof(name_local));

    lv_style_selector_t selector;
    const char * prop_name = lv_xml_style_string_process(name_local, &selector);

    SET_STYLE_IF(width, lv_xml_to_size(value));
    else SET_STYLE_IF(min_width, lv_xml_to_size(value));
    else SET_STYLE_IF(max_width, lv_xml_to_size(value));
    else SET_STYLE_IF(height, lv_xml_to_size(value));
    else SET_STYLE_IF(min_height, lv_xml_to_size(value));
    else SET_STYLE_IF(max_height, lv_xml_to_size(value));
    else SET_STYLE_IF(length, lv_xml_to_size(value));
    else SET_STYLE_IF(radius, lv_xml_to_size(value));
    else SET_STYLE_IF(radial_offset, lv_xml_atoi(value));
    else SET_STYLE_IF(align, lv_xml_align_to_enum(value));

    else SET_STYLE_IF(pad_left, lv_xml_atoi(value));
    else SET_STYLE_IF(pad_right, lv_xml_atoi(value));
    else SET_STYLE_IF(pad_top, lv_xml_atoi(value));
    else SET_STYLE_IF(pad_bottom, lv_xml_atoi(value));
    else SET_STYLE_IF(pad_hor, lv_xml_atoi(value));
    else SET_STYLE_IF(pad_ver, lv_xml_atoi(value));
    else SET_STYLE_IF(pad_all, lv_xml_atoi(value));
    else SET_STYLE_IF(pad_row, lv_xml_atoi(value));
    else SET_STYLE_IF(pad_column, lv_xml_atoi(value));
    else SET_STYLE_IF(pad_gap, lv_xml_atoi(value));
    else SET_STYLE_IF(pad_radial, lv_xml_atoi(value));

    else SET_STYLE_IF(margin_left, lv_xml_atoi(value));
    else SET_STYLE_IF(margin_right, lv_xml_atoi(value));
    else SET_STYLE_IF(margin_top, lv_xml_atoi(value));
    else SET_STYLE_IF(margin_bottom, lv_xml_atoi(value));
    else SET_STYLE_IF(margin_hor, lv_xml_atoi(value));
    else SET_STYLE_IF(margin_ver, lv_xml_atoi(value));
    else SET_STYLE_IF(margin_all, lv_xml_atoi(value));

    else SET_STYLE_IF(base_dir, lv_xml_base_dir_to_enum(value));
    else SET_STYLE_IF(clip_corner, lv_xml_to_bool(value));

    else SET_STYLE_IF(bg_opa, lv_xml_to_opa(value));
    else SET_STYLE_IF(bg_color, lv_xml_to_color(value));
    else SET_STYLE_IF(bg_grad_dir, lv_xml_grad_dir_to_enum(value));
    else SET_STYLE_IF(bg_grad_color, lv_xml_to_color(value));
    else SET_STYLE_IF(bg_main_stop, lv_xml_atoi(value));
    else SET_STYLE_IF(bg_grad_stop, lv_xml_atoi(value));
    else SET_STYLE_IF(bg_grad, lv_xml_component_get_grad(&state->scope, value));

    else SET_STYLE_IF(bg_image_src, lv_xml_get_image(&state->scope, value));
    else SET_STYLE_IF(bg_image_tiled, lv_xml_to_bool(value));
    else SET_STYLE_IF(bg_image_recolor, lv_xml_to_color(value));
    else SET_STYLE_IF(bg_image_recolor_opa, lv_xml_to_opa(value));

    else SET_STYLE_IF(border_color, lv_xml_to_color(value));
    else SET_STYLE_IF(border_width, lv_xml_atoi(value));
    else SET_STYLE_IF(border_opa, lv_xml_to_opa(value));
    else SET_STYLE_IF(border_side, lv_xml_border_side_to_enum(value));
    else SET_STYLE_IF(border_post, lv_xml_to_bool(value));

    else SET_STYLE_IF(outline_color, lv_xml_to_color(value));
    else SET_STYLE_IF(outline_width, lv_xml_atoi(value));
    else SET_STYLE_IF(outline_opa, lv_xml_to_opa(value));
    else SET_STYLE_IF(outline_pad, lv_xml_atoi(value));

    else SET_STYLE_IF(shadow_width, lv_xml_atoi(value));
    else SET_STYLE_IF(shadow_color, lv_xml_to_color(value));
    else SET_STYLE_IF(shadow_offset_x, lv_xml_atoi(value));
    else SET_STYLE_IF(shadow_offset_y, lv_xml_atoi(value));
    else SET_STYLE_IF(shadow_spread, lv_xml_atoi(value));
    else SET_STYLE_IF(shadow_opa, lv_xml_to_opa(value));

    else SET_STYLE_IF(text_color, lv_xml_to_color(value));
    else SET_STYLE_IF(text_font, lv_xml_get_font(&state->scope, value));
    else SET_STYLE_IF(text_opa, lv_xml_to_opa(value));
    else SET_STYLE_IF(text_align, lv_xml_text_align_to_enum(value));
    else SET_STYLE_IF(text_letter_space, lv_xml_atoi(value));
    else SET_STYLE_IF(text_line_space, lv_xml_atoi(value));
    else SET_STYLE_IF(text_decor, lv_xml_text_decor_to_enum(value));

    else SET_STYLE_IF(image_opa, lv_xml_to_opa(value));
    else SET_STYLE_IF(image_recolor, lv_xml_to_color(value));
    else SET_STYLE_IF(image_recolor_opa, lv_xml_to_opa(value));

    else SET_STYLE_IF(line_color, lv_xml_to_color(value));
    else SET_STYLE_IF(line_opa, lv_xml_to_opa(value));
    else SET_STYLE_IF(line_width, lv_xml_atoi(value));
    else SET_STYLE_IF(line_dash_width, lv_xml_atoi(value));
    else SET_STYLE_IF(line_dash_gap, lv_xml_atoi(value));
    else SET_STYLE_IF(line_rounded, lv_xml_to_bool(value));

    else SET_STYLE_IF(arc_color, lv_xml_to_color(value));
    else SET_STYLE_IF(arc_opa, lv_xml_to_opa(value));
    else SET_STYLE_IF(arc_width, lv_xml_atoi(value));
    else SET_STYLE_IF(arc_rounded, lv_xml_to_bool(value));
    else SET_STYLE_IF(arc_image_src, lv_xml_get_image(&state->scope, value));

    else SET_STYLE_IF(opa, lv_xml_to_opa(value));
    else SET_STYLE_IF(opa_layered, lv_xml_to_opa(value));
    else SET_STYLE_IF(color_filter_opa, lv_xml_to_opa(value));
    else SET_STYLE_IF(anim_duration, lv_xml_atoi(value));
    else SET_STYLE_IF(blend_mode, lv_xml_blend_mode_to_enum(value));
    else SET_STYLE_IF(transform_width, lv_xml_atoi(value));
    else SET_STYLE_IF(transform_height, lv_xml_atoi(value));
    else SET_STYLE_IF(translate_x, lv_xml_atoi(value));
    else SET_STYLE_IF(translate_y, lv_xml_atoi(value));
    else SET_STYLE_IF(translate_radial, lv_xml_atoi(value));
    else SET_STYLE_IF(transform_scale_x, lv_xml_atoi(value));
    else SET_STYLE_IF(transform_scale_y, lv_xml_atoi(value));
    else SET_STYLE_IF(transform_rotation, lv_xml_atoi(value));
    else SET_STYLE_IF(transform_pivot_x, lv_xml_atoi(value));
    else SET_STYLE_IF(transform_pivot_y, lv_xml_atoi(value));
    else SET_STYLE_IF(transform_skew_x, lv_xml_atoi(value));
    else SET_STYLE_IF(transform_skew_y, lv_xml_atoi(value));
    else SET_STYLE_IF(bitmap_mask_src, lv_xml_get_image(&state->scope, value));
    else SET_STYLE_IF(rotary_sensitivity, lv_xml_atoi(value));
    else SET_STYLE_IF(recolor, lv_xml_to_color(value));
    else SET_STYLE_IF(recolor_opa, lv_xml_to_opa(value));

    else SET_STYLE_IF(layout, lv_xml_layout_to_enum(value));

    else SET_STYLE_IF(flex_flow, lv_xml_flex_flow_to_enum(value));
    else SET_STYLE_IF(flex_grow, lv_xml_atoi(value));
    else SET_STYLE_IF(flex_main_place, lv_xml_flex_align_to_enum(value));
    else SET_STYLE_IF(flex_cross_place, lv_xml_flex_align_to_enum(value));
    else SET_STYLE_IF(flex_track_place, lv_xml_flex_align_to_enum(value));

    else SET_STYLE_IF(grid_column_align, lv_xml_grid_align_to_enum(value));
    else SET_STYLE_IF(grid_row_align, lv_xml_grid_align_to_enum(value));
    else SET_STYLE_IF(grid_cell_column_pos, lv_xml_atoi(value));
    else SET_STYLE_IF(grid_cell_column_span, lv_xml_atoi(value));
    else SET_STYLE_IF(grid_cell_x_align, lv_xml_grid_align_to_enum(value));
    else SET_STYLE_IF(grid_cell_row_pos, lv_xml_atoi(value));
    else SET_STYLE_IF(grid_cell_row_span, lv_xml_atoi(value));
    else SET_STYLE_IF(grid_cell_y_align, lv_xml_grid_align_to_enum(value));
    else if(lv_streq(prop_name, "style_grid_column_dsc_array") ||
            lv_streq(prop_name, "style_grid_row_dsc_array")) {

        uint32_t item_cnt = 0;
        uint32_t i;
        for(i = 0; value[i] != '\0'; i++) {
            if(value[i] == ' ') item_cnt++;
        }

        int32_t * dsc_array = lv_malloc((item_cnt + 2) * sizeof(int32_t)); /*+2 for LV_GRID_TEMPLATE_LAST*/

        char * value_buf = (char *)value;
        item_cnt = 0;
        const char * sub_value = lv_xml_split_str(&value_buf, ' ');
        while(sub_value) {
            if(sub_value[0] == 'f' && sub_value[1] == 'r') {
                dsc_array[item_cnt] = LV_GRID_FR(lv_xml_atoi(sub_value + 3)); /*+3 to skip "fr("*/
            }
            else {
                dsc_array[item_cnt] = lv_xml_atoi(sub_value);
            }

            item_cnt++;
            sub_value = lv_xml_split_str(&value_buf, ' ');
        }

        dsc_array[item_cnt] = LV_GRID_TEMPLATE_LAST;

        lv_obj_add_event_cb(obj, lv_event_free_user_data_cb, LV_EVENT_DELETE, dsc_array);

        if(lv_streq(prop_name, "style_grid_column_dsc_array")) {
            lv_obj_set_style_grid_column_dsc_array(obj, dsc_array, selector);
        }
        else {
            lv_obj_set_style_grid_row_dsc_array(obj, dsc_array, selector);
        }
    }
}


static void screen_create_on_trigger_event_cb(lv_event_t * e)
{
    screen_load_anim_dsc_t * dsc = lv_event_get_user_data(e);
    LV_ASSERT_NULL(dsc);

    lv_obj_t * screen = lv_xml_create(NULL, dsc->screen_name, NULL);
    if(screen == NULL) {
        LV_LOG_WARN("Couldn't create screen `%s`", dsc->screen_name);
        return;
    }
    lv_screen_load_anim(screen, dsc->anim_type, dsc->duration, dsc->delay, false);
    lv_obj_add_event_cb(screen, delete_on_screen_unloaded_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);
    lv_obj_add_event_cb(screen, free_screen_create_user_data_on_delete_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);
}

static void screen_load_on_trigger_event_cb(lv_event_t * e)
{
    screen_load_anim_dsc_t * dsc = lv_event_get_user_data(e);
    LV_ASSERT_NULL(dsc);

    lv_obj_t * screen = lv_display_get_screen_by_name(NULL, dsc->screen_name);
    if(screen == NULL) {
        LV_LOG_WARN("No screen is found with `%s` name", dsc->screen_name);
        return;
    }

    lv_screen_load_anim(screen, dsc->anim_type, dsc->duration, dsc->delay, false);
}

static void delete_on_screen_unloaded_event_cb(lv_event_t * e)
{
    lv_obj_delete(lv_event_get_target_obj(e));
}

static void free_screen_create_user_data_on_delete_event_cb(lv_event_t * e)
{
    screen_load_anim_dsc_t * dsc = lv_event_get_user_data(e);
    lv_free((void *)dsc->screen_name);
    lv_free(dsc);
}

static void play_anim_on_trigger_event_cb(lv_event_t * e)
{
    play_anim_dsc_t * dsc = lv_event_get_user_data(e);
    LV_ASSERT_NULL(dsc);

    lv_obj_t * target;

    if(lv_streq(dsc->target_name, "self")) {
        target = dsc->base_obj;
    }
    else {
        target = lv_obj_find_by_name(dsc->base_obj, dsc->target_name);
    }

    if(target == NULL) {
        LV_LOG_WARN("No target widget is found with `%s` name", dsc->target_name);
        return;
    }

    lv_anim_timeline_t * timeline = NULL;
    lv_anim_timeline_t ** timeline_array = NULL;
    lv_obj_send_event(target, lv_event_xml_store_timeline, &timeline_array);
    if(timeline_array == NULL) {
        LV_LOG_WARN("No time lines are stored in `%s`", dsc->target_name);
        return;
    }

    uint32_t i;
    for(i = 0; timeline_array[i]; i++) {
        const char * name = lv_anim_timeline_get_user_data(timeline_array[i]);
        if(lv_streq(name, dsc->timeline_name)) {
            timeline = timeline_array[i];
            break;
        }
    }

    if(timeline == NULL) {
        LV_LOG_WARN("No timeline is found for `%s` with `%s` name", dsc->target_name, dsc->timeline_name);
        return;
    }

    if(dsc->reverse) {
        lv_anim_timeline_set_reverse(timeline, true);
        lv_anim_timeline_set_progress(timeline, LV_ANIM_TIMELINE_PROGRESS_MAX);
    }
    else {
        lv_anim_timeline_set_reverse(timeline, false);
        lv_anim_timeline_set_progress(timeline, 0);
    }

    lv_anim_timeline_set_delay(timeline, dsc->delay);
    lv_anim_timeline_start(timeline);

}

static void free_play_anim_user_data_on_delete_event_cb(lv_event_t * e)
{
    play_anim_dsc_t * dsc = lv_event_get_user_data(e);
    lv_free((void *)dsc->target_name);
    lv_free((void *)dsc->timeline_name);
    lv_free(dsc);
}

#endif /* LV_USE_XML */
