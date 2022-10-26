/**
 * @file lv_demo_layout_previewer_view_ctrl_pad.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_layout_previewer_main.h"

/*********************
 *      DEFINES
 *********************/

#define ROLLER_ALIGN_CREATE_DEF(item)   \
    do {                                \
        ui->ctrl_pad.tab.align.roller_align_##item = \
                                                     roller_create(tab,                            \
                                                                   "Align "#item" place",          \
                                                                   "FLEX_ALIGN_START\n"            \
                                                                   "FLEX_ALIGN_END\n"              \
                                                                   "FLEX_ALIGN_CENTER\n"           \
                                                                   "FLEX_ALIGN_SPACE_EVENLY\n"     \
                                                                   "FLEX_ALIGN_SPACE_AROUND\n"     \
                                                                   "FLEX_ALIGN_SPACE_BETWEEN"      \
                                                                  );                               \
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
    lv_obj_t * tv = lv_tabview_create(par, LV_DIR_TOP, 50);
    lv_obj_set_width(tv, lv_pct(100));
    lv_obj_set_flex_grow(tv, 1);
    lv_obj_set_style_radius(tv, 0, 0);
    lv_obj_set_style_bg_color(tv, lv_color_hex(0xffffff), 0);
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

static lv_obj_t * roller_create(lv_obj_t * par, const char * name, const char * options)
{
    lv_obj_t * cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_width(cont, lv_pct(80));
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, name);

    lv_obj_t * roller = lv_roller_create(cont);
    lv_obj_set_width(roller, lv_pct(100));
    lv_roller_set_options(roller, options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller, 3);
    return roller;
}

static void tab_flex_create(lv_obj_t * tab, view_t * ui)
{
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        tab,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_SPACE_AROUND
    );

    ui->ctrl_pad.tab.flex.roller_flow = roller_create(
                                            tab,
                                            "Flow",
                                            "FLEX_FLOW_ROW\n"
                                            "FLEX_FLOW_COLUMN\n"
                                            "FLEX_FLOW_ROW_WRAP\n"
                                            "FLEX_FLOW_ROW_REVERSE\n"
                                            "FLEX_FLOW_ROW_WRAP_REVERSE\n"
                                            "FLEX_FLOW_COLUMN_WRAP\n"
                                            "FLEX_FLOW_COLUMN_REVERSE\n"
                                            "FLEX_FLOW_COLUMN_WRAP_REVERSE"
                                        );

    lv_obj_t * cb = lv_checkbox_create(tab);
    lv_checkbox_set_text(cb, "SCROLLABLE");
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

    ROLLER_ALIGN_CREATE_DEF(main);
    ROLLER_ALIGN_CREATE_DEF(cross);
    ROLLER_ALIGN_CREATE_DEF(track);
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

static lv_obj_t * spinbox_ctrl_create(lv_obj_t * par, const char * txt, lv_style_prop_t prop)
{
    lv_obj_t * cont_main = lv_obj_create(par);

    lv_obj_set_width(cont_main, lv_pct(100));
    lv_obj_set_flex_flow(cont_main, LV_FLEX_FLOW_COLUMN);

    lv_obj_clear_flag(cont_main, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * label = lv_label_create(cont_main);
    lv_label_set_text(label, txt);

    lv_obj_t * cont_spinbox = lv_obj_create(cont_main);
    lv_obj_remove_style_all(cont_spinbox);
    lv_obj_set_width(cont_spinbox, lv_pct(100));
    lv_obj_set_flex_flow(cont_spinbox, LV_FLEX_FLOW_ROW);

    lv_obj_t * btn_inc = lv_btn_create(cont_spinbox);
    label = lv_label_create(btn_inc);
    lv_label_set_text(label, "+");

    lv_obj_t * spinbox = lv_spinbox_create(cont_spinbox);
    lv_obj_set_width(spinbox, lv_pct(50));
    lv_obj_set_user_data(spinbox, (void *)(lv_uintptr_t)prop);
    lv_spinbox_set_range(spinbox, LV_COORD_MIN, LV_COORD_MAX);
    lv_spinbox_set_digit_format(spinbox, 4, 0);
    lv_spinbox_step_prev(spinbox);

    lv_obj_t * btn_dec = lv_btn_create(cont_spinbox);
    label = lv_label_create(btn_dec);
    lv_label_set_text(label, "-");

    lv_obj_add_event_cb(btn_inc, btn_inc_event_handler, LV_EVENT_ALL, spinbox);
    lv_obj_add_event_cb(btn_dec, btn_dec_event_handler, LV_EVENT_ALL, spinbox);

    return spinbox;
}

static void tab_layout_create(lv_obj_t * tab, view_t * ui)
{
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        tab,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );

    ui->ctrl_pad.tab.layout.spinbox_width = spinbox_ctrl_create(tab, "width", LV_STYLE_WIDTH);
    ui->ctrl_pad.tab.layout.spinbox_height = spinbox_ctrl_create(tab, "height", LV_STYLE_HEIGHT);
    ui->ctrl_pad.tab.layout.spinbox_pad_top = spinbox_ctrl_create(tab, "pad-top", LV_STYLE_PAD_TOP);
    ui->ctrl_pad.tab.layout.spinbox_pad_bottom = spinbox_ctrl_create(tab, "pad-bottom", LV_STYLE_PAD_BOTTOM);
    ui->ctrl_pad.tab.layout.spinbox_pad_left = spinbox_ctrl_create(tab, "pad-left", LV_STYLE_PAD_LEFT);
    ui->ctrl_pad.tab.layout.spinbox_pad_right = spinbox_ctrl_create(tab, "pad-right", LV_STYLE_PAD_RIGHT);
    ui->ctrl_pad.tab.layout.spinbox_pad_column = spinbox_ctrl_create(tab, "pad-column", LV_STYLE_PAD_COLUMN);
    ui->ctrl_pad.tab.layout.spinbox_pad_row = spinbox_ctrl_create(tab, "pad-row", LV_STYLE_PAD_ROW);
    ui->ctrl_pad.tab.layout.spinbox_flex_grow = spinbox_ctrl_create(tab, "flex-grow", LV_STYLE_FLEX_GROW);
}
