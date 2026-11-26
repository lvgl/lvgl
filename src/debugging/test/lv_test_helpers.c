/**
 * @file lv_test_helpers.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"

#if LV_USE_TEST

#include "../../display/lv_display_private.h"

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
    uint32_t step = 1;
    while(ms) {
        lv_tick_inc(step);
        ms -= step;
        lv_display_t * disp;

        /* to speed up a potential slow render, disable rendering here and render only once
         * after the the wait (see `lv_refr_now` at the bottom of the function)
         * We need to keep disabling the display refr_timer as any other timer (e.g an animation)
         * might resume it again */
        for(disp = lv_display_get_next(NULL); disp; disp = lv_display_get_next(disp)) {
            if(disp->refr_timer) {
                lv_timer_reset(disp->refr_timer);
            }
        }
        uint32_t next = lv_timer_handler();

        /* still update the layouts as animations might rely on the object positions */
        for(disp = lv_display_get_next(NULL); disp; disp = lv_display_get_next(disp)) {
            lv_obj_update_layout(lv_display_get_screen_active(disp));
        }
        step = next == LV_NO_TIMER_READY ? ms : LV_CLAMP(1, next, ms);
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
