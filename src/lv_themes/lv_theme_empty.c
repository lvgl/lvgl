/**
 * @file lv_theme_empty.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_EMPTY

#include "../lv_misc/lv_gc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_style_t opa_cover;
} theme_styles_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void theme_apply(lv_theme_t * th, lv_obj_t * obj, lv_theme_style_t name);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t theme;
static theme_styles_t * styles;

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
    /* This trick is required only to avoid the garbage collection of
     * styles' data if LVGL is used in a binding (e.g. Micropython)
     * In a general case styles could be simple `static lv_style_t my style` variables*/
    if(styles == NULL) {
        styles = lv_mem_alloc(sizeof(theme_styles_t));
        if(styles == NULL) return NULL;
        _lv_memset_00(styles, sizeof(theme_styles_t));
        LV_GC_ROOT(_lv_theme_empty_styles) = styles;
    }

    theme.color_primary = color_primary;
    theme.color_secondary = color_secondary;
    theme.font_small = font_small;
    theme.font_normal = font_normal;
    theme.font_subtitle = font_subtitle;
    theme.font_title = font_title;
    theme.flags = flags;

    lv_style_reset(&styles->opa_cover);
    lv_style_set_bg_opa(&styles->opa_cover, LV_STATE_DEFAULT, LV_OPA_COVER);

    theme.apply_xcb = NULL;
    theme.apply_cb = theme_apply;
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void theme_apply(lv_theme_t * th, lv_obj_t * obj, lv_theme_style_t name)
{
    LV_UNUSED(th);
    if(name == LV_THEME_SCR) {
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &styles->opa_cover);
        lv_obj_refresh_style(obj, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }
}

#endif
