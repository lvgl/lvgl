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
#define LV_LED_BRIGHTNESS_DEF	128
#define LV_LED_BRIGHTNESS_ON	50
#define LV_LED_BRIGHTNESS_OFF	255

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_led_design(lv_obj_t * led, const area_t * mask, lv_design_mode_t mode);
static void lv_leds_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_leds_t lv_leds_def; /*Red*/
static lv_leds_t lv_leds_green;

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
    ext->bright = LV_LED_BRIGHTNESS_DEF;

    lv_obj_set_signal_f(new_led, lv_led_signal);
    lv_obj_set_design_f(new_led, lv_led_design);

    /*Init the new led object*/
    if(copy == NULL) {
    	lv_obj_set_style(new_led, lv_leds_get(LV_LEDS_DEF, NULL));
    	lv_obj_set_size_us(new_led, 40, 40);
    }
    /*Copy an existing object*/
    else {
    	lv_led_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->bright = copy_ext->bright;

    	/*Set the style of 'copy' and isolate it if it is necessary*/
        if(lv_obj_get_style_iso(new_led) == false) {
            lv_obj_set_style(new_led, lv_obj_get_style(copy));
        } else {
            lv_obj_set_style(new_led, lv_obj_get_style(copy));
            lv_obj_iso_style(new_led, sizeof(lv_leds_t));
        }
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
	lv_led_set_bright(led, LV_LED_BRIGHTNESS_ON);
}

/**
 * Light off a LED
 * @param led pointer to a LED object
 */
void lv_led_off(lv_obj_t * led)
{
	lv_led_set_bright(led, LV_LED_BRIGHTNESS_OFF);
}


/**
 * Toggle the state of a LED
 * @param led pointer to a LED object
 */
void lv_led_tgl(lv_obj_t * led)
{
	uint8_t bright = lv_led_get_bright(led);
	if(bright > 60) lv_led_off(led);
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

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_leds_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_leds_t style
 */
lv_leds_t * lv_leds_get(lv_leds_builtin_t style, lv_leds_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_leds_init();
		style_inited = true;
	}

	lv_leds_t  *style_p;

	switch(style) {
		case LV_LEDS_DEF:
		case LV_LEDS_RED:
			style_p = &lv_leds_def;
			break;
		case LV_LEDS_GREEN:
			style_p = &lv_leds_green;
			break;
		default:
			style_p = &lv_leds_def;
	}

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_leds_t));
		else memcpy(copy, &lv_leds_def, sizeof(lv_leds_t));
	}

	return style_p;
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
    	/*Return false if the object is not covers the mask_p area*/
    	return false;
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Make darker colors in a temporary style according to the brightness*/
		lv_led_ext_t * ext = lv_obj_get_ext(led);
		lv_leds_t * style = lv_obj_get_style(led);
		lv_leds_t leds_tmp;

		memcpy(&leds_tmp, style, sizeof(leds_tmp));

		leds_tmp.bg_rect.objs.color = color_mix(leds_tmp.bg_rect.objs.color, COLOR_BLACK, ext->bright);
		leds_tmp.bg_rect.gcolor = color_mix(leds_tmp.bg_rect.gcolor, COLOR_BLACK, ext->bright);

		opa_t opa = lv_obj_get_opa(led);
		area_t area;
		lv_obj_get_cords(led, &area);

		lv_draw_rect(&area, mask, &leds_tmp.bg_rect, opa);
    }
    return true;
}

/**
 * Initialize the led styles
 */
static void lv_leds_init(void)
{
	/*Default style*/
	lv_rects_get(LV_RECTS_DEF, &lv_leds_def.bg_rect);
	lv_leds_def.bg_rect.objs.color = COLOR_RED;
	lv_leds_def.bg_rect.gcolor = COLOR_MARRON,
	lv_leds_def.bg_rect.bcolor = COLOR_WHITE;
	lv_leds_def.bg_rect.bwidth = 4 * LV_DOWNSCALE;
	lv_leds_def.bg_rect.bopa = 50;
	lv_leds_def.bg_rect.round = LV_RECT_CIRCLE;
	lv_leds_def.bg_rect.hpad = 0;
	lv_leds_def.bg_rect.vpad = 0;
	lv_leds_def.bg_rect.opad = 0;

	/* Green style */
	memcpy(&lv_leds_green, &lv_leds_def, sizeof(lv_leds_t));
	lv_leds_green.bg_rect.objs.color = COLOR_LIME;
	lv_leds_green.bg_rect.gcolor = COLOR_GREEN;
	lv_leds_green.bg_rect.bcolor = COLOR_WHITE;
}

#endif
