/**
 * @file lv_theme.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"
#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_THEME_LIVE_UPDATE == 0
static lv_theme_t * current_theme;
#else
/* If live update is used then a big `lv_style_t` array is used to store the real styles of the theme not only pointers.
 * On `lv_theme_set_current` the styles of the theme are copied to this array.
 * The pointers in `current_theme` are initialized to point to the styles in the array.
 * This way the theme styles will always point to the same memory address even after theme is change.
 * (The pointers in the theme points to the styles declared by the theme itself) */

/* Store the styles in this array.
 * Can't determine the size in compile time because sizeof is not evaluated (should be `sizeof(lv_theme_t) / sizeof(lv_style_t*)`).
 * Error will be generated in run time if too small.*/
static lv_style_t th_styles[100];
static bool inited = false;
static lv_theme_t current_theme;
#endif

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
void lv_theme_set_current(lv_theme_t * th)
{
#if LV_THEME_LIVE_UPDATE == 0
    current_theme = th;
#else
    uint32_t style_num = sizeof(lv_theme_t) / sizeof(lv_style_t *);     /*Number of styles in a theme*/

    if(!inited) {
        /*It's not sure `th_styles` is big enough. Check it now!*/
        if(style_num > sizeof(th_styles) / sizeof(lv_style_t)) {
            LV_LOG_ERROR("Themes: th_styles array is too small. Increase it's size!");
            while(1);
        }

        /*Initialize the style pointers `current_theme` to point to the `th_styles` style array */
        uint16_t i;
        lv_style_t ** cur_th_style_p = (lv_style_t **) &current_theme;
        for(i = 0; i < style_num; i++) {
            uint64_t adr = (uint64_t)&th_styles[i];
            memcpy(&cur_th_style_p[i], &adr, sizeof(lv_style_t *));
        }
        inited = true;
    }


    /*Copy the styles pointed by the new theme to the `th_styles` style array*/
    uint16_t i;
    lv_style_t ** th_style = (lv_style_t **) th;
    for(i = 0; i < style_num; i++) {
        uint64_t s = (uint64_t)th_style[i];
        if(s) memcpy(&th_styles[i], (void *)s, sizeof(lv_style_t));
    }

    /*Let the object know their style might change*/
    lv_obj_report_style_mod(NULL);
#endif
}

/**
 * Get the current system theme.
 * @return pointer to the current system theme. NULL if not set.
 */
lv_theme_t * lv_theme_get_current(void)
{
#if LV_THEME_LIVE_UPDATE == 0
    return current_theme;
#else
    if(!inited) return NULL;
    else return &current_theme;
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
