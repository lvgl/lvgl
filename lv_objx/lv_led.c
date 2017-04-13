/**
 * @file lv_led.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_LED != 0

#include "lv_rect.h"
#include "lv_led.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LED_WIDTH_DEF    (LV_DPI / 2)
#define LV_LED_HEIGHT_DEF   (LV_DPI / 2)
#define LV_LED_BRIGHT_OFF	40
#define LV_LED_BRIGHT_ON	255

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_led_design(lv_obj_t * led, const area_t * mask, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_f_t ancestor_design_f;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*----------------- 
 * Create function
 *-----------------*/

/**
 * Create a led objects
 * @param par pointer to an object, it will be the parent of the new led
 * @param copy pointer to a led object, if not NULL then the new object will be copied from it
 * @return pointer to the created led
 */
lv_obj_t * lv_led_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor basic object*/
	lv_obj_t * new_led = lv_rect_create(par, copy);
    dm_assert(new_led);
    
    /*Allocate the object type specific extended data*/
    lv_led_ext_t * ext = lv_obj_alloc_ext(new_led, sizeof(lv_led_ext_t));
    dm_assert(ext);
    ext->bright = LV_LED_BRIGHT_ON;

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_led);

    lv_obj_set_signal_f(new_led, lv_led_signal);
    lv_obj_set_design_f(new_led, lv_led_design);

    /*Init the new led object*/
    if(copy == NULL) {
    	lv_obj_set_style(new_led, lv_style_get(LV_STYLE_PRETTY_COLOR, NULL));
    	lv_obj_set_size(new_led, LV_LED_WIDTH_DEF, LV_LED_HEIGHT_DEF);
    }
    /*Copy an existing object*/
    else {
    	lv_led_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->bright = copy_ext->bright;

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_led);
    }
    
    return new_led;
}

/**
 * Signal function of the led
 * @param led pointer to a led object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_led_signal(lv_obj_t * led, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_rect_signal(led, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			/*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    			break;
    		default:
    			break;
    	}
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the brightness of a LED object
 * @param led pointer to a LED object
 * @param bright 0 (max. dark) ... 255 (max. light)
 */
void lv_led_set_bright(lv_obj_t * led, uint8_t bright)
{
	/*Set the brightness*/
	lv_led_ext_t * ext = lv_obj_get_ext(led);
	ext->bright = bright;

	/*Invalidate the object there fore it will be redrawn*/
	lv_obj_inv(led);
}

/**
 * Light on a LED
 * @param led pointer to a LED object
 */
void lv_led_on(lv_obj_t * led)
{
	lv_led_set_bright(led, LV_LED_BRIGHT_ON);
}

/**
 * Light off a LED
 * @param led pointer to a LED object
 */
void lv_led_off(lv_obj_t * led)
{
	lv_led_set_bright(led, LV_LED_BRIGHT_OFF);
}


/**
 * Toggle the state of a LED
 * @param led pointer to a LED object
 */
void lv_led_tgl(lv_obj_t * led)
{
	uint8_t bright = lv_led_get_bright(led);
	if(bright > (LV_LED_BRIGHT_OFF + LV_LED_BRIGHT_ON) >> 1) lv_led_off(led);
	else lv_led_on(led);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the brightness of a LEd object
 * @param led pointer to LED object
 * @return bright 0 (max. dark) ... 255 (max. light)
 */
uint8_t lv_led_get_bright(lv_obj_t * led)
{
	lv_led_ext_t * ext = lv_obj_get_ext(led);
	return ext->bright;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the leds
 * @param led pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_led_design(lv_obj_t * led, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask area*/
    	return ancestor_design_f(led, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Make darker colors in a temporary style according to the brightness*/
		lv_led_ext_t * ext = lv_obj_get_ext(led);
		lv_style_t * style = lv_obj_get_style(led);

		/*Create a temporal style*/
        lv_style_t leds_tmp;
		memcpy(&leds_tmp, style, sizeof(leds_tmp));

		/*Mix. the color with black proportionally with brightness*/
		leds_tmp.mcolor = color_mix(leds_tmp.mcolor, COLOR_BLACK, ext->bright);
		leds_tmp.gcolor = color_mix(leds_tmp.gcolor, COLOR_BLACK, ext->bright);

		/*Set the current swidth according to brightness proportionally between LV_LED_BRIGHT_OFF and LV_LED_BRIGHT_ON*/
		leds_tmp.swidth = (uint16_t)(uint16_t)(ext->bright * style->swidth) >> 8;

		led->style_p = &leds_tmp;
		ancestor_design_f(led, mask, mode);
        led->style_p = style;
    }
    return true;
}

#endif
