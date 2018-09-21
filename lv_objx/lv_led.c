/**
 * @file lv_led.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_LED != 0

#include "lv_led.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LED_WIDTH_DEF    (LV_DPI / 3)
#define LV_LED_HEIGHT_DEF   (LV_DPI / 3)
#define LV_LED_BRIGHT_OFF	100
#define LV_LED_BRIGHT_ON	255

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_led_design(lv_obj_t * led, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_led_signal(lv_obj_t * led, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_func_t ancestor_design_f;
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a led objects
 * @param par pointer to an object, it will be the parent of the new led
 * @param copy pointer to a led object, if not NULL then the new object will be copied from it
 * @return pointer to the created led
 */
lv_obj_t * lv_led_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor basic object*/
	lv_obj_t * new_led = lv_obj_create(par, copy);
    lv_mem_assert(new_led);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_led);
    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_func(new_led);
    
    /*Allocate the object type specific extended data*/
    lv_led_ext_t * ext = lv_obj_allocate_ext_attr(new_led, sizeof(lv_led_ext_t));
    lv_mem_assert(ext);
    ext->bright = LV_LED_BRIGHT_ON;

    lv_obj_set_signal_func(new_led, lv_led_signal);
    lv_obj_set_design_func(new_led, lv_led_design);

    /*Init the new led object*/
    if(copy == NULL) {
    	lv_obj_set_size(new_led, LV_LED_WIDTH_DEF, LV_LED_HEIGHT_DEF);

        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            lv_led_set_style(new_led, th->led);
        } else {
            lv_led_set_style(new_led, &lv_style_pretty_color);
        }
    }
    /*Copy an existing object*/
    else {
    	lv_led_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	ext->bright = copy_ext->bright;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_led);
    }
    
    return new_led;
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
	lv_led_ext_t * ext = lv_obj_get_ext_attr(led);
	if(ext->bright == bright) return;

	ext->bright = bright;

	/*Invalidate the object there fore it will be redrawn*/
	lv_obj_invalidate(led);
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
void lv_led_toggle(lv_obj_t * led)
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
	lv_led_ext_t * ext = lv_obj_get_ext_attr(led);
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
static bool lv_led_design(lv_obj_t * led, const lv_area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask area*/
    	return ancestor_design_f(led, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Make darker colors in a temporary style according to the brightness*/
		lv_led_ext_t * ext = lv_obj_get_ext_attr(led);
		lv_style_t * style = lv_obj_get_style(led);

        /* Store the real pointer because of 'lv_group'
         * If the object is in focus 'lv_obj_get_style()' will give a pointer to tmp style
         * and to the real object style. It is important because of style change tricks below*/
        lv_style_t *style_ori_p = led->style_p;

		/*Create a temporal style*/
        lv_style_t leds_tmp;
		memcpy(&leds_tmp, style, sizeof(leds_tmp));

		/*Mix. the color with black proportionally with brightness*/
		leds_tmp.body.main_color = lv_color_mix(leds_tmp.body.main_color, LV_COLOR_BLACK, ext->bright);
		leds_tmp.body.grad_color = lv_color_mix(leds_tmp.body.grad_color, LV_COLOR_BLACK, ext->bright);
        leds_tmp.body.border.color = lv_color_mix(leds_tmp.body.border.color, LV_COLOR_BLACK, ext->bright);

		/*Set the current swidth according to brightness proportionally between LV_LED_BRIGHT_OFF and LV_LED_BRIGHT_ON*/
		uint16_t bright_tmp = ext->bright;
        leds_tmp.body.shadow.width = ((bright_tmp - LV_LED_BRIGHT_OFF) * style->body.shadow.width) / (LV_LED_BRIGHT_ON - LV_LED_BRIGHT_OFF);

		led->style_p = &leds_tmp;
		ancestor_design_f(led, mask, mode);
        led->style_p = style_ori_p;                 /*Restore the ORIGINAL style pointer*/
    }
    return true;
}

/**
 * Signal function of the led
 * @param led pointer to a led object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_led_signal(lv_obj_t * led, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(led, sign, param);
    if(res != LV_RES_OK) return res;


    if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_led";
    }

    return res;
}
#endif
