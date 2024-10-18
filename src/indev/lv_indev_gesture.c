/**
 * @file lv_indev_gesture.c
 * @description Recognize gestures that consist of multiple touch events
 *
 * @author EDGEMTech Ltd. erik.tagirov@edgemtech.ch
 */

/********************
 *      INCLUDES
 ********************/

#include <math.h>
#include "lv_indev_gesture.h"

/********************
 *      DEFINES
 ********************/

#define LV_GESTURE_PINCH_DOWN_THRESHOLD 0.85
#define LV_GESTURE_PINCH_UP_THRESHOLD 1.25

/********************
 *     TYPEDEFS
 ********************/

/********************
 * STATIC PROTOTYPES
 ********************/

static lv_indev_gesture_t *init_gesture_info(void);
static lv_indev_gesture_motion_t *get_motion(uint8_t id, lv_indev_gesture_t *info);
static int8_t get_motion_idx(uint8_t id, lv_indev_gesture_t *info);

/* Some of the following functions might become global - it is not decided yet */
/* It might be necessary for having a separate file per recognizer */

static void process_touch_event(lv_indev_touch_data_t *touch, lv_indev_gesture_t *info);
static void gesture_update_center_point(lv_indev_gesture_t *gesture);
static void gesture_calculate_factors(lv_indev_gesture_t *gesture);

/********************
 * STATIC VARIABLES
 ********************/

/********************
 *      MACROS
 ********************/

/********************
 * GLOBAL FUNCTIONS
 ********************/

void lv_indev_gesture_detect_pinch(lv_indev_gesture_pinch_t *gesture, lv_indev_touch_data_t *touches, uint16_t touch_cnt)
{
	lv_indev_touch_data_t *touch;
	lv_indev_gesture_pinch_t *g = gesture;
	uint8_t i;

	if (g->info == NULL) {
		LV_LOG_TRACE("init gesture descriptor");
		g->info = init_gesture_info();
	}

	/* Process collected touch events */
	for (i = 0; i < touch_cnt; i++) {

		touch = touches;
		process_touch_event(touch, g->info);
		touches++;

		LV_LOG_TRACE("processed touch ev: %d finger id: %d state: %d x: %d y: %d", 
				i, touch->id, touch->state, touch->point.x, touch->point.y);
	}

	LV_LOG_TRACE("Current finger count: %d %d", g->info->finger_cnt, g->state);

	if (g->info->finger_cnt == 2) {

		switch(g->state) {
		case LV_INDEV_GESTURE_STATE_NONE:

			/* 2 fingers down */
			g->info->scale = g->scale = 1;
			gesture_update_center_point(g->info);
			g->state = LV_INDEV_GESTURE_STATE_ONGOING;

			LV_LOG_TRACE("potential pinch gesture: %d", g->info->finger_cnt);
			break;

		case LV_INDEV_GESTURE_STATE_ONGOING:
		case LV_INDEV_GESTURE_STATE_RECOGNIZED:

			/* It's an ongoing pinch gesture - update the factors */
			gesture_calculate_factors(g->info);

			if (g->info->scale > LV_GESTURE_PINCH_UP_THRESHOLD || 
				g->info->scale < LV_GESTURE_PINCH_DOWN_THRESHOLD) {

				g->scale = g->info->scale;
				g->state = LV_INDEV_GESTURE_STATE_RECOGNIZED;
			}

			LV_LOG_TRACE("On-going pinch gesture: %d scale: %f rot: %f", 
				g->info->finger_cnt, g->info->scale, g->info->rotation);
			break;
		}

	} else if (g->info->finger_cnt != 2) {

		switch(g->state) {
		case LV_INDEV_GESTURE_STATE_RECOGNIZED:
			/* Gesture has ended */
			LV_LOG_TRACE("Reset pinch gesture: %d", g->info->finger_cnt);
			g->state = LV_INDEV_GESTURE_STATE_NONE;
			break;

		case LV_INDEV_GESTURE_STATE_ONGOING:
			/* User lifted a finger before reaching threshold */
			LV_LOG_TRACE("Reset pinch gesture: %d", g->info->finger_cnt);
			g->state = LV_INDEV_GESTURE_STATE_CANCELED;
			break;

		}

	} else if (g->info->finger_cnt == 0) {

		/* User lifted up all fingers */
		g->state = LV_INDEV_GESTURE_STATE_NONE;
		LV_LOG_TRACE("Reset gesture info");
	}

}

/********************
 * STATIC FUNCTIONS
 ********************/

static lv_indev_gesture_t *init_gesture_info(void)
{
	lv_indev_gesture_t *info;
	uint8_t i;
	
	info = lv_malloc(sizeof(lv_indev_gesture_t));
	LV_ASSERT_NULL(info);

	lv_memset(info, 0, sizeof(lv_indev_gesture_t));
	info->scale = 1;

	for (i = 0; i < LV_GESTURE_MAX_TOUCHES; i++) {
		info->motions[i].finger = -1;
	}

	return info;
}

static lv_indev_gesture_motion_t *get_motion(uint8_t id, lv_indev_gesture_t *info)
{
	uint8_t i;

	for (i = 0; i < LV_GESTURE_MAX_TOUCHES; i++) {
		if (info->motions[i].finger == id) {
			return &info->motions[i];
		}
	}

	return NULL;
	
}

static int8_t get_motion_idx(uint8_t id, lv_indev_gesture_t *info)
{
	uint8_t i;

	for (i = 0; i < LV_GESTURE_MAX_TOUCHES; i++) {
		if (info->motions[i].finger == id) {
			return i;
		}
	}

	return -1;
	
}

static void process_touch_event(lv_indev_touch_data_t *touch, lv_indev_gesture_t *info)
{
	lv_indev_gesture_t *g = info;
	lv_indev_gesture_motion_t *motion;
	int8_t motion_idx;
	uint8_t len;

	motion_idx = get_motion_idx(touch->id, g);

	if (motion_idx == -1 && touch->state == LV_INDEV_STATE_PRESSED)  {

		if (g->finger_cnt == LV_GESTURE_MAX_TOUCHES - 1) {
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

		g->finger_cnt++;

	} else if (motion_idx >= 0 && touch->state == LV_INDEV_STATE_RELEASED) {
		
		if (motion_idx == g->finger_cnt - 1) {

			/* Mark last item as un-used */
			motion = get_motion(touch->id, g);
			motion->finger = -1;

		} else {

			/* Move back by one */
			len = (g->finger_cnt - 1) - motion_idx;
			lv_memmove(g->motions + motion_idx,
				   g->motions + motion_idx + 1, 
				   sizeof(lv_indev_gesture_motion_t) * len);

			g->motions[g->finger_cnt-1].finger = -1;

		}

		g->finger_cnt--;

	} else if (motion_idx >= 0) {

		motion = get_motion(touch->id, g);
		motion->point.x = touch->point.x;
		motion->point.y = touch->point.y;

	} else {
		LV_LOG_TRACE("Ignore extra touch id: %d", touch->id);
	}

}

static void gesture_update_center_point(lv_indev_gesture_t *gesture)
{
	lv_indev_gesture_motion_t *motion;
	lv_indev_gesture_t *g = gesture;
	int32_t x = 0;
	int32_t y = 0;
	uint8_t i;
	float scale_factor = 0.0f;
	float delta_x[LV_GESTURE_MAX_TOUCHES] = {0.0f};
	float delta_y[LV_GESTURE_MAX_TOUCHES] = {0.0f};
	uint8_t touch_cnt = 0;
	x = y = 0;

	g->p_scale = g->scale;
	g->p_delta_x = g->delta_x;
	g->p_delta_y = g->delta_y;
	g->p_rotation = g->rotation;

	for (i = 0; i < LV_GESTURE_MAX_TOUCHES; i++) {
		motion = &g->motions[i];

		if (motion->finger >= 0) {
			x += motion->point.x;
			y += motion->point.y;
			touch_cnt++;

		} else {
			break;
		}
	}

	LV_ASSERT(touch_cnt > 0);
	g->center.x = x / touch_cnt;
	g->center.y = y / touch_cnt;

	for (i = 0; i < LV_GESTURE_MAX_TOUCHES; i++) {

		motion = &g->motions[i];
		if (motion->finger >= 0) {
			delta_x[i] = motion->point.x - g->center.x;
			delta_y[i] = motion->point.y - g->center.y;
			scale_factor += (delta_x[i] * delta_x[i]) + (delta_y[i] * delta_y[i]);
		}
	}
	for (i = 0; i < LV_GESTURE_MAX_TOUCHES; i++) {

		motion = &g->motions[i];
		if (motion->finger >= 0) {
			g->scale_factors_x[i] = delta_x[i] / scale_factor;
			g->scale_factors_y[i] = delta_y[i] / scale_factor;
		}
	}
}

static void gesture_calculate_factors(lv_indev_gesture_t *gesture)
{
	lv_indev_gesture_motion_t *motion;
	lv_indev_gesture_t *g = gesture;
	float center_x = 0;
	float center_y = 0;
	float a = 0;
	float b = 0;
	float d_x;
	float d_y;
	int8_t i;
	int8_t touch_cnt = 0;


	for (i = 0; i < LV_GESTURE_MAX_TOUCHES; i++) {
		motion = &g->motions[i];

		if (motion->finger >= 0) {
			center_x += motion->point.x;
			center_y += motion->point.y;
			touch_cnt++;

		} else {
			break;
		}
	}

	LV_ASSERT(touch_cnt > 0);
	center_x = center_x / touch_cnt;
	center_y = center_y / touch_cnt;

	/* translation */
	g->delta_x = g->p_delta_x + (center_x - g->center.x);
	g->delta_y = g->p_delta_x + (center_y - g->center.y);

	/* rotation & scaling */
	for (i = 0; i < LV_GESTURE_MAX_TOUCHES; i++) {
		motion = &g->motions[i];

		if (motion->finger >= 0) {
			d_x = (motion->point.x - center_x);
			d_y = (motion->point.y - center_y);
			a += g->scale_factors_x[i] * d_x + g->scale_factors_y[i] * d_y;
			b += g->scale_factors_x[i] * d_y + g->scale_factors_y[i] * d_x;
		}
	}

	g->rotation = g->p_rotation + atan2f(b, a);
	g->scale = g->p_scale * sqrtf((a * a) + (b * b));

	g->center.x = center_x;
	g->center.y = center_y;

}
