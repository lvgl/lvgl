/**
 * @file lv_indev_gesture.h
 * @author EDGEMTech Ltd, erik.tagirov@edgemtech.ch
 */

#ifndef LV_INDEV_GESTURE_H
#define LV_INDEV_GESTURE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

#define LV_GESTURE_MAX_TOUCHES 4 

/**********************
 *      TYPEDEFS
 **********************/

/* The tates of a gesture recognizer */
typedef enum {
	LV_INDEV_GESTURE_STATE_NONE = 0, 	/* Beginning & end */
	LV_INDEV_GESTURE_STATE_ONGOING,		/* Set when there is a probability */
	LV_INDEV_GESTURE_STATE_RECOGNIZED,	/* Recognized, the event will contain touch info */
	LV_INDEV_GESTURE_STATE_CANCELED,	/* Canceled - usually a finger is lifted */
} lv_indev_gesture_state_t;

/* Data structures for touch events - used to repsensent a libinput event */
/* Emitted by devices capable of tracking identifiable contacts (type B) */
typedef struct {
	lv_point_t point;		/* Coordinates of the touch */
	lv_indev_state_t state;		/* The state i.e PRESSED or RELEASED */
	uint8_t id;			/* Identification/slot of the contact point */
	uint32_t ts;			/* Timestamp in milliseconds */
} lv_indev_touch_data_t;

/* Represent the motion of a finger */
typedef struct {
	int8_t finger; 			/* The ID of the tracked finger */
	lv_point_t start_point;		/* The coordinates where the DOWN event occured */
	lv_point_t point;               /* The current coordinates */
	lv_indev_state_t state;		/* The state i.e PRESSED or RELEASED */
} lv_indev_gesture_motion_t;

/* General descriptor for a gesture, used by recognizer state machines to track 
 * the scale, rotation, and translation */
typedef struct {
	lv_indev_gesture_type_t type;    

	/* Motion descriptor, stores the coordinates and velocity of a contact point */
	lv_indev_gesture_motion_t motions[LV_GESTURE_MAX_TOUCHES];

	lv_point_t center;		 		/* Center point */
	float scale;			 		/* Scale factor & previous scale factor */
	float p_scale;
	float scale_factors_x[LV_GESTURE_MAX_TOUCHES];	/* Scale factor relative to center for each point */
	float scale_factors_y[LV_GESTURE_MAX_TOUCHES];

	float delta_x;			 		/* Translation & previous translation */
	float delta_y;
	float p_delta_x;
	float p_delta_y;
	float rotation;			 		/* Rotation & previous rotation*/
	float p_rotation;
	uint8_t finger_cnt;		 		/* Current number of contact points */

} lv_indev_gesture_t;

/* Gesture recognizers - pinch gesture */
typedef struct {
	lv_indev_gesture_state_t state;
	lv_indev_gesture_t *info;
	float scale;
	float rotation;

} lv_indev_gesture_pinch_t;

/* Gesture recognizer - swipe gesture [TBD] */
typedef struct {
	lv_indev_gesture_state_t state;
	lv_indev_gesture_t *info;
	float distance;
	float speed;

} lv_indev_gesture_swipe_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Detect a pinch gesture
 * @param gesture_state 	pointer to a gesture recognizer
 * @param touches 		pointer to the first element of the collected touch events
 * @param touch_cnt 		length of passed touch event array.
 */
void lv_indev_gesture_detect_pinch(lv_indev_gesture_pinch_t *gesture, lv_indev_touch_data_t *touches, uint16_t touch_cnt);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_INDEV_GESTURE_H*/
