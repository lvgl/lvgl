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
static void apply_theme(lv_theme_t * th, lv_obj_t * obj);

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
void lv_theme_apply(lv_obj_t * obj)
{
    lv_obj_enable_style_refresh(false);

    lv_obj_remove_style(obj, LV_PART_ANY, LV_STATE_ANY, NULL);

    if(act_theme == NULL) return;
    apply_theme(act_theme, obj);    /*Apply the theme including the base theme(s)*/

    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ALL);
}

/**
 * Set a base theme for a theme.
 * The styles from the base them will be added before the styles of the current theme.
 * Arbitrary long chain of themes can be created by setting base themes.
 * @param new_theme pointer to theme which base should be set
 * @param base pointer to the base theme
 */
void lv_theme_set_parent(lv_theme_t * new_theme, lv_theme_t * base)
{
    new_theme->parent = base;
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

const lv_font_t * lv_theme_get_font_small(void)
{
    return act_theme->font_small;
}

const lv_font_t * lv_theme_get_font_normal(void)
{
    return act_theme->font_normal;
}

const lv_font_t * lv_theme_get_font_large(void)
{
    return act_theme->font_large;
}

const lv_font_t * lv_theme_get_font_extra_large(void)
{
    return act_theme->font_extra_large;
}

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

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void apply_theme(lv_theme_t * th, lv_obj_t * obj)
{
    if(th->parent) apply_theme(th->parent, obj);
    if(th->apply_cb) th->apply_cb(act_theme, obj);
}
