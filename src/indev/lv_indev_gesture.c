/******************************************************************
 * @file lv_indev_gesture.c
 *
 * Recognize gestures that consist of multiple touch events
 *
 * Copyright (c) 2024 EDGEMTech Ltd
 *
 * Author EDGEMTech Ltd. (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/

/********************
 *      INCLUDES
 ********************/

#include "lv_indev_private.h"
#include "../misc/lv_event_private.h"

#if LV_USE_GESTURE_RECOGNITION

#include <math.h>
#include "lv_indev_gesture.h"
#include "lv_indev_gesture_private.h"

/********************
 *      DEFINES
 ********************/

#define LV_GESTURE_PINCH_DOWN_THRESHOLD 0.75f /* Default value - start sending events when reached */
#define LV_GESTURE_PINCH_UP_THRESHOLD 1.5f /* Default value - start sending events when reached */
#define LV_GESTURE_PINCH_MAX_INITIAL_SCALE 2.5f /* Default value */
#define LV_GESTURE_ROTATION_ANGLE_RAD_THRESHOLD 0.2f /* Default value - start sending events when reached */


/********************
 *     TYPEDEFS
 ********************/

/********************
 * STATIC PROTOTYPES
 ********************/

static lv_indev_gesture_t * init_gesture_info(void);
static lv_indev_gesture_motion_t * get_motion(uint8_t id, lv_indev_gesture_t * info);
static int8_t get_motion_idx(uint8_t id, lv_indev_gesture_t * info);
static void process_touch_event(lv_indev_touch_data_t * touch, lv_indev_gesture_t * info);
static void gesture_update_center_point(lv_indev_gesture_t * gesture, int touch_points_nb);
static void gesture_calculate_factors(lv_indev_gesture_t * gesture, int touch_points_nb);
static void reset_recognizer(lv_indev_gesture_recognizer_t * recognizer);
static lv_indev_gesture_recognizer_t * lv_indev_get_gesture_recognizer(lv_event_t * gesture_event,
                                                                       lv_indev_gesture_type_t type);
static lv_dir_t calculate_swipe_dir(lv_indev_gesture_recognizer_t * recognizer);
static lv_indev_gesture_type_t get_first_recognized_or_ended_gesture(lv_indev_t * indev);

/********************
 * STATIC VARIABLES
 ********************/

/********************
 *      MACROS
 ********************/
#define SQUARE(x)              ((x) * (x))
#define SQUARE_SUM(x, y)       (SQUARE(x) + SQUARE(y))

/********************
 * GLOBAL FUNCTIONS
 ********************/

void lv_indev_set_pinch_up_threshold(lv_indev_t * indev, float threshold)
{
    /* A up threshold MUST always be bigger than 1 */
    LV_ASSERT(threshold > 1.0f);

    lv_indev_gesture_recognizer_t * recognizer = &indev->recognizers[LV_INDEV_GESTURE_PINCH];

    if(recognizer->config == NULL) {
        recognizer->config = lv_malloc_zeroed(sizeof(lv_indev_gesture_configuration_t));
        LV_ASSERT_MALLOC(recognizer->config);
        recognizer->config->pinch_down_threshold = LV_GESTURE_PINCH_DOWN_THRESHOLD;
    }

    recognizer->config->pinch_up_threshold = threshold;
}

void lv_indev_set_pinch_down_threshold(lv_indev_t * indev, float threshold)
{
    /* A down threshold MUST always be smaller than 1 */
    LV_ASSERT(threshold < 1.0f);

    lv_indev_gesture_recognizer_t * recognizer = &indev->recognizers[LV_INDEV_GESTURE_PINCH];

    if(recognizer->config == NULL) {
        recognizer->config = lv_malloc_zeroed(sizeof(lv_indev_gesture_configuration_t));
        LV_ASSERT_MALLOC(recognizer->config);
        recognizer->config->pinch_up_threshold = LV_GESTURE_PINCH_UP_THRESHOLD;
    }

    recognizer->config->pinch_down_threshold = threshold;
}

void lv_indev_set_rotation_rad_threshold(lv_indev_t * indev, float threshold)
{
    /* A rotation threshold MUST always be a positive number */
    LV_ASSERT(threshold > 0.0f);

    lv_indev_gesture_recognizer_t * recognizer = &indev->recognizers[LV_INDEV_GESTURE_ROTATE];

    if(recognizer->config == NULL) {

        recognizer->config = lv_malloc_zeroed(sizeof(lv_indev_gesture_configuration_t));
        LV_ASSERT(recognizer->config != NULL);
        recognizer->config->rotation_angle_rad_threshold = LV_GESTURE_ROTATION_ANGLE_RAD_THRESHOLD;
    }

    recognizer->config->rotation_angle_rad_threshold = threshold;
}

void lv_indev_get_gesture_primary_point(lv_indev_gesture_recognizer_t * recognizer, lv_point_t * point)
{
    if(recognizer->info->motions[0].finger != -1) {
        point->x = recognizer->info->motions[0].point.x;
        point->y = recognizer->info->motions[0].point.y;
        return;
    }

    /* There are currently no active contact points */
    point->x = 0;
    point->y = 0;
}

bool lv_indev_recognizer_is_active(lv_indev_gesture_recognizer_t * recognizer)
{
    if(recognizer->state == LV_INDEV_GESTURE_STATE_ENDED ||
       recognizer->info->finger_cnt == 0) {
        return false;
    }

    return true;
}

float lv_event_get_pinch_scale(lv_event_t * gesture_event)
{
    lv_indev_gesture_recognizer_t * recognizer;

    if((recognizer = lv_indev_get_gesture_recognizer(gesture_event, LV_INDEV_GESTURE_PINCH)) == NULL) {
        return 0.0f;
    }

    return recognizer->scale;
}

float lv_event_get_rotation(lv_event_t * gesture_event)
{
    lv_indev_gesture_recognizer_t * recognizer;

    if((recognizer = lv_indev_get_gesture_recognizer(gesture_event, LV_INDEV_GESTURE_ROTATE)) == NULL) {
        return 0.0f;
    }

    return recognizer->rotation;
}

float lv_event_get_two_fingers_swipe_distance(lv_event_t * gesture_event)
{
    lv_indev_gesture_recognizer_t * recognizer;

    if((recognizer = lv_indev_get_gesture_recognizer(gesture_event, LV_INDEV_GESTURE_TWO_FINGERS_SWIPE)) == NULL) {
        return 0.0f;
    }

    return recognizer->distance;
}

lv_dir_t lv_event_get_two_fingers_swipe_dir(lv_event_t * gesture_event)
{

    lv_indev_gesture_recognizer_t * recognizer;

    if((recognizer = lv_indev_get_gesture_recognizer(gesture_event, LV_INDEV_GESTURE_TWO_FINGERS_SWIPE)) == NULL) {
        return LV_DIR_NONE;
    }

    return recognizer->two_fingers_swipe_dir;
}

void lv_indev_get_gesture_center_point(lv_indev_gesture_recognizer_t * recognizer, lv_point_t * point)
{
    if(lv_indev_recognizer_is_active(recognizer) == false) {
        point->x = 0;
        point->y = 0;
        return;
    }

    point->x = recognizer->info->center.x;
    point->y = recognizer->info->center.y;
}

lv_indev_gesture_state_t lv_event_get_gesture_state(lv_event_t * gesture_event, lv_indev_gesture_type_t type)
{
    lv_indev_gesture_recognizer_t * recognizer;

    if((recognizer = lv_indev_get_gesture_recognizer(gesture_event, type)) == NULL) {
        return LV_INDEV_GESTURE_STATE_NONE;
    }

    return recognizer->state;
}

lv_indev_gesture_type_t lv_event_get_gesture_type(lv_event_t * gesture_event)
{
    lv_indev_t * indev = (lv_indev_t *) gesture_event->param;

    if(indev == NULL) {
        return LV_INDEV_GESTURE_NONE;
    }

    return indev->cur_gesture;
}

void lv_indev_set_gesture_data(lv_indev_data_t * data, lv_indev_gesture_recognizer_t * recognizer,
                               lv_indev_gesture_type_t type)
{
    bool is_active;
    lv_point_t cur_pnt;

    if(recognizer == NULL) return;

    /* If there is a single contact point use its coords,
     * when there are no contact points it's set to 0,0
     *
     * Note: If a gesture was detected, the primary point is overwritten below
     */

    lv_indev_get_gesture_primary_point(recognizer, &cur_pnt);
    data->point.x = cur_pnt.x;
    data->point.y = cur_pnt.y;

    data->gesture_type[type] = LV_INDEV_GESTURE_NONE;
    data->gesture_data[type] = NULL;

    /* The call below returns false if there are no active contact points */
    /* - OR when the gesture has ended, false is considered as a RELEASED state */
    is_active = lv_indev_recognizer_is_active(recognizer);

    if(is_active == false) {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    else {
        data->state = LV_INDEV_STATE_PRESSED;
    }

    switch(recognizer->state) {
        case LV_INDEV_GESTURE_STATE_RECOGNIZED:
            lv_indev_get_gesture_center_point(recognizer, &cur_pnt);
            data->point.x = cur_pnt.x;
            data->point.y = cur_pnt.y;
            data->gesture_type[type] = type;
            data->gesture_data[type] = (void *) recognizer;
            break;

        case LV_INDEV_GESTURE_STATE_ENDED:
            data->gesture_type[type] = type;
            data->gesture_data[type] = (void *) recognizer;
            break;

        default:
            break;
    }
}


void lv_indev_gesture_detect_pinch(lv_indev_gesture_recognizer_t * recognizer, lv_indev_touch_data_t * touches,
                                   uint16_t touch_cnt)
{
    lv_indev_touch_data_t * touch;
    lv_indev_gesture_recognizer_t * r = recognizer;
    uint8_t i;

    if(r->info == NULL) {
        LV_LOG_TRACE("init gesture info");
        r->info = init_gesture_info();
    }

    if(r->config == NULL) {
        LV_LOG_TRACE("init gesture configuration - set defaults");
        r->config = lv_malloc_zeroed(sizeof(lv_indev_gesture_configuration_t));
        LV_ASSERT_MALLOC(r->config);

        r->config->pinch_up_threshold = LV_GESTURE_PINCH_UP_THRESHOLD;
        r->config->pinch_down_threshold = LV_GESTURE_PINCH_DOWN_THRESHOLD;
    }

    /* Process collected touch events */
    for(i = 0; i < touch_cnt; i++) {
        touch = touches;
        process_touch_event(touch, r->info);
        touches++;

        LV_LOG_TRACE("processed touch ev: %d finger id: %d state: %d x: %" LV_PRId32 " y: %" LV_PRId32 " finger_cnt: %d",
                     i, touch->id, touch->state, touch->point.x, touch->point.y, r->info->finger_cnt);
    }

    LV_LOG_TRACE("Current finger count: %d state: %d", r->info->finger_cnt, r->state);

    if(r->info->finger_cnt == 2) {
        switch(r->state) {
            case LV_INDEV_GESTURE_STATE_NONE:

                /* 2 fingers down - potential pinch or swipe */
                reset_recognizer(recognizer);
                gesture_update_center_point(r->info, 2);
                r->state = LV_INDEV_GESTURE_STATE_ONGOING;
                break;
            case LV_INDEV_GESTURE_STATE_ONGOING:
                gesture_calculate_factors(r->info, 2);
                if(r->info->scale > LV_GESTURE_PINCH_MAX_INITIAL_SCALE) {
                    r->state = LV_INDEV_GESTURE_STATE_CANCELED;
                    break;
                }
                if(r->info->scale > r->config->pinch_up_threshold ||
                   r->info->scale < r->config->pinch_down_threshold) {
                    r->state = LV_INDEV_GESTURE_STATE_RECOGNIZED;
                }
                break;
            case LV_INDEV_GESTURE_STATE_RECOGNIZED:
                /* It's an ongoing pinch gesture - update the factors */
                gesture_calculate_factors(r->info, 2);
                LV_ASSERT(r->info != NULL);
                r->scale = r->info->scale;
                r->type = LV_INDEV_GESTURE_PINCH;
                break;
            case LV_INDEV_GESTURE_STATE_ENDED:
            case LV_INDEV_GESTURE_STATE_CANCELED:
                reset_recognizer(r);
                break;
            default:
                LV_ASSERT_MSG(true, "invalid gesture recognizer state");
        }
    }
    else {
        switch(r->state) {
            case LV_INDEV_GESTURE_STATE_RECOGNIZED:
                /* Gesture has ended */
                r->state = LV_INDEV_GESTURE_STATE_ENDED;
                break;
            case LV_INDEV_GESTURE_STATE_ONGOING:
                /* User lifted a finger before reaching threshold */
                r->state = LV_INDEV_GESTURE_STATE_CANCELED;
                break;
            case LV_INDEV_GESTURE_STATE_ENDED:
            case LV_INDEV_GESTURE_STATE_CANCELED:
                reset_recognizer(r);
                break;

            default:
                LV_ASSERT_MSG(true, "invalid gesture recognizer state");
        }
    }
}

void lv_indev_gesture_detect_rotation(lv_indev_gesture_recognizer_t * recognizer, lv_indev_touch_data_t * touches,
                                      uint16_t touch_cnt)
{
    lv_indev_touch_data_t * touch;
    lv_indev_gesture_recognizer_t * r = recognizer;
    uint8_t i;

    if(r->info == NULL) {
        LV_LOG_TRACE("init gesture info");
        r->info = init_gesture_info();
    }

    if(r->config == NULL) {
        LV_LOG_TRACE("init gesture configuration - set defaults");
        r->config = lv_malloc_zeroed(sizeof(lv_indev_gesture_configuration_t));

        LV_ASSERT(r->config != NULL);
    }

    /* Process collected touch events */
    for(i = 0; i < touch_cnt; i++) {

        touch = touches;
        process_touch_event(touch, r->info);
        touches++;

        LV_LOG_TRACE("processed touch ev: %d finger id: %d state: %d x: %" LV_PRId32 " y: %" LV_PRId32 " finger_cnt: %d",
                     i, touch->id, touch->state, touch->point.x, touch->point.y, r->info->finger_cnt);
    }

    LV_LOG_TRACE("Current finger count: %d state: %d", r->info->finger_cnt, r->state);

    if(r->info->finger_cnt == 2) {
        switch(r->state) {
            case LV_INDEV_GESTURE_STATE_NONE:
                /* 2 fingers down - potential rotation or swipe */
                reset_recognizer(recognizer);
                gesture_update_center_point(r->info, 2);
                r->state = LV_INDEV_GESTURE_STATE_ONGOING;
                break;
            case LV_INDEV_GESTURE_STATE_ONGOING:
                /* Update the rotation from the inputs */
                gesture_calculate_factors(r->info, 2);
                if(fabs(r->info->rotation - r->info->p_rotation) > r->config->rotation_angle_rad_threshold) {

                    gesture_update_center_point(r->info, 2);
                    r->state = LV_INDEV_GESTURE_STATE_RECOGNIZED;
                }
                break;
            case LV_INDEV_GESTURE_STATE_RECOGNIZED:
                /* It's a recognized rotation gesture - update the factors */
                gesture_calculate_factors(r->info, 2);
                r->type = LV_INDEV_GESTURE_ROTATE;
                r->rotation = r->info->rotation;
                break;
            case LV_INDEV_GESTURE_STATE_ENDED:
            case LV_INDEV_GESTURE_STATE_CANCELED:
                reset_recognizer(r);
                r->type = LV_INDEV_GESTURE_NONE;
                r->state = LV_INDEV_GESTURE_STATE_CANCELED;
                break;
            default:
                LV_ASSERT_MSG(true, "invalid gesture recognizer state");
        }
    }
    else {
        switch(r->state) {
            case LV_INDEV_GESTURE_STATE_RECOGNIZED:
                /* Gesture has ended */
                r->type = LV_INDEV_GESTURE_ROTATE;
                r->state = LV_INDEV_GESTURE_STATE_ENDED;
                break;
            case LV_INDEV_GESTURE_STATE_ONGOING:
                /* User lifted a finger before reaching threshold */
                reset_recognizer(r);
                break;
            case LV_INDEV_GESTURE_STATE_CANCELED:
            case LV_INDEV_GESTURE_STATE_ENDED:
                reset_recognizer(r);
                break;
            default:
                LV_ASSERT_MSG(true, "invalid gesture recognizer state");
        }
    }
}

void lv_indev_gesture_detect_two_fingers_swipe(lv_indev_gesture_recognizer_t * recognizer,
                                               lv_indev_touch_data_t * touches,
                                               uint16_t touch_cnt)
{
    lv_indev_touch_data_t * touch;
    lv_indev_gesture_recognizer_t * r = recognizer;
    uint8_t i;
    float dist;

    if(r->info == NULL) {
        LV_LOG_TRACE("init gesture info");
        r->info = init_gesture_info();
    }

    if(r->config == NULL) {
        LV_LOG_TRACE("init gesture configuration - set defaults");
        r->config = lv_malloc_zeroed(sizeof(lv_indev_gesture_configuration_t));

        LV_ASSERT(r->config != NULL);
    }

    /* Process collected touch events */
    for(i = 0; i < touch_cnt; i++) {

        touch = touches;
        process_touch_event(touch, r->info);
        touches++;

        LV_LOG_TRACE("processed touch ev: %d finger id: %d state: %d x: %" LV_PRId32 " y: %" LV_PRId32 " finger_cnt: %d",
                     i, touch->id, touch->state, touch->point.x, touch->point.y, r->info->finger_cnt);
    }

    LV_LOG_TRACE("Current finger count: %d state: %d", r->info->finger_cnt, r->state);

    if(r->info->finger_cnt == 2) {

        switch(r->state) {
            case LV_INDEV_GESTURE_STATE_NONE:
                /* 2 fingers down - potential rotation or swipe */
                reset_recognizer(recognizer);
                gesture_update_center_point(r->info, 2);
                r->state = LV_INDEV_GESTURE_STATE_ONGOING;
                break;
            case LV_INDEV_GESTURE_STATE_ONGOING:
                /* The gesture is ongoing, now wait for the distance from the center
                to be higher than the threshold to pass it as recognized */
                gesture_calculate_factors(r->info, 2);
                dist = SQUARE_SUM(r->info->delta_x, r->info->delta_y);
                if(dist > SQUARE(lv_indev_active()->gesture_limit)) {
                    r->state = LV_INDEV_GESTURE_STATE_RECOGNIZED;
                }
                break;
            case LV_INDEV_GESTURE_STATE_RECOGNIZED:
                /* The gesture is now recognized, and will stay recognized
                until a finger is lifted */
                gesture_calculate_factors(r->info, 2);
                r->distance = (float) sqrt(SQUARE_SUM(r->info->delta_x, r->info->delta_y));
                r->two_fingers_swipe_dir = calculate_swipe_dir(r);
                r->type = LV_INDEV_GESTURE_TWO_FINGERS_SWIPE;
                break;
            case LV_INDEV_GESTURE_STATE_ENDED:
            case LV_INDEV_GESTURE_STATE_CANCELED:
                reset_recognizer(r);
                break;
            default:
                LV_ASSERT_MSG(true, "invalid gesture recognizer state");
        }
    }
    else {

        switch(r->state) {
            case LV_INDEV_GESTURE_STATE_RECOGNIZED:
                /* Gesture has ended */
                r->state = LV_INDEV_GESTURE_STATE_ENDED;
                r->type = LV_INDEV_GESTURE_TWO_FINGERS_SWIPE;
                break;
            case LV_INDEV_GESTURE_STATE_ONGOING:
                /* User lifted a finger before reaching threshold */
                reset_recognizer(r);
                r->state = LV_INDEV_GESTURE_STATE_ENDED;
                break;
            case LV_INDEV_GESTURE_STATE_CANCELED:
            case LV_INDEV_GESTURE_STATE_ENDED:
                reset_recognizer(r);
                r->state = LV_INDEV_GESTURE_STATE_NONE;
                break;
            default:
                LV_ASSERT_MSG(true, "invalid gesture recognizer state");
        }
    }
}

void lv_indev_gesture_recognizers_update(lv_indev_t * indev, lv_indev_touch_data_t * touches, uint16_t touch_cnt)
{
    lv_indev_gesture_type_t type;

    /* First check if a recognizer state is RECOGNIZED or ENDED. *
     * In that case, call its recongizer function and reset the other*/
    type = get_first_recognized_or_ended_gesture(indev);
    if(type != LV_INDEV_GESTURE_NONE) {

        for(int i = 0; i < LV_INDEV_GESTURE_CNT; i++) {

            if(indev->recognizers[i].recog_fn != NULL) {

                /* Update all recognizers to let them process input */
                indev->recognizers[i].recog_fn(&indev->recognizers[i], &touches[0], touch_cnt);

                /* Then reset the recognizers which did not repport RECONIZED or ENDED */
                if(((lv_indev_gesture_type_t)i) != type) {
                    reset_recognizer(&indev->recognizers[i]);
                }
            }
        }

    }
    else {

        /* Otherwise call all recognizer functions, and stop as soon as one recognizer *
        * reports the state RECOGNIZED or ENDED */
        for(int i = 0; i < LV_INDEV_GESTURE_CNT; i++) {
            if(indev->recognizers[i].recog_fn != NULL) {
                indev->recognizers[i].recog_fn(&indev->recognizers[i], &touches[0], touch_cnt);

                /* If the new state is RECOGNIZED or ENDED */
                if(indev->recognizers[i].state == LV_INDEV_GESTURE_STATE_RECOGNIZED ||
                   indev->recognizers[i].state == LV_INDEV_GESTURE_STATE_ENDED) {

                    /* Reset the others registered recognizers */
                    for(int j = 0; j < LV_INDEV_GESTURE_CNT; j++) {
                        if(j != i && indev->recognizers[j].recog_fn != NULL) {
                            reset_recognizer(&indev->recognizers[j]);
                        }
                    }
                    break;
                }
            }
        }
    }
}

void lv_indev_gesture_recognizers_set_data(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_indev_gesture_type_t type;
    type = get_first_recognized_or_ended_gesture(indev);

    /* If a gesture is RECOGNIZED or ENDED, set only its data */
    if(type != LV_INDEV_GESTURE_NONE) {
        lv_indev_set_gesture_data(data, &indev->recognizers[type], type);
    }
    else {
        /* Otherwise, set data from all initialized recognizer */
        for(int i = 0; i < LV_INDEV_GESTURE_CNT; i++) {
            if(indev->recognizers[i].recog_fn != NULL) {
                lv_indev_set_gesture_data(data, &indev->recognizers[i], i);
            }
        }
    }
}


/********************
 * STATIC FUNCTIONS
 ********************/

/**
 * Caluclate the direction from the starting center of a two fingers swipe gesture
 * @param recognizer        pointer to the recognizer handling the two fingers
 *                          swipe gesture
 * @return                  the direction of the swipe, from the starting center
 */
static lv_dir_t calculate_swipe_dir(lv_indev_gesture_recognizer_t * recognizer)
{
    float abs_x = LV_ABS(recognizer->info->delta_x);
    float abs_y = LV_ABS(recognizer->info->delta_y);

    if(abs_x > abs_y) {
        return recognizer->info->delta_x > 0 ? LV_DIR_RIGHT : LV_DIR_LEFT;
    }
    else {
        return recognizer->info->delta_y > 0 ? LV_DIR_BOTTOM : LV_DIR_TOP;
    }
}

/**
 * Get the gesture recognizer associated to the event
 * @param gesture_event     an LV_GESTURE_EVENT event
 * @param type              the type of the recognizer we want to get
 * @return                  a pointer to the gesture recognizer that emitted the event
 */
lv_indev_gesture_recognizer_t * lv_indev_get_gesture_recognizer(lv_event_t * gesture_event,
                                                                lv_indev_gesture_type_t type)
{
    lv_indev_t * indev;

    if(gesture_event == NULL || gesture_event->param == NULL) return NULL;

    indev = (lv_indev_t *) gesture_event->param;

    if(indev == NULL || indev->gesture_data[type] == NULL) return NULL;

    return (lv_indev_gesture_recognizer_t *) indev->gesture_data[type];
}

/**
 * Resets a gesture recognizer, motion descriptors are preserved
 * @param recognizer        a pointer to the recognizer to reset
 */
static void reset_recognizer(lv_indev_gesture_recognizer_t * recognizer)
{
    uint8_t finger_cnt;
    lv_indev_gesture_t * info;
    lv_indev_gesture_configuration_t * conf;
    lv_recognizer_func_t recog_fn;

    if(recognizer == NULL) return;

    finger_cnt = recognizer->info->finger_cnt;
    info = recognizer->info;
    conf = recognizer->config;
    recog_fn = recognizer->recog_fn;

    /* Set everything to zero but preserve the motion descriptors,
     * which are located at the start of the lv_indev_gesture_t struct */
    lv_memzero((uint8_t *)info + sizeof(info->motions), sizeof(lv_indev_gesture_t) - sizeof(info->motions));
    lv_memzero(recognizer, sizeof(lv_indev_gesture_recognizer_t));

    recognizer->info = info;
    recognizer->config = conf;
    recognizer->recog_fn = recog_fn;

    recognizer->scale = recognizer->info->scale = 1.0;
    recognizer->info->finger_cnt = finger_cnt;

    recognizer->state = LV_INDEV_GESTURE_STATE_NONE;
    recognizer->type = LV_INDEV_GESTURE_NONE;
}

/**
 * Initializes a motion descriptors used with the recognizer(s)
 * @return a pointer to gesture descriptor
 */
static lv_indev_gesture_t * init_gesture_info(void)
{
    lv_indev_gesture_t * info;
    uint8_t i;

    info = lv_malloc_zeroed(sizeof(lv_indev_gesture_t));
    LV_ASSERT_MALLOC(info);

    info->scale = 1;
    info->rotation = 0.0;

    for(i = 0; i < LV_GESTURE_MAX_POINTS; i++) {
        info->motions[i].finger = -1;
    }

    return info;
}

/**
 * Obtains the contact point motion descriptor with id
 * @param id        the id of the contact point
 * @param info      a pointer to the gesture descriptor that stores the motion of each contact point
 * @return          a pointer to the motion descriptor or NULL if not found
 */
static lv_indev_gesture_motion_t * get_motion(uint8_t id, lv_indev_gesture_t * info)
{
    uint8_t i;

    for(i = 0; i < LV_GESTURE_MAX_POINTS; i++) {
        if(info->motions[i].finger == id) {
            return &info->motions[i];
        }
    }

    return NULL;
}

/**
 * Obtains the index of the contact point motion descriptor
 * @param id        the id of the contact point
 * @param info      a pointer to the gesture descriptor that stores the motion of each contact point
 * @return          the index of the motion descriptor or -1 if not found
 */
static int8_t get_motion_idx(uint8_t id, lv_indev_gesture_t * info)
{
    uint8_t i;

    for(i = 0; i < LV_GESTURE_MAX_POINTS; i++) {
        if(info->motions[i].finger == id) {
            return i;
        }
    }

    return -1;
}

/**
 * Update the motion descriptors of a gesture
 * @param touch     a pointer to a touch data structure
 * @param info      a pointer to a gesture descriptor
 */
static void process_touch_event(lv_indev_touch_data_t * touch, lv_indev_gesture_t * info)
{
    lv_indev_gesture_t * g = info;
    lv_indev_gesture_motion_t * motion;
    int8_t motion_idx;
    int8_t len;

    motion_idx = get_motion_idx(touch->id, g);

    if(motion_idx == -1 && touch->state == LV_INDEV_STATE_PRESSED) {
        if(g->finger_cnt >= LV_GESTURE_MAX_POINTS) {
            /* Skip touch */
            return;
        }

        /* New touch point id */
        motion = &g->motions[g->finger_cnt];
        motion->start_point.x = touch->point.x;
        motion->start_point.y = touch->point.y;
        motion->point.x = touch->point.x;
        motion->point.y = touch->point.y;
        motion->finger = touch->id;
        motion->state = touch->state;

        g->finger_cnt++;
    }
    else if(motion_idx >= 0 && touch->state == LV_INDEV_STATE_RELEASED) {
        if(motion_idx == g->finger_cnt - 1) {

            /* Mark last item as un-used */
            motion = get_motion(touch->id, g);

            if(motion == NULL) {
                LV_LOG_ERROR("Released touch not found (id=%d)", touch->id);
                return;
            }

            motion->finger = -1;
            motion->state = touch->state;
        }
        else {

            /* Move back by one */
            len = (g->finger_cnt - 1) - motion_idx;

            if(len > 0) {
                lv_memmove(g->motions + motion_idx,
                           g->motions + motion_idx + 1,
                           sizeof(lv_indev_gesture_motion_t) * len);
            }

            if(g->finger_cnt > 0) {
                g->motions[g->finger_cnt - 1].finger = -1;
            }

            LV_ASSERT(g->motions[motion_idx + 1].finger == -1);
        }

        if(g->finger_cnt > 0) {
            g->finger_cnt--;
        }
        else {
            g->finger_cnt = 0;
        }
    }
    else if(motion_idx >= 0) {
        motion = get_motion(touch->id, g);

        if(motion == NULL) {
            LV_LOG_ERROR("Active touch missing (id=%d state=%d)", touch->id, touch->state);
            return;
        }

        motion->point.x = touch->point.x;
        motion->point.y = touch->point.y;
        motion->state = touch->state;
    }
    else {
        LV_LOG_TRACE("Ignore extra touch id: %d", touch->id);
    }
}

/**
 * Calculate the center point of a gesture, called when there
 * is a probability for the gesture to occur
 * @param touch             a pointer to a touch data structure
 * @param touch_points_nb   The number of contact point to take into account
 */
static void gesture_update_center_point(lv_indev_gesture_t * gesture, int touch_points_nb)
{
    lv_indev_gesture_motion_t * motion;
    lv_indev_gesture_t * g = gesture;
    int32_t x = 0;
    int32_t y = 0;
    uint8_t i;
    float scale_factor = 0.0f;
    float delta_x[LV_GESTURE_MAX_POINTS] = {0.0f};
    float delta_y[LV_GESTURE_MAX_POINTS] = {0.0f};
    uint8_t touch_cnt = 0;
    x = y = 0;

    g->p_scale = g->scale;
    g->p_delta_x = g->delta_x;
    g->p_delta_y = g->delta_y;
    g->p_rotation = g->rotation;

    for(i = 0; i < touch_points_nb; i++) {
        motion = &g->motions[i];

        if(motion->finger >= 0) {
            x += motion->point.x;
            y += motion->point.y;
            touch_cnt++;
        }
        else {
            break;
        }
    }

    g->center.x = x / touch_cnt;
    g->center.y = y / touch_cnt;

    for(i = 0; i < touch_points_nb; i++) {
        motion = &g->motions[i];
        if(motion->finger >= 0) {
            delta_x[i] = motion->point.x - g->center.x;
            delta_y[i] = motion->point.y - g->center.y;
            scale_factor += (delta_x[i] * delta_x[i]) + (delta_y[i] * delta_y[i]);
        }
    }
    for(i = 0; i < touch_points_nb; i++) {
        motion = &g->motions[i];
        if(motion->finger >= 0) {
            g->scale_factors_x[i] = delta_x[i] / scale_factor;
            g->scale_factors_y[i] = delta_y[i] / scale_factor;
        }
    }
}

/**
 * Calculate the scale, translation and rotation of a gesture, called when
 * the gesture has been recognized
 * @param gesture           a pointer to the gesture descriptor
 * @param touch_points_nb   the number of contact points to take into account
 */
static void gesture_calculate_factors(lv_indev_gesture_t * gesture, int touch_points_nb)
{
    lv_indev_gesture_motion_t * motion;
    lv_indev_gesture_t * g = gesture;
    float center_x = 0;
    float center_y = 0;
    float a = 0;
    float b = 0;
    float d_x;
    float d_y;
    int8_t i;
    int8_t touch_cnt = 0;

    for(i = 0; i < touch_points_nb; i++) {
        motion = &g->motions[i];

        if(motion->finger >= 0) {
            center_x += motion->point.x;
            center_y += motion->point.y;
            touch_cnt++;
        }
        else {
            break;
        }
    }

    center_x = center_x / touch_cnt;
    center_y = center_y / touch_cnt;

    /* translation */
    g->delta_x = g->p_delta_x + (center_x - g->center.x);
    g->delta_y = g->p_delta_y + (center_y - g->center.y);

    /* rotation & scaling */
    for(i = 0; i < touch_points_nb; i++) {
        motion = &g->motions[i];

        if(motion->finger >= 0) {
            d_x = (motion->point.x - center_x);
            d_y = (motion->point.y - center_y);
            a += g->scale_factors_x[i] * d_x + g->scale_factors_y[i] * d_y;
            b += g->scale_factors_x[i] * d_y - g->scale_factors_y[i] * d_x;
        }
    }

    g->rotation = g->p_rotation + atan2f(b, a);
    g->scale = g->p_scale * sqrtf((a * a) + (b * b));
}

/**
 * Get the type of the first gesture which reports either a LV_INDEV_GESTURE_STATE_RECOGNIZED
 * or LV_INDEV_GESTURE_STATE_ENDED state
 * @param indev         pointer to the indev device from which we want to check the gestures states
 * @return              the type of the gesture having the state LV_INDEV_GESTURE_STATE_RECOGNIZED or
 *                      LV_INDEV_GESTURE_STATE_ENDED, if found
 *                      LV_INDEV_GESTURE_NONE otherwise
 */
static lv_indev_gesture_type_t get_first_recognized_or_ended_gesture(lv_indev_t * indev)
{
    for(int i = 0; i < LV_INDEV_GESTURE_CNT; i++) {
        if(indev->recognizers[i].state == LV_INDEV_GESTURE_STATE_RECOGNIZED ||
           indev->recognizers[i].state == LV_INDEV_GESTURE_STATE_ENDED)
            return (lv_indev_gesture_type_t) i;
    }

    return LV_INDEV_GESTURE_NONE;
}


#endif /* LV_USE_GESTURE_RECOGNITION */
