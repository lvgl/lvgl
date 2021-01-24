/**
 * @file lv_theme_material.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_DEFAULT

#include "../lv_misc/lv_gc.h"

#if defined(LV_GC_INCLUDE)
#include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/

#define RADIUS_DEFAULT LV_DPX(8)

/*SCREEN*/
#define COLOR_SCR        (IS_LIGHT ? lv_color_hex(0xeaeff3) : lv_color_hex(0x444b5a))
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
#define COLOR_GRAY              (IS_LIGHT ? lv_color_hex(0xd4d7d9) : lv_color_hex(0x45494d))
#define COLOR_BG_SEC_BORDER     (IS_LIGHT ? lv_color_hex(0xdfe7ed) : lv_color_hex(0x404040))
#define COLOR_BG_SEC_TEXT       (IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xa5a8ad))
#define COLOR_BG_SEC_TEXT_DIS   (IS_LIGHT ? lv_color_hex(0xaaaaaa) : lv_color_hex(0xa5a8ad))

#define TRANSITION_TIME         ((theme.flags & LV_THEME_MATERIAL_FLAG_NO_TRANSITION) ? 0 : 85)
#define BORDER_WIDTH            LV_DPX(2)
#define OUTLINE_WIDTH           ((theme.flags & LV_THEME_MATERIAL_FLAG_NO_FOCUS) ? 0 : LV_DPX(2))
#define IS_LIGHT (theme.flags & LV_THEME_MATERIAL_FLAG_LIGHT)

#define PAD_DEF (lv_disp_get_size_category(NULL) <= LV_DISP_SIZE_MEDIUM ? LV_DPX(15) : (LV_DPX(30)))

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
    lv_style_t bg_color_white;
    lv_style_t pressed;
    lv_style_t disabled;
    lv_style_t pad_zero;
    lv_style_t pad_small;
    lv_style_t pad_gap;
    lv_style_t pad_small_negative;
    lv_style_t focus_border;
    lv_style_t focus_outline;
    lv_style_t edit_outline;
    lv_style_t edit_border;
    lv_style_t circle;
    lv_style_t no_radius;
    lv_style_t clip_corner;
    lv_style_t grow;
    lv_style_t transition_delayed;
    lv_style_t transition_normal;
    lv_style_t anim;
    lv_style_t line_dashed;

    /*Parts*/
    lv_style_t knob;
    lv_style_t indic;

#if LV_USE_ARC
    lv_style_t arc_indic;
    lv_style_t arc_indic_primary;
#endif

#if LV_USE_CHART
    lv_style_t chart_series, chart_ticks;
#endif

#if LV_USE_CHECKBOX
    lv_style_t cb_marker, cb_marker_checked;
#endif

#if LV_USE_DROPDOWN
    lv_style_t ddlist_flip;
#endif

//#if LV_USE_ROLLER
//    lv_style_t roller_bg, roller_sel;
//#endif


#if LV_USE_TABLE
    lv_style_t table_cell;
#endif

#if LV_USE_METER
    lv_style_t meter_marker, meter_indic;
#endif

#if LV_USE_TEXTAREA
    lv_style_t ta_cursor, ta_placeholder;
#endif

} theme_styles_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void theme_apply(lv_theme_t * th, lv_obj_t * obj);
static void style_init_reset(lv_style_t * style);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t theme;
static theme_styles_t * styles;

static bool inited;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void basic_init(void)
{
    const static lv_style_prop_t trans_props[] = {
            LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR,
            LV_STYLE_TRANSFORM_WIDTH,
            LV_STYLE_TRANSFORM_HEIGHT,
            LV_STYLE_TRANSFORM_ZOOM,
            LV_STYLE_TRANSFORM_ANGLE,
            LV_STYLE_COLOR_FILTER_OPA, LV_STYLE_COLOR_FILTER_CB,
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
    lv_style_set_pad_left(&styles->scrollbar, LV_DPX(7));
    lv_style_set_pad_right(&styles->scrollbar,  LV_DPX(7));
    lv_style_set_pad_top(&styles->scrollbar,  LV_DPX(7));
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

    style_init_reset(&styles->focus_border);
    lv_style_set_border_color(&styles->focus_border, theme.color_primary);

    style_init_reset(&styles->edit_border);
    lv_style_set_border_color(&styles->edit_border, theme.color_secondary);

    style_init_reset(&styles->focus_outline);
    lv_style_set_outline_color(&styles->focus_outline, theme.color_primary);
    lv_style_set_outline_width(&styles->focus_outline, OUTLINE_WIDTH);
    lv_style_set_outline_opa(&styles->focus_outline, LV_OPA_50);

    style_init_reset(&styles->edit_outline);
    lv_style_set_outline_color(&styles->edit_outline, theme.color_secondary);
    lv_style_set_outline_opa(&styles->edit_outline, LV_OPA_50);

    style_init_reset(&styles->btn);
    lv_style_set_radius(&styles->btn, RADIUS_DEFAULT);
    lv_style_set_bg_opa(&styles->btn, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->btn, COLOR_GRAY);
    lv_style_set_text_color(&styles->btn, CARD_TEXT_COLOR);
    lv_style_set_pad_hor(&styles->btn, LV_DPX(40));
    lv_style_set_pad_ver(&styles->btn, LV_DPX(15));


    style_init_reset(&styles->pressed);
    lv_style_set_color_filter_cb(&styles->pressed, lv_color_darken);
    lv_style_set_color_filter_opa(&styles->pressed, LV_OPA_20);

    style_init_reset(&styles->disabled);
    lv_style_set_color_filter_cb(&styles->disabled, lv_color_lighten);
    lv_style_set_color_filter_opa(&styles->disabled, LV_OPA_40);

    style_init_reset(&styles->clip_corner);
    lv_style_set_clip_corner(&styles->clip_corner, true);

    style_init_reset(&styles->pad_small);
    lv_style_set_pad_all(&styles->pad_small, LV_DPX(10));

    style_init_reset(&styles->pad_gap);
    lv_style_set_pad_row(&styles->pad_gap, LV_DPX(10));
    lv_style_set_pad_column(&styles->pad_gap, LV_DPX(10));

    style_init_reset(&styles->pad_small_negative);
    lv_style_set_pad_all(&styles->pad_small_negative, - LV_DPX(4));

    style_init_reset(&styles->pad_zero);
    lv_style_set_pad_all(&styles->pad_zero, 0);

    style_init_reset(&styles->bg_color_primary);
    lv_style_set_bg_color(&styles->bg_color_primary, theme.color_primary);
    lv_style_set_text_color(&styles->bg_color_primary, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&styles->bg_color_primary, LV_OPA_COVER);

    style_init_reset(&styles->bg_color_secondary);
    lv_style_set_bg_color(&styles->bg_color_secondary, theme.color_secondary);
    lv_style_set_text_color(&styles->bg_color_secondary, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&styles->bg_color_secondary, LV_OPA_COVER);

    style_init_reset(&styles->bg_color_gray);
    lv_style_set_bg_color(&styles->bg_color_gray, COLOR_GRAY);
    lv_style_set_bg_opa(&styles->bg_color_gray, LV_OPA_COVER);
    lv_style_set_text_color(&styles->bg_color_gray, CARD_TEXT_COLOR);

    style_init_reset(&styles->bg_color_white);
    lv_style_set_bg_color(&styles->bg_color_white, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&styles->bg_color_white, LV_OPA_COVER);

    style_init_reset(&styles->circle);
    lv_style_set_radius(&styles->circle, LV_RADIUS_CIRCLE);

    style_init_reset(&styles->grow);
    lv_style_set_transform_width(&styles->grow, LV_DPX(3));
    lv_style_set_transform_angle(&styles->grow, LV_DPX(3));

    style_init_reset(&styles->knob);
    lv_style_set_bg_color(&styles->knob, IS_LIGHT ? theme.color_primary : LV_COLOR_WHITE);
    lv_style_set_bg_opa(&styles->knob, LV_OPA_COVER);
    lv_style_set_pad_all(&styles->knob, LV_DPX(5));
    lv_style_set_radius(&styles->knob, LV_RADIUS_CIRCLE);

    style_init_reset(&styles->anim);
    lv_style_set_anim_time(&styles->anim, 200);

    style_init_reset(&styles->line_dashed);
    lv_style_set_line_dash_width(&styles->line_dashed, LV_DPX(10));
    lv_style_set_line_dash_gap(&styles->line_dashed, LV_DPX(10));

#if LV_USE_ARC
    style_init_reset(&styles->arc_indic);
    lv_style_set_line_color(&styles->arc_indic, COLOR_GRAY);
    lv_style_set_line_width(&styles->arc_indic, LV_DPX(15));
    lv_style_set_line_rounded(&styles->arc_indic, true);

    style_init_reset(&styles->arc_indic_primary);
    lv_style_set_line_color(&styles->arc_indic_primary, theme.color_primary);
    LV_IMG_DECLARE(asd);
    lv_style_set_bg_img_src(&styles->arc_indic_primary, &asd);
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
    lv_style_set_bg_color(&styles->cb_marker, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&styles->cb_marker, LV_OPA_COVER);
    lv_style_set_radius(&styles->cb_marker, RADIUS_DEFAULT / 2);

    style_init_reset(&styles->cb_marker_checked);
    lv_style_set_content_text(&styles->cb_marker_checked, LV_SYMBOL_OK);
    lv_style_set_text_color(&styles->cb_marker_checked, LV_COLOR_WHITE);
    lv_style_set_text_font(&styles->cb_marker_checked, theme.font_small);
#endif

#if LV_USE_CHART
    style_init_reset(&styles->chart_series);
    lv_style_set_line_width(&styles->chart_series, LV_DPX(3));
    lv_style_set_radius(&styles->chart_series, LV_DPX(1));
    lv_style_set_size(&styles->chart_series, LV_DPX(5));

    style_init_reset(&styles->chart_ticks);
    lv_style_set_line_width(&styles->chart_ticks, LV_DPX(1));
    lv_style_set_line_color(&styles->chart_ticks, COLOR_GRAY);
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
}

//
//static void ddlist_init(void)
//{
//#if LV_USE_DROPDOWN != 0
//
//    style_init_reset(&styles->ddlist_page);
//    lv_style_set_text_line_space(&styles->ddlist_page, LV_STATE_DEFAULT, LV_DPX(20));
//    lv_style_set_clip_corner(&styles->ddlist_page, LV_STATE_DEFAULT, true);
//
//    style_init_reset(&styles->ddlist_sel);
//    lv_style_set_bg_opa(&styles->ddlist_sel, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&styles->ddlist_sel, LV_STATE_DEFAULT, theme.color_primary);
//    lv_style_set_text_color(&styles->ddlist_sel, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex3(0xfff) : lv_color_hex3(0xfff));
//    lv_style_set_bg_color(&styles->ddlist_sel, LV_STATE_PRESSED, CARD_PR_COLOR);
//    lv_style_set_text_color(&styles->ddlist_sel, LV_STATE_PRESSED, CARD_TEXT_PR_COLOR);
//#endif
//}

//static void roller_init(void)
//{
//#if LV_USE_ROLLER != 0
//    style_init_reset(&styles->roller_bg);
//    lv_style_set_text_line_space(&styles->roller_bg, LV_STATE_DEFAULT, LV_DPX(25));
//
//    style_init_reset(&styles->roller_sel);
//    lv_style_set_bg_opa(&styles->roller_sel, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&styles->roller_sel, LV_STATE_DEFAULT, theme.color_primary);
//    lv_style_set_text_color(&styles->roller_sel, LV_STATE_DEFAULT, LV_COLOR_WHITE);
//#endif
//}



/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the default
 * @param color_primary the primary color of the theme
 * @param color_secondary the secondary color for the theme
 * @param flags ORed flags starting with `LV_THEME_DEF_FLAG_...`
 * @param font_small pointer to a small font
 * @param font_normal pointer to a normal font
 * @param font_subtitle pointer to a large font
 * @param font_title pointer to a extra large font
 * @return a pointer to reference this theme later
 */
lv_theme_t * lv_theme_default_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
        const lv_font_t * font_small, const lv_font_t * font_normal, const lv_font_t * font_subtitle,
        const lv_font_t * font_title)
{

    /* This trick is required only to avoid the garbage collection of
     * styles' data if LVGL is used in a binding (e.g. Micropython)
     * In a general case styles could be simple `static lv_style_t my style` variables*/
    if(!inited) {
        LV_GC_ROOT(_lv_theme_material_styles) = lv_mem_alloc(sizeof(theme_styles_t));
        styles = (theme_styles_t *)LV_GC_ROOT(_lv_theme_material_styles);
    }

    theme.color_primary = color_primary;
    theme.color_secondary = color_secondary;
    theme.font_small = font_small;
    theme.font_normal = font_normal;
    theme.font_subtitle = font_subtitle;
    theme.font_title = font_title;
    theme.flags = flags;

    basic_init();

    theme.apply_cb = theme_apply;

    inited = true;

    lv_obj_report_style_change(NULL);

    return &theme;
}


static void theme_apply(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);

    if(lv_obj_get_parent(obj) == NULL) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->scr);
        lv_obj_add_style_no_refresh(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style_no_refresh(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        return;
    }

    if(lv_obj_check_type(obj, &lv_obj)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style_no_refresh(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style_no_refresh(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
    }
#if LV_USE_BTN
    else if(lv_obj_check_type(obj, &lv_btn)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->btn);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->transition_delayed);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->pressed);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->transition_normal);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->grow);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_CHECKED, &styles->bg_color_secondary);
    }
#endif

#if LV_USE_BTNMATRIX
    else if(lv_obj_check_type(obj, &lv_btnmatrix)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_gap);
        lv_obj_add_style_no_refresh(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->btn);
        lv_obj_add_style_no_refresh(obj, LV_PART_ITEMS, LV_STATE_PRESSED, &styles->pressed);
        lv_obj_add_style_no_refresh(obj, LV_PART_ITEMS, LV_STATE_PRESSED, &styles->grow);
        lv_obj_add_style_no_refresh(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->transition_delayed);
        lv_obj_add_style_no_refresh(obj, LV_PART_ITEMS, LV_STATE_PRESSED, &styles->transition_normal);
    }
#endif
#if LV_USE_BAR
    else if(lv_obj_check_type(obj, &lv_bar)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style_no_refresh(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style_no_refresh(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->circle);
    }
#endif

#if LV_USE_SLIDER
    else if(lv_obj_check_type(obj, &lv_slider)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->transition_delayed);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->transition_normal);
        lv_obj_add_style_no_refresh(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style_no_refresh(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style_no_refresh(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->knob);
        lv_obj_add_style_no_refresh(obj, LV_PART_KNOB, LV_STATE_PRESSED, &styles->grow);
        lv_obj_add_style_no_refresh(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->transition_delayed);
        lv_obj_add_style_no_refresh(obj, LV_PART_KNOB, LV_STATE_PRESSED, &styles->transition_normal);
    }
#endif

#if LV_USE_TABLE
    else if(lv_obj_check_type(obj, &lv_table)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style_no_refresh(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style_no_refresh(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_SCROLLED, &styles->transition_normal);
        lv_obj_add_style_no_refresh(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->bg_color_white);
        lv_obj_add_style_no_refresh(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->table_cell);
        lv_obj_add_style_no_refresh(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->pad_small);
    }
#endif

#if LV_USE_CHECKBOX
    else if(lv_obj_check_type(obj, &lv_checkbox)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_gap);
        lv_obj_add_style_no_refresh(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->cb_marker);
        lv_obj_add_style_no_refresh(obj, LV_PART_MARKER, LV_STATE_CHECKED, &styles->bg_color_primary);
        lv_obj_add_style_no_refresh(obj, LV_PART_MARKER, LV_STATE_CHECKED, &styles->cb_marker_checked);
        lv_obj_add_style_no_refresh(obj, LV_PART_MARKER, LV_STATE_PRESSED, &styles->pressed);
        lv_obj_add_style_no_refresh(obj, LV_PART_MARKER, LV_STATE_PRESSED, &styles->grow);
        lv_obj_add_style_no_refresh(obj, LV_PART_MARKER, LV_STATE_PRESSED, &styles->transition_normal);
        lv_obj_add_style_no_refresh(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->transition_delayed);
    }
#endif

#if LV_USE_SWITCH
    else if(lv_obj_check_type(obj, &lv_switch)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style_no_refresh(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style_no_refresh(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style_no_refresh(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->knob);
        lv_obj_add_style_no_refresh(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->bg_color_white);
        lv_obj_add_style_no_refresh(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->pad_small_negative);
    }
#endif

#if LV_USE_CHART
    else if(lv_obj_check_type(obj, &lv_chart)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->line_dashed);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_zero);
        lv_obj_add_style_no_refresh(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style_no_refresh(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        lv_obj_add_style_no_refresh(obj, LV_PART_ITEMS, LV_STATE_DEFAULT, &styles->chart_series);
        lv_obj_add_style_no_refresh(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->pad_small);
        lv_obj_add_style_no_refresh(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->chart_ticks);
    }
#endif

#if LV_USE_ROLLER
    else if(lv_obj_check_type(obj, &lv_roller)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->anim);
        lv_obj_add_style_no_refresh(obj, LV_PART_HIGHLIGHT, LV_STATE_DEFAULT, &styles->bg_color_primary);
    }
#endif

#if LV_USE_DROPDOWN
    else if(lv_obj_check_type(obj, &lv_dropdown)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_CHECKED, &styles->ddlist_flip);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->transition_normal);
    }
    else if(lv_obj_check_type(obj, &lv_dropdown_list)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style_no_refresh(obj, LV_PART_HIGHLIGHT, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style_no_refresh(obj, LV_PART_HIGHLIGHT, LV_STATE_PRESSED, &styles->bg_color_gray);
    }
#endif

#if LV_USE_ARC
    else if(lv_obj_check_type(obj, &lv_arc)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->arc_indic);
        lv_obj_add_style_no_refresh(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->arc_indic);
        lv_obj_add_style_no_refresh(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->arc_indic_primary);
        lv_obj_add_style_no_refresh(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->knob);
    }
#endif

#if LV_USE_METER
    else if(lv_obj_check_type(obj, &lv_meter)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style_no_refresh(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->meter_marker);
    }
#endif

#if LV_USE_TEXTAREA
    else if(lv_obj_check_type(obj, &lv_textarea)) {
        lv_obj_add_style_no_refresh(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style_no_refresh(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style_no_refresh(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        lv_obj_add_style_no_refresh(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->ta_cursor);
    }
#endif

//#if LV_USE_LINEMETER
//case LV_THEME_LINEMETER:
//    list = _lv_obj_get_style_list(obj, LV_LINEMETER_PART_MAIN);
//    _lv_style_list_add_style(list, &styles->card);
//    _lv_style_list_add_style(list, &styles->lmeter);
//    break;
//#endif
//
//#if LV_USE_GAUGE
//case LV_THEME_GAUGE:
//    list = _lv_obj_get_style_list(obj, LV_GAUGE_PART_MAIN);
//    _lv_style_list_add_style(list, &styles->card);
//    _lv_style_list_add_style(list, &styles->gauge_main);
//
//    list = _lv_obj_get_style_list(obj, LV_GAUGE_PART_MAJOR);
//    _lv_style_list_add_style(list, &styles->gauge_strong);
//
//    list = _lv_obj_get_style_list(obj, LV_GAUGE_PART_NEEDLE);
//    _lv_style_list_add_style(list, &styles->gauge_needle);
//    break;
//#endif
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
