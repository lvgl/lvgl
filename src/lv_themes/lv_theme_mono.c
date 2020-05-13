/**
 * @file lv_theme_mono.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_MONO

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void theme_apply(lv_obj_t * obj, lv_theme_style_t name);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t theme;

static lv_style_t style_scr;
static lv_style_t style_bg;
static lv_style_t style_clip_corner;
static lv_style_t style_btn;
static lv_style_t style_round;
static lv_style_t style_no_radius;
static lv_style_t style_fg_color;
static lv_style_t style_border_none;
static lv_style_t style_big_line_space;       /*In roller or dropdownlist*/
static lv_style_t style_pad_none;
static lv_style_t style_pad_normal;
static lv_style_t style_pad_small;
static lv_style_t style_pad_inner;

#if LV_USE_ARC
    static lv_style_t style_arc_bg, style_arc_indic;
#endif

#if LV_USE_LIST
    static lv_style_t style_list_btn;
#endif

#if LV_USE_CALENDAR
    static lv_style_t style_calendar_date;
#endif

#if LV_USE_CHART
    static lv_style_t style_chart_series;
#endif

#if LV_USE_LINEMETER
    static lv_style_t style_linemeter;
#endif

#if LV_USE_GAUGE
    static lv_style_t style_gauge_needle, style_gauge_major;
#endif

#if LV_USE_PAGE
    static lv_style_t style_sb;
#endif

#if LV_USE_SPINNER
    static lv_style_t style_tick_line;
#endif

#if LV_USE_TEXTAREA
    static lv_style_t style_ta_cursor;
#endif

#if LV_USE_TABVIEW
    static lv_style_t style_tab_bg;
#endif


/**********************
 *      MACROS
 **********************/
#define COLOR_INV(c)    ((c).ch.red == 0 ? LV_COLOR_WHITE : LV_COLOR_BLACK)
#define BG_COLOR        theme.color_primary.ch.red == 0 ? LV_COLOR_WHITE : LV_COLOR_BLACK
#define FG_COLOR        COLOR_INV(BG_COLOR)
#define RADIUS          (LV_MATH_MAX(LV_DPI / 30, 2))
#define BORDER_WIDTH    (LV_MATH_MAX(LV_DPI / 60, 1))

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void basic_init(void)
{
    lv_style_init(&style_scr);
    lv_style_set_bg_opa(&style_scr, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style_scr, LV_STATE_DEFAULT, BG_COLOR);
    lv_style_set_text_color(&style_scr, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_value_color(&style_scr, LV_STATE_DEFAULT, FG_COLOR);

    lv_style_init(&style_bg);
    lv_style_set_border_post(&style_bg, LV_STATE_DEFAULT, true);
    lv_style_set_radius(&style_bg, LV_STATE_DEFAULT, RADIUS);
    lv_style_set_bg_opa(&style_bg, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style_bg, LV_STATE_DEFAULT, BG_COLOR);
    lv_style_set_border_width(&style_bg, LV_STATE_DEFAULT, BORDER_WIDTH);
    lv_style_set_border_color(&style_bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_line_width(&style_bg, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 100, 1));
    lv_style_set_scale_end_line_width(&style_bg, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 100, 1));
    lv_style_set_line_color(&style_bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_scale_grad_color(&style_bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_scale_end_color(&style_bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_text_color(&style_bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_value_color(&style_bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_pad_left(&style_bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_right(&style_bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_top(&style_bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_bottom(&style_bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_inner(&style_bg, LV_STATE_DEFAULT, LV_DPI / 10);

    lv_style_init(&style_clip_corner);
    lv_style_set_clip_corner(&style_clip_corner, LV_STATE_DEFAULT, true);


    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, LV_STATE_DEFAULT, RADIUS);
    lv_style_set_border_width(&style_btn, LV_STATE_DEFAULT, BORDER_WIDTH);
    lv_style_set_border_color(&style_btn, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_bg_color(&style_btn, LV_STATE_DEFAULT, BG_COLOR);
    lv_style_set_bg_color(&style_btn, LV_STATE_PRESSED, FG_COLOR);
    lv_style_set_bg_color(&style_btn, LV_STATE_CHECKED, FG_COLOR);
    lv_style_set_bg_color(&style_btn, LV_STATE_CHECKED | LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_text_color(&style_btn, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_text_color(&style_btn, LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_text_color(&style_btn, LV_STATE_CHECKED, BG_COLOR);
    lv_style_set_text_color(&style_btn, LV_STATE_CHECKED | LV_STATE_PRESSED, FG_COLOR);
    lv_style_set_value_color(&style_btn, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_value_color(&style_btn, LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_value_color(&style_btn, LV_STATE_CHECKED, BG_COLOR);
    lv_style_set_value_color(&style_btn, LV_STATE_CHECKED | LV_STATE_PRESSED, FG_COLOR);
    lv_style_set_image_recolor(&style_btn, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_image_recolor(&style_btn, LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_image_recolor(&style_btn, LV_STATE_CHECKED, BG_COLOR);
    lv_style_set_image_recolor(&style_btn, LV_STATE_CHECKED | LV_STATE_PRESSED, FG_COLOR);

    lv_style_init(&style_round);
    lv_style_set_radius(&style_round, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

    lv_style_init(&style_no_radius);
    lv_style_set_radius(&style_no_radius, LV_STATE_DEFAULT, 0);


    lv_style_init(&style_border_none);
    lv_style_set_border_width(&style_border_none, LV_STATE_DEFAULT, 0);


    lv_style_init(&style_fg_color);
    lv_style_set_bg_color(&style_fg_color, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_text_color(&style_fg_color, LV_STATE_DEFAULT, BG_COLOR);
    lv_style_set_image_recolor(&style_fg_color, LV_STATE_DEFAULT, BG_COLOR);
    lv_style_set_line_color(&style_fg_color, LV_STATE_DEFAULT, FG_COLOR);

    lv_style_init(&style_big_line_space);
    lv_style_set_text_line_space(&style_big_line_space, LV_STATE_DEFAULT, LV_DPI / 10);

    lv_style_init(&style_pad_none);
    lv_style_set_pad_left(&style_pad_none, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_pad_none, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_pad_none, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_pad_none, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_inner(&style_pad_none, LV_STATE_DEFAULT, 0);

    lv_style_init(&style_pad_normal);
    lv_style_set_pad_left(&style_pad_normal, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_right(&style_pad_normal, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_top(&style_pad_normal, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_bottom(&style_pad_normal, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_inner(&style_pad_normal, LV_STATE_DEFAULT, LV_DPI / 10);

    lv_style_init(&style_pad_small);
    lv_style_set_pad_left(&style_pad_small, LV_STATE_DEFAULT, LV_DPI / 20);
    lv_style_set_pad_right(&style_pad_small, LV_STATE_DEFAULT, LV_DPI / 20);
    lv_style_set_pad_top(&style_pad_small, LV_STATE_DEFAULT, LV_DPI / 20);
    lv_style_set_pad_bottom(&style_pad_small, LV_STATE_DEFAULT, LV_DPI / 20);
    lv_style_set_pad_inner(&style_pad_small, LV_STATE_DEFAULT, LV_DPI / 20);

    lv_style_init(&style_pad_inner);
    lv_style_set_pad_inner(&style_pad_inner, LV_STATE_DEFAULT, LV_DPI / 15);
}

static void arc_init(void)
{
#if LV_USE_ARC != 0
    lv_style_init(&style_arc_bg);
    lv_style_set_line_width(&style_arc_bg, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 100, 1));
    lv_style_set_line_color(&style_arc_bg, LV_STATE_DEFAULT, FG_COLOR);

    lv_style_init(&style_arc_indic);
    lv_style_set_line_width(&style_arc_indic, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 10, 3));
    lv_style_set_line_color(&style_arc_indic, LV_STATE_DEFAULT, FG_COLOR);
#endif
}

static void bar_init(void)
{
#if LV_USE_BAR

#endif
}

static void btn_init(void)
{
#if LV_USE_BTN != 0

#endif
}


static void btnmatrix_init(void)
{
#if LV_USE_BTNMATRIX

#endif
}


static void calendar_init(void)
{
#if LV_USE_CALENDAR
    lv_style_init(&style_calendar_date);
    lv_style_set_value_str(&style_calendar_date, LV_STATE_CHECKED, LV_SYMBOL_BULLET);
    lv_style_set_value_font(&style_calendar_date, LV_STATE_CHECKED, LV_THEME_DEFAULT_FONT_TITLE);
    lv_style_set_value_align(&style_calendar_date, LV_STATE_CHECKED, LV_ALIGN_IN_TOP_RIGHT);
    lv_style_set_value_color(&style_calendar_date, LV_STATE_CHECKED, FG_COLOR);
    lv_style_set_value_ofs_y(&style_calendar_date, LV_STATE_CHECKED,
                             - lv_font_get_line_height(LV_THEME_DEFAULT_FONT_TITLE) / 4);
    lv_style_set_bg_color(&style_calendar_date, LV_STATE_CHECKED, BG_COLOR);
    lv_style_set_text_color(&style_calendar_date, LV_STATE_CHECKED, FG_COLOR);
    lv_style_set_value_color(&style_calendar_date, LV_STATE_CHECKED | LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_bg_color(&style_calendar_date, LV_STATE_CHECKED | LV_STATE_PRESSED, FG_COLOR);
    lv_style_set_text_color(&style_calendar_date, LV_STATE_CHECKED | LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_border_width(&style_calendar_date, LV_STATE_FOCUSED, BORDER_WIDTH);
    lv_style_set_pad_inner(&style_calendar_date, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 100, 1));

#endif
}

static void chart_init(void)
{
#if LV_USE_CHART
    lv_style_init(&style_chart_series);
    lv_style_set_size(&style_chart_series, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_opa(&style_chart_series, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_line_width(&style_chart_series, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 50, 1));

#endif
}


static void cpicker_init(void)
{
#if LV_USE_CPICKER

#endif
}

static void checkbox_init(void)
{
#if LV_USE_CHECKBOX != 0

#endif
}


static void cont_init(void)
{
#if LV_USE_CONT != 0

#endif
}


static void gauge_init(void)
{
#if LV_USE_GAUGE != 0
    lv_style_init(&style_gauge_needle);
    lv_style_set_line_width(&style_gauge_needle, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 30, 2));
    lv_style_set_line_color(&style_gauge_needle, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_size(&style_gauge_needle, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 10, 4));
    lv_style_set_bg_opa(&style_gauge_needle, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style_gauge_needle, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_radius(&style_gauge_needle, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

    lv_style_init(&style_gauge_major);
    lv_style_set_line_width(&style_gauge_major, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 25, 2));
    lv_style_set_line_color(&style_gauge_major, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_scale_end_color(&style_gauge_major, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_scale_grad_color(&style_gauge_major, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_scale_end_line_width(&style_gauge_major, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 25, 2));

#endif
}

static void img_init(void)
{
#if LV_USE_IMG != 0

#endif
}

static void label_init(void)
{
#if LV_USE_LABEL != 0

#endif
}


static void linemeter_init(void)
{
#if LV_USE_LINEMETER != 0
    lv_style_init(&style_linemeter);
    lv_style_set_line_width(&style_linemeter, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 25, 2));
    lv_style_set_scale_end_line_width(&style_linemeter, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 70, 1));
#endif
}

static void line_init(void)
{
#if LV_USE_LINE != 0

#endif
}

static void led_init(void)
{
#if LV_USE_LED != 0

#endif
}

static void page_init(void)
{
#if LV_USE_PAGE

#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0

#endif
}

static void switch_init(void)
{
#if LV_USE_SWITCH != 0
    lv_style_init(&style_sb);
    lv_style_set_bg_opa(&style_sb, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style_sb, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_radius(&style_sb, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_pad_right(&style_sb, LV_STATE_DEFAULT, LV_DPI / 30);
    lv_style_set_pad_bottom(&style_sb, LV_STATE_DEFAULT, LV_DPI / 30);
    lv_style_set_size(&style_sb, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 25, 3));
#endif
}


static void spinbox_init(void)
{
#if LV_USE_SPINBOX

#endif
}


static void spinner_init(void)
{
#if LV_USE_SPINNER != 0

#endif
}

static void keyboard_init(void)
{
#if LV_USE_KEYBOARD

#endif
}

static void msgbox_init(void)
{
#if LV_USE_MSGBOX

#endif
}

static void textarea_init(void)
{
#if LV_USE_TEXTAREA
    lv_style_init(&style_ta_cursor);
    lv_style_set_bg_opa(&style_ta_cursor, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_width(&style_ta_cursor, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 100, 1));
    lv_style_set_border_side(&style_ta_cursor, LV_STATE_DEFAULT, LV_BORDER_SIDE_LEFT);
    lv_style_set_border_color(&style_ta_cursor, LV_STATE_DEFAULT, FG_COLOR);

#endif
}

static void list_init(void)
{
#if LV_USE_LIST != 0
    lv_style_init(&style_list_btn);
    lv_style_set_bg_opa(&style_list_btn, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&style_list_btn, LV_STATE_PRESSED, LV_OPA_COVER);
    lv_style_set_bg_opa(&style_list_btn, LV_STATE_CHECKED, LV_OPA_COVER);
    lv_style_set_radius(&style_list_btn, LV_STATE_DEFAULT, 0);
    lv_style_set_border_side(&style_list_btn, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
#endif
}

static void ddlist_init(void)
{
#if LV_USE_DROPDOWN != 0

#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0

#endif
}

static void tabview_init(void)
{
#if LV_USE_TABVIEW != 0
    lv_style_init(&style_tab_bg);
    lv_style_set_border_width(&style_tab_bg, LV_STATE_DEFAULT, BORDER_WIDTH);
    lv_style_set_border_color(&style_tab_bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_border_side(&style_tab_bg, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);

#endif
}

static void tileview_init(void)
{
#if LV_USE_TILEVIEW != 0
#endif
}

static void table_init(void)
{
#if LV_USE_TABLE != 0

#endif
}

static void win_init(void)
{
#if LV_USE_WIN != 0

#endif
}


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
lv_theme_t * lv_theme_mono_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
                                const lv_font_t * font_small, const lv_font_t * font_normal, const lv_font_t * font_subtitle,
                                const lv_font_t * font_title)
{
    theme.color_primary = color_primary;
    theme.color_secondary = color_secondary;
    theme.font_small = font_small;
    theme.font_normal = font_normal;
    theme.font_subtitle = font_subtitle;
    theme.font_title = font_title;
    theme.flags = flags;

    basic_init();
    cont_init();
    btn_init();
    label_init();
    bar_init();
    img_init();
    line_init();
    led_init();
    slider_init();
    switch_init();
    linemeter_init();
    gauge_init();
    arc_init();
    spinner_init();
    chart_init();
    calendar_init();
    cpicker_init();
    checkbox_init();
    btnmatrix_init();
    keyboard_init();
    msgbox_init();
    page_init();
    textarea_init();
    spinbox_init();
    list_init();
    ddlist_init();
    roller_init();
    tabview_init();
    tileview_init();
    table_init();
    win_init();

    theme.apply_xcb = theme_apply;

    return &theme;
}


static void theme_apply(lv_obj_t * obj, lv_theme_style_t name)
{
    lv_style_list_t * list;

    switch(name) {
        case LV_THEME_NONE:
            break;

        case LV_THEME_SCR:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
            _lv_style_list_add_style(list, &style_scr);
            break;
        case LV_THEME_OBJ:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            break;
#if LV_USE_CONT
        case LV_THEME_CONT:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_CONT_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            break;
#endif

#if LV_USE_BTN
        case LV_THEME_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
            break;
#endif

#if LV_USE_BTNMATRIX
        case LV_THEME_BTNMATRIX:
            lv_obj_clean_style_list(obj, LV_BTNMATRIX_PART_BG);
            list = lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_BTNMATRIX_PART_BTN);
            list = lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BTN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
            break;
#endif

#if LV_USE_KEYBOARD
        case LV_THEME_KEYBOARD:
            lv_obj_clean_style_list(obj, LV_KEYBOARD_PART_BG);
            list = lv_obj_get_style_list(obj, LV_KEYBOARD_PART_BG);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_pad_small);

            lv_obj_clean_style_list(obj, LV_KEYBOARD_PART_BTN);
            list = lv_obj_get_style_list(obj, LV_KEYBOARD_PART_BTN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
            break;
#endif

#if LV_USE_BAR
        case LV_THEME_BAR:
            lv_obj_clean_style_list(obj, LV_BAR_PART_BG);
            list = lv_obj_get_style_list(obj, LV_BAR_PART_BG);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_pad_none);
            _lv_style_list_add_style(list, &style_round);

            lv_obj_clean_style_list(obj, LV_BAR_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_fg_color);
            _lv_style_list_add_style(list, &style_round);
            break;
#endif

#if LV_USE_SWITCH
        case LV_THEME_SWITCH:
            lv_obj_clean_style_list(obj, LV_SWITCH_PART_BG);
            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_BG);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_pad_none);
            _lv_style_list_add_style(list, &style_round);

            lv_obj_clean_style_list(obj, LV_SWITCH_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_INDIC);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_fg_color);

            lv_obj_clean_style_list(obj, LV_SWITCH_PART_KNOB);
            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_KNOB);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_pad_none);
            _lv_style_list_add_style(list, &style_round);
            break;
#endif

#if LV_USE_CANVAS
        case LV_THEME_CANVAS:
            lv_obj_clean_style_list(obj, LV_CANVAS_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_CANVAS_PART_MAIN);
            break;
#endif

#if LV_USE_IMG
        case LV_THEME_IMAGE:
            lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
            break;
#endif

#if LV_USE_IMGBTN
        case LV_THEME_IMGBTN:
            lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
            break;
#endif

#if LV_USE_LABEL
        case LV_THEME_LABEL:
            lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
            break;
#endif

#if LV_USE_LINE
        case LV_THEME_LINE:
            lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
            break;
#endif

#if LV_USE_ARC
        case LV_THEME_ARC:
            lv_obj_clean_style_list(obj, LV_ARC_PART_BG);
            list = lv_obj_get_style_list(obj, LV_ARC_PART_BG);
            _lv_style_list_add_style(list, &style_arc_bg);

            lv_obj_clean_style_list(obj, LV_ARC_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_ARC_PART_INDIC);
            _lv_style_list_add_style(list, &style_arc_indic);
            break;
#endif

#if LV_USE_SPINNER
        case LV_THEME_SPINNER:
            lv_obj_clean_style_list(obj, LV_SPINNER_PART_BG);
            list = lv_obj_get_style_list(obj, LV_SPINNER_PART_BG);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_tick_line);

            lv_obj_clean_style_list(obj, LV_SPINNER_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_SPINNER_PART_INDIC);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_fg_color);
            _lv_style_list_add_style(list, &style_tick_line);
            break;
#endif

#if LV_USE_SLIDER
        case LV_THEME_SLIDER:
            lv_obj_clean_style_list(obj, LV_SLIDER_PART_BG);
            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_BG);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_pad_none);

            lv_obj_clean_style_list(obj, LV_SLIDER_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_INDIC);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_fg_color);

            lv_obj_clean_style_list(obj, LV_SLIDER_PART_KNOB);
            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_KNOB);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_round);
            _lv_style_list_add_style(list, &style_pad_small);
            break;
#endif

#if LV_USE_CHECKBOX
        case LV_THEME_CHECKBOX:
            lv_obj_clean_style_list(obj, LV_CHECKBOX_PART_BG);
            list = lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BG);
            _lv_style_list_add_style(list, &style_pad_small);

            lv_obj_clean_style_list(obj, LV_CHECKBOX_PART_BULLET);
            list = lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BULLET);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
            break;
#endif

#if LV_USE_MSGBOX
        case LV_THEME_MSGBOX:
            lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BG);
            list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BG);
            _lv_style_list_add_style(list, &style_bg);
            break;

        case LV_THEME_MSGBOX_BTNS:
            lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BTN_BG);
            list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BTN_BG);
            _lv_style_list_add_style(list, &style_pad_inner);

            lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BTN);
            list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BTN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
            break;

#endif
#if LV_USE_LED
        case LV_THEME_LED:
            lv_obj_clean_style_list(obj, LV_LED_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_LED_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_round);
            break;
#endif
#if LV_USE_PAGE
        case LV_THEME_PAGE:
            lv_obj_clean_style_list(obj, LV_PAGE_PART_BG);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            _lv_style_list_add_style(list, &style_pad_inner);

            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_sb);
            break;
#endif
#if LV_USE_TABVIEW
        case LV_THEME_TABVIEW:
            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_BG);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_BG);
            _lv_style_list_add_style(list, &style_scr);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_BG_SCRLLABLE);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_TAB_BG);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB_BG);
            _lv_style_list_add_style(list, &style_tab_bg);
            _lv_style_list_add_style(list, &style_pad_small);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_INDIC);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_TAB_BTN);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB_BTN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
            break;

        case LV_THEME_TABVIEW_PAGE:
            lv_obj_clean_style_list(obj, LV_PAGE_PART_BG);

            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            _lv_style_list_add_style(list, &style_pad_normal);

            break;
#endif

#if LV_USE_TILEVIEW
        case LV_THEME_TILEVIEW:
            lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_BG);
            list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_sb);

            lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_EDGE_FLASH);
            list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_EDGE_FLASH);
            _lv_style_list_add_style(list, &style_bg);
            break;
#endif


#if LV_USE_ROLLER
        case LV_THEME_ROLLER:
            lv_obj_clean_style_list(obj, LV_ROLLER_PART_BG);
            list = lv_obj_get_style_list(obj, LV_ROLLER_PART_BG);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_big_line_space);

            lv_obj_clean_style_list(obj, LV_ROLLER_PART_SELECTED);
            list = lv_obj_get_style_list(obj, LV_ROLLER_PART_SELECTED);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_fg_color);
            _lv_style_list_add_style(list, &style_no_radius);
            break;
#endif


#if LV_USE_OBJMASK
        case LV_THEME_OBJMASK:
            lv_obj_clean_style_list(obj, LV_OBJMASK_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_OBJMASK_PART_MAIN);
            break;
#endif

#if LV_USE_LIST
        case LV_THEME_LIST:
            lv_obj_clean_style_list(obj, LV_LIST_PART_BG);
            list = lv_obj_get_style_list(obj, LV_LIST_PART_BG);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_pad_none);
            _lv_style_list_add_style(list, &style_clip_corner);

            lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLABLE);

            lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_LIST_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_sb);
            break;

        case LV_THEME_LIST_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
            _lv_style_list_add_style(list, &style_list_btn);

            break;
#endif

#if LV_USE_DROPDOWN
        case LV_THEME_DROPDOWN:
            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);

            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_LIST);
            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_LIST);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_big_line_space);

            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_sb);

            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SELECTED);
            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SELECTED);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_fg_color);
            _lv_style_list_add_style(list, &style_no_radius);
            break;
#endif

#if LV_USE_CHART
        case LV_THEME_CHART:
            lv_obj_clean_style_list(obj, LV_CHART_PART_BG);
            list = lv_obj_get_style_list(obj, LV_CHART_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_CHART_PART_SERIES_BG);
            list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES_BG);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_border_none);

            lv_obj_clean_style_list(obj, LV_CHART_PART_SERIES);
            list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES);
            _lv_style_list_add_style(list, &style_chart_series);
            break;
#endif
#if LV_USE_TABLE
        case LV_THEME_TABLE:
            lv_obj_clean_style_list(obj, LV_TABLE_PART_BG);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL1);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL1);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_no_radius);

            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL2);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL2);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_no_radius);

            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL3);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL3);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_no_radius);

            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL4);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL4);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_no_radius);
            break;
#endif

#if LV_USE_WIN
        case LV_THEME_WIN:
            lv_obj_clean_style_list(obj, LV_WIN_PART_BG);
            list = lv_obj_get_style_list(obj, LV_WIN_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_WIN_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_WIN_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_sb);

            lv_obj_clean_style_list(obj, LV_WIN_PART_CONTENT_SCROLLABLE);
            list = lv_obj_get_style_list(obj, LV_WIN_PART_CONTENT_SCROLLABLE);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_WIN_PART_HEADER);
            list = lv_obj_get_style_list(obj, LV_WIN_PART_HEADER);
            _lv_style_list_add_style(list, &style_bg);
            break;

        case LV_THEME_WIN_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
            break;
#endif

#if LV_USE_TEXTAREA
        case LV_THEME_TEXTAREA:
            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_BG);
            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);

            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_CURSOR);
            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_CURSOR);
            _lv_style_list_add_style(list, &style_ta_cursor);

            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_sb);
            break;
#endif


#if LV_USE_SPINBOX
        case LV_THEME_SPINBOX:
            lv_obj_clean_style_list(obj, LV_SPINBOX_PART_BG);
            list = lv_obj_get_style_list(obj, LV_SPINBOX_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_SPINBOX_PART_CURSOR);
            list = lv_obj_get_style_list(obj, LV_SPINBOX_PART_CURSOR);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_fg_color);
            _lv_style_list_add_style(list, &style_pad_none);
            _lv_style_list_add_style(list, &style_no_radius);
            break;

        case LV_THEME_SPINBOX_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
            break;
#endif

#if LV_USE_CALENDAR
        case LV_THEME_CALENDAR:
            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_BG);
            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_DATE);
            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_DATE);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
            _lv_style_list_add_style(list, &style_pad_small);
            _lv_style_list_add_style(list, &style_border_none);
            _lv_style_list_add_style(list, &style_calendar_date);

            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_HEADER);
            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_HEADER);
            _lv_style_list_add_style(list, &style_pad_normal);
            _lv_style_list_add_style(list, &style_border_none);

            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_DAY_NAMES);
            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_DAY_NAMES);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_pad_small);
            break;
#endif
#if LV_USE_CPICKER
        case LV_THEME_CPICKER:
            lv_obj_clean_style_list(obj, LV_CPICKER_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_CPICKER_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_CPICKER_PART_KNOB);
            list = lv_obj_get_style_list(obj, LV_CPICKER_PART_KNOB);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_round);
            break;
#endif

#if LV_USE_LINEMETER
        case LV_THEME_LINEMETER:
            lv_obj_clean_style_list(obj, LV_LINEMETER_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_LINEMETER_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_round);
            _lv_style_list_add_style(list, &style_linemeter);
            break;
#endif
#if LV_USE_GAUGE
        case LV_THEME_GAUGE:
            lv_obj_clean_style_list(obj, LV_GAUGE_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_GAUGE_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_round);

            lv_obj_clean_style_list(obj, LV_GAUGE_PART_MAJOR);
            list = lv_obj_get_style_list(obj, LV_GAUGE_PART_MAJOR);
            _lv_style_list_add_style(list, &style_gauge_major);

            lv_obj_clean_style_list(obj, LV_GAUGE_PART_NEEDLE);
            list = lv_obj_get_style_list(obj, LV_GAUGE_PART_NEEDLE);
            _lv_style_list_add_style(list, &style_gauge_needle);
            break;
#endif
        default:
            break;
    }


    lv_obj_refresh_style(obj, LV_STYLE_PROP_ALL);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif
