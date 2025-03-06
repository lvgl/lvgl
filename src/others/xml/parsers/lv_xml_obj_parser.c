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
#if LV_USE_OBJ_NAME
        if(lv_streq("name", name)) lv_obj_set_name(item, value);
#endif
        if(lv_streq("x", name)) lv_obj_set_x(item, lv_xml_to_size(value));
        else if(lv_streq("y", name)) lv_obj_set_y(item, lv_xml_to_size(value));
        else if(lv_streq("width", name)) lv_obj_set_width(item, lv_xml_to_size(value));
        else if(lv_streq("height", name)) lv_obj_set_height(item, lv_xml_to_size(value));
        else if(lv_streq("align", name)) lv_obj_set_align(item, lv_xml_align_to_enum(value));
        else if(lv_streq("flex_flow", name)) lv_obj_set_flex_flow(item, lv_xml_flex_flow_to_enum(value));
        else if(lv_streq("flex_grow", name)) lv_obj_set_flex_grow(item, lv_xml_atoi(value));

        else if(lv_streq("hidden", name))               lv_obj_update_flag(item, LV_OBJ_FLAG_HIDDEN, lv_xml_to_bool(value));
        else if(lv_streq("clickable", name))            lv_obj_update_flag(item, LV_OBJ_FLAG_CLICKABLE, lv_xml_to_bool(value));
        else if(lv_streq("click_focusable", name))      lv_obj_update_flag(item, LV_OBJ_FLAG_CLICK_FOCUSABLE,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("checkable", name))            lv_obj_update_flag(item, LV_OBJ_FLAG_CHECKABLE, lv_xml_to_bool(value));
        else if(lv_streq("scrollable", name))           lv_obj_update_flag(item, LV_OBJ_FLAG_SCROLLABLE, lv_xml_to_bool(value));
        else if(lv_streq("scroll_elastic", name))       lv_obj_update_flag(item, LV_OBJ_FLAG_SCROLL_ELASTIC,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("scroll_momentum", name))      lv_obj_update_flag(item, LV_OBJ_FLAG_SCROLL_MOMENTUM,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("scroll_one", name))           lv_obj_update_flag(item, LV_OBJ_FLAG_SCROLL_ONE, lv_xml_to_bool(value));
        else if(lv_streq("scroll_chain_hor", name))     lv_obj_update_flag(item, LV_OBJ_FLAG_SCROLL_CHAIN_HOR,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("scroll_chain_ver", name))     lv_obj_update_flag(item, LV_OBJ_FLAG_SCROLL_CHAIN_VER,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("scroll_chain", name))         lv_obj_update_flag(item, LV_OBJ_FLAG_SCROLL_CHAIN,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("scroll_on_focus", name))      lv_obj_update_flag(item, LV_OBJ_FLAG_SCROLL_ON_FOCUS,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("scroll_with_arrow", name))    lv_obj_update_flag(item, LV_OBJ_FLAG_SCROLL_WITH_ARROW,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("snappable", name))            lv_obj_update_flag(item, LV_OBJ_FLAG_SNAPPABLE, lv_xml_to_bool(value));
        else if(lv_streq("press_lock", name))           lv_obj_update_flag(item, LV_OBJ_FLAG_PRESS_LOCK, lv_xml_to_bool(value));
        else if(lv_streq("event_bubble", name))         lv_obj_update_flag(item, LV_OBJ_FLAG_EVENT_BUBBLE,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("gesture_bubble", name))       lv_obj_update_flag(item, LV_OBJ_FLAG_GESTURE_BUBBLE,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("adv_hittest", name))          lv_obj_update_flag(item, LV_OBJ_FLAG_ADV_HITTEST,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("ignore_layout", name))        lv_obj_update_flag(item, LV_OBJ_FLAG_IGNORE_LAYOUT,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("floating", name))             lv_obj_update_flag(item, LV_OBJ_FLAG_FLOATING, lv_xml_to_bool(value));
        else if(lv_streq("send_draw_task_events", name))lv_obj_update_flag(item, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("overflow_visible", name))     lv_obj_update_flag(item, LV_OBJ_FLAG_OVERFLOW_VISIBLE,
                                                                               lv_xml_to_bool(value));
        else if(lv_streq("flex_in_new_track", name))    lv_obj_update_flag(item, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK,
                                                                               lv_xml_to_bool(value));

        else if(lv_streq("styles", name)) lv_xml_style_add_to_obj(state, item, value);

        else if(lv_strlen(name) > 6 && lv_memcmp("style_", name, 6) == 0) {
            apply_styles(state, item, name, value);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void apply_styles(lv_xml_parser_state_t * state, lv_obj_t * obj, const char * name, const char * value)
{
    char name_local[512];
    lv_strlcpy(name_local, name, sizeof(name_local));

    lv_style_selector_t selector;
    const char * prop_name = lv_xml_style_string_process(name_local, &selector);

    SET_STYLE_IF(width, lv_xml_to_size(value));
    else SET_STYLE_IF(height, lv_xml_to_size(value));
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
    else SET_STYLE_IF(bg_grad, lv_xml_component_get_grad(&state->ctx, value));

    else SET_STYLE_IF(bg_image_src, lv_xml_get_image(&state->ctx, value));
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
    else SET_STYLE_IF(text_font, lv_xml_get_font(&state->ctx, value));
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
    else SET_STYLE_IF(arc_image_src, lv_xml_get_image(&state->ctx, value));

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
    else SET_STYLE_IF(bitmap_mask_src, lv_xml_get_image(&state->ctx, value));
    else SET_STYLE_IF(rotary_sensitivity, lv_xml_atoi(value));

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
