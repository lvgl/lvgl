/**
 * @file lv_theme_default.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_DEFAULT

#include "lv_theme_default.h"
#include "../../../misc/lv_gc.h"

/*********************
 *      DEFINES
 *********************/
#define MODE_DARK 1
#define RADIUS_DEFAULT (disp_size == DISP_LARGE ? lv_disp_dpx(theme.disp, 12) : lv_disp_dpx(theme.disp, 8))

/*SCREEN*/
#define LIGHT_COLOR_SCR        lv_palette_lighten(LV_PALETTE_GREY, 4)
#define LIGHT_COLOR_CARD       lv_color_white()
#define LIGHT_COLOR_TEXT       lv_palette_darken(LV_PALETTE_GREY, 4)
#define LIGHT_COLOR_GREY       lv_palette_lighten(LV_PALETTE_GREY, 2)
#define DARK_COLOR_SCR         lv_color_hex(0x15171A)
#define DARK_COLOR_CARD        lv_color_hex(0x282b30)
#define DARK_COLOR_TEXT        lv_palette_lighten(LV_PALETTE_GREY, 5)
#define DARK_COLOR_GREY        lv_color_hex(0x2f3237)

#define TRANSITION_TIME         LV_THEME_DEFAULT_TRANSITION_TIME
#define BORDER_WIDTH            lv_disp_dpx(theme.disp, 2)
#define OUTLINE_WIDTH           lv_disp_dpx(theme.disp, 3)

#define PAD_DEF     (disp_size == DISP_LARGE ? lv_disp_dpx(theme.disp, 24) : disp_size == DISP_MEDIUM ? lv_disp_dpx(theme.disp, 20) : lv_disp_dpx(theme.disp, 16))
#define PAD_SMALL   (disp_size == DISP_LARGE ? lv_disp_dpx(theme.disp, 14) : disp_size == DISP_MEDIUM ? lv_disp_dpx(theme.disp, 12) : lv_disp_dpx(theme.disp, 10))
#define PAD_TINY   (disp_size == DISP_LARGE ? lv_disp_dpx(theme.disp, 8) : disp_size == DISP_MEDIUM ? lv_disp_dpx(theme.disp, 6) : lv_disp_dpx(theme.disp, 2))

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
    lv_style_t bg_color_primary_muted;
    lv_style_t bg_color_secondary;
    lv_style_t bg_color_secondary_muted;
    lv_style_t bg_color_grey;
    lv_style_t bg_color_white;
    lv_style_t pressed;
    lv_style_t disabled;
    lv_style_t pad_zero;
    lv_style_t pad_tiny;
    lv_style_t pad_small;
    lv_style_t pad_normal;
    lv_style_t pad_gap;
    lv_style_t line_space_large;
    lv_style_t text_align_center;
    lv_style_t outline_primary;
    lv_style_t outline_secondary;
    lv_style_t circle;
    lv_style_t no_radius;
    lv_style_t clip_corner;
#if LV_THEME_DEFAULT_GROW
    lv_style_t grow;
#endif
    lv_style_t transition_delayed;
    lv_style_t transition_normal;
    lv_style_t anim;
    lv_style_t anim_fast;

    /*Parts*/
    lv_style_t knob;
    lv_style_t indic;

#if LV_USE_ARC
    lv_style_t arc_indic;
    lv_style_t arc_indic_primary;
#endif

#if LV_USE_CHART
    lv_style_t chart_series, chart_indic, chart_ticks, chart_bg;
#endif

#if LV_USE_DROPDOWN
    lv_style_t dropdown_list;
#endif

#if LV_USE_CHECKBOX
    lv_style_t cb_marker, cb_marker_checked;
#endif

#if LV_USE_SWITCH
    lv_style_t switch_knob;
#endif

#if LV_USE_LINE
    lv_style_t line;
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
    lv_style_t calendar_bg, calendar_day;
#endif

#if LV_USE_COLORWHEEL
    lv_style_t colorwheel_main;
#endif

#if LV_USE_MSGBOX
    lv_style_t msgbox_bg, msgbox_btn_bg, msgbox_backdrop_bg;
#endif

#if LV_USE_KEYBOARD
    lv_style_t keyboard_btn_bg;
#endif

#if LV_USE_LIST
    lv_style_t list_bg, list_btn, list_item_grow, list_label;
#endif

#if LV_USE_TABVIEW
    lv_style_t tab_bg_focus, tab_btn;
#endif
#if LV_USE_LED
    lv_style_t led;
#endif
} my_theme_styles_t;

typedef struct {
    lv_theme_t base;
    uint8_t light : 1;
} my_theme_t;

typedef enum {
    DISP_SMALL = 3,
    DISP_MEDIUM = 2,
    DISP_LARGE = 1,
} disp_size_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void theme_apply(lv_theme_t * th, lv_obj_t * obj);
static void style_init_reset(lv_style_t * style);

/**********************
 *  STATIC VARIABLES
 **********************/
static my_theme_styles_t * styles;
static lv_theme_t theme;
static disp_size_t disp_size;
static bool inited;
static lv_color_t color_scr;
static lv_color_t color_text;
static lv_color_t color_card;
static lv_color_t color_grey;


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

static lv_color_t grey_filter_cb(const lv_color_filter_dsc_t * f, lv_color_t color, lv_opa_t opa)
{
    LV_UNUSED(f);
    if(theme.flags & MODE_DARK) return lv_color_mix(lv_palette_darken(LV_PALETTE_GREY, 2), color, opa);
    else return lv_color_mix(lv_palette_lighten(LV_PALETTE_GREY, 2), color, opa);
}

static void style_init(void)
{
    static const lv_style_prop_t trans_props[] = {
        LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR,
        LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT,
        LV_STYLE_TRANSLATE_Y, LV_STYLE_TRANSLATE_X,
        LV_STYLE_TRANSFORM_ZOOM, LV_STYLE_TRANSFORM_ANGLE,
        LV_STYLE_COLOR_FILTER_OPA, LV_STYLE_COLOR_FILTER_DSC,
        0
    };

    color_scr = theme.flags & MODE_DARK ? DARK_COLOR_SCR : LIGHT_COLOR_SCR;
    color_text = theme.flags & MODE_DARK ? DARK_COLOR_TEXT : LIGHT_COLOR_TEXT;
    color_card = theme.flags & MODE_DARK ? DARK_COLOR_CARD : LIGHT_COLOR_CARD;
    color_grey = theme.flags & MODE_DARK ? DARK_COLOR_GREY : LIGHT_COLOR_GREY;

    static lv_style_transition_dsc_t trans_delayed;
    lv_style_transition_dsc_init(&trans_delayed, trans_props, lv_anim_path_linear, TRANSITION_TIME, 70, NULL);

    static lv_style_transition_dsc_t trans_normal;
    lv_style_transition_dsc_init(&trans_normal, trans_props, lv_anim_path_linear, TRANSITION_TIME, 0, NULL);

    style_init_reset(&styles->transition_delayed);
    lv_style_set_transition(&styles->transition_delayed, &trans_delayed); /*Go back to default state with delay*/

    style_init_reset(&styles->transition_normal);
    lv_style_set_transition(&styles->transition_normal, &trans_normal); /*Go back to default state with delay*/

    style_init_reset(&styles->scrollbar);
    lv_style_set_bg_color(&styles->scrollbar, (theme.flags & MODE_DARK) ? lv_palette_darken(LV_PALETTE_GREY,
                                                                                            2) : lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_radius(&styles->scrollbar, LV_RADIUS_CIRCLE);
    lv_style_set_pad_right(&styles->scrollbar, lv_disp_dpx(theme.disp, 7));
    lv_style_set_pad_top(&styles->scrollbar,  lv_disp_dpx(theme.disp, 7));
    lv_style_set_size(&styles->scrollbar,  lv_disp_dpx(theme.disp, 5));
    lv_style_set_bg_opa(&styles->scrollbar,  LV_OPA_40);
    lv_style_set_transition(&styles->scrollbar, &trans_normal);

    style_init_reset(&styles->scrollbar_scrolled);
    lv_style_set_bg_opa(&styles->scrollbar_scrolled,  LV_OPA_COVER);

    style_init_reset(&styles->scr);
    lv_style_set_bg_opa(&styles->scr, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->scr, color_scr);
    lv_style_set_text_color(&styles->scr, color_text);
    lv_style_set_pad_row(&styles->scr, PAD_SMALL);
    lv_style_set_pad_column(&styles->scr, PAD_SMALL);

    style_init_reset(&styles->card);
    lv_style_set_radius(&styles->card, RADIUS_DEFAULT);
    lv_style_set_bg_opa(&styles->card, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->card, color_card);
    lv_style_set_border_color(&styles->card, color_grey);
    lv_style_set_border_width(&styles->card, BORDER_WIDTH);
    lv_style_set_border_post(&styles->card, true);
    lv_style_set_text_color(&styles->card, color_text);
    lv_style_set_pad_all(&styles->card, PAD_DEF);
    lv_style_set_pad_row(&styles->card, PAD_SMALL);
    lv_style_set_pad_column(&styles->card, PAD_SMALL);
    lv_style_set_line_color(&styles->card, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_line_width(&styles->card, lv_disp_dpx(theme.disp, 1));

    style_init_reset(&styles->outline_primary);
    lv_style_set_outline_color(&styles->outline_primary, theme.color_primary);
    lv_style_set_outline_width(&styles->outline_primary, OUTLINE_WIDTH);
    lv_style_set_outline_pad(&styles->outline_primary, OUTLINE_WIDTH);
    lv_style_set_outline_opa(&styles->outline_primary, LV_OPA_50);

    style_init_reset(&styles->outline_secondary);
    lv_style_set_outline_color(&styles->outline_secondary, theme.color_secondary);
    lv_style_set_outline_width(&styles->outline_secondary, OUTLINE_WIDTH);
    lv_style_set_outline_opa(&styles->outline_secondary, LV_OPA_50);

    style_init_reset(&styles->btn);
    lv_style_set_radius(&styles->btn, (disp_size == DISP_LARGE ? lv_disp_dpx(theme.disp,
                                                                             16) : disp_size == DISP_MEDIUM ? lv_disp_dpx(theme.disp, 12) : lv_disp_dpx(theme.disp, 8)));
    lv_style_set_bg_opa(&styles->btn, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->btn, color_grey);
    if(!(theme.flags & MODE_DARK)) {
        lv_style_set_shadow_color(&styles->btn, lv_palette_main(LV_PALETTE_GREY));
        lv_style_set_shadow_width(&styles->btn, LV_DPX(3));
        lv_style_set_shadow_opa(&styles->btn, LV_OPA_50);
        lv_style_set_shadow_ofs_y(&styles->btn, lv_disp_dpx(theme.disp, LV_DPX(4)));
    }
    lv_style_set_text_color(&styles->btn, color_text);
    lv_style_set_pad_hor(&styles->btn, PAD_DEF);
    lv_style_set_pad_ver(&styles->btn, PAD_SMALL);
    lv_style_set_pad_column(&styles->btn, lv_disp_dpx(theme.disp, 5));
    lv_style_set_pad_row(&styles->btn, lv_disp_dpx(theme.disp, 5));

    static lv_color_filter_dsc_t dark_filter;
    lv_color_filter_dsc_init(&dark_filter, dark_color_filter_cb);

    static lv_color_filter_dsc_t grey_filter;
    lv_color_filter_dsc_init(&grey_filter, grey_filter_cb);

    style_init_reset(&styles->pressed);
    lv_style_set_color_filter_dsc(&styles->pressed, &dark_filter);
    lv_style_set_color_filter_opa(&styles->pressed, 35);

    style_init_reset(&styles->disabled);
    lv_style_set_color_filter_dsc(&styles->disabled, &grey_filter);
    lv_style_set_color_filter_opa(&styles->disabled, LV_OPA_50);

    style_init_reset(&styles->clip_corner);
    lv_style_set_clip_corner(&styles->clip_corner, true);
    lv_style_set_border_post(&styles->clip_corner, true);

    style_init_reset(&styles->pad_normal);
    lv_style_set_pad_all(&styles->pad_normal, PAD_DEF);
    lv_style_set_pad_row(&styles->pad_normal, PAD_DEF);
    lv_style_set_pad_column(&styles->pad_normal, PAD_DEF);

    style_init_reset(&styles->pad_small);
    lv_style_set_pad_all(&styles->pad_small, PAD_SMALL);
    lv_style_set_pad_gap(&styles->pad_small, PAD_SMALL);

    style_init_reset(&styles->pad_gap);
    lv_style_set_pad_row(&styles->pad_gap, lv_disp_dpx(theme.disp, 10));
    lv_style_set_pad_column(&styles->pad_gap, lv_disp_dpx(theme.disp, 10));

    style_init_reset(&styles->line_space_large);
    lv_style_set_text_line_space(&styles->line_space_large, lv_disp_dpx(theme.disp, 20));

    style_init_reset(&styles->text_align_center);
    lv_style_set_text_align(&styles->text_align_center, LV_TEXT_ALIGN_CENTER);

    style_init_reset(&styles->pad_zero);
    lv_style_set_pad_all(&styles->pad_zero, 0);
    lv_style_set_pad_row(&styles->pad_zero, 0);
    lv_style_set_pad_column(&styles->pad_zero, 0);

    style_init_reset(&styles->pad_tiny);
    lv_style_set_pad_all(&styles->pad_tiny, PAD_TINY);
    lv_style_set_pad_row(&styles->pad_tiny, PAD_TINY);
    lv_style_set_pad_column(&styles->pad_tiny, PAD_TINY);

    style_init_reset(&styles->bg_color_primary);
    lv_style_set_bg_color(&styles->bg_color_primary, theme.color_primary);
    lv_style_set_text_color(&styles->bg_color_primary, lv_color_white());
    lv_style_set_bg_opa(&styles->bg_color_primary, LV_OPA_COVER);

    style_init_reset(&styles->bg_color_primary_muted);
    lv_style_set_bg_color(&styles->bg_color_primary_muted, theme.color_primary);
    lv_style_set_text_color(&styles->bg_color_primary_muted, theme.color_primary);
    lv_style_set_bg_opa(&styles->bg_color_primary_muted, LV_OPA_20);

    style_init_reset(&styles->bg_color_secondary);
    lv_style_set_bg_color(&styles->bg_color_secondary, theme.color_secondary);
    lv_style_set_text_color(&styles->bg_color_secondary, lv_color_white());
    lv_style_set_bg_opa(&styles->bg_color_secondary, LV_OPA_COVER);

    style_init_reset(&styles->bg_color_secondary_muted);
    lv_style_set_bg_color(&styles->bg_color_secondary_muted, theme.color_secondary);
    lv_style_set_text_color(&styles->bg_color_secondary_muted, theme.color_secondary);
    lv_style_set_bg_opa(&styles->bg_color_secondary_muted, LV_OPA_20);

    style_init_reset(&styles->bg_color_grey);
    lv_style_set_bg_color(&styles->bg_color_grey, color_grey);
    lv_style_set_bg_opa(&styles->bg_color_grey, LV_OPA_COVER);
    lv_style_set_text_color(&styles->bg_color_grey, color_text);

    style_init_reset(&styles->bg_color_white);
    lv_style_set_bg_color(&styles->bg_color_white, color_card);
    lv_style_set_bg_opa(&styles->bg_color_white, LV_OPA_COVER);
    lv_style_set_text_color(&styles->bg_color_white, color_text);

    style_init_reset(&styles->circle);
    lv_style_set_radius(&styles->circle, LV_RADIUS_CIRCLE);

    style_init_reset(&styles->no_radius);
    lv_style_set_radius(&styles->no_radius, 0);

#if LV_THEME_DEFAULT_GROW
    style_init_reset(&styles->grow);
    lv_style_set_transform_width(&styles->grow, lv_disp_dpx(theme.disp, 3));
    lv_style_set_transform_height(&styles->grow, lv_disp_dpx(theme.disp, 3));
#endif

    style_init_reset(&styles->knob);
    lv_style_set_bg_color(&styles->knob, theme.color_primary);
    lv_style_set_bg_opa(&styles->knob, LV_OPA_COVER);
    lv_style_set_pad_all(&styles->knob, lv_disp_dpx(theme.disp, 6));
    lv_style_set_radius(&styles->knob, LV_RADIUS_CIRCLE);

    style_init_reset(&styles->anim);
    lv_style_set_anim_time(&styles->anim, 200);

    style_init_reset(&styles->anim_fast);
    lv_style_set_anim_time(&styles->anim_fast, 120);

#if LV_USE_ARC
    style_init_reset(&styles->arc_indic);
    lv_style_set_arc_color(&styles->arc_indic, color_grey);
    lv_style_set_arc_width(&styles->arc_indic, lv_disp_dpx(theme.disp, 15));
    lv_style_set_arc_rounded(&styles->arc_indic, true);

    style_init_reset(&styles->arc_indic_primary);
    lv_style_set_arc_color(&styles->arc_indic_primary, theme.color_primary);
#endif

#if LV_USE_DROPDOWN
    style_init_reset(&styles->dropdown_list);
    lv_style_set_max_height(&styles->dropdown_list, LV_DPI_DEF * 2);
#endif
#if LV_USE_CHECKBOX
    style_init_reset(&styles->cb_marker);
    lv_style_set_pad_all(&styles->cb_marker, lv_disp_dpx(theme.disp, 3));
    lv_style_set_border_width(&styles->cb_marker, BORDER_WIDTH);
    lv_style_set_border_color(&styles->cb_marker, theme.color_primary);
    lv_style_set_bg_color(&styles->cb_marker, color_card);
    lv_style_set_bg_opa(&styles->cb_marker, LV_OPA_COVER);
    lv_style_set_radius(&styles->cb_marker, RADIUS_DEFAULT / 2);

    style_init_reset(&styles->cb_marker_checked);
    lv_style_set_bg_img_src(&styles->cb_marker_checked, LV_SYMBOL_OK);
    lv_style_set_text_color(&styles->cb_marker_checked, lv_color_white());
    lv_style_set_text_font(&styles->cb_marker_checked, theme.font_small);
#endif

#if LV_USE_SWITCH
    style_init_reset(&styles->switch_knob);
    lv_style_set_pad_all(&styles->switch_knob, - lv_disp_dpx(theme.disp, 4));
    lv_style_set_bg_color(&styles->switch_knob, lv_color_white());
#endif

#if LV_USE_LINE
    style_init_reset(&styles->line);
    lv_style_set_line_width(&styles->line, 1);
    lv_style_set_line_color(&styles->line, color_text);
#endif

#if LV_USE_CHART
    style_init_reset(&styles->chart_bg);
    lv_style_set_border_post(&styles->chart_bg, false);
    lv_style_set_pad_column(&styles->chart_bg, lv_disp_dpx(theme.disp, 10));
    lv_style_set_line_color(&styles->chart_bg, color_grey);

    style_init_reset(&styles->chart_series);
    lv_style_set_line_width(&styles->chart_series, lv_disp_dpx(theme.disp, 3));
    lv_style_set_radius(&styles->chart_series, lv_disp_dpx(theme.disp, 3));
    lv_style_set_size(&styles->chart_series, lv_disp_dpx(theme.disp, 8));
    lv_style_set_pad_column(&styles->chart_series, lv_disp_dpx(theme.disp, 2));

    style_init_reset(&styles->chart_indic);
    lv_style_set_radius(&styles->chart_indic, LV_RADIUS_CIRCLE);
    lv_style_set_size(&styles->chart_indic, lv_disp_dpx(theme.disp, 8));
    lv_style_set_bg_color(&styles->chart_indic, theme.color_primary);
    lv_style_set_bg_opa(&styles->chart_indic, LV_OPA_COVER);

    style_init_reset(&styles->chart_ticks);
    lv_style_set_line_width(&styles->chart_ticks, lv_disp_dpx(theme.disp, 1));
    lv_style_set_line_color(&styles->chart_ticks, color_text);
    lv_style_set_pad_all(&styles->chart_ticks, lv_disp_dpx(theme.disp, 2));
    lv_style_set_text_color(&styles->chart_ticks, lv_palette_main(LV_PALETTE_GREY));
#endif

#if LV_USE_METER
    style_init_reset(&styles->meter_marker);
    lv_style_set_line_width(&styles->meter_marker, lv_disp_dpx(theme.disp, 5));
    lv_style_set_line_color(&styles->meter_marker, color_text);
    lv_style_set_size(&styles->meter_marker, lv_disp_dpx(theme.disp, 20));
    lv_style_set_pad_left(&styles->meter_marker, lv_disp_dpx(theme.disp, 15));

    style_init_reset(&styles->meter_indic);
    lv_style_set_radius(&styles->meter_indic, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&styles->meter_indic, color_text);
    lv_style_set_bg_opa(&styles->meter_indic, LV_OPA_COVER);
    lv_style_set_size(&styles->meter_indic, lv_disp_dpx(theme.disp, 15));
#endif

#if LV_USE_TABLE
    style_init_reset(&styles->table_cell);
    lv_style_set_border_width(&styles->table_cell, lv_disp_dpx(theme.disp, 1));
    lv_style_set_border_color(&styles->table_cell, color_grey);
    lv_style_set_border_side(&styles->table_cell, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
#endif

#if LV_USE_TEXTAREA
    style_init_reset(&styles->ta_cursor);
    lv_style_set_border_color(&styles->ta_cursor, color_text);
    lv_style_set_border_width(&styles->ta_cursor, lv_disp_dpx(theme.disp, 2));
    lv_style_set_pad_left(&styles->ta_cursor, - lv_disp_dpx(theme.disp, 1));
    lv_style_set_border_side(&styles->ta_cursor, LV_BORDER_SIDE_LEFT);
    lv_style_set_anim_time(&styles->ta_cursor, 400);

    style_init_reset(&styles->ta_placeholder);
    lv_style_set_text_color(&styles->ta_placeholder, (theme.flags & MODE_DARK) ? lv_palette_darken(LV_PALETTE_GREY,
                                                                                                   2) : lv_palette_lighten(LV_PALETTE_GREY, 1));
#endif

#if LV_USE_CALENDAR
    style_init_reset(&styles->calendar_bg);
    lv_style_set_pad_all(&styles->calendar_bg, PAD_SMALL);
    lv_style_set_pad_gap(&styles->calendar_bg, PAD_SMALL / 2);
    lv_style_set_radius(&styles->calendar_bg, 0);

    style_init_reset(&styles->calendar_day);
    lv_style_set_border_width(&styles->calendar_day, lv_disp_dpx(theme.disp, 1));
    lv_style_set_border_color(&styles->calendar_day, color_grey);
    lv_style_set_bg_color(&styles->calendar_day, color_card);
    lv_style_set_bg_opa(&styles->calendar_day, LV_OPA_20);
#endif

#if LV_USE_COLORWHEEL
    style_init_reset(&styles->colorwheel_main);
    lv_style_set_arc_width(&styles->colorwheel_main, lv_disp_dpx(theme.disp, 10));
#endif

#if LV_USE_MSGBOX
    /*To add space for for the button shadow*/
    style_init_reset(&styles->msgbox_btn_bg);
    lv_style_set_pad_all(&styles->msgbox_btn_bg, lv_disp_dpx(theme.disp, 4));

    style_init_reset(&styles->msgbox_bg);
    lv_style_set_max_width(&styles->msgbox_bg, lv_pct(100));

    style_init_reset(&styles->msgbox_backdrop_bg);
    lv_style_set_bg_color(&styles->msgbox_backdrop_bg, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_opa(&styles->msgbox_backdrop_bg, LV_OPA_50);
#endif
#if LV_USE_KEYBOARD
    style_init_reset(&styles->keyboard_btn_bg);
    lv_style_set_shadow_width(&styles->keyboard_btn_bg, 0);
    lv_style_set_radius(&styles->keyboard_btn_bg, disp_size == DISP_SMALL ? RADIUS_DEFAULT / 2 : RADIUS_DEFAULT);
#endif

#if LV_USE_TABVIEW
    style_init_reset(&styles->tab_btn);
    lv_style_set_border_color(&styles->tab_btn, theme.color_primary);
    lv_style_set_border_width(&styles->tab_btn, BORDER_WIDTH * 2);
    lv_style_set_border_side(&styles->tab_btn, LV_BORDER_SIDE_BOTTOM);

    style_init_reset(&styles->tab_bg_focus);
    lv_style_set_outline_pad(&styles->tab_bg_focus, -BORDER_WIDTH);
#endif

#if LV_USE_LIST
    style_init_reset(&styles->list_bg);
    lv_style_set_pad_hor(&styles->list_bg, PAD_DEF);
    lv_style_set_pad_ver(&styles->list_bg, 0);
    lv_style_set_pad_gap(&styles->list_bg, 0);
    lv_style_set_clip_corner(&styles->list_bg, true);

    style_init_reset(&styles->list_btn);
    lv_style_set_border_width(&styles->list_btn, lv_disp_dpx(theme.disp, 1));
    lv_style_set_border_color(&styles->list_btn, color_grey);
    lv_style_set_border_side(&styles->list_btn, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_all(&styles->list_btn, PAD_SMALL);
    lv_style_set_pad_column(&styles->list_btn, PAD_SMALL);

    style_init_reset(&styles->list_item_grow);
    lv_style_set_transform_width(&styles->list_item_grow, PAD_DEF);
#endif


#if LV_USE_LED
    style_init_reset(&styles->led);
    lv_style_set_bg_opa(&styles->led, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->led, lv_color_white());
    lv_style_set_bg_grad_color(&styles->led, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_radius(&styles->led, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_width(&styles->led, lv_disp_dpx(theme.disp, 15));
    lv_style_set_shadow_color(&styles->led, lv_color_white());
    lv_style_set_shadow_spread(&styles->led, lv_disp_dpx(theme.disp, 5));
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_theme_t * lv_theme_default_init(lv_disp_t * disp, lv_color_t color_primary, lv_color_t color_secondary, bool dark,
                                   const lv_font_t * font)
{

    /*This trick is required only to avoid the garbage collection of
     *styles' data if LVGL is used in a binding (e.g. Micropython)
     *In a general case styles could be in simple `static lv_style_t my_style...` variables*/
    if(!inited) {
        LV_GC_ROOT(_lv_theme_default_styles) = lv_mem_alloc(sizeof(my_theme_styles_t));
        styles = (my_theme_styles_t *)LV_GC_ROOT(_lv_theme_default_styles);
    }

    if(LV_HOR_RES <= 320) disp_size = DISP_SMALL;
    else if(LV_HOR_RES < 720) disp_size = DISP_MEDIUM;
    else disp_size = DISP_LARGE;

    theme.disp = disp;
    theme.color_primary = color_primary;
    theme.color_secondary = color_secondary;
    theme.font_small = font;
    theme.font_normal = font;
    theme.font_large = font;
    theme.apply_cb = theme_apply;
    theme.flags = dark ? MODE_DARK : 0;

    style_init();

    inited = true;

    if(disp == NULL || lv_disp_get_theme(disp) == &theme) lv_obj_report_style_change(NULL);

    return (lv_theme_t *)&theme;
}

lv_theme_t * lv_theme_default_get(void)
{
    if(!inited) return NULL;

    return (lv_theme_t *)&theme;
}

bool lv_theme_default_is_inited(void)
{
    return inited;
}


static void theme_apply(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);

    if(lv_obj_get_parent(obj) == NULL) {
        lv_obj_add_style(obj, &styles->scr, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
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
            lv_obj_add_style(obj, &styles->pad_normal, 0);
            lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
            lv_obj_add_style(obj, &styles->scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
            return;
        }
#endif

#if LV_USE_WIN
        /*Header*/
        if(lv_obj_get_index(obj) == 0 && lv_obj_check_type(lv_obj_get_parent(obj), &lv_win_class)) {
            lv_obj_add_style(obj, &styles->bg_color_grey, 0);
            lv_obj_add_style(obj, &styles->pad_tiny, 0);
            return;
        }
        /*Content*/
        else if(lv_obj_get_index(obj) == 1 && lv_obj_check_type(lv_obj_get_parent(obj), &lv_win_class)) {
            lv_obj_add_style(obj, &styles->scr, 0);
            lv_obj_add_style(obj, &styles->pad_normal, 0);
            lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
            lv_obj_add_style(obj, &styles->scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
            return;
        }
#endif
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    }
#if LV_USE_BTN
    else if(lv_obj_check_type(obj, &lv_btn_class)) {
        lv_obj_add_style(obj, &styles->btn, 0);
        lv_obj_add_style(obj, &styles->bg_color_primary, 0);
        lv_obj_add_style(obj, &styles->transition_delayed, 0);
        lv_obj_add_style(obj, &styles->pressed, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->transition_normal, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
#if LV_THEME_DEFAULT_GROW
        lv_obj_add_style(obj, &styles->grow, LV_STATE_PRESSED);
#endif
        lv_obj_add_style(obj, &styles->bg_color_secondary, LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->disabled, LV_STATE_DISABLED);
    }
#endif

#if LV_USE_LINE
    else if(lv_obj_check_type(obj, &lv_line_class)) {
        lv_obj_add_style(obj, &styles->line, 0);
    }
#endif

#if LV_USE_BTNMATRIX
    else if(lv_obj_check_type(obj, &lv_btnmatrix_class)) {
#if LV_USE_MSGBOX
        if(lv_obj_check_type(lv_obj_get_parent(obj), &lv_msgbox_class)) {
            lv_obj_add_style(obj, &styles->msgbox_btn_bg, 0);
            lv_obj_add_style(obj, &styles->pad_gap, 0);
            lv_obj_add_style(obj, &styles->btn, LV_PART_ITEMS);
            lv_obj_add_style(obj, &styles->pressed, LV_PART_ITEMS | LV_STATE_PRESSED);
            lv_obj_add_style(obj, &styles->disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
            lv_obj_add_style(obj, &styles->bg_color_primary, LV_PART_ITEMS | LV_STATE_CHECKED);
            lv_obj_add_style(obj, &styles->bg_color_primary_muted, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &styles->bg_color_secondary_muted, LV_PART_ITEMS | LV_STATE_EDITED);
            return;
        }
#endif
#if LV_USE_TABVIEW
        if(lv_obj_check_type(lv_obj_get_parent(obj), &lv_tabview_class)) {
            lv_obj_add_style(obj, &styles->bg_color_white, 0);
            lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &styles->tab_bg_focus, LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &styles->pressed, LV_PART_ITEMS | LV_STATE_PRESSED);
            lv_obj_add_style(obj, &styles->bg_color_primary_muted, LV_PART_ITEMS | LV_STATE_CHECKED);
            lv_obj_add_style(obj, &styles->tab_btn, LV_PART_ITEMS | LV_STATE_CHECKED);
            lv_obj_add_style(obj, &styles->outline_primary, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &styles->outline_secondary, LV_PART_ITEMS | LV_STATE_EDITED);
            lv_obj_add_style(obj, &styles->tab_bg_focus, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            return;
        }
#endif
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->btn, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &styles->pressed, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->bg_color_primary, LV_PART_ITEMS | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->outline_primary, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_PART_ITEMS | LV_STATE_EDITED);
    }
#endif

#if LV_USE_BAR
    else if(lv_obj_check_type(obj, &lv_bar_class)) {
        lv_obj_add_style(obj, &styles->bg_color_primary_muted, 0);
        lv_obj_add_style(obj, &styles->circle, 0);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->bg_color_primary, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->circle, LV_PART_INDICATOR);
    }
#endif

#if LV_USE_SLIDER
    else if(lv_obj_check_type(obj, &lv_slider_class)) {
        lv_obj_add_style(obj, &styles->bg_color_primary_muted, 0);
        lv_obj_add_style(obj, &styles->circle, 0);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->bg_color_primary, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->circle, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->knob, LV_PART_KNOB);
#if LV_THEME_DEFAULT_GROW
        lv_obj_add_style(obj, &styles->grow, LV_PART_KNOB | LV_STATE_PRESSED);
#endif
        lv_obj_add_style(obj, &styles->transition_delayed, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->transition_normal, LV_PART_KNOB | LV_STATE_PRESSED);
    }
#endif

#if LV_USE_TABLE
    else if(lv_obj_check_type(obj, &lv_table_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->pad_zero, 0);
        lv_obj_add_style(obj, &styles->no_radius, 0);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        lv_obj_add_style(obj, &styles->bg_color_white, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->table_cell, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->pad_normal, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->pressed, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->bg_color_primary, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->bg_color_secondary, LV_PART_ITEMS | LV_STATE_EDITED);
    }
#endif

#if LV_USE_CHECKBOX
    else if(lv_obj_check_type(obj, &lv_checkbox_class)) {
        lv_obj_add_style(obj, &styles->pad_gap, 0);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->disabled, LV_PART_INDICATOR | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &styles->cb_marker, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->bg_color_primary, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->cb_marker_checked, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->pressed, LV_PART_INDICATOR | LV_STATE_PRESSED);
#if LV_THEME_DEFAULT_GROW
        lv_obj_add_style(obj, &styles->grow, LV_PART_INDICATOR | LV_STATE_PRESSED);
#endif
        lv_obj_add_style(obj, &styles->transition_normal, LV_PART_INDICATOR | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->transition_delayed, LV_PART_INDICATOR);
    }
#endif

#if LV_USE_SWITCH
    else if(lv_obj_check_type(obj, &lv_switch_class)) {
        lv_obj_add_style(obj, &styles->bg_color_grey, 0);
        lv_obj_add_style(obj, &styles->circle, 0);
        lv_obj_add_style(obj, &styles->anim_fast, 0);
        lv_obj_add_style(obj, &styles->disabled, LV_STATE_DISABLED);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->bg_color_primary, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->circle, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->disabled, LV_PART_INDICATOR | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &styles->knob, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->bg_color_white, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->switch_knob, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->disabled, LV_PART_KNOB | LV_STATE_DISABLED);

        lv_obj_add_style(obj, &styles->transition_normal, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->transition_normal, LV_PART_INDICATOR);
    }
#endif

#if LV_USE_CHART
    else if(lv_obj_check_type(obj, &lv_chart_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->pad_small, 0);
        lv_obj_add_style(obj, &styles->chart_bg, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        lv_obj_add_style(obj, &styles->chart_series, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->chart_indic, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->chart_ticks, LV_PART_TICKS);
        lv_obj_add_style(obj, &styles->chart_series, LV_PART_CURSOR);
    }
#endif

#if LV_USE_ROLLER
    else if(lv_obj_check_type(obj, &lv_roller_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->anim, 0);
        lv_obj_add_style(obj, &styles->line_space_large, 0);
        lv_obj_add_style(obj, &styles->text_align_center, 0);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->bg_color_primary, LV_PART_SELECTED);
    }
#endif

#if LV_USE_DROPDOWN
    else if(lv_obj_check_type(obj, &lv_dropdown_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->pad_small, 0);
        lv_obj_add_style(obj, &styles->transition_delayed, 0);
        lv_obj_add_style(obj, &styles->transition_normal, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->pressed, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->transition_normal, LV_PART_INDICATOR);
    }
    else if(lv_obj_check_type(obj, &lv_dropdownlist_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->clip_corner, 0);
        lv_obj_add_style(obj, &styles->line_space_large, 0);
        lv_obj_add_style(obj, &styles->dropdown_list, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        lv_obj_add_style(obj, &styles->bg_color_white, LV_PART_SELECTED);
        lv_obj_add_style(obj, &styles->bg_color_primary, LV_PART_SELECTED | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->pressed, LV_PART_SELECTED | LV_STATE_PRESSED);
    }
#endif

#if LV_USE_ARC
    else if(lv_obj_check_type(obj, &lv_arc_class)) {
        lv_obj_add_style(obj, &styles->arc_indic, 0);
        lv_obj_add_style(obj, &styles->arc_indic, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->arc_indic_primary, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->knob, LV_PART_KNOB);
    }
#endif


#if LV_USE_SPINNER
    else if(lv_obj_check_type(obj, &lv_spinner_class)) {
        lv_obj_add_style(obj, &styles->arc_indic, 0);
        lv_obj_add_style(obj, &styles->arc_indic, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles->arc_indic_primary, LV_PART_INDICATOR);
    }
#endif

#if LV_USE_METER
    else if(lv_obj_check_type(obj, &lv_meter_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->circle, 0);
        lv_obj_add_style(obj, &styles->meter_indic, LV_PART_INDICATOR);
    }
#endif

#if LV_USE_TEXTAREA
    else if(lv_obj_check_type(obj, &lv_textarea_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->pad_small, 0);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        lv_obj_add_style(obj, &styles->ta_cursor, LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_add_style(obj, &styles->ta_placeholder, LV_PART_TEXTAREA_PLACEHOLDER);
    }
#endif

#if LV_USE_CALENDAR
    else if(lv_obj_check_type(obj, &lv_calendar_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->calendar_bg, 0);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->calendar_day, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->pressed, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &styles->outline_primary, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_PART_ITEMS | LV_STATE_EDITED);
    }
#endif

#if LV_USE_CALENDAR_HEADER_ARROW
    else if(lv_obj_check_type(obj, &lv_calendar_header_arrow_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
    }
#endif

#if LV_USE_CALENDAR_HEADER_DROPDOWN
    else if(lv_obj_check_type(obj, &lv_calendar_header_dropdown_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
    }
#endif

#if LV_USE_KEYBOARD
    else if(lv_obj_check_type(obj, &lv_keyboard_class)) {
        lv_obj_add_style(obj, &styles->scr, 0);
        lv_obj_add_style(obj, disp_size == DISP_LARGE ? &styles->pad_small : &styles->pad_tiny, 0);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->btn, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &styles->bg_color_white, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->keyboard_btn_bg, LV_PART_ITEMS);
        lv_obj_add_style(obj, &styles->pressed, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->bg_color_grey, LV_PART_ITEMS | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &styles->bg_color_primary_muted, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->bg_color_secondary_muted, LV_PART_ITEMS | LV_STATE_EDITED);
    }
#endif
#if LV_USE_LIST
    else if(lv_obj_check_type(obj, &lv_list_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->list_bg, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        return;
    }
    else if(lv_obj_check_type(obj, &lv_list_text_class)) {
        lv_obj_add_style(obj, &styles->bg_color_grey, 0);
        lv_obj_add_style(obj, &styles->list_item_grow, 0);
    }
    else if(lv_obj_check_type(obj, &lv_list_btn_class)) {
        lv_obj_add_style(obj, &styles->bg_color_white, 0);
        lv_obj_add_style(obj, &styles->list_btn, 0);
        lv_obj_add_style(obj, &styles->bg_color_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->list_item_grow, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->list_item_grow, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &styles->pressed, LV_STATE_PRESSED);

    }
#endif
#if LV_USE_MSGBOX
    else if(lv_obj_check_type(obj, &lv_msgbox_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->msgbox_bg, 0);
        return;
    }
    else if(lv_obj_check_type(obj, &lv_msgbox_backdrop_class)) {
        lv_obj_add_style(obj, &styles->msgbox_backdrop_bg, 0);
    }
#endif
#if LV_USE_SPINBOX
    else if(lv_obj_check_type(obj, &lv_spinbox_class)) {
        lv_obj_add_style(obj, &styles->card, 0);
        lv_obj_add_style(obj, &styles->pad_small, 0);
        lv_obj_add_style(obj, &styles->outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles->outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &styles->bg_color_primary, LV_PART_CURSOR);
    }
#endif
#if LV_USE_TILEVIEW
    else if(lv_obj_check_type(obj, &lv_tileview_class)) {
        lv_obj_add_style(obj, &styles->scr, 0);
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    }
    else if(lv_obj_check_type(obj, &lv_tileview_tile_class)) {
        lv_obj_add_style(obj, &styles->scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles->scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    }
#endif

#if LV_USE_TABVIEW
    else if(lv_obj_check_type(obj, &lv_tabview_class)) {
        lv_obj_add_style(obj, &styles->scr, 0);
        lv_obj_add_style(obj, &styles->pad_zero, 0);
    }
#endif

#if LV_USE_WIN
    else if(lv_obj_check_type(obj, &lv_win_class)) {
        lv_obj_add_style(obj, &styles->clip_corner, 0);
    }
#endif

#if LV_USE_COLORWHEEL
    else if(lv_obj_check_type(obj, &lv_colorwheel_class)) {
        lv_obj_add_style(obj, &styles->colorwheel_main, 0);
        lv_obj_add_style(obj, &styles->pad_normal, 0);
        lv_obj_add_style(obj, &styles->bg_color_white, LV_PART_KNOB);
        lv_obj_add_style(obj, &styles->pad_normal, LV_PART_KNOB);
    }
#endif

#if LV_USE_LED
    else if(lv_obj_check_type(obj, &lv_led_class)) {
        lv_obj_add_style(obj, &styles->led, 0);
    }
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void style_init_reset(lv_style_t * style)
{
    if(inited) lv_style_reset(style);
    else lv_style_init(style);
}

#endif
