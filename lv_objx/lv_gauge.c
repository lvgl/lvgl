/**
 * @file lv_gauge.c
 *
 */


/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_GAUGE != 0

#include "lv_gauge.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_trigo.h"
#include "../lv_misc/lv_math.h"
#include <stdio.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define LV_GAUGE_DEF_NEEDLE_COLOR       LV_COLOR_RED
#define LV_GAUGE_DEF_LABEL_COUNT        6
#define LV_GAUGE_DEF_LINE_COUNT         21      /*Should be: ((label_cnt - 1) * internal_lines) + 1*/
#define LV_GAUGE_DEF_ANGLE              220


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_gauge_design(lv_obj_t * gauge, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_gauge_signal(lv_obj_t * gauge, lv_signal_t sign, void * param);
static void lv_gauge_draw_scale(lv_obj_t * gauge, const lv_area_t * mask);
static void lv_gauge_draw_needle(lv_obj_t * gauge, const lv_area_t * mask);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_func_t ancestor_design;
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

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
    lv_mem_assert(new_gauge);

    /*Allocate the gauge type specific extended data*/
    lv_gauge_ext_t * ext = lv_obj_allocate_ext_attr(new_gauge, sizeof(lv_gauge_ext_t));
    lv_mem_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->needle_count = 0;
    ext->values = NULL;
    ext->needle_colors = NULL;
    ext->label_count = LV_GAUGE_DEF_LABEL_COUNT;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_gauge);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_gauge);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_gauge, lv_gauge_signal);
    lv_obj_set_design_func(new_gauge, lv_gauge_design);

    /*Init the new gauge gauge*/
    if(copy == NULL) {
        lv_gauge_set_scale(new_gauge, LV_GAUGE_DEF_ANGLE, LV_GAUGE_DEF_LINE_COUNT, LV_GAUGE_DEF_LABEL_COUNT);
        lv_gauge_set_needle_count(new_gauge, 1, NULL);
        lv_gauge_set_critical_value(new_gauge, 80);
        lv_obj_set_size(new_gauge, 2 * LV_DPI, 2 * LV_DPI);

        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            lv_gauge_set_style(new_gauge, th->gauge);
        } else {
            lv_gauge_set_style(new_gauge, &lv_style_pretty_color);
        }
    }
    /*Copy an existing gauge*/
    else {
    	lv_gauge_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        lv_gauge_set_needle_count(new_gauge, copy_ext->needle_count, copy_ext->needle_colors);

        uint8_t i;
        for(i = 0; i < ext->needle_count; i++) {
            ext->values[i] = copy_ext->values[i];
        }
        ext->label_count = copy_ext->label_count;
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_gauge);
    }

    return new_gauge;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of needles
 * @param gauge pointer to gauge object
 * @param needle_cnt new count of needles
 * @param colors an array of colors for needles (with 'num' elements)
 */
void lv_gauge_set_needle_count(lv_obj_t * gauge, uint8_t needle_cnt, const lv_color_t * colors)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    if(ext->values != NULL) {
        lv_mem_free(ext->values);
        ext->values = NULL;
    }

    ext->values = lv_mem_realloc(ext->values, needle_cnt * sizeof(int16_t));

    int16_t min = lv_gauge_get_min_value(gauge);
    uint8_t n;
    for(n = ext->needle_count; n < needle_cnt; n++) {
        ext->values[n] = min;
    }

    ext->needle_count = needle_cnt;
    ext->needle_colors = colors;
    lv_obj_invalidate(gauge);
}

/**
 * Set the value of a needle
 * @param gauge pointer to a gauge
 * @param needle_id the id of the needle
 * @param value the new value
 */
void lv_gauge_set_value(lv_obj_t * gauge, uint8_t needle_id, int16_t value)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);

    if(needle_id >= ext->needle_count) return;

    int16_t min = lv_gauge_get_min_value(gauge);
    int16_t max = lv_gauge_get_max_value(gauge);

    if(value > max) value = max;
    else if(value < min) value = min;

    ext->values[needle_id] = value;


    lv_obj_invalidate(gauge);
}


/**
 * Set the scale settings of a gauge
 * @param gauge pointer to a gauge object
 * @param angle angle of the scale (0..360)
 * @param line_cnt count of scale lines
 * @param label_cnt count of scale labels
 */
void lv_gauge_set_scale(lv_obj_t * gauge, uint16_t angle, uint8_t line_cnt, uint8_t label_cnt)
{
    lv_lmeter_set_scale(gauge, angle, line_cnt);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    ext->label_count = label_cnt;
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a needle
 * @param gauge pointer to gauge object
 * @param needle the id of the needle
 * @return the value of the needle [min,max]
 */
int16_t lv_gauge_get_value(lv_obj_t * gauge,  uint8_t needle)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    int16_t min = lv_gauge_get_min_value(gauge);

    if(needle >= ext->needle_count) return min;

    return ext->values[needle];
}

/**
 * Get the count of needles on a gauge
 * @param gauge pointer to gauge
 * @return count of needles
 */
uint8_t lv_gauge_get_needle_count(lv_obj_t * gauge)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    return ext->needle_count;
}

/**
 * Set the number of labels (and the thicker lines too)
 * @param gauge pointer to a gauge object
 * @return count of labels
 */
uint8_t lv_gauge_get_label_count(lv_obj_t * gauge)
{
    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    return ext->label_count;
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
static bool lv_gauge_design(lv_obj_t * gauge, const lv_area_t * mask, lv_design_mode_t mode)
{

    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {

        /* Store the real pointer because of 'lv_group'
         * If the object is in focus 'lv_obj_get_style()' will give a pointer to tmp style
         * and to the real object style. It is important because of style change tricks below*/
        lv_style_t *style_ori_p = gauge->style_p;
        lv_style_t *style = lv_obj_get_style(gauge);
        lv_gauge_ext_t *ext = lv_obj_get_ext_attr(gauge);

        lv_gauge_draw_scale(gauge, mask);

        /*Draw the ancestor line meter with max value to show the rainbow like line colors*/
        uint16_t line_cnt_tmp = ext->lmeter.line_cnt;
        ancestor_design(gauge, mask, mode);           /*To draw lines*/

        /*Temporally modify the line meter to draw thicker and longer lines where labels are*/
        lv_style_t style_tmp;
        lv_style_copy(&style_tmp, style);
        ext->lmeter.line_cnt = ext->label_count;                        /*Only to labels*/
        style_tmp.line.width = style_tmp.line.width * 2;                /*Ticker lines*/
        style_tmp.body.padding.hor = style_tmp.body.padding.hor * 2;    /*Longer lines*/
        gauge->style_p = &style_tmp;

        ancestor_design(gauge, mask, mode);           /*To draw lines*/

        ext->lmeter.line_cnt = line_cnt_tmp;          /*Restore the parameters*/
        gauge->style_p = style_ori_p;                 /*Restore the ORIGINAL style pointer*/

        lv_gauge_draw_needle(gauge, mask);

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(gauge, mask, mode);
    }

    return true;
}

/**
 * Signal function of the gauge
 * @param gauge pointer to a gauge object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_gauge_signal(lv_obj_t * gauge, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(gauge, sign, param);
    if(res != LV_RES_OK) return res;

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    if(sign == LV_SIGNAL_CLEANUP) {
        lv_mem_free(ext->values);
        ext->values = NULL;
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_gauge";
    }

    return res;
}

/**
 * Draw the scale on a gauge
 * @param gauge pointer to gauge object
 * @param mask mask of drawing
 */
static void lv_gauge_draw_scale(lv_obj_t * gauge, const lv_area_t * mask)
{
    char scale_txt[16];

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    lv_style_t * style = lv_obj_get_style(gauge);
    lv_coord_t r = lv_obj_get_width(gauge) / 2 - (3 * style->body.padding.hor) - style->body.padding.inner;
    lv_coord_t x_ofs = lv_obj_get_width(gauge) / 2 + gauge->coords.x1;
    lv_coord_t y_ofs = lv_obj_get_height(gauge) / 2 + gauge->coords.y1;
    int16_t scale_angle = lv_lmeter_get_scale_angle(gauge);
    uint16_t label_num = ext->label_count;
    int16_t angle_ofs = 90 + (360 - scale_angle) / 2;
    int16_t min = lv_gauge_get_min_value(gauge);
    int16_t max = lv_gauge_get_max_value(gauge);

    uint8_t i;
    for(i = 0; i < label_num; i++) {
        /*Calculate the position a scale label*/
        int16_t angle = (i * scale_angle) / (label_num - 1) + angle_ofs;

        lv_coord_t y = (int32_t)((int32_t)lv_trigo_sin(angle) * r) / LV_TRIGO_SIN_MAX;
        y += y_ofs;

        lv_coord_t x = (int32_t)((int32_t)lv_trigo_sin(angle + 90) * r) / LV_TRIGO_SIN_MAX;
        x += x_ofs;

        int16_t scale_act = (int32_t)((int32_t)(max - min) * i) /  (label_num - 1);
        scale_act += min;
        lv_math_num_to_str(scale_act, scale_txt);

        lv_area_t label_cord;
        lv_point_t label_size;
        lv_txt_get_size(&label_size, scale_txt, style->text.font,
                style->text.letter_space, style->text.line_space, LV_COORD_MAX, LV_TXT_FLAG_NONE);

        /*Draw the label*/
        label_cord.x1 = x - label_size.x / 2;
        label_cord.y1 = y - label_size.y / 2;
        label_cord.x2 = label_cord.x1 + label_size.x;
        label_cord.y2 = label_cord.y1 + label_size.y;

        lv_draw_label(&label_cord, mask, style, scale_txt, LV_TXT_FLAG_NONE, NULL);
    }
}
/**
 * Draw the needles of a gauge
 * @param gauge pointer to gauge object
 * @param mask mask of drawing
 */
static void lv_gauge_draw_needle(lv_obj_t * gauge, const lv_area_t * mask)
{
    lv_style_t style_needle;
    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    lv_style_t * style = lv_gauge_get_style(gauge);

    lv_coord_t r = lv_obj_get_width(gauge) / 2 - style->body.padding.hor;
    lv_coord_t x_ofs = lv_obj_get_width(gauge) / 2 + gauge->coords.x1;
    lv_coord_t y_ofs = lv_obj_get_height(gauge) / 2 + gauge->coords.y1;
    uint16_t angle = lv_lmeter_get_scale_angle(gauge);
    int16_t angle_ofs = 90 + (360 - angle) / 2;
    int16_t min = lv_gauge_get_min_value(gauge);
    int16_t max = lv_gauge_get_max_value(gauge);
    lv_point_t p_mid;
    lv_point_t p_end;
    uint8_t i;

    lv_style_copy(&style_needle, style);

    p_mid.x = x_ofs;
    p_mid.y = y_ofs;
    for(i = 0; i < ext->needle_count; i++) {
        /*Calculate the end point of a needle*/
        int16_t needle_angle = (ext->values[i] - min) * angle / (max - min) + angle_ofs;
        p_end.y = (lv_trigo_sin(needle_angle) * r) / LV_TRIGO_SIN_MAX + y_ofs;
        p_end.x = (lv_trigo_sin(needle_angle + 90) * r) / LV_TRIGO_SIN_MAX + x_ofs;

        /*Draw the needle with the corresponding color*/
        if(ext->needle_colors == NULL) style_needle.line.color = LV_GAUGE_DEF_NEEDLE_COLOR;
        else style_needle.line.color = ext->needle_colors[i];

        lv_draw_line(&p_mid, &p_end, mask, &style_needle);
    }

    /*Draw the needle middle area*/
    lv_style_t style_neddle_mid;
    lv_style_copy(&style_neddle_mid, &lv_style_plain);
    style_neddle_mid.body.main_color = style->body.border.color;
    style_neddle_mid.body.grad_color = style->body.border.color;
    style_neddle_mid.body.radius = LV_RADIUS_CIRCLE;

    lv_area_t nm_cord;
    nm_cord.x1 = x_ofs - style->body.padding.ver;
    nm_cord.y1 = y_ofs - style->body.padding.ver;
    nm_cord.x2 = x_ofs + style->body.padding.ver;
    nm_cord.y2 = y_ofs + style->body.padding.ver;

    lv_draw_rect(&nm_cord, mask, &style_neddle_mid);
}

#endif
