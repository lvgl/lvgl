
/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_obj_properties.c
 */

#include "../../core/lv_obj.h"

#if LV_USE_OBJ_PROPERTY && LV_USE_OBJ_PROPERTY_NAME


/**
 * Obj widget property names, name must be in order.
 * Generated code from properties.py
 */
/* *INDENT-OFF* */
const lv_property_name_t lv_obj_property_names[74] = {
    {"align",                  LV_PROPERTY_OBJ_ALIGN,},
    {"child_count",            LV_PROPERTY_OBJ_CHILD_COUNT,},
    {"content_height",         LV_PROPERTY_OBJ_CONTENT_HEIGHT,},
    {"content_width",          LV_PROPERTY_OBJ_CONTENT_WIDTH,},
    {"display",                LV_PROPERTY_OBJ_DISPLAY,},
    {"event_count",            LV_PROPERTY_OBJ_EVENT_COUNT,},
    {"ext_draw_size",          LV_PROPERTY_OBJ_EXT_DRAW_SIZE,},
    {"flag_adv_hittest",       LV_PROPERTY_OBJ_FLAG_ADV_HITTEST,},
    {"flag_checkable",         LV_PROPERTY_OBJ_FLAG_CHECKABLE,},
    {"flag_click_focusable",   LV_PROPERTY_OBJ_FLAG_CLICK_FOCUSABLE,},
    {"flag_clickable",         LV_PROPERTY_OBJ_FLAG_CLICKABLE,},
    {"flag_end",               LV_PROPERTY_OBJ_FLAG_END,},
    {"flag_event_bubble",      LV_PROPERTY_OBJ_FLAG_EVENT_BUBBLE,},
    {"flag_event_trickle",     LV_PROPERTY_OBJ_FLAG_EVENT_TRICKLE,},
    {"flag_flex_in_new_track", LV_PROPERTY_OBJ_FLAG_FLEX_IN_NEW_TRACK,},
    {"flag_floating",          LV_PROPERTY_OBJ_FLAG_FLOATING,},
    {"flag_gesture_bubble",    LV_PROPERTY_OBJ_FLAG_GESTURE_BUBBLE,},
    {"flag_hidden",            LV_PROPERTY_OBJ_FLAG_HIDDEN,},
    {"flag_ignore_layout",     LV_PROPERTY_OBJ_FLAG_IGNORE_LAYOUT,},
    {"flag_layout_1",          LV_PROPERTY_OBJ_FLAG_LAYOUT_1,},
    {"flag_layout_2",          LV_PROPERTY_OBJ_FLAG_LAYOUT_2,},
    {"flag_overflow_visible",  LV_PROPERTY_OBJ_FLAG_OVERFLOW_VISIBLE,},
    {"flag_press_lock",        LV_PROPERTY_OBJ_FLAG_PRESS_LOCK,},
    {"flag_radio_button",      LV_PROPERTY_OBJ_FLAG_RADIO_BUTTON,},
    {"flag_scroll_chain_hor",  LV_PROPERTY_OBJ_FLAG_SCROLL_CHAIN_HOR,},
    {"flag_scroll_chain_ver",  LV_PROPERTY_OBJ_FLAG_SCROLL_CHAIN_VER,},
    {"flag_scroll_elastic",    LV_PROPERTY_OBJ_FLAG_SCROLL_ELASTIC,},
    {"flag_scroll_momentum",   LV_PROPERTY_OBJ_FLAG_SCROLL_MOMENTUM,},
    {"flag_scroll_on_focus",   LV_PROPERTY_OBJ_FLAG_SCROLL_ON_FOCUS,},
    {"flag_scroll_one",        LV_PROPERTY_OBJ_FLAG_SCROLL_ONE,},
    {"flag_scroll_with_arrow", LV_PROPERTY_OBJ_FLAG_SCROLL_WITH_ARROW,},
    {"flag_scrollable",        LV_PROPERTY_OBJ_FLAG_SCROLLABLE,},
    {"flag_send_draw_task_events", LV_PROPERTY_OBJ_FLAG_SEND_DRAW_TASK_EVENTS,},
    {"flag_snappable",         LV_PROPERTY_OBJ_FLAG_SNAPPABLE,},
    {"flag_start",             LV_PROPERTY_OBJ_FLAG_START,},
    {"flag_state_trickle",     LV_PROPERTY_OBJ_FLAG_STATE_TRICKLE,},
    {"flag_user_1",            LV_PROPERTY_OBJ_FLAG_USER_1,},
    {"flag_user_2",            LV_PROPERTY_OBJ_FLAG_USER_2,},
    {"flag_widget_1",          LV_PROPERTY_OBJ_FLAG_WIDGET_1,},
    {"flag_widget_2",          LV_PROPERTY_OBJ_FLAG_WIDGET_2,},
    {"h",                      LV_PROPERTY_OBJ_H,},
    {"index",                  LV_PROPERTY_OBJ_INDEX,},
    {"layout",                 LV_PROPERTY_OBJ_LAYOUT,},
    {"parent",                 LV_PROPERTY_OBJ_PARENT,},
    {"screen",                 LV_PROPERTY_OBJ_SCREEN,},
    {"scroll_bottom",          LV_PROPERTY_OBJ_SCROLL_BOTTOM,},
    {"scroll_dir",             LV_PROPERTY_OBJ_SCROLL_DIR,},
    {"scroll_end",             LV_PROPERTY_OBJ_SCROLL_END,},
    {"scroll_left",            LV_PROPERTY_OBJ_SCROLL_LEFT,},
    {"scroll_right",           LV_PROPERTY_OBJ_SCROLL_RIGHT,},
    {"scroll_snap_x",          LV_PROPERTY_OBJ_SCROLL_SNAP_X,},
    {"scroll_snap_y",          LV_PROPERTY_OBJ_SCROLL_SNAP_Y,},
    {"scroll_top",             LV_PROPERTY_OBJ_SCROLL_TOP,},
    {"scroll_x",               LV_PROPERTY_OBJ_SCROLL_X,},
    {"scroll_y",               LV_PROPERTY_OBJ_SCROLL_Y,},
    {"scrollbar_mode",         LV_PROPERTY_OBJ_SCROLLBAR_MODE,},
    {"state_any",              LV_PROPERTY_OBJ_STATE_ANY,},
    {"state_checked",          LV_PROPERTY_OBJ_STATE_CHECKED,},
    {"state_disabled",         LV_PROPERTY_OBJ_STATE_DISABLED,},
    {"state_edited",           LV_PROPERTY_OBJ_STATE_EDITED,},
    {"state_end",              LV_PROPERTY_OBJ_STATE_END,},
    {"state_focus_key",        LV_PROPERTY_OBJ_STATE_FOCUS_KEY,},
    {"state_focused",          LV_PROPERTY_OBJ_STATE_FOCUSED,},
    {"state_hovered",          LV_PROPERTY_OBJ_STATE_HOVERED,},
    {"state_pressed",          LV_PROPERTY_OBJ_STATE_PRESSED,},
    {"state_scrolled",         LV_PROPERTY_OBJ_STATE_SCROLLED,},
    {"state_start",            LV_PROPERTY_OBJ_STATE_START,},
    {"state_user_1",           LV_PROPERTY_OBJ_STATE_USER_1,},
    {"state_user_2",           LV_PROPERTY_OBJ_STATE_USER_2,},
    {"state_user_3",           LV_PROPERTY_OBJ_STATE_USER_3,},
    {"state_user_4",           LV_PROPERTY_OBJ_STATE_USER_4,},
    {"w",                      LV_PROPERTY_OBJ_W,},
    {"x",                      LV_PROPERTY_OBJ_X,},
    {"y",                      LV_PROPERTY_OBJ_Y,},
};
/* *INDENT-ON* */
#endif
