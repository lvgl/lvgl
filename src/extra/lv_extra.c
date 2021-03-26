/**
 * @file lv_extra.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"

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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_extra_init(void)
{
#if LV_USE_FLEX
    lv_flex_init();
#endif

#if LV_USE_GRID
    lv_grid_init();
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
