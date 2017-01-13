/**
 * @file anim.h
 *
 */

#ifndef ANIM_H
#define ANIM_H

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum
{
	ANIM_PATH_LIN,
	ANIM_PATH_STEP,
}anim_path_name_t;

typedef uint8_t anim_path_t;

typedef void (*anim_fp_t)(void *, int32_t);
typedef void (*anim_cb_t)(void *);

typedef struct
{
	void * var;						/*Variable to animate*/
	anim_fp_t fp;	/*Animator function*/
	anim_cb_t end_cb;		/*Call it when the animation is ready*/
	anim_path_t * path;			/*An array with the steps of animations*/
	int32_t start;					/*Start value*/
	int32_t end;					/*End value*/
	int16_t time;					/*Animation time in ms*/
	int16_t act_time;				/*Current time in animation. Set to negative to make delay.*/
	uint16_t playback_pause;		/*Wait before play back*/
	uint16_t repeat_pause;			/*Wait before repeat*/
	uint8_t playback :1;			/*When the animation is ready play it back*/
	uint8_t repeat :1;				/*Repeat the animation infinitely*/
	/*Animation system use these - user shouldn't set*/
	uint8_t playback_now :1;		/*Play back is in progress*/
}anim_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init. the animation module
 */
void anim_init(void);

/**
 * Create an animation
 * @param anim_p an initialized 'anim_t' variable. Not required after call.
 */
void anim_create(anim_t * anim_p);

/**
 * Delete an animation for a variable with a given animatior function
 * @param var pointer to variable
 * @param fp a function pointer which is animating 'var',
 *           or NULL to ignore it and delete all animation with 'var
 * @return true: at least 1 animation is deleted, false: no animation is deleted
 */
bool anim_del(void * var, anim_fp_t fp);

/**
 * Calculate the time of an animation with a given speed and the start and end values
 * @param speed speed of animation in unit/sec
 * @param start start value of the animation
 * @param end end value of the animation
 * @return the required time [ms] for the animation with the given parameters
 */
uint16_t anim_speed_to_time(uint16_t speed, int32_t start, int32_t end);

/**
 * Get a predefine animation path
 * @param name name of the path from 'anim_path_name_t'
 * @return pointer to the path array
 */
anim_path_t * anim_get_path(anim_path_name_t name);

/**********************
 *      MACROS
 **********************/

#endif
