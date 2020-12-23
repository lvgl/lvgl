/**
 * @file lv_theme_mono.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_MONO

#include "../lv_misc/lv_gc.h"

#if defined(LV_GC_INCLUDE)
    #include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_style_t scr;
    lv_style_t bg;
    lv_style_t clip_corner;
    lv_style_t btn;
    lv_style_t round;
    lv_style_t no_radius;
    lv_style_t fg_color;
    lv_style_t border_none;
    lv_style_t big_line_space;       /*In roller or dropdownlist*/
    lv_style_t pad_none;
    lv_style_t pad_normal;
    lv_style_t pad_small;
    lv_style_t pad_inner;
    lv_style_t txt_underline;

#if LV_USE_ARC
    lv_style_t arc_bg, arc_indic;
#endif

#if LV_USE_LIST
    lv_style_t list_btn;
#endif

#if LV_USE_CALENDAR
    lv_style_t calendar_date;
#endif

#if LV_USE_CHART
    lv_style_t chart_series;
#endif

#if LV_USE_LINEMETER
    lv_style_t linemeter;
#endif

#if LV_USE_GAUGE
    lv_style_t gauge_needle, gauge_major;
#endif

#if LV_USE_PAGE
    lv_style_t sb;
#endif

#if LV_USE_SPINNER
    lv_style_t tick_line;
#endif

#if LV_USE_TEXTAREA
    lv_style_t ta_cursor;
#endif

#if LV_USE_TABVIEW
    lv_style_t tab_bg;
#endif
} theme_styles_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void theme_apply(lv_theme_t * th, lv_obj_t * obj, lv_theme_style_t name);
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
    style_init_reset(&styles->scr);
    lv_style_set_bg_opa(&styles->scr, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->scr, LV_STATE_DEFAULT, BG_COLOR);
    lv_style_set_text_color(&styles->scr, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_text_sel_color(&styles->scr, LV_STATE_DEFAULT, BG_COLOR);
    lv_style_set_text_sel_bg_color(&styles->scr, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_value_color(&styles->scr, LV_STATE_DEFAULT, FG_COLOR);

    style_init_reset(&styles->bg);
    lv_style_set_border_post(&styles->bg, LV_STATE_DEFAULT, true);
    lv_style_set_radius(&styles->bg, LV_STATE_DEFAULT, RADIUS);
    lv_style_set_bg_opa(&styles->bg, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->bg, LV_STATE_DEFAULT, BG_COLOR);
    lv_style_set_border_width(&styles->bg, LV_STATE_DEFAULT, BORDER_WIDTH);
    lv_style_set_border_width(&styles->bg, LV_STATE_FOCUSED, BORDER_WIDTH * 2);
    lv_style_set_border_width(&styles->bg, LV_STATE_FOCUSED | LV_STATE_EDITED, BORDER_WIDTH * 3);
    lv_style_set_border_color(&styles->bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_line_width(&styles->bg, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 100, 1));
    lv_style_set_scale_end_line_width(&styles->bg, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 100, 1));
    lv_style_set_line_color(&styles->bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_scale_grad_color(&styles->bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_scale_end_color(&styles->bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_text_color(&styles->bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_value_color(&styles->bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_pad_left(&styles->bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_right(&styles->bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_top(&styles->bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_bottom(&styles->bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_inner(&styles->bg, LV_STATE_DEFAULT, LV_DPI / 10);

    style_init_reset(&styles->clip_corner);
    lv_style_set_clip_corner(&styles->clip_corner, LV_STATE_DEFAULT, true);

    style_init_reset(&styles->btn);
    lv_style_set_radius(&styles->btn, LV_STATE_DEFAULT, RADIUS);
    lv_style_set_border_width(&styles->btn, LV_STATE_DEFAULT, BORDER_WIDTH);
    lv_style_set_border_width(&styles->btn, LV_STATE_FOCUSED, BORDER_WIDTH + 1);
    lv_style_set_border_width(&styles->btn, LV_STATE_FOCUSED | LV_STATE_EDITED, BORDER_WIDTH + 2);
    lv_style_set_border_color(&styles->btn, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_bg_color(&styles->btn, LV_STATE_DEFAULT, BG_COLOR);
    lv_style_set_bg_color(&styles->btn, LV_STATE_PRESSED, FG_COLOR);
    lv_style_set_bg_color(&styles->btn, LV_STATE_CHECKED, FG_COLOR);
    lv_style_set_bg_color(&styles->btn, LV_STATE_CHECKED | LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_text_color(&styles->btn, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_text_color(&styles->btn, LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_text_color(&styles->btn, LV_STATE_CHECKED, BG_COLOR);
    lv_style_set_text_color(&styles->btn, LV_STATE_CHECKED | LV_STATE_PRESSED, FG_COLOR);
    lv_style_set_value_color(&styles->btn, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_value_color(&styles->btn, LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_value_color(&styles->btn, LV_STATE_CHECKED, BG_COLOR);
    lv_style_set_value_color(&styles->btn, LV_STATE_CHECKED | LV_STATE_PRESSED, FG_COLOR);
    lv_style_set_image_recolor(&styles->btn, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_image_recolor(&styles->btn, LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_image_recolor(&styles->btn, LV_STATE_CHECKED, BG_COLOR);
    lv_style_set_image_recolor(&styles->btn, LV_STATE_CHECKED | LV_STATE_PRESSED, FG_COLOR);

    style_init_reset(&styles->round);
    lv_style_set_radius(&styles->round, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

    style_init_reset(&styles->no_radius);
    lv_style_set_radius(&styles->no_radius, LV_STATE_DEFAULT, 0);


    style_init_reset(&styles->border_none);
    lv_style_set_border_width(&styles->border_none, LV_STATE_DEFAULT, 0);


    style_init_reset(&styles->fg_color);
    lv_style_set_bg_color(&styles->fg_color, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_text_color(&styles->fg_color, LV_STATE_DEFAULT, BG_COLOR);
    lv_style_set_image_recolor(&styles->fg_color, LV_STATE_DEFAULT, BG_COLOR);
    lv_style_set_line_color(&styles->fg_color, LV_STATE_DEFAULT, FG_COLOR);

    style_init_reset(&styles->big_line_space);
    lv_style_set_text_line_space(&styles->big_line_space, LV_STATE_DEFAULT, LV_DPI / 10);

    style_init_reset(&styles->pad_none);
    lv_style_set_pad_left(&styles->pad_none, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&styles->pad_none, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&styles->pad_none, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&styles->pad_none, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_inner(&styles->pad_none, LV_STATE_DEFAULT, 0);

    style_init_reset(&styles->pad_normal);
    lv_style_set_pad_left(&styles->pad_normal, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_right(&styles->pad_normal, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_top(&styles->pad_normal, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_bottom(&styles->pad_normal, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_inner(&styles->pad_normal, LV_STATE_DEFAULT, LV_DPI / 10);

    style_init_reset(&styles->pad_small);
    lv_style_set_pad_left(&styles->pad_small, LV_STATE_DEFAULT, LV_DPI / 20);
    lv_style_set_pad_right(&styles->pad_small, LV_STATE_DEFAULT, LV_DPI / 20);
    lv_style_set_pad_top(&styles->pad_small, LV_STATE_DEFAULT, LV_DPI / 20);
    lv_style_set_pad_bottom(&styles->pad_small, LV_STATE_DEFAULT, LV_DPI / 20);
    lv_style_set_pad_inner(&styles->pad_small, LV_STATE_DEFAULT, LV_DPI / 20);

    style_init_reset(&styles->pad_inner);
    lv_style_set_pad_inner(&styles->pad_inner, LV_STATE_DEFAULT, LV_DPI / 15);

    style_init_reset(&styles->txt_underline);
    lv_style_set_text_decor(&styles->txt_underline, LV_STATE_FOCUSED, LV_TEXT_DECOR_UNDERLINE);
}

static void arc_init(void)
{
#if LV_USE_ARC != 0
    style_init_reset(&styles->arc_bg);
    lv_style_set_line_width(&styles->arc_bg, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 100, 1));
    lv_style_set_line_color(&styles->arc_bg, LV_STATE_DEFAULT, FG_COLOR);

    style_init_reset(&styles->arc_indic);
    lv_style_set_line_width(&styles->arc_indic, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 10, 3));
    lv_style_set_line_color(&styles->arc_indic, LV_STATE_DEFAULT, FG_COLOR);
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
    style_init_reset(&styles->calendar_date);
    lv_style_set_value_str(&styles->calendar_date, LV_STATE_CHECKED, LV_SYMBOL_BULLET);
    lv_style_set_value_font(&styles->calendar_date, LV_STATE_CHECKED, LV_THEME_DEFAULT_FONT_TITLE);
    lv_style_set_value_align(&styles->calendar_date, LV_STATE_CHECKED, LV_ALIGN_IN_TOP_RIGHT);
    lv_style_set_value_color(&styles->calendar_date, LV_STATE_CHECKED, FG_COLOR);
    lv_style_set_value_ofs_y(&styles->calendar_date, LV_STATE_CHECKED,
                             - lv_font_get_line_height(LV_THEME_DEFAULT_FONT_TITLE) / 4);
    lv_style_set_bg_color(&styles->calendar_date, LV_STATE_CHECKED, BG_COLOR);
    lv_style_set_text_color(&styles->calendar_date, LV_STATE_CHECKED, FG_COLOR);
    lv_style_set_value_color(&styles->calendar_date, LV_STATE_CHECKED | LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_bg_color(&styles->calendar_date, LV_STATE_CHECKED | LV_STATE_PRESSED, FG_COLOR);
    lv_style_set_text_color(&styles->calendar_date, LV_STATE_CHECKED | LV_STATE_PRESSED, BG_COLOR);
    lv_style_set_border_width(&styles->calendar_date, LV_STATE_FOCUSED, BORDER_WIDTH);
    lv_style_set_pad_inner(&styles->calendar_date, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 100, 1));

#endif
}

static void chart_init(void)
{
#if LV_USE_CHART
    style_init_reset(&styles->chart_series);
    lv_style_set_size(&styles->chart_series, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_opa(&styles->chart_series, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_line_width(&styles->chart_series, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 50, 1));

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
    style_init_reset(&styles->gauge_needle);
    lv_style_set_line_width(&styles->gauge_needle, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 30, 2));
    lv_style_set_line_color(&styles->gauge_needle, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_size(&styles->gauge_needle, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 10, 4));
    lv_style_set_bg_opa(&styles->gauge_needle, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->gauge_needle, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_radius(&styles->gauge_needle, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

    style_init_reset(&styles->gauge_major);
    lv_style_set_line_width(&styles->gauge_major, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 25, 2));
    lv_style_set_line_color(&styles->gauge_major, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_scale_end_color(&styles->gauge_major, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_scale_grad_color(&styles->gauge_major, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_scale_end_line_width(&styles->gauge_major, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 25, 2));

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
    style_init_reset(&styles->linemeter);
    lv_style_set_line_width(&styles->linemeter, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 25, 2));
    lv_style_set_scale_end_line_width(&styles->linemeter, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 70, 1));
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
    style_init_reset(&styles->sb);
    lv_style_set_bg_opa(&styles->sb, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->sb, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_radius(&styles->sb, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_pad_right(&styles->sb, LV_STATE_DEFAULT, LV_DPI / 30);
    lv_style_set_pad_bottom(&styles->sb, LV_STATE_DEFAULT, LV_DPI / 30);
    lv_style_set_size(&styles->sb, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 25, 3));
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
    style_init_reset(&styles->ta_cursor);
    lv_style_set_bg_opa(&styles->ta_cursor, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_width(&styles->ta_cursor, LV_STATE_DEFAULT, LV_MATH_MAX(LV_DPI / 100, 1));
    lv_style_set_border_side(&styles->ta_cursor, LV_STATE_DEFAULT, LV_BORDER_SIDE_LEFT);
    lv_style_set_border_color(&styles->ta_cursor, LV_STATE_DEFAULT, FG_COLOR);

#endif
}

static void list_init(void)
{
#if LV_USE_LIST != 0
    style_init_reset(&styles->list_btn);
    lv_style_set_bg_opa(&styles->list_btn, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&styles->list_btn, LV_STATE_PRESSED, LV_OPA_COVER);
    lv_style_set_bg_opa(&styles->list_btn, LV_STATE_CHECKED, LV_OPA_COVER);
    lv_style_set_radius(&styles->list_btn, LV_STATE_DEFAULT, 0);
    lv_style_set_border_side(&styles->list_btn, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
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
    style_init_reset(&styles->tab_bg);
    lv_style_set_border_width(&styles->tab_bg, LV_STATE_DEFAULT, BORDER_WIDTH);
    lv_style_set_border_color(&styles->tab_bg, LV_STATE_DEFAULT, FG_COLOR);
    lv_style_set_border_side(&styles->tab_bg, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);

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

    /* This trick is required only to avoid the garbage collection of
     * styles' data if LVGL is used in a binding (e.g. Micropython)
     * In a general case styles could be simple `static lv_style_t my style` variables*/
    if(!inited) {
        LV_GC_ROOT(_lv_theme_mono_styles) = lv_mem_alloc(sizeof(theme_styles_t));
        styles = (theme_styles_t *)LV_GC_ROOT(_lv_theme_mono_styles);
    }

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

    theme.apply_xcb = NULL;
    theme.apply_cb = theme_apply;

    return &theme;
}


static void theme_apply(lv_theme_t * th, lv_obj_t * obj, lv_theme_style_t name)
{
    LV_UNUSED(th);

    lv_style_list_t * list;

    switch(name) {
        case LV_THEME_NONE:
            break;

        case LV_THEME_SCR:
            list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
            _lv_style_list_add_style(list, &styles->scr);
            break;
        case LV_THEME_OBJ:
            list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            break;
#if LV_USE_CONT
        case LV_THEME_CONT:
            list = lv_obj_get_style_list(obj, LV_CONT_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            break;
#endif

#if LV_USE_BTN
        case LV_THEME_BTN:
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->btn);
            _lv_style_list_add_style(list, &styles->txt_underline);
            break;
#endif

#if LV_USE_BTNMATRIX
        case LV_THEME_BTNMATRIX:
            list = lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);

            list = lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BTN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->btn);
            _lv_style_list_add_style(list, &styles->txt_underline);
            break;
#endif

#if LV_USE_KEYBOARD
        case LV_THEME_KEYBOARD:
            list = lv_obj_get_style_list(obj, LV_KEYBOARD_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->pad_small);

            list = lv_obj_get_style_list(obj, LV_KEYBOARD_PART_BTN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->btn);
            break;
#endif

#if LV_USE_BAR
        case LV_THEME_BAR:
            list = lv_obj_get_style_list(obj, LV_BAR_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->pad_none);
            _lv_style_list_add_style(list, &styles->round);

            list = lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->fg_color);
            _lv_style_list_add_style(list, &styles->round);
            break;
#endif

#if LV_USE_SWITCH
        case LV_THEME_SWITCH:
            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->pad_none);
            _lv_style_list_add_style(list, &styles->round);

            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_INDIC);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->fg_color);

            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_KNOB);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->pad_none);
            _lv_style_list_add_style(list, &styles->round);
            break;
#endif

#if LV_USE_CANVAS
        case LV_THEME_CANVAS:
            list = lv_obj_get_style_list(obj, LV_CANVAS_PART_MAIN);
            break;
#endif

#if LV_USE_IMG
        case LV_THEME_IMAGE:
            list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
            break;
#endif

#if LV_USE_IMGBTN
        case LV_THEME_IMGBTN:
            list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
            break;
#endif

#if LV_USE_LABEL
        case LV_THEME_LABEL:
            list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
            break;
#endif

#if LV_USE_LINE
        case LV_THEME_LINE:
            list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
            break;
#endif

#if LV_USE_ARC
        case LV_THEME_ARC:
            list = lv_obj_get_style_list(obj, LV_ARC_PART_BG);
            _lv_style_list_add_style(list, &styles->arc_bg);

            list = lv_obj_get_style_list(obj, LV_ARC_PART_INDIC);
            _lv_style_list_add_style(list, &styles->arc_indic);
            break;
#endif

#if LV_USE_SPINNER
        case LV_THEME_SPINNER:
            list = lv_obj_get_style_list(obj, LV_SPINNER_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->tick_line);

            list = lv_obj_get_style_list(obj, LV_SPINNER_PART_INDIC);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->fg_color);
            _lv_style_list_add_style(list, &styles->tick_line);
            break;
#endif

#if LV_USE_SLIDER
        case LV_THEME_SLIDER:
            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->pad_none);

            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_INDIC);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->fg_color);

            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_KNOB);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->round);
            _lv_style_list_add_style(list, &styles->pad_small);
            _lv_style_list_add_style(list, &styles->fg_color);
            break;
#endif

#if LV_USE_CHECKBOX
        case LV_THEME_CHECKBOX:
            list = lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BG);
            _lv_style_list_add_style(list, &styles->pad_small);

            list = lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BULLET);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->btn);
            _lv_style_list_add_style(list, &styles->pad_small);
            break;
#endif

#if LV_USE_MSGBOX
        case LV_THEME_MSGBOX:
            list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);
            break;

        case LV_THEME_MSGBOX_BTNS:
            list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BTN_BG);
            _lv_style_list_add_style(list, &styles->pad_inner);

            list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BTN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->btn);
            _lv_style_list_add_style(list, &styles->txt_underline);
            break;

#endif
#if LV_USE_LED
        case LV_THEME_LED:
            list = lv_obj_get_style_list(obj, LV_LED_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->round);
            break;
#endif
#if LV_USE_PAGE
        case LV_THEME_PAGE:
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);

            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            _lv_style_list_add_style(list, &styles->pad_inner);

            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &styles->sb);
            break;
#endif
#if LV_USE_TABVIEW
        case LV_THEME_TABVIEW:
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_BG);
            _lv_style_list_add_style(list, &styles->scr);

            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB_BG);
            _lv_style_list_add_style(list, &styles->tab_bg);
            _lv_style_list_add_style(list, &styles->pad_small);

            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB_BTN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->btn);
            _lv_style_list_add_style(list, &styles->txt_underline);
            break;

        case LV_THEME_TABVIEW_PAGE:
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            _lv_style_list_add_style(list, &styles->pad_normal);

            break;
#endif

#if LV_USE_TILEVIEW
        case LV_THEME_TILEVIEW:
            list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);

            list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &styles->sb);

            list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_EDGE_FLASH);
            _lv_style_list_add_style(list, &styles->bg);
            break;
#endif


#if LV_USE_ROLLER
        case LV_THEME_ROLLER:
            list = lv_obj_get_style_list(obj, LV_ROLLER_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->big_line_space);

            list = lv_obj_get_style_list(obj, LV_ROLLER_PART_SELECTED);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->fg_color);
            _lv_style_list_add_style(list, &styles->no_radius);
            break;
#endif


#if LV_USE_OBJMASK
        case LV_THEME_OBJMASK:
            list = lv_obj_get_style_list(obj, LV_OBJMASK_PART_MAIN);
            break;
#endif

#if LV_USE_LIST
        case LV_THEME_LIST:
            list = lv_obj_get_style_list(obj, LV_LIST_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->pad_none);
            _lv_style_list_add_style(list, &styles->clip_corner);

            list = lv_obj_get_style_list(obj, LV_LIST_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &styles->sb);
            break;

        case LV_THEME_LIST_BTN:
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->btn);
            _lv_style_list_add_style(list, &styles->list_btn);
            _lv_style_list_add_style(list, &styles->txt_underline);

            break;
#endif

#if LV_USE_DROPDOWN
        case LV_THEME_DROPDOWN:
            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->btn);

            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_LIST);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->big_line_space);

            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &styles->sb);

            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SELECTED);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->fg_color);
            _lv_style_list_add_style(list, &styles->no_radius);
            break;
#endif

#if LV_USE_CHART
        case LV_THEME_CHART:
            list = lv_obj_get_style_list(obj, LV_CHART_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);

            list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES_BG);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->border_none);

            list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES);
            _lv_style_list_add_style(list, &styles->chart_series);
            break;
#endif
#if LV_USE_TABLE
        case LV_THEME_TABLE: {
                list = lv_obj_get_style_list(obj, LV_TABLE_PART_BG);
                _lv_style_list_add_style(list, &styles->bg);

                int idx = 1; /* start value should be 1, not zero, since cell styles
                            start at 1 due to presence of LV_TABLE_PART_BG=0
                            in the enum (lv_table.h) */
                /* declaring idx outside loop to work with older compilers */
                for(; idx <= LV_TABLE_CELL_STYLE_CNT; idx ++) {
                    list = lv_obj_get_style_list(obj, idx);
                    _lv_style_list_add_style(list, &styles->bg);
                    _lv_style_list_add_style(list, &styles->no_radius);
                }
                break;
            }
#endif

#if LV_USE_WIN
        case LV_THEME_WIN:
            list = lv_obj_get_style_list(obj, LV_WIN_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);

            list = lv_obj_get_style_list(obj, LV_WIN_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &styles->sb);

            list = lv_obj_get_style_list(obj, LV_WIN_PART_CONTENT_SCROLLABLE);
            _lv_style_list_add_style(list, &styles->bg);

            list = lv_obj_get_style_list(obj, LV_WIN_PART_HEADER);
            _lv_style_list_add_style(list, &styles->bg);
            break;

        case LV_THEME_WIN_BTN:
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->btn);
            break;
#endif

#if LV_USE_TEXTAREA
        case LV_THEME_TEXTAREA:
            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);

            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_CURSOR);
            _lv_style_list_add_style(list, &styles->ta_cursor);

            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &styles->sb);
            break;
#endif


#if LV_USE_SPINBOX
        case LV_THEME_SPINBOX:
            list = lv_obj_get_style_list(obj, LV_SPINBOX_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);

            list = lv_obj_get_style_list(obj, LV_SPINBOX_PART_CURSOR);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->fg_color);
            _lv_style_list_add_style(list, &styles->pad_none);
            _lv_style_list_add_style(list, &styles->no_radius);
            _lv_style_list_add_style(list, &styles->txt_underline);
            break;

        case LV_THEME_SPINBOX_BTN:
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->btn);
            _lv_style_list_add_style(list, &styles->txt_underline);
            break;
#endif

#if LV_USE_CALENDAR
        case LV_THEME_CALENDAR:
            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);

            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_DATE);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->btn);
            _lv_style_list_add_style(list, &styles->pad_small);
            _lv_style_list_add_style(list, &styles->border_none);
            _lv_style_list_add_style(list, &styles->calendar_date);

            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_HEADER);
            _lv_style_list_add_style(list, &styles->pad_normal);
            _lv_style_list_add_style(list, &styles->border_none);

            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_DAY_NAMES);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->pad_small);
            break;
#endif
#if LV_USE_CPICKER
        case LV_THEME_CPICKER:
            list = lv_obj_get_style_list(obj, LV_CPICKER_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);

            list = lv_obj_get_style_list(obj, LV_CPICKER_PART_KNOB);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->round);
            break;
#endif

#if LV_USE_LINEMETER
        case LV_THEME_LINEMETER:
            list = lv_obj_get_style_list(obj, LV_LINEMETER_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->round);
            _lv_style_list_add_style(list, &styles->linemeter);
            break;
#endif
#if LV_USE_GAUGE
        case LV_THEME_GAUGE:
            list = lv_obj_get_style_list(obj, LV_GAUGE_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->round);

            list = lv_obj_get_style_list(obj, LV_GAUGE_PART_MAJOR);
            _lv_style_list_add_style(list, &styles->gauge_major);

            list = lv_obj_get_style_list(obj, LV_GAUGE_PART_NEEDLE);
            _lv_style_list_add_style(list, &styles->gauge_needle);
            break;
#endif
        default:
            break;
    }


    lv_obj_refresh_style(obj, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);


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
