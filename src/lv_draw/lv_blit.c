/**
 * @file lv_blit.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_blit.h"

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

void lv_blit_color(lv_color_t * dest_buf, lv_color_t * bg_buf, lv_coord_t len, lv_color_t color, lv_blit_mode_t mode)
{
    uint32_t i;
    for(i = 0; i < len; i++) {
        dest_buf[i].full = color.full;
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
