/**
 * @file lv_theme_empty.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include "../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_EMPTY

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
static lv_style_t opa_cover;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

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
lv_theme_t * lv_theme_empty_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
                                 const lv_font_t * font_small, const lv_font_t * font_normal, const lv_font_t * font_subtitle,
                                 const lv_font_t * font_title)
{
    lv_style_init(&opa_cover);
    lv_style_set_bg_opa(&opa_cover, LV_STATE_DEFAULT, LV_OPA_COVER);

    theme.apply_xcb = theme_apply;
    return &theme;
}


void theme_apply(lv_obj_t * obj, lv_theme_style_t name)
{
    if(name == LV_THEME_SCR) {
        lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &opa_cover);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif
