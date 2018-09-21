

/**
 * @file lv_bar.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_BAR != 0

#include "lv_bar.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_anim.h"
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
static bool lv_bar_design(lv_obj_t * bar, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_bar_signal(lv_obj_t * bar, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_func_t ancestor_design_f;
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

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
    lv_mem_assert(new_bar);

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_bar);
    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_func(new_bar);

    /*Allocate the object type specific extended data*/
    lv_bar_ext_t * ext = lv_obj_allocate_ext_attr(new_bar, sizeof(lv_bar_ext_t));
    lv_mem_assert(ext);
    ext->min_value = 0;
    ext->max_value = 100;
    ext->cur_value = 0;
    ext->style_indic = &lv_style_pretty_color;

    lv_obj_set_signal_func(new_bar, lv_bar_signal);
    lv_obj_set_design_func(new_bar, lv_bar_design);

    /*Init the new  bar object*/
    if(copy == NULL) {
        lv_obj_set_click(new_bar, false);
    	lv_obj_set_size(new_bar, LV_DPI * 2, LV_DPI / 3);
    	lv_bar_set_value(new_bar, ext->cur_value);

    	lv_theme_t *th = lv_theme_get_current();
    	if(th) {
    	    lv_bar_set_style(new_bar, LV_BAR_STYLE_BG, th->bar.bg);
            lv_bar_set_style(new_bar, LV_BAR_STYLE_INDIC, th->bar.indic);
    	} else {
            lv_obj_set_style(new_bar, &lv_style_pretty);
    	}
    } else {
    	lv_bar_ext_t * ext_copy = lv_obj_get_ext_attr(copy);
		ext->min_value = ext_copy->min_value;
		ext->max_value = ext_copy->max_value;
		ext->cur_value = ext_copy->cur_value;
        ext->style_indic = ext_copy->style_indic;
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_bar);

        lv_bar_set_value(new_bar, ext->cur_value);
    }
    return new_bar;
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
	lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
	if(ext->cur_value == value) return;

	ext->cur_value = value > ext->max_value ? ext->max_value : value;
    ext->cur_value = ext->cur_value < ext->min_value ? ext->min_value : ext->cur_value;
	lv_obj_invalidate(bar);
}

#if USE_LV_ANIMATION
/**
 * Set a new value with animation on the bar
 * @param bar pointer to a bar object
 * @param value new value
 * @param anim_time animation time in milliseconds
 */
void lv_bar_set_value_anim(lv_obj_t * bar, int16_t value, uint16_t anim_time)
{
    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
	if(ext->cur_value == value) return;

    int16_t new_value;
    new_value = value > ext->max_value ? ext->max_value : value;
    new_value = new_value < ext->min_value ? ext->min_value : new_value;

    lv_anim_t a;
    a.var = bar;
    a.start = ext->cur_value;
    a.end = new_value;
    a.fp = (lv_anim_fp_t)lv_bar_set_value;
    a.path = lv_anim_path_linear;
    a.end_cb = NULL;
    a.act_time = 0;
    a.time = anim_time;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;

    lv_anim_create(&a);
}
#endif


/**
 * Set minimum and the maximum values of a bar
 * @param bar pointer to the bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_bar_set_range(lv_obj_t * bar, int16_t min, int16_t max)
{
	lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
	if(ext->min_value == min && ext->max_value == max) return;

	ext->max_value = max;
	ext->min_value = min;
	if(ext->cur_value > max) {
		ext->cur_value = max;
		lv_bar_set_value(bar, ext->cur_value);
	}
    if(ext->cur_value < min) {
        ext->cur_value = min;
        lv_bar_set_value(bar, ext->cur_value);
    }
	lv_obj_invalidate(bar);
}


/**
 * Set a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_bar_set_style(lv_obj_t *bar, lv_bar_style_t type, lv_style_t *style)
{
    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);

    switch (type) {
        case LV_BAR_STYLE_BG:
            lv_obj_set_style(bar, style);
            break;
        case LV_BAR_STYLE_INDIC:
            ext->style_indic = style;
            lv_obj_refresh_ext_size(bar);
            break;
    }
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
	lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
	return ext->cur_value;
}

/**
 * Get the minimum value of a bar
 * @param bar pointer to a bar object
 * @return the minimum value of the bar
 */
int16_t lv_bar_get_min_value(lv_obj_t * bar)
{
    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    return ext->min_value;
}

/**
 * Get the maximum value of a bar
 * @param bar pointer to a bar object
 * @return the maximum value of the bar
 */
int16_t lv_bar_get_max_value(lv_obj_t * bar)
{
    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    return ext->max_value;
}

/**
 * Get a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_bar_get_style(lv_obj_t *bar, lv_bar_style_t type)
{
    lv_bar_ext_t *ext = lv_obj_get_ext_attr(bar);

    switch (type) {
        case LV_BAR_STYLE_BG:    return lv_obj_get_style(bar);
        case LV_BAR_STYLE_INDIC: return ext->style_indic;
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
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
static bool lv_bar_design(lv_obj_t * bar, const lv_area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask area*/
    	return  ancestor_design_f(bar, mask, mode);;
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design_f(bar, mask, mode);

		lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);

        lv_style_t *style_indic = lv_bar_get_style(bar, LV_BAR_STYLE_INDIC);
		lv_area_t indic_area;
		lv_area_copy(&indic_area, &bar->coords);
		indic_area.x1 += style_indic->body.padding.hor;
		indic_area.x2 -= style_indic->body.padding.hor;
		indic_area.y1 += style_indic->body.padding.ver;
		indic_area.y2 -= style_indic->body.padding.ver;

		lv_coord_t w = lv_area_get_width(&indic_area);
        lv_coord_t h = lv_area_get_height(&indic_area);

		if(w >= h) {
		    indic_area.x2 = (int32_t) ((int32_t)w * (ext->cur_value - ext->min_value)) / (ext->max_value - ext->min_value);
            indic_area.x2 = indic_area.x1 + indic_area.x2 - 1;
		} else {
		    indic_area.y1 = (int32_t) ((int32_t)h * (ext->cur_value - ext->min_value)) / (ext->max_value - ext->min_value);
            indic_area.y1 = indic_area.y2 - indic_area.y1 + 1;
		}

		/*Draw the indicator*/
        lv_draw_rect(&indic_area, mask, style_indic);
    }
    return true;
}

/**
 * Signal function of the bar
 * @param bar pointer to a bar object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_bar_signal(lv_obj_t * bar, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(bar, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_REFR_EXT_SIZE) {
        lv_style_t * style_indic = lv_bar_get_style(bar, LV_BAR_STYLE_INDIC);
        if(style_indic->body.shadow.width > bar->ext_size) bar->ext_size = style_indic->body.shadow.width;
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_bar";
    }

    return res;
}


#endif
