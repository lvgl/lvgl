/**
 * @file lv_xml_base_types.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"
#if LV_USE_XML

#include "lv_xml_base_types.h"
#include "lv_xml_private.h"
#include "lv_xml_parser.h"
#include "lv_xml_style.h"
#include "lv_xml_component_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_state_t lv_xml_state_to_enum(const char * txt)
{
    if(lv_streq("default", txt)) return LV_STATE_DEFAULT;
    if(lv_streq("pressed", txt)) return LV_STATE_PRESSED;
    if(lv_streq("checked", txt)) return LV_STATE_CHECKED;
    if(lv_streq("hovered", txt)) return LV_STATE_HOVERED;
    if(lv_streq("scrolled", txt)) return LV_STATE_SCROLLED;
    if(lv_streq("disabled", txt)) return LV_STATE_DISABLED;
    if(lv_streq("focused", txt)) return LV_STATE_FOCUSED;
    if(lv_streq("focus_key", txt)) return LV_STATE_FOCUS_KEY;
    if(lv_streq("edited", txt)) return LV_STATE_EDITED;
    if(lv_streq("user_1", txt)) return LV_STATE_USER_1;
    if(lv_streq("user_2", txt)) return LV_STATE_USER_2;
    if(lv_streq("user_3", txt)) return LV_STATE_USER_3;
    if(lv_streq("user_4", txt)) return LV_STATE_USER_4;

    LV_LOG_WARN("%s is an unknown value for state", txt);
    return 0; /*Return 0 in lack of a better option. */
}

int32_t lv_xml_to_size(const char * txt)
{
    if(lv_streq(txt, "content")) return LV_SIZE_CONTENT;

    int32_t v = lv_xml_atoi(txt);
    if(txt[lv_strlen(txt) - 1] == '%') return lv_pct(v);
    else return v;
}

lv_align_t lv_xml_align_to_enum(const char * txt)
{
    if(lv_streq("top_left", txt)) return LV_ALIGN_TOP_LEFT;
    if(lv_streq("top_mid", txt)) return LV_ALIGN_TOP_MID;
    if(lv_streq("top_right", txt)) return LV_ALIGN_TOP_RIGHT;
    if(lv_streq("bottom_left", txt)) return LV_ALIGN_BOTTOM_LEFT;
    if(lv_streq("bottom_mid", txt)) return LV_ALIGN_BOTTOM_MID;
    if(lv_streq("bottom_right", txt)) return LV_ALIGN_BOTTOM_RIGHT;
    if(lv_streq("right_mid", txt)) return LV_ALIGN_RIGHT_MID;
    if(lv_streq("left_mid", txt)) return LV_ALIGN_LEFT_MID;
    if(lv_streq("center", txt)) return LV_ALIGN_CENTER;

    LV_LOG_WARN("%s is an unknown value for align", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_dir_t lv_xml_dir_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_DIR_NONE;
    if(lv_streq("top", txt)) return LV_DIR_TOP;
    if(lv_streq("bottom", txt)) return LV_DIR_BOTTOM;
    if(lv_streq("left", txt)) return LV_DIR_LEFT;
    if(lv_streq("right", txt)) return LV_DIR_RIGHT;
    if(lv_streq("hor", txt)) return LV_DIR_HOR;
    if(lv_streq("ver", txt)) return LV_DIR_VER;
    if(lv_streq("all", txt)) return LV_DIR_ALL;

    LV_LOG_WARN("%s is an unknown value for dir", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_border_side_t lv_xml_border_side_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_BORDER_SIDE_NONE;
    if(lv_streq("top", txt)) return LV_BORDER_SIDE_TOP;
    if(lv_streq("bottom", txt)) return LV_BORDER_SIDE_BOTTOM;
    if(lv_streq("left", txt)) return LV_BORDER_SIDE_LEFT;
    if(lv_streq("right", txt)) return LV_BORDER_SIDE_RIGHT;
    if(lv_streq("full", txt)) return LV_BORDER_SIDE_FULL;

    LV_LOG_WARN("%s is an unknown value for border_side", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_grad_dir_t lv_xml_grad_dir_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_GRAD_DIR_NONE;
    if(lv_streq("hor", txt)) return LV_GRAD_DIR_HOR;
    if(lv_streq("ver", txt)) return LV_GRAD_DIR_VER;

    LV_LOG_WARN("%s is an unknown value for grad_dir", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_base_dir_t lv_xml_base_dir_to_enum(const char * txt)
{
    if(lv_streq("auto", txt)) return LV_BASE_DIR_AUTO;
    if(lv_streq("ltr", txt)) return LV_BASE_DIR_LTR;
    if(lv_streq("rtl", txt)) return LV_BASE_DIR_RTL;

    LV_LOG_WARN("%s is an unknown value for base_dir", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_text_align_t lv_xml_text_align_to_enum(const char * txt)
{
    if(lv_streq("left", txt)) return LV_TEXT_ALIGN_LEFT;
    if(lv_streq("right", txt)) return LV_TEXT_ALIGN_RIGHT;
    if(lv_streq("center", txt)) return LV_TEXT_ALIGN_CENTER;
    if(lv_streq("auto", txt)) return LV_TEXT_ALIGN_AUTO;

    LV_LOG_WARN("%s is an unknown value for text_align", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_text_decor_t lv_xml_text_decor_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_TEXT_DECOR_NONE;
    if(lv_streq("underline", txt)) return LV_TEXT_DECOR_UNDERLINE;
    if(lv_streq("strikethrough", txt)) return LV_TEXT_DECOR_STRIKETHROUGH;

    LV_LOG_WARN("%s is an unknown value for text_decor", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_scroll_snap_t lv_xml_scroll_snap_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_SCROLL_SNAP_NONE;
    if(lv_streq("start", txt)) return LV_SCROLL_SNAP_START;
    if(lv_streq("center", txt)) return LV_SCROLL_SNAP_CENTER;
    if(lv_streq("end", txt)) return LV_SCROLL_SNAP_END;

    LV_LOG_WARN("%s is an unknown value for scroll_snap", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_flex_flow_t lv_xml_flex_flow_to_enum(const char * txt)
{
    if(lv_streq("column", txt)) return LV_FLEX_FLOW_COLUMN;
    if(lv_streq("column_reverse", txt)) return LV_FLEX_FLOW_COLUMN_REVERSE;
    if(lv_streq("column_wrap", txt)) return LV_FLEX_FLOW_COLUMN_WRAP;
    if(lv_streq("column_wrap_reverse", txt)) return LV_FLEX_FLOW_COLUMN_WRAP_REVERSE;
    if(lv_streq("row", txt)) return LV_FLEX_FLOW_ROW;
    if(lv_streq("row_reverse", txt)) return LV_FLEX_FLOW_ROW_REVERSE;
    if(lv_streq("row_wrap", txt)) return LV_FLEX_FLOW_ROW_WRAP;
    if(lv_streq("row_wrap_reverse", txt)) return LV_FLEX_FLOW_ROW_WRAP_REVERSE;

    LV_LOG_WARN("%s is an unknown value for flex_flow", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_flex_align_t lv_xml_flex_align_to_enum(const char * txt)
{
    if(lv_streq("center", txt)) return LV_FLEX_ALIGN_CENTER;
    if(lv_streq("end", txt)) return LV_FLEX_ALIGN_END;
    if(lv_streq("start", txt)) return LV_FLEX_ALIGN_START;
    if(lv_streq("space_around", txt)) return LV_FLEX_ALIGN_SPACE_AROUND;
    if(lv_streq("space_between", txt)) return LV_FLEX_ALIGN_SPACE_BETWEEN;
    if(lv_streq("space_evenly", txt)) return LV_FLEX_ALIGN_SPACE_EVENLY;

    LV_LOG_WARN("%s is an unknown value for flex_align", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_grid_align_t lv_xml_grid_align_to_enum(const char * txt)
{
    if(lv_streq("center", txt)) return LV_GRID_ALIGN_CENTER;
    if(lv_streq("end", txt)) return LV_GRID_ALIGN_END;
    if(lv_streq("start", txt)) return LV_GRID_ALIGN_START;
    if(lv_streq("stretch", txt)) return LV_GRID_ALIGN_STRETCH;
    if(lv_streq("space_around", txt)) return LV_GRID_ALIGN_SPACE_AROUND;
    if(lv_streq("space_between", txt)) return LV_GRID_ALIGN_SPACE_BETWEEN;
    if(lv_streq("space_evenly", txt)) return LV_GRID_ALIGN_SPACE_EVENLY;

    LV_LOG_WARN("%s is an unknown value for grid_align", txt);
    return 0; /*Return 0 in lack of a better option. */
}


lv_layout_t lv_xml_layout_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_LAYOUT_NONE;
    if(lv_streq("flex", txt)) return LV_LAYOUT_FLEX;
    if(lv_streq("grid", txt)) return LV_LAYOUT_GRID;

    LV_LOG_WARN("%s is an unknown value for layout", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_blend_mode_t lv_xml_blend_mode_to_enum(const char * txt)
{
    if(lv_streq("normal", txt)) return LV_BLEND_MODE_NORMAL;
    if(lv_streq("additive", txt)) return LV_BLEND_MODE_ADDITIVE;
    if(lv_streq("subtractive", txt)) return LV_BLEND_MODE_SUBTRACTIVE;
    if(lv_streq("multiply", txt)) return LV_BLEND_MODE_MULTIPLY;
    if(lv_streq("difference", txt)) return LV_BLEND_MODE_DIFFERENCE;

    LV_LOG_WARN("%s is an unknown value for blend_mode", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_event_code_t lv_xml_trigger_text_to_enum_value(const char * txt)
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


lv_screen_load_anim_t lv_xml_screen_load_anim_text_to_enum_value(const char * txt)
{
    if(lv_streq("none", txt)) return LV_SCREEN_LOAD_ANIM_NONE;
    if(lv_streq("over_left", txt)) return LV_SCREEN_LOAD_ANIM_OVER_LEFT;
    if(lv_streq("over_right", txt)) return LV_SCREEN_LOAD_ANIM_OVER_RIGHT;
    if(lv_streq("over_top", txt)) return LV_SCREEN_LOAD_ANIM_OVER_TOP;
    if(lv_streq("over_bottom", txt)) return LV_SCREEN_LOAD_ANIM_OVER_BOTTOM;
    if(lv_streq("move_left", txt)) return LV_SCREEN_LOAD_ANIM_MOVE_LEFT;
    if(lv_streq("move_right", txt)) return LV_SCREEN_LOAD_ANIM_MOVE_RIGHT;
    if(lv_streq("move_top", txt)) return LV_SCREEN_LOAD_ANIM_MOVE_TOP;
    if(lv_streq("move_bottom", txt)) return LV_SCREEN_LOAD_ANIM_MOVE_BOTTOM;
    if(lv_streq("fade_in", txt)) return LV_SCREEN_LOAD_ANIM_FADE_IN;
    if(lv_streq("fade_on", txt)) return LV_SCREEN_LOAD_ANIM_FADE_ON;
    if(lv_streq("fade_out", txt)) return LV_SCREEN_LOAD_ANIM_FADE_OUT;
    if(lv_streq("out_left", txt)) return LV_SCREEN_LOAD_ANIM_OUT_LEFT;
    if(lv_streq("out_right", txt)) return LV_SCREEN_LOAD_ANIM_OUT_RIGHT;
    if(lv_streq("out_top", txt)) return LV_SCREEN_LOAD_ANIM_OUT_TOP;
    if(lv_streq("out_bottom", txt)) return LV_SCREEN_LOAD_ANIM_OUT_BOTTOM;

    LV_LOG_WARN("%s is an unknown value for screen_load_anim", txt);
    return LV_SCREEN_LOAD_ANIM_NONE;
}

lv_style_prop_t lv_xml_style_prop_to_enum(const char * txt)
{
    if(lv_streq(txt, "width")) return LV_STYLE_WIDTH;
    if(lv_streq(txt, "min_width")) return LV_STYLE_MIN_WIDTH;
    if(lv_streq(txt, "max_width")) return LV_STYLE_MAX_WIDTH;
    else if(lv_streq(txt, "height")) return LV_STYLE_HEIGHT;
    else if(lv_streq(txt, "min_height")) return LV_STYLE_MIN_HEIGHT;
    else if(lv_streq(txt, "max_height")) return LV_STYLE_MAX_HEIGHT;
    else if(lv_streq(txt, "length")) return LV_STYLE_LENGTH;
    else if(lv_streq(txt, "radius")) return LV_STYLE_RADIUS;
    else if(lv_streq(txt, "radial_offset")) return LV_STYLE_RADIAL_OFFSET;
    else if(lv_streq(txt, "align")) return LV_STYLE_ALIGN;

    else if(lv_streq(txt, "pad_left")) return LV_STYLE_PAD_LEFT;
    else if(lv_streq(txt, "pad_right")) return LV_STYLE_PAD_RIGHT;
    else if(lv_streq(txt, "pad_top")) return LV_STYLE_PAD_TOP;
    else if(lv_streq(txt, "pad_bottom")) return LV_STYLE_PAD_BOTTOM;
    else if(lv_streq(txt, "pad_row")) return LV_STYLE_PAD_ROW;
    else if(lv_streq(txt, "pad_column")) return LV_STYLE_PAD_COLUMN;
    else if(lv_streq(txt, "pad_radial")) return LV_STYLE_PAD_RADIAL;

    else if(lv_streq(txt, "margin_left")) return LV_STYLE_MARGIN_LEFT;
    else if(lv_streq(txt, "margin_right")) return LV_STYLE_MARGIN_RIGHT;
    else if(lv_streq(txt, "margin_top")) return LV_STYLE_MARGIN_TOP;
    else if(lv_streq(txt, "margin_bottom")) return LV_STYLE_MARGIN_BOTTOM;

    else if(lv_streq(txt, "base_dir")) return LV_STYLE_BASE_DIR;
    else if(lv_streq(txt, "clip_corner")) return LV_STYLE_CLIP_CORNER;

    else if(lv_streq(txt, "bg_opa")) return LV_STYLE_BG_OPA;
    else if(lv_streq(txt, "bg_color")) return LV_STYLE_BG_COLOR;
    else if(lv_streq(txt, "bg_grad_dir")) return LV_STYLE_BG_GRAD_DIR;
    else if(lv_streq(txt, "bg_grad_color")) return LV_STYLE_BG_GRAD_COLOR;
    else if(lv_streq(txt, "bg_main_stop")) return LV_STYLE_BG_MAIN_STOP;
    else if(lv_streq(txt, "bg_grad_stop")) return LV_STYLE_BG_GRAD_STOP;
    else if(lv_streq(txt, "bg_grad")) return LV_STYLE_BG_GRAD;

    else if(lv_streq(txt, "bg_image_src")) return LV_STYLE_BG_IMAGE_SRC;
    else if(lv_streq(txt, "bg_image_tiled")) return LV_STYLE_BG_IMAGE_TILED;
    else if(lv_streq(txt, "bg_image_recolor")) return LV_STYLE_BG_IMAGE_RECOLOR;
    else if(lv_streq(txt, "bg_image_recolor_opa")) return LV_STYLE_BG_IMAGE_RECOLOR_OPA;

    else if(lv_streq(txt, "border_color")) return LV_STYLE_BORDER_COLOR;
    else if(lv_streq(txt, "border_width")) return LV_STYLE_BORDER_WIDTH;
    else if(lv_streq(txt, "border_opa")) return LV_STYLE_BORDER_OPA;
    else if(lv_streq(txt, "border_side")) return LV_STYLE_BORDER_SIDE;
    else if(lv_streq(txt, "border_post")) return LV_STYLE_BORDER_POST;

    else if(lv_streq(txt, "outline_color")) return LV_STYLE_OUTLINE_COLOR;
    else if(lv_streq(txt, "outline_width")) return LV_STYLE_OUTLINE_WIDTH;
    else if(lv_streq(txt, "outline_opa")) return LV_STYLE_OUTLINE_OPA;
    else if(lv_streq(txt, "outline_pad")) return LV_STYLE_OUTLINE_PAD;

    else if(lv_streq(txt, "shadow_width")) return LV_STYLE_SHADOW_WIDTH;
    else if(lv_streq(txt, "shadow_color")) return LV_STYLE_SHADOW_COLOR;
    else if(lv_streq(txt, "shadow_offset_x")) return LV_STYLE_SHADOW_OFFSET_X;
    else if(lv_streq(txt, "shadow_offset_y")) return LV_STYLE_SHADOW_OFFSET_Y;
    else if(lv_streq(txt, "shadow_spread")) return LV_STYLE_SHADOW_SPREAD;
    else if(lv_streq(txt, "shadow_opa")) return LV_STYLE_SHADOW_OPA;

    else if(lv_streq(txt, "text_color")) return LV_STYLE_TEXT_COLOR;
    else if(lv_streq(txt, "text_font")) return LV_STYLE_TEXT_FONT;
    else if(lv_streq(txt, "text_opa")) return LV_STYLE_TEXT_OPA;
    else if(lv_streq(txt, "text_align")) return LV_STYLE_TEXT_ALIGN;
    else if(lv_streq(txt, "text_letter_space")) return LV_STYLE_TEXT_LETTER_SPACE;
    else if(lv_streq(txt, "text_line_space")) return LV_STYLE_TEXT_LINE_SPACE;
    else if(lv_streq(txt, "text_decor")) return LV_STYLE_TEXT_DECOR;

    else if(lv_streq(txt, "image_opa")) return LV_STYLE_IMAGE_OPA;
    else if(lv_streq(txt, "image_recolor")) return LV_STYLE_IMAGE_RECOLOR;
    else if(lv_streq(txt, "image_recolor_opa")) return LV_STYLE_IMAGE_RECOLOR_OPA;

    else if(lv_streq(txt, "line_color")) return LV_STYLE_LINE_COLOR;
    else if(lv_streq(txt, "line_opa")) return LV_STYLE_LINE_OPA;
    else if(lv_streq(txt, "line_width")) return LV_STYLE_LINE_WIDTH;
    else if(lv_streq(txt, "line_dash_width")) return LV_STYLE_LINE_DASH_WIDTH;
    else if(lv_streq(txt, "line_dash_gap")) return LV_STYLE_LINE_DASH_GAP;
    else if(lv_streq(txt, "line_rounded")) return LV_STYLE_LINE_ROUNDED;

    else if(lv_streq(txt, "arc_color")) return LV_STYLE_ARC_COLOR;
    else if(lv_streq(txt, "arc_opa")) return LV_STYLE_ARC_OPA;
    else if(lv_streq(txt, "arc_width")) return LV_STYLE_ARC_WIDTH;
    else if(lv_streq(txt, "arc_rounded")) return LV_STYLE_ARC_ROUNDED;
    else if(lv_streq(txt, "arc_image_src")) return LV_STYLE_ARC_IMAGE_SRC;

    else if(lv_streq(txt, "opa")) return LV_STYLE_OPA;
    else if(lv_streq(txt, "opa_layered")) return LV_STYLE_OPA_LAYERED;
    else if(lv_streq(txt, "color_filter_opa")) return LV_STYLE_COLOR_FILTER_OPA;
    else if(lv_streq(txt, "anim_duration")) return LV_STYLE_ANIM_DURATION;
    else if(lv_streq(txt, "blend_mode")) return LV_STYLE_BLEND_MODE;
    else if(lv_streq(txt, "transform_width")) return LV_STYLE_TRANSFORM_WIDTH;
    else if(lv_streq(txt, "transform_height")) return LV_STYLE_TRANSFORM_HEIGHT;
    else if(lv_streq(txt, "translate_x")) return LV_STYLE_TRANSLATE_X;
    else if(lv_streq(txt, "translate_y")) return LV_STYLE_TRANSLATE_Y;
    else if(lv_streq(txt, "translate_radial")) return LV_STYLE_TRANSLATE_RADIAL;
    else if(lv_streq(txt, "transform_scale_x")) return LV_STYLE_TRANSFORM_SCALE_X;
    else if(lv_streq(txt, "transform_scale_y")) return LV_STYLE_TRANSFORM_SCALE_Y;
    else if(lv_streq(txt, "transform_rotation")) return LV_STYLE_TRANSFORM_ROTATION;
    else if(lv_streq(txt, "transform_pivot_x")) return LV_STYLE_TRANSFORM_PIVOT_X;
    else if(lv_streq(txt, "transform_pivot_y")) return LV_STYLE_TRANSFORM_PIVOT_Y;
    else if(lv_streq(txt, "transform_skew_x")) return LV_STYLE_TRANSFORM_SKEW_X;
    else if(lv_streq(txt, "transform_skew_y")) return LV_STYLE_TRANSFORM_SKEW_Y;
    else if(lv_streq(txt, "bitmap_mask_src")) return LV_STYLE_BITMAP_MASK_SRC;
    else if(lv_streq(txt, "rotary_sensitivity")) return LV_STYLE_ROTARY_SENSITIVITY;
    else if(lv_streq(txt, "recolor")) return LV_STYLE_RECOLOR;
    else if(lv_streq(txt, "recolor_opa")) return LV_STYLE_RECOLOR_OPA;

    else if(lv_streq(txt, "layout")) return LV_STYLE_LAYOUT;

    else if(lv_streq(txt, "flex_flow")) return LV_STYLE_FLEX_FLOW;
    else if(lv_streq(txt, "flex_grow")) return LV_STYLE_FLEX_GROW;
    else if(lv_streq(txt, "flex_main_place")) return LV_STYLE_FLEX_MAIN_PLACE;
    else if(lv_streq(txt, "flex_cross_place")) return LV_STYLE_FLEX_CROSS_PLACE;
    else if(lv_streq(txt, "flex_track_place")) return LV_STYLE_FLEX_TRACK_PLACE;

    else if(lv_streq(txt, "grid_column_align")) return LV_STYLE_GRID_COLUMN_ALIGN;
    else if(lv_streq(txt, "grid_row_align")) return LV_STYLE_GRID_ROW_ALIGN;
    else if(lv_streq(txt, "grid_cell_column_pos")) return LV_STYLE_GRID_CELL_COLUMN_POS;
    else if(lv_streq(txt, "grid_cell_column_span")) return LV_STYLE_GRID_CELL_COLUMN_SPAN;
    else if(lv_streq(txt, "grid_cell_x_align")) return LV_STYLE_GRID_CELL_X_ALIGN;
    else if(lv_streq(txt, "grid_cell_row_pos")) return LV_STYLE_GRID_CELL_ROW_POS;
    else if(lv_streq(txt, "grid_cell_row_span")) return LV_STYLE_GRID_CELL_ROW_SPAN;
    else if(lv_streq(txt, "grid_cell_y_align")) return LV_STYLE_GRID_CELL_Y_ALIGN;

    return LV_STYLE_PROP_INV;
}

lv_state_t lv_xml_style_state_to_enum(const char * txt)
{
    if(lv_streq("default", txt)) return LV_STATE_DEFAULT;
    else if(lv_streq("pressed", txt)) return LV_STATE_PRESSED;
    else if(lv_streq("checked", txt)) return LV_STATE_CHECKED;
    else if(lv_streq("scrolled", txt)) return LV_STATE_SCROLLED;
    else if(lv_streq("focused", txt)) return LV_STATE_FOCUSED;
    else if(lv_streq("focus_key", txt)) return LV_STATE_FOCUS_KEY;
    else if(lv_streq("edited", txt)) return LV_STATE_EDITED;
    else if(lv_streq("hovered", txt)) return LV_STATE_HOVERED;
    else if(lv_streq("disabled", txt)) return LV_STATE_DISABLED;
    else if(lv_streq("user_1", txt)) return LV_STATE_USER_1;
    else if(lv_streq("user_2", txt)) return LV_STATE_USER_2;
    else if(lv_streq("user_3", txt)) return LV_STATE_USER_3;
    else if(lv_streq("user_4", txt)) return LV_STATE_USER_4;

    return 0; /*Return 0 in lack of a better option. */
}

lv_part_t lv_xml_style_part_to_enum(const char * txt)
{
    if(lv_streq("main", txt)) return LV_PART_MAIN;
    else if(lv_streq("scrollbar", txt)) return LV_PART_SCROLLBAR;
    else if(lv_streq("indicator", txt)) return LV_PART_INDICATOR;
    else if(lv_streq("knob", txt)) return LV_PART_KNOB;
    else if(lv_streq("selected", txt)) return LV_PART_SELECTED;
    else if(lv_streq("items", txt)) return LV_PART_ITEMS;
    else if(lv_streq("cursor", txt)) return LV_PART_CURSOR;

    return 0; /*Return 0 in lack of a better option. */
}

lv_style_selector_t lv_xml_style_selector_text_to_enum(const char * str)
{
    if(str == NULL) return 0;
    lv_style_selector_t selector = 0;
    char buf[256];
    lv_strncpy(buf, str, sizeof(buf));

    char * bufp = buf;
    const char * next = lv_xml_split_str(&bufp, '|');

    while(next) {
        /* Handle different states and parts */
        selector |= lv_xml_style_state_to_enum(next);
        selector |= lv_xml_style_part_to_enum(next);

        /* Move to the next token */
        next = lv_xml_split_str(&bufp, '|');
    }

    return selector;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
