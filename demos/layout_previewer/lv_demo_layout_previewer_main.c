/**
 * @file lv_demo_layout_previewer_main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_layout_previewer_main.h"
#include "lv_demo_layout_previewer.h"

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

static view_t view;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_layout_previewer(void)
{
    view_create(lv_scr_act(), &view);
    ctrl_pad_attach(&view);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
