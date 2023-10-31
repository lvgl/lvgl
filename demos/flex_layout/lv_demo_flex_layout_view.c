/**
 * @file lv_demo_flex_layout_view.c
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

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_obj_t * btn_create(lv_obj_t * par, const char * str, lv_color_t color);
static void obj_child_node_def_style_init(lv_style_t * style);
static void obj_child_node_checked_style_init(lv_style_t * style);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void view_create(lv_obj_t * par, view_t * ui)
{
    /* layout */
    lv_obj_set_flex_flow(par, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(
        par,
        LV_FLEX_ALIGN_SPACE_AROUND,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_SPACE_AROUND
    );

    /* style */
    obj_child_node_def_style_init(&ui->obj_def_style);
    obj_child_node_checked_style_init(&ui->obj_checked_style);

    /* root */
    ui->root = obj_child_node_create(par, ui);
    lv_obj_set_size(ui->root, lv_pct(50), lv_pct(80));

    /* mian ctrl_pad */
    lv_obj_t * obj = lv_obj_create(par);
    lv_obj_set_size(obj, lv_pct(40), lv_pct(80));
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_pad_gap(obj, 0, 0);
    lv_obj_set_style_radius(obj, 10, 0);
    lv_obj_set_style_clip_corner(obj, true, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_shadow_color(obj, lv_color_hex3(0xaaa), 0);
    lv_obj_set_style_shadow_width(obj, 20, 0);
    lv_obj_set_style_shadow_offset_y(obj, 2, 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        obj,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );
    ui->ctrl_pad.cont = obj;

    /* tabview */
    view_ctrl_pad_create(ui->ctrl_pad.cont, ui);

    /* btn_cont */
    obj = lv_obj_create(ui->ctrl_pad.cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_border_width(obj, 2, 0);
    lv_obj_set_style_border_color(obj, lv_palette_lighten(LV_PALETTE_GREY, 2), 0);
    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_ver(obj, 16, 0);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(
        obj,
        LV_FLEX_ALIGN_SPACE_AROUND,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );
    ui->ctrl_pad.btn.cont = obj;
    ui->ctrl_pad.btn.add = btn_create(obj, "Add", lv_palette_main(LV_PALETTE_BLUE_GREY));
    ui->ctrl_pad.btn.remove = btn_create(obj, "Remove", lv_palette_main(LV_PALETTE_RED));

    lv_obj_send_event(ui->root, LV_EVENT_CLICKED, NULL); /*Make it active by default*/

    /* fade effect */
    lv_obj_fade_in(ui->root, 600, 0);
    lv_obj_fade_in(ui->ctrl_pad.cont, 600, 300);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * btn_create(lv_obj_t * par, const char * str, lv_color_t color)
{
    lv_obj_t * btn = lv_button_create(par);
    lv_obj_set_width(btn, lv_pct(30));
    lv_obj_set_height(btn, 30);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, color, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 5, LV_PART_MAIN);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, str);
    lv_obj_center(label);

    return btn;
}

static void obj_child_node_def_style_init(lv_style_t * style)
{
    lv_style_init(style);
    lv_style_set_size(style, LV_PCT(45), LV_PCT(45));
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_radius(style, 0);
}

static void obj_child_node_checked_style_init(lv_style_t * style)
{
    lv_style_init(style);
    lv_style_set_border_color(style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_shadow_color(style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_width(style, 20);

    static lv_style_transition_dsc_t tran;
    static const lv_style_prop_t prop[] = {
        LV_STYLE_SHADOW_OPA,
        LV_STYLE_BORDER_COLOR,
        LV_STYLE_PROP_INV
    };
    lv_style_transition_dsc_init(&tran, prop, lv_anim_path_ease_out, 300, 0, NULL);
    lv_style_set_transition(style, &tran);
}

#endif
