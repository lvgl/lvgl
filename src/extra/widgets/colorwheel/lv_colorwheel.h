/**
 * @file lv_colorwheel.h
 *
 */

#ifndef LV_COLORWHEEL_H
#define LV_COLORWHEEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_COLORWHEEL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_COLORWHEEL_MODE_HUE,
    LV_COLORWHEEL_MODE_SATURATION,
    LV_COLORWHEEL_MODE_VALUE
};
typedef uint8_t lv_colorwheel_mode_t;

/*Data of color picker*/
typedef struct {
    lv_obj_t obj;
    lv_color_hsv_t hsv;
    struct {
        lv_point_t pos;
        uint8_t recolor     : 1;
    } knob;
    uint32_t last_click_time;
    uint32_t last_change_time;
    lv_point_t last_press_point;
    lv_colorwheel_mode_t mode  : 2;
    uint8_t mode_fixed            : 1;
} lv_colorwheel_t;

extern const lv_obj_class_t lv_colorwheel_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a color picker object with disc shape
 * @param parent pointer to an object, it will be the parent of the new color picker
 * @param knob_recolor true: set the knob's color to the current color
 * @return pointer to the created color picker
 */
lv_obj_t * lv_colorwheel_create(lv_obj_t * parent, bool knob_recolor);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the current hsv of a color wheel.
 * @param colorwheel pointer to color wheel object
 * @param color current selected hsv
 * @return true if changed, otherwise false
 */
bool lv_colorwheel_set_hsv(lv_obj_t * obj, lv_color_hsv_t hsv);

/**
 * Set the current color of a color wheel.
 * @param colorwheel pointer to color wheel object
 * @param color current selected color
 * @return true if changed, otherwise false
 */
bool lv_colorwheel_set_rgb(lv_obj_t * obj, lv_color_t color);

/**
 * Set the current color mode.
 * @param colorwheel pointer to color wheel object
 * @param mode color mode (hue/sat/val)
 */
void lv_colorwheel_set_mode(lv_obj_t * obj, lv_colorwheel_mode_t mode);

/**
 * Set if the color mode is changed on long press on center
 * @param colorwheel pointer to color wheel object
 * @param fixed color mode cannot be changed on long press
 */
void lv_colorwheel_set_mode_fixed(lv_obj_t * obj, bool fixed);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current selected hsv of a color wheel.
 * @param colorwheel pointer to color wheel object
 * @return current selected hsv
 */
lv_color_hsv_t lv_colorwheel_get_hsv(lv_obj_t * obj);

/**
 * Get the current selected color of a color wheel.
 * @param colorwheel pointer to color wheel object
 * @return color current selected color
 */
lv_color_t lv_colorwheel_get_rgb(lv_obj_t * obj);

/**
 * Get the current color mode.
 * @param colorwheel pointer to color wheel object
 * @return color mode (hue/sat/val)
 */
lv_colorwheel_mode_t lv_colorwheel_get_color_mode(lv_obj_t * obj);

/**
 * Get if the color mode is changed on long press on center
 * @param colorwheel pointer to color wheel object
 * @return mode cannot be changed on long press
 */
bool lv_colorwheel_get_color_mode_fixed(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_COLORWHEEL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_COLORWHEEL_H*/
