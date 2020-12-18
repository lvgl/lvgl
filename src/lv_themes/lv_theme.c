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
    lv_obj_remove_all_styles(obj);

    /*Apply the theme including the base theme(s)*/
    apply_theme(act_theme, obj);
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

static void apply_theme(lv_theme_t * th, lv_obj_t * obj)
{
    if(th->base) apply_theme(th->base, obj);

    if(th->apply_cb) th->apply_cb(act_theme, obj);
}
