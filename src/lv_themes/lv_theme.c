/**
 * @file lv_theme.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void apply_theme(lv_theme_t * th, lv_obj_t * obj, lv_theme_style_t name);
static void clear_styles(lv_obj_t * obj, lv_theme_style_t name);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t * act_theme;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Set a theme for the system.
 * From now, all the created objects will use styles from this theme by default
 * @param th pointer to theme (return value of: 'lv_theme_init_xxx()')
 */
void lv_theme_set_act(lv_theme_t * th)
{
    act_theme = th;
}

/**
 * Get the current system theme.
 * @return pointer to the current system theme. NULL if not set.
 */
lv_theme_t * lv_theme_get_act(void)
{
    return act_theme;
}

/**
 * Apply the active theme on an object
 * @param obj pointer to an object
 * @param name the name of the theme element to apply. E.g. `LV_THEME_BTN`
 */
void lv_theme_apply(lv_obj_t * obj, lv_theme_style_t name)
{
    /* Remove the existing styles from all part of the object. */
    clear_styles(obj, name);

    /*Apply the theme including the base theme(s)*/

    apply_theme(act_theme, obj, name);
}

/**
 * Copy a theme to an other or initialize a theme
 * @param theme pointer to a theme to initialize
 * @param copy pointer to a theme to copy
 *             or `NULL` to initialize `theme` to empty
 */
void lv_theme_copy(lv_theme_t * theme, const lv_theme_t * copy)
{
    _lv_memset_00(theme, sizeof(lv_theme_t));

    if(copy) {
        theme->font_small = copy->font_small;
        theme->font_normal = copy->font_normal;
        theme->font_subtitle = copy->font_subtitle;
        theme->font_title = copy->font_title;
        theme->color_primary = copy->color_primary;
        theme->color_secondary = copy->color_secondary;
        theme->flags = copy->flags;
        theme->base = copy->base;
        theme->apply_cb = copy->apply_cb;
        theme->apply_xcb = copy->apply_xcb;
    }

}

/**
 * Set a base theme for a theme.
 * The styles from the base them will be added before the styles of the current theme.
 * Arbitrary long chain of themes can be created by setting base themes.
 * @param new_theme pointer to theme which base should be set
 * @param base pointer to the base theme
 */
void lv_theme_set_base(lv_theme_t * new_theme, lv_theme_t * base)
{
    new_theme->base = base;
}

/**
 * Set a callback for a theme.
 * The callback is used to add styles to different objects
 * @param theme pointer to theme which callback should be set
 * @param cb pointer to the callback
 */
void lv_theme_set_apply_cb(lv_theme_t * theme, lv_theme_apply_cb_t apply_cb)
{
    theme->apply_cb = apply_cb;
}

/**
 * Get the small font of the theme
 * @return pointer to the font
 */
const lv_font_t * lv_theme_get_font_small(void)
{
    return act_theme->font_small;
}

/**
 * Get the normal font of the theme
 * @return pointer to the font
 */
const lv_font_t * lv_theme_get_font_normal(void)
{
    return act_theme->font_normal;
}

/**
 * Get the subtitle font of the theme
 * @return pointer to the font
 */
const lv_font_t * lv_theme_get_font_subtitle(void)
{
    return act_theme->font_subtitle;
}

/**
 * Get the title font of the theme
 * @return pointer to the font
 */
const lv_font_t * lv_theme_get_font_title(void)
{
    return act_theme->font_title;
}

/**
 * Get the primary color of the theme
 * @return the color
 */
lv_color_t lv_theme_get_color_primary(void)
{
    return act_theme->color_primary;
}

/**
 * Get the secondary color of the theme
 * @return the color
 */
lv_color_t lv_theme_get_color_secondary(void)
{
    return act_theme->color_secondary;
}

/**
 * Get the flags of the theme
 * @return the flags
 */
uint32_t lv_theme_get_flags(void)
{
    return act_theme->flags;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void apply_theme(lv_theme_t * th, lv_obj_t * obj, lv_theme_style_t name)
{
    if(th->base) {
        apply_theme(th->base, obj, name);
    }

    /*apply_xcb is deprecated, use apply_cb instead*/
    if(th->apply_xcb) {
        th->apply_xcb(obj, name);
    }
    else if(th->apply_cb) {
        th->apply_cb(act_theme, obj, name);
    }
}

static void clear_styles(lv_obj_t * obj, lv_theme_style_t name)
{
    switch(name) {
        case LV_THEME_NONE:
            break;

        case LV_THEME_SCR:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            break;
        case LV_THEME_OBJ:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            break;
#if LV_USE_CONT
        case LV_THEME_CONT:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            break;
#endif

#if LV_USE_BTN
        case LV_THEME_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            break;
#endif

#if LV_USE_BTNMATRIX
        case LV_THEME_BTNMATRIX:
            lv_obj_clean_style_list(obj, LV_BTNMATRIX_PART_BG);
            lv_obj_clean_style_list(obj, LV_BTNMATRIX_PART_BTN);
            break;
#endif

#if LV_USE_KEYBOARD
        case LV_THEME_KEYBOARD:
            lv_obj_clean_style_list(obj, LV_KEYBOARD_PART_BG);
            lv_obj_clean_style_list(obj, LV_KEYBOARD_PART_BTN);
            break;
#endif

#if LV_USE_BAR
        case LV_THEME_BAR:
            lv_obj_clean_style_list(obj, LV_BAR_PART_BG);
            lv_obj_clean_style_list(obj, LV_BAR_PART_INDIC);
            break;
#endif

#if LV_USE_SWITCH
        case LV_THEME_SWITCH:
            lv_obj_clean_style_list(obj, LV_SWITCH_PART_BG);
            lv_obj_clean_style_list(obj, LV_SWITCH_PART_INDIC);
            lv_obj_clean_style_list(obj, LV_SWITCH_PART_KNOB);
            break;
#endif

#if LV_USE_CANVAS
        case LV_THEME_CANVAS:
            lv_obj_clean_style_list(obj, LV_CANVAS_PART_MAIN);
            break;
#endif

#if LV_USE_IMG
        case LV_THEME_IMAGE:
            lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
            break;
#endif

#if LV_USE_IMGBTN
        case LV_THEME_IMGBTN:
            lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
            break;
#endif

#if LV_USE_LABEL
        case LV_THEME_LABEL:
            lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
            break;
#endif

#if LV_USE_LINE
        case LV_THEME_LINE:
            lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
            break;
#endif

#if LV_USE_ARC
        case LV_THEME_ARC:
            lv_obj_clean_style_list(obj, LV_ARC_PART_BG);
            lv_obj_clean_style_list(obj, LV_ARC_PART_INDIC);
            break;
#endif

#if LV_USE_SPINNER
        case LV_THEME_SPINNER:
            lv_obj_clean_style_list(obj, LV_SPINNER_PART_BG);
            lv_obj_clean_style_list(obj, LV_SPINNER_PART_INDIC);
            break;
#endif

#if LV_USE_SLIDER
        case LV_THEME_SLIDER:
            lv_obj_clean_style_list(obj, LV_SLIDER_PART_BG);
            lv_obj_clean_style_list(obj, LV_SLIDER_PART_INDIC);
            lv_obj_clean_style_list(obj, LV_SLIDER_PART_KNOB);
            break;
#endif

#if LV_USE_CHECKBOX
        case LV_THEME_CHECKBOX:
            lv_obj_clean_style_list(obj, LV_CHECKBOX_PART_BG);
            lv_obj_clean_style_list(obj, LV_CHECKBOX_PART_BULLET);
            break;
#endif

#if LV_USE_MSGBOX
        case LV_THEME_MSGBOX:
            lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BG);
            break;

        case LV_THEME_MSGBOX_BTNS:
            lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BTN_BG);
            lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BTN);
            break;

#endif
#if LV_USE_LED
        case LV_THEME_LED:
            lv_obj_clean_style_list(obj, LV_LED_PART_MAIN);
            break;
#endif
#if LV_USE_PAGE
        case LV_THEME_PAGE:
            lv_obj_clean_style_list(obj, LV_PAGE_PART_BG);
            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLBAR);
            break;
#endif
#if LV_USE_TABVIEW
        case LV_THEME_TABVIEW:
            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_BG);
            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_BG_SCROLLABLE);
            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_TAB_BG);
            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_INDIC);
            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_TAB_BTN);
            break;

        case LV_THEME_TABVIEW_PAGE:
            lv_obj_clean_style_list(obj, LV_PAGE_PART_BG);
            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            break;
#endif

#if LV_USE_TILEVIEW
        case LV_THEME_TILEVIEW:
            lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_BG);
            lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_SCROLLBAR);
            lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_EDGE_FLASH);
            break;
#endif


#if LV_USE_ROLLER
        case LV_THEME_ROLLER:
            lv_obj_clean_style_list(obj, LV_ROLLER_PART_BG);
            lv_obj_clean_style_list(obj, LV_ROLLER_PART_SELECTED);
            break;
#endif


#if LV_USE_OBJMASK
        case LV_THEME_OBJMASK:
            lv_obj_clean_style_list(obj, LV_OBJMASK_PART_MAIN);
            break;
#endif

#if LV_USE_LIST
        case LV_THEME_LIST:
            lv_obj_clean_style_list(obj, LV_LIST_PART_BG);
            lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLABLE);
            lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLBAR);
            break;

        case LV_THEME_LIST_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            break;
#endif

#if LV_USE_DROPDOWN
        case LV_THEME_DROPDOWN:
            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_MAIN);
            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_LIST);
            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SCROLLBAR);
            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SELECTED);
            break;
#endif

#if LV_USE_CHART
        case LV_THEME_CHART:
            lv_obj_clean_style_list(obj, LV_CHART_PART_BG);
            lv_obj_clean_style_list(obj, LV_CHART_PART_SERIES_BG);
            lv_obj_clean_style_list(obj, LV_CHART_PART_SERIES);
            break;
#endif
#if LV_USE_TABLE
        case LV_THEME_TABLE:
            lv_obj_clean_style_list(obj, LV_TABLE_PART_BG);
            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL1);
            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL2);
            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL3);
            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL4);
            break;
#endif

#if LV_USE_WIN
        case LV_THEME_WIN:
            lv_obj_clean_style_list(obj, LV_WIN_PART_BG);
            lv_obj_clean_style_list(obj, LV_WIN_PART_SCROLLBAR);
            lv_obj_clean_style_list(obj, LV_WIN_PART_CONTENT_SCROLLABLE);
            lv_obj_clean_style_list(obj, LV_WIN_PART_HEADER);
            break;

        case LV_THEME_WIN_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            break;
#endif

#if LV_USE_TEXTAREA
        case LV_THEME_TEXTAREA:
            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_BG);
            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);
            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_CURSOR);
            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_SCROLLBAR);
            break;
#endif


#if LV_USE_SPINBOX
        case LV_THEME_SPINBOX:
            lv_obj_clean_style_list(obj, LV_SPINBOX_PART_BG);
            lv_obj_clean_style_list(obj, LV_SPINBOX_PART_CURSOR);
            break;

        case LV_THEME_SPINBOX_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            break;
#endif

#if LV_USE_CALENDAR
        case LV_THEME_CALENDAR:
            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_BG);
            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_DATE);
            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_HEADER);
            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_DAY_NAMES);
            break;
#endif
#if LV_USE_CPICKER
        case LV_THEME_CPICKER:
            lv_obj_clean_style_list(obj, LV_CPICKER_PART_MAIN);
            lv_obj_clean_style_list(obj, LV_CPICKER_PART_KNOB);
            break;
#endif

#if LV_USE_LINEMETER
        case LV_THEME_LINEMETER:
            lv_obj_clean_style_list(obj, LV_LINEMETER_PART_MAIN);
            break;
#endif
#if LV_USE_GAUGE
        case LV_THEME_GAUGE:
            lv_obj_clean_style_list(obj, LV_GAUGE_PART_MAIN);
            lv_obj_clean_style_list(obj, LV_GAUGE_PART_MAJOR);
            lv_obj_clean_style_list(obj, LV_GAUGE_PART_NEEDLE);
            break;
#endif
        default:
            break;
    }

}
