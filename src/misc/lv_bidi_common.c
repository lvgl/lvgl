/**
 * @file lv_bidi_common.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_bidi_private.h"

#include "../lvgl_public.h"


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

void lv_bidi_calculate_align(lv_text_align_t * align, lv_base_dir_t * base_dir, const char * txt)
{
    LV_CHECK_ARG(align != NULL, return);
    LV_CHECK_ARG(base_dir != NULL, return);
    LV_CHECK_ARG(txt != NULL, return);
    lv_bidi_calculate_align_internal(align, base_dir, txt);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
