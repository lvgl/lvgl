/**
 * @file lv_theme.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"

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
static lv_theme_t *current_theme;

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
void lv_theme_set_current(lv_theme_t *th)
{
    current_theme = th;
}

/**
 * Get the current system theme.
 * @return pointer to the current system theme. NULL if not set.
 */
lv_theme_t * lv_theme_get_current(void)
{
    return current_theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
