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
#define LV_GAUGE_DEF_WIDTH          (3 * LV_DPI)
#define LV_GAUGE_DEF_HEIGHT         (3 * LV_DPI)
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
    lv_obj_t * new_gauge = lv_rect_create(par, copy);
    dm_assert(new_gauge);
    
    /*Allocate the gauge type specific extended data*/
    lv_gauge_ext_t * ext = lv_obj_alloc_ext(new_gauge, sizeof(lv_gauge_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->min = 0;
    ext->max = 100;
    ext->needle_num = 0;
    ext->values = NULL;
    ext->needle_color = NULL;
    ext->low_critical = 0;
    ext->scale_angle = 120;
    ext->scale_label_num = 6;
    ext->style_critical = lv_style_get(LV_STYLE_PRETTY_COLOR, NULL);

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_gauge);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_gauge, lv_gauge_signal);
    lv_obj_set_design_f(new_gauge, lv_gauge_design);

    /*Init the new gauge gauge*/
    if(copy == NULL) {
        lv_gauge_set_needle_num(new_gauge, 1, NULL);
        lv_obj_set_size(new_gauge, LV_GAUGE_DEF_WIDTH, LV_GAUGE_DEF_HEIGHT);
        lv_obj_set_style(new_gauge, lv_style_get(LV_STYLE_PRETTY, NULL));
    }
    /*Copy an existing gauge*/
    else {
    	lv_gauge_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->min = copy_ext->min;
        ext->max = copy_ext->max;
        ext->low_critical = copy_ext->low_critical;
        lv_gauge_set_needle_num(new_gauge, copy_ext->needle_num, copy_ext->needle_color);

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
    	if(sign == LV_SIGNAL_CLEANUP) {
            dm_free(ext->values);
            ext->values = NULL;
    	}
    	else if(sign == LV_SIGNAL_REFR_EXT_SIZE) {
    	    lv_style_t * style_crit = lv_gauge_get_style_crit(gauge);
            if(style_crit->swidth > gauge->ext_size) gauge->ext_size = style_crit->swidth;
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
    if(ext->values != NULL) dm_free(ext->values);

    ext->values = dm_realloc(ext->values, num * sizeof(int16_t));

    uint8_t n;
    for(n = ext->needle_num; n < num; n++) {
        ext->values[n] = ext->min;
    }

    ext->needle_num = num;
    ext->needle_color = colors;
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
 * Set the scale settings of a gauge
 * @param gauge pointer to a gauge object
 * @param angle angle of the scale (0..360)
 * @param label_num number of labels on the scale (~5)
 */
void lv_gauge_set_scale(lv_obj_t * gauge, uint16_t angle, uint8_t label_num)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);
    ext->scale_angle = angle;
    ext->scale_label_num = label_num;

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

    if(needle >= ext->needle_num) return ext->min;

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
lv_style_t * lv_gauge_get_style_crit(lv_obj_t * gauge)
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
    	return ancestor_design_f(gauge, mask, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_style_t * style_base = lv_obj_get_style(gauge);
        lv_style_t * style_critical = lv_gauge_get_style_crit(gauge);
        lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

        /* Draw the background
         * Re-color the gauge according to the critical value*/
        lv_style_t style_bg;

        int16_t critical_val = ext->low_critical == 0 ? ext->min : ext->max;
        uint8_t i;

        for(i = 0; i < ext->needle_num; i++) {
            critical_val = ext->low_critical == 0 ? MATH_MAX(critical_val, ext->values[i]) : MATH_MIN(critical_val, ext->values[i]);
        }

        opa_t ratio = ((critical_val - ext->min) * OPA_COVER) / (ext->max - ext->min);

        if(ext->low_critical != 0) ratio = OPA_COVER - ratio;

        /*Mix the normal and the critical style*/
        memcpy(&style_bg, style_base, sizeof(lv_style_t));
        style_bg.ccolor = color_mix(style_critical->ccolor, style_base->ccolor, ratio);
        style_bg.mcolor= color_mix(style_critical->mcolor, style_base->mcolor, ratio);
        style_bg.gcolor = color_mix(style_critical->gcolor, style_base->gcolor, ratio);
        style_bg.bcolor = color_mix(style_critical->bcolor, style_base->bcolor, ratio);
        style_bg.scolor = color_mix(style_critical->scolor, style_base->scolor, ratio);
        style_bg.swidth = (cord_t)((cord_t)(style_critical->swidth + style_base->swidth) * ratio) >> 8;

        gauge->style_p = &style_bg;
        ancestor_design_f(gauge, mask, mode);
        gauge->style_p = style_base;

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
    lv_gauge_ext_t * ext = lv_obj_get_ext(gauge);

    char scale_txt[16];

    cord_t r = lv_obj_get_width(gauge) / 2 - style->hpad;
    cord_t x_ofs = lv_obj_get_width(gauge) / 2 + gauge->cords.x1;
    cord_t y_ofs = lv_obj_get_height(gauge) / 2 + gauge->cords.y1;
    int16_t angle_ofs = 90 + (360 - ext->scale_angle) / 2;

    uint8_t i;
    for(i = 0; i < ext->scale_label_num; i++) {
        /*Calculate the position a scale label*/
        int16_t angle = (i * ext->scale_angle) / (ext->scale_label_num - 1) + angle_ofs;

        cord_t y = (int32_t)((int32_t)trigo_sin(angle) * r) / TRIGO_SIN_MAX;
        y += y_ofs;

        cord_t x = (int32_t)((int32_t)trigo_sin(angle + 90) * r) / TRIGO_SIN_MAX;
        x += x_ofs;

        int16_t scale_act = (int32_t)((int32_t)(ext->max - ext->min) * i) /  (ext->scale_label_num - 1);
        scale_act += ext->min;
        sprintf(scale_txt, "%d", scale_act);

        area_t label_cord;
        point_t label_size;
        txt_get_size(&label_size, scale_txt, style->font,
                style->letter_space, style->line_space,
                LV_CORD_MAX, TXT_FLAG_NONE);

        /*Draw the label*/
        label_cord.x1 = x - label_size.x / 2;
        label_cord.y1 = y - label_size.y / 2;
        label_cord.x2 = label_cord.x1 + label_size.x;
        label_cord.y2 = label_cord.y1 + label_size.y;

        lv_draw_label(&label_cord, mask, style, scale_txt, TXT_FLAG_NONE);
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

    cord_t r = lv_obj_get_width(gauge) / 2 - style->opad;
    cord_t x_ofs = lv_obj_get_width(gauge) / 2 + gauge->cords.x1;
    cord_t y_ofs = lv_obj_get_height(gauge) / 2 + gauge->cords.y1;
    int16_t angle_ofs = 90 + (360 - ext->scale_angle) / 2;
    point_t p_mid;
    point_t p_end;
    uint8_t i;

    memcpy(&style_needle, style, sizeof(lv_style_t));

    p_mid.x = x_ofs;
    p_mid.y = y_ofs;
    for(i = 0; i < ext->needle_num; i++) {
        /*Calculate the end point of a needle*/
        int16_t needle_angle = (ext->values[i] - ext->min) * ext->scale_angle /
                               (ext->max - ext->min) + angle_ofs;
        p_end.y = (trigo_sin(needle_angle) * r) / TRIGO_SIN_MAX + y_ofs;
        p_end.x = (trigo_sin(needle_angle + 90) * r) / TRIGO_SIN_MAX + x_ofs;

        /*Draw the needle with the corresponding color*/
        if(ext->needle_color == NULL) style_needle.ccolor = LV_GAUGE_DEF_NEEDLE_COLOR;
        else style_needle.ccolor = ext->needle_color[i];

        lv_draw_line(&p_mid, &p_end, mask, &style_needle);
    }

    /*Draw the needle middle area*/
    lv_style_t style_neddle_mid;
    lv_style_get(LV_STYLE_PLAIN, &style_neddle_mid);
    style_neddle_mid.mcolor = style->bcolor;
    style_neddle_mid.gcolor = style->bcolor;
    style_neddle_mid.radius = LV_RECT_CIRCLE;

    area_t nm_cord;
    nm_cord.x1 = x_ofs - style->opad;
    nm_cord.y1 = y_ofs - style->opad;
    nm_cord.x2 = x_ofs + style->opad;
    nm_cord.y2 = y_ofs + style->opad;

    lv_draw_rect(&nm_cord, mask, &style_neddle_mid);
}

#endif
