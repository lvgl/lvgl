/**
 * @file anim.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stddef.h>
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef uint8_t anim_path_t;

typedef struct
{

}anim_t;

typedef enum
{
	ANIM_NONE = 0,
	ANIM_SHOW,
	ANIM_FLOAT_TOP,
	ANIM_FLOAT_LEFT,
	ANIM_FLOAT_BOTTOM,
	ANIM_FLOAT_RIGHT,
	ANIM_SLIDE_TOP,
	ANIM_SLIDE_LEFT,
	ANIM_SLIDE_BOTTOM,
	ANIM_SLIDE_RIGHT,
	ANIM_FADE,
	ANIM_FADER,
	ANIM_SIZE,
	ANIM_PINGPONG_H,
	ANIM_PINGPONG_V,
}anim_builtin_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void anim_start(lv_obj_t * obj_dp, anim_builtin_t anim, uint32_t t, uint32_t delay)
{

}

anim_t * anim_create(void * p, void * fp, int32_t v1, int32_t v2, anim_path_t * path_p)
{
	return NULL;
}

void ani_set_cb(anim_t * anim_dp, void (*cb)(void *, void *))
{

}

void anim_start_t(anim_t anim_dp, uint32_t time, uint32_t delay)
{

}


void anim_start_v(anim_t anim_dp, uint32_t v, uint32_t delay)
{

}



/**********************
 *   STATIC FUNCTIONS
 **********************/
