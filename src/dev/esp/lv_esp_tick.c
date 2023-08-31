/**
 * @file lv_esp_tick.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_esp_tick.h"
#if LV_USE_ESP_TICK

#include <lvgl/lvgl.h>

#include "esp_timer.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void tick_timer_cb(void * arg);


/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

esp_timer_handle_t lv_esp_tick_init(void)
{
    const esp_timer_create_args_t tick_timer_args = {
        .callback = &tick_timer_cb,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t tick_timer = NULL;
    esp_timer_create(&tick_timer_args, &tick_timer);
    esp_timer_start_periodic(tick_timer, LV_ESP_TICK_PERIOD_MS * 1000);
    return tick_timer;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void tick_timer_cb(void * arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(LV_ESP_TICK_PERIOD_MS);
}

#endif /*LV_USE_ESP_TICK*/
