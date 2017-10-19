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
#include "../lv_draw/lv_draw.h"
#include "misc/gfx/text.h"
#include "misc/math/trigo.h"
#include "misc/math/math_base.h"
#include <stdio.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#ifndef LV_GAUGE_MAX_NEEDLE
#define LV_GAUGE_MAX_NEEDLE     4   /*Max number of needles. Used in the style.*/
#endif

#define LV_GAUGE_DEF_NEEDLE_COLOR   COLOR_RED

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_gauge_design(lv_obj_t * gauge, const area_t * mask, lv_design_mode_t mode);
static void lv_gauge_draw_scale(lv_obj_t * gauge, const area_t * mask, lv_style_t * style);
static void lv_gauge_draw_needle(lv_obj_t * gauge, const area_t * mask, lv_style_t * style);

/**********************
 *  STATIC VARIABLES
 **********************/
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
    lv_obj_t * new_gauge = lv_lmeter_create(par, copy);
    dm_assert(new_gauge);
    
    /*Allocate the gauge type specific extended data*/
    lv_gauge_ext_t * ext = lv_obj_alloc_ext(new_gauge, sizeof(lv_gauge_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->needle_num = 0;
    ext->values = NULL;
    ext->needle_colors = NULL;
    ext->low_critical = 0;
    ext->style_critical = lv_style_get(LV_STYLE_PRETTY_COLOR, NULL);

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_gauge);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_gauge, lv_gauge_signal);
    lv_obj_set_design_f(new_gauge, lv_gauge_design);

    /*Init the new gauge gauge*/
    if(copy == NULL) {
        lv_lmeter_set_scale(new_gauge, 220, 6);
        lv_gauge_set_needle_num(new_gauge, 1, NULL);
        lv_obj_set_size(new_gauge, 2 * LV_DPI, 2 * LV_DPI);
        lv_obj_set_style(new_gauge, lv_style_get(LV_STYLE_PRETTY, NULL));
    }
    /*Copy an existing gauge*/
    else {
    	lv_gauge_ext_t * copy_ext = lv_obj_get_ext(copy);
        ext->low_critical = copy_ext->low_critical;
        lv_gauge_set_style_critical(new_gauge, lv_gauge_get_style_critical(copy));
        lv_gauge_set_needle_num(new_gauge, copy_ext->needle_num, copy_ext->needle_colors);

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
    valid = lv_lmeter_signal(gauge, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);
    	if(sign == LV_SIGNAL_CLEANUP) {
            dm_free(ext->values);
            ext->values = NULL;
    	}
    	else if(sign == LV_SIGNAL_REFR_EXT_SIZE) {
    	    lv_style_t * style_crit = lv_gauge_get_style_critical(gauge);
            if(style_crit->shadow.width > gauge->ext_size) gauge->ext_size = style_crit->shadow.width;
    	}
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of needles
 * @param gauge pointer to gauge object
 * @param num number of needles
 * @param colors an array of colors for needles (with 'num' elements)
 */
void lv_gauge_set_needle_num(lv_obj_t * gauge, uint8_t num, color_t * colors)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);
    if(ext->values != NULL) {
        dm_free(ext->values);
        ext->values = NULL;
    }

    ext->values = dm_realloc(ext->values, num * sizeof(int16_t));

    int16_t min = lv_bar_get_min_value(gauge);
    uint8_t n;
    for(n = ext->needle_num; n < num; n++) {
        ext->values[n] = min;
    }

    ext->needle_num = num;
    ext->needle_colors = colors;
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

    int16_t min = lv_bar_get_min_value(gauge);
    int16_t max = lv_bar_get_max_value(gauge);

    if(value > max) value = max;
    else if(value < min) value = min;

    ext->values[needle] = value;

    /*To be consistent with bar set the first needle's value for the bar*/
    if(needle == 0) lv_bar_set_value(gauge, value);

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

/**
 * Set the critical style of the gauge
 * @param gauge pointer to a gauge object
 * @param style pointer to the new critical style
 */
void lv_gauge_set_style_critical(lv_obj_t * gauge, lv_style_t * style)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);
    ext->style_critical = style;
    gauge->signal_f(gauge, LV_SIGNAL_REFR_EXT_SIZE, NULL);
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
    int16_t min = lv_bar_get_min_value(gauge);

    if(needle >= ext->needle_num) return min;

    return ext->values[needle];
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
 * Get the critical style of the gauge
 * @param gauge pointer to a gauge object
 * @return pointer to the critical style
 */
lv_style_t * lv_gauge_get_style_critical(lv_obj_t * gauge)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

    if(ext->style_critical == NULL) return lv_obj_get_style(gauge);

    return ext->style_critical;
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
    	return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_style_t * style_base = lv_obj_get_style(gauge);
        lv_style_t * style_critical = lv_gauge_get_style_critical(gauge);
        lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

        /* Draw the background
         * Re-color the gauge according to the critical value*/
        lv_style_t style_bg;
        int16_t min = lv_bar_get_min_value(gauge);
        int16_t max = lv_bar_get_max_value(gauge);

        /*To be consistent with bar use the bar value as the first needle*/
        if(ext->needle_num  != 0) {
            ext->values[0] = lv_bar_get_value(gauge);
        }

        int16_t critical_val = ext->low_critical == 0 ? min : max;
        uint8_t i;

        for(i = 0; i < ext->needle_num; i++) {
            critical_val = ext->low_critical == 0 ? MATH_MAX(critical_val, ext->values[i]) : MATH_MIN(critical_val, ext->values[i]);
        }

        opa_t ratio = ((critical_val - min) * OPA_COVER) / (max - min);

        if(ext->low_critical != 0) ratio = OPA_COVER - ratio;

        /*Mix the normal and the critical style*/
        memcpy(&style_bg, style_base, sizeof(lv_style_t));
        style_bg.text.color = color_mix(style_critical->text.color, style_base->text.color, ratio);
        style_bg.body.color_main= color_mix(style_critical->body.color_main, style_base->body.color_main, ratio);
        style_bg.body.color_gradient = color_mix(style_critical->body.color_gradient, style_base->body.color_gradient, ratio);
        style_bg.border.color = color_mix(style_critical->border.color, style_base->border.color, ratio);
        style_bg.shadow.color = color_mix(style_critical->shadow.color, style_base->shadow.color, ratio);
        style_bg.shadow.width = (cord_t)(((cord_t)style_critical->shadow.width * ratio) + ((cord_t)style_base->shadow.width * (OPA_COVER - ratio))) >> 8;
        style_bg.opacity = (cord_t)(((uint16_t)style_critical->opacity * ratio) + ((uint16_t)style_base->opacity * (OPA_COVER - ratio))) >> 8;

        lv_draw_rect(&gauge->cords, mask, &style_bg);

        lv_gauge_draw_scale(gauge, mask, &style_bg);

        lv_gauge_draw_needle(gauge, mask, &style_bg);
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
static void lv_gauge_draw_scale(lv_obj_t * gauge, const area_t * mask, lv_style_t * style)
{
    char scale_txt[16];

    cord_t r = lv_obj_get_width(gauge) / 2 - style->body.padding.horizontal;
    cord_t x_ofs = lv_obj_get_width(gauge) / 2 + gauge->cords.x1;
    cord_t y_ofs = lv_obj_get_height(gauge) / 2 + gauge->cords.y1;
    int16_t scale_angle = lv_lmeter_get_scale_angle(gauge);
    uint16_t scale_num = lv_lmeter_get_scale_num(gauge);
    int16_t angle_ofs = 90 + (360 - scale_angle) / 2;
    int16_t min = lv_bar_get_min_value(gauge);
    int16_t max = lv_bar_get_max_value(gauge);

    uint8_t i;
    for(i = 0; i < scale_num; i++) {
        /*Calculate the position a scale label*/
        int16_t angle = (i * scale_angle) / (scale_num - 1) + angle_ofs;

        cord_t y = (int32_t)((int32_t)trigo_sin(angle) * r) / TRIGO_SIN_MAX;
        y += y_ofs;

        cord_t x = (int32_t)((int32_t)trigo_sin(angle + 90) * r) / TRIGO_SIN_MAX;
        x += x_ofs;

        int16_t scale_act = (int32_t)((int32_t)(max - min) * i) /  (scale_num - 1);
        scale_act += min;
        sprintf(scale_txt, "%d", scale_act);

        area_t label_cord;
        point_t label_size;
        txt_get_size(&label_size, scale_txt, style->text.font,
                style->text.space_letter, style->text.space_line,
                CORD_MAX, TXT_FLAG_NONE);

        /*Draw the label*/
        label_cord.x1 = x - label_size.x / 2;
        label_cord.y1 = y - label_size.y / 2;
        label_cord.x2 = label_cord.x1 + label_size.x;
        label_cord.y2 = label_cord.y1 + label_size.y;

        lv_draw_label(&label_cord, mask, style, scale_txt, TXT_FLAG_NONE, NULL);
    }
}
/**
 * Draw the needles of a gauge
 * @param gauge pointer to gauge object
 * @param mask mask of drawing
 */
static void lv_gauge_draw_needle(lv_obj_t * gauge, const area_t * mask, lv_style_t * style)
{
    lv_style_t style_needle;
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

    cord_t r = lv_obj_get_width(gauge) / 2 - style->body.padding.inner;
    cord_t x_ofs = lv_obj_get_width(gauge) / 2 + gauge->cords.x1;
    cord_t y_ofs = lv_obj_get_height(gauge) / 2 + gauge->cords.y1;
    uint16_t angle = lv_lmeter_get_scale_angle(gauge);
    int16_t angle_ofs = 90 + (360 - angle) / 2;
    int16_t min = lv_bar_get_min_value(gauge);
    int16_t max = lv_bar_get_max_value(gauge);
    point_t p_mid;
    point_t p_end;
    uint8_t i;

    memcpy(&style_needle, style, sizeof(lv_style_t));

    p_mid.x = x_ofs;
    p_mid.y = y_ofs;
    for(i = 0; i < ext->needle_num; i++) {
        /*Calculate the end point of a needle*/
        int16_t needle_angle = (ext->values[i] - min) * angle / (max - min) + angle_ofs;
        p_end.y = (trigo_sin(needle_angle) * r) / TRIGO_SIN_MAX + y_ofs;
        p_end.x = (trigo_sin(needle_angle + 90) * r) / TRIGO_SIN_MAX + x_ofs;

        /*Draw the needle with the corresponding color*/
        if(ext->needle_colors == NULL) style_needle.line.color = LV_GAUGE_DEF_NEEDLE_COLOR;
        else style_needle.line.color = ext->needle_colors[i];

        lv_draw_line(&p_mid, &p_end, mask, &style_needle);
    }

    /*Draw the needle middle area*/
    lv_style_t style_neddle_mid;
    lv_style_get(LV_STYLE_PLAIN, &style_neddle_mid);
    style_neddle_mid.body.color_main = style->border.color;
    style_neddle_mid.body.color_gradient = style->border.color;
    style_neddle_mid.body.radius = LV_RADIUS_CIRCLE;

    area_t nm_cord;
    nm_cord.x1 = x_ofs - style->body.padding.inner;
    nm_cord.y1 = y_ofs - style->body.padding.inner;
    nm_cord.x2 = x_ofs + style->body.padding.inner;
    nm_cord.y2 = y_ofs + style->body.padding.inner;

    lv_draw_rect(&nm_cord, mask, &style_neddle_mid);
}

#endif
