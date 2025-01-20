/**
 * @file lv_led.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_led_private.h"
#include "../../core/lv_obj_private.h"
#include "../../core/lv_obj_class_private.h"

#if LV_USE_LED

#include "../../misc/lv_assert.h"
#include "../../themes/lv_theme.h"
#include "../../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_led_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_led_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_led_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_led_class  = {
    .base_class = &lv_obj_class,
    .constructor_cb = lv_led_constructor,
    .width_def = LV_DPI_DEF / 5,
    .height_def = LV_DPI_DEF / 5,
    .event_cb = lv_led_event,
    .instance_size = sizeof(lv_led_t),
    .name = "led",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_led_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_led_set_color(lv_obj_t * obj, lv_color_t color)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_led_t * led = (lv_led_t *)obj;
    led->color = color;
    lv_obj_invalidate(obj);
}

void lv_led_set_brightness(lv_obj_t * obj, uint8_t bright)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_led_t * led = (lv_led_t *)obj;
    if(led->bright == bright) return;

    led->bright = LV_CLAMP(LV_LED_BRIGHT_MIN, bright, LV_LED_BRIGHT_MAX);

    /*Invalidate the object there fore it will be redrawn*/
    lv_obj_invalidate(obj);
}

void lv_led_on(lv_obj_t * led)
{
    lv_led_set_brightness(led, LV_LED_BRIGHT_MAX);
}

void lv_led_off(lv_obj_t * led)
{
    lv_led_set_brightness(led, LV_LED_BRIGHT_MIN);
}

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

uint8_t lv_led_get_brightness(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_led_t * led = (lv_led_t *)obj;
    return led->bright;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_led_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_led_t * led = (lv_led_t *)obj;
    led->color = lv_theme_get_color_primary(obj);
    led->bright = LV_LED_BRIGHT_MAX;
}

static void lv_led_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /* Call the ancestor's event handler */
    lv_event_code_t code = lv_event_get_code(e);
    if(code != LV_EVENT_DRAW_MAIN && code != LV_EVENT_DRAW_MAIN_END) {
        res = lv_obj_event_base(MY_CLASS, e);
        if(res != LV_RESULT_OK) return;
    }

    lv_obj_t * obj = lv_event_get_current_target(e);
    if(code == LV_EVENT_DRAW_MAIN) {
        /*Make darker colors in a temporary style according to the brightness*/
        lv_led_t * led = (lv_led_t *)obj;
        lv_layer_t * layer = lv_event_get_layer(e);

        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.base.layer = layer;
        lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &rect_dsc);

        /*Use the original colors brightness to modify color->led*/
        rect_dsc.bg_color = lv_color_mix(led->color, lv_color_black(), lv_color_brightness(rect_dsc.bg_color));
        rect_dsc.bg_grad.stops[0].color = lv_color_mix(led->color, lv_color_black(),
                                                       lv_color_brightness(rect_dsc.bg_grad.stops[0].color));
        rect_dsc.bg_grad.stops[1].color = lv_color_mix(led->color, lv_color_black(),
                                                       lv_color_brightness(rect_dsc.bg_grad.stops[1].color));
        rect_dsc.shadow_color = lv_color_mix(led->color, lv_color_black(), lv_color_brightness(rect_dsc.shadow_color));
        rect_dsc.border_color = lv_color_mix(led->color, lv_color_black(), lv_color_brightness(rect_dsc.border_color));
        rect_dsc.outline_color = lv_color_mix(led->color, lv_color_black(), lv_color_brightness(rect_dsc.outline_color));

        /*Mix. the color with black proportionally with brightness*/
        rect_dsc.bg_color = lv_color_mix(rect_dsc.bg_color, lv_color_black(), led->bright);
        rect_dsc.bg_grad.stops[0].color   = lv_color_mix(rect_dsc.bg_grad.stops[0].color, lv_color_black(), led->bright);
        rect_dsc.bg_grad.stops[1].color   = lv_color_mix(rect_dsc.bg_grad.stops[1].color, lv_color_black(), led->bright);
        rect_dsc.border_color = lv_color_mix(rect_dsc.border_color, lv_color_black(), led->bright);
        rect_dsc.shadow_color = lv_color_mix(rect_dsc.shadow_color, lv_color_black(), led->bright);
        rect_dsc.outline_color = lv_color_mix(rect_dsc.outline_color, lv_color_black(), led->bright);

        /*Set the current shadow width according to brightness proportionally between LV_LED_BRIGHT_OFF
         * and LV_LED_BRIGHT_ON*/
        rect_dsc.shadow_width = ((led->bright - LV_LED_BRIGHT_MIN) * rect_dsc.shadow_width) /
                                (LV_LED_BRIGHT_MAX - LV_LED_BRIGHT_MIN);
        rect_dsc.shadow_spread = ((led->bright - LV_LED_BRIGHT_MIN) * rect_dsc.shadow_spread) /
                                 (LV_LED_BRIGHT_MAX - LV_LED_BRIGHT_MIN);

        lv_draw_rect(layer, &rect_dsc, &obj->coords);
    }
}

#endif
