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

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_flag_t flag_to_enum(const char * txt);
static void apply_styles(lv_xml_parser_state_t * state, lv_obj_t * obj, const char * name, const char * value);

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
        else if(lv_streq("styles", name)) lv_xml_style_add_to_obj(state, item, value);

        else if(lv_streq("bind_checked", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                lv_obj_bind_checked(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in lv_obj bind_checked", value);
            }
        }
        else if(name_len >= 15 && lv_memcmp("bind_flag_if_", name, 13) == 0) {
            lv_observer_t * (*cb)(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value) = NULL;
            if(name[13] == 'e' && name[14] == 'q') cb = lv_obj_bind_flag_if_eq;
            else if(name[13] == 'n' && name[14] == 'o') cb = lv_obj_bind_flag_if_not_eq;
            else if(name[13] == 'g' && name[14] == 't') cb = lv_obj_bind_flag_if_gt;
            else if(name[13] == 'g' && name[14] == 'e') cb = lv_obj_bind_flag_if_ge;
            else if(name[13] == 'l' && name[14] == 't') cb = lv_obj_bind_flag_if_lt;
            else if(name[13] == 'l' && name[14] == 'e') cb = lv_obj_bind_flag_if_le;

            if(cb) {
                char buf[128];
                lv_strlcpy(buf, value, sizeof(buf));
                char * bufp = buf;
                const char * subject_str =  lv_xml_split_str(&bufp, ' ');
                const char * flag_str =  lv_xml_split_str(&bufp, ' ');
                const char * ref_value_str =  lv_xml_split_str(&bufp, ' ');

                if(subject_str == NULL) {
                    LV_LOG_WARN("Subject is missing in lv_obj bind_flag");
                }
                else if(flag_str == NULL) {
                    LV_LOG_WARN("Flag is missing in lv_obj bind_flag");
                }
                else if(ref_value_str == NULL) {
                    LV_LOG_WARN("Reference value is missing in lv_obj bind_flag");
                }
                else {
                    lv_subject_t * subject = lv_xml_get_subject(&state->scope, subject_str);
                    if(subject == NULL) {
                        LV_LOG_WARN("Subject \"%s\" doesn't exist in lv_obj bind_flag", value);
                    }
                    else {
                        lv_obj_flag_t flag = flag_to_enum(flag_str);
                        int32_t ref_value = lv_xml_atoi(ref_value_str);
                        cb(item, subject, flag, ref_value);
                    }
                }
            }
        }
        else if(name_len >= 16 && lv_memcmp("bind_state_if_", name, 14) == 0) {
            lv_observer_t * (*cb)(lv_obj_t * obj, lv_subject_t * subject, lv_state_t flag, int32_t ref_value) = NULL;
            if(name[14] == 'e' && name[15] == 'q') cb = lv_obj_bind_state_if_eq;
            else if(name[14] == 'n' && name[15] == 'o') cb = lv_obj_bind_state_if_not_eq;
            else if(name[14] == 'g' && name[15] == 't') cb = lv_obj_bind_state_if_gt;
            else if(name[14] == 'g' && name[15] == 'e') cb = lv_obj_bind_state_if_ge;
            else if(name[14] == 'l' && name[15] == 't') cb = lv_obj_bind_state_if_lt;
            else if(name[14] == 'l' && name[15] == 'e') cb = lv_obj_bind_state_if_le;

            if(cb) {
                char buf[128];
                lv_strlcpy(buf, value, sizeof(buf));
                char * bufp = buf;
                const char * subject_str =  lv_xml_split_str(&bufp, ' ');
                const char * state_str =  lv_xml_split_str(&bufp, ' ');
                const char * ref_value_str =  lv_xml_split_str(&bufp, ' ');

                if(subject_str == NULL) {
                    LV_LOG_WARN("Subject is missing in lv_obj bind_state");
                }
                else if(state_str == NULL) {
                    LV_LOG_WARN("State is missing in lv_obj bind_state");
                }
                else if(ref_value_str == NULL) {
                    LV_LOG_WARN("Reference value is missing in lv_obj bind_state");
                }
                else {
                    lv_subject_t * subject = lv_xml_get_subject(&state->scope, subject_str);
                    if(subject == NULL) {
                        LV_LOG_WARN("Subject \"%s\" doesn't exist in lv_obj bind_state", value);
                    }
                    else {
                        lv_state_t obj_state = lv_xml_state_to_enum(state_str);
                        int32_t ref_value = lv_xml_atoi(ref_value_str);
                        cb(item, subject, obj_state, ref_value);
                    }
                }
            }
        }

        else if(name_len > 6 && lv_memcmp("style_", name, 6) == 0) {
            apply_styles(state, item, name, value);
        }
    }
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
}


#endif /* LV_USE_XML */
