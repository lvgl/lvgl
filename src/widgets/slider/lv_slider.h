/**
 * @file lv_slider.h
 *
 */

#ifndef LV_SLIDER_H
#define LV_SLIDER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../bar/lv_bar.h"

#if LV_USE_SLIDER != 0

/*Testing of dependencies*/
#if LV_USE_BAR == 0
#error "lv_slider: lv_bar is required. Enable it in lv_conf.h (LV_USE_BAR 1)"
#endif


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_SLIDER_MODE_NORMAL = LV_BAR_MODE_NORMAL,
    LV_SLIDER_MODE_SYMMETRICAL = LV_BAR_MODE_SYMMETRICAL,
    LV_SLIDER_MODE_RANGE = LV_BAR_MODE_RANGE
};
typedef uint8_t lv_slider_mode_t;

typedef struct {
    lv_bar_t bar;       /*Add the ancestor's type first*/
    lv_area_t left_knob_area;
    lv_area_t right_knob_area;
    int32_t * value_to_set; /*Which bar value to set*/
    uint8_t dragging : 1;       /*1: the slider is being dragged*/
    uint8_t left_knob_focus : 1; /*1: with encoder now the right knob can be adjusted*/
} lv_slider_t;

extern const lv_obj_class_t lv_slider_class;

/**
 * `type` field in `lv_obj_draw_part_dsc_t` if `class_p = lv_slider_class`
 * Used in `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END`
 */
typedef enum {
    LV_SLIDER_DRAW_PART_KNOB,           /**< The main (right) knob's rectangle*/
    LV_SLIDER_DRAW_PART_KNOB_LEFT,      /**< The left knob's rectangle*/
} lv_slider_draw_part_type_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a slider object
 * @param parent    pointer to an object, it will be the parent of the new slider.
 * @return          pointer to the created slider
 */
lv_obj_t * lv_slider_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the slider
 * @param obj       pointer to a slider object
 * @param value     the new value
 * @param anim      LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
static inline void lv_slider_set_value(lv_obj_t * obj, int32_t value, lv_anim_enable_t anim)
{
    lv_bar_set_value(obj, value, anim);
}

/**
 * Set a new value for the left knob of a slider
 * @param obj       pointer to a slider object
 * @param value     new value
 * @param anim      LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
static inline void lv_slider_set_left_value(lv_obj_t * obj, int32_t value, lv_anim_enable_t anim)
{
    lv_bar_set_start_value(obj, value, anim);
}

/**
 * Set minimum and the maximum values of a bar
 * @param obj       pointer to the slider object
 * @param min       minimum value
 * @param max       maximum value
 */
static inline void lv_slider_set_range(lv_obj_t * obj, int32_t min, int32_t max)
{
    lv_bar_set_range(obj, min, max);
}

/**
 * Set the mode of slider.
 * @param obj       pointer to a slider object
 * @param mode      the mode of the slider. See ::lv_slider_mode_t
 */
static inline void lv_slider_set_mode(lv_obj_t * obj, lv_slider_mode_t mode)
{
    lv_bar_set_mode(obj, (lv_bar_mode_t)mode);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of the main knob of a slider
 * @param obj       pointer to a slider object
 * @return          the value of the main knob of the slider
 */
static inline int32_t lv_slider_get_value(const lv_obj_t * obj)
{
    return lv_bar_get_value(obj);
}

/**
 * Get the value of the left knob of a slider
 * @param obj       pointer to a slider object
 * @return          the value of the left knob of the slider
 */
static inline int32_t lv_slider_get_left_value(const lv_obj_t * obj)
{
    return lv_bar_get_start_value(obj);
}

/**
 * Get the minimum value of a slider
 * @param obj       pointer to a slider object
 * @return          the minimum value of the slider
 */
static inline int32_t lv_slider_get_min_value(const lv_obj_t * obj)
{
    return lv_bar_get_min_value(obj);
}

/**
 * Get the maximum value of a slider
 * @param obj       pointer to a slider object
 * @return          the maximum value of the slider
 */
static inline int32_t lv_slider_get_max_value(const lv_obj_t * obj)
{
    return lv_bar_get_max_value(obj);
}

/**
 * Give the slider is being dragged or not
 * @param obj       pointer to a slider object
 * @return          true: drag in progress false: not dragged
 */
bool lv_slider_is_dragged(const lv_obj_t * obj);

/**
 * Get the mode of the slider.
 * @param obj       pointer to a bar object
 * @return          see ::lv_slider_mode_t
 */
static inline lv_slider_mode_t lv_slider_get_mode(lv_obj_t * slider)
{
    lv_bar_mode_t mode = lv_bar_get_mode(slider);
    if(mode == LV_BAR_MODE_SYMMETRICAL) return LV_SLIDER_MODE_SYMMETRICAL;
    else if(mode == LV_BAR_MODE_RANGE) return LV_SLIDER_MODE_RANGE;
    else return LV_SLIDER_MODE_NORMAL;
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SLIDER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SLIDER_H*/
