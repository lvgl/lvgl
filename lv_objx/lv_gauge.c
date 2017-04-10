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
#include <string.h>
#include "../lv_draw/lv_draw.h"
#include "../lv_misc/text.h"
#include "misc/math/trigo.h"
#include "misc/math/math_base.h"

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
static void lv_gauge_draw_scale(lv_obj_t * gauge, const area_t * mask);
static void lv_gauge_draw_needle(lv_obj_t * gauge, const area_t * mask);
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
    ext->needle_num = 1;
    ext->low_critical = 0;
    ext->values = NULL;
    ext->txt = NULL;

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_gauge);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_gauge, lv_gauge_signal);
    lv_obj_set_design_f(new_gauge, lv_gauge_design);

    /*Init the new gauge gauge*/
    if(copy == NULL) {
        lv_gauge_set_needle_num(new_gauge, 1);
        lv_gauge_set_text(new_gauge, "%d");
        lv_obj_set_size(new_gauge, LV_GAUGE_DEF_WIDTH, LV_GAUGE_DEF_HEIGHT);
        lv_obj_set_style(new_gauge, lv_gauges_get(LV_GAUGES_DEF, NULL));
    }
    /*Copy an existing gauge*/
    else {
    	lv_gauge_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->min = copy_ext->min;
        ext->max = copy_ext->max;
        ext->low_critical = copy_ext->low_critical;
        lv_gauge_set_needle_num(new_gauge, lv_gauge_get_needle_num(copy));
        lv_gauge_set_text(new_gauge, lv_gauge_get_text(copy));

        uint8_t i;
        for(i = 0; i < ext->needle_num; i++) {
            ext->values[i] = copy_ext->values[i];
        }

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
        lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    		    dm_free(ext->values);
    		    ext->values = NULL;
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
 * Set the number of needles (should be  <= LV_GAUGE_MAX_NEEDLE)
 * @param gauge pointer to gauge object
 * @param num number of needles
 */
void lv_gauge_set_needle_num(lv_obj_t * gauge, uint8_t num)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);
    if(ext->values != NULL) dm_free(ext->values);

    ext->values = dm_alloc(num * sizeof(int16_t));

    ext->needle_num = num;
    lv_obj_inv(gauge);
}

/**
 * Set the range of a gauge
 * @param gauge pointer to gauge object
 * @param min min value
 * @param max max value
 */
void lv_gauge_set_range(lv_obj_t * gauge, int16_t min, int16_t max)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

    /*Be sure the smaller value is min and the greater is max*/
    ext->min = MATH_MIN(min, max);
    ext->max = MATH_MAX(min, max);

    lv_obj_inv(gauge);
}

/**
 * Set the value of a needle
 * @param gauge pointer to gauge
 * @param needle the id of the needle
 * @param value the new value
 */
void lv_gauge_set_value(lv_obj_t * gauge, uint8_t needle, int16_t value)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

    if(needle >= ext->needle_num) return;

    if(value > ext->max) value = ext->max;
    if(value < ext->min) value = ext->min;

    ext->values[needle] = value;

    lv_obj_inv(gauge);
}

/**
 * Set text on a gauge
 * @param gauge pinter to a gauge object
 * @param txt a printf like format string
 *            with 1 place for a number (e.g. "Value: %d");
 */
void lv_gauge_set_text(lv_obj_t * gauge, const char * txt)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

    if(ext->txt != NULL) dm_free(ext->txt);

    ext->txt = dm_alloc(strlen(txt) + 1);
    strcpy(ext->txt, txt);

    lv_obj_inv(gauge);
}

/**
 * Set which value is more critical (lower or higher)
 * @param gauge pointer to a gauge object
 * @param low false: higher / true: lower value is more critical
 */
void lv_gauge_set_low_critical(lv_obj_t * gauge, bool low)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

    ext->low_critical = low == false ? 0 : 1;

    lv_obj_inv(gauge);
}


/*=====================
 * Getter functions
 *====================*/

/**
 * Get the number of needles on a gauge
 * @param gauge pointer to gauge
 * @return number of needles
 */
uint8_t lv_gauge_get_needle_num(lv_obj_t * gauge)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);
    return ext->needle_num;
}

/**
 * Get the value of a needle
 * @param gauge pointer to gauge object
 * @param needle the id of the needle
 * @return the value of the needle [min,max]
 */
int16_t lv_gauge_get_value(lv_obj_t * gauge,  uint8_t needle)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

    if(needle >= ext->needle_num) return ext->min;

    return ext->values[needle];
}

/**
 * Get the text of a gauge
 * @param gauge pointer to gauge
 * @return the set text. (not with the current value)
 */
const char * lv_gauge_get_text(lv_obj_t * gauge)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);
    return ext->txt;

}

/**
 * Get which value is more critical (lower or higher)
 * @param gauge pointer to a gauge object
 * @param low false: higher / true: lower value is more critical
 */
bool lv_gauge_get_low_critical(lv_obj_t * gauge)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

    return ext->low_critical == 0 ? false : true;
}

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

        /* Draw the background
         * Re-color the gauge according to the critical value*/
        color_t mcolor_min = style->rects.objs.color;
        color_t gcolor_min = style->rects.gcolor;

        int16_t critical_val = ext->low_critical == 0 ? ext->min : ext->max;
        uint8_t i;

        for(i = 0; i < ext->needle_num; i++) {
            critical_val = ext->low_critical == 0 ? MATH_MAX(critical_val, ext->values[i]) : MATH_MIN(critical_val, ext->values[i]);
        }

        opa_t ratio = ((critical_val - ext->min) * OPA_COVER) / (ext->max - ext->min);

        if(ext->low_critical != 0) ratio = OPA_COVER - ratio;

        style->rects.objs.color= color_mix(style->mcolor_critical, mcolor_min, ratio);
        style->rects.gcolor = color_mix(style->gcolor_critical, gcolor_min, ratio);
        ancestor_design_f(gauge, mask, mode);
        style->rects.objs.color= mcolor_min;
        style->rects.gcolor = gcolor_min;

        lv_gauge_draw_scale(gauge, mask);

        lv_gauge_draw_needle(gauge, mask);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design_f(gauge, mask, mode);
    }

    return true;
}

/**
 * Draw the scale on a gauge
 * @param gauge pointer to gauge object
 * @param mask mask of drawing
 */
static void lv_gauge_draw_scale(lv_obj_t * gauge, const area_t * mask)
{
    lv_gauges_t * style = lv_obj_get_style(gauge);
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

    char scale_txt[16];

    cord_t r = lv_obj_get_width(gauge) / 2 - style->scale_pad;
    cord_t x_ofs = lv_obj_get_width(gauge) / 2 + gauge->cords.x1;
    cord_t y_ofs = lv_obj_get_height(gauge) / 2 + gauge->cords.y1;
    int16_t angle_ofs = 90 + (360 - style->scale_angle) / 2;

    uint8_t i;
    for(i = 0; i < style->scale_label_num; i++) {
        /*Calculate the position a scale label*/
        int16_t angle = (i * style->scale_angle) / (style->scale_label_num - 1) + angle_ofs;

        cord_t y = (int32_t)((int32_t)trigo_sin(angle) * r) / TRIGO_SIN_MAX;
        y += y_ofs;

        cord_t x = (int32_t)((int32_t)trigo_sin(angle + 90) * r) / TRIGO_SIN_MAX;
        x += x_ofs;

        int16_t scale_act = (int32_t)((int32_t)(ext->max - ext->min) * i) /  (style->scale_label_num - 1);
        scale_act += ext->min;
        sprintf(scale_txt, "%d", scale_act);

        area_t label_cord;
        point_t label_size;
        txt_get_size(&label_size, scale_txt, font_get(style->scale_labels.font),
                style->scale_labels.letter_space, style->scale_labels.line_space,
                LV_CORD_MAX, TXT_FLAG_NONE);

        /*Draw the label*/
        label_cord.x1 = x - label_size.x / 2;
        label_cord.y1 = y - label_size.y / 2;
        label_cord.x2 = label_cord.x1 + label_size.x;
        label_cord.y2 = label_cord.y1 + label_size.y;

        lv_draw_label(&label_cord, mask, &style->scale_labels, OPA_COVER, scale_txt, TXT_FLAG_NONE);
    }

    /*Calculate the critical value*/
    int16_t critical_value = ext->low_critical == 0 ? ext->min : ext->max;;
    for(i = 0; i < ext->needle_num; i++) {
        critical_value = ext->low_critical == 0 ?
                MATH_MAX(critical_value, ext->values[i]) : MATH_MIN(critical_value, ext->values[i]);
    }

    /*Write the critical value if enabled*/
    if(ext->txt[0] != '\0') {
        char value_txt[16];
        sprintf(value_txt, ext->txt, critical_value);

        area_t label_cord;
        point_t label_size;
        txt_get_size(&label_size, value_txt, font_get(style->value_labels.font),
                style->value_labels.letter_space, style->value_labels.line_space,
                LV_CORD_MAX, TXT_FLAG_NONE);

        /*Draw the label*/
        label_cord.x1 = gauge->cords.x1 + lv_obj_get_width(gauge) / 2 - label_size.x / 2;
        label_cord.y1 = gauge->cords.y1 +
                        (cord_t)style->value_pos * lv_obj_get_height(gauge) / 100 - label_size.y / 2;

        label_cord.x2 = label_cord.x1 + label_size.x;
        label_cord.y2 = label_cord.y1 + label_size.y;

        lv_draw_label(&label_cord, mask, &style->value_labels, OPA_COVER, value_txt, TXT_FLAG_NONE);
    }

}
/**
 * Draw the needles of a gauge
 * @param gauge pointer to gauge object
 * @param mask mask of drawing
 */
static void lv_gauge_draw_needle(lv_obj_t * gauge, const area_t * mask)
{
    lv_gauges_t * style = lv_obj_get_style(gauge);
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

    cord_t r = lv_obj_get_width(gauge) / 2 - style->scale_pad;
    cord_t x_ofs = lv_obj_get_width(gauge) / 2 + gauge->cords.x1;
    cord_t y_ofs = lv_obj_get_height(gauge) / 2 + gauge->cords.y1;
    int16_t angle_ofs = 90 + (360 - style->scale_angle) / 2;
    point_t p_mid;
    point_t p_end;
    uint8_t i;

    p_mid.x = x_ofs;
    p_mid.y = y_ofs;
    for(i = 0; i < ext->needle_num; i++) {
        /*Calculate the end point of a needle*/
        int16_t needle_angle = (ext->values[i] - ext->min) * style->scale_angle /
                               (ext->max - ext->min) + angle_ofs;
        p_end.y = (trigo_sin(needle_angle) * r) / TRIGO_SIN_MAX + y_ofs;
        p_end.x = (trigo_sin(needle_angle + 90) * r) / TRIGO_SIN_MAX + x_ofs;

        /*Draw the needle with the corresponding color*/
        style->needle_lines.objs.color = style->needle_color[i];

        lv_draw_line(&p_mid, &p_end, mask, &style->needle_lines, style->needle_opa);

    }

    /*Draw the needle middle area*/
    lv_rects_t nm;
    area_t nm_cord;
    lv_rects_get(LV_RECTS_DEF, &nm);
    nm.bwidth = 0;
    nm.radius = LV_RECT_CIRCLE;
    nm.objs.color = style->needle_mid_color;
    nm.gcolor = style->needle_mid_color;

    nm_cord.x1 = x_ofs - style->needle_mid_r;
    nm_cord.y1 = y_ofs - style->needle_mid_r;
    nm_cord.x2 = x_ofs + style->needle_mid_r;
    nm_cord.y2 = y_ofs + style->needle_mid_r;

    lv_draw_rect(&nm_cord, mask, &nm, OPA_100);

}

/**
 * Initialize the built-in gauge styles
 */
static void lv_gauges_init(void)
{
	/*Default style*/
    lv_rects_get(LV_RECTS_DEF, &lv_gauges_def.rects);
    lv_gauges_def.rects.radius = LV_RECT_CIRCLE;
    lv_gauges_def.rects.bwidth = 4 * LV_DOWNSCALE;
    lv_gauges_def.rects.objs.color = COLOR_MAKE(0x00, 0xaa, 0x00);//GREEN;
    lv_gauges_def.rects.gcolor = COLOR_BLACK;
    lv_gauges_def.rects.bcolor = COLOR_BLACK;

    lv_gauges_def.gcolor_critical = COLOR_BLACK;
    lv_gauges_def.mcolor_critical = COLOR_MAKE(0xff, 0x50, 0x50);

    lv_labels_get(LV_LABELS_DEF, &lv_gauges_def.scale_labels);
    lv_gauges_def.scale_labels.objs.color = COLOR_MAKE(0xd0, 0xd0, 0xd0);

    lv_labels_get(LV_LABELS_DEF, &lv_gauges_def.value_labels);
    lv_gauges_def.value_labels.objs.color = COLOR_WHITE;
    lv_gauges_def.value_labels.letter_space = 3 * LV_DOWNSCALE;
    lv_gauges_def.value_labels.mid = 1;

    lv_gauges_def.value_pos = 75;

    lv_lines_get(LV_LINES_DEF, &lv_gauges_def.needle_lines);
    lv_gauges_def.needle_lines.objs.color = COLOR_WHITE;
    lv_gauges_def.needle_lines.width = 3 * LV_DOWNSCALE;

    lv_gauges_def.needle_color[0] = COLOR_SILVER;
    lv_gauges_def.needle_color[1] = COLOR_MAKE(0x40, 0x90, 0xe0);
    lv_gauges_def.needle_color[2] = COLOR_MAKE(0x50, 0xe0, 0x50);
    lv_gauges_def.needle_color[3] = COLOR_MAKE(0xff, 0xff, 0x70);

    lv_gauges_def.needle_mid_r = 5 * LV_DOWNSCALE;
    lv_gauges_def.needle_mid_color = COLOR_GRAY;
    lv_gauges_def.needle_opa = OPA_80;
    lv_gauges_def.scale_pad = 20 * LV_DOWNSCALE;
    lv_gauges_def.scale_label_num = 6;
    lv_gauges_def.scale_angle = 220;
}

#endif
