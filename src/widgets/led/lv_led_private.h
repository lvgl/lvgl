/**
 * @file lv_led_private.h
 *
 */

#ifndef LV_LED_PRIVATE_H
#define LV_LED_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_led.h"

#if LV_USE_LED
#include "../../core/lv_obj_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Data of led */
struct lv_led_t {
    lv_obj_t obj;
    lv_color_t color;
    uint8_t bright;     /**< Current brightness of the LED (0..255)*/
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LED */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LED_PRIVATE_H*/
