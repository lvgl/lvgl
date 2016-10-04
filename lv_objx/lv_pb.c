

/**
 * @file lv_pb.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_PB != 0

#include "lv_pb.h"
#include "../lv_draw/lv_draw.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define LV_PB_TXT_MAX_LENGTH	64
#define LV_PB_DEF_FORMAT	"%d %%"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_pb_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);
static void lv_pbs_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_f_t ancestor_design_fp;
static lv_pbs_t lv_pbs_def;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*----------------- 
 * Create function
 *-----------------*/

/**
 * Create a progress bar objects
 * @param par_dp pointer to an object, it will be the parent of the new progress bar
 * @param copy_dp pointer to a progress bar object, if not NULL then the new object will be copied from it
 * @return pointer to the created progress bar
 */
lv_obj_t* lv_pb_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    /*Create the ancestor basic object*/
    lv_obj_t* new_obj_dp = lv_rect_create(par_dp, copy_dp);
    dm_assert(new_obj_dp);

    /*Allocate the object type specific extended data*/
    lv_pb_ext_t * ext_dp = lv_obj_alloc_ext(new_obj_dp, sizeof(lv_pb_ext_t));
    dm_assert(ext_dp);

    /* Save the rectangle design function.
     * It will be used in the progress bar design function*/
    if(ancestor_design_fp == NULL) {
    	ancestor_design_fp = lv_obj_get_design_f(new_obj_dp);
    }

    /*Init the new progress bar object*/
    if(copy_dp == NULL) {
    	ext_dp->format_dp = dm_alloc(strlen(LV_PB_DEF_FORMAT) + 1);
    	strcpy(ext_dp->format_dp, LV_PB_DEF_FORMAT);
    	ext_dp->min_value = 0;
    	ext_dp->max_value = 100;
    	ext_dp->act_value = 0;

    	lv_label_create(new_obj_dp, NULL);

    	lv_rect_set_layout(new_obj_dp, LV_RECT_LAYOUT_CENTER);
    	lv_obj_set_signal_f(new_obj_dp, lv_pb_signal);
    	lv_obj_set_style(new_obj_dp, lv_pbs_get(LV_PBS_DEF, NULL));
    	lv_obj_set_design_f(new_obj_dp, lv_pb_design);

    	lv_pb_set_value(new_obj_dp, ext_dp->act_value);
    } else {
    	lv_pb_ext_t * ext_copy_dp = lv_obj_get_ext(copy_dp);
    	ext_dp->format_dp = dm_alloc(strlen(ext_copy_dp->format_dp) + 1);
		strcpy(ext_dp->format_dp, ext_copy_dp->format_dp);
		ext_dp->min_value = ext_copy_dp->min_value;
		ext_dp->max_value = ext_copy_dp->max_value;
		ext_dp->act_value = ext_copy_dp->act_value;

    }
    return new_obj_dp;
}

/**
 * Signal function of the progress bar
 * @param obj_dp pointer to a progress bar object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_pb_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_rect_signal(obj_dp, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	lv_pb_ext_t * ext_dp = lv_obj_get_ext(obj_dp);

    	switch(sign) {
    	case LV_SIGNAL_CORD_CHG:
    		lv_pb_set_value(obj_dp, ext_dp->act_value);
    		break;
    	case LV_SIGNAL_CLEANUP:
    		dm_free(ext_dp->format_dp);
    		break;
    		default:
    			break;
    	}
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the progress bar
 * @param obj_dp pointer to a progress bar object
 * @param value new value
 */
void lv_pb_set_value(lv_obj_t * obj_dp, uint16_t value)
{
	lv_pb_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	ext_dp->act_value = value > ext_dp->max_value ? ext_dp->max_value : value;

	char buf[LV_PB_TXT_MAX_LENGTH];
	sprintf(buf, ext_dp->format_dp, ext_dp->act_value);
	lv_label_set_text(lv_obj_get_child(obj_dp, NULL), buf);

	lv_obj_inv(obj_dp);
}

/**
 * Set minimum and the maximum values of a progress bar
 * @param obj_dp pointer to he progress bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_pb_set_min_max_value(lv_obj_t * obj_dp, uint16_t min, uint16_t max)
{
	lv_pb_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	ext_dp->max_value = max;
	ext_dp->max_value = max;
	if(ext_dp->act_value > max) {
		ext_dp->act_value = max;
		lv_pb_set_value(obj_dp, ext_dp->act_value);
	}
	lv_obj_inv(obj_dp);
}

/**
 * Set format string  for the label of the progress bar
 * @param obj_dp pointer to progress bar object
 * @param format a printf-like format string with one number (e.g. "Loading (%d)")
 */
void lv_pb_set_format(lv_obj_t * obj_dp, const char * format)
{
	lv_pb_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	dm_free(ext_dp->format_dp);
	ext_dp->format_dp = dm_alloc(strlen(format) + 1);
	strcpy(ext_dp->format_dp, format);
	lv_pb_set_value(obj_dp, ext_dp->act_value);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a progress bar
 * @param obj_dp pointer to a progress bar object
 * @return the value of the progress bar
 */
uint16_t lv_pb_get_value(lv_obj_t * obj_dp)
{
	lv_pb_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	return ext_dp->act_value;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_pbs_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_pbs_t style
 */
lv_pbs_t * lv_pbs_get(lv_pbs_builtin_t style, lv_pbs_t * copy_p)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_pbs_init();
		style_inited = true;
	}

	lv_pbs_t  *style_p;

	switch(style) {
		case LV_PBS_DEF:
			style_p = &lv_pbs_def;
			break;
		default:
			style_p = &lv_pbs_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_pbs_t));
		else memcpy(copy_p, &lv_pbs_def, sizeof(lv_pbs_t));
	}

	return style_p;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Handle the drawing related tasks of the progress bars
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_pb_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
	if(ancestor_design_fp == NULL) return false;

    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return  ancestor_design_fp(obj_dp, mask_p, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		ancestor_design_fp(obj_dp, mask_p, mode);

		lv_pb_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
		area_t bar_area;
		uint32_t tmp;
		area_cpy(&bar_area, &obj_dp->cords);
		tmp = (uint32_t)ext_dp->act_value * lv_obj_get_width(obj_dp);
		tmp = (uint32_t) tmp / (ext_dp->max_value - ext_dp->min_value);
		bar_area.x2 = bar_area.x1 + (cord_t) tmp;

		lv_pbs_t * style_p = lv_obj_get_style(obj_dp);
		lv_draw_rect(&bar_area, mask_p, &style_p->bar, OPA_COVER);
    }
    return true;
}

/**
 * Initialize the progess bar styles
 */
static void lv_pbs_init(void)
{
	/*Default style*/
	lv_rects_get(LV_RECTS_DEF, &lv_pbs_def.bg);	/*Background*/
	lv_pbs_def.bg.objs.color = COLOR_WHITE;
	lv_pbs_def.bg.gcolor = COLOR_SILVER,
	lv_pbs_def.bg.bcolor = COLOR_BLACK;

	lv_rects_get(LV_RECTS_DEF, &lv_pbs_def.bar);	/*Bar*/
	lv_pbs_def.bar.objs.color = COLOR_LIME;
	lv_pbs_def.bar.gcolor = COLOR_GREEN;
	lv_pbs_def.bar.bcolor = COLOR_BLACK;

	lv_labels_get(LV_LABELS_DEF, &lv_pbs_def.label);
	lv_pbs_def.label.objs.color = COLOR_MAKE(0x20, 0x20, 0x20);

}
#endif
