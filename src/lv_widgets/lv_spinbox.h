/**
 * @file lv_spinbox.h
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
#include "../lv_conf_internal.h"

#if LV_USE_SPINBOX != 0

/*Testing of dependencies*/
#if LV_USE_TEXTAREA == 0
#error "lv_spinbox: lv_ta is required. Enable it in lv_conf.h (LV_USE_TEXTAREA  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "../lv_widgets/lv_textarea.h"

/*********************
 *      DEFINES
 *********************/
#define LV_SPINBOX_MAX_DIGIT_COUNT 10

/**********************
 *      TYPEDEFS
 **********************/

/*Data of spinbox*/
typedef struct {
    lv_textarea_ext_t ta; /*Ext. of ancestor*/
    /*New data for this type */
    int32_t value;
    int32_t range_max;
    int32_t range_min;
    int32_t step;
    uint8_t rollover : 1;   // Set to true for rollover functionality
    uint16_t digit_count : 4;
    uint16_t dec_point_pos : 4; /*if 0, there is no separator and the number is an integer*/
    uint16_t digit_padding_left : 4;
} lv_spinbox_ext_t;

/*Styles*/
enum {
    LV_SPINBOX_PART_BG = LV_TEXTAREA_PART_BG,
    LV_SPINBOX_PART_CURSOR = LV_TEXTAREA_PART_CURSOR,
    _LV_SPINBOX_PART_VIRTUAL_LAST = _LV_TEXTAREA_PART_VIRTUAL_LAST,
    _LV_SPINBOX_PART_REAL_LAST = _LV_TEXTAREA_PART_REAL_LAST,
};
typedef uint8_t lv_spinbox_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a spinbox objects
 * @param par pointer to an object, it will be the parent of the new spinbox
 * @param copy pointer to a spinbox object, if not NULL then the new object will be copied from it
 * @return pointer to the created spinbox
 */
lv_obj_t * lv_spinbox_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set spinbox rollover function
 * @param spinbox pointer to spinbox
 * @param b true or false to enable or disable (default)
 */
void lv_spinbox_set_rollover(lv_obj_t * spinbox, bool b);

/**
 * Set spinbox value
 * @param spinbox pointer to spinbox
 * @param i value to be set
 */
void lv_spinbox_set_value(lv_obj_t * spinbox, int32_t i);

/**
 * Set spinbox digit format (digit count and decimal format)
 * @param spinbox pointer to spinbox
 * @param digit_count number of digit excluding the decimal separator and the sign
 * @param separator_position number of digit before the decimal point. If 0, decimal point is not
 * shown
 */
void lv_spinbox_set_digit_format(lv_obj_t * spinbox, uint8_t digit_count, uint8_t separator_position);

/**
 * Set spinbox step
 * @param spinbox pointer to spinbox
 * @param step steps on increment/decrement
 */
void lv_spinbox_set_step(lv_obj_t * spinbox, uint32_t step);

/**
 * Set spinbox value range
 * @param spinbox pointer to spinbox
 * @param range_min maximum value, inclusive
 * @param range_max minimum value, inclusive
 */
void lv_spinbox_set_range(lv_obj_t * spinbox, int32_t range_min, int32_t range_max);

/**
 * Set spinbox left padding in digits count (added between sign and first digit)
 * @param spinbox pointer to spinbox
 * @param cb Callback function called on value change event
 */
void lv_spinbox_set_padding_left(lv_obj_t * spinbox, uint8_t padding);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get spinbox rollover function status
 * @param spinbox pointer to spinbox
 */
bool lv_spinbox_get_rollover(lv_obj_t * spinbox);

/**
 * Get the spinbox numeral value (user has to convert to float according to its digit format)
 * @param spinbox pointer to spinbox
 * @return value integer value of the spinbox
 */
int32_t lv_spinbox_get_value(lv_obj_t * spinbox);

/*=====================
 * Other functions
 *====================*/

/**
 * Select next lower digit for edition by dividing the step by 10
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_step_next(lv_obj_t * spinbox);

/**
 * Select next higher digit for edition by multiplying the step by 10
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_step_prev(lv_obj_t * spinbox);

/**
 * Increment spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_increment(lv_obj_t * spinbox);

/**
 * Decrement spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_decrement(lv_obj_t * spinbox);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SPINBOX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_SPINBOX_H*/
