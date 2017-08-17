/**
 * @file lv_hello_world.c
 *
 */

/*
 * Greetings,
 * this is the first example in the tutorial hence this is the most simple one.
 * It only creates a Label, set its text, and align to the middle.
 *
 * Be sure in lv_conf.h LV_APP_ENEBLE is 0 (just for simplicity)
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_hello_world.h"
#if USE_LV_EXAMPLE != 0

#include "lvgl/lvgl.h"

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

/**
 * Create a simple 'Hello world!' label
 */
void lv_hello_world_init(void)
{
    /*Create a Label on the current screen*/
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);

    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello world!");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the and means an x, y offset after alignment*/
    lv_obj_align_us(label1, NULL, LV_ALIGN_CENTER, 0, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_EXAMPLE != 0*/
