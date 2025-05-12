/**
 * @file lv_test_helpers.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_test_helpers.h"

#if LV_USE_TEST
#include "../../lvgl.h"

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

void lv_test_wait(uint32_t ms)
{
    while(ms) {
        lv_tick_inc(1);
        lv_timer_handler();
        ms--;
    }
    lv_refr_now(NULL);
}

void lv_test_fast_forward(uint32_t ms)
{
    lv_tick_inc(ms);
    lv_timer_handler();
    lv_refr_now(NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_TEST*/
