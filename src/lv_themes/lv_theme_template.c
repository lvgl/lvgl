/**
 * @file lv_theme_template.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_TEMPLATE

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

static lv_style_t style_bg;
static lv_style_t style_btn;
static lv_style_t style_round;
static lv_style_t style_color;
static lv_style_t style_gray;
static lv_style_t style_tick_line;
static lv_style_t style_tight;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void basic_init(void)
{
    lv_style_init(&style_bg);
    lv_style_set_bg_opa(&style_bg, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_border_width(&style_bg, LV_STATE_DEFAULT, 1);
    lv_style_set_border_width(&style_bg, LV_STATE_FOCUSED, 2);
    lv_style_set_border_color(&style_bg, LV_STATE_FOCUSED, theme.color_secondary);
    lv_style_set_border_color(&style_bg, LV_STATE_EDITED, lv_color_darken(theme.color_secondary, LV_OPA_30));
    lv_style_set_line_width(&style_bg, LV_STATE_DEFAULT, 1);
    lv_style_set_scale_end_line_width(&style_bg, LV_STATE_DEFAULT, 1);
    lv_style_set_scale_end_color(&style_bg, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_text_color(&style_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_pad_left(&style_bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_right(&style_bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_top(&style_bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_bottom(&style_bg, LV_STATE_DEFAULT, LV_DPI / 10);
    lv_style_set_pad_inner(&style_bg, LV_STATE_DEFAULT, LV_DPI / 10);

    lv_style_init(&style_btn);
    lv_style_set_bg_color(&style_btn, LV_STATE_PRESSED, lv_color_hex3(0xccc));
    lv_style_set_bg_color(&style_btn, LV_STATE_CHECKED, theme.color_primary);
    lv_style_set_bg_color(&style_btn, LV_STATE_CHECKED | LV_STATE_PRESSED, lv_color_darken(theme.color_primary, LV_OPA_30));
    lv_style_set_bg_color(&style_btn, LV_STATE_DISABLED, LV_COLOR_SILVER);
    lv_style_set_text_color(&style_btn, LV_STATE_DISABLED, LV_COLOR_GRAY);
    lv_style_set_image_recolor(&style_btn, LV_STATE_DISABLED, LV_COLOR_GRAY);

    lv_style_init(&style_round);
    lv_style_set_radius(&style_round, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

    lv_style_init(&style_color);
    lv_style_set_bg_color(&style_color, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_line_color(&style_color, LV_STATE_DEFAULT, theme.color_primary);

    lv_style_init(&style_gray);
    lv_style_set_bg_color(&style_gray, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_style_set_line_color(&style_gray, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_style_set_text_color(&style_gray, LV_STATE_DEFAULT, LV_COLOR_GRAY);

    lv_style_init(&style_tick_line);
    lv_style_set_line_width(&style_tick_line, LV_STATE_DEFAULT, 5);
    lv_style_set_scale_end_line_width(&style_tick_line, LV_STATE_DEFAULT, 5);
    lv_style_set_scale_end_color(&style_tick_line, LV_STATE_DEFAULT, theme.color_primary);

    lv_style_init(&style_tight);
    lv_style_set_pad_left(&style_tight, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_tight, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_tight, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_tight, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_inner(&style_tight, LV_STATE_DEFAULT, 0);
}

static void arc_init(void)
{
#if LV_USE_ARC != 0

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

#endif
}

static void chart_init(void)
{
#if LV_USE_CHART

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

#endif
}

static void list_init(void)
{
#if LV_USE_LIST != 0

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
lv_theme_t * lv_theme_template_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
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


void theme_apply(lv_obj_t * obj, lv_theme_style_t name)
{
    lv_style_list_t * list;

    switch(name) {
        case LV_THEME_NONE:
            break;

        case LV_THEME_SCR:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_tight);
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
            _lv_style_list_add_style(list, &style_tight);

            lv_obj_clean_style_list(obj, LV_BAR_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_color);
            break;
#endif

#if LV_USE_SWITCH
        case LV_THEME_SWITCH:
            lv_obj_clean_style_list(obj, LV_SWITCH_PART_BG);
            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_BG);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_tight);
            _lv_style_list_add_style(list, &style_round);

            lv_obj_clean_style_list(obj, LV_SWITCH_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_INDIC);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_color);

            lv_obj_clean_style_list(obj, LV_SWITCH_PART_KNOB);
            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_KNOB);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_tight);
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
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_tick_line);
            _lv_style_list_add_style(list, &style_round);

            lv_obj_clean_style_list(obj, LV_ARC_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_ARC_PART_INDIC);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_color);
            _lv_style_list_add_style(list, &style_tick_line);
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
            _lv_style_list_add_style(list, &style_color);
            _lv_style_list_add_style(list, &style_tick_line);
            break;
#endif

#if LV_USE_SLIDER
        case LV_THEME_SLIDER:
            lv_obj_clean_style_list(obj, LV_SLIDER_PART_BG);
            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_SLIDER_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_INDIC);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_color);

            lv_obj_clean_style_list(obj, LV_SLIDER_PART_KNOB);
            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_KNOB);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_round);
            break;
#endif

#if LV_USE_CHECKBOX
        case LV_THEME_CHECKBOX:
            lv_obj_clean_style_list(obj, LV_CHECKBOX_PART_BG);
            list = lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BG);

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
            _lv_style_list_add_style(list, &style_bg);

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
            _lv_style_list_add_style(list, &style_color);
            _lv_style_list_add_style(list, &style_round);
            break;
#endif
#if LV_USE_PAGE
        case LV_THEME_PAGE:
            lv_obj_clean_style_list(obj, LV_PAGE_PART_BG);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_BG);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_gray);

            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_bg);
            break;
#endif
#if LV_USE_TABVIEW
        case LV_THEME_TABVIEW:
            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_BG);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_BG_SCRLLABLE);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_BG_SCRLLABLE);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_color);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_TAB_BG);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_INDIC);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_color);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_TAB_BTN);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB_BTN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
            break;

        case LV_THEME_TABVIEW_PAGE:
            lv_obj_clean_style_list(obj, LV_PAGE_PART_BG);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_BG);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_gray);

            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            _lv_style_list_add_style(list, &style_bg);

            break;
#endif

#if LV_USE_TILEVIEW
        case LV_THEME_TILEVIEW:
            lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_BG);
            list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_bg);

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

            lv_obj_clean_style_list(obj, LV_ROLLER_PART_SELECTED);
            list = lv_obj_get_style_list(obj, LV_ROLLER_PART_SELECTED);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_color);
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

            lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLABLE);
            list = lv_obj_get_style_list(obj, LV_LIST_PART_SCROLLABLE);

            lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_LIST_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_bg);
            break;

        case LV_THEME_LIST_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_btn);
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

            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SELECTED);
            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SELECTED);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_color);
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

            lv_obj_clean_style_list(obj, LV_CHART_PART_SERIES);
            list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_tight);
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

            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL2);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL2);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL3);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL3);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL4);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL4);
            _lv_style_list_add_style(list, &style_bg);
            break;
#endif

#if LV_USE_WIN
        case LV_THEME_WIN:
            lv_obj_clean_style_list(obj, LV_WIN_PART_BG);
            list = lv_obj_get_style_list(obj, LV_WIN_PART_BG);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_WIN_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_WIN_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_bg);

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
            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);
            _lv_style_list_add_style(list, &style_gray);

            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_CURSOR);
            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_CURSOR);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_tight);

            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &style_bg);
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
            _lv_style_list_add_style(list, &style_tight);

            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_HEADER);
            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_HEADER);
            _lv_style_list_add_style(list, &style_bg);

            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_DAY_NAMES);
            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_DAY_NAMES);
            _lv_style_list_add_style(list, &style_bg);
            _lv_style_list_add_style(list, &style_tight);
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
            _lv_style_list_add_style(list, &style_tick_line);

            lv_obj_clean_style_list(obj, LV_GAUGE_PART_NEEDLE);
            list = lv_obj_get_style_list(obj, LV_GAUGE_PART_NEEDLE);
            _lv_style_list_add_style(list, &style_bg);
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
