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
#include "../../../widgets/lv_img.h"

#if LV_USE_ANIMBTN != 0

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_ANIMBTN_STATE_RELEASED = 1,
    LV_ANIMBTN_STATE_PRESSED  = 2,
    LV_ANIMBTN_STATE_DISABLED = 3,
    LV_ANIMBTN_STATE_CHECKED_RELEASED = 4,
    LV_ANIMBTN_STATE_CHECKED_PRESSED  = 5,
    LV_ANIMBTN_STATE_CHECKED_DISABLED = 6,
    _LV_ANIMBTN_STATE_NUM = 6,
} lv_animbtn_state_t;


typedef enum {
    LV_ANIMBTN_CTRL_FORWARD  = LV_IMG_CTRL_FORWARD,
    LV_ANIMBTN_CTRL_BACKWARD = LV_IMG_CTRL_BACKWARD,
    LV_ANIMBTN_CTRL_LOOP     = LV_IMG_CTRL_LOOP,
} lv_animbtn_ctrl_t; /* Should match lv_img_ctrl_t */

/*State status for anim button*/
typedef struct {
    lv_frame_index_t  first_frame;
    lv_frame_index_t  last_frame;
    lv_animbtn_ctrl_t control;
} lv_animbtn_state_desc_t;

/*A transition from one state to another*/
typedef struct {
    lv_animbtn_state_desc_t  desc;
    lv_animbtn_state_t from : 4;
    lv_animbtn_state_t to   : 4;
} lv_animbtn_transition_t;

/*Data of anim button*/
typedef struct {
    lv_img_t                  img;
    lv_animbtn_state_desc_t   state_desc[_LV_ANIMBTN_STATE_NUM];
    lv_animbtn_state_t        prev_state;
    lv_animbtn_transition_t * trans_desc;
    uint8_t                   trans_count;
} lv_animbtn_t;

extern const lv_obj_class_t lv_animbtn_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an animated button object
 * @param parent pointer to an object, it will be the parent of the new animation button
 * @return pointer to the created anim button
 */
lv_obj_t * lv_animbtn_create(lv_obj_t * parent);

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
 * Set animation for a transition from one state to another.
 * @param animbtn pointer to an animation button object
 * @param from_state the state to set transition from
 * @param to_state the state to set transition to
 * @param desc description of what to do when the button while transiting from from_state to to_state
 *
 * In a specific state, the button can either play a short sequence, forward or backward or go to a specific frame and pause from there.
 */
void lv_animbtn_set_transition_desc(lv_obj_t * animbtn, lv_animbtn_state_t from_state, lv_animbtn_state_t to_state,
                                    lv_animbtn_state_desc_t desc);

/**
 * Use this function instead of `lv_obj_add/clear_state` to set a state manually
 * @param animbtn           pointer to an animation button object
 * @param state             the new state
 * @param skip_transition   whether to skip the transition animation to the new state
 */
void lv_animbtn_set_state(lv_obj_t * animbtn, lv_animbtn_state_t state, bool skip_transition);

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
