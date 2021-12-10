/**
 * @file lv_animbtn.h
 *
 */

#ifndef LV_ANIMBTN_H
#define LV_ANIMBTN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_RLOTTIE != 0 && LV_USE_ANIMBTN != 0

/*********************
 *      DEFINES
 *********************/
typedef enum {
    LV_ANIMBTN_STATE_RELEASED,
    LV_ANIMBTN_STATE_PRESSED,
    LV_ANIMBTN_STATE_DISABLED,
    LV_ANIMBTN_STATE_CHECKED_RELEASED,
    LV_ANIMBTN_STATE_CHECKED_PRESSED,
    LV_ANIMBTN_STATE_CHECKED_DISABLED,
    _LV_ANIMBTN_STATE_NUM,
} lv_animbtn_state_t;

/**********************
 *      TYPEDEFS
 **********************/
/*State status for anim button*/
typedef struct {
    size_t first_frame;
    size_t last_frame;
    size_t control; /* A lv_rlottie_ctrl_t instance and only checking Forward/Backward and Loop here */
} lv_animbtn_state_desc_t;

/*Data of anim button*/
typedef struct {
    lv_obj_t obj;
    lv_animbtn_state_desc_t state_desc[_LV_ANIMBTN_STATE_NUM];
    lv_obj_t * lottie;
    lv_animbtn_state_t prev_state;
} lv_animbtn_t;

extern const lv_obj_class_t lv_animbtn_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an animated button object
 * @param parent pointer to an object, it will be the parent of the new animation button
 * @param anim  pointer to a lv_rlottie object used as backend of this button
 * @return pointer to the created anim button
 */
lv_obj_t * lv_animbtn_create(lv_obj_t * parent, lv_obj_t * anim);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set animation for a state of the animation button
 * @param animbtn pointer to an animation button object
 * @param state for which state set the new animation
 * @param desc description of what to do when the button is in this state
 *
 * In a specific state, the button can either play a short sequence once or loop, forward or backward or go to a specific frame and pause from there.
 */
void lv_animbtn_set_state_desc(lv_obj_t * animbtn, lv_animbtn_state_t state, lv_animbtn_state_desc_t desc);


/**
 * Use this function instead of `lv_obj_add/clear_state` to set a state manually
 * @param animbtn pointer to an animation button object
 * @param state  the new state
 */
void lv_animbtn_set_state(lv_obj_t * animbtn, lv_animbtn_state_t state);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get animation for a state of the animation button
 * @param animbtn pointer to an animation button object
 * @param state the state where to get the animation (from `lv_btn_state_t`) `
 * @return pointer to the description for this state
 */
const lv_animbtn_state_desc_t * lv_animbtn_get_state_desc(lv_obj_t * animbtn, lv_animbtn_state_t state);


/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ANIMBTN*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ANIMBTN_H*/
