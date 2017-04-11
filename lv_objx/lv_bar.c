

/**
 * @file lv_bar.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_BAR != 0

#include <lvgl/lv_objx/lv_bar.h>
#include "../lv_draw/lv_draw.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define LV_BAR_TXT_MAX_LENGTH	64
#define LV_BAR_DEF_FORMAT	"%d %%"
#define LV_BAR_DEF_WIDTH     (LV_DPI * 2)
#define LV_BAR_DEF_HEIGHT    (LV_DPI / 2)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_bar_design(lv_obj_t * bar, const area_t * mask, lv_design_mode_t mode);
static void lv_bars_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_bars_t lv_bars_def;
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
    lv_obj_t * new_bar = lv_rect_create(par, copy);
    dm_assert(new_bar);

    /*Allocate the object type specific extended data*/
    lv_bar_ext_t * ext = lv_obj_alloc_ext(new_bar, sizeof(lv_bar_ext_t));
    dm_assert(ext);
    ext->min_value = 0;
    ext->max_value = 100;
    ext->act_value = 0;
    ext->format_str = NULL;
    ext->label = NULL;

    /* Save the rectangle design function.
     * It will be used in the bar design function*/
    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_bar);

    lv_obj_set_signal_f(new_bar, lv_bar_signal);
    lv_obj_set_design_f(new_bar, lv_bar_design);

    /*Init the new  bar object*/
    if(copy == NULL) {
    	ext->format_str = dm_alloc(strlen(LV_BAR_DEF_FORMAT) + 1);
    	strcpy(ext->format_str, LV_BAR_DEF_FORMAT);

    	ext->label = lv_label_create(new_bar, NULL);

    	lv_rect_set_layout(new_bar, LV_RECT_LAYOUT_CENTER);
        lv_obj_set_click(new_bar, false);
    	lv_obj_set_size(new_bar, LV_BAR_DEF_WIDTH, LV_BAR_DEF_HEIGHT);
    	lv_obj_set_style(new_bar, lv_bars_get(LV_BARS_DEF, NULL));

    	lv_bar_set_value(new_bar, ext->act_value);
    } else {
    	lv_bar_ext_t * ext_copy = lv_obj_get_ext(copy);
    	ext->format_str = dm_alloc(strlen(ext_copy->format_str) + 1);
		strcpy(ext->format_str, ext_copy->format_str);
		ext->min_value = ext_copy->min_value;
		ext->max_value = ext_copy->max_value;
		ext->act_value = ext_copy->act_value;
        ext->label = lv_label_create(new_bar, ext_copy->label);

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
    valid = lv_rect_signal(bar, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	lv_bar_ext_t * ext = lv_obj_get_ext(bar);
        lv_bars_t * style = lv_obj_get_style(bar);
        point_t p;
        char buf[LV_BAR_TXT_MAX_LENGTH];

    	switch(sign) {
            case LV_SIGNAL_CLEANUP:
                dm_free(ext->format_str);
                ext->format_str = NULL;
                break;
            case LV_SIGNAL_STYLE_CHG:
                lv_obj_set_style(ext->label, &style->label);
                lv_bar_set_value(bar, lv_bar_get_value(bar));
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
 * Set a new value on the bar
 * @param bar pointer to a bar object
 * @param value new value
 */
void lv_bar_set_value(lv_obj_t * bar, int16_t value)
{
	lv_bar_ext_t * ext = lv_obj_get_ext(bar);
	ext->act_value = value > ext->max_value ? ext->max_value : value;
    ext->act_value = ext->act_value < ext->min_value ? ext->min_value : ext->act_value;

	char buf[LV_BAR_TXT_MAX_LENGTH];
	sprintf(buf, ext->format_str, ext->act_value);
	lv_label_set_text(ext->label, buf);

	lv_obj_inv(bar);
}

/**
 * Set minimum and the maximum values of a  bar
 * @param bar pointer to he bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_bar_set_range(lv_obj_t * bar, int16_t min, int16_t max)
{
	lv_bar_ext_t * ext = lv_obj_get_ext(bar);
	ext->max_value = max;
	ext->max_value = max;
	if(ext->act_value > max) {
		ext->act_value = max;
		lv_bar_set_value(bar, ext->act_value);
	}
	lv_obj_inv(bar);
}

/**
 * Set format string for the label of the bar
 * @param bar pointer to bar object
 * @param format a printf-like format string with one number (e.g. "Loading (%d)")
 */
void lv_bar_set_format_str(lv_obj_t * bar, const char * format)
{
	lv_bar_ext_t * ext = lv_obj_get_ext(bar);
	dm_free(ext->format_str);
	ext->format_str = dm_alloc(strlen(format) + 1);
	strcpy(ext->format_str, format);
	lv_bar_set_value(bar, ext->act_value);
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
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_bars_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_bars_t style
 */
lv_bars_t * lv_bars_get(lv_bars_builtin_t style, lv_bars_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_bars_init();
		style_inited = true;
	}

	lv_bars_t  *style_p;

	switch(style) {
		case LV_BARS_DEF:
			style_p = &lv_bars_def;
			break;
		default:
			style_p = &lv_bars_def;
	}

	if(copy != NULL) memcpy(copy, style_p, sizeof(lv_bars_t));

	return style_p;
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
	if(ancestor_design_f == NULL) return false;

    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return  ancestor_design_f(bar, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		ancestor_design_f(bar, mask, mode);

		lv_bar_ext_t * ext = lv_obj_get_ext(bar);
        lv_bars_t * style = lv_obj_get_style(bar);
		area_t bar_area;
		uint32_t tmp;
		area_cpy(&bar_area, &bar->cords);

		cord_t w = lv_obj_get_width(bar);
        cord_t h = lv_obj_get_height(bar);

		if(w >= h) {
		    bar_area.x2 = (int32_t) ((int32_t)w * ext->act_value) / (ext->max_value - ext->min_value);
            bar_area.x2 += bar_area.x1;
		} else {
		    bar_area.y1 = (int32_t) ((int32_t)h * ext->act_value) / (ext->max_value - ext->min_value);
            bar_area.y1 = bar_area.y2 - bar_area.y1;
		}

		/*Draw the main bar*/
        lv_draw_rect(&bar_area, mask, &style->indic);
    }
    return true;
}

/**
 * Initialize the bar styles
 */
static void lv_bars_init(void)
{
	/*Default style*/
    lv_rects_get(LV_RECTS_FANCY, &lv_bars_def.bg); /*Background*/
    lv_bars_def.bg.base.color = COLOR_WHITE;
    lv_bars_def.bg.gcolor = COLOR_SILVER,
    lv_bars_def.bg.bcolor = COLOR_BLACK;

    lv_rects_get(LV_RECTS_FANCY, &lv_bars_def.indic);    /*Bar*/
    lv_bars_def.indic.base.color = COLOR_LIME;
    lv_bars_def.indic.gcolor = COLOR_GREEN;
    lv_bars_def.indic.bcolor = COLOR_BLACK;
    lv_bars_def.indic.swidth = 0;

    lv_labels_get(LV_LABELS_TXT, &lv_bars_def.label);    /*Label*/
    lv_bars_def.label.line_space = 0;

}
#endif
