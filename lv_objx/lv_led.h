/**
 * @file lv_led.h
 * 
 */

#ifndef LV_LED_H
#define LV_LED_H

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

/*Style of led*/
typedef struct
{
	lv_rects_t bg_rect;/*Style of ancestor*/
	/*New style element for this type */
}lv_leds_t;

/*Built-in styles of led*/
typedef enum
{
	LV_LEDS_DEF,
	LV_LEDS_RED,
	LV_LEDS_GREEN,
}lv_leds_builtin_t;

/*Data of led*/
typedef struct
{
	lv_rect_ext_t rect_ext;	/*Ext. of ancestor*/
	/*New data for this type */
	uint8_t bright;			/*Current brightness of the LED*/
}lv_led_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_led_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_led_signal(lv_obj_t * led, lv_signal_t sign, void * param);
lv_leds_t * lv_leds_get(lv_leds_builtin_t style, lv_leds_t * copy);

void lv_led_set_bright(lv_obj_t * led, uint8_t bright);
uint8_t lv_led_get_bright(lv_obj_t * led);

void lv_led_on(lv_obj_t * led);
void lv_led_off(lv_obj_t * led);
void lv_led_tgl(lv_obj_t * led);


/**********************
 *      MACROS
 **********************/

#endif

#endif
