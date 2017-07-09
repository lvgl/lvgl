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
#include "lv_conf.h"
#if USE_LV_LED != 0

#include "../lv_obj/lv_obj.h"

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
}lv_led_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a led objects
 * @param par pointer to an object, it will be the parent of the new led
 * @param copy pointer to a led object, if not NULL then the new object will be copied from it
 * @return pointer to the created led
 */
lv_obj_t * lv_led_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the led
 * @param led pointer to a led object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_led_signal(lv_obj_t * led, lv_signal_t sign, void * param);

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
void lv_led_tgl(lv_obj_t * led);

/**
 * Get the brightness of a LEd object
 * @param led pointer to LED object
 * @return bright 0 (max. dark) ... 255 (max. light)
 */
uint8_t lv_led_get_bright(lv_obj_t * led);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_LED*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_LED_H*/
