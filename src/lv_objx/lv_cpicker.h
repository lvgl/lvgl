/**
 * @file lv_cpicker.h
 *
 */

#ifndef LV_CPICKER_H
#define LV_CPICKER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#if LV_USE_CPICKER != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of colorpicker*/
typedef struct {
    uint16_t hue;
    uint8_t saturation;
    uint8_t value;
    struct
    {
        lv_style_t * style;
        uint8_t type;
    } indicator;
    uint16_t prev_hue;
    uint8_t prev_saturation;
    uint8_t prev_value;
    uint16_t prev_pos;
    uint8_t color_mode:2;
    uint8_t color_mode_fixed:1;
    uint8_t type:1;
    uint32_t last_click;
    lv_area_t rect_preview_area;
    lv_area_t rect_gradient_area;
} lv_cpicker_ext_t;

/*Styles*/
enum {
    LV_CPICKER_STYLE_MAIN,
    LV_CPICKER_STYLE_INDICATOR,
};
typedef uint8_t lv_cpicker_style_t;

enum {
    LV_CPICKER_INDICATOR_NONE,
    LV_CPICKER_INDICATOR_LINE,
    LV_CPICKER_INDICATOR_CIRCLE,
    LV_CPICKER_INDICATOR_IN
};
typedef uint8_t lv_cpicker_indicator_type_t;

enum {
    LV_CPICKER_TYPE_RECT,
    LV_CPICKER_TYPE_DISC,
};
typedef uint8_t lv_cpicker_type_t;

enum {
    LV_CPICKER_COLOR_MODE_HUE,
    LV_CPICKER_COLOR_MODE_SATURATION,
    LV_CPICKER_COLOR_MODE_VALUE
};
typedef uint8_t lv_cpicker_color_mode_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a colorpicker objects
 * @param par pointer to an object, it will be the parent of the new colorpicker
 * @param copy pointer to a colorpicker object, if not NULL then the new object will be copied from it
 * @return pointer to the created colorpicker
 */
lv_obj_t * lv_cpicker_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_cpicker_set_style(lv_obj_t * cpicker, lv_cpicker_style_t type, lv_style_t *style);

/**
 * Set a type of a colorpicker indicator.
 * @param cpicker pointer to colorpicker object
 * @param type indicator type
 */
void lv_cpicker_set_indicator_type(lv_obj_t * cpicker, lv_cpicker_indicator_type_t type);

/**
 * Set the current hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param hue current selected hue
 */
void lv_cpicker_set_hue(lv_obj_t * cpicker, uint16_t hue);

/**
 * Set the current saturation of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param saturation current selected saturation
 */
void lv_cpicker_set_saturation(lv_obj_t * cpicker, uint8_t saturation);

/**
 * Set the current value of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param val current selected value
 */
void lv_cpicker_set_value(lv_obj_t * cpicker, uint8_t val);

/**
 * Set the current color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param color current selected color
 */
void lv_cpicker_set_color(lv_obj_t * cpicker, lv_color_t color);

/**
 * Set the current color mode.
 * @param cpicker pointer to colorpicker object
 * @param mode color mode (hue/sat/val)
 */
void lv_cpicker_set_color_mode(lv_obj_t * cpicker, lv_cpicker_color_mode_t mode);

/**
 * Set if the color mode is changed on long press on center
 * @param cpicker pointer to colorpicker object
 * @param fixed color mode cannot be changed on long press
 */
void lv_cpicker_set_color_mode_fixed(lv_obj_t * cpicker, bool fixed);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current color mode.
 * @param cpicker pointer to colorpicker object
 * @return color mode (hue/sat/val)
 */
lv_cpicker_color_mode_t lv_cpicker_get_color_mode(lv_obj_t * cpicker);

/**
 * Get if the color mode is changed on long press on center
 * @param cpicker pointer to colorpicker object
 * @return mode cannot be changed on long press
 */
bool lv_cpicker_get_color_mode_fixed(lv_obj_t * cpicker);

/**
 * Get style of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_cpicker_get_style(const lv_obj_t * cpicker, lv_cpicker_style_t type);

/**
 * Get the current hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return hue current selected hue
 */
uint16_t lv_cpicker_get_hue(lv_obj_t * cpicker);

/**
 * Get the current saturation of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected saturation
 */
uint8_t lv_cpicker_get_saturation(lv_obj_t * cpicker);

/**
 * Get the current hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected value
 */
uint8_t lv_cpicker_get_value(lv_obj_t * cpicker);

/**
 * Get the current selected color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return color current selected color
 */
lv_color_t lv_cpicker_get_color(lv_obj_t * cpicker);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_CPICKER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_CPICKER_H*/
