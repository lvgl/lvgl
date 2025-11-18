/**
 * @file lv_indev_private.h
 *
 */

#ifndef LV_INDEV_PRIVATE_H
#define LV_INDEV_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_indev.h"
#include "../misc/lv_anim.h"
#include "lv_indev_scroll.h"
#include "lv_indev_gesture.h"

/*********************
 *      DEFINES
 *********************/
#define LV_INDEV_VECT_HIST_SIZE 8

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_indev_t {
    /** Input device type*/
    lv_indev_type_t type;

    /** Function pointer to read input device data.*/
    lv_indev_read_cb_t read_cb;

    lv_indev_state_t state; /**< Current state of the input device.*/
    lv_indev_state_t prev_state; /**< Previous state of the input device.*/
    lv_indev_mode_t mode;

    /*Flags*/
    uint8_t long_pr_sent : 1;
    uint8_t reset_query : 1;
    uint8_t enabled : 1;
    uint8_t wait_until_release : 1;
    uint8_t stop_processing_query : 1;

    uint32_t timestamp;            /**< Timestamp of last event */
    uint32_t pr_timestamp;         /**< Pressed time stamp*/
    uint32_t longpr_rep_timestamp; /**< Long press repeat time stamp*/

    void * driver_data;
    void * user_data;

    /**< Pointer to the assigned display*/
    lv_display_t * disp;

    /**< Timer to periodically read the input device*/
    lv_timer_t * read_timer;

    /**< Number of pixels to slide before actually drag the object*/
    uint8_t scroll_limit;

    /**< Drag throw slow-down in [%]. Greater value means faster slow-down*/
    uint8_t scroll_throw;

    /**< At least this difference should be between two points to evaluate as gesture*/
    uint8_t gesture_min_velocity;

    /**< At least this difference should be to send a gesture*/
    uint8_t gesture_limit;

    /**< Long press time in milliseconds*/
    uint16_t long_press_time;

    /**< Repeated trigger period in long press [ms]*/
    uint16_t long_press_repeat_time;

    /**< Rotary diff count will be multiplied by this value and divided by 256*/
    int32_t rotary_sensitivity;

    struct {
        /*Pointer and button data*/
        lv_point_t act_point; /**< Current point of input device.*/
        lv_point_t last_point; /**< Last point of input device.*/
        lv_point_t last_raw_point; /**< Last point read from read_cb. */
        lv_point_t vect; /**< Difference between `act_point` and `last_point`.*/
        lv_point_t vect_hist[LV_INDEV_VECT_HIST_SIZE];
        uint32_t   vect_hist_timestamp[LV_INDEV_VECT_HIST_SIZE];
        uint8_t    vect_hist_index;
        lv_point_t scroll_sum; /*Count the dragged pixels to check LV_INDEV_DEF_SCROLL_LIMIT*/
        lv_point_t scroll_throw_vect;
        lv_point_t scroll_throw_vect_ori;
        lv_obj_t * act_obj;      /*The object being pressed*/
        lv_obj_t * last_obj;     /*The last object which was pressed*/
        lv_obj_t * scroll_obj;   /*The object being scrolled*/
        lv_obj_t * last_pressed; /*The lastly pressed object*/
        lv_obj_t * last_hovered; /*The lastly hovered object*/
        lv_area_t scroll_area;
        lv_point_t gesture_sum; /*Count the gesture pixels to check LV_INDEV_DEF_GESTURE_LIMIT*/
        int32_t diff;
        /*Short click streaks*/
        uint8_t short_click_streak;
        lv_point_t last_short_click_point;
        uint32_t last_short_click_timestamp;
        /*Flags*/
        uint8_t scroll_dir : 4;
        uint8_t gesture_dir : 4;
        uint8_t gesture_sent : 1;
        uint8_t press_moved : 1;
        uint8_t pressed : 1;
    } pointer;
    struct {
        /*Keypad data*/
        lv_indev_state_t last_state;
        uint32_t last_key;
    } keypad;

    lv_obj_t * cursor;     /**< Cursor for LV_INPUT_TYPE_POINTER*/
    lv_group_t * group;   /**< Keypad destination group*/
    const lv_point_t * btn_points; /**< Array points assigned to the button ()screen will be pressed
                                      here by the buttons*/
    lv_event_list_t event_list;
    lv_anim_t * scroll_throw_anim;

#if LV_USE_GESTURE_RECOGNITION
    lv_indev_gesture_recognizer_t recognizers[LV_INDEV_GESTURE_CNT];
    lv_indev_gesture_type_t cur_gesture;
    void * gesture_data[LV_INDEV_GESTURE_CNT];
    lv_indev_gesture_type_t gesture_type[LV_INDEV_GESTURE_CNT];
#endif
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Find a scrollable object based on the current scroll vector in the indev.
 * In handles scroll propagation to the parent if needed, and scroll directions too.
 * @param indev     pointer to an indev
 * @return          the found scrollable object or NULL if not found.
 */
lv_obj_t * lv_indev_find_scroll_obj(lv_indev_t * indev);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_INDEV_PRIVATE_H*/
