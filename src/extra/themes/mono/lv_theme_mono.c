/**
 * @file lv_theme_mono.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_THEME_MONO

#include "lv_theme_mono.h"
#include "../../../misc/lv_gc.h"

/*********************
 *      DEFINES
 *********************/

#define COLOR_FG      dark_bg ? lv_color_white() : lv_color_black()
#define COLOR_BG      dark_bg ? lv_color_black() : lv_color_white()

#define BORDER_W_NORMAL  1
#define BORDER_W_PR      3
#define BORDER_W_DIS     0
#define BORDER_W_FOCUS   1
#define BORDER_W_EDIT    2
#define PAD_DEF          4

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_style_t scr;
    lv_style_t card;
    lv_style_t scrollbar;
    lv_style_t btn;
    lv_style_t pr;
    lv_style_t inv;
    lv_style_t disabled;
    lv_style_t focus;
    lv_style_t edit;
    lv_style_t pad_gap;
    lv_style_t pad_zero;
    lv_style_t no_radius;
    lv_style_t radius_circle;
    lv_style_t large_border;
    lv_style_t large_line_space;
    lv_style_t underline;
#if LV_USE_TEXTAREA
    lv_style_t ta_cursor;
#endif
} my_theme_styles_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void style_init_reset(lv_style_t * style);
static void theme_apply(lv_theme_t * th, lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
static my_theme_styles_t * styles;
static lv_theme_t theme;
static bool inited;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void style_init(bool dark_bg, const lv_font_t * font)
{
    style_init_reset(&styles->scrollbar);
    lv_style_set_bg_opa(&styles->scrollbar, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->scrollbar, COLOR_FG);
    lv_style_set_width(&styles->scrollbar,  PAD_DEF);

    style_init_reset(&styles->scr);
    lv_style_set_bg_opa(&styles->scr, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->scr, COLOR_BG);
    lv_style_set_text_color(&styles->scr, COLOR_FG);
    lv_style_set_pad_row(&styles->scr, PAD_DEF);
    lv_style_set_pad_column(&styles->scr, PAD_DEF);
    lv_style_set_text_font(&styles->scr, font);

    style_init_reset(&styles->card);
    lv_style_set_bg_opa(&styles->card, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->card, COLOR_BG);
    lv_style_set_border_color(&styles->card, COLOR_FG);
    lv_style_set_radius(&styles->card, 2);
    lv_style_set_border_width(&styles->card, BORDER_W_NORMAL);
    lv_style_set_pad_all(&styles->card, PAD_DEF);
    lv_style_set_pad_gap(&styles->card, PAD_DEF);
    lv_style_set_text_color(&styles->card, COLOR_FG);
    lv_style_set_line_width(&styles->card, 2);
    lv_style_set_line_color(&styles->card, COLOR_FG);
    lv_style_set_arc_width(&styles->card, 2);
    lv_style_set_arc_color(&styles->card, COLOR_FG);
    lv_style_set_outline_color(&styles->card, COLOR_FG);
    lv_style_set_anim_time(&styles->card, 300);

    style_init_reset(&styles->pr);
    lv_style_set_border_width(&styles->pr, BORDER_W_PR);

    style_init_reset(&styles->inv);
    lv_style_set_bg_opa(&styles->inv, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->inv, COLOR_FG);
    lv_style_set_border_color(&styles->inv, COLOR_BG);
    lv_style_set_line_color(&styles->inv, COLOR_BG);
    lv_style_set_arc_color(&styles->inv, COLOR_BG);
    lv_style_set_text_color(&styles->inv, COLOR_BG);
    lv_style_set_outline_color(&styles->inv, COLOR_BG);

    style_init_reset(&styles->disabled);
    lv_style_set_border_width(&styles->disabled, BORDER_W_DIS);

    style_init_reset(&styles->focus);
    lv_style_set_outline_width(&styles->focus, 1);
    lv_style_set_outline_pad(&styles->focus, BORDER_W_FOCUS);

    style_init_reset(&styles->edit);
    lv_style_set_outline_width(&styles->edit, BORDER_W_EDIT);

    style_init_reset(&styles->large_border);
    lv_style_set_border_width(&styles->large_border, BORDER_W_EDIT);

    style_init_reset(&styles->pad_gap);
    lv_style_set_pad_gap(&styles->pad_gap, PAD_DEF);

    style_init_reset(&styles->pad_zero);
    lv_style_set_pad_all(&styles->pad_zero, 0);
    lv_style_set_pad_gap(&styles->pad_zero, 0);

    style_init_reset(&styles->no_radius);
    lv_style_set_radius(&styles->no_radius, 0);

    style_init_reset(&styles->radius_circle);
    lv_style_set_radius(&styles->radius_circle, LV_RADIUS_CIRCLE);

    style_init_reset(&styles->large_line_space);
    lv_style_set_text_line_space(&styles->large_line_space, 6);

    style_init_reset(&styles->underline);
    lv_style_set_text_decor(&styles->underline, LV_TEXT_DECOR_UNDERLINE);

#if LV_USE_TEXTAREA
    style_init_reset(&styles->ta_cursor);
    lv_style_set_border_side(&styles->ta_cursor, LV_BORDER_SIDE_LEFT);
    lv_style_set_border_color(&styles->ta_cursor, COLOR_FG);
    lv_style_set_border_width(&styles->ta_cursor, 2);
    lv_style_set_bg_opa(&styles->ta_cursor, LV_OPA_TRANSP);
    lv_style_set_anim_time(&styles->ta_cursor, 500);
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_theme_mono_is_inited(void)
{
    return  LV_GC_ROOT(_lv_theme_default_styles) == NULL ? false : true;
}

lv_theme_t * lv_theme_mono_init(lv_disp_t * disp, bool dark_bg, const lv_font_t * font)
{

    /*This trick is required only to avoid the garbage collection of
     *styles' data if LVGL is used in a binding (e.g. Micropython)
     *In a general case styles could be in simple `static lv_style_t my_style...` variables*/
    if(!inited) {
        inited = false;
        LV_GC_ROOT(_lv_theme_default_styles) = lv_mem_alloc(sizeof(my_theme_styles_t));
        styles = (my_theme_styles_t *)LV_GC_ROOT(_lv_theme_default_styles);
    }

    theme.disp = disp;
    theme.font_small = LV_FONT_DEFAULT;
    theme.font_normal = LV_FONT_DEFAULT;
    theme.font_large = LV_FONT_DEFAULT;
    theme.apply_cb = theme_apply;

    style_init(dark_bg, font);

    if(disp == NULL || lv_disp_get_theme(disp) == &theme) lv_obj_report_style_change(NULL);

    inited = true;

    return (lv_theme_t *)&theme;
}

static void theme_apply(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);

    if(lv_obj_get_parent(obj) == NULL) {
        lv_obj_add_style(obj, &styles->scr, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        return;
    }

    if(lv_obj_check_type(obj, &lv_obj_class)) {
#if LV_USE_TABVIEW
        lv_obj_t * parent = lv_obj_get_parent(obj);
        /*Tabview content area*/
        if(lv_obj_check_type(parent, &lv_tabview_class)) {
            return;
        }
        /*Tabview pages*/
        else if(lv_obj_check_type(lv_obj_get_parent(parent), &lv_tabview_class)) {
            lv_obj_add_style(obj, &styles->card, 0);
            lv_obj_add_style(obj, &styles->no_radius, 0);
            lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
            return;
        }
#endif

#if LV_USE_WIN
        /*Header*/
        if(lv_obj_get_index(obj) == 0 && lv_obj_check_type(lv_obj_get_parent(obj), &lv_win_class)) {
            lv_obj_add_style(obj, &styles->card, 0);
            lv_obj_add_style(obj, &styles->no_radius, 0);
            return;
        }
        /*Content*/
        else if(lv_obj_get_index(obj) == 1 && lv_obj_check_type(lv_obj_get_parent(obj), &lv_win_class)) {
            lv_obj_add_style(obj, &styles->card, 0);
            lv_obj_add_style(obj, &styles->no_radius, 0);
            lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
            return;
        }
#endif
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
    }
#if LV_USE_BTN
    else if(lv_obj_check_type(obj, &lv_btn_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->pr, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->inv, LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->disabled, LV_STATE_DISABLED);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_BTNMATRIX
    else if(lv_obj_check_type(obj, &lv_btnmatrix_class)) {
#if LV_USE_MSGBOX
        if(lv_obj_check_type(lv_obj_get_parent(obj), &lv_msgbox_class)) {
            lv_obj_add_style(obj, &styles->pad_gap, 0);
            lv_obj_add_style(obj, &styles->card, LV_PART_ITEMS);
            lv_obj_add_style(obj, &styles->pr, LV_PART_ITEMS | LV_STATE_PRESSED);
            lv_obj_add_style(obj, &styles->disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
            lv_obj_add_style(obj, &styles->underline, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &styles->large_border, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            return;
        }
#endif
#if LV_USE_TABVIEW
        if(lv_obj_check_type(lv_obj_get_parent(obj), &lv_tabview_class)) {
            lv_obj_add_style(obj, &styles->pad_gap, 0);
            lv_obj_add_style(obj, &styles->card, LV_PART_ITEMS);
            lv_obj_add_style(obj, &styles->pr, LV_PART_ITEMS | LV_STATE_PRESSED);
            lv_obj_add_style(obj, &styles->inv, LV_PART_ITEMS | LV_STATE_CHECKED);
            lv_obj_add_style(obj, &styles->disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
            lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &styles->underline, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &styles->large_border, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            return;
        }
#endif
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->card, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->pr, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->inv, LV_PART_ITEMS | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &styles->underline, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->large_border, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
    }
#endif

#if LV_USE_BAR
    else if(lv_obj_check_type(obj, &lv_bar_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->pad_zero, 0);
        lv_obj_add_style(obj, &styles->inv, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
    }
#endif

#if LV_USE_SLIDER
    else if(lv_obj_check_type(obj, &lv_slider_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->pad_zero, 0);
        lv_obj_add_style(obj, &styles->inv, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->card, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->radius_circle, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_TABLE
    else if(lv_obj_check_type(obj, &lv_table_class)) {
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->card, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->no_radius, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->pr, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->inv, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_CHECKBOX
    else if(lv_obj_check_type(obj, &lv_checkbox_class)) {
        lv_obj_add_style(obj, &styles->pad_gap, LV_PART_MAIN);
        lv_obj_add_style(obj, &styles->card, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->disabled, LV_PART_INDICATOR | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &styles->inv, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->pr, LV_PART_INDICATOR | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_SWITCH
    else if(lv_obj_check_type(obj, &lv_switch_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->radius_circle, 0);
        lv_obj_add_style(obj, &styles->pad_zero, 0);
        lv_obj_add_style(obj, &styles->inv, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->radius_circle, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->card, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->radius_circle, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->pad_zero, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_CHART
    else if(lv_obj_check_type(obj, &lv_chart_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->card, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->card, LV_PART_TICKS);
        lv_obj_add_style(obj, &styles->card, LV_PART_CURSOR);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
    }
#endif

#if LV_USE_ROLLER
    else if(lv_obj_check_type(obj, &lv_roller_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->large_line_space, 0);
        lv_obj_add_style(obj, &styles->inv, LV_PART_SELECTED);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_DROPDOWN
    else if(lv_obj_check_type(obj, &lv_dropdown_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->pr, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
    }
    else if(lv_obj_check_type(obj, &lv_dropdownlist_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->large_line_space, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->inv, LV_PART_SELECTED | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->pr, LV_PART_SELECTED | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_ARC
    else if(lv_obj_check_type(obj, &lv_arc_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->inv, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->pad_zero, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->card, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->radius_circle, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_METER
    else if(lv_obj_check_type(obj, &lv_meter_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
    }
#endif

#if LV_USE_TEXTAREA
    else if(lv_obj_check_type(obj, &lv_textarea_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->ta_cursor, LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUSED);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_CALENDAR
    else if(lv_obj_check_type(obj, &lv_calendar_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->no_radius, 0);
        lv_obj_add_style(obj, &styles->pr, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->large_border, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
    }
#endif

#if LV_USE_KEYBOARD
    else if(lv_obj_check_type(obj, &lv_keyboard_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->card, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->pr, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->inv, LV_PART_ITEMS | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->large_border, LV_PART_ITEMS | LV_STATE_EDITED);
    }
#endif
#if LV_USE_LIST
    else if(lv_obj_check_type(obj, &lv_list_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        return;
    }
    else if(lv_obj_check_type(obj, &lv_list_text_class)) {

    }
    else if(lv_obj_check_type(obj, &lv_list_btn_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->pr, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->large_border, LV_STATE_EDITED);

    }
#endif
#if LV_USE_MSGBOX
    else if(lv_obj_check_type(obj, &lv_msgbox_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        return;
    }
#endif
#if LV_USE_SPINBOX
    else if(lv_obj_check_type(obj, &lv_spinbox_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->inv, LV_PART_CURSOR);
        lv_obj_add_style(obj, &styles->focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->edit, LV_STATE_EDITED);
    }
#endif
#if LV_USE_TILEVIEW
    else if(lv_obj_check_type(obj, &lv_tileview_class)) {
        lv_obj_add_style(obj, &styles->scr, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
    }
    else if(lv_obj_check_type(obj, &lv_tileview_tile_class)) {
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
    }
#endif

#if LV_USE_LED
    else if(lv_obj_check_type(obj, &lv_led_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
    }
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void style_init_reset(lv_style_t * style)
{
    if(inited) {
        lv_style_reset(style);
    }
    else {
        lv_style_init(style);
    }
}

#endif
