/**
 * @file lv_pb.h
 * 
 */

#ifndef LV_PB_H
#define LV_PB_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_PB != 0

#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of progress bar*/
typedef struct
{
	lv_rects_t bg;
	lv_rects_t bar;
	lv_labels_t label;
}lv_pbs_t;

/*Built-in styles of progress bar*/
typedef enum
{
	LV_PBS_DEF,
}lv_pbs_builtin_t;

/*Data of progress bar*/
typedef struct
{
	lv_rect_ext_t rect_ext;
	uint16_t act_value;
	uint16_t min_value;
	uint16_t max_value;
	char * format_dp; /*Format string of the label*/
}lv_pb_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* lv_pb_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);
bool lv_pb_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);
void lv_pb_set_value(lv_obj_t * obj_dp, uint16_t value);
void lv_pb_set_min_max_value(lv_obj_t * obj_dp, uint16_t min, uint16_t max);
void lv_pb_set_format(lv_obj_t * obj_dp, const char * format);
uint16_t lv_pb_get_value(lv_obj_t * obj_dp);
lv_pbs_t * lv_pbs_get(lv_pbs_builtin_t style, lv_pbs_t * copy_p);

/**********************
 *      MACROS
 **********************/

#endif

#endif
