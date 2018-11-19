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
#include "../../lv_conf.h"
#endif

#if USE_LV_CPICKER != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of colorpicker*/
typedef struct {
    /*New data for this type */
    uint16_t hue;
    uint8_t saturation;
    uint8_t value;
    struct
    {
        lv_style_t * style;
        uint8_t type;
    }ind;
} lv_cpicker_ext_t;


/*Styles*/
enum {
    LV_CPICKER_STYLE_MAIN,
    LV_CPICKER_STYLE_IND,
};
typedef uint8_t lv_cpicker_style_t;

enum {
    LV_CPICKER_IND_LINE,
    LV_CPICKER_IND_CIRCLE,
};
typedef uint8_t lv_cpicker_ind_type_t;




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

/*======================
 * Add/remove functions
 *=====================*/


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
 * Set the current hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param hue current selected hue
 */
void lv_cpicker_set_hue(lv_obj_t * cpicker, uint16_t hue);

/**
 * Set the current color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param color current selected color
 */
void lv_cpicker_set_color(lv_obj_t * cpicker, lv_color_t color);

/*=====================
 * Getter functions
 *====================*/

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

#endif  /*USE_LV_CPICKER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_CPICKER_H*/
