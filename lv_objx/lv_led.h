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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if USE_LV_LED != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of led*/
typedef struct
{
    /*No inherited ext.*/
    /*New data for this type */
    uint8_t bright;         /*Current brightness of the LED (0..255)*/
} lv_led_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a led objects
 * @param par pointer to an object, it will be the parent of the new led
 * @param copy pointer to a led object, if not NULL then the new object will be copied from it
 * @return pointer to the created led
 */
lv_obj_t * lv_led_create(lv_obj_t * par, const lv_obj_t * copy);

/**
 * Set the brightness of a LED object
 * @param led pointer to a LED object
 * @param bright 0 (max. dark) ... 255 (max. light)
 */
void lv_led_set_bright(lv_obj_t * led, uint8_t bright);

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
 * Set the style of a led
 * @param led pointer to a led object
 * @param style pointer to a style
 */
static inline void lv_led_set_style(lv_obj_t *led, lv_style_t *style)
{
    lv_obj_set_style(led, style);
}

/**
 * Get the brightness of a LEd object
 * @param led pointer to LED object
 * @return bright 0 (max. dark) ... 255 (max. light)
 */
uint8_t lv_led_get_bright(const lv_obj_t * led);

/**
 * Get the style of an led object
 * @param led pointer to an led object
 * @return pointer to the led's style
 */
static inline lv_style_t* lv_led_get_style(const lv_obj_t *led)
{
    return lv_obj_get_style(led);
}

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_LED*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_LED_H*/
