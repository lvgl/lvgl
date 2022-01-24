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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_theme_t  * lv_theme_get_from_obj(lv_obj_t * obj)
{
    lv_disp_t * disp = obj ? lv_obj_get_disp(obj) : lv_disp_get_default();
    return lv_disp_get_theme(disp);
}

/**
 * Apply the active theme on an object
 * @param obj pointer to an object
 * @param name the name of the theme element to apply. E.g. `LV_THEME_BTN`
 */
void lv_theme_apply(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    if(th == NULL) return;

    lv_obj_remove_style_all(obj);

    apply_theme(th, obj);    /*Apply the theme including the base theme(s)*/
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

const lv_font_t * lv_theme_get_font_small(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    return th ? th->font_small : LV_FONT_DEFAULT;
}

const lv_font_t * lv_theme_get_font_normal(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    return th ? th->font_normal : LV_FONT_DEFAULT;
}

const lv_font_t * lv_theme_get_font_large(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    return th ? th->font_large : LV_FONT_DEFAULT;
}

lv_color_t lv_theme_get_color_primary(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    return th ? th->color_primary : lv_palette_main(LV_PALETTE_BLUE_GREY);
}

lv_color_t lv_theme_get_color_secondary(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    return th ? th->color_secondary : lv_palette_main(LV_PALETTE_BLUE);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void apply_theme(lv_theme_t * th, lv_obj_t * obj)
{
    if(th->parent) apply_theme(th->parent, obj);
    if(th->apply_cb) th->apply_cb(th, obj);
}
