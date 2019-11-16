/**
 * @file lv_bar.h
 *
 */

#ifndef LV_BAR_H
#define LV_BAR_H

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

#if LV_USE_BAR != 0

#include "../lv_core/lv_obj.h"
#include "../lv_misc/lv_anim.h"
#include "lv_cont.h"
#include "lv_btn.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
	LV_BAR_TYPE_NORMAL,
	LV_BAR_TYPE_SYM,
	LV_BAR_TYPE_CUSTOM
};
typedef uint8_t lv_bar_type_t;

#if LV_USE_ANIMATION
typedef struct {
    lv_obj_t * bar;
	lv_anim_value_t anim_start;
    lv_anim_value_t anim_end;
    lv_anim_value_t anim_val;
	uint8_t is_animating : 1;
} lv_bar_anim_t;
#endif

/** Data of bar*/
typedef struct
{
    /*No inherited ext, derived from the base object */

    /*New data for this type */
    int16_t cur_value; /*Current value of the bar*/
    int16_t min_value; /*Minimum value of the bar*/
    int16_t max_value; /*Maximum value of the bar*/
    int16_t start_value; /*Start value of the bar*/
    lv_area_t indic_area;   /*Save the indicator area. MIght be used by derived types*/
#if LV_USE_ANIMATION
    lv_anim_value_t anim_time;
    lv_bar_anim_t cur_value_anim;
	lv_bar_anim_t start_value_anim;
#endif
	uint8_t type : 2;			/*Type of bar*/
    const lv_style_t * style_indic; /*Style of the indicator*/
} lv_bar_ext_t;

/** Bar styles. */
enum {
    LV_BAR_STYLE_BG, /** Bar background style. */
    LV_BAR_STYLE_INDIC, /** Bar fill area style. */
};
typedef uint8_t lv_bar_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a bar objects
 * @param par pointer to an object, it will be the parent of the new bar
 * @param copy pointer to a bar object, if not NULL then the new object will be copied from it
 * @return pointer to the created bar
 */
lv_obj_t * lv_bar_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the bar
 * @param bar pointer to a bar object
 * @param value new value
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_bar_set_value(lv_obj_t * bar, int16_t value, lv_anim_enable_t anim);

/**
 * Set a new start value on the bar
 * @param bar pointer to a bar object
 * @param value new start value
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediatelly
 */
void lv_bar_set_start_value(lv_obj_t * bar, int16_t start_value, lv_anim_enable_t anim);

/**
 * Set minimum and the maximum values of a bar
 * @param bar pointer to the bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_bar_set_range(lv_obj_t * bar, int16_t min, int16_t max);

/**
 * Set the type of bar.
 * @param bar pointer to bar object
 * @param type bar type
 */
void lv_bar_set_type(lv_obj_t * bar, lv_bar_type_t type);

/**
 * Make the bar symmetric to zero. The indicator will grow from zero instead of the minimum
 * position.
 * @param bar pointer to a bar object
 * @param en true: enable disable symmetric behavior; false: disable
 */
static inline void lv_bar_set_sym(lv_obj_t * bar, bool en)
{
	if(en)
		lv_bar_set_type(bar, LV_BAR_TYPE_SYM);
	else
		lv_bar_set_type(bar, LV_BAR_TYPE_NORMAL);
}

/**
 * Set the animation time of the bar
 * @param bar pointer to a bar object
 * @param anim_time the animation time in milliseconds.
 */
void lv_bar_set_anim_time(lv_obj_t * bar, uint16_t anim_time);

/**
 * Set a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_bar_set_style(lv_obj_t * bar, lv_bar_style_t type, const lv_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a bar
 * @param bar pointer to a bar object
 * @return the value of the bar
 */
int16_t lv_bar_get_value(const lv_obj_t * bar);

/**
 * Get the start value of a bar
 * @param bar pointer to a bar object
 * @return the start value of the bar
 */
int16_t lv_bar_get_start_value(const lv_obj_t * bar);

/**
 * Get the minimum value of a bar
 * @param bar pointer to a bar object
 * @return the minimum value of the bar
 */
int16_t lv_bar_get_min_value(const lv_obj_t * bar);

/**
 * Get the maximum value of a bar
 * @param bar pointer to a bar object
 * @return the maximum value of the bar
 */
int16_t lv_bar_get_max_value(const lv_obj_t * bar);

/**
 * Get the type of bar.
 * @param bar pointer to bar object
 * @return bar type
 */
lv_bar_type_t lv_bar_get_type(lv_obj_t * bar);

/**
 * Get whether the bar is symmetric or not.
 * @param bar pointer to a bar object
 * @return true: symmetric is enabled; false: disable
 */
static inline bool lv_bar_get_sym(lv_obj_t * bar) {
	return lv_bar_get_type(bar) == LV_BAR_TYPE_SYM;
}

/**
 * Get the animation time of the bar
 * @param bar pointer to a bar object
 * @return the animation time in milliseconds.
 */
uint16_t lv_bar_get_anim_time(lv_obj_t * bar);

/**
 * Get a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_bar_get_style(const lv_obj_t * bar, lv_bar_style_t type);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BAR*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BAR_H*/
