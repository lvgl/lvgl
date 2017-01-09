/**
 * @file lv_gauge.c
 * 
 */


/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_GAUGE != 0

#include "lv_gauge.h"
#include <stdio.h>
#include "../lv_draw/lv_draw.h"
#include "../lv_misc/text.h"
#include "misc/math/trigo.h"

/*********************
 *      DEFINES
 *********************/
#define LV_GAUGE_DEF_WIDTH  (150 * LV_DOWNSCALE)
#define LV_GAUGE_DEF_HEIGHT  (150 * LV_DOWNSCALE)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_gauge_design(lv_obj_t * gauge, const area_t * mask, lv_design_mode_t mode);
static void lv_gauges_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_gauges_t lv_gauges_def;	/*Default gauge style*/
static lv_design_f_t ancestor_design_f = NULL;
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
 * Create a gauge objects
 * @param par pointer to an object, it will be the parent of the new gauge
 * @param copy pointer to a gauge object, if not NULL then the new object will be copied from it
 * @return pointer to the created gauge
 */
lv_obj_t * lv_gauge_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor gauge*/
    lv_obj_t * new_gauge = lv_rect_create(par, copy);
    dm_assert(new_gauge);
    
    /*Allocate the gauge type specific extended data*/
    lv_gauge_ext_t * ext = lv_obj_alloc_ext(new_gauge, sizeof(lv_gauge_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->min = 0;
    ext->max = 100;
    ext->value = 90 ;


    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_gauge);


    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_gauge, lv_gauge_signal);
    lv_obj_set_design_f(new_gauge, lv_gauge_design);

    /*Init the new gauge gauge*/
    if(copy == NULL) {
        lv_obj_set_size(new_gauge, LV_GAUGE_DEF_WIDTH, LV_GAUGE_DEF_HEIGHT);
        lv_obj_set_style(new_gauge, lv_gauges_get(LV_GAUGES_DEF, NULL));
    }
    /*Copy an existing gauge*/
    else {
    	lv_gauge_ext_t * copy_ext = lv_obj_get_ext(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_gauge);
    }
    
    return new_gauge;
}

/**
 * Signal function of the gauge
 * @param gauge pointer to a gauge object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_gauge_signal(lv_obj_t * gauge, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_rect_signal(gauge, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			/*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
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

void lv_gauge_set_value(lv_obj_t * gauge, int16_t value)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);
    if(value > ext->max) value = ext->max;
    if(value < ext->min) value = ext->min;

    ext->value = value;

    lv_obj_inv(gauge);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_gauges_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_gauges_t style
 */
lv_gauges_t * lv_gauges_get(lv_gauges_builtin_t style, lv_gauges_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_gauges_init();
		style_inited = true;
	}

	lv_gauges_t  *style_p;

	switch(style) {
		case LV_GAUGES_DEF:
			style_p = &lv_gauges_def;
			break;
		default:
			style_p = &lv_gauges_def;
	}

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_gauges_t));
		else memcpy(copy, &lv_gauges_def, sizeof(lv_gauges_t));
	}

	return style_p;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Handle the drawing related tasks of the gauges
 * @param gauge pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_gauge_design(lv_obj_t * gauge, const area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	return ancestor_design_f(gauge, mask, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_gauges_t * style = lv_obj_get_style(gauge);
        lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

        ancestor_design_f(gauge, mask, mode);

        char scale_txt[16];
        area_t label_cord;
        point_t label_size;

        uint8_t i;
        cord_t r = lv_obj_get_width(gauge) / 2 - style->label_pad;
        cord_t x;
        cord_t y;
        cord_t x_ofs = lv_obj_get_width(gauge) / 2 + gauge->cords.x1;
        cord_t y_ofs = lv_obj_get_height(gauge) / 2 + gauge->cords.y1;
        int16_t angle;
        int16_t angle_ofs = 90 + (360 - style->angle) / 2;
        int16_t scale_act;
        for(i = 0; i < style->label_num; i++) {

            angle = (i * style->angle) / (style->label_num - 1) + angle_ofs;
            y = (int32_t)((int32_t)trigo_sin(angle) * r) / TRIGO_SIN_MAX;
            y += y_ofs;

            x = (int32_t)((int32_t)trigo_sin(angle + 90) * r) / TRIGO_SIN_MAX;
            x += x_ofs;

            scale_act = (int32_t)((int32_t)(ext->max - ext->min) * i) /  (style->label_num - 1);
            scale_act += ext->min;
            sprintf(scale_txt, "%d", scale_act);

            txt_get_size(&label_size, scale_txt, font_get(style->scale_labels.font),
                    style->scale_labels.letter_space, style->scale_labels.line_space, LV_CORD_MAX);


            label_cord.x1 = x - label_size.x / 2;
            label_cord.y1 = y - label_size.y / 2;
            label_cord.x2 = label_cord.x1 + label_size.x;
            label_cord.y2 = label_cord.y1 + label_size.y;

            lv_draw_label(&label_cord, mask, &style->scale_labels, OPA_COVER, scale_txt);
        }

        point_t p_mid;
        point_t p_end;

        int16_t needle_angle = ext->value * 220 / (ext->max - ext->min) + angle_ofs;
        int16_t needle_y = (trigo_sin(needle_angle) * r) / TRIGO_SIN_MAX;
        int16_t needle_x = (trigo_sin(needle_angle + 90) * r) / TRIGO_SIN_MAX;

        p_mid.x = x_ofs;
        p_mid.y = y_ofs;
        p_end.x = needle_x + x_ofs;
        p_end.y = needle_y + y_ofs;

        lv_draw_line(&p_mid, &p_end, mask, &style->needle_lines, OPA_50);

        lv_rects_t nm;
        area_t nm_cord;
        lv_rects_get(LV_RECTS_DEF, &nm);
        nm.bwidth = 0;
        nm.round = LV_RECT_CIRCLE;
        nm.objs.color = COLOR_GRAY;//style->needle_lines.objs.color;
        nm.gcolor = COLOR_GRAY;//style->needle_lines.objs.color;

        nm_cord.x1 = x_ofs - 5 * LV_DOWNSCALE;
        nm_cord.y1 = y_ofs - 5 * LV_DOWNSCALE;
        nm_cord.x2 = x_ofs + 5 * LV_DOWNSCALE;
        nm_cord.y2 = y_ofs + 5 * LV_DOWNSCALE;

        lv_draw_rect(&nm_cord, mask, &nm, OPA_100);

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design_f(gauge, mask, mode);
    }

    return true;
}


/**
 * Initialize the built-in gauge styles
 */
static void lv_gauges_init(void)
{
	/*Default style*/
    lv_rects_get(LV_RECTS_DEF, &lv_gauges_def.rects);
    lv_gauges_def.rects.round = LV_RECT_CIRCLE;
    lv_gauges_def.rects.bwidth = 6 * LV_DOWNSCALE;
    lv_gauges_def.rects.gcolor = COLOR_RED;

    lv_labels_get(LV_RECTS_DEF, &lv_gauges_def.scale_labels);

    lv_lines_get(LV_LINES_DEF, &lv_gauges_def.needle_lines);
    lv_gauges_def.needle_lines.objs.color = COLOR_WHITE;
    lv_gauges_def.needle_lines.width = 3 * LV_DOWNSCALE;

    lv_gauges_def.label_pad = 20 * LV_DOWNSCALE;
    lv_gauges_def.label_num = 5;
    lv_gauges_def.angle = 220;
}

#endif
