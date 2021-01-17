/**
 * @file lv_imgbtn.h
 *
 */

#ifndef LV_IMGBTN_H
#define LV_IMGBTN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_IMGBTN != 0

/*Testing of dependencies*/
#if LV_USE_BTN == 0
#error "lv_imgbtn: lv_btn is required. Enable it in lv_conf.h (LV_USE_BTN 1)"
#endif

#include "../lv_core/lv_obj.h"
#include "lv_btn.h"
#include "../lv_draw/lv_draw_img.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of image button*/
typedef struct {
    lv_btn_ext_t btn; /*Ext. of ancestor*/
    /*New data for this type */
    const void * img_src_mid[_LV_BTN_STATE_LAST];   /*Store center images to each state*/
#if LV_IMGBTN_TILED
    const void * img_src_left[_LV_BTN_STATE_LAST];  /*Store left side images to each state*/
    const void * img_src_right[_LV_BTN_STATE_LAST]; /*Store right side images to each state*/
#endif
    lv_img_cf_t act_cf; /*Color format of the currently active image*/
    uint8_t tiled   : 1; /*1: the middle src will be repeated to fill the user defined width*/
} lv_imgbtn_ext_t;

/*Parts of the image button*/
enum {
    LV_IMGBTN_PART_MAIN = LV_BTN_PART_MAIN,
};
typedef uint8_t lv_imgbtn_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a image button objects
 * @param par pointer to an object, it will be the parent of the new image button
 * @param copy pointer to a image button object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created image button
 */
lv_obj_t * lv_imgbtn_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set images for a state of the image button
 * @param imgbtn pointer to an image button object
 * @param state for which state set the new image (from `lv_btn_state_t`) `
 * @param src pointer to an image source (a C array or path to a file)
 */
void lv_imgbtn_set_src(lv_obj_t * imgbtn, lv_btn_state_t state, const void * src);

#if LV_IMGBTN_TILED
/**
 * Set images for a state of the image button
 * @param imgbtn pointer to an image button object
 * @param state for which state set the new image (from `lv_btn_state_t`) `
 * @param src_left pointer to an image source for the left side of the button (a C array or path to
 * a file)
 * @param src_mid pointer to an image source for the middle of the button (ideally 1px wide) (a C
 * array or path to a file)
 * @param src_right pointer to an image source for the right side of the button (a C array or path
 * to a file)
 */
void lv_imgbtn_set_src_tiled(lv_obj_t * imgbtn, lv_btn_state_t state, const void * src_left, const void * src_mid,
                             const void * src_right);

#endif

/**
 * Set the state of the image button
 * @param imgbtn pointer to an image button object
 * @param state the new state of the button (from lv_btn_state_t enum)
 */
void lv_imgbtn_set_state(lv_obj_t * imgbtn, lv_btn_state_t state);

/**
 * Toggle the state of the image button (ON->OFF, OFF->ON)
 * @param imgbtn pointer to a image button object
 */
void lv_imgbtn_toggle(lv_obj_t * imgbtn);

/**
 * Enable the toggled states. On release the button will change from/to toggled state.
 * @param imgbtn pointer to an image button object
 * @param tgl true: enable toggled states, false: disable
 */
static inline void lv_imgbtn_set_checkable(lv_obj_t * imgbtn, bool tgl)
{
    lv_btn_set_checkable(imgbtn, tgl);
}

/*=====================
 * Getter functions
 *====================*/

#if LV_IMGBTN_TILED == 0
/**
 * Get the images in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to an image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src(lv_obj_t * imgbtn, lv_btn_state_t state);

#else

/**
 * Get the left image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_left(lv_obj_t * imgbtn, lv_btn_state_t state);

/**
 * Get the middle image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the middle image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_middle(lv_obj_t * imgbtn, lv_btn_state_t state);

/**
 * Get the right image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_right(lv_obj_t * imgbtn, lv_btn_state_t state);

#endif
/**
 * Get the current state of the image button
 * @param imgbtn pointer to a image button object
 * @return the state of the button (from lv_btn_state_t enum)
 */
static inline lv_btn_state_t lv_imgbtn_get_state(const lv_obj_t * imgbtn)
{
    return lv_btn_get_state(imgbtn);
}

/**
 * Get the toggle enable attribute of the image button
 * @param imgbtn pointer to a image button object
 * @return true: toggle enabled, false: disabled
 */
static inline bool lv_imgbtn_get_checkable(const lv_obj_t * imgbtn)
{
    return lv_btn_get_checkable(imgbtn);
}

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_IMGBTN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_IMGBTN_H*/
