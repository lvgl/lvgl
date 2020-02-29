/**
 * @file lv_theme_empty.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include "lv_theme.h"
#include "../lv_widgets/lv_img.h"
#include "../lv_misc/lv_types.h"

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
static void lv_theme_empty_apply(lv_obj_t * obj, lv_theme_style_t name);


/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t theme;

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
                                    lv_font_t * font_small, lv_font_t * font_normal, lv_font_t * font_subtitle, lv_font_t * font_title)
{
    theme.apply_cb = lv_theme_empty_apply;
    return &theme;
}


void lv_theme_empty_apply(lv_obj_t * obj, lv_theme_style_t name)
{

}

/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif
