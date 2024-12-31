/*******************************************************************
 *
 * @file lv_indev_gesture_private.h
 *
 * Contains declarations and definition that are internal
 * to the gesture detection logic
 *
 * Copyright (c) 2024 EDGEMTech Ltd.
 *
 * Author EDGEMTech Ltd, (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/

#ifndef LV_INDEV_GESTURE_PRIVATE_H
#define LV_INDEV_GESTURE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"

#if LV_USE_GESTURE_RECOGNITION

/*********************
 *      DEFINES
 *********************/

#define LV_GESTURE_MAX_POINTS 2


/**********************
 *      TYPEDEFS
 **********************/

/* Represent the motion of a finger */
struct lv_indev_gesture_motion {
    int8_t finger;                      /* The ID of the tracked finger */
    lv_point_t start_point;             /* The coordinates where the DOWN event occurred */
    lv_point_t point;                   /* The current coordinates */
    lv_indev_state_t state;             /* DEBUG: The state i.e PRESSED or RELEASED */
};

typedef struct lv_indev_gesture_motion lv_indev_gesture_motion_t;

/* General descriptor for a gesture, used by recognizer state machines to track
 * the scale, rotation, and translation NOTE: (this will likely become private) */
struct lv_indev_gesture {

    /* Motion descriptor, stores the coordinates and velocity of a contact point */
    lv_indev_gesture_motion_t motions[LV_GESTURE_MAX_POINTS];

    lv_point_t center;                  /* Center point */
    float scale;                        /* Scale factor & previous scale factor */
    float p_scale;
    float scale_factors_x[LV_GESTURE_MAX_POINTS];   /* Scale factor relative to center for each point */
    float scale_factors_y[LV_GESTURE_MAX_POINTS];

    float delta_x;                      /* Translation & previous translation */
    float delta_y;
    float p_delta_x;
    float p_delta_y;
    float rotation;                     /* Rotation & previous rotation*/
    float p_rotation;
    uint8_t finger_cnt;                 /* Current number of contact points */

};

struct lv_indev_gesture_configuration {

    float pinch_up_threshold;           /* When the gesture reaches the threshold - start sending events */
    float pinch_down_threshold;         /* When the gesture reaches the threshold - start sending events */

};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* END LV_USE_RECOGNITION */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* END LV_INDEV_GESTURE_PRIVATE_H */
