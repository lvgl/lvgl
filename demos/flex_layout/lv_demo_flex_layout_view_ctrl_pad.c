/**
 * @file lv_demo_flex_layout_view_ctrl_pad.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_flex_layout_main.h"

#if LV_USE_DEMO_FLEX_LAYOUT

/*********************
 *      DEFINES
 *********************/

#define DDLIST_ALIGN_CREATE_DEF(item)   \
    do {                                \
        ui->ctrl_pad.tab.align.ddlist_align_##item = \
                                                     ddlist_create(tab,                     \
                                                                   "Align "#item" place",   \
                                                                   "start\n"                \
                                                                   "end\n"                  \
                                                                   "center\n"               \
                                                                   "space evenly\n"         \
                                                                   "space around\n"         \
                                                                   "space between");        \
    } while(0)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void tab_flex_create(lv_obj_t * tab, view_t * ui);
static void tab_align_create(lv_obj_t * tab, view_t * ui);
static void tab_layout_create(lv_obj_t * tab, view_t * ui);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void view_ctrl_pad_create(lv_obj_t * par, view_t * ui)
{
    lv_obj_t * tv = lv_tabview_create(par);
    lv_tabview_set_tab_bar_size(tv, 50);
    lv_obj_set_width(tv, lv_pct(100));
    lv_obj_set_flex_grow(tv, 1);
    lv_obj_set_style_radius(tv, 0, 0);
    lv_obj_set_style_bg_color(tv, lv_color_hex(0xffffff), 0);

    lv_obj_t * buttons = lv_tabview_get_tab_bar(tv);
    lv_obj_set_style_outline_width(buttons, 0,  LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_width(buttons, 0,   LV_STATE_FOCUS_KEY);
    lv_obj_set_style_bg_color(buttons, lv_color_white(), LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_height(buttons, 40);

    ui->ctrl_pad.tab.view = tv;

    ui->ctrl_pad.tab.flex.tab = lv_tabview_add_tab(tv, "Flex");
    ui->ctrl_pad.tab.align.tab = lv_tabview_add_tab(tv, "Align");
    ui->ctrl_pad.tab.layout.tab = lv_tabview_add_tab(tv, "Layout");

    tab_flex_create(ui->ctrl_pad.tab.flex.tab, ui);
    tab_align_create(ui->ctrl_pad.tab.align.tab, ui);
    tab_layout_create(ui->ctrl_pad.tab.layout.tab, ui);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * ddlist_create(lv_obj_t * par, const char * name, const char * options)
{
    lv_obj_t * cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(80), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, name);

    lv_obj_t * ddlist = lv_dropdown_create(cont);
    lv_obj_set_width(ddlist, lv_pct(100));
    lv_dropdown_set_options_static(ddlist, options);
    return ddlist;
}

static void tab_flex_create(lv_obj_t * tab, view_t * ui)
{
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        tab,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );

    ui->ctrl_pad.tab.flex.ddlist_flow = ddlist_create(
                                            tab,
                                            "Flow",
                                            "Row\n"
                                            "Column\n"
                                            "Row wrap\n"
                                            "Row reverse\n"
                                            "Row wrap reverse\n"
                                            "Column wrap\n"
                                            "Column reverse\n"
                                            "Column wrap reverse"
                                        );

    lv_obj_t * cb = lv_checkbox_create(tab);
    lv_checkbox_set_text(cb, "Scrollable");
    ui->ctrl_pad.tab.flex.checkbox_scrl = cb;
}

static void tab_align_create(lv_obj_t * tab, view_t * ui)
{
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        tab,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );

    DDLIST_ALIGN_CREATE_DEF(main);
    DDLIST_ALIGN_CREATE_DEF(cross);
    DDLIST_ALIGN_CREATE_DEF(track);
}

static void btn_inc_event_handler(lv_event_t * e)
{
    lv_obj_t * spinbox = lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(spinbox);
    }
}

static void btn_dec_event_handler(lv_event_t * e)
{
    lv_obj_t * spinbox = lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_decrement(spinbox);
    }
}

static lv_obj_t * spinbox_ctrl_create(lv_obj_t * par, lv_style_prop_t prop, lv_obj_t ** cont_)
{
    lv_obj_t * cont_main = par;
    lv_obj_t * label;

    lv_obj_t * cont_spinbox = lv_obj_create(cont_main);
    if(cont_) *cont_ = cont_spinbox;
    lv_obj_remove_style_all(cont_spinbox);
    lv_obj_set_height(cont_spinbox, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(cont_spinbox, 1);
    lv_obj_set_style_radius(cont_spinbox, 5, LV_PART_MAIN);
    lv_obj_set_style_clip_corner(cont_spinbox, true, LV_PART_MAIN);
    lv_obj_set_style_outline_width(cont_spinbox, 2, LV_PART_MAIN);
    lv_obj_set_style_outline_color(cont_spinbox, lv_color_hex3(0xddd), LV_PART_MAIN);

    lv_obj_set_flex_flow(cont_spinbox, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(cont_spinbox, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN);

    static lv_style_t btn_style;
    if(btn_style.prop_cnt < 1) {
        lv_style_init(&btn_style);
        lv_style_set_radius(&btn_style, 0);
        lv_style_set_outline_width(&btn_style, 1);
        lv_style_set_outline_pad(&btn_style, 1);
        lv_style_set_outline_color(&btn_style, lv_color_hex3(0xddd));
        lv_style_set_shadow_width(&btn_style, 0);
        lv_style_set_bg_color(&btn_style, lv_color_white());
        lv_style_set_text_color(&btn_style, lv_theme_get_color_primary(par));
    }

    lv_obj_t * btn_dec = lv_button_create(cont_spinbox);
    lv_obj_set_width(btn_dec, 30);
    lv_obj_add_style(btn_dec, &btn_style, LV_PART_MAIN);
    label = lv_label_create(btn_dec);
    lv_label_set_text(label, "-");
    lv_obj_center(label);

    lv_obj_t * spinbox = lv_spinbox_create(cont_spinbox);
    lv_obj_set_flex_grow(spinbox, 1);
    lv_obj_set_style_shadow_width(spinbox, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(spinbox, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(spinbox, 1, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(spinbox, 1, LV_PART_MAIN);
    lv_obj_set_style_outline_color(spinbox, lv_color_hex3(0xddd), LV_PART_MAIN);
    lv_obj_set_style_radius(spinbox, 0, LV_PART_MAIN);
    lv_obj_set_user_data(spinbox, (void *)(lv_uintptr_t)prop);
    lv_spinbox_set_range(spinbox, LV_COORD_MIN, LV_COORD_MAX);
    lv_spinbox_set_digit_format(spinbox, 3, 0);
    lv_spinbox_step_prev(spinbox);

    lv_obj_t * btn_inc = lv_button_create(cont_spinbox);
    lv_obj_set_width(btn_inc, 30);
    lv_obj_add_style(btn_inc, &btn_style, LV_PART_MAIN);
    label = lv_label_create(btn_inc);
    lv_label_set_text(label, "+");
    lv_obj_center(label);

    lv_obj_add_event_cb(btn_inc, btn_inc_event_handler, LV_EVENT_ALL, spinbox);
    lv_obj_add_event_cb(btn_dec, btn_dec_event_handler, LV_EVENT_ALL, spinbox);

    return spinbox;
}

static void tab_layout_create(lv_obj_t * tab, view_t * ui)
{
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(tab, 5, LV_PART_MAIN);
    lv_obj_set_flex_align(
        tab,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );

    static lv_style_t group_style;
    lv_style_init(&group_style);
    lv_style_set_pad_all(&group_style, 2);
    lv_style_set_border_width(&group_style, 0);
    lv_style_set_width(&group_style, LV_PCT(100));
    lv_style_set_height(&group_style, LV_SIZE_CONTENT);

    lv_obj_t * label = lv_label_create(tab);
    lv_obj_set_style_pad_top(label, 10, LV_PART_MAIN);
    lv_checkbox_set_text(label, "WIDTH x HEIGHT");

    lv_obj_t * temp_group = lv_obj_create(tab);
    ui->ctrl_pad.tab.layout.group_width_and_height = temp_group;
    lv_obj_add_style(temp_group, &group_style, LV_PART_MAIN);
    lv_obj_set_flex_flow(temp_group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(temp_group, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    ui->ctrl_pad.tab.layout.spinbox_width = spinbox_ctrl_create(temp_group, LV_STYLE_WIDTH, NULL);
    ui->ctrl_pad.tab.layout.spinbox_height = spinbox_ctrl_create(temp_group, LV_STYLE_HEIGHT, NULL);

    label = lv_label_create(tab);
    lv_obj_set_style_pad_top(label, 10, LV_PART_MAIN);
    lv_checkbox_set_text(label, "MIN-WIDTH x MIN-HEIGHT");

    temp_group = lv_obj_create(tab);
    ui->ctrl_pad.tab.layout.group_width_and_height_min = temp_group;
    lv_obj_add_style(temp_group, &group_style, LV_PART_MAIN);
    lv_obj_set_flex_flow(temp_group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(temp_group, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    ui->ctrl_pad.tab.layout.spinbox_min_width = spinbox_ctrl_create(temp_group, LV_STYLE_MIN_WIDTH, NULL);
    ui->ctrl_pad.tab.layout.spinbox_min_height = spinbox_ctrl_create(temp_group, LV_STYLE_MIN_HEIGHT, NULL);

    label = lv_label_create(tab);
    lv_obj_set_style_pad_top(label, 10, LV_PART_MAIN);
    lv_checkbox_set_text(label, "MAX-WIDTH x MAX-HEIGHT");

    temp_group = lv_obj_create(tab);
    ui->ctrl_pad.tab.layout.group_width_and_height_max = temp_group;
    lv_obj_add_style(temp_group, &group_style, LV_PART_MAIN);
    lv_obj_set_flex_flow(temp_group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(temp_group, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    ui->ctrl_pad.tab.layout.spinbox_max_width = spinbox_ctrl_create(temp_group, LV_STYLE_MAX_WIDTH, NULL);
    ui->ctrl_pad.tab.layout.spinbox_max_height = spinbox_ctrl_create(temp_group, LV_STYLE_MAX_HEIGHT, NULL);

    temp_group = lv_obj_create(tab);
    ui->ctrl_pad.tab.layout.group_width_and_height_max = temp_group;
    lv_obj_add_style(temp_group, &group_style, LV_PART_MAIN);
    lv_obj_set_style_pad_top(temp_group, 20, LV_PART_MAIN);
    lv_obj_set_flex_flow(temp_group, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(temp_group, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * temp_cont;
    ui->ctrl_pad.tab.layout.spinbox_pad_top = spinbox_ctrl_create(temp_group, LV_STYLE_PAD_TOP, &temp_cont);
    lv_obj_set_style_width(ui->ctrl_pad.tab.layout.spinbox_pad_top, 30, LV_PART_MAIN);
    lv_obj_set_flex_grow(temp_cont, 0);
    ui->ctrl_pad.tab.layout.spinbox_pad_left = spinbox_ctrl_create(temp_group, LV_STYLE_PAD_LEFT, &temp_cont);
    lv_obj_add_flag(temp_cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    label = lv_label_create(temp_group);
    lv_checkbox_set_text(label, "PADDING");
    ui->ctrl_pad.tab.layout.spinbox_pad_right = spinbox_ctrl_create(temp_group, LV_STYLE_PAD_RIGHT, NULL);
    ui->ctrl_pad.tab.layout.spinbox_pad_bottom = spinbox_ctrl_create(temp_group, LV_STYLE_PAD_BOTTOM, &temp_cont);
    lv_obj_set_flex_grow(temp_cont, 0);
    lv_obj_add_flag(temp_cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    label = lv_label_create(tab);
    lv_obj_set_style_pad_top(label, 20, LV_PART_MAIN);
    lv_checkbox_set_text(label, "PAD: COL x ROW x GROW");

    temp_group = lv_obj_create(tab);
    ui->ctrl_pad.tab.layout.group_width_and_height_max = temp_group;
    lv_obj_add_style(temp_group, &group_style, LV_PART_MAIN);
    lv_obj_set_flex_flow(temp_group, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(temp_group, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    ui->ctrl_pad.tab.layout.spinbox_pad_column = spinbox_ctrl_create(temp_group, LV_STYLE_PAD_COLUMN, NULL);
    ui->ctrl_pad.tab.layout.spinbox_pad_row = spinbox_ctrl_create(temp_group, LV_STYLE_PAD_ROW, NULL);
    ui->ctrl_pad.tab.layout.spinbox_flex_grow = spinbox_ctrl_create(temp_group, LV_STYLE_FLEX_GROW, NULL);
}

#endif
