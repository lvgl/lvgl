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
#include "../../lv_conf.h"
#if USE_LV_BAR != 0

#include "../lv_core/lv_obj.h"
#include "lv_cont.h"
#include "lv_btn.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of bar*/
typedef struct
{
    /*No inherited ext*/            /*Ext. of ancestor*/
    /*New data for this type */
    int16_t cur_value;              /*Current value of the bar*/
    int16_t min_value;              /*Minimum value of the bar*/
    int16_t max_value;              /*Maximum value of the bar*/
    lv_style_t *style_indic;        /*Style of the indicator*/
}lv_bar_ext_t;

typedef enum {
    LV_BAR_STYLE_BG,
    LV_BAR_STYLE_INDIC,
}lv_bar_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a bar objects
 * @param par pointer to an object, it will be the parent of the new bar
 * @param copy pointer to a bar object, if not NULL then the new object will be copied from it
 * @return pointer to the created bar
 */
lv_obj_t * lv_bar_create(lv_obj_t * par, lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the bar
 * @param bar pointer to a bar object
 * @param value new value
 */
void lv_bar_set_value(lv_obj_t * bar, int16_t value);

/**
 * Set a new value with animation on the bar
 * @param bar pointer to a bar object
 * @param value new value
 * @param anim_time animation time in milliseconds
 */
void lv_bar_set_value_anim(lv_obj_t * bar, int16_t value, uint16_t anim_time);


/**
 * Set minimum and the maximum values of a bar
 * @param bar pointer to the bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_bar_set_range(lv_obj_t * bar, int16_t min, int16_t max);

/**
 * Set a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_bar_set_style(lv_obj_t *bar, lv_bar_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a bar
 * @param bar pointer to a bar object
 * @return the value of the bar
 */
int16_t lv_bar_get_value(lv_obj_t * bar);

/**
 * Get the minimum value of a bar
 * @param bar pointer to a bar object
 * @return the minimum value of the bar
 */
int16_t lv_bar_get_min_value(lv_obj_t * bar);

/**
 * Get the maximum value of a bar
 * @param bar pointer to a bar object
 * @return the maximum value of the bar
 */
int16_t lv_bar_get_max_value(lv_obj_t * bar);


/**
 * Get a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_bar_get_style(lv_obj_t *bar, lv_bar_style_t type);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_BAR*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BAR_H*/
