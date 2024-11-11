/*******************************************************************
 *
 * @file lv_indev_gesture.h
 *
 * Copyright (c) 2024 EDGEMTech Ltd.
 *
 * Author EDGEMTech Ltd, (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/

#ifndef LV_INDEV_GESTURE_H
#define LV_INDEV_GESTURE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"

#if LV_USE_GESTURE_RECOGNITION

#if LV_USE_FLOAT == 0
#error "LV_USE_FLOAT is required for gesture detection."
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* Opaque types defined in the private header */
struct lv_indev_gesture;
struct lv_indev_gesture_configuration;

typedef struct lv_indev_gesture lv_indev_gesture_t;
typedef struct lv_indev_gesture_configuration lv_indev_gesture_configuration_t;

/* The states of a gesture recognizer */
typedef enum {
    LV_INDEV_GESTURE_STATE_NONE = 0,    /* Beginning & end */
    LV_INDEV_GESTURE_STATE_ONGOING,     /* Set when there is a probability */
    LV_INDEV_GESTURE_STATE_RECOGNIZED,  /* Recognized, the event will contain touch info */
    LV_INDEV_GESTURE_STATE_ENDED,       /* A recognized gesture has ended */
    LV_INDEV_GESTURE_STATE_CANCELED,    /* Canceled - usually a finger is lifted */
} lv_indev_gesture_state_t;

/* Data structures for touch events - used to repsensent a libinput event */
/* Emitted by devices capable of tracking identifiable contacts (type B) */
typedef struct {
    lv_point_t point;                   /* Coordinates of the touch */
    lv_indev_state_t state;             /* The state i.e PRESSED or RELEASED */
    uint8_t id;                         /* Identification/slot of the contact point */
    uint32_t timestamp;                 /* Timestamp in milliseconds */
} lv_indev_touch_data_t;

/* Gesture recognizer */
typedef struct {
    lv_indev_gesture_type_t type;       /* The detected gesture type */
    lv_indev_gesture_state_t state;     /* The gesture state ongoing, recognized */
    lv_indev_gesture_t * info;          /* Information on the motion of each touch point */
    float scale;                        /* Relevant for the pinch gesture */
    float rotation;                     /* Relevant for rotation */
    float distance;                     /* Relevant for swipes */
    float speed;

    lv_indev_gesture_configuration_t * config;

} lv_indev_gesture_recognizer_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/* PINCH Gesture */

/**
 * Detects a pinch gesture
 * @param recognizer        pointer to a gesture recognizer
 * @param touches           pointer to the first element of the collected touch events
 * @param touch_cnt         length of passed touch event array.
 */
void lv_indev_gesture_detect_pinch(lv_indev_gesture_recognizer_t * recognizer, lv_indev_touch_data_t * touches,
                                   uint16_t touch_cnt);


/**
 * Set the threshold for the pinch gesture scale up, when the scale factor of gesture
 * reaches the threshold events get sent
 * @param recognizer        pointer to a gesture recognizer
 * @param touches           pointer to the first element of the collected touch events
 * @param touch_cnt         length of passed touch event array.
 */
void lv_indev_set_pinch_up_threshold(lv_indev_gesture_recognizer_t * recognizer, float threshold);

/**
 * Set the threshold for the pinch gesture scale down, when the scale factor of gesture
 * reaches the threshold events get sent
 * @param recognizer        pointer to a gesture recognizer
 * @param touches           pointer to the first element of the collected touch events
 * @param touch_cnt         length of passed touch event array.
 */
void lv_indev_set_pinch_down_threshold(lv_indev_gesture_recognizer_t * recognizer, float threshold);

/**
 * Obtains the current scale of a pinch gesture
 * @param gesture_event     pointer to a gesture recognizer event
 * @return                  the scale of the current gesture
 */
float lv_event_get_pinch_scale(lv_event_t * gesture_event);

/**
 * Sets the state of the recognizer to a indev data structure,
 * it is usually called from the indev read callback
 * @param data the indev data
 * @param recognizer pointer to a gesture recognizer
 */
void lv_indev_set_gesture_data(lv_indev_data_t * data, lv_indev_gesture_recognizer_t * recognizer);

/**
 * Obtains the center point of a gesture
 * @param gesture_event     pointer to a gesture recognizer event
 * @param point             pointer to a point
 */
void lv_indev_get_gesture_center_point(lv_indev_gesture_recognizer_t * recognizer, lv_point_t * point);

/**
 * Obtains the current state of the gesture recognizer attached to an event
 * @param gesture_event     pointer to a gesture recognizer event
 * @return                  current state of the gesture recognizer
 */
lv_indev_gesture_state_t lv_event_get_gesture_state(lv_event_t * gesture_event);

/**
 * Obtains the coordinates of the current primary point
 * @param recognizer        pointer to a gesture recognizer
 * @param point             pointer to a point
 */
void lv_indev_get_gesture_primary_point(lv_indev_gesture_recognizer_t * recognizer, lv_point_t * point);

/**
 * Allows to determine if there is an are ongoing gesture
 * @param recognizer        pointer to a gesture recognizer
 * @return                  false if there are no contact points, or the gesture has ended - true otherwise
 */
bool lv_indev_recognizer_is_active(lv_indev_gesture_recognizer_t * recognizer);


/**********************
 *      MACROS
 **********************/

#endif /* END LV_USE_RECOGNITION */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* END LV_INDEV_GESTURE_H */
