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
	anim_t * new_anim = ll_ins_head(&anim_ll);
	dm_assert(new_anim);

	memcpy(new_anim, anim_p, sizeof(anim_t));

	new_anim->fp(new_anim->p, new_anim->start);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Periodically handle animations.
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

			a->fp(a->p, new_val);	/*Apply the calculated value*/

			/*Delete the animation if it is ready*/
			if(a->act_time >= a->time) {
				void (*cb) (void *) = a->end_cb;
				void * p = a->p;
				ll_rem(&anim_ll, a);
				dm_free(a);

				/*Call the callback function at the end*/
				if(cb != NULL) cb(p);
			}
		}

		a = a_next;
	}

	last_task_run = systick_get();
}
