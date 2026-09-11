/**
 * @file lv_test_helpers.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"

#if LV_USE_TEST

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

    lv_indev_read(lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER));
    lv_indev_read(lv_test_indev_get_indev(LV_INDEV_TYPE_ENCODER));
    lv_indev_read(lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD));
    lv_refr_now(NULL);
}

void lv_test_fast_forward(uint32_t ms)
{
    lv_tick_inc(ms);
    lv_timer_handler();
    lv_indev_read(lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER));
    lv_indev_read(lv_test_indev_get_indev(LV_INDEV_TYPE_ENCODER));
    lv_indev_read(lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD));
    lv_refr_now(NULL);
}

size_t lv_test_get_allocation_count(void)
{
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    return mon.used_cnt;
#else
    /* The heap does not tell how many allocations are live */
    return 0;
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_TEST*/
