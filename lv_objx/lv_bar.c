

/**
 * @file lv_bar.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_BAR != 0

#include "lv_bar.h"
#include "../lv_draw/lv_draw.h"
#include "misc/gfx/anim.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_bar_design(lv_obj_t * bar, const area_t * mask, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_f_t ancestor_design_f;

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
 * Create a bar objects
 * @param par pointer to an object, it will be the parent of the new bar
 * @param copy pointer to a bar object, if not NULL then the new object will be copied from it
 * @return pointer to the created bar
 */
lv_obj_t * lv_bar_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor basic object*/
    lv_obj_t * new_bar = lv_obj_create(par, copy);
    dm_assert(new_bar);

    /*Allocate the object type specific extended data*/
    lv_bar_ext_t * ext = lv_obj_alloc_ext(new_bar, sizeof(lv_bar_ext_t));
    dm_assert(ext);
    ext->min_value = 0;
    ext->max_value = 100;
    ext->act_value = 0;
    ext->style_indic = lv_style_get(LV_STYLE_PRETTY_COLOR, NULL);

    /* Save the ancient design function.
     * It will be used in the bar design function*/
    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_bar);

    lv_obj_set_signal_f(new_bar, lv_bar_signal);
    lv_obj_set_design_f(new_bar, lv_bar_design);

    /*Init the new  bar object*/
    if(copy == NULL) {
        lv_obj_set_click(new_bar, false);
    	lv_obj_set_size(new_bar, LV_DPI * 2, LV_DPI / 3);
        lv_obj_set_style(new_bar, lv_style_get(LV_STYLE_PRETTY, NULL));
    	lv_bar_set_value(new_bar, ext->act_value);
    } else {
    	lv_bar_ext_t * ext_copy = lv_obj_get_ext(copy);
		ext->min_value = ext_copy->min_value;
		ext->max_value = ext_copy->max_value;
		ext->act_value = ext_copy->act_value;
        ext->style_indic = ext_copy->style_indic;
        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_bar);

        lv_bar_set_value(new_bar, ext->act_value);
    }
    return new_bar;
}

/**
 * Signal function of the bar
 * @param bar pointer to a bar object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_bar_signal(lv_obj_t * bar, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(bar, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        if(sign == LV_SIGNAL_REFR_EXT_SIZE) {
            lv_style_t * style_indic = lv_bar_get_style_indic(bar);
            if(style_indic->shadow.width > bar->ext_size) bar->ext_size = style_indic->shadow.width;
        }

    }

    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the bar
 * @param bar pointer to a bar object
 * @param value new value
 */
void lv_bar_set_value(lv_obj_t * bar, int16_t value)
{
	lv_bar_ext_t * ext = lv_obj_get_ext(bar);
	ext->act_value = value > ext->max_value ? ext->max_value : value;
    ext->act_value = ext->act_value < ext->min_value ? ext->min_value : ext->act_value;
	lv_obj_inv(bar);
}

/**
 * Set a new value with animation on the bar
 * @param bar pointer to a bar object
 * @param value new value
 * @param anim_time animation time in milliseconds
 */
void lv_bar_set_value_anim(lv_obj_t * bar, int16_t value, uint16_t anim_time)
{

    lv_bar_ext_t * ext = lv_obj_get_ext(bar);
    int16_t new_value;
    new_value = value > ext->max_value ? ext->max_value : value;
    new_value = new_value < ext->min_value ? ext->min_value : new_value;

    anim_t a;
    a.var = bar;
    a.start = ext->act_value;
    a.end = new_value;
    a.fp = (anim_fp_t)lv_bar_set_value;
    a.path = anim_get_path(ANIM_PATH_LIN);
    a.end_cb = NULL;
    a.act_time = 0;
    a.time = anim_time;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;

    anim_create(&a);

}


/**
 * Set minimum and the maximum values of a bar
 * @param bar pointer to he bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_bar_set_range(lv_obj_t * bar, int16_t min, int16_t max)
{
	lv_bar_ext_t * ext = lv_obj_get_ext(bar);
	ext->max_value = max;
	ext->min_value = min;
	if(ext->act_value > max) {
		ext->act_value = max;
		lv_bar_set_value(bar, ext->act_value);
	}
    if(ext->act_value < min) {
        ext->act_value = min;
        lv_bar_set_value(bar, ext->act_value);
    }
	lv_obj_inv(bar);
}

/**
 * Set the style of bar indicator
 * @param bar pointer to a bar object
 * @param style pointer to a style
 */
void lv_bar_set_style_indic(lv_obj_t * bar, lv_style_t * style)
{
    lv_bar_ext_t * ext = lv_obj_get_ext(bar);

    ext->style_indic = style;

    bar->signal_f(bar, LV_SIGNAL_REFR_EXT_SIZE, NULL);

    lv_obj_inv(bar);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a bar
 * @param bar pointer to a bar object
 * @return the value of the bar
 */
int16_t lv_bar_get_value(lv_obj_t * bar)
{
	lv_bar_ext_t * ext = lv_obj_get_ext(bar);
	return ext->act_value;
}

/**
 * Get the minimum value of a bar
 * @param bar pointer to a bar object
 * @return the minimum value of the bar
 */
int16_t lv_bar_get_min_value(lv_obj_t * bar)
{
    lv_bar_ext_t * ext = lv_obj_get_ext(bar);
    return ext->min_value;
}

/**
 * Get the maximum value of a bar
 * @param bar pointer to a bar object
 * @return the maximum value of the bar
 */
int16_t lv_bar_get_max_value(lv_obj_t * bar)
{
    lv_bar_ext_t * ext = lv_obj_get_ext(bar);
    return ext->max_value;
}

/**
 * Get the style of bar indicator
 * @param bar pointer to a bar object
 * @return pointer to the bar indicator style
 */
lv_style_t * lv_bar_get_style_indic(lv_obj_t * bar)
{
    lv_bar_ext_t * ext = lv_obj_get_ext(bar);

    if(ext->style_indic == NULL) return lv_obj_get_style(bar);

    return ext->style_indic;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the bars
 * @param bar pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_bar_design(lv_obj_t * bar, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask area*/
    	return  ancestor_design_f(bar, mask, mode);;
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design_f(bar, mask, mode);

		lv_bar_ext_t * ext = lv_obj_get_ext(bar);

        lv_style_t * style_indic = lv_bar_get_style_indic(bar);
		area_t indic_area;
		area_cpy(&indic_area, &bar->cords);
		indic_area.x1 += style_indic->body.pad_hor;
		indic_area.x2 -= style_indic->body.pad_hor;
		indic_area.y1 += style_indic->body.pad_ver;
		indic_area.y2 -= style_indic->body.pad_ver;

		cord_t w = area_get_width(&indic_area);
        cord_t h = area_get_height(&indic_area);

		if(w >= h) {
		    indic_area.x2 = (int32_t) ((int32_t)w * (ext->act_value - ext->min_value)) / (ext->max_value - ext->min_value);
            indic_area.x2 += indic_area.x1;
		} else {
		    indic_area.y1 = (int32_t) ((int32_t)h * (ext->act_value - ext->min_value)) / (ext->max_value - ext->min_value);
            indic_area.y1 = indic_area.y2 - indic_area.y1;
		}

		/*Draw the indicator*/
        lv_draw_rect(&indic_area, mask, style_indic);
    }
    return true;
}

#endif
