/**
 * @file lv_templ.h
 *
 */


#ifndef LV_SPINBOX_H
#define LV_SPINBOX_H

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

#if USE_LV_SPINBOX != 0

#include "../lv_core/lv_obj.h"
#include "../lv_objx/lv_ta.h"

/*********************
 *      DEFINES
 *********************/
#define LV_SPINBOX_MAX_DIGIT_COUNT	16

/**********************
 *      TYPEDEFS
 **********************/
/*Data of template*/
typedef struct {
    lv_ta_ext_t ta; /*Ext. of ancestor*/
    /*New data for this type */
    double value;
    int32_t valueDigit;
    int32_t rangeMax;
    int32_t rangeMin;
    int32_t step;
    uint8_t digitCount:4;
    uint8_t decPointPos:4;      /*if 0, there is no separator and the number is an integer*/
    uint8_t digits[1+1+LV_SPINBOX_MAX_DIGIT_COUNT];     /*1 sign, 1 point, 16 num digits*/

} lv_spinbox_ext_t;


/*Styles*/
enum {
    LV_SPINBOX_STYLE_BG,
    LV_SPINBOX_STYLE_SB,
    LV_SPINBOX_STYLE_CURSOR,
};
typedef uint8_t lv_spinbox_style_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a template objects
 * @param par pointer to an object, it will be the parent of the new template
 * @param copy pointer to a template object, if not NULL then the new object will be copied from it
 * @return pointer to the created template
 */
lv_obj_t * lv_spinbox_create(lv_obj_t * par, const lv_obj_t * copy);

void lv_spinbox_step_next(lv_obj_t * spinbox);
void lv_spinbox_step_previous(lv_obj_t * spinbox);


/*======================
 * Add/remove functions
 *=====================*/


/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a template.
 * @param templ pointer to template object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_spinbox_set_style(lv_obj_t * templ, lv_spinbox_style_t type, lv_style_t *style);


void lv_spinbox_set_double(const lv_obj_t * spinbox, double d);
void lv_spinbox_set_int(const lv_obj_t * spinbox, int32_t i);
void lv_spinbox_set_digit_format(const lv_obj_t * spinbox, uint8_t digit_count, uint8_t separator_position);
void lv_spinbox_set_range_int(const lv_obj_t * spinbox, int32_t rangeMin, int32_t rangeMax);
void lv_spinbox_set_range_double(const lv_obj_t * spinbox, double rangeMin, double rangeMax);


/*=====================
 * Getter functions
 *====================*/

/**
 * Get style of a template.
 * @param templ pointer to template object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_spinbox_get_style(const lv_obj_t * templ, lv_spinbox_style_t type);


double lv_spinbox_get_double(const lv_obj_t * spinbox);
int32_t lv_spinbox_get_int(const lv_obj_t * spinbox);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_TEMPL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_SPINBOX_H*/
