

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
static bool lv_pb_design(lv_obj_t * pb, const area_t * mask, lv_design_mode_t mode);
static void lv_pbs_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_pbs_t lv_pbs_def;
static lv_design_f_t ancestor_design_fp;

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
 * @param par pointer to an object, it will be the parent of the new progress bar
 * @param copy pointer to a progress bar object, if not NULL then the new object will be copied from it
 * @return pointer to the created progress bar
 */
lv_obj_t * lv_pb_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor basic object*/
    lv_obj_t * new_pb = lv_rect_create(par, copy);
    dm_assert(new_pb);

    /*Allocate the object type specific extended data*/
    lv_pb_ext_t * ext = lv_obj_alloc_ext(new_pb, sizeof(lv_pb_ext_t));
    dm_assert(ext);

    /* Save the rectangle design function.
     * It will be used in the progress bar design function*/
    if(ancestor_design_fp == NULL) {
    	ancestor_design_fp = lv_obj_get_design_f(new_pb);
    }

    /*Init the new progress bar object*/
    if(copy == NULL) {
    	ext->format = dm_alloc(strlen(LV_PB_DEF_FORMAT) + 1);
    	strcpy(ext->format, LV_PB_DEF_FORMAT);
    	ext->min_value = 0;
    	ext->max_value = 100;
    	ext->act_value = 0;

    	lv_label_create(new_pb, NULL);

    	lv_rect_set_layout(new_pb, LV_RECT_LAYOUT_CENTER);
    	lv_obj_set_signal_f(new_pb, lv_pb_signal);
    	lv_obj_set_style(new_pb, lv_pbs_get(LV_PBS_DEF, NULL));
    	lv_obj_set_design_f(new_pb, lv_pb_design);

    	lv_pb_set_value(new_pb, ext->act_value);
    } else {
    	lv_pb_ext_t * ext_copy = lv_obj_get_ext(copy);
    	ext->format = dm_alloc(strlen(ext_copy->format) + 1);
		strcpy(ext->format, ext_copy->format);
		ext->min_value = ext_copy->min_value;
		ext->max_value = ext_copy->max_value;
		ext->act_value = ext_copy->act_value;

    }
    return new_pb;
}

/**
 * Signal function of the progress bar
 * @param pb pointer to a progress bar object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_pb_signal(lv_obj_t * pb, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_rect_signal(pb, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	lv_pb_ext_t * ext = lv_obj_get_ext(pb);

    	switch(sign) {
    	case LV_SIGNAL_CORD_CHG:
    		lv_pb_set_value(pb, ext->act_value);
    		break;
    	case LV_SIGNAL_CLEANUP:
    		dm_free(ext->format);
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
 * @param pb pointer to a progress bar object
 * @param value new value
 */
void lv_pb_set_value(lv_obj_t * pb, uint16_t value)
{
	lv_pb_ext_t * ext = lv_obj_get_ext(pb);
	ext->act_value = value > ext->max_value ? ext->max_value : value;

	char buf[LV_PB_TXT_MAX_LENGTH];
	sprintf(buf, ext->format, ext->act_value);
	lv_label_set_text(lv_obj_get_child(pb, NULL), buf);

	lv_obj_inv(pb);
}

/**
 * Set minimum and the maximum values of a progress bar
 * @param pb pointer to he progress bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_pb_set_min_max_value(lv_obj_t * pb, uint16_t min, uint16_t max)
{
	lv_pb_ext_t * ext = lv_obj_get_ext(pb);
	ext->max_value = max;
	ext->max_value = max;
	if(ext->act_value > max) {
		ext->act_value = max;
		lv_pb_set_value(pb, ext->act_value);
	}
	lv_obj_inv(pb);
}

/**
 * Set format string  for the label of the progress bar
 * @param pb pointer to progress bar object
 * @param format a printf-like format string with one number (e.g. "Loading (%d)")
 */
void lv_pb_set_format(lv_obj_t * pb, const char * format)
{
	lv_pb_ext_t * ext = lv_obj_get_ext(pb);
	dm_free(ext->format);
	ext->format = dm_alloc(strlen(format) + 1);
	strcpy(ext->format, format);
	lv_pb_set_value(pb, ext->act_value);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a progress bar
 * @param pb pointer to a progress bar object
 * @return the value of the progress bar
 */
uint16_t lv_pb_get_value(lv_obj_t * pb)
{
	lv_pb_ext_t * ext = lv_obj_get_ext(pb);
	return ext->act_value;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_pbs_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_pbs_t style
 */
lv_pbs_t * lv_pbs_get(lv_pbs_builtin_t style, lv_pbs_t * copy)
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

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_pbs_t));
		else memcpy(copy, &lv_pbs_def, sizeof(lv_pbs_t));
	}

	return style_p;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Handle the drawing related tasks of the progress bars
 * @param pb pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_pb_design(lv_obj_t * pb, const area_t * mask, lv_design_mode_t mode)
{
	if(ancestor_design_fp == NULL) return false;

    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return  ancestor_design_fp(pb, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		ancestor_design_fp(pb, mask, mode);

		lv_pb_ext_t * ext = lv_obj_get_ext(pb);
		area_t bar_area;
		uint32_t tmp;
		area_cpy(&bar_area, &pb->cords);
		tmp = (uint32_t)ext->act_value * lv_obj_get_width(pb);
		tmp = (uint32_t) tmp / (ext->max_value - ext->min_value);
		bar_area.x2 = bar_area.x1 + (cord_t) tmp;

		lv_pbs_t * style_p = lv_obj_get_style(pb);
		lv_draw_rect(&bar_area, mask, &style_p->bar, OPA_COVER);
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
