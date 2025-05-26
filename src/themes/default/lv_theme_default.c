/**
 * @file lv_theme_default.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_DEFAULT

#include "../lv_theme_private.h"
#include "../../misc/lv_color.h"
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

struct _my_theme_t;
typedef struct _my_theme_t my_theme_t;

#define theme_def (*(my_theme_t **)(&LV_GLOBAL_DEFAULT()->theme_default))

#define MODE_DARK 1
#define RADIUS_DEFAULT LV_DPX_CALC(theme->disp_dpi, theme->disp_size == DISP_LARGE ? 12 : 8)

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
#define BORDER_WIDTH            LV_DPX_CALC(theme->disp_dpi, 2)
#define OUTLINE_WIDTH           LV_DPX_CALC(theme->disp_dpi, 3)

#define PAD_DEF     LV_DPX_CALC(theme->disp_dpi, theme->disp_size == DISP_LARGE ? 24 : theme->disp_size == DISP_MEDIUM ? 20 : 16)
#define PAD_SMALL   LV_DPX_CALC(theme->disp_dpi, theme->disp_size == DISP_LARGE ? 14 : theme->disp_size == DISP_MEDIUM ? 12 : 10)
#define PAD_TINY    LV_DPX_CALC(theme->disp_dpi, theme->disp_size == DISP_LARGE ? 8 : theme->disp_size == DISP_MEDIUM ? 6 : 2)

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
    lv_style_t rotary_scroll;
#if LV_THEME_DEFAULT_GROW
    lv_style_t grow;
#endif
    lv_style_t transition_delayed;
    lv_style_t transition_normal;
    lv_style_t anim;
    lv_style_t anim_fast;

    /*Parts*/
    lv_style_t knob;

#if LV_USE_ARC
    lv_style_t arc_indic;
    lv_style_t arc_indic_primary;
#endif

#if LV_USE_CHART
    lv_style_t chart_series, chart_indic, chart_bg;
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

#if LV_USE_TEXTAREA
    lv_style_t ta_cursor, ta_placeholder;
#endif

#if LV_USE_CALENDAR
    lv_style_t calendar_btnm_bg, calendar_btnm_day, calendar_header;
#endif

#if LV_USE_MENU
    lv_style_t menu_bg, menu_cont, menu_sidebar_cont, menu_main_cont, menu_page, menu_header_cont, menu_header_btn,
               menu_section, menu_pressed, menu_separator;
#endif

#if LV_USE_MSGBOX
    lv_style_t msgbox_backdrop_bg;
#endif

#if LV_USE_KEYBOARD
    lv_style_t keyboard_button_bg;
#endif

#if LV_USE_LIST
    lv_style_t list_bg, list_btn, list_item_grow;
#endif

#if LV_USE_TABVIEW
    lv_style_t tab_bg_focus, tab_btn;
#endif
#if LV_USE_LED
    lv_style_t led;
#endif

#if LV_USE_SCALE
    lv_style_t scale;
#endif
} my_theme_styles_t;

typedef enum {
    DISP_SMALL = 3,
    DISP_MEDIUM = 2,
    DISP_LARGE = 1,
} disp_size_t;

struct _my_theme_t {
    lv_theme_t base;
    disp_size_t disp_size;
    int32_t disp_dpi;
    lv_color_t color_scr;
    lv_color_t color_text;
    lv_color_t color_card;
    lv_color_t color_grey;
    bool inited;
    my_theme_styles_t styles;

#if LV_THEME_DEFAULT_TRANSITION_TIME
    lv_style_transition_dsc_t trans_delayed;
    lv_style_transition_dsc_t trans_normal;
#endif
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void theme_apply(lv_theme_t * th, lv_obj_t * obj);
static void style_init_reset(lv_style_t * style);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void style_init(my_theme_t * theme)
{
#if TRANSITION_TIME
    static const lv_style_prop_t trans_props[] = {
        LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR,
        LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT,
        LV_STYLE_TRANSLATE_Y, LV_STYLE_TRANSLATE_X,
        LV_STYLE_TRANSFORM_ROTATION,
        LV_STYLE_TRANSFORM_SCALE_X, LV_STYLE_TRANSFORM_SCALE_Y,
        LV_STYLE_RECOLOR_OPA, LV_STYLE_RECOLOR,
        0
    };
#endif

    theme->color_scr = theme->base.flags & MODE_DARK ? DARK_COLOR_SCR : LIGHT_COLOR_SCR;
    theme->color_text = theme->base.flags & MODE_DARK ? DARK_COLOR_TEXT : LIGHT_COLOR_TEXT;
    theme->color_card = theme->base.flags & MODE_DARK ? DARK_COLOR_CARD : LIGHT_COLOR_CARD;
    theme->color_grey = theme->base.flags & MODE_DARK ? DARK_COLOR_GREY : LIGHT_COLOR_GREY;

    style_init_reset(&theme->styles.transition_delayed);
    style_init_reset(&theme->styles.transition_normal);
#if TRANSITION_TIME
    lv_style_transition_dsc_init(&theme->trans_delayed, trans_props, lv_anim_path_linear, TRANSITION_TIME, 70, NULL);
    lv_style_transition_dsc_init(&theme->trans_normal, trans_props, lv_anim_path_linear, TRANSITION_TIME, 0, NULL);

    lv_style_set_transition(&theme->styles.transition_delayed,
                            &theme->trans_delayed); /*Go back to default state with delay*/

    lv_style_set_transition(&theme->styles.transition_normal, &theme->trans_normal); /*Go back to default state with delay*/
#endif

    style_init_reset(&theme->styles.scrollbar);
    lv_color_t sb_color = (theme->base.flags & MODE_DARK) ? lv_palette_darken(LV_PALETTE_GREY,
                                                                              2) : lv_palette_main(LV_PALETTE_GREY);
    lv_style_set_bg_color(&theme->styles.scrollbar, sb_color);

    lv_style_set_radius(&theme->styles.scrollbar, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(&theme->styles.scrollbar, LV_DPX_CALC(theme->disp_dpi, 7));
    lv_style_set_width(&theme->styles.scrollbar,  LV_DPX_CALC(theme->disp_dpi, 5));
    lv_style_set_bg_opa(&theme->styles.scrollbar,  LV_OPA_40);
#if TRANSITION_TIME
    lv_style_set_transition(&theme->styles.scrollbar, &theme->trans_normal);
#endif

    style_init_reset(&theme->styles.scrollbar_scrolled);
    lv_style_set_bg_opa(&theme->styles.scrollbar_scrolled,  LV_OPA_COVER);

    style_init_reset(&theme->styles.scr);
    lv_style_set_bg_opa(&theme->styles.scr, LV_OPA_COVER);
    lv_style_set_bg_color(&theme->styles.scr, theme->color_scr);
    lv_style_set_text_color(&theme->styles.scr, theme->color_text);
    lv_style_set_text_font(&theme->styles.scr, theme->base.font_normal);
    lv_style_set_pad_row(&theme->styles.scr, PAD_SMALL);
    lv_style_set_pad_column(&theme->styles.scr, PAD_SMALL);
    lv_style_set_rotary_sensitivity(&theme->styles.scr, theme->disp_dpi / 4 * 256);

    style_init_reset(&theme->styles.card);
    lv_style_set_radius(&theme->styles.card, RADIUS_DEFAULT);
    lv_style_set_bg_opa(&theme->styles.card, LV_OPA_COVER);
    lv_style_set_bg_color(&theme->styles.card, theme->color_card);
    lv_style_set_border_color(&theme->styles.card, theme->color_grey);
    lv_style_set_border_width(&theme->styles.card, BORDER_WIDTH);
    lv_style_set_border_post(&theme->styles.card, true);
    lv_style_set_text_color(&theme->styles.card, theme->color_text);
    lv_style_set_pad_all(&theme->styles.card, PAD_DEF);
    lv_style_set_pad_row(&theme->styles.card, PAD_SMALL);
    lv_style_set_pad_column(&theme->styles.card, PAD_SMALL);
    lv_style_set_line_color(&theme->styles.card, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_line_width(&theme->styles.card, LV_DPX_CALC(theme->disp_dpi, 1));

    style_init_reset(&theme->styles.outline_primary);
    lv_style_set_outline_color(&theme->styles.outline_primary, theme->base.color_primary);
    lv_style_set_outline_width(&theme->styles.outline_primary, OUTLINE_WIDTH);
    lv_style_set_outline_pad(&theme->styles.outline_primary, OUTLINE_WIDTH);
    lv_style_set_outline_opa(&theme->styles.outline_primary, LV_OPA_50);

    style_init_reset(&theme->styles.outline_secondary);
    lv_style_set_outline_color(&theme->styles.outline_secondary, theme->base.color_secondary);
    lv_style_set_outline_width(&theme->styles.outline_secondary, OUTLINE_WIDTH);
    lv_style_set_outline_opa(&theme->styles.outline_secondary, LV_OPA_50);

    style_init_reset(&theme->styles.btn);
    lv_style_set_radius(&theme->styles.btn,
                        LV_DPX_CALC(theme->disp_dpi, theme->disp_size == DISP_LARGE ? 16 : theme->disp_size == DISP_MEDIUM ? 12 : 8));
    lv_style_set_bg_opa(&theme->styles.btn, LV_OPA_COVER);
    lv_style_set_bg_color(&theme->styles.btn, theme->color_grey);
    if(!(theme->base.flags & MODE_DARK)) {
        lv_style_set_shadow_color(&theme->styles.btn, lv_palette_main(LV_PALETTE_GREY));
        lv_style_set_shadow_width(&theme->styles.btn, LV_DPX(3));
        lv_style_set_shadow_opa(&theme->styles.btn, LV_OPA_50);
        lv_style_set_shadow_offset_y(&theme->styles.btn, LV_DPX_CALC(theme->disp_dpi, LV_DPX(4)));
    }
    lv_style_set_text_color(&theme->styles.btn, theme->color_text);
    lv_style_set_pad_hor(&theme->styles.btn, PAD_DEF);
    lv_style_set_pad_ver(&theme->styles.btn, PAD_SMALL);
    lv_style_set_pad_column(&theme->styles.btn, LV_DPX_CALC(theme->disp_dpi, 5));
    lv_style_set_pad_row(&theme->styles.btn, LV_DPX_CALC(theme->disp_dpi, 5));

    style_init_reset(&theme->styles.pressed);
    lv_style_set_recolor(&theme->styles.pressed, lv_color_black());
    lv_style_set_recolor_opa(&theme->styles.pressed, 35);

    style_init_reset(&theme->styles.disabled);
    if(theme_def->base.flags & MODE_DARK)
        lv_style_set_recolor(&theme->styles.disabled, lv_palette_darken(LV_PALETTE_GREY, 2));
    else
        lv_style_set_recolor(&theme->styles.disabled, lv_palette_lighten(LV_PALETTE_GREY, 2));
    lv_style_set_recolor_opa(&theme->styles.disabled, LV_OPA_50);

    style_init_reset(&theme->styles.clip_corner);
    lv_style_set_clip_corner(&theme->styles.clip_corner, true);
    lv_style_set_border_post(&theme->styles.clip_corner, true);

    style_init_reset(&theme->styles.pad_normal);
    lv_style_set_pad_all(&theme->styles.pad_normal, PAD_DEF);
    lv_style_set_pad_row(&theme->styles.pad_normal, PAD_DEF);
    lv_style_set_pad_column(&theme->styles.pad_normal, PAD_DEF);

    style_init_reset(&theme->styles.pad_small);
    lv_style_set_pad_all(&theme->styles.pad_small, PAD_SMALL);
    lv_style_set_pad_gap(&theme->styles.pad_small, PAD_SMALL);

    style_init_reset(&theme->styles.pad_gap);
    lv_style_set_pad_row(&theme->styles.pad_gap, LV_DPX_CALC(theme->disp_dpi, 10));
    lv_style_set_pad_column(&theme->styles.pad_gap, LV_DPX_CALC(theme->disp_dpi, 10));

    style_init_reset(&theme->styles.line_space_large);
    lv_style_set_text_line_space(&theme->styles.line_space_large, LV_DPX_CALC(theme->disp_dpi, 20));

    style_init_reset(&theme->styles.text_align_center);
    lv_style_set_text_align(&theme->styles.text_align_center, LV_TEXT_ALIGN_CENTER);

    style_init_reset(&theme->styles.pad_zero);
    lv_style_set_pad_all(&theme->styles.pad_zero, 0);
    lv_style_set_pad_row(&theme->styles.pad_zero, 0);
    lv_style_set_pad_column(&theme->styles.pad_zero, 0);

    style_init_reset(&theme->styles.pad_tiny);
    lv_style_set_pad_all(&theme->styles.pad_tiny, PAD_TINY);
    lv_style_set_pad_row(&theme->styles.pad_tiny, PAD_TINY);
    lv_style_set_pad_column(&theme->styles.pad_tiny, PAD_TINY);

    style_init_reset(&theme->styles.bg_color_primary);
    lv_style_set_bg_color(&theme->styles.bg_color_primary, theme->base.color_primary);
    lv_style_set_text_color(&theme->styles.bg_color_primary, lv_color_white());
    lv_style_set_bg_opa(&theme->styles.bg_color_primary, LV_OPA_COVER);

    style_init_reset(&theme->styles.bg_color_primary_muted);
    lv_style_set_bg_color(&theme->styles.bg_color_primary_muted, theme->base.color_primary);
    lv_style_set_text_color(&theme->styles.bg_color_primary_muted, theme->base.color_primary);
    lv_style_set_bg_opa(&theme->styles.bg_color_primary_muted, LV_OPA_20);

    style_init_reset(&theme->styles.bg_color_secondary);
    lv_style_set_bg_color(&theme->styles.bg_color_secondary, theme->base.color_secondary);
    lv_style_set_text_color(&theme->styles.bg_color_secondary, lv_color_white());
    lv_style_set_bg_opa(&theme->styles.bg_color_secondary, LV_OPA_COVER);

    style_init_reset(&theme->styles.bg_color_secondary_muted);
    lv_style_set_bg_color(&theme->styles.bg_color_secondary_muted, theme->base.color_secondary);
    lv_style_set_text_color(&theme->styles.bg_color_secondary_muted, theme->base.color_secondary);
    lv_style_set_bg_opa(&theme->styles.bg_color_secondary_muted, LV_OPA_20);

    style_init_reset(&theme->styles.bg_color_grey);
    lv_style_set_bg_color(&theme->styles.bg_color_grey, theme->color_grey);
    lv_style_set_bg_opa(&theme->styles.bg_color_grey, LV_OPA_COVER);
    lv_style_set_text_color(&theme->styles.bg_color_grey, theme->color_text);

    style_init_reset(&theme->styles.bg_color_white);
    lv_style_set_bg_color(&theme->styles.bg_color_white, theme->color_card);
    lv_style_set_bg_opa(&theme->styles.bg_color_white, LV_OPA_COVER);
    lv_style_set_text_color(&theme->styles.bg_color_white, theme->color_text);

    style_init_reset(&theme->styles.circle);
    lv_style_set_radius(&theme->styles.circle, LV_RADIUS_CIRCLE);

    style_init_reset(&theme->styles.no_radius);
    lv_style_set_radius(&theme->styles.no_radius, 0);

    style_init_reset(&theme->styles.rotary_scroll);
    lv_style_set_rotary_sensitivity(&theme->styles.rotary_scroll, theme->disp_dpi / 4 * 256);

#if LV_THEME_DEFAULT_GROW
    style_init_reset(&theme->styles.grow);
    lv_style_set_transform_width(&theme->styles.grow, LV_DPX_CALC(theme->disp_dpi, 3));
    lv_style_set_transform_height(&theme->styles.grow, LV_DPX_CALC(theme->disp_dpi, 3));
#endif

    style_init_reset(&theme->styles.knob);
    lv_style_set_bg_color(&theme->styles.knob, theme->base.color_primary);
    lv_style_set_bg_opa(&theme->styles.knob, LV_OPA_COVER);
    lv_style_set_pad_all(&theme->styles.knob, LV_DPX_CALC(theme->disp_dpi, 6));
    lv_style_set_radius(&theme->styles.knob, LV_RADIUS_CIRCLE);

    style_init_reset(&theme->styles.anim);
    lv_style_set_anim_duration(&theme->styles.anim, 200);

    style_init_reset(&theme->styles.anim_fast);
    lv_style_set_anim_duration(&theme->styles.anim_fast, 120);

#if LV_USE_ARC
    style_init_reset(&theme->styles.arc_indic);
    lv_style_set_arc_color(&theme->styles.arc_indic, theme->color_grey);
    lv_style_set_arc_width(&theme->styles.arc_indic, LV_DPX_CALC(theme->disp_dpi, 15));
    lv_style_set_arc_rounded(&theme->styles.arc_indic, true);

    style_init_reset(&theme->styles.arc_indic_primary);
    lv_style_set_arc_color(&theme->styles.arc_indic_primary, theme->base.color_primary);
#endif

#if LV_USE_DROPDOWN
    style_init_reset(&theme->styles.dropdown_list);
    lv_style_set_max_height(&theme->styles.dropdown_list, LV_DPI_DEF * 2);
#endif
#if LV_USE_CHECKBOX
    style_init_reset(&theme->styles.cb_marker);
    lv_style_set_pad_all(&theme->styles.cb_marker, LV_DPX_CALC(theme->disp_dpi, 3));
    lv_style_set_border_width(&theme->styles.cb_marker, BORDER_WIDTH);
    lv_style_set_border_color(&theme->styles.cb_marker, theme->base.color_primary);
    lv_style_set_bg_color(&theme->styles.cb_marker, theme->color_card);
    lv_style_set_bg_opa(&theme->styles.cb_marker, LV_OPA_COVER);
    lv_style_set_radius(&theme->styles.cb_marker, RADIUS_DEFAULT / 2);
    lv_style_set_text_font(&theme->styles.cb_marker, theme->base.font_small);
    lv_style_set_text_color(&theme->styles.cb_marker, lv_color_white());

    style_init_reset(&theme->styles.cb_marker_checked);
    lv_style_set_bg_image_src(&theme->styles.cb_marker_checked, LV_SYMBOL_OK);
#endif

#if LV_USE_SWITCH
    style_init_reset(&theme->styles.switch_knob);
    lv_style_set_pad_all(&theme->styles.switch_knob, - LV_DPX_CALC(theme->disp_dpi, 4));
    lv_style_set_bg_color(&theme->styles.switch_knob, lv_color_white());
#endif

#if LV_USE_LINE
    style_init_reset(&theme->styles.line);
    lv_style_set_line_width(&theme->styles.line, 1);
    lv_style_set_line_color(&theme->styles.line, theme->color_text);
#endif

#if LV_USE_CHART
    style_init_reset(&theme->styles.chart_bg);
    lv_style_set_border_post(&theme->styles.chart_bg, false);
    lv_style_set_pad_column(&theme->styles.chart_bg, LV_DPX_CALC(theme->disp_dpi, 10));
    lv_style_set_line_color(&theme->styles.chart_bg, theme->color_grey);

    style_init_reset(&theme->styles.chart_series);
    lv_style_set_line_width(&theme->styles.chart_series, LV_DPX_CALC(theme->disp_dpi, 3));
    lv_style_set_radius(&theme->styles.chart_series, LV_DPX_CALC(theme->disp_dpi, 3));

    int32_t chart_size = LV_DPX_CALC(theme->disp_dpi, 8);
    lv_style_set_size(&theme->styles.chart_series, chart_size, chart_size);
    lv_style_set_pad_column(&theme->styles.chart_series, LV_DPX_CALC(theme->disp_dpi, 2));

    style_init_reset(&theme->styles.chart_indic);
    lv_style_set_radius(&theme->styles.chart_indic, LV_RADIUS_CIRCLE);
    lv_style_set_size(&theme->styles.chart_indic, chart_size, chart_size);
    lv_style_set_bg_color(&theme->styles.chart_indic, theme->base.color_primary);
    lv_style_set_bg_opa(&theme->styles.chart_indic, LV_OPA_COVER);
#endif

#if LV_USE_MENU
    style_init_reset(&theme->styles.menu_bg);
    lv_style_set_pad_all(&theme->styles.menu_bg, 0);
    lv_style_set_pad_gap(&theme->styles.menu_bg, 0);
    lv_style_set_radius(&theme->styles.menu_bg, 0);
    lv_style_set_clip_corner(&theme->styles.menu_bg, true);
    lv_style_set_border_side(&theme->styles.menu_bg, LV_BORDER_SIDE_NONE);

    style_init_reset(&theme->styles.menu_section);
    lv_style_set_radius(&theme->styles.menu_section, RADIUS_DEFAULT);
    lv_style_set_clip_corner(&theme->styles.menu_section, true);
    lv_style_set_bg_opa(&theme->styles.menu_section, LV_OPA_COVER);
    lv_style_set_bg_color(&theme->styles.menu_section, theme->color_card);
    lv_style_set_text_color(&theme->styles.menu_section, theme->color_text);

    style_init_reset(&theme->styles.menu_cont);
    lv_style_set_pad_hor(&theme->styles.menu_cont, PAD_SMALL);
    lv_style_set_pad_ver(&theme->styles.menu_cont, PAD_SMALL);
    lv_style_set_pad_gap(&theme->styles.menu_cont, PAD_SMALL);
    lv_style_set_border_width(&theme->styles.menu_cont, LV_DPX_CALC(theme->disp_dpi, 1));
    lv_style_set_border_opa(&theme->styles.menu_cont, LV_OPA_10);
    lv_style_set_border_color(&theme->styles.menu_cont, theme->color_text);
    lv_style_set_border_side(&theme->styles.menu_cont, LV_BORDER_SIDE_NONE);

    style_init_reset(&theme->styles.menu_sidebar_cont);
    lv_style_set_pad_all(&theme->styles.menu_sidebar_cont, 0);
    lv_style_set_pad_gap(&theme->styles.menu_sidebar_cont, 0);
    lv_style_set_border_width(&theme->styles.menu_sidebar_cont, LV_DPX_CALC(theme->disp_dpi, 1));
    lv_style_set_border_opa(&theme->styles.menu_sidebar_cont, LV_OPA_10);
    lv_style_set_border_color(&theme->styles.menu_sidebar_cont, theme->color_text);
    lv_style_set_border_side(&theme->styles.menu_sidebar_cont, LV_BORDER_SIDE_RIGHT);

    style_init_reset(&theme->styles.menu_main_cont);
    lv_style_set_pad_all(&theme->styles.menu_main_cont, 0);
    lv_style_set_pad_gap(&theme->styles.menu_main_cont, 0);

    style_init_reset(&theme->styles.menu_header_cont);
    lv_style_set_pad_hor(&theme->styles.menu_header_cont, PAD_SMALL);
    lv_style_set_pad_ver(&theme->styles.menu_header_cont, PAD_TINY);
    lv_style_set_pad_gap(&theme->styles.menu_header_cont, PAD_SMALL);

    style_init_reset(&theme->styles.menu_header_btn);
    lv_style_set_pad_hor(&theme->styles.menu_header_btn, PAD_TINY);
    lv_style_set_pad_ver(&theme->styles.menu_header_btn, PAD_TINY);
    lv_style_set_shadow_opa(&theme->styles.menu_header_btn, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&theme->styles.menu_header_btn, LV_OPA_TRANSP);
    lv_style_set_text_color(&theme->styles.menu_header_btn, theme->color_text);

    style_init_reset(&theme->styles.menu_page);
    lv_style_set_pad_hor(&theme->styles.menu_page, 0);
    lv_style_set_pad_gap(&theme->styles.menu_page, 0);

    style_init_reset(&theme->styles.menu_pressed);
    lv_style_set_bg_opa(&theme->styles.menu_pressed, LV_OPA_20);
    lv_style_set_bg_color(&theme->styles.menu_pressed, lv_palette_main(LV_PALETTE_GREY));

    style_init_reset(&theme->styles.menu_separator);
    lv_style_set_bg_opa(&theme->styles.menu_separator, LV_OPA_TRANSP);
    lv_style_set_pad_ver(&theme->styles.menu_separator, PAD_TINY);
#endif

#if LV_USE_TABLE
    style_init_reset(&theme->styles.table_cell);
    lv_style_set_border_width(&theme->styles.table_cell, LV_DPX_CALC(theme->disp_dpi, 1));
    lv_style_set_border_color(&theme->styles.table_cell, theme->color_grey);
    lv_style_set_border_side(&theme->styles.table_cell, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
#endif

#if LV_USE_TEXTAREA
    style_init_reset(&theme->styles.ta_cursor);
    lv_style_set_border_color(&theme->styles.ta_cursor, theme->color_text);
    lv_style_set_border_width(&theme->styles.ta_cursor, LV_DPX_CALC(theme->disp_dpi, 2));
    lv_style_set_pad_left(&theme->styles.ta_cursor, - LV_DPX_CALC(theme->disp_dpi, 1));
    lv_style_set_border_side(&theme->styles.ta_cursor, LV_BORDER_SIDE_LEFT);
    lv_style_set_anim_duration(&theme->styles.ta_cursor, 400);

    style_init_reset(&theme->styles.ta_placeholder);
    lv_style_set_text_color(&theme->styles.ta_placeholder,
                            (theme->base.flags & MODE_DARK) ? lv_palette_darken(LV_PALETTE_GREY,
                                                                                2) : lv_palette_lighten(LV_PALETTE_GREY, 1));
#endif

#if LV_USE_CALENDAR
    style_init_reset(&theme->styles.calendar_btnm_bg);
    lv_style_set_pad_all(&theme->styles.calendar_btnm_bg, PAD_SMALL);
    lv_style_set_pad_gap(&theme->styles.calendar_btnm_bg, PAD_SMALL / 2);

    style_init_reset(&theme->styles.calendar_btnm_day);
    lv_style_set_border_width(&theme->styles.calendar_btnm_day, LV_DPX_CALC(theme->disp_dpi, 1));
    lv_style_set_border_color(&theme->styles.calendar_btnm_day, theme->color_grey);
    lv_style_set_bg_color(&theme->styles.calendar_btnm_day, theme->color_card);
    lv_style_set_bg_opa(&theme->styles.calendar_btnm_day, LV_OPA_20);

    style_init_reset(&theme->styles.calendar_header);
    lv_style_set_pad_hor(&theme->styles.calendar_header, PAD_SMALL);
    lv_style_set_pad_top(&theme->styles.calendar_header, PAD_SMALL);
    lv_style_set_pad_bottom(&theme->styles.calendar_header, PAD_TINY);
    lv_style_set_pad_gap(&theme->styles.calendar_header, PAD_SMALL);
#endif

#if LV_USE_MSGBOX
    style_init_reset(&theme->styles.msgbox_backdrop_bg);
    lv_style_set_bg_color(&theme->styles.msgbox_backdrop_bg, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_opa(&theme->styles.msgbox_backdrop_bg, LV_OPA_50);
#endif
#if LV_USE_KEYBOARD
    style_init_reset(&theme->styles.keyboard_button_bg);
    lv_style_set_shadow_width(&theme->styles.keyboard_button_bg, 0);
    lv_style_set_radius(&theme->styles.keyboard_button_bg,
                        theme->disp_size == DISP_SMALL ? RADIUS_DEFAULT / 2 : RADIUS_DEFAULT);
#endif

#if LV_USE_TABVIEW
    style_init_reset(&theme->styles.tab_btn);
    lv_style_set_border_color(&theme->styles.tab_btn, theme->base.color_primary);
    lv_style_set_border_width(&theme->styles.tab_btn, BORDER_WIDTH * 2);
    lv_style_set_border_side(&theme->styles.tab_btn, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_top(&theme->styles.tab_btn, BORDER_WIDTH * 2);

    style_init_reset(&theme->styles.tab_bg_focus);
    lv_style_set_outline_pad(&theme->styles.tab_bg_focus, -BORDER_WIDTH);
#endif

#if LV_USE_LIST
    style_init_reset(&theme->styles.list_bg);
    lv_style_set_pad_hor(&theme->styles.list_bg, PAD_DEF);
    lv_style_set_pad_ver(&theme->styles.list_bg, 0);
    lv_style_set_pad_gap(&theme->styles.list_bg, 0);
    lv_style_set_clip_corner(&theme->styles.list_bg, true);

    style_init_reset(&theme->styles.list_btn);
    lv_style_set_border_width(&theme->styles.list_btn, LV_DPX_CALC(theme->disp_dpi, 1));
    lv_style_set_border_color(&theme->styles.list_btn, theme->color_grey);
    lv_style_set_border_side(&theme->styles.list_btn, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_all(&theme->styles.list_btn, PAD_SMALL);
    lv_style_set_pad_column(&theme->styles.list_btn, PAD_SMALL);

    style_init_reset(&theme->styles.list_item_grow);
    lv_style_set_transform_width(&theme->styles.list_item_grow, PAD_DEF);
#endif

#if LV_USE_LED
    style_init_reset(&theme->styles.led);
    lv_style_set_bg_opa(&theme->styles.led, LV_OPA_COVER);
    lv_style_set_bg_color(&theme->styles.led, lv_color_white());
    lv_style_set_bg_grad_color(&theme->styles.led, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_radius(&theme->styles.led, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_width(&theme->styles.led, LV_DPX_CALC(theme->disp_dpi, 15));
    lv_style_set_shadow_color(&theme->styles.led, lv_color_white());
    lv_style_set_shadow_spread(&theme->styles.led, LV_DPX_CALC(theme->disp_dpi, 5));
#endif

#if LV_USE_SCALE
    style_init_reset(&theme->styles.scale);
    lv_style_set_line_color(&theme->styles.scale, theme->color_text);
    lv_style_set_line_width(&theme->styles.scale, LV_DPX(2));
    lv_style_set_arc_color(&theme->styles.scale, theme->color_text);
    lv_style_set_arc_width(&theme->styles.scale, LV_DPX(2));
    lv_style_set_length(&theme->styles.scale, LV_DPX(6));
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_theme_t * lv_theme_default_init(lv_display_t * disp, lv_color_t color_primary, lv_color_t color_secondary, bool dark,
                                   const lv_font_t * font)
{
    /*This trick is required only to avoid the garbage collection of
     *styles' data if LVGL is used in a binding (e.g. MicroPython)
     *In a general case styles could be in a simple `static lv_style_t my_style...` variables*/

    if(!lv_theme_default_is_inited()) {
        theme_def = lv_malloc_zeroed(sizeof(my_theme_t));
    }

    my_theme_t * theme = theme_def;

    lv_display_t * new_disp = disp == NULL ? lv_display_get_default() : disp;
    int32_t new_dpi = lv_display_get_dpi(new_disp);
    int32_t hor_res = lv_display_get_horizontal_resolution(new_disp);
    disp_size_t new_size;

    if(hor_res <= 320) new_size = DISP_SMALL;
    else if(hor_res < 720) new_size = DISP_MEDIUM;
    else new_size = DISP_LARGE;

    /* check theme information whether will change or not*/
    if(theme->inited && theme->disp_dpi == new_dpi &&
       theme->disp_size == new_size &&
       lv_color_eq(theme->base.color_primary, color_primary) &&
       lv_color_eq(theme->base.color_secondary, color_secondary) &&
       (theme->base.flags == (dark ? MODE_DARK : 0)) &&
       theme->base.font_small == font) {
        return (lv_theme_t *) theme;

    }

    theme->disp_size = new_size;
    theme->disp_dpi = new_dpi;
    theme->base.disp = new_disp;
    theme->base.color_primary = color_primary;
    theme->base.color_secondary = color_secondary;
    theme->base.font_small = font;
    theme->base.font_normal = font;
    theme->base.font_large = font;
    theme->base.apply_cb = theme_apply;
    theme->base.flags = dark ? MODE_DARK : 0;

    style_init(theme);

    if(disp == NULL || lv_display_get_theme(disp) == (lv_theme_t *)theme) lv_obj_report_style_change(NULL);

    theme->inited = true;

    return (lv_theme_t *) theme;
}

void lv_theme_default_deinit(void)
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

lv_theme_t * lv_theme_default_get(void)
{
    if(!lv_theme_default_is_inited()) {
        return NULL;
    }

    return (lv_theme_t *)theme_def;
}

bool lv_theme_default_is_inited(void)
{
    my_theme_t * theme = theme_def;
    if(theme == NULL) return false;
    return theme->inited;
}

static void theme_apply(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);

    my_theme_t * theme = theme_def;
    lv_obj_t * parent = lv_obj_get_parent(obj);

    if(parent == NULL) {
        lv_obj_add_style(obj, &theme->styles.scr, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        return;
    }

    if(lv_obj_check_type(obj, &lv_obj_class)) {
#if LV_USE_TABVIEW
        /*Tabview content area*/
        if(lv_obj_check_type(parent, &lv_tabview_class) && lv_obj_get_child(parent, 1) == obj) {
            return;
        }
        /*Tabview button container*/
        else if(lv_obj_check_type(parent, &lv_tabview_class) && lv_obj_get_child(parent, 0) == obj) {
            lv_obj_add_style(obj, &theme->styles.bg_color_white, 0);
            lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &theme->styles.tab_bg_focus, LV_STATE_FOCUS_KEY);
            return;
        }
        /*Tabview pages*/
        else if(lv_obj_check_type(lv_obj_get_parent(parent), &lv_tabview_class)) {
            lv_obj_add_style(obj, &theme->styles.pad_normal, 0);
            lv_obj_add_style(obj, &theme->styles.rotary_scroll, 0);
            lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
            lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
            return;
        }
#endif

#if LV_USE_WIN
        /*Header*/
        if(lv_obj_check_type(parent, &lv_win_class) && lv_obj_get_child(parent, 0) == obj) {
            lv_obj_add_style(obj, &theme->styles.bg_color_grey, 0);
            lv_obj_add_style(obj, &theme->styles.pad_tiny, 0);
            return;
        }
        /*Content*/
        else if(lv_obj_check_type(parent, &lv_win_class) && lv_obj_get_child(parent, 1) == obj) {
            lv_obj_add_style(obj, &theme->styles.scr, 0);
            lv_obj_add_style(obj, &theme->styles.pad_normal, 0);
            lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
            lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
            return;
        }
#endif

#if LV_USE_CALENDAR
        if(lv_obj_check_type(parent, &lv_calendar_class)) {
            /*No style*/
            return;
        }
#endif

        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    }
#if LV_USE_BUTTON
    else if(lv_obj_check_type(obj, &lv_button_class)) {

#if LV_USE_TABVIEW
        lv_obj_t * tv = lv_obj_get_parent(parent); /*parent is the tabview header*/
        if(tv && lv_obj_get_child(tv, 0) == parent) { /*The button is on the tab view header*/
            if(lv_obj_check_type(tv, &lv_tabview_class)) {
                lv_obj_add_style(obj, &theme->styles.pressed, LV_STATE_PRESSED);
                lv_obj_add_style(obj, &theme->styles.bg_color_primary_muted, LV_STATE_CHECKED);
                lv_obj_add_style(obj, &theme->styles.tab_btn, LV_STATE_CHECKED);
                lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
                lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_STATE_EDITED);
                lv_obj_add_style(obj, &theme->styles.tab_bg_focus, LV_STATE_FOCUS_KEY);
                return;
            }
        }

#endif
        lv_obj_add_style(obj, &theme->styles.btn, 0);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, 0);
        lv_obj_add_style(obj, &theme->styles.transition_delayed, 0);
        lv_obj_add_style(obj, &theme->styles.pressed, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.transition_normal, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
#if LV_THEME_DEFAULT_GROW
        lv_obj_add_style(obj, &theme->styles.grow, LV_STATE_PRESSED);
#endif
        lv_obj_add_style(obj, &theme->styles.bg_color_secondary, LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_STATE_DISABLED);

#if LV_USE_MENU
        if(lv_obj_check_type(parent, &lv_menu_sidebar_header_cont_class) ||
           lv_obj_check_type(parent, &lv_menu_main_header_cont_class)) {
            lv_obj_add_style(obj, &theme->styles.menu_header_btn, 0);
            lv_obj_add_style(obj, &theme->styles.menu_pressed, LV_STATE_PRESSED);
        }
#endif
    }
#endif

#if LV_USE_LINE
    else if(lv_obj_check_type(obj, &lv_line_class)) {
        lv_obj_add_style(obj, &theme->styles.line, 0);
    }
#endif

#if LV_USE_BUTTONMATRIX
    else if(lv_obj_check_type(obj, &lv_buttonmatrix_class)) {

#if LV_USE_CALENDAR
        if(lv_obj_check_type(parent, &lv_calendar_class)) {
            lv_obj_add_style(obj, &theme->styles.calendar_btnm_bg, 0);
            lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_STATE_EDITED);
            lv_obj_add_style(obj, &theme->styles.calendar_btnm_day, LV_PART_ITEMS);
            lv_obj_add_style(obj, &theme->styles.pressed, LV_PART_ITEMS | LV_STATE_PRESSED);
            lv_obj_add_style(obj, &theme->styles.disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
            lv_obj_add_style(obj, &theme->styles.outline_primary, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
            lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_PART_ITEMS | LV_STATE_EDITED);
            return;
        }
#endif
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &theme->styles.btn, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &theme->styles.pressed, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_PART_ITEMS | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_PART_ITEMS | LV_STATE_EDITED);
    }
#endif

#if LV_USE_BAR
    else if(lv_obj_check_type(obj, &lv_bar_class)) {
        lv_obj_add_style(obj, &theme->styles.bg_color_primary_muted, 0);
        lv_obj_add_style(obj, &theme->styles.circle, 0);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.circle, LV_PART_INDICATOR);
    }
#endif

#if LV_USE_SLIDER
    else if(lv_obj_check_type(obj, &lv_slider_class)) {
        lv_obj_add_style(obj, &theme->styles.bg_color_primary_muted, 0);
        lv_obj_add_style(obj, &theme->styles.circle, 0);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.circle, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.knob, LV_PART_KNOB);
#if LV_THEME_DEFAULT_GROW
        lv_obj_add_style(obj, &theme->styles.grow, LV_PART_KNOB | LV_STATE_PRESSED);
#endif
        lv_obj_add_style(obj, &theme->styles.transition_delayed, LV_PART_KNOB);
        lv_obj_add_style(obj, &theme->styles.transition_normal, LV_PART_KNOB | LV_STATE_PRESSED);
    }
#endif

#if LV_USE_TABLE
    else if(lv_obj_check_type(obj, &lv_table_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pad_zero, 0);
        lv_obj_add_style(obj, &theme->styles.no_radius, 0);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        lv_obj_add_style(obj, &theme->styles.bg_color_white, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.table_cell, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.pad_normal, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.pressed, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.bg_color_secondary, LV_PART_ITEMS | LV_STATE_EDITED);
    }
#endif

#if LV_USE_CHECKBOX
    else if(lv_obj_check_type(obj, &lv_checkbox_class)) {
        lv_obj_add_style(obj, &theme->styles.pad_gap, 0);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_PART_INDICATOR | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &theme->styles.cb_marker, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.cb_marker_checked, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.pressed, LV_PART_INDICATOR | LV_STATE_PRESSED);
#if LV_THEME_DEFAULT_GROW
        lv_obj_add_style(obj, &theme->styles.grow, LV_PART_INDICATOR | LV_STATE_PRESSED);
#endif
        lv_obj_add_style(obj, &theme->styles.transition_normal, LV_PART_INDICATOR | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.transition_delayed, LV_PART_INDICATOR);
    }
#endif

#if LV_USE_SWITCH
    else if(lv_obj_check_type(obj, &lv_switch_class)) {
        lv_obj_add_style(obj, &theme->styles.bg_color_grey, 0);
        lv_obj_add_style(obj, &theme->styles.circle, 0);
        lv_obj_add_style(obj, &theme->styles.anim_fast, 0);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_STATE_DISABLED);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.circle, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.knob, LV_PART_KNOB);
        lv_obj_add_style(obj, &theme->styles.bg_color_white, LV_PART_KNOB);
        lv_obj_add_style(obj, &theme->styles.switch_knob, LV_PART_KNOB);

        lv_obj_add_style(obj, &theme->styles.transition_normal, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.transition_normal, LV_PART_INDICATOR);
    }
#endif

#if LV_USE_CHART
    else if(lv_obj_check_type(obj, &lv_chart_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pad_small, 0);
        lv_obj_add_style(obj, &theme->styles.chart_bg, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        lv_obj_add_style(obj, &theme->styles.chart_series, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.chart_indic, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.chart_series, LV_PART_CURSOR);
    }
#endif

#if LV_USE_ROLLER
    else if(lv_obj_check_type(obj, &lv_roller_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.anim, 0);
        lv_obj_add_style(obj, &theme->styles.line_space_large, 0);
        lv_obj_add_style(obj, &theme->styles.text_align_center, 0);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_PART_SELECTED);
    }
#endif

#if LV_USE_DROPDOWN
    else if(lv_obj_check_type(obj, &lv_dropdown_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pad_small, 0);
        lv_obj_add_style(obj, &theme->styles.transition_delayed, 0);
        lv_obj_add_style(obj, &theme->styles.transition_normal, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.pressed, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &theme->styles.transition_normal, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_STATE_DISABLED);
    }
    else if(lv_obj_check_type(obj, &lv_dropdownlist_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.clip_corner, 0);
        lv_obj_add_style(obj, &theme->styles.line_space_large, 0);
        lv_obj_add_style(obj, &theme->styles.dropdown_list, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        lv_obj_add_style(obj, &theme->styles.bg_color_white, LV_PART_SELECTED);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_PART_SELECTED | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.pressed, LV_PART_SELECTED | LV_STATE_PRESSED);
    }
#endif

#if LV_USE_ARC
    else if(lv_obj_check_type(obj, &lv_arc_class)) {
        lv_obj_add_style(obj, &theme->styles.arc_indic, 0);
        lv_obj_add_style(obj, &theme->styles.arc_indic, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.arc_indic_primary, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.knob, LV_PART_KNOB);
    }
#endif

#if LV_USE_SPINNER
    else if(lv_obj_check_type(obj, &lv_spinner_class)) {
        lv_obj_add_style(obj, &theme->styles.arc_indic, 0);
        lv_obj_add_style(obj, &theme->styles.arc_indic, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.arc_indic_primary, LV_PART_INDICATOR);
    }
#endif

#if LV_USE_TEXTAREA
    else if(lv_obj_check_type(obj, &lv_textarea_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pad_small, 0);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_STATE_DISABLED);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        lv_obj_add_style(obj, &theme->styles.ta_cursor, LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_add_style(obj, &theme->styles.ta_placeholder, LV_PART_TEXTAREA_PLACEHOLDER);
    }
#endif

#if LV_USE_CALENDAR
    else if(lv_obj_check_type(obj, &lv_calendar_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pad_zero, 0);
    }

#if LV_USE_CALENDAR_HEADER_ARROW
    else if(lv_obj_check_type(obj, &lv_calendar_header_arrow_class)) {
        lv_obj_add_style(obj, &theme->styles.calendar_header, 0);
    }
#endif

#if LV_USE_CALENDAR_HEADER_DROPDOWN
    else if(lv_obj_check_type(obj, &lv_calendar_header_dropdown_class)) {
        lv_obj_add_style(obj, &theme->styles.calendar_header, 0);
    }
#endif
#endif

#if LV_USE_KEYBOARD
    else if(lv_obj_check_type(obj, &lv_keyboard_class)) {
        lv_obj_add_style(obj, &theme->styles.scr, 0);
        lv_obj_add_style(obj, theme->disp_size == DISP_LARGE ? &theme->styles.pad_small : &theme->styles.pad_tiny, 0);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &theme->styles.btn, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_PART_ITEMS | LV_STATE_DISABLED);
        lv_obj_add_style(obj, &theme->styles.bg_color_white, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.keyboard_button_bg, LV_PART_ITEMS);
        lv_obj_add_style(obj, &theme->styles.pressed, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.bg_color_grey, LV_PART_ITEMS | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary_muted, LV_PART_ITEMS | LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.bg_color_secondary_muted, LV_PART_ITEMS | LV_STATE_EDITED);
    }
#endif

#if LV_USE_LABEL && LV_USE_TEXTAREA
    else if(lv_obj_check_type(obj, &lv_label_class) && lv_obj_check_type(parent, &lv_textarea_class)) {
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_PART_SELECTED);
    }
#endif

#if LV_USE_LIST
    else if(lv_obj_check_type(obj, &lv_list_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.list_bg, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        return;
    }
    else if(lv_obj_check_type(obj, &lv_list_text_class)) {
        lv_obj_add_style(obj, &theme->styles.bg_color_grey, 0);
        lv_obj_add_style(obj, &theme->styles.list_item_grow, 0);
    }
    else if(lv_obj_check_type(obj, &lv_list_button_class)) {
        lv_obj_add_style(obj, &theme->styles.bg_color_white, 0);
        lv_obj_add_style(obj, &theme->styles.list_btn, 0);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.list_item_grow, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.list_item_grow, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.pressed, LV_STATE_PRESSED);

    }
#endif
#if LV_USE_MENU
    else if(lv_obj_check_type(obj, &lv_menu_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.menu_bg, 0);
    }
    else if(lv_obj_check_type(obj, &lv_menu_sidebar_cont_class)) {
        lv_obj_add_style(obj, &theme->styles.menu_sidebar_cont, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    }
    else if(lv_obj_check_type(obj, &lv_menu_main_cont_class)) {
        lv_obj_add_style(obj, &theme->styles.menu_main_cont, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    }
    else if(lv_obj_check_type(obj, &lv_menu_cont_class)) {
        lv_obj_add_style(obj, &theme->styles.menu_cont, 0);
        lv_obj_add_style(obj, &theme->styles.menu_pressed, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary_muted, LV_STATE_PRESSED | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary_muted, LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_STATE_FOCUS_KEY);
    }
    else if(lv_obj_check_type(obj, &lv_menu_sidebar_header_cont_class) ||
            lv_obj_check_type(obj, &lv_menu_main_header_cont_class)) {
        lv_obj_add_style(obj, &theme->styles.menu_header_cont, 0);
    }
    else if(lv_obj_check_type(obj, &lv_menu_page_class)) {
        lv_obj_add_style(obj, &theme->styles.menu_page, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    }
    else if(lv_obj_check_type(obj, &lv_menu_section_class)) {
        lv_obj_add_style(obj, &theme->styles.menu_section, 0);
    }
    else if(lv_obj_check_type(obj, &lv_menu_separator_class)) {
        lv_obj_add_style(obj, &theme->styles.menu_separator, 0);
    }
#endif
#if LV_USE_MSGBOX
    else if(lv_obj_check_type(obj, &lv_msgbox_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pad_zero, 0);
        lv_obj_add_style(obj, &theme->styles.clip_corner, 0);
        return;
    }
    else if(lv_obj_check_type(obj, &lv_msgbox_backdrop_class)) {
        lv_obj_add_style(obj, &theme->styles.msgbox_backdrop_bg, 0);
        return;
    }
    else if(lv_obj_check_type(obj, &lv_msgbox_header_class)) {
        lv_obj_add_style(obj, &theme->styles.pad_tiny, 0);
        lv_obj_add_style(obj, &theme->styles.bg_color_grey, 0);
        return;
    }
    else if(lv_obj_check_type(obj, &lv_msgbox_footer_class)) {
        lv_obj_add_style(obj, &theme->styles.pad_tiny, 0);
        return;
    }
    else if(lv_obj_check_type(obj, &lv_msgbox_content_class)) {
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
        lv_obj_add_style(obj, &theme->styles.pad_tiny, 0);
        return;
    }
    else if(lv_obj_check_type(obj, &lv_msgbox_header_button_class) ||
            lv_obj_check_type(obj, &lv_msgbox_footer_button_class)) {
        lv_obj_add_style(obj, &theme->styles.btn, 0);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, 0);
        lv_obj_add_style(obj, &theme->styles.transition_delayed, 0);
        lv_obj_add_style(obj, &theme->styles.pressed, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.transition_normal, LV_STATE_PRESSED);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.bg_color_secondary, LV_STATE_CHECKED);
        lv_obj_add_style(obj, &theme->styles.disabled, LV_STATE_DISABLED);
        return;
    }

#endif

#if LV_USE_SPINBOX
    else if(lv_obj_check_type(obj, &lv_spinbox_class)) {
        lv_obj_add_style(obj, &theme->styles.card, 0);
        lv_obj_add_style(obj, &theme->styles.pad_small, 0);
        lv_obj_add_style(obj, &theme->styles.outline_primary, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &theme->styles.outline_secondary, LV_STATE_EDITED);
        lv_obj_add_style(obj, &theme->styles.bg_color_primary, LV_PART_CURSOR);
    }
#endif
#if LV_USE_TILEVIEW
    else if(lv_obj_check_type(obj, &lv_tileview_class)) {
        lv_obj_add_style(obj, &theme->styles.scr, 0);
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    }
    else if(lv_obj_check_type(obj, &lv_tileview_tile_class)) {
        lv_obj_add_style(obj, &theme->styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &theme->styles.scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    }
#endif

#if LV_USE_TABVIEW
    else if(lv_obj_check_type(obj, &lv_tabview_class)) {
        lv_obj_add_style(obj, &theme->styles.scr, 0);
        lv_obj_add_style(obj, &theme->styles.pad_zero, 0);
    }
#endif

#if LV_USE_WIN
    else if(lv_obj_check_type(obj, &lv_win_class)) {
        lv_obj_add_style(obj, &theme->styles.clip_corner, 0);
    }
#endif

#if LV_USE_LED
    else if(lv_obj_check_type(obj, &lv_led_class)) {
        lv_obj_add_style(obj, &theme->styles.led, 0);
    }
#endif

#if LV_USE_SCALE
    else if(lv_obj_check_type(obj, &lv_scale_class)) {
        lv_obj_add_style(obj, &theme->styles.scale, LV_PART_MAIN);
        lv_obj_add_style(obj, &theme->styles.scale, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &theme->styles.scale, LV_PART_ITEMS);
    }
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void style_init_reset(lv_style_t * style)
{
    if(theme_def->inited) {
        lv_style_reset(style);
    }
    else {
        lv_style_init(style);
    }
}

#endif
