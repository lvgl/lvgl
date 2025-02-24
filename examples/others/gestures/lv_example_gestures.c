/*******************************************************************
 *
 * @file lv_example_gestures.c
 *
 * This is a simple example program that demonstrates how to use
 * the gesture recognition API, please refer to lv_indev_gesture.h or the documentation
 * for more details
 *
 * The application starts with a single rectangle that is scaled when a pinch gesture
 * is detected. A single finger moves the rectangle around,
 *
 * Copyright (c) 2024 EDGEMTech Ltd
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_examples.h"

#if LV_USE_GESTURE_RECOGNITION && LV_BUILD_EXAMPLES

/*********************
 *      DEFINES
 *********************/

#define RECT_INIT_WIDTH 300.0
#define RECT_INIT_HEIGHT 300.0
#define RECT_COLOR 0xC1BCFF

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void label_scale(lv_event_t * gesture_event);
static void label_move(lv_event_t * event);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_obj_t * label;
static lv_style_t label_style;
static float label_width;
static float label_height;
static float label_x;
static float label_y;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Entry point it creates the screen, and the label
 * Set event callbacks on the label
 */
void lv_example_gestures(void)
{
    lv_obj_t * root_view;

    label_width = RECT_INIT_WIDTH;
    label_height = RECT_INIT_HEIGHT;
    label_y = label_x = 300;

    root_view = lv_screen_active();

    lv_obj_set_style_bg_color(root_view, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    label = lv_label_create(root_view);
    lv_obj_remove_flag(root_view, LV_OBJ_FLAG_SCROLLABLE);

    lv_label_set_text(label, "Zoom or move");
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);

    lv_style_init(&label_style);
    lv_style_set_bg_color(&label_style, lv_color_hex(RECT_COLOR));
    lv_style_set_bg_opa(&label_style, LV_OPA_COVER);

    lv_style_set_width(&label_style, (int)label_width);
    lv_style_set_height(&label_style, (int)label_height);

    lv_style_set_x(&label_style, (int)label_x);
    lv_style_set_y(&label_style, (int)label_y);

    lv_obj_add_style(label, &label_style, LV_STATE_DEFAULT);

    lv_obj_add_event_cb(label, label_scale, LV_EVENT_GESTURE, label);
    lv_obj_add_event_cb(label, label_move, LV_EVENT_PRESSING, label);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called when a pinch event occurs - scales the label
 * @param gesture_event point to a LV_EVENT_PINCH event
 */
static void label_scale(lv_event_t * gesture_event)
{

    static int initial_w = -1;
    static int initial_h = -1;
    lv_indev_gesture_state_t state;
    lv_point_t center_pnt;
    float scale;

    scale = lv_event_get_pinch_scale(gesture_event);
    state = lv_event_get_gesture_state(gesture_event);

    lv_indev_get_point(lv_indev_active(), &center_pnt);

    if(state == LV_INDEV_GESTURE_STATE_ENDED) {
        /* Pinch gesture has ended - reset the width/height for the next pinch gesture*/
        initial_w = -1;
        initial_h = -1;

        LV_LOG_USER("label end scale: %g, state: %d", scale, state);
        return;
    }

    if(initial_h == -1 || initial_w == -1) {

        LV_ASSERT(state == LV_INDEV_GESTURE_STATE_RECOGNIZED);

        /* Pinch gesture has been recognized - this is the first event in a series of recognized events */
        /* The scaling is applied relative to the original width/height of the rectangle */
        initial_w = (int)label_width;
        initial_h = (int)label_height;

        LV_LOG_USER("label start scale: %g", scale);
    }

    /* The gesture has started or is on-going */

    /* Avoids a situation where the rectangle becomes too small,
     * do not perform the scaling - leave straight away */
    if(scale < 0.4) {
        return;
    }

    label_width = initial_w * scale;
    label_height = initial_h * scale;
    label_x = center_pnt.x - label_width / 2;
    label_y = center_pnt.y - label_height / 2;

    LV_LOG_USER("label scale: %g label x: %g label y: %g w: %g h: %g",
                scale, label_x, label_y, label_width, label_height);

    /* Update position and size */
    lv_style_set_width(&label_style, (int)label_width);
    lv_style_set_height(&label_style, (int)label_height);
    lv_style_set_x(&label_style, (int)label_x);
    lv_style_set_y(&label_style, (int)label_y);

    lv_obj_add_style(label, &label_style, LV_STATE_DEFAULT);
}

/**
 * Called when a LV_EVENT_PRESSING occurs on the rectangle - moves the label
 * @param event pointer to the event
 */
static void label_move(lv_event_t * event)
{
    lv_point_t pnt;
    lv_indev_gesture_state_t state;

    state = lv_event_get_gesture_state(event);
    lv_indev_get_point(lv_indev_active(), &pnt);

    /* Do not move and when a pinch gesture is ongoing */
    if(state == LV_INDEV_GESTURE_STATE_RECOGNIZED) {
        return;
    }

    LV_LOG_USER("label move x: %d, y: %d", pnt.x, pnt.y);

    label_x = pnt.x - label_width / 2;
    label_y = pnt.y - label_height / 2;

    /* Update position */
    lv_style_set_x(&label_style, (int)label_x);
    lv_style_set_y(&label_style, (int)label_y);

    lv_obj_add_style(label, &label_style, LV_STATE_DEFAULT);
}

#endif /* LV_USE_GESTURE_RECOGNITION && LV_USE_FLOAT */
