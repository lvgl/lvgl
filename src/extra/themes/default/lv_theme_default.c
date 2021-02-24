/**
 * @file lv_theme_defau.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_DEFAULT

#include "lv_theme_default.h"
#include "../../../lv_misc/lv_gc.h"

#if defined(LV_GC_INCLUDE)
#include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/

#define RADIUS_DEFAULT LV_DPX(8)

/*SCREEN*/
//#define COLOR_SCR        (IS_LIGHT ? lv_color_hex(0xf5f8fa) : lv_color_hex(0x444b5a))
#define COLOR_SCR        (IS_LIGHT ? lv_color_blue_grey_lighten_5() : lv_color_hex(0x444b5a))
#define COLOR_SCR_TEXT   (IS_LIGHT ? lv_color_hex(0x3b3e42) : lv_color_hex(0xe7e9ec))

/*BUTTON*/
#define BTN_COLOR           (theme.color_primary)
#define BTN_PR_COLOR        (lv_color_darken(theme.color_primary, LV_OPA_20))
#define BTN_CHK_DIS_COLOR   (lv_color_lighten(theme.color_primary, LV_OPA_40))

#define BTN_CHK_COLOR       (theme.color_secondary)
#define BTN_CHK_PR_COLOR    (lv_color_darken(theme.color_secondary, LV_OPA_20))
#define BTN_DIS_COLOR       (lv_color_lighten(theme.color_secondary, LV_OPA_40))

#define BTN_BORDER_COLOR        theme.color_primary
#define COLOR_BTN_BORDER_PR     theme.color_primary
#define COLOR_BTN_BORDER_CHK    theme.color_primary
#define COLOR_BTN_BORDER_CHK_PR theme.color_primary
#define COLOR_BTN_BORDER_INA    (IS_LIGHT ? lv_color_hex3(0x888) : lv_color_hex(0x404040))

/*BACKGROUND*/
#define CARD_COLOR            (IS_LIGHT ? lv_color_hex(0xffffff) : lv_color_hex(0x586273))
#define CARD_PR_COLOR         (IS_LIGHT ? lv_color_hex(0xeeeeee) : lv_color_hex(0x494f57))
#define COLOR_BG_CHK        theme.color_primary
#define COLOR_BG_PR_CHK     lv_color_darken(theme.color_primary, LV_OPA_20)
#define COLOR_BG_DIS        CARD_COLOR

#define CARD_BORDER_COLOR         (IS_LIGHT ? lv_color_hex(0xd6dde3) : lv_color_hex(0x808a97))   /*dfe7ed*/
#define COLOR_BG_BORDER_PR      (IS_LIGHT ? lv_color_hex3(0xccc) : lv_color_hex(0x5f656e))
#define COLOR_BG_BORDER_CHK     (IS_LIGHT ? lv_color_hex(0xd6dde3) : lv_color_hex(0x5f656e))
#define COLOR_BG_BORDER_CHK_PR  (IS_LIGHT ? lv_color_hex3(0xccc) : lv_color_hex(0x5f656e))
#define COLOR_BG_BORDER_DIS     (IS_LIGHT ? lv_color_hex(0xd6dde3) : lv_color_hex(0x5f656e))

#define CARD_TEXT_COLOR           (IS_LIGHT ? lv_color_hex(0x3b3e42) : lv_color_hex(0xffffff))
#define CARD_TEXT_PR_COLOR        (IS_LIGHT ? lv_color_hex(0x3b3e42) : lv_color_hex(0xffffff))
#define COLOR_BG_TEXT_CHK       (IS_LIGHT ? lv_color_hex(0xffffff) : lv_color_hex(0xffffff))
#define COLOR_BG_TEXT_CHK_PR    (IS_LIGHT ? lv_color_hex(0xffffff) : lv_color_hex(0xffffff))
#define COLOR_BG_TEXT_DIS       (IS_LIGHT ? lv_color_hex3(0xaaa) : lv_color_hex3(0x999))

/*SECONDARY BACKGROUND*/
#define COLOR_GRAY              (IS_LIGHT ? lv_color_hex(0xcccfd1) : lv_color_hex(0x45494d))
#define COLOR_BG_SEC_BORDER     (IS_LIGHT ? lv_color_hex(0xdfe7ed) : lv_color_hex(0x404040))
#define COLOR_BG_SEC_TEXT       (IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xa5a8ad))
#define COLOR_BG_SEC_TEXT_DIS   (IS_LIGHT ? lv_color_hex(0xaaaaaa) : lv_color_hex(0xa5a8ad))

#define TRANSITION_TIME         LV_THEME_DEFAULT_TRANSITON_TIME
#define BORDER_WIDTH            LV_DPX(2)
#define OUTLINE_WIDTH           LV_DPX(2)
#define IS_LIGHT (LV_THEME_DEFAULT_PALETTE_LIGHT)

#define PAD_DEF LV_DPX(15)
//(lv_disp_get_size_category(NULL) <= LV_DISP_SIZE_MEDIUM ? LV_DPX(15) : (LV_DPX(30)))

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_style_t scr;
    lv_style_t scrollbar;
    lv_style_t scrollbar_scrolled;
    lv_style_t card;
    lv_style_t btn;

    /*Utility*/
    lv_style_t bg_color_primary;
    lv_style_t bg_color_secondary;
    lv_style_t bg_color_gray;
    lv_style_t bg_color_panel;
    lv_style_t pressed;
    lv_style_t disabled;
    lv_style_t pad_zero;
    lv_style_t pad_small;
    lv_style_t pad_normal;
    lv_style_t pad_gap;
    lv_style_t pad_small_negative;
    lv_style_t line_space_large;
    lv_style_t text_align_center;
    lv_style_t focus_outline;
    lv_style_t edit_outline;
    lv_style_t circle;
    lv_style_t no_radius;
    lv_style_t clip_corner;
#if LV_THEME_DEFAULT_GROW
    lv_style_t grow;
#endif
    lv_style_t transition_delayed;
    lv_style_t transition_normal;
    lv_style_t anim;

    /*Parts*/
    lv_style_t knob;
    lv_style_t indic;

#if LV_USE_ARC
    lv_style_t arc_indic;
    lv_style_t arc_indic_primary;
#endif

#if LV_USE_CHART
    lv_style_t chart_series, chart_ticks, chart_bg;
#endif

#if LV_USE_CHECKBOX
    lv_style_t cb_marker, cb_marker_checked, cb_bg_outline_pad;
#endif

#if LV_USE_DROPDOWN
    lv_style_t ddlist_flip;
#endif

#if LV_USE_TABLE
    lv_style_t table_cell;
#endif

#if LV_USE_METER
    lv_style_t meter_marker, meter_indic;
#endif

#if LV_USE_TEXTAREA
    lv_style_t ta_cursor, ta_placeholder;
#endif

#if LV_USE_CALENDAR
    lv_style_t calendar_day;
#endif

#if LV_USE_COLORWHEEL
    lv_style_t colorwheel_main;
#endif

#if LV_USE_MSGBOX
    lv_style_t msgbox_btns_bg;
#endif

#if LV_USE_LIST
    lv_style_t list_bg, list_btn, list_item_grow, list_label;
#endif

#if LV_USE_LED
    lv_style_t led;
#endif
} my_theme_styles_t;

typedef struct {
    lv_theme_t base;
    uint8_t light :1;
}my_theme_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void theme_apply(lv_theme_t * th, lv_obj_t * obj);
static lv_color_t gray_filter(lv_color_t color, lv_opa_t opa);
static void style_init_reset(lv_style_t * style);

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

static lv_color_t dark_color_filter_cb(const lv_color_filter_dsc_t * f, lv_color_t c, lv_opa_t opa)
{
    LV_UNUSED(f);
    return lv_color_darken(c, opa);
}

static void style_init(void)
{
    static const lv_style_prop_t trans_props[] = {
            LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR, LV_STYLE_CONTENT_OPA,
            LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT,
            LV_STYLE_TRANSFORM_ZOOM, LV_STYLE_TRANSFORM_ANGLE,
            LV_STYLE_CONTENT_OFS_X, LV_STYLE_CONTENT_OFS_Y,
            LV_STYLE_COLOR_FILTER_OPA, LV_STYLE_COLOR_FILTER_DSC,
            0
    };

    static lv_style_transition_dsc_t trans_delayed;
    lv_style_transition_dsc_init(&trans_delayed, trans_props, &lv_anim_path_def, TRANSITION_TIME, 70);

    static lv_style_transition_dsc_t trans_normal;
    lv_style_transition_dsc_init(&trans_normal, trans_props, &lv_anim_path_def, TRANSITION_TIME, 0);

    style_init_reset(&styles->transition_delayed);
    lv_style_set_transition(&styles->transition_delayed, &trans_delayed); /*Go back to default state with delay*/

    style_init_reset(&styles->transition_normal);
    lv_style_set_transition(&styles->transition_normal, &trans_normal); /*Go back to default state with delay*/

    style_init_reset(&styles->scrollbar);
    lv_style_set_bg_opa(&styles->scrollbar, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->scrollbar, (IS_LIGHT ? lv_color_hex(0xcccfd1) : lv_color_hex(0x777f85)));
    lv_style_set_radius(&styles->scrollbar, LV_RADIUS_CIRCLE);
    lv_style_set_pad_right(&styles->scrollbar, LV_DPX(7));
    lv_style_set_pad_top(&styles->scrollbar,  LV_DPX(7));
    lv_style_set_size(&styles->scrollbar,  LV_DPX(5));
    lv_style_set_bg_opa(&styles->scrollbar,  LV_OPA_40);
    lv_style_set_transition(&styles->scrollbar, &trans_normal);

    style_init_reset(&styles->scrollbar_scrolled);
    lv_style_set_bg_opa(&styles->scrollbar_scrolled,  LV_OPA_COVER);

    style_init_reset(&styles->scr);
    lv_style_set_bg_opa(&styles->scr, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->scr, COLOR_SCR);
    lv_style_set_text_color(&styles->scr, COLOR_SCR_TEXT);
    lv_style_set_pad_row(&styles->scr, PAD_DEF);
    lv_style_set_pad_column(&styles->scr, PAD_DEF);

    style_init_reset(&styles->card);
    lv_style_set_radius(&styles->card, RADIUS_DEFAULT);
    lv_style_set_bg_opa(&styles->card, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->card, CARD_COLOR);
    lv_style_set_border_color(&styles->card, CARD_BORDER_COLOR);
    lv_style_set_border_width(&styles->card, BORDER_WIDTH);
    lv_style_set_border_post(&styles->card, true);
    lv_style_set_text_color(&styles->card, CARD_TEXT_COLOR);
    lv_style_set_pad_all(&styles->card, PAD_DEF);
    lv_style_set_pad_row(&styles->card, PAD_DEF);
    lv_style_set_pad_column(&styles->card, PAD_DEF);
    lv_style_set_line_color(&styles->card, COLOR_GRAY);
    lv_style_set_line_width(&styles->card, LV_DPX(1));

    style_init_reset(&styles->focus_outline);
    lv_style_set_outline_color(&styles->focus_outline, theme.color_primary);
    lv_style_set_outline_width(&styles->focus_outline, OUTLINE_WIDTH);
    lv_style_set_outline_opa(&styles->focus_outline, LV_OPA_50);

    style_init_reset(&styles->edit_outline);
    lv_style_set_outline_color(&styles->edit_outline, theme.color_secondary);
    lv_style_set_outline_width(&styles->edit_outline, OUTLINE_WIDTH);
    lv_style_set_outline_opa(&styles->edit_outline, LV_OPA_50);

    style_init_reset(&styles->btn);
    lv_style_set_radius(&styles->btn, RADIUS_DEFAULT);
    lv_style_set_bg_opa(&styles->btn, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->btn, COLOR_GRAY);
    lv_style_set_text_color(&styles->btn, CARD_TEXT_COLOR);
    lv_style_set_pad_hor(&styles->btn, LV_DPX(40));
    lv_style_set_pad_ver(&styles->btn, LV_DPX(15));
    lv_style_set_pad_column(&styles->btn, LV_DPX(5));
    lv_style_set_pad_row(&styles->btn, LV_DPX(5));


    static lv_color_filter_dsc_t dark_filter;
    lv_color_filter_dsc_init(&dark_filter, dark_color_filter_cb);

    style_init_reset(&styles->pressed);
    lv_style_set_color_filter_dsc(&styles->pressed, &dark_filter);
    lv_style_set_color_filter_opa(&styles->pressed, 35);

    style_init_reset(&styles->disabled);
//    lv_style_set_color_filter_dsc(&styles->disabled, gray_filter);
//    lv_style_set_color_filter_opa(&styles->disabled, LV_OPA_50);

    style_init_reset(&styles->clip_corner);
    lv_style_set_clip_corner(&styles->clip_corner, true);

    style_init_reset(&styles->pad_normal);
    lv_style_set_pad_all(&styles->pad_normal, PAD_DEF);
    lv_style_set_pad_row(&styles->pad_normal, PAD_DEF);
    lv_style_set_pad_column(&styles->pad_normal, PAD_DEF);

    style_init_reset(&styles->pad_small);
    lv_style_set_pad_all(&styles->pad_small, LV_DPX(10));
    lv_style_set_pad_row(&styles->pad_small, LV_DPX(5));
    lv_style_set_pad_column(&styles->pad_small, LV_DPX(5));

    style_init_reset(&styles->pad_gap);
    lv_style_set_pad_row(&styles->pad_gap, LV_DPX(10));
    lv_style_set_pad_column(&styles->pad_gap, LV_DPX(10));

    style_init_reset(&styles->pad_small_negative);
    lv_style_set_pad_all(&styles->pad_small_negative, - LV_DPX(4));

    style_init_reset(&styles->line_space_large);
    lv_style_set_text_line_space(&styles->line_space_large, LV_DPX(20));

    style_init_reset(&styles->text_align_center);
    lv_style_set_text_align(&styles->text_align_center, LV_TEXT_ALIGN_CENTER);

    style_init_reset(&styles->pad_zero);
    lv_style_set_pad_all(&styles->pad_zero, 0);
    lv_style_set_pad_row(&styles->pad_zero, 0);
    lv_style_set_pad_column(&styles->pad_zero, 0);

    style_init_reset(&styles->bg_color_primary);
    lv_style_set_bg_color(&styles->bg_color_primary, theme.color_primary);
    lv_style_set_text_color(&styles->bg_color_primary, lv_color_white());
    lv_style_set_content_color(&styles->bg_color_primary, lv_color_white());
    lv_style_set_bg_opa(&styles->bg_color_primary, LV_OPA_COVER);

    style_init_reset(&styles->bg_color_secondary);
    lv_style_set_bg_color(&styles->bg_color_secondary, theme.color_secondary);
    lv_style_set_text_color(&styles->bg_color_secondary, lv_color_white());
    lv_style_set_content_color(&styles->bg_color_secondary, lv_color_white());
    lv_style_set_bg_opa(&styles->bg_color_secondary, LV_OPA_COVER);

    style_init_reset(&styles->bg_color_gray);
    lv_style_set_bg_color(&styles->bg_color_gray, COLOR_GRAY);
    lv_style_set_bg_opa(&styles->bg_color_gray, LV_OPA_COVER);
    lv_style_set_text_color(&styles->bg_color_gray, CARD_TEXT_COLOR);
    lv_style_set_content_color(&styles->bg_color_gray, CARD_TEXT_COLOR);

    style_init_reset(&styles->bg_color_panel);
    lv_style_set_bg_color(&styles->bg_color_panel, lv_color_white());
    lv_style_set_bg_opa(&styles->bg_color_panel, LV_OPA_COVER);
    lv_style_set_text_color(&styles->bg_color_panel, CARD_TEXT_COLOR);
    lv_style_set_content_color(&styles->bg_color_panel, CARD_TEXT_COLOR);

    style_init_reset(&styles->circle);
    lv_style_set_radius(&styles->circle, LV_RADIUS_CIRCLE);

    style_init_reset(&styles->no_radius);
    lv_style_set_radius(&styles->no_radius, 0);

#if LV_THEME_DEFAULT_GROW
    style_init_reset(&styles->grow);
    lv_style_set_transform_width(&styles->grow, LV_DPX(3));
    lv_style_set_transform_height(&styles->grow, LV_DPX(3));
#endif

    style_init_reset(&styles->knob);
    lv_style_set_bg_color(&styles->knob, IS_LIGHT ? theme.color_primary : lv_color_white());
    lv_style_set_bg_opa(&styles->knob, LV_OPA_COVER);
    lv_style_set_pad_all(&styles->knob, LV_DPX(5));
    lv_style_set_radius(&styles->knob, LV_RADIUS_CIRCLE);

    style_init_reset(&styles->anim);
    lv_style_set_anim_time(&styles->anim, 200);

#if LV_USE_ARC
    style_init_reset(&styles->arc_indic);
    lv_style_set_arc_color(&styles->arc_indic, COLOR_GRAY);
    lv_style_set_arc_width(&styles->arc_indic, LV_DPX(15));
    lv_style_set_arc_rounded(&styles->arc_indic, true);

    style_init_reset(&styles->arc_indic_primary);
    lv_style_set_arc_color(&styles->arc_indic_primary, theme.color_primary);
#endif

#if LV_USE_DROPDOWN
    style_init_reset(&styles->ddlist_flip);
    lv_style_set_transform_angle(&styles->ddlist_flip, 1800);
#endif

#if LV_USE_CHECKBOX
    style_init_reset(&styles->cb_marker);
    lv_style_set_pad_all(&styles->cb_marker, LV_DPX(3));
    lv_style_set_border_width(&styles->cb_marker, BORDER_WIDTH);
    lv_style_set_border_color(&styles->cb_marker, theme.color_primary);
    lv_style_set_bg_color(&styles->cb_marker, lv_color_white());
    lv_style_set_bg_opa(&styles->cb_marker, LV_OPA_COVER);
    lv_style_set_radius(&styles->cb_marker, RADIUS_DEFAULT / 2);

    style_init_reset(&styles->cb_marker_checked);
    lv_style_set_content_text(&styles->cb_marker_checked, LV_SYMBOL_OK);
    lv_style_set_content_color(&styles->cb_marker_checked, lv_color_white());
    lv_style_set_content_font(&styles->cb_marker_checked, theme.font_small);

    style_init_reset(&styles->cb_bg_outline_pad);
    lv_style_set_outline_pad(&styles->cb_bg_outline_pad, LV_DPX(5));
#endif

#if LV_USE_CHART
    style_init_reset(&styles->chart_bg);
    lv_style_set_line_dash_width(&styles->chart_bg, LV_DPX(10));
    lv_style_set_line_dash_gap(&styles->chart_bg, LV_DPX(10));

    style_init_reset(&styles->chart_series);
    lv_style_set_line_width(&styles->chart_series, LV_DPX(3));
    lv_style_set_radius(&styles->chart_series, LV_DPX(1));
    lv_style_set_size(&styles->chart_series, LV_DPX(5));
    lv_style_set_pad_column(&styles->chart_series, LV_DPX(2));

    style_init_reset(&styles->chart_ticks);
    lv_style_set_line_width(&styles->chart_ticks, LV_DPX(1));
    lv_style_set_line_color(&styles->chart_ticks, COLOR_SCR_TEXT);
    lv_style_set_pad_all(&styles->chart_ticks, LV_DPX(2));
#endif

#if LV_USE_METER
    style_init_reset(&styles->meter_marker);
    lv_style_set_line_width(&styles->meter_marker, LV_DPX(5));
    lv_style_set_line_color(&styles->meter_marker, CARD_TEXT_COLOR);
    lv_style_set_size(&styles->meter_marker, LV_DPX(20));
    lv_style_set_pad_left(&styles->meter_marker, LV_DPX(15));

    style_init_reset(&styles->meter_indic);
    lv_style_set_radius(&styles->meter_indic, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&styles->meter_indic, CARD_TEXT_COLOR);
    lv_style_set_bg_opa(&styles->meter_indic, LV_OPA_COVER);
    lv_style_set_size(&styles->meter_indic, LV_DPX(15));
#endif

#if LV_USE_TABLE
    style_init_reset(&styles->table_cell);
    lv_style_set_border_width(&styles->table_cell, LV_DPX(1));
    lv_style_set_border_color(&styles->table_cell, CARD_BORDER_COLOR);
    lv_style_set_border_side(&styles->table_cell, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM );
#endif

#if LV_USE_TEXTAREA
    style_init_reset(&styles->ta_cursor);
    lv_style_set_border_color(&styles->ta_cursor, COLOR_BG_SEC_TEXT);
    lv_style_set_border_width(&styles->ta_cursor, LV_DPX(2));
    lv_style_set_pad_left(&styles->ta_cursor, LV_DPX(1));
    lv_style_set_border_side(&styles->ta_cursor, LV_BORDER_SIDE_LEFT);
    lv_style_set_anim_time(&styles->ta_cursor, 400);

    style_init_reset(&styles->ta_placeholder);
    lv_style_set_text_color(&styles->ta_placeholder, IS_LIGHT ? COLOR_BG_TEXT_DIS : lv_color_hex(0xa1adbd));
#endif

#if LV_USE_CALENDAR
    style_init_reset(&styles->calendar_day);
    lv_style_set_border_width(&styles->calendar_day, LV_DPX(1));
    lv_style_set_border_color(&styles->calendar_day, CARD_BORDER_COLOR);
    lv_style_set_bg_opa(&styles->calendar_day, LV_OPA_COVER);
#endif

#if LV_USE_COLORWHEEL
    style_init_reset(&styles->colorwheel_main);
    lv_style_set_arc_width(&styles->colorwheel_main, LV_DPX(10));
#endif

#if LV_USE_MSGBOX
    /*To add space for the buttons outline*/
    style_init_reset(&styles->msgbox_btns_bg);
    lv_style_set_pad_all(&styles->msgbox_btns_bg, OUTLINE_WIDTH);
#endif

#if LV_USE_LIST
    style_init_reset(&styles->list_bg);
    lv_style_set_pad_hor(&styles->list_bg, PAD_DEF);
    lv_style_set_pad_ver(&styles->list_bg, 0);
    lv_style_set_pad_gap(&styles->list_bg, 0);
    lv_style_set_clip_corner(&styles->list_bg, true);

    style_init_reset(&styles->list_btn);
    lv_style_set_border_width(&styles->list_btn, LV_DPX(1));
    lv_style_set_border_color(&styles->list_btn, CARD_BORDER_COLOR);
    lv_style_set_border_side(&styles->list_btn, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_all(&styles->list_btn, PAD_DEF);
    lv_style_set_pad_column(&styles->list_btn, PAD_DEF);

    style_init_reset(&styles->list_item_grow);
    lv_style_set_transform_width(&styles->list_item_grow, PAD_DEF);
#endif


#if LV_USE_LED
    lv_style_init(&styles->led);
    lv_style_set_bg_opa(&styles->led, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->led, lv_color_white());
    lv_style_set_bg_grad_color(&styles->led, lv_color_grey());
    lv_style_set_radius(&styles->led, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_width(&styles->led, LV_DPX(15));
    lv_style_set_shadow_color(&styles->led, lv_color_white());
    lv_style_set_shadow_spread(&styles->led, LV_DPX(5));
#endif
}


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_theme_t * lv_theme_default_init(lv_color_t color_primary, lv_color_t color_secondary,
                                   const lv_font_t * font_small, const lv_font_t * font_normal,
                                   const lv_font_t * font_large, const lv_font_t * font_extra_large)
{

    /* This trick is required only to avoid the garbage collection of
     * styles' data if LVGL is used in a binding (e.g. Micropython)
     * In a general case styles could be in simple `static lv_style_t my_style...` variables*/
    if(!inited) {
        LV_GC_ROOT(_lv_theme_default_styles) = lv_mem_alloc(sizeof(my_theme_styles_t));
        styles = (my_theme_styles_t *)LV_GC_ROOT(_lv_theme_default_styles);
    }

    theme.color_primary = color_primary;
    theme.color_secondary = color_secondary;
    theme.font_small = font_small;
    theme.font_normal = font_normal;
    theme.font_large = font_large;
    theme.font_extra_large = font_extra_large;
    theme.apply_cb = theme_apply;

    style_init();

    inited = true;

    return (lv_theme_t *)&theme;
}


static void theme_apply(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);

    if(lv_obj_get_parent(obj) == NULL) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->scr);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        return;
    }

    if(lv_obj_check_type(obj, &lv_obj_class)) {
#if LV_USE_TABVIEW
        lv_obj_t * parent = lv_obj_get_parent(obj);
        /*Tabview content area*/
        if(lv_obj_check_type(parent, &lv_tabview_class)) {
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_gap);
            return;
        }
        /*Tabview pages*/
        else if(lv_obj_check_type(lv_obj_get_parent(parent), &lv_tabview_class)) {
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->scr);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_normal);
            lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
            lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
            return;
        }
#endif

#if LV_USE_WIN
        /*Header*/
        if(lv_obj_get_child_id(obj) == 0 && lv_obj_check_type(lv_obj_get_parent(obj), &lv_win_class)) {
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_normal);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->text_align_center);
            return;
        }
        /*Content*/
        else if(lv_obj_get_child_id(obj) == 1 && lv_obj_check_type(lv_obj_get_parent(obj), &lv_win_class)) {
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->scr);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_normal);
            lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
            lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
            return;
        }
#endif
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
    }
#if LV_USE_BTN
    else if(lv_obj_check_type(obj, &lv_btn_class)) {
#if LV_USE_LIST
        /*Add different buttons to the lists*/
        if(lv_obj_check_type(lv_obj_get_parent(obj), &lv_list_class)) {
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_panel);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->list_btn);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->bg_color_primary);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->list_item_grow);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->list_item_grow);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->pressed);
            return;
        }
#endif
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->btn);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->transition_delayed);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->pressed);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->transition_normal);
#if LV_THEME_DEFAULT_GROW
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->grow);
#endif
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_CHECKED, &styles->bg_color_secondary);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DISABLED, &styles->disabled);
    }
#endif

#if LV_USE_BTNMATRIX
    else if(lv_obj_check_type(obj, &lv_btnmatrix_class)) {
#if LV_USE_MSGBOX
        if(lv_obj_check_type(lv_obj_get_parent(obj), &lv_msgbox_class)) {
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->msgbox_btns_bg);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_gap);
            lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->btn);
            lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_PRESSED, &styles->pressed);
            lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_CHECKED, &styles->bg_color_primary);
            lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_CHECKED, &styles->bg_color_primary);
            lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_FOCUSED, &styles->focus_outline);
            return;
        }
#endif
#if LV_USE_TABVIEW
        if(lv_obj_check_type(lv_obj_get_parent(obj), &lv_tabview_class)) {
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_panel);
            lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->bg_color_gray);
            lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_PRESSED, &styles->pressed);
            lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_CHECKED, &styles->bg_color_panel);
            return;
        }
#endif
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->focus_outline);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->btn);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_PRESSED, &styles->pressed);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_CHECKED, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_FOCUS_KEY, &styles->focus_outline);
    }
#endif

#if LV_USE_BAR
    else if(lv_obj_check_type(obj, &lv_bar_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->focus_outline);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_EDITED, &styles->edit_outline);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->circle);
    }
#endif

#if LV_USE_SLIDER
    else if(lv_obj_check_type(obj, &lv_slider_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->transition_delayed);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->transition_normal);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->focus_outline);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_EDITED, &styles->edit_outline);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->knob);
#if LV_THEME_DEFAULT_GROW
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_PRESSED, &styles->grow);
#endif
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->transition_delayed);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_PRESSED, &styles->transition_normal);
    }
#endif

#if LV_USE_TABLE
    else if(lv_obj_check_type(obj, &lv_table_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_zero);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->no_radius);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->bg_color_panel);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->table_cell);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->pad_normal);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_PRESSED, &styles->pressed);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_FOCUS_KEY, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_EDITED, &styles->bg_color_secondary);
    }
#endif

#if LV_USE_CHECKBOX
    else if(lv_obj_check_type(obj, &lv_checkbox_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_gap);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->focus_outline);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->cb_bg_outline_pad);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_DISABLED, &styles->disabled);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->cb_marker);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_CHECKED, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_CHECKED, &styles->cb_marker_checked);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_PRESSED, &styles->pressed);
#if LV_THEME_DEFAULT_GROW
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_PRESSED, &styles->grow);
#endif
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_PRESSED, &styles->transition_normal);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->transition_delayed);
    }
#endif

#if LV_USE_SWITCH
    else if(lv_obj_check_type(obj, &lv_switch_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DISABLED, &styles->disabled);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->focus_outline);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DISABLED, &styles->disabled);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->knob);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->bg_color_panel);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->pad_small_negative);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DISABLED, &styles->disabled);
    }
#endif

#if LV_USE_CHART
    else if(lv_obj_check_type(obj, &lv_chart_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->chart_bg);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_small);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->chart_series);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_PRESSED, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->chart_ticks);
    }
#endif

#if LV_USE_ROLLER
    else if(lv_obj_check_type(obj, &lv_roller_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->anim);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->line_space_large);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->text_align_center);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->focus_outline);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_EDITED, &styles->edit_outline);
        lv_obj_add_style(obj, LV_PART_SELECTED, LV_STATE_DEFAULT, &styles->bg_color_primary);
    }
#endif

#if LV_USE_DROPDOWN
    else if(lv_obj_check_type(obj, &lv_dropdown_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->btn);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_normal);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->transition_delayed);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->pressed);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->transition_normal);
#if LV_THEME_DEFAULT_GROW
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->grow);
#endif
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_CHECKED, &styles->ddlist_flip);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->transition_normal);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->focus_outline);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_EDITED, &styles->edit_outline);
    }
    else if(lv_obj_check_type(obj, &lv_dropdown_list_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->line_space_large);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        lv_obj_add_style(obj, LV_PART_SELECTED, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_SELECTED, LV_STATE_PRESSED, &styles->bg_color_gray);
    }
#endif

#if LV_USE_LABEL
    else if(lv_obj_check_type(obj, &lv_label_class)) {
#if LV_USE_LIST
        if(lv_obj_check_type(lv_obj_get_parent(obj), &lv_list_class)) {
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->list_item_grow);
        }
#endif
    }
#endif

#if LV_USE_ARC
    else if(lv_obj_check_type(obj, &lv_arc_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->arc_indic);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->arc_indic);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->arc_indic_primary);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->knob);
    }
#endif

#if LV_USE_METER
    else if(lv_obj_check_type(obj, &lv_meter_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->meter_indic);
    }
#endif

#if LV_USE_TEXTAREA
    else if(lv_obj_check_type(obj, &lv_textarea_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->focus_outline);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_EDITED, &styles->edit_outline);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_FOCUSED, &styles->ta_cursor);
        lv_obj_add_style(obj, LV_PART_TEXTAREA_PLACEHOLDER, LV_STATE_DEFAULT, &styles->ta_placeholder);
    }
#endif

#if LV_USE_CALENDAR
    else if(lv_obj_check_type(obj, &lv_calendar_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_small);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->focus_outline);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_EDITED, &styles->edit_outline);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->calendar_day);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_PRESSED, &styles->pressed);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_DISABLED, &styles->disabled);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_FOCUS_KEY, &styles->focus_outline);
    }
#endif

#if LV_USE_KEYBOARD
    else if(lv_obj_check_type(obj, &lv_keyboard_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->scr);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_small);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->focus_outline);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_EDITED, &styles->edit_outline);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->btn);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->bg_color_panel);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_PRESSED, &styles->pressed);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_CHECKED, &styles->bg_color_gray);
        lv_obj_add_style(obj, LV_PART_ITEMS, LV_STATE_FOCUSED, &styles->focus_outline);
    }
#endif
#if LV_USE_LIST
    else if(lv_obj_check_type(obj, &lv_list_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->list_bg);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        return;
    }
#endif
#if LV_USE_MSGBOX
    else if(lv_obj_check_type(obj, &lv_msgbox_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        return;
    }
#endif
#if LV_USE_SPINBOX
    else if(lv_obj_check_type(obj, &lv_spinbox_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_FOCUS_KEY, &styles->focus_outline);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_EDITED, &styles->edit_outline);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->bg_color_gray);
    }
#endif
#if LV_USE_TILEVIEW
    else if(lv_obj_check_type(obj, &lv_tileview_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->scr);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
    }
    else if(lv_obj_check_type(obj, &lv_tileview_tile_class)) {
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
    }
#endif

#if LV_USE_COLORWHEEL
    else if(lv_obj_check_type(obj, &lv_colorwheel_class)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->colorwheel_main);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->pad_normal);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->bg_color_panel);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->pad_normal);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->bg_color_panel);
    }
#endif

#if LV_USE_LED
    else if(lv_obj_check_type(obj, &lv_led_class)) {
            lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->led);
        }
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_color_t gray_filter(lv_color_t color, lv_opa_t opa)
{
    return lv_color_mix(lv_color_grey_lighten_2(), color, opa);
}
static void style_init_reset(lv_style_t * style)
{
    if(inited) lv_style_reset(style);
    else lv_style_init(style);
}

#endif
