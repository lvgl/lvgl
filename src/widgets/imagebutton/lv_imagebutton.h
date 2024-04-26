/**
 * @file lv_imagebutton.h
 *
 */

#ifndef LV_IMAGEBUTTON_H
#define LV_IMAGEBUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_IMAGEBUTTON != 0

/*********************
 *      DEFINES
 *********************/
typedef enum {
    LV_IMAGEBUTTON_STATE_RELEASED,
    LV_IMAGEBUTTON_STATE_PRESSED,
    LV_IMAGEBUTTON_STATE_DISABLED,
    LV_IMAGEBUTTON_STATE_CHECKED_RELEASED,
    LV_IMAGEBUTTON_STATE_CHECKED_PRESSED,
    LV_IMAGEBUTTON_STATE_CHECKED_DISABLED,
    LV_IMAGEBUTTON_STATE_NUM,
} lv_imagebutton_state_t;

/**********************
 *      TYPEDEFS
 **********************/
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_imagebutton_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an image button object
 * @param parent pointer to an object, it will be the parent of the new image button
 * @return pointer to the created image button
 */
lv_obj_t * lv_imagebutton_create(lv_obj_t * parent);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set images for a state of the image button
 * @param imagebutton   pointer to an image button object
 * @param state         for which state set the new image
 * @param src_left      pointer to an image source for the left side of the button (a C array or path to
 * a file)
 * @param src_mid       pointer to an image source for the middle of the button (ideally 1px wide) (a C
 * array or path to a file)
 * @param src_right     pointer to an image source for the right side of the button (a C array or path
 * to a file)
 */
void lv_imagebutton_set_src(lv_obj_t * imagebutton, lv_imagebutton_state_t state, const void * src_left,
                            const void * src_mid,
                            const void * src_right);

/**
 * Use this function instead of `lv_obj_add/remove_state` to set a state manually
 * @param imagebutton   pointer to an image button object
 * @param state         the new state
 */
void lv_imagebutton_set_state(lv_obj_t * imagebutton, lv_imagebutton_state_t state);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the left image in a given state
 * @param imagebutton   pointer to an image button object
 * @param state         the state where to get the image (from `lv_button_state_t`) `
 * @return              pointer to the left image source (a C array or path to a file)
 */
const void * lv_imagebutton_get_src_left(lv_obj_t * imagebutton, lv_imagebutton_state_t state);

/**
 * Get the middle image in a given state
 * @param imagebutton   pointer to an image button object
 * @param state         the state where to get the image (from `lv_button_state_t`) `
 * @return              pointer to the middle image source (a C array or path to a file)
 */
const void * lv_imagebutton_get_src_middle(lv_obj_t * imagebutton, lv_imagebutton_state_t state);

/**
 * Get the right image in a given state
 * @param imagebutton   pointer to an image button object
 * @param state         the state where to get the image (from `lv_button_state_t`) `
 * @return              pointer to the left image source (a C array or path to a file)
 */
const void * lv_imagebutton_get_src_right(lv_obj_t * imagebutton, lv_imagebutton_state_t state);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_IMAGEBUTTON*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGEBUTTON_H*/
