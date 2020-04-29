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
#include "../lv_conf_internal.h"

#if LV_USE_CPICKER != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

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



/*Data of colorpicker*/
typedef struct {
    lv_color_hsv_t hsv;
    struct {
        lv_style_list_t style_list;
        lv_point_t pos;
        uint8_t colored     : 1;

    } knob;
    uint32_t last_click_time;
    uint32_t last_change_time;
    lv_point_t last_press_point;
    lv_cpicker_color_mode_t color_mode  : 2;
    uint8_t color_mode_fixed            : 1;
    lv_cpicker_type_t type              : 1;
} lv_cpicker_ext_t;

/*Parts*/
enum {
    LV_CPICKER_PART_MAIN = LV_OBJ_PART_MAIN,
    LV_CPICKER_PART_KNOB = _LV_OBJ_PART_VIRTUAL_LAST,
    _LV_CPICKER_PART_VIRTUAL_LAST,
    _LV_CPICKER_PART_REAL_LAST = _LV_OBJ_PART_REAL_LAST,
};


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
 * Set a new type for a colorpicker
 * @param cpicker pointer to a colorpicker object
 * @param type new type of the colorpicker (from 'lv_cpicker_type_t' enum)
 */
void lv_cpicker_set_type(lv_obj_t * cpicker, lv_cpicker_type_t type);

/**
 * Set the current hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param hue current selected hue [0..360]
 * @return true if changed, otherwise false
 */
bool lv_cpicker_set_hue(lv_obj_t * cpicker, uint16_t hue);

/**
 * Set the current saturation of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param saturation current selected saturation [0..100]
 * @return true if changed, otherwise false
 */
bool lv_cpicker_set_saturation(lv_obj_t * cpicker, uint8_t saturation);

/**
 * Set the current value of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param val current selected value [0..100]
 * @return true if changed, otherwise false
 */
bool lv_cpicker_set_value(lv_obj_t * cpicker, uint8_t val);

/**
 * Set the current hsv of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param hsv current selected hsv
 * @return true if changed, otherwise false
 */
bool lv_cpicker_set_hsv(lv_obj_t * cpicker, lv_color_hsv_t hsv);

/**
 * Set the current color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param color current selected color
 * @return true if changed, otherwise false
 */
bool lv_cpicker_set_color(lv_obj_t * cpicker, lv_color_t color);

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

/**
 * Make the knob to be colored to the current color
 * @param cpicker pointer to colorpicker object
 * @param en true: color the knob; false: not color the knob
 */
void lv_cpicker_set_knob_colored(lv_obj_t * cpicker, bool en);

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
 * Get the current hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected hue
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
 * Get the current selected hsv of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected hsv
 */
lv_color_hsv_t lv_cpicker_get_hsv(lv_obj_t * cpicker);

/**
 * Get the current selected color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected color
 */
lv_color_t lv_cpicker_get_color(lv_obj_t * cpicker);

/**
 * Whether the knob is colored to the current color or not
 * @param cpicker pointer to color picker object
 * @return true: color the knob; false: not color the knob
 */
bool lv_cpicker_get_knob_colored(lv_obj_t * cpicker);

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
