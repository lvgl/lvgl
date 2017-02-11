

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
#define LV_PB_DEF_WIDTH     (120 * LV_DOWNSCALE)
#define LV_PB_DEF_HEIGHT    (40 * LV_DOWNSCALE)

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
static lv_pbs_t lv_pbs_slider;
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
    ext->min_value = 0;
    ext->max_value = 100;
    ext->act_value = 0;
    ext->tmp_value = 0;
    ext->format_str = NULL;
    ext->label = NULL;

    /* Save the rectangle design function.
     * It will be used in the progress bar design function*/
    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_pb);

    lv_obj_set_signal_f(new_pb, lv_pb_signal);
    lv_obj_set_design_f(new_pb, lv_pb_design);

    /*Init the new progress bar object*/
    if(copy == NULL) {
    	ext->format_str = dm_alloc(strlen(LV_PB_DEF_FORMAT) + 1);
    	strcpy(ext->format_str, LV_PB_DEF_FORMAT);

    	ext->label = lv_label_create(new_pb, NULL);

    	lv_rect_set_layout(new_pb, LV_RECT_LAYOUT_CENTER);
        lv_obj_set_click(new_pb, false);
    	lv_obj_set_size(new_pb, LV_PB_DEF_WIDTH, LV_PB_DEF_HEIGHT);
    	lv_obj_set_style(new_pb, lv_pbs_get(LV_PBS_DEF, NULL));

    	lv_pb_set_value(new_pb, ext->act_value);
    } else {
    	lv_pb_ext_t * ext_copy = lv_obj_get_ext(copy);
    	ext->format_str = dm_alloc(strlen(ext_copy->format_str) + 1);
		strcpy(ext->format_str, ext_copy->format_str);
		ext->min_value = ext_copy->min_value;
		ext->max_value = ext_copy->max_value;
		ext->act_value = ext_copy->act_value;
        ext->label = lv_label_create(new_pb, ext_copy->label);

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_pb);

        lv_pb_set_value(new_pb, ext->act_value);

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
        lv_pbs_t * style = lv_obj_get_style(pb);
        point_t p;
        char buf[LV_PB_TXT_MAX_LENGTH];

    	switch(sign) {
    	case LV_SIGNAL_CORD_CHG:
    		lv_pb_set_value(pb, ext->act_value);
    		break;
    	case LV_SIGNAL_CLEANUP:
    		dm_free(ext->format_str);
    		ext->format_str = NULL;
    		break;
    	case LV_SIGNAL_STYLE_CHG:
    	    lv_obj_set_style(ext->label, &style->label);
    	    lv_pb_set_value(pb, lv_pb_get_value(pb));
    	    break;
    	case LV_SIGNAL_PRESSING:
    	    lv_dispi_get_point(param, &p);
    	    if(lv_obj_get_width(pb) > lv_obj_get_height(pb)) {
    	        p.x -= pb->cords.x1 + style->btn_size / 2;
    	        ext->tmp_value = (int32_t) ((int32_t) p.x * (ext->max_value - ext->min_value + 1)) /
                        (lv_obj_get_width(pb) - style->btn_size);
    	    } else {
                p.y -= pb->cords.y1 + style->btn_size / 2;
                ext->tmp_value = (int32_t) ((int32_t) p.y * (ext->max_value - ext->min_value + 1)) /
                                  (lv_obj_get_height(pb) - style->btn_size);

                /*Invert the value: greater y means smaller value
                 * because it on a lower position on the screen*/
                ext->tmp_value = ext->max_value - ext->tmp_value;
            }

    	    ext->tmp_value = ext->tmp_value > ext->max_value ? ext->max_value : ext->tmp_value;
    	    ext->tmp_value = ext->tmp_value < ext->min_value ? ext->min_value : ext->tmp_value;

    	    sprintf(buf, ext->format_str, ext->tmp_value);
    	    lv_label_set_text(ext->label, buf);

    	    lv_obj_inv(pb);
    	    break;
        case LV_SIGNAL_PRESS_LOST:
            ext->tmp_value = ext->act_value;
            sprintf(buf, ext->format_str, ext->act_value);
            lv_label_set_text(ext->label, buf);
            lv_obj_inv(pb);
            break;
    	case LV_SIGNAL_RELEASED:
    	    lv_pb_set_value(pb, ext->tmp_value);
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
void lv_pb_set_value(lv_obj_t * pb, int16_t value)
{
	lv_pb_ext_t * ext = lv_obj_get_ext(pb);
	ext->act_value = value > ext->max_value ? ext->max_value : value;
    ext->act_value = ext->act_value < ext->min_value ? ext->min_value : ext->act_value;

    ext->tmp_value = ext->act_value;
	char buf[LV_PB_TXT_MAX_LENGTH];
	sprintf(buf, ext->format_str, ext->act_value);
	lv_label_set_text(ext->label, buf);

	lv_obj_inv(pb);
}

/**
 * Set minimum and the maximum values of a progress bar
 * @param pb pointer to he progress bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_pb_set_min_max_value(lv_obj_t * pb, int16_t min, int16_t max)
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
 * Set format string for the label of the progress bar
 * @param pb pointer to progress bar object
 * @param format a printf-like format string with one number (e.g. "Loading (%d)")
 */
void lv_pb_set_format_str(lv_obj_t * pb, const char * format)
{
	lv_pb_ext_t * ext = lv_obj_get_ext(pb);
	dm_free(ext->format_str);
	ext->format_str = dm_alloc(strlen(format) + 1);
	strcpy(ext->format_str, format);
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
int16_t lv_pb_get_value(lv_obj_t * pb)
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
        case LV_PBS_SLIDER:
            style_p = &lv_pbs_slider;
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
	if(ancestor_design_f == NULL) return false;

    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return  ancestor_design_f(pb, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		ancestor_design_f(pb, mask, mode);

		lv_pb_ext_t * ext = lv_obj_get_ext(pb);
        lv_pbs_t * style = lv_obj_get_style(pb);
		area_t bar_area;
		uint32_t tmp;
		area_cpy(&bar_area, &pb->cords);

		cord_t w = lv_obj_get_width(pb);
        cord_t h = lv_obj_get_height(pb);

		if(w >= h) {
            tmp = (int32_t)ext->tmp_value * (w - style->btn_size);
            tmp = (int32_t) tmp / (ext->max_value - ext->min_value);
            bar_area.x2 = bar_area.x1 + style->btn_size + (cord_t) tmp;
		} else {
            tmp = (int32_t)ext->tmp_value * (h - style->btn_size);
            tmp = (int32_t) tmp / (ext->max_value - ext->min_value);
            bar_area.y1 = bar_area.y2 - style->btn_size - (cord_t) tmp;
		}

		/*Draw the main bar*/
		opa_t opa = lv_obj_get_opa(pb);
        lv_draw_rect(&bar_area, mask, &style->bar, opa);

        /*Draw a button if its size is not 0*/
        if(style->btn_size != 0) {
            lv_rects_t tmp_rects;
            memcpy(&tmp_rects, &style->btn, sizeof(lv_rects_t));

            if(w >= h) {
                bar_area.x1 = bar_area.x2 - style->btn_size ;

                if(bar_area.x1 < pb->cords.x1) {
                    bar_area.x1 = pb->cords.x1;
                    bar_area.x2 = bar_area.x1 + style->btn_size;
                }

                if(bar_area.x2 > pb->cords.x2) {
                    bar_area.x2 = pb->cords.x2;
                    bar_area.x1 = bar_area.x2 - style->btn_size;
                }
            } else {
                bar_area.y2 = bar_area.y1 + style->btn_size ;

                if(bar_area.y1 < pb->cords.y1) {
                    bar_area.y1 = pb->cords.y1;
                    bar_area.y2 = bar_area.y1 + style->btn_size;
                }

                if(bar_area.y2 > pb->cords.y2) {
                    bar_area.y2 = pb->cords.y2;
                    bar_area.y1 = bar_area.y2 - style->btn_size;
                }

            }
            lv_draw_rect(&bar_area, mask, &tmp_rects, opa );
        }
    }
    return true;
}

/**
 * Set a new temporal (ghost) value on the progress bar
 * @param pb pointer to a progress bar object
 * @param value new value
 */
void lv_pb_set_tmp_value(lv_obj_t * pb, int16_t value)
{
    lv_pb_ext_t * ext = lv_obj_get_ext(pb);
    ext->act_value = value > ext->max_value ? ext->max_value : value;

    char buf[LV_PB_TXT_MAX_LENGTH];
    sprintf(buf, ext->format_str, ext->act_value);
    lv_label_set_text(ext->label, buf);

    lv_obj_inv(pb);
}

/**
 * Initialize the progress bar styles
 */
static void lv_pbs_init(void)
{
	/*Default style*/
    lv_rects_get(LV_RECTS_DEF, &lv_pbs_def.bg); /*Background*/
    lv_pbs_def.bg.objs.color = COLOR_WHITE;
    lv_pbs_def.bg.gcolor = COLOR_SILVER,
    lv_pbs_def.bg.bcolor = COLOR_BLACK;

    lv_rects_get(LV_RECTS_DEF, &lv_pbs_def.bar);    /*Bar*/
    lv_pbs_def.bar.objs.color = COLOR_LIME;
    lv_pbs_def.bar.gcolor = COLOR_GREEN;
    lv_pbs_def.bar.bcolor = COLOR_BLACK;

    lv_rects_get(LV_RECTS_DEF, &lv_pbs_def.btn);    /*Button*/
    lv_pbs_def.btn.objs.color = COLOR_WHITE;
    lv_pbs_def.btn.gcolor = COLOR_GRAY;
    lv_pbs_def.btn.bcolor = COLOR_GRAY;
    lv_pbs_def.btn.bopa = 100;
    lv_pbs_def.btn_size = 0;

    lv_labels_get(LV_LABELS_DEF, &lv_pbs_def.label);    /*Label*/
    lv_pbs_def.label.objs.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_pbs_def.label.line_space = 0;

    /*Slider style*/
    memcpy(&lv_pbs_slider, &lv_pbs_def, sizeof(lv_pbs_t));
    lv_pbs_slider.bg.round = LV_RECT_CIRCLE;
	lv_pbs_slider.bar.round = LV_RECT_CIRCLE;
    lv_pbs_slider.btn.round = LV_RECT_CIRCLE;
    lv_pbs_slider.btn_size = 40 * LV_DOWNSCALE;

}
#endif
