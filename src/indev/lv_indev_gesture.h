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

typedef struct lv_indev_gesture_recognizer lv_indev_gesture_recognizer_t;
typedef struct lv_indev_touch_data lv_indev_touch_data_t;

typedef struct lv_indev_gesture lv_indev_gesture_t;
typedef struct lv_indev_gesture_configuration lv_indev_gesture_configuration_t;

typedef void (*lv_recognizer_func_t)(lv_indev_gesture_recognizer_t *, lv_indev_touch_data_t *, uint16_t);

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
struct lv_indev_touch_data {
    lv_point_t point;                   /* Coordinates of the touch */
    lv_indev_state_t state;             /* The state i.e PRESSED or RELEASED */
    uint8_t id;                         /* Identification/slot of the contact point */
    uint32_t timestamp;                 /* Timestamp in milliseconds */
};

/* Gesture recognizer */
struct lv_indev_gesture_recognizer {
    lv_indev_gesture_type_t type;       /* The detected gesture type */
    lv_indev_gesture_state_t state;     /* The gesture state ongoing, recognized */
    lv_indev_gesture_t * info;          /* Information on the motion of each touch point */
    float scale;                        /* Relevant for the pinch gesture */
    float rotation;                     /* Relevant for rotation */
    float distance;                     /* Relevant for swipes */
    float speed;
    lv_dir_t two_fingers_swipe_dir;     /* Relevant for swipes */

    lv_indev_gesture_configuration_t * config;  /* The recognizer config, containing the gestures
                                                   thresholds */
    lv_recognizer_func_t recog_fn;      /* The recognizer function that this recongnizer must execute */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/* PINCH Gesture */

/**
 * Pinch gesture recognizer function
 * Will update the recognizer data
 * @param recognizer        pointer to a gesture recognizer
 * @param touches           pointer to the first element of the collected touch events
 * @param touch_cnt         length of passed touch event array.
 */
void lv_indev_gesture_detect_pinch(lv_indev_gesture_recognizer_t * recognizer, lv_indev_touch_data_t * touches,
                                   uint16_t touch_cnt);

/**
 * Rotation gesture recognizer function
 * Will update the recognizer data
 * @param recognizer        pointer to a gesture recognizer
 * @param touches           pointer to the first element of the collected touch events
 * @param touch_cnt         length of passed touch event array.
 */
void lv_indev_gesture_detect_rotation(lv_indev_gesture_recognizer_t * recognizer, lv_indev_touch_data_t * touches,
                                      uint16_t touch_cnt);

/**
 * Two finger swipe gesture recognizer function
 * Will update the recognizer data
 * @param recognizer        pointer to a gesture recognizer
 * @param touches           pointer to the first element of the collected touch events
 * @param touch_cnt         length of passed touch event array.
 */
void lv_indev_gesture_detect_two_fingers_swipe(lv_indev_gesture_recognizer_t * recognizer,
                                               lv_indev_touch_data_t * touches,
                                               uint16_t touch_cnt);

/**
 * Set the threshold for the pinch gesture scale up, when the scale factor of gesture
 * reaches the threshold events get sent
 * @param indev             pointer to the indev device containing the pinch recognizer
 * @param threshold         threshold for a pinch up gesture to be recognized
 */
void lv_indev_set_pinch_up_threshold(lv_indev_t * indev, float threshold);

/**
 * Set the threshold for the pinch gesture scale down, when the scale factor of gesture
 * reaches the threshold events get sent
 * @param indev             pointer to the indev device containing the pinch recognizer
 * @param threshold         threshold for a pinch down gesture to be recognized
 */
void lv_indev_set_pinch_down_threshold(lv_indev_t * indev, float threshold);

/**
 * Set the rotation threshold in radian for the rotation gesture
 * @param indev             pointer to the indev device containing the rotation recognizer
 * @param threshold         threshold in radian for a rotation gesture to be recognized
 */
void lv_indev_set_rotation_rad_threshold(lv_indev_t * indev, float threshold);

/**
 * Obtains the current scale of a pinch gesture
 * @param gesture_event     pointer to a gesture event
 * @return                  the scale of the current gesture
 */
float lv_event_get_pinch_scale(lv_event_t * gesture_event);

/**
 * Obtains the current angle in radian of a rotation gesture
 * @param gesture_event     pointer to a gesture event
 * @return                  the rotation angle in radian of the current gesture
 */
float lv_event_get_rotation(lv_event_t * gesture_event);

/**
 * Obtains the current distance in pixels of a two fingers swipe gesture, from the starting center
 * @param gesture_event     pointer to a gesture event
 * @return                  the distance from the center, in pixels, of the current gesture
 */
float lv_event_get_two_fingers_swipe_distance(lv_event_t * gesture_event);

/**
 * Obtains the current direction from the center of a two finger swipe
 * @param gesture_event     pointer to a gesture event
 * @return                  the rotation angle in radian of the current gesture
 */
lv_dir_t lv_event_get_two_fingers_swipe_dir(lv_event_t * gesture_event);

/**
 * Sets the state of the recognizer to a indev data structure,
 * it is usually called from the indev read callback
 * @param data the indev data
 * @param recognizer pointer to a gesture recognizer
 */
void lv_indev_set_gesture_data(lv_indev_data_t * data, lv_indev_gesture_recognizer_t * recognizer,
                               lv_indev_gesture_type_t type);

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
lv_indev_gesture_state_t lv_event_get_gesture_state(lv_event_t * gesture_event, lv_indev_gesture_type_t type);

/**
 * Obtains the current event type of the gesture recognizer attached to an event
 * @param gesture_event     pointer to a gesture recognizer event
 * @return                  current event type of the gesture recognizer
 */
lv_indev_gesture_type_t lv_event_get_gesture_type(lv_event_t * gesture_event);

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

/**
 * Update the recognizers. It execute the recognizers functions and checks for
 * LV_GESTURE_STATE_RECOGNIZED or LV_GESTURE_STATE_ENDED gestures.
 * To be called in the indev read_cb.
 * @param indev             pointer to the indev containing from which the reconizer need an update
 * @param touches           indev touch data array, containing the last touch data from indev
 *                          since the last recognizers update
 * @param touch_cnt         number of indev touch data in touches
 */
void lv_indev_gesture_recognizers_update(lv_indev_t * indev, lv_indev_touch_data_t * touches, uint16_t touch_cnt);

/**
 * Set the lv_indev_data_t struct from the recognizer data.
 * To be called in the indev read_cb.
 */
void lv_indev_gesture_recognizers_set_data(lv_indev_t * indev, lv_indev_data_t * data);


/**********************
 *      MACROS
 **********************/

#endif /* END LV_USE_RECOGNITION */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_INDEV_GESTURE_H */
