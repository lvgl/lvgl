/**
 * @file lv_theme_mono.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_theme_private.h"
#include "../../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_MONO

#include "lv_theme_mono.h"
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

struct _my_theme_t;
typedef struct _my_theme_t my_theme_t;

#define theme_def (*(my_theme_t **)(&LV_GLOBAL_DEFAULT()->theme_mono))

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
#if LV_USE_CHART
    lv_style_t chart_indic;
#endif
} my_theme_styles_t;

struct _my_theme_t {
    lv_theme_t base;
    my_theme_styles_t styles;
    bool inited;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void style_init_reset(lv_style_t * style);
static void theme_apply(lv_theme_t * th, lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void style_init(my_theme_t * theme, bool dark_bg, const lv_font_t * font)
{
    style_init_reset(&theme->styles.scrollbar);
    lv_style_set_bg_opa(&theme->styles.scrollbar, LV_OPA_COVER);
    lv_style_set_bg_color(&theme->styles.scrollbar, COLOR_FG);
    lv_style_set_width(&theme->styles.scrollbar, PAD_DEF);

    style_init_reset(&theme->styles.scr);
    lv_style_set_bg_opa(&theme->styles.scr, LV_OPA_COVER);
    lv_style_set_bg_color(&theme->styles.scr, COLOR_BG);
    lv_style_set_text_color(&theme->styles.scr, COLOR_FG);
    lv_style_set_pad_row(&theme->styles.scr, PAD_DEF);
    lv_style_set_pad_column(&theme->styles.scr, PAD_DEF);
    lv_style_set_text_font(&theme->styles.scr, font);

    style_init_reset(&theme->styles.card);
    lv_style_set_bg_opa(&theme->styles.card, LV_OPA_COVER);
    lv_style_set_bg_color(&theme->styles.card, COLOR_BG);
    lv_style_set_border_color(&theme->styles.card, COLOR_FG);
    lv_style_set_radius(&theme->styles.card, 2);
    lv_style_set_border_width(&theme->styles.card, BORDER_W_NORMAL);
    lv_style_set_pad_all(&theme->styles.card, PAD_DEF);
    lv_style_set_pad_gap(&theme->styles.card, PAD_DEF);
    lv_style_set_text_color(&theme->styles.card, COLOR_FG);
    lv_style_set_line_width(&theme->styles.card, 2);
    lv_style_set_line_color(&theme->styles.card, COLOR_FG);
    lv_style_set_arc_width(&theme->styles.card, 2);
    lv_style_set_arc_color(&theme->styles.card, COLOR_FG);
    lv_style_set_outline_color(&theme->styles.card, COLOR_FG);
    lv_style_set_anim_duration(&theme->styles.card, 300);

    style_init_reset(&theme->styles.pr);
    lv_style_set_border_width(&theme->styles.pr, BORDER_W_PR);

    style_init_reset(&theme->styles.inv);
    lv_style_set_bg_opa(&theme->styles.inv, LV_OPA_COVER);
    lv_style_set_bg_color(&theme->styles.inv, COLOR_FG);
    lv_style_set_border_color(&theme->styles.inv, COLOR_BG);
    lv_style_set_line_color(&theme->styles.inv, COLOR_BG);
    lv_style_set_arc_color(&theme->styles.inv, COLOR_BG);
    lv_style_set_text_color(&theme->styles.inv, COLOR_BG);
    lv_style_set_outline_color(&theme->styles.inv, COLOR_BG);

    style_init_reset(&theme->styles.disabled);
    lv_style_set_border_width(&theme->styles.disabled, BORDER_W_DIS);

    style_init_reset(&theme->styles.focus);
    lv_style_set_outline_width(&theme->styles.focus, 1);
    lv_style_set_outline_pad(&theme->styles.focus, BORDER_W_FOCUS);

    style_init_reset(&theme->styles.edit);
    lv_style_set_outline_width(&theme->styles.edit, BORDER_W_EDIT);

    style_init_reset(&theme->styles.large_border);
    lv_style_set_border_width(&theme->styles.large_border, BORDER_W_EDIT);

    style_init_reset(&theme->styles.pad_gap);
    lv_style_set_pad_gap(&theme->styles.pad_gap, PAD_DEF);

    style_init_reset(&theme->styles.pad_zero);
    lv_style_set_pad_all(&theme->styles.pad_zero, 0);
    lv_style_set_pad_gap(&theme->styles.pad_zero, 0);

    style_init_reset(&theme->styles.no_radius);
    lv_style_set_radius(&theme->styles.no_radius, 0);

    style_init_reset(&theme->styles.radius_circle);
    lv_style_set_radius(&theme->styles.radius_circle, LV_RADIUS_CIRCLE);

    style_init_reset(&theme->styles.large_line_space);
    lv_style_set_text_line_space(&theme->styles.large_line_space, 6);

    style_init_reset(&theme->styles.underline);
    lv_style_set_text_decor(&theme->styles.underline, LV_TEXT_DECOR_UNDERLINE);

#if LV_USE_TEXTAREA
    style_init_reset(&theme->styles.ta_cursor);
    lv_style_set_border_side(&theme->styles.ta_cursor, LV_BORDER_SIDE_LEFT);
    lv_style_set_border_color(&theme->styles.ta_cursor, COLOR_FG);
    lv_style_set_border_width(&theme->styles.ta_cursor, 2);
    lv_style_set_bg_opa(&theme->styles.ta_cursor, LV_OPA_TRANSP);
    lv_style_set_anim_duration(&theme->styles.ta_cursor, 500);
#endif

#if LV_USE_CHART
    style_init_reset(&theme->styles.chart_indic);
    lv_style_set_radius(&theme->styles.chart_indic, LV_RADIUS_CIRCLE);
    lv_style_set_size(&theme->styles.chart_indic, lv_display_dpx(theme->base.disp, 8), lv_display_dpx(theme->base.disp, 8));
    lv_style_set_bg_color(&theme->styles.chart_indic, COLOR_FG);
    lv_style_set_bg_opa(&theme->styles.chart_indic, LV_OPA_COVER);
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_theme_t * lv_theme_mono_init(lv_display_t * disp, bool dark_bg, const lv_font_t * font)
{
    /*This trick is required only to avoid the garbage collection of
     *styles' data if LVGL is used in a binding (e.g. MicroPython)
     *In a general case styles could be in a simple `static lv_style_t my_style...` variables*/
    if(!lv_theme_mono_is_inited()) {
        theme_def = lv_malloc_zeroed(sizeof(my_theme_t));
        LV_ASSERT_MALLOC(theme_def);
    }

    my_theme_t * theme = theme_def;

    theme->base.disp = disp;
    theme->base.font_small = LV_FONT_DEFAULT;
    theme->base.font_normal = LV_FONT_DEFAULT;
    theme->base.font_large = LV_FONT_DEFAULT;
    theme->base.apply_cb = theme_apply;

    style_init(theme, dark_bg, font);

    if(disp == NULL || lv_display_get_theme(disp) == (lv_theme_t *)theme) {
        lv_obj_report_style_change(NULL);
    }

    theme->inited = true;

    return (lv_theme_t *)theme_def;
}

bool lv_theme_mono_is_inited(void)
{
    my_theme_t * theme = theme_def;
    if(theme == NULL) return false;
    return theme->inited;
}

lv_theme_t * lv_theme_mono_get(void)
{
    if(!lv_theme_mono_is_inited()) {
        return NULL;
    }

    return (lv_theme_t *)theme_def;
}

void lv_theme_mono_deinit(void)
{
    my_theme_t * theme = theme_def;
    if(theme) {
        if(theme->inited) {
            lv_style_t * theme_styles = (lv_style_t *)(&(theme->styles));
            uint32_t i;
            for(i = 0; i < sizeof(my_theme_styles_t) / sizeof(lv_style_t); i++) {
                lv_style_reset(theme_styles + i);
            }
        }
        lv_free(theme_def);
        theme_def = NULL;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void theme_apply(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);

    my_theme_t * theme = theme_def;
    lv_obj_t * parent = lv_obj_get_parent(obj);

    if(parent == NULL) {
        lv_obj_add_style(obj, &theme->styles.scr, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        return;
    }

    if(lv_obj_check_type(obj, &lv_obj_class)) {
#if LV_USE_TABVIEW
        /*Tabview content area*/
        if(lv_obj_check_type(parent, &lv_tabview_class)) {
            return;
        }
        /*Tabview pages*/
        else if(lv_obj_check_type(lv_obj_get_parent(parent), &lv_tabview_class)) {
            lv_obj_add_style(obj, &theme->styles.card, 0);
            lv_obj_add_style(obj, &theme->styles.no_radius, 0);
            lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
            return;
        }
#endif

#if LV_USE_WIN
        /*Header*/
        if(lv_obj_check_type(parent, &lv_win_class) && lv_obj_get_child(parent, 0) == 0) {
            lv_obj_add_style(obj, &theme->styles.card, 0);
            lv_obj_add_style(obj, &theme->styles.no_radius, 0);
            return;
        }
        /*Content*/
        else if(lv_obj_check_type(parent, &lv_win_class) && lv_obj_get_child(parent, 1) == obj) {
            lv_obj_add_style(obj, &theme->styles.card, 0);
            lv_obj_add_style(obj, &theme->styles.no_radius, 0);
            lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
            return;
        }
#endif
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
    }
#if LV_USE_BUTTON
    else if(lv_obj_check_type(obj, &lv_button_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pr, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.inv, LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_STATE_DISABLED);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_BUTTONMATRIX
    else if(lv_obj_check_type(obj, &lv_buttonmatrix_class)) {
#if LV_USE_MSGBOX
        if(lv_obj_check_type(parent, &lv_msgbox_class)) {
            lv_obj_add_style(obj, &theme->styles.pad_gap, 0);
            lv_obj_add_style(obj, &theme->styles.card, LV_PART_ITEMS);
            lv_obj_add_style(obj, &theme->styles.pr, LV_PART_ITEMS | LV_STATE_PRESSED);
            lv_obj_add_style(obj, &theme->styles.disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
            lv_obj_add_style(obj, &theme->styles.underline, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &theme->styles.large_border, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            return;
        }
#endif
#if LV_USE_TABVIEW
        if(lv_obj_check_type(parent, &lv_tabview_class)) {
            lv_obj_add_style(obj, &theme->styles.pad_gap, 0);
            lv_obj_add_style(obj, &theme->styles.card, LV_PART_ITEMS);
            lv_obj_add_style(obj, &theme->styles.pr, LV_PART_ITEMS | LV_STATE_PRESSED);
            lv_obj_add_style(obj, &theme->styles.inv, LV_PART_ITEMS | LV_STATE_CHECKED);
            lv_obj_add_style(obj, &theme->styles.disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
            lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &theme->styles.underline, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &theme->styles.large_border, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            return;
        }
#endif
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.card, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.pr, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.inv, LV_PART_ITEMS | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &theme->styles.underline, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.large_border, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
    }
#endif

#if LV_USE_BAR
    else if(lv_obj_check_type(obj, &lv_bar_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pad_zero, 0);
        lv_obj_add_style(obj, &theme->styles.inv, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
    }
#endif

#if LV_USE_SLIDER
    else if(lv_obj_check_type(obj, &lv_slider_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pad_zero, 0);
        lv_obj_add_style(obj, &theme->styles.inv, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.card, LV_PART_KNOB);
        lv_obj_add_style(obj, &theme->styles.radius_circle, LV_PART_KNOB);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_TABLE
    else if(lv_obj_check_type(obj, &lv_table_class)) {
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.card, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.no_radius, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.pr, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.inv, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_CHECKBOX
    else if(lv_obj_check_type(obj, &lv_checkbox_class)) {
        lv_obj_add_style(obj, &theme->styles.pad_gap, LV_PART_MAIN);
        lv_obj_add_style(obj, &theme->styles.card, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_PART_INDICATOR | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &theme->styles.inv, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.pr, LV_PART_INDICATOR | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_SWITCH
    else if(lv_obj_check_type(obj, &lv_switch_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.radius_circle, 0);
        lv_obj_add_style(obj, &theme->styles.pad_zero, 0);
        lv_obj_add_style(obj, &theme->styles.inv, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.radius_circle, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.card, LV_PART_KNOB);
        lv_obj_add_style(obj, &theme->styles.radius_circle, LV_PART_KNOB);
        lv_obj_add_style(obj, &theme->styles.pad_zero, LV_PART_KNOB);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_CHART
    else if(lv_obj_check_type(obj, &lv_chart_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.chart_indic, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.card, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.card, LV_PART_CURSOR);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
    }
#endif

#if LV_USE_ROLLER
    else if(lv_obj_check_type(obj, &lv_roller_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.large_line_space, 0);
        lv_obj_add_style(obj, &theme->styles.inv, LV_PART_SELECTED);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_DROPDOWN
    else if(lv_obj_check_type(obj, &lv_dropdown_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pr, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
    }
    else if(lv_obj_check_type(obj, &lv_dropdownlist_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.large_line_space, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.inv, LV_PART_SELECTED | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.pr, LV_PART_SELECTED | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_ARC
    else if(lv_obj_check_type(obj, &lv_arc_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.inv, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.pad_zero, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.card, LV_PART_KNOB);
        lv_obj_add_style(obj, &theme->styles.radius_circle, LV_PART_KNOB);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_TEXTAREA
    else if(lv_obj_check_type(obj, &lv_textarea_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.ta_cursor, LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUSED);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
    }
#endif

#if LV_USE_CALENDAR
    else if(lv_obj_check_type(obj, &lv_calendar_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.no_radius, 0);
        lv_obj_add_style(obj, &theme->styles.pr, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
        lv_obj_add_style(obj, &theme->styles.large_border, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
    }
#endif

#if LV_USE_KEYBOARD
    else if(lv_obj_check_type(obj, &lv_keyboard_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.card, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.pr, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.inv, LV_PART_ITEMS | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
        lv_obj_add_style(obj, &theme->styles.large_border, LV_PART_ITEMS | LV_STATE_EDITED);
    }
#endif

#if LV_USE_LIST
    else if(lv_obj_check_type(obj, &lv_list_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        return;
    }
    else if(lv_obj_check_type(obj, &lv_list_text_class)) {

    }
    else if(lv_obj_check_type(obj, &lv_list_button_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pr, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.large_border, LV_STATE_EDITED);
    }
#endif
#if LV_USE_MSGBOX
    else if(lv_obj_check_type(obj, &lv_msgbox_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        return;
    }
#endif

#if LV_USE_SPINBOX
    else if(lv_obj_check_type(obj, &lv_spinbox_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.inv, LV_PART_CURSOR);
        lv_obj_add_style(obj, &theme->styles.focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.edit, LV_STATE_EDITED);
    }
#endif
#if LV_USE_TILEVIEW
    else if(lv_obj_check_type(obj, &lv_tileview_class)) {
        lv_obj_add_style(obj, &theme->styles.scr, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
    }
    else if(lv_obj_check_type(obj, &lv_tileview_tile_class)) {
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
    }
#endif

#if LV_USE_LED
    else if(lv_obj_check_type(obj, &lv_led_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
    }
#endif
}

static void style_init_reset(lv_style_t * style)
{
    if(lv_theme_mono_is_inited()) {
        lv_style_reset(style);
    }
    else {
        lv_style_init(style);
    }
}

#endif
