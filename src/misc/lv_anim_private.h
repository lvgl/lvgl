/**
 * @file lv_anim_private.h
 *
 */

#ifndef LV_ANIM_PRIVATE_H
#define LV_ANIM_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_anim.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_anim_bezier3_para_t {
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
}; /**< Parameter used when path is custom_bezier*/

/** Describes an animation*/
struct lv_anim_t {
    void * var;                                 /**<Variable to animate*/
    lv_anim_exec_xcb_t exec_cb;                 /**< Function to execute to animate*/
    lv_anim_custom_exec_cb_t custom_exec_cb;/**< Function to execute to animate,
                                                 same purpose as exec_cb but different parameters*/
    lv_anim_start_cb_t start_cb;         /**< Call it when the animation is starts (considering `delay`)*/
    lv_anim_completed_cb_t completed_cb; /**< Call it when the animation is fully completed*/
    lv_anim_deleted_cb_t deleted_cb;     /**< Call it when the animation is deleted*/
    lv_anim_get_value_cb_t get_value_cb; /**< Get the current value in relative mode*/
    void * user_data;                    /**< Custom user data*/
    lv_anim_path_cb_t path_cb;         /**< Describe the path (curve) of animations*/
    int32_t start_value;               /**< Start value*/
    int32_t current_value;             /**< Current value*/
    int32_t end_value;                 /**< End value*/
    int32_t duration;                /**< Animation time in ms*/
    int32_t act_time;            /**< Current time in animation. Set to negative to make delay.*/
    uint32_t playback_delay;     /**< Wait before play back*/
    uint32_t playback_duration;      /**< Duration of playback animation*/
    uint32_t repeat_delay;       /**< Wait before repeat*/
    uint32_t repeat_cnt;         /**< Repeat count for the animation*/
    union lv_anim_path_para_t {
        lv_anim_bezier3_para_t bezier3; /**< Parameter used when path is custom_bezier*/
    } parameter;

    /*Animation system use these - user shouldn't set*/
    uint32_t last_timer_run;
    uint8_t playback_now : 1; /**< Play back is in progress*/
    uint8_t run_round : 1;    /**< Indicates the animation has run in this round*/
    uint8_t start_cb_called : 1;    /**< Indicates that the `start_cb` was already called*/
    uint8_t early_apply  : 1;    /**< 1: Apply start value immediately even is there is `delay`*/
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init the animation module
 */
void lv_anim_core_init(void);

/**
 * Deinit the animation module
 */
void lv_anim_core_deinit(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ANIM_PRIVATE_H*/
