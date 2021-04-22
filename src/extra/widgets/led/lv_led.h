/**
 * @file lv_led.h
 *
 */

#ifndef LV_LED_H
#define LV_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_LED


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of led*/
typedef struct {
    lv_obj_t obj;
    lv_color_t color;
    uint8_t bright;     /**< Current brightness of the LED (0..255)*/
} lv_led_t;

extern const lv_obj_class_t lv_led_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a led objects
 * @param par pointer to an object, it will be the parent of the new led
 * @return pointer to the created led
 */
lv_obj_t * lv_led_create(lv_obj_t * parent);

/**
 * Set the color of the LED
 * @param led       pointer to a LED object
 * @param color     the color of the the LED
 */
void lv_led_set_color(lv_obj_t * led, lv_color_t color);

/**
 * Set the brightness of a LED object
 * @param led pointer to a LED object
 * @param bright LV_LED_BRIGHT_MIN (max. dark) ... LV_LED_BRIGHT_MAX (max. light)
 */
void lv_led_set_brightness(lv_obj_t * led, uint8_t bright);

/**
 * Light on a LED
 * @param led pointer to a LED object
 */
void lv_led_on(lv_obj_t * led);

/**
 * Light off a LED
 * @param led pointer to a LED object
 */
void lv_led_off(lv_obj_t * led);

/**
 * Toggle the state of a LED
 * @param led pointer to a LED object
 */
void lv_led_toggle(lv_obj_t * led);

/**
 * Get the brightness of a LEd object
 * @param led pointer to LED object
 * @return bright 0 (max. dark) ... 255 (max. light)
 */
uint8_t lv_led_get_brightness(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LED*/

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif /*LV_LED_H*/
