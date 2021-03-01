/**
* @file lv_gauge.c
*
*/

/*********************
 *      INCLUDES
 *********************/
#include "lv_gauge.h"
#if LV_USE_GAUGE != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_utils.h"
#include "lv_img.h"
#include <stdio.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_gauge"

#define LV_GAUGE_DEF_NEEDLE_COLOR LV_COLOR_RED
#define LV_GAUGE_DEF_LABEL_COUNT 6
#define LV_GAUGE_DEF_LINE_COUNT 21 /*Should be: ((label_cnt - 1) * internal_lines) + 1*/
#define LV_GAUGE_DEF_ANGLE 270

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_gauge_design(lv_obj_t * gauge, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_gauge_signal(lv_obj_t * gauge, lv_signal_t sign, void * param);
static lv_style_list_t * lv_gauge_get_style(lv_obj_t * gauge, uint8_t part);
static void lv_gauge_draw_labels(lv_obj_t * gauge, const lv_area_t * mask);
static void lv_gauge_draw_needle(lv_obj_t * gauge, const lv_area_t * clip_area);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_cb_t ancestor_design;
static lv_signal_cb_t ancestor_signal;

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
lv_obj_t * lv_gauge_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("gauge create started");

    /*Create the ancestor gauge*/
    lv_obj_t * gauge = lv_linemeter_create(par, copy);
    LV_ASSERT_MEM(gauge);
    if(gauge == NULL) return NULL;

    /*Allocate the gauge type specific extended data*/
    lv_gauge_ext_t * ext = lv_obj_allocate_ext_attr(gauge, sizeof(lv_gauge_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(gauge);
        return NULL;
    }

    /*Initialize the allocated 'ext' */
    ext->needle_count  = 0;
    ext->values        = NULL;
    ext->needle_colors = NULL;
    ext->label_count   = LV_GAUGE_DEF_LABEL_COUNT;
    ext->format_cb     = NULL;

    ext->needle_img = 0;
    ext->needle_img_pivot.x = 0;
    ext->needle_img_pivot.y = 0;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(gauge);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(gauge);

    lv_style_list_init(&ext->style_strong);
    lv_style_list_init(&ext->style_needle);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(gauge, lv_gauge_signal);
    lv_obj_set_design_cb(gauge, lv_gauge_design);

    /*Init the new gauge gauge*/
    if(copy == NULL) {
        lv_gauge_set_scale(gauge, LV_GAUGE_DEF_ANGLE, LV_GAUGE_DEF_LINE_COUNT, LV_GAUGE_DEF_LABEL_COUNT);
        lv_gauge_set_needle_count(gauge, 1, NULL);
        lv_gauge_set_critical_value(gauge, 80);

        lv_theme_apply(gauge, LV_THEME_GAUGE);

    }
    /*Copy an existing gauge*/
    else {
        lv_gauge_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        lv_gauge_set_needle_count(gauge, copy_ext->needle_count, copy_ext->needle_colors);

        lv_style_list_copy(&ext->style_strong, &copy_ext->style_strong);
        lv_style_list_copy(&ext->style_needle, &copy_ext->style_needle);

        uint8_t i;
        for(i = 0; i < ext->needle_count; i++) {
            ext->values[i] = copy_ext->values[i];
        }
        ext->label_count = copy_ext->label_count;
        ext->format_cb   = copy_ext->format_cb;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(gauge, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    LV_LOG_INFO("gauge created");

    return gauge;
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
void lv_gauge_set_needle_count(lv_obj_t * gauge, uint8_t needle_cnt, const lv_color_t colors[])
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);

    if(ext->needle_count != needle_cnt) {
        if(ext->values != NULL) {
            lv_mem_free(ext->values);
            ext->values = NULL;
        }

        ext->values = lv_mem_realloc(ext->values, needle_cnt * sizeof(ext->values[0]));
        LV_ASSERT_MEM(ext->values);
        if(ext->values == NULL) return;

        int16_t min = lv_gauge_get_min_value(gauge);
        uint8_t n;
        for(n = ext->needle_count; n < needle_cnt; n++) {
            ext->values[n] = min;
        }

        ext->needle_count = needle_cnt;
    }

    ext->needle_colors = colors;
    lv_obj_invalidate(gauge);
}

/**
 * Set the value of a needle
 * @param gauge pointer to a gauge
 * @param needle_id the id of the needle
 * @param value the new value
 */
void lv_gauge_set_value(lv_obj_t * gauge, uint8_t needle_id, int32_t value)
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);

    if(needle_id >= ext->needle_count) return;
    if(ext->values[needle_id] == value) return;

    int16_t min = lv_gauge_get_min_value(gauge);
    int16_t max = lv_gauge_get_max_value(gauge);

    if(value > max)
        value = max;
    else if(value < min)
        value = min;

    int32_t old_value = ext->values[needle_id];
    ext->values[needle_id] = value;

    lv_style_int_t pad = lv_obj_get_style_pad_inner(gauge, LV_GAUGE_PART_NEEDLE);
    lv_style_int_t left = lv_obj_get_style_pad_left(gauge, LV_GAUGE_PART_MAIN);
    lv_style_int_t right = lv_obj_get_style_pad_right(gauge, LV_GAUGE_PART_MAIN);
    lv_style_int_t top = lv_obj_get_style_pad_top(gauge, LV_GAUGE_PART_MAIN);
    lv_coord_t r      = (lv_obj_get_width(gauge) - left - right) / 2 - pad;
    lv_coord_t x_ofs  = gauge->coords.x1 + r + left + pad;
    lv_coord_t y_ofs  = gauge->coords.y1 + r + top + pad;
    uint16_t angle    = lv_linemeter_get_scale_angle(gauge);
    int16_t angle_ofs = 90 + (360 - angle) / 2 + lv_gauge_get_angle_offset(gauge);
    lv_point_t p_mid;
    lv_point_t p_end;
    lv_coord_t needle_w;

    if(ext->needle_img) {
        lv_img_header_t info;
        lv_img_decoder_get_info(ext->needle_img, &info);
        needle_w = info.h;
    }
    else {
        needle_w = lv_obj_get_style_line_width(gauge, LV_GAUGE_PART_NEEDLE);
    }

    p_mid.x = x_ofs;
    p_mid.y = y_ofs;
    /*Calculate the end point of a needle*/
    int16_t needle_angle = (old_value - min) * angle / (max - min) + angle_ofs;

    p_end.y = (_lv_trigo_sin(needle_angle) * r) / LV_TRIGO_SIN_MAX + y_ofs;
    p_end.x = (_lv_trigo_sin(needle_angle + 90) * r) / LV_TRIGO_SIN_MAX + x_ofs;

    lv_area_t a;
    a.x1 = LV_MATH_MIN(p_mid.x, p_end.x) - needle_w;
    a.y1 = LV_MATH_MIN(p_mid.y, p_end.y) - needle_w;
    a.x2 = LV_MATH_MAX(p_mid.x, p_end.x) + needle_w;
    a.y2 = LV_MATH_MAX(p_mid.y, p_end.y) + needle_w;
    lv_obj_invalidate_area(gauge, &a);

    needle_angle = (value - min) * angle / (max - min) + angle_ofs;
    p_end.y = (_lv_trigo_sin(needle_angle) * r) / LV_TRIGO_SIN_MAX + y_ofs;
    p_end.x = (_lv_trigo_sin(needle_angle + 90) * r) / LV_TRIGO_SIN_MAX + x_ofs;

    a.x1 = LV_MATH_MIN(p_mid.x, p_end.x) - needle_w;
    a.y1 = LV_MATH_MIN(p_mid.y, p_end.y) - needle_w;
    a.x2 = LV_MATH_MAX(p_mid.x, p_end.x) + needle_w;
    a.y2 = LV_MATH_MAX(p_mid.y, p_end.y) + needle_w;
    lv_obj_invalidate_area(gauge, &a);

    /*The image might have long "end" on the opposite side ant it also needs to be invalidated*/
    if(ext->needle_img) {
        needle_angle = (old_value - min) * angle / (max - min) + angle_ofs;
        needle_angle += 180;
        r = LV_MATH_MAX(ext->needle_img_pivot.x, ext->needle_img_pivot.y);

        p_end.y = (_lv_trigo_sin(needle_angle) * r) / LV_TRIGO_SIN_MAX + y_ofs;
        p_end.x = (_lv_trigo_sin(needle_angle + 90) * r) / LV_TRIGO_SIN_MAX + x_ofs;

        a.x1 = LV_MATH_MIN(p_mid.x, p_end.x) - needle_w;
        a.y1 = LV_MATH_MIN(p_mid.y, p_end.y) - needle_w;
        a.x2 = LV_MATH_MAX(p_mid.x, p_end.x) + needle_w;
        a.y2 = LV_MATH_MAX(p_mid.y, p_end.y) + needle_w;
        lv_obj_invalidate_area(gauge, &a);

        needle_angle = (value - min) * angle / (max - min) + angle_ofs;
        needle_angle += 180;
        p_end.y = (_lv_trigo_sin(needle_angle) * r) / LV_TRIGO_SIN_MAX + y_ofs;
        p_end.x = (_lv_trigo_sin(needle_angle + 90) * r) / LV_TRIGO_SIN_MAX + x_ofs;

        a.x1 = LV_MATH_MIN(p_mid.x, p_end.x) - needle_w;
        a.y1 = LV_MATH_MIN(p_mid.y, p_end.y) - needle_w;
        a.x2 = LV_MATH_MAX(p_mid.x, p_end.x) + needle_w;
        a.y2 = LV_MATH_MAX(p_mid.y, p_end.y) + needle_w;
        lv_obj_invalidate_area(gauge, &a);
    }

}

/**
 * Set the scale settings of a gauge
 * @param gauge pointer to a gauge object
 * @param angle angle of the scale (0..360)
 * @param line_cnt count of scale lines.
 * To get a given "subdivision" lines between labels:
 * `line_cnt = (sub_div + 1) * (label_cnt - 1) + 1 `
 * @param label_cnt count of scale labels.
 */
void lv_gauge_set_scale(lv_obj_t * gauge, uint16_t angle, uint8_t line_cnt, uint8_t label_cnt)
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_linemeter_set_scale(gauge, angle, line_cnt);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    ext->label_count     = label_cnt;
    lv_obj_invalidate(gauge);
}

/**
 * Set an image to display as needle(s).
 * The needle image should be horizontal and pointing to the right (`--->`).
 * @param gauge pointer to a gauge object
 * @param img_src pointer to an `lv_img_dsc_t` variable or a path to an image
 *        (not an `lv_img` object)
 * @param pivot_x the X coordinate of rotation center of the image
 * @param pivot_y the Y coordinate of rotation center of the image
 */
void lv_gauge_set_needle_img(lv_obj_t * gauge, const void * img, lv_coord_t pivot_x, lv_coord_t pivot_y)
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);

    ext->needle_img = img;
    ext->needle_img_pivot.x = pivot_x;
    ext->needle_img_pivot.y = pivot_y;

    lv_obj_invalidate(gauge);
}

/**
 * Assign a function to format gauge values
 * @param gauge pointer to a gauge object
 * @param format_cb pointer to function of lv_gauge_format_cb_t
 */
void lv_gauge_set_formatter_cb(lv_obj_t * gauge, lv_gauge_format_cb_t format_cb)
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);

    ext->format_cb = format_cb;
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
int32_t lv_gauge_get_value(const lv_obj_t * gauge, uint8_t needle)
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    int16_t min          = lv_gauge_get_min_value(gauge);

    if(needle >= ext->needle_count) return min;

    return ext->values[needle];
}

/**
 * Get the count of needles on a gauge
 * @param gauge pointer to gauge
 * @return count of needles
 */
uint8_t lv_gauge_get_needle_count(const lv_obj_t * gauge)
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    return ext->needle_count;
}

/**
 * Set the number of labels (and the thicker lines too)
 * @param gauge pointer to a gauge object
 * @return count of labels
 */
uint8_t lv_gauge_get_label_count(const lv_obj_t * gauge)
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    return ext->label_count;
}

/**
 * Get an image to display as needle(s).
 * @param gauge pointer to a gauge object
 * @return pointer to an `lv_img_dsc_t` variable or a path to an image
 *        (not an `lv_img` object). `NULL` if not used.
 */
const void * lv_gauge_get_needle_img(lv_obj_t * gauge)
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);

    return ext->needle_img;
}

/**
 * Get the X coordinate of the rotation center of the needle image
 * @param gauge pointer to a gauge object
 * @return the X coordinate of rotation center of the image
 */
lv_coord_t lv_gauge_get_needle_img_pivot_x(lv_obj_t * gauge)
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);

    return ext->needle_img_pivot.x;
}

/**
 * Get the Y coordinate of the rotation center of the needle image
 * @param gauge pointer to a gauge object
 * @return the X coordinate of rotation center of the image
 */
lv_coord_t lv_gauge_get_needle_img_pivot_y(lv_obj_t * gauge)
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);

    return ext->needle_img_pivot.y;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the gauges
 * @param gauge pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_gauge_design(lv_obj_t * gauge, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return LV_DESIGN_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design(gauge, clip_area, mode);

        lv_gauge_ext_t * ext           = lv_obj_get_ext_attr(gauge);
        lv_gauge_draw_labels(gauge, clip_area);

        /*Add the strong lines*/
        uint16_t line_cnt_tmp = ext->lmeter.line_cnt;
        ext->lmeter.line_cnt         = ext->label_count;                 /*Only to labels*/
        lv_linemeter_draw_scale(gauge, clip_area, LV_GAUGE_PART_MAJOR);
        ext->lmeter.line_cnt = line_cnt_tmp; /*Restore the parameters*/

        lv_gauge_draw_needle(gauge, clip_area);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(gauge, clip_area, mode);
    }

    return LV_DESIGN_RES_OK;
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
    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_gauge_get_style(gauge, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(gauge, sign, param);
    }

    /* Include the ancient signal function */
    res = ancestor_signal(gauge, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    if(sign == LV_SIGNAL_CLEANUP) {
        lv_mem_free(ext->values);
        ext->values = NULL;
        lv_obj_clean_style_list(gauge, LV_GAUGE_PART_NEEDLE);
        lv_obj_clean_style_list(gauge, LV_GAUGE_PART_MAJOR);
    }

    return res;
}
/**
 * Get the style descriptor of a part of the object
 * @param page pointer the object
 * @param part the part from `lv_gauge_part_t`. (LV_GAUGE_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_list_t * lv_gauge_get_style(lv_obj_t * gauge, uint8_t part)
{
    LV_ASSERT_OBJ(gauge, LV_OBJX_NAME);

    lv_gauge_ext_t * ext = lv_obj_get_ext_attr(gauge);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_GAUGE_PART_MAIN:
            style_dsc_p = &gauge->style_list;
            break;
        case LV_GAUGE_PART_MAJOR:
            style_dsc_p = &ext->style_strong;
            break;
        case LV_GAUGE_PART_NEEDLE:
            style_dsc_p = &ext->style_needle;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}
/**
 * Draw the scale on a gauge
 * @param gauge pointer to gauge object
 * @param mask mask of drawing
 */
static void lv_gauge_draw_labels(lv_obj_t * gauge, const lv_area_t * mask)
{
    lv_gauge_ext_t * ext     = lv_obj_get_ext_attr(gauge);
    lv_style_int_t scale_width = lv_obj_get_style_scale_width(gauge, LV_GAUGE_PART_MAJOR);
    lv_style_int_t left = lv_obj_get_style_pad_left(gauge, LV_GAUGE_PART_MAIN);
    lv_style_int_t right = lv_obj_get_style_pad_right(gauge, LV_GAUGE_PART_MAIN);
    lv_style_int_t top = lv_obj_get_style_pad_top(gauge, LV_GAUGE_PART_MAIN);
    lv_style_int_t txt_pad = lv_obj_get_style_pad_inner(gauge, LV_GAUGE_PART_MAIN);
    lv_coord_t r             = (lv_obj_get_width(gauge) - left - right) / 2 - scale_width - txt_pad;
    lv_coord_t x_ofs         = gauge->coords.x1 + r + left + scale_width + txt_pad;
    lv_coord_t y_ofs         = gauge->coords.y1 + r + top + scale_width + txt_pad;
    int16_t scale_angle      = lv_linemeter_get_scale_angle(gauge);
    uint16_t label_num       = ext->label_count;
    int16_t angle_ofs        = 90 + (360 - scale_angle) / 2 + lv_gauge_get_angle_offset(gauge);
    int32_t min              = lv_gauge_get_min_value(gauge);
    int32_t max              = lv_gauge_get_max_value(gauge);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(gauge, LV_GAUGE_PART_MAJOR, &label_dsc);

    uint8_t i;
    for(i = 0; i < label_num; i++) {
        /*Calculate the position a scale label*/
        int16_t angle = (i * scale_angle) / (label_num - 1) + angle_ofs;

        lv_coord_t y = (int32_t)((int32_t)_lv_trigo_sin(angle) * r) / LV_TRIGO_SIN_MAX;
        y += y_ofs;

        lv_coord_t x = (int32_t)((int32_t)_lv_trigo_sin(angle + 90) * r) / LV_TRIGO_SIN_MAX;
        x += x_ofs;

        int32_t scale_act = (int32_t)((int32_t)(max - min) * i) / (label_num - 1);
        scale_act += min;
        char scale_txt[16];
        if(ext->format_cb == NULL)
            _lv_utils_num_to_str(scale_act, scale_txt);
        else
            ext->format_cb(gauge, scale_txt, sizeof(scale_txt), scale_act);

        lv_area_t label_cord;
        lv_point_t label_size;
        _lv_txt_get_size(&label_size, scale_txt, label_dsc.font, label_dsc.letter_space, label_dsc.line_space,
                         LV_COORD_MAX, LV_TXT_FLAG_NONE);

        /*Draw the label*/
        label_cord.x1 = x - label_size.x / 2;
        label_cord.y1 = y - label_size.y / 2;
        label_cord.x2 = label_cord.x1 + label_size.x;
        label_cord.y2 = label_cord.y1 + label_size.y;

        lv_draw_label(&label_cord, mask, &label_dsc, scale_txt, NULL);
    }
}
/**
 * Draw the needles of a gauge
 * @param gauge pointer to gauge object
 * @param mask mask of drawing
 */
static void lv_gauge_draw_needle(lv_obj_t * gauge, const lv_area_t * clip_area)
{
    lv_gauge_ext_t * ext     = lv_obj_get_ext_attr(gauge);

    lv_style_int_t pad = lv_obj_get_style_pad_inner(gauge, LV_GAUGE_PART_NEEDLE);
    lv_style_int_t left = lv_obj_get_style_pad_left(gauge, LV_GAUGE_PART_MAIN);
    lv_style_int_t right = lv_obj_get_style_pad_right(gauge, LV_GAUGE_PART_MAIN);
    lv_style_int_t top = lv_obj_get_style_pad_top(gauge, LV_GAUGE_PART_MAIN);

    lv_coord_t r      = (lv_obj_get_width(gauge) - left - right) / 2 - pad;
    lv_coord_t x_ofs  = gauge->coords.x1 + r + left + pad;
    lv_coord_t y_ofs  = gauge->coords.y1 + r + top + pad;
    uint16_t angle    = lv_linemeter_get_scale_angle(gauge);
    int16_t angle_ofs = 90 + (360 - angle) / 2 + lv_gauge_get_angle_offset(gauge);
    int16_t min       = lv_gauge_get_min_value(gauge);
    int16_t max       = lv_gauge_get_max_value(gauge);
    lv_point_t p_mid;
    lv_point_t p_end;
    uint8_t i;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_img_dsc_t img_dsc;
    if(ext->needle_img == NULL) {
        lv_draw_line_dsc_init(&line_dsc);
        lv_obj_init_draw_line_dsc(gauge, LV_GAUGE_PART_NEEDLE, &line_dsc);
    }
    else {
        lv_draw_img_dsc_init(&img_dsc);
        lv_obj_init_draw_img_dsc(gauge, LV_GAUGE_PART_NEEDLE, &img_dsc);
    }

    p_mid.x = x_ofs;
    p_mid.y = y_ofs;
    for(i = 0; i < ext->needle_count; i++) {
        /*Calculate the end point of a needle*/
        int16_t needle_angle =
            (ext->values[i] - min) * angle / (max - min) + angle_ofs;

        /*Draw line*/
        if(ext->needle_img == NULL) {
            p_end.y = (_lv_trigo_sin(needle_angle) * r) / LV_TRIGO_SIN_MAX + y_ofs;
            p_end.x = (_lv_trigo_sin(needle_angle + 90) * r) / LV_TRIGO_SIN_MAX + x_ofs;

            /*Draw the needle with the corresponding color*/
            if(ext->needle_colors != NULL) line_dsc.color = ext->needle_colors[i];

            lv_draw_line(&p_mid, &p_end, clip_area, &line_dsc);
        }
        /*Draw image*/
        else {
            lv_img_header_t info;
            lv_img_decoder_get_info(ext->needle_img, &info);

            lv_area_t a;
            a.x1 = x_ofs - ext->needle_img_pivot.x;
            a.y1 = y_ofs - ext->needle_img_pivot.y;
            a.x2 = a.x1 + info.w - 1;
            a.y2 = a.y1 + info.h - 1;
            img_dsc.pivot.x = ext->needle_img_pivot.x;
            img_dsc.pivot.y = ext->needle_img_pivot.y;

            if(ext->needle_colors != NULL) img_dsc.recolor = ext->needle_colors[i];

            needle_angle = (needle_angle * 10);
            if(needle_angle > 3600) needle_angle -= 3600;
            img_dsc.angle = needle_angle;
            lv_draw_img(&a, clip_area, ext->needle_img, &img_dsc);
        }
    }

    lv_draw_rect_dsc_t mid_dsc;
    lv_draw_rect_dsc_init(&mid_dsc);
    lv_obj_init_draw_rect_dsc(gauge, LV_GAUGE_PART_NEEDLE, &mid_dsc);
    lv_style_int_t size = lv_obj_get_style_size(gauge, LV_GAUGE_PART_NEEDLE) / 2;
    lv_area_t nm_cord;
    nm_cord.x1 = x_ofs - size;
    nm_cord.y1 = y_ofs - size;
    nm_cord.x2 = x_ofs + size;
    nm_cord.y2 = y_ofs + size;
    lv_draw_rect(&nm_cord, clip_area, &mid_dsc);
}

#endif
