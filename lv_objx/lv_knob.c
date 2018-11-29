/**
 * @file lv_knob.c
 *
 */


/*********************
 *      INCLUDES
 *********************/
#include "lv_knob.h"
#if USE_LV_KNOB != 0

#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_math.h"
#include <stdio.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define LV_KNOB_DEF_NEEDLE_COLOR       LV_COLOR_RED
#define LV_KNOB_DEF_LABEL_COUNT        0
#define LV_KNOB_DEF_LINE_COUNT         0      /*Should be: ((label_cnt - 1) * internal_lines) + 1*/
#define LV_KNOB_DEF_ANGLE              270


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_knob_design(lv_obj_t * knob, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_knob_signal(lv_obj_t * knob, lv_signal_t sign, void * param);
static void lv_knob_draw_scale(lv_obj_t * knob, const lv_area_t * mask);
static void lv_knob_draw_body(lv_obj_t * knob, const lv_area_t * mask);

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
 * Create a knob objects
 * @param par pointer to an object, it will be the parent of the new knob
 * @param copy pointer to a knob object, if not NULL then the new object will be copied from it
 * @return pointer to the created knob
 */
lv_obj_t * lv_knob_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor knob*/
    lv_obj_t * new_knob = lv_lmeter_create(par, copy);
    lv_mem_assert(new_knob);

    /*Allocate the knob type specific extended data*/
    lv_knob_ext_t * ext = lv_obj_allocate_ext_attr(new_knob, sizeof(lv_knob_ext_t));
    lv_mem_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->needle_count = 0;
    ext->values = NULL;
    ext->needle_colors = NULL;
    ext->label_count = LV_KNOB_DEF_LABEL_COUNT;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_knob);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_knob);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_knob, lv_knob_signal);
    lv_obj_set_design_func(new_knob, lv_knob_design);

    /*Init the new knob knob*/
    if(copy == NULL) {
        lv_knob_set_scale(new_knob, LV_KNOB_DEF_ANGLE, LV_KNOB_DEF_LINE_COUNT, LV_KNOB_DEF_LABEL_COUNT);
        lv_knob_set_needle_count(new_knob, 1, NULL);
        lv_knob_set_critical_value(new_knob, 80);
        lv_obj_set_size(new_knob, 2 * LV_DPI, 2 * LV_DPI);

        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            lv_knob_set_style(new_knob, th->knob);
        } else {
            lv_knob_set_style(new_knob, &lv_style_pretty_color);
        }
    }
    /*Copy an existing knob*/
    else {
    	lv_knob_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        lv_knob_set_needle_count(new_knob, copy_ext->needle_count, copy_ext->needle_colors);

        uint8_t i;
        for(i = 0; i < ext->needle_count; i++) {
            ext->values[i] = copy_ext->values[i];
        }
        ext->label_count = copy_ext->label_count;
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_knob);
    }

    return new_knob;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of needles
 * @param knob pointer to knob object
 * @param needle_cnt new count of needles
 * @param colors an array of colors for needles (with 'num' elements)
 */
void lv_knob_set_needle_count(lv_obj_t * knob, uint8_t needle_cnt, const lv_color_t * colors)
{
    lv_knob_ext_t * ext = lv_obj_get_ext_attr(knob);

    if(ext->needle_count != needle_cnt) {
		if(ext->values != NULL) {
			lv_mem_free(ext->values);
			ext->values = NULL;
		}

		ext->values = lv_mem_realloc(ext->values, needle_cnt * sizeof(int16_t));

		int16_t min = lv_knob_get_min_value(knob);
		uint8_t n;
		for(n = ext->needle_count; n < needle_cnt; n++) {
			ext->values[n] = min;
		}

		ext->needle_count = needle_cnt;
    }

    ext->needle_colors = colors;
    lv_obj_invalidate(knob);
}

/**
 * Set the value of a needle
 * @param knob pointer to a knob
 * @param needle_id the id of the needle
 * @param value the new value
 */
void lv_knob_set_value(lv_obj_t * knob, int16_t value)
{
    lv_knob_ext_t * ext = lv_obj_get_ext_attr(knob);

    //if(needle_id >= ext->needle_count) return;
    if(ext->values[0] == value) return;


    int16_t min = lv_knob_get_min_value(knob);
    int16_t max = lv_knob_get_max_value(knob);

    if(value > max) value = max;
    else if(value < min) value = min;

    ext->values[0] = value;


    lv_obj_invalidate(knob);
}


/**
 * Set the scale settings of a knob
 * @param knob pointer to a knob object
 * @param angle angle of the scale (0..360)
 * @param line_cnt count of scale lines
 * @param label_cnt count of scale labels
 */
void lv_knob_set_scale(lv_obj_t * knob, uint16_t angle, uint8_t line_cnt, uint8_t label_cnt)
{
    lv_lmeter_set_scale(knob, angle, line_cnt);

    lv_knob_ext_t * ext = lv_obj_get_ext_attr(knob);
    ext->label_count = label_cnt;
    lv_obj_invalidate(knob);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a needle
 * @param knob pointer to knob object
 * @param needle the id of the needle
 * @return the value of the needle [min,max]
 */
int16_t lv_knob_get_value(lv_obj_t * knob)
{
    lv_knob_ext_t * ext = lv_obj_get_ext_attr(knob);
    int16_t min = lv_knob_get_min_value(knob);


    return ext->values[0];
}

/**
 * Get the count of needles on a knob
 * @param knob pointer to knob
 * @return count of needles
 */
uint8_t lv_knob_get_needle_count(lv_obj_t * knob)
{
    lv_knob_ext_t * ext = lv_obj_get_ext_attr(knob);
    return ext->needle_count;
}

/**
 * Set the number of labels (and the thicker lines too)
 * @param knob pointer to a knob object
 * @return count of labels
 */
uint8_t lv_knob_get_label_count(lv_obj_t * knob)
{
    lv_knob_ext_t * ext = lv_obj_get_ext_attr(knob);
    return ext->label_count;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the knobs
 * @param knob pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_knob_design(lv_obj_t * knob, const lv_area_t * mask, lv_design_mode_t mode)
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
        lv_style_t *style_ori_p = knob->style_p;
        lv_style_t *style = lv_obj_get_style(knob);
        lv_knob_ext_t *ext = lv_obj_get_ext_attr(knob);

        lv_knob_draw_scale(knob, mask);

        /*Draw the ancestor line meter with max value to show the rainbow like line colors*/
        //uint16_t line_cnt_tmp = ext->lmeter.line_cnt;
        //ancestor_design(knob, mask, mode);           /*To draw lines*/

        /*Temporally modify the line meter to draw thicker and longer lines where labels are*/
        //lv_style_t style_tmp;
        //lv_style_copy(&style_tmp, style);
        //ext->lmeter.line_cnt = ext->label_count;                        /*Only to labels*/
        //style_tmp.line.width = style_tmp.line.width * 2;                /*Ticker lines*/
        //style_tmp.body.padding.hor = style_tmp.body.padding.hor * 2;    /*Longer lines*/
        //knob->style_p = &style_tmp;

        //ancestor_design(knob, mask, mode);           /*To draw lines*/

        //ext->lmeter.line_cnt = line_cnt_tmp;          /*Restore the parameters*/
        //knob->style_p = style_ori_p;                 /*Restore the ORIGINAL style pointer*/

        lv_knob_draw_body(knob, mask);

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(knob, mask, mode);
    }

    return true;
}

/**
 * Signal function of the knob
 * @param knob pointer to a knob object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_knob_signal(lv_obj_t * knob, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(knob, sign, param);
    if(res != LV_RES_OK) return res;

    lv_knob_ext_t * ext = lv_obj_get_ext_attr(knob);
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
        buf->type[i] = "lv_knob";
    }

    return res;
}

/**
 * Draw the scale on a knob
 * @param knob pointer to knob object
 * @param mask mask of drawing
 */
static void lv_knob_draw_scale(lv_obj_t * knob, const lv_area_t * mask)
{
    char scale_txt[5];

    lv_knob_ext_t * ext = lv_obj_get_ext_attr(knob);
    lv_style_t * style = lv_obj_get_style(knob);
    lv_opa_t opa_scale = lv_obj_get_opa_scale(knob);
    lv_coord_t r = (lv_obj_get_width(knob) / 2);
    lv_coord_t x_ofs = lv_obj_get_width(knob) / 2 + knob->coords.x1;
    lv_coord_t y_ofs = lv_obj_get_height(knob) / 2 + knob->coords.y1;
    //int16_t scale_angle = lv_lmeter_get_scale_angle(knob);
    uint16_t label_num = ext->label_count;
    //int16_t angle_ofs = 90 + (360 - scale_angle) / 2;
    //int16_t min = lv_knob_get_min_value(knob);
    //int16_t max = lv_knob_get_max_value(knob);

    uint8_t i;
    for(i = 0; i < label_num; i++) {
        /*Calculate the position a scale label*/
        //int16_t angle = (i * scale_angle) / (label_num - 1) + angle_ofs;

        lv_coord_t y = y_ofs;

        lv_coord_t x = x_ofs;

        //int16_t scale_act = (int32_t)((int32_t)(max - min) * i) /  (label_num - 1);
        //scale_act += min;
        lv_math_num_to_str(ext->values[0], scale_txt);

        lv_area_t label_cord;
        lv_point_t label_size;
        lv_txt_get_size(&label_size, scale_txt, style->text.font,
                style->text.letter_space, style->text.line_space, LV_COORD_MAX, LV_TXT_FLAG_NONE);

        /*Draw the label*/
        label_cord.x1 = x - label_size.x / 2;
        label_cord.y1 = y - label_size.y / 2 - r;
        label_cord.x2 = label_cord.x1 + label_size.x;
        label_cord.y2 = label_cord.y1 + label_size.y - r;

        //lv_draw_label(&label_cord, mask, style, scale_txt, LV_TXT_FLAG_NONE, NULL);
        lv_draw_label(&label_cord, mask, style, opa_scale, scale_txt, LV_TXT_FLAG_NONE, NULL);
    }
}
/**
 * Draw the body of a knob
 * @param knob pointer to knob object
 * @param mask mask of drawing
 */
static void lv_knob_draw_body(lv_obj_t * knob, const lv_area_t * mask)
{

    lv_knob_ext_t * ext = lv_obj_get_ext_attr(knob);
    lv_style_t * style = lv_knob_get_style(knob);

    lv_coord_t r = lv_obj_get_width(knob) / 2 - style->body.padding.hor;
    lv_coord_t x_ofs = lv_obj_get_width(knob) / 2 + knob->coords.x1;
    lv_coord_t y_ofs = lv_obj_get_height(knob) / 2 + knob->coords.y1;
    uint16_t angle = lv_lmeter_get_scale_angle(knob);
    int16_t angle_ofs = 90 + (360 - angle) / 2;
    int16_t min = lv_knob_get_min_value(knob);
    int16_t max = lv_knob_get_max_value(knob);
    lv_point_t p_mid;
    lv_point_t p_end;
    uint8_t i;

    lv_style_t style_needle;
    lv_style_copy(&style_needle, style);

    /*Draw the knob body*/
    lv_style_t style_needle_mid;
    lv_style_copy(&style_needle_mid, &lv_style_plain);
    style_needle_mid.body.empty = 1;
    style_needle_mid.body.main_color = style->body.main_color;
    style_needle_mid.body.grad_color = style->body.main_color;
    style_needle_mid.body.border.color = style->body.border.color;
    style_needle_mid.body.border.width = style->body.border.width;
    style_needle_mid.body.radius = LV_RADIUS_CIRCLE;


    lv_area_t nm_cord;
    nm_cord.x1 = x_ofs - r;
    nm_cord.y1 = y_ofs - r;
    nm_cord.x2 = x_ofs + r;
    nm_cord.y2 = y_ofs + r;

    //lv_draw_rect(&nm_cord, mask, &style_needle_mid);
    lv_draw_rect(&nm_cord, mask, &style_needle_mid, lv_obj_get_opa_scale(knob));


    /*Draw the knob indicator*/
    lv_style_t style_needle_ind;
    lv_style_copy(&style_needle_ind, &lv_style_plain);
    style_needle_ind.body.main_color = LV_KNOB_DEF_NEEDLE_COLOR;
    style_needle_ind.body.grad_color = LV_KNOB_DEF_NEEDLE_COLOR;
    style_needle_ind.body.radius = style->body.radius;
    style_needle_ind.line.width = (lv_obj_get_width(knob) / 10);

    int16_t needle_angle = (ext->values[i] - min) * angle / (max - min) + angle_ofs;
    p_end.y = (lv_trigo_sin(needle_angle) * (r - style_needle_ind.line.width)) / LV_TRIGO_SIN_MAX + y_ofs;
    p_end.x = (lv_trigo_sin(needle_angle + 90) * (r - style_needle_ind.line.width)) / LV_TRIGO_SIN_MAX + x_ofs;

    //lv_area_t nm_cord;
    nm_cord.x1 = p_end.x - style_needle_ind.line.width;
    nm_cord.y1 = p_end.y - style_needle_ind.line.width;
    nm_cord.x2 = p_end.x + style_needle_ind.line.width;
    nm_cord.y2 = p_end.y + style_needle_ind.line.width;

    //lv_draw_rect(&nm_cord, mask, &style_needle_ind);
    lv_draw_rect(&nm_cord, mask, &style_needle_ind, lv_obj_get_opa_scale(knob));


//    p_mid.x = x_ofs;
//    p_mid.y = y_ofs;
//    for(i = 0; i < ext->needle_count; i++) {
//        /*Calculate the end point of a needle*/
//        int16_t needle_angle = (ext->values[i] - min) * angle / (max - min) + angle_ofs;
//        p_end.y = (lv_trigo_sin(needle_angle) * r) / LV_TRIGO_SIN_MAX + y_ofs;
//        p_end.x = (lv_trigo_sin(needle_angle + 90) * r) / LV_TRIGO_SIN_MAX + x_ofs;
//
//        /*Draw the needle with the corresponding color*/
//        if(ext->needle_colors == NULL) style_needle.line.color = LV_KNOB_DEF_NEEDLE_COLOR;
//        else style_needle.line.color = ext->needle_colors[i];
//
//        lv_draw_line(&p_mid, &p_end, mask, &style_needle);
//    }


}

#endif
