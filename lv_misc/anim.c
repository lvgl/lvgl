/**
 * @file anim.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include "anim.h"
#include "misc/os/ptask.h"
#include "hal/systick/systick.h"


/*********************
 *      DEFINES
 *********************/
#define ANIM_PATH_LENGTH		129	/*Elements in a path array*/
#define ANIM_PATH_START			64  /*In path array a value which corresponds to the start position*/
#define ANIM_PATH_END			192 /* ... to the end position. Not required, just for clearance.*/
#define ANIM_PATH_NORM_SHIFT	7 	/*ANIM_PATH_START - ANIM_PATH_END. Must be 2^N. The exponent goes here. */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_task (void);
static void anim_ready_handler(anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/
static ll_dsc_t anim_ll;
static uint32_t last_task_run;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init. the animation module
 */
void anim_init(void)
{
	ll_init(&anim_ll, sizeof(anim_t));
	last_task_run = systick_get();
	ptask_create(anim_task, LV_REFR_PERIOD, PTASK_PRIO_MID);
}

/**
 * Create an animation
 * @param anim_p an initialized 'anim_t' variable. Not required after call.
 */
void anim_create(anim_t * anim_p)
{
	/*Add the new animation to the animation linked list*/
	anim_t * new_anim = ll_ins_head(&anim_ll);
	dm_assert(new_anim);

	/*Initialize the animation descriptor*/
	anim_p->playback_now = 0;
	memcpy(new_anim, anim_p, sizeof(anim_t));

	/*Set the start value*/
	new_anim->fp(new_anim->var, new_anim->start);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Periodically handle the animations.
 */
static void anim_task (void)
{
	uint32_t elaps;
	elaps = systick_elaps(last_task_run);

	anim_t * a;
	anim_t * a_next;
	a = ll_get_head(&anim_ll);
	while(a != NULL) {
		/*'a' might be deleted, so get the next object while 'a' is valid*/
		a_next = ll_get_next(&anim_ll, a);

		a->act_time += elaps;
		if(a->act_time >= 0) {
			if(a->act_time > a->time) a->act_time = a->time;

			/* Get the index of the path array based on the elapsed time*/
			uint8_t path_i;
			path_i = a->act_time * (ANIM_PATH_LENGTH - 1) / a->time;

			/* Get the new value which will be proportional to the current element of 'path_p'
			 * and the 'start' and 'end' values*/
			int32_t new_val;
			new_val =  (int32_t)(a->path_p[path_i] - ANIM_PATH_START) * (a->end - a->start);
			new_val = new_val >> ANIM_PATH_NORM_SHIFT;
			new_val += a->start;

			a->fp(a->var, new_val);	/*Apply the calculated value*/

			/*If the time is elapsed the animation is ready*/
			if(a->act_time >= a->time) {
				anim_ready_handler(a);
			}
		}

		a = a_next;
	}

	last_task_run = systick_get();
}

/**
 * Called when an animation is ready to do the necessary thinks
 * e.g. repeat, play back, delete etc.
 * @param a pointer to an animation descriptor
 * */
static void anim_ready_handler(anim_t * a)
{
	/*Delete the animation if
	 * - no repeat and no play back (simple one shot animation)
	 * - no repeat, play back is enabled and play back is ready */
	if((a->repeat == 0 && a->playback == 0) ||
	   (a->repeat == 0 && a->playback == 1 && a->playback_now == 1)) {
		void (*cb) (void *) = a->end_cb;
		void * p = a->var;
		ll_rem(&anim_ll, a);
		dm_free(a);

		/*Call the callback function at the end*/
		if(cb != NULL) cb(p);
	}
	/*If the animation is not deleted then restart it*/
	else {
		a->act_time = 0;	/*Restart the animation*/
		/*Swap the start and end values in play back mode*/
		if(a->playback != 0) {
			/*Toggle the play back state*/
			a->playback_now = a->playback_now == 0 ? 1: 0;
			/*Swap the start and end values*/
			int32_t tmp;
			tmp = a->start;
			a->start = a->end;
			a->end = tmp;
		}
	}
}
