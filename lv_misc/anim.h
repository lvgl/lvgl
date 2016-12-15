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

typedef struct
{
	void * var;						/*Variable to animate*/
	anim_fp_t fp;	/*Animator function*/
	void (*end_cb) (void *);		/*Call it when the animation is ready*/
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
void anim_init(void);
void anim_create(anim_t * anim_p);
anim_path_t * anim_get_path(anim_path_name_t type);
bool anim_del(void * var, anim_fp_t fp);
uint16_t anim_speed_to_time(uint16_t speed, int32_t start, int32_t end);

/**********************
 *      MACROS
 **********************/

#endif
