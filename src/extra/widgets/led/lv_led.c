/**
 * @file lv_led.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_led.h"
#if LV_USE_LED

/*********************
 *      DEFINES
 *********************/

#define LV_LED_WIDTH_DEF (LV_DPI_DEF / 5)
#define LV_LED_HEIGHT_DEF (LV_DPI_DEF / 5)

#ifndef LV_LED_BRIGHT_MIN
# define LV_LED_BRIGHT_MIN 80
#endif

#ifndef LV_LED_BRIGHT_MAX
# define LV_LED_BRIGHT_MAX 255
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_led_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static lv_draw_res_t lv_led_draw(lv_obj_t * led, const lv_area_t * clip_area, lv_draw_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_led_class  = {
        .base_class = &lv_obj_class,
        .constructor_cb = lv_led_constructor,
        .draw_cb = lv_led_draw,
        .instance_size = sizeof(lv_led_t),
};

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
lv_obj_t * lv_led_create(lv_obj_t * parent)
{
    return lv_obj_create_from_class(&lv_led_class, parent, NULL);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the color of the LED
 * @param led       pointer to a LED object
 * @param color     the color of the the LED
 */
void lv_led_set_color(lv_obj_t * led, lv_color_t color)
{

}
/**
 * Set the brightness of a LED object
 * @param led pointer to a LED object
 * @param bright LV_LED_BRIGHT_MIN (max. dark) ... LV_LED_BRIGHT_MAX (max. light)
 */
void lv_led_set_brightness(lv_obj_t * obj, uint8_t bright)
{
    /*Set the brightness*/
    lv_led_t * led = (lv_led_t *) obj;
    if(led->bright == bright) return;

    if(bright <= LV_LED_BRIGHT_MIN) bright = LV_LED_BRIGHT_MIN;
    if(bright >= LV_LED_BRIGHT_MAX) bright = LV_LED_BRIGHT_MAX;

    led->bright = bright;

    /*Invalidate the object there fore it will be redrawn*/
    lv_obj_invalidate(obj);
}

/**
 * Light on a LED
 * @param led pointer to a LED object
 */
void lv_led_on(lv_obj_t * led)
{
    lv_led_set_brightness(led, LV_LED_BRIGHT_MAX);
}

/**
 * Light off a LED
 * @param led pointer to a LED object
 */
void lv_led_off(lv_obj_t * led)
{
    lv_led_set_brightness(led, LV_LED_BRIGHT_MIN);
}

/**
 * Toggle the state of a LED
 * @param led pointer to a LED object
 */
void lv_led_toggle(lv_obj_t * obj)
{
    uint8_t bright = lv_led_get_brightness(obj);
    if(bright > (LV_LED_BRIGHT_MIN + LV_LED_BRIGHT_MAX) >> 1)
        lv_led_off(obj);
    else
        lv_led_on(obj);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the brightness of a LEd object
 * @param led pointer to LED object
 * @return bright 0 (max. dark) ... 255 (max. light)
 */
uint8_t lv_led_get_brightness(const lv_obj_t * obj)
{
    lv_led_t * led = (lv_led_t *) obj;
    return led->bright;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_led_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{

    lv_led_t * led = (lv_led_t *) obj;
    led->color = lv_theme_get_color_primary();
    led->bright = LV_LED_BRIGHT_MAX;
    led->bright = LV_LED_BRIGHT_MAX;

    lv_obj_set_size(obj, LV_LED_WIDTH_DEF, LV_LED_HEIGHT_DEF);
}


static lv_draw_res_t lv_led_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    if(mode == LV_DRAW_MODE_COVER_CHECK) {
        /*Return false if the object is not covers the clip_area area*/
        return lv_obj_draw_base(&lv_led_class, obj, clip_area, mode);
    }
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {
        /*Make darker colors in a temporary style according to the brightness*/
        lv_led_t * led = (lv_led_t *) obj;

        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &rect_dsc);

        /*Use the original colors brightness to modify color->led*/
        rect_dsc.bg_color = lv_color_mix(led->color, LV_COLOR_BLACK, lv_color_brightness(rect_dsc.bg_color));
        rect_dsc.bg_grad_color = lv_color_mix(led->color, LV_COLOR_BLACK, lv_color_brightness(rect_dsc.bg_grad_color));
        rect_dsc.shadow_color = lv_color_mix(led->color, LV_COLOR_BLACK, lv_color_brightness(rect_dsc.shadow_color));
        rect_dsc.border_color = lv_color_mix(led->color, LV_COLOR_BLACK, lv_color_brightness(rect_dsc.border_color));
        rect_dsc.outline_color = lv_color_mix(led->color, LV_COLOR_BLACK, lv_color_brightness(rect_dsc.outline_color));

        /*Mix. the color with black proportionally with brightness*/
        rect_dsc.bg_color   = lv_color_mix(rect_dsc.bg_color, LV_COLOR_BLACK, led->bright);
        rect_dsc.bg_grad_color   = lv_color_mix(rect_dsc.bg_grad_color, LV_COLOR_BLACK, led->bright);
        rect_dsc.border_color = lv_color_mix(rect_dsc.border_color, LV_COLOR_BLACK, led->bright);
        rect_dsc.shadow_color = lv_color_mix(rect_dsc.shadow_color, LV_COLOR_BLACK, led->bright);
        rect_dsc.outline_color = lv_color_mix(rect_dsc.outline_color, LV_COLOR_BLACK, led->bright);

        /*Set the current shadow width according to brightness proportionally between LV_LED_BRIGHT_OFF
         * and LV_LED_BRIGHT_ON*/
        rect_dsc.shadow_width = ((led->bright - LV_LED_BRIGHT_MIN) * rect_dsc.shadow_width) /
                                (LV_LED_BRIGHT_MAX - LV_LED_BRIGHT_MIN);
        rect_dsc.shadow_spread = ((led->bright - LV_LED_BRIGHT_MIN) * rect_dsc.shadow_spread) /
                                 (LV_LED_BRIGHT_MAX - LV_LED_BRIGHT_MIN);

        lv_draw_rect(&obj->coords, clip_area, &rect_dsc);
    }
    return LV_DRAW_RES_OK;
}
#endif
