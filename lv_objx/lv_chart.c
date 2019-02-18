/**
 * @file lv_chart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_chart.h"
#if USE_LV_CHART != 0

#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#define LV_CHART_YMIN_DEF   0
#define LV_CHART_YMAX_DEF   100
#define LV_CHART_HDIV_DEF   3
#define LV_CHART_VDIV_DEF   5
#define LV_CHART_PNUM_DEF   10

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_chart_design(lv_obj_t * chart, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_chart_signal(lv_obj_t * chart, lv_signal_t sign, void * param);
static void lv_chart_draw_div(lv_obj_t * chart, const lv_area_t * mask);
static void lv_chart_draw_lines(lv_obj_t * chart, const lv_area_t * mask);
static void lv_chart_draw_points(lv_obj_t * chart, const lv_area_t * mask);
static void lv_chart_draw_cols(lv_obj_t * chart, const lv_area_t * mask);
static void lv_chart_draw_vertical_lines(lv_obj_t * chart, const lv_area_t * mask);

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
 * Create a chart background objects
 * @param par pointer to an object, it will be the parent of the new chart background
 * @param copy pointer to a chart background object, if not NULL then the new object will be copied from it
 * @return pointer to the created chart background
 */
lv_obj_t * lv_chart_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("chart create started");

    /*Create the ancestor basic object*/
    lv_obj_t * new_chart = lv_obj_create(par, copy);
    lv_mem_assert(new_chart);
    if(new_chart == NULL) return NULL;

    /*Allocate the object type specific extended data*/
    lv_chart_ext_t * ext = lv_obj_allocate_ext_attr(new_chart, sizeof(lv_chart_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    lv_ll_init(&ext->series_ll, sizeof(lv_chart_series_t));
    ext->series.num = 0;
    ext->ymin = LV_CHART_YMIN_DEF;
    ext->ymax = LV_CHART_YMAX_DEF;
    ext->hdiv_cnt = LV_CHART_HDIV_DEF;
    ext->vdiv_cnt = LV_CHART_VDIV_DEF;
    ext->point_cnt = LV_CHART_PNUM_DEF;
    ext->type = LV_CHART_TYPE_LINE;
    ext->series.opa = LV_OPA_COVER;
    ext->series.dark = LV_OPA_50;
    ext->series.width = 2;

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_func(new_chart);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_chart);

    lv_obj_set_signal_func(new_chart, lv_chart_signal);
    lv_obj_set_design_func(new_chart, lv_chart_design);

    /*Init the new chart background object*/
    if(copy == NULL) {
        lv_obj_set_size(new_chart, LV_HOR_RES / 3, LV_VER_RES / 3);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_chart_set_style(new_chart, th->chart);
        } else {
            lv_chart_set_style(new_chart, &lv_style_pretty);
        }

    } else {
        lv_chart_ext_t * ext_copy = lv_obj_get_ext_attr(copy);
        ext->type = ext_copy->type;
        ext->ymin = ext_copy->ymin;
        ext->ymax = ext_copy->ymax;
        ext->hdiv_cnt = ext_copy->hdiv_cnt;
        ext->vdiv_cnt = ext_copy->vdiv_cnt;
        ext->point_cnt = ext_copy->point_cnt;
        ext->series.opa =  ext_copy->series.opa;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_chart);
    }

    LV_LOG_INFO("chart created");


    return new_chart;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Allocate and add a data series to the chart
 * @param chart pointer to a chart object
 * @param color color of the data series
 * @return pointer to the allocated data series
 */
lv_chart_series_t * lv_chart_add_series(lv_obj_t * chart, lv_color_t color)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    lv_chart_series_t * ser = lv_ll_ins_head(&ext->series_ll);
    lv_mem_assert(ser);
    if(ser == NULL) return NULL;

    lv_coord_t def = LV_CHART_POINT_DEF;

    if(ser == NULL) return NULL;

    ser->color = color;

    ser->points = lv_mem_alloc(sizeof(lv_coord_t) * ext->point_cnt);
    lv_mem_assert(ser->points);
    if(ser->points == NULL) {
        lv_ll_rem(&ext->series_ll, ser);
        lv_mem_free(ser);
        return NULL;
    }

    ser->start_point = 0;

    uint16_t i;
    lv_coord_t * p_tmp = ser->points;
    for(i = 0; i < ext->point_cnt; i++) {
        *p_tmp = def;
        p_tmp++;
    }

    ext->series.num++;

    return ser;
}

/**
 * Clear the point of a serie
 * @param chart pointer to a chart object
 * @param serie pointer to the chart's serie to clear
 */
void lv_chart_clear_serie(lv_obj_t * chart, lv_chart_series_t * serie)
{
    if(chart == NULL || serie == NULL)
        return;
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext == NULL) return;

    uint32_t i;
    for(i = 0; i < ext->point_cnt; i++)
    {
        serie->points[i] = LV_CHART_POINT_DEF;
    }

    serie->start_point = 0;

}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of horizontal and vertical division lines
 * @param chart pointer to a graph background object
 * @param hdiv number of horizontal division lines
 * @param vdiv number of vertical division lines
 */
void lv_chart_set_div_line_count(lv_obj_t * chart, uint8_t hdiv, uint8_t vdiv)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->hdiv_cnt == hdiv && ext->vdiv_cnt == vdiv) return;

    ext->hdiv_cnt = hdiv;
    ext->vdiv_cnt = vdiv;

    lv_obj_invalidate(chart);
}

/**
 * Set the minimal and maximal y values
 * @param chart pointer to a graph background object
 * @param ymin y minimum value
 * @param ymax y maximum value
 */
void lv_chart_set_range(lv_obj_t * chart, lv_coord_t ymin, lv_coord_t ymax)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->ymin == ymin && ext->ymax == ymax) return;

    ext->ymin = ymin;
    ext->ymax = ymax;

    lv_chart_refresh(chart);
}

/**
 * Set a new type for a chart
 * @param chart pointer to a chart object
 * @param type new type of the chart (from 'lv_chart_type_t' enum)
 */
void lv_chart_set_type(lv_obj_t * chart, lv_chart_type_t type)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->type == type) return;

    ext->type = type;

    lv_chart_refresh(chart);
}

/**
 * Set the number of points on a data line on a chart
 * @param chart pointer r to chart object
 * @param point_cnt new number of points on the data lines
 */
void lv_chart_set_point_count(lv_obj_t * chart, uint16_t point_cnt)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->point_cnt == point_cnt) return;

    lv_chart_series_t * ser;
    uint16_t point_cnt_old = ext->point_cnt;
    uint16_t i;
    lv_coord_t def = LV_CHART_POINT_DEF;

    if(point_cnt < 1) point_cnt = 1;

    LL_READ_BACK(ext->series_ll, ser) {
        if(ser->start_point != 0) {
            lv_coord_t * new_points = lv_mem_alloc(sizeof(lv_coord_t) * point_cnt);
            lv_mem_assert(new_points);
            if(new_points == NULL) return;

            if(point_cnt >= point_cnt_old) {
                for(i = 0; i < point_cnt_old; i++) {
                    new_points[i] = ser->points[(i + ser->start_point) % point_cnt_old];    /*Copy old contents to new array*/
                }
                for(i = point_cnt_old; i < point_cnt; i++) {
                    new_points[i] = def;                                                    /*Fill up the rest with default value*/
                }
            } else {
                for(i = 0; i < point_cnt; i++) {
                    new_points[i] = ser->points[(i + ser->start_point) % point_cnt_old];    /*Copy old contents to new array*/
                }
            }

            /*Switch over pointer from old to new*/
            lv_mem_free(ser->points);
            ser->points = new_points;
        } else {
            ser->points = lv_mem_realloc(ser->points, sizeof(lv_coord_t) * point_cnt);
            lv_mem_assert(ser->points);
            if(ser->points == NULL) return;
            /*Initialize the new points*/
            if(point_cnt > point_cnt_old) {
                for(i = point_cnt_old - 1; i < point_cnt; i++) {
                    ser->points[i] = def;
                }
            }
        }

        ser->start_point = 0;
    }

    ext->point_cnt = point_cnt;

    lv_chart_refresh(chart);
}

/**
 * Set the opacity of the data series
 * @param chart pointer to a chart object
 * @param opa opacity of the data series
 */
void lv_chart_set_series_opa(lv_obj_t * chart, lv_opa_t opa)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->series.opa == opa) return;

    ext->series.opa = opa;
    lv_obj_invalidate(chart);
}

/**
 * Set the line width or point radius of the data series
 * @param chart pointer to a chart object
 * @param width the new width
 */
void lv_chart_set_series_width(lv_obj_t * chart, lv_coord_t width)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->series.width == width) return;

    ext->series.width = width;
    lv_obj_invalidate(chart);
}
/**
 * Set the dark effect on the bottom of the points or columns
 * @param chart pointer to a chart object
 * @param dark_eff dark effect level (LV_OPA_TRANSP to turn off)
 */
void lv_chart_set_series_darking(lv_obj_t * chart, lv_opa_t dark_eff)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->series.dark == dark_eff) return;

    ext->series.dark = dark_eff;
    lv_obj_invalidate(chart);
}

/**
 * Initialize all data points with a value
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y the new value  for all points
 */
void lv_chart_init_points(lv_obj_t * chart, lv_chart_series_t * ser, lv_coord_t y)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    uint16_t i;
    for(i = 0; i < ext->point_cnt; i++) {
        ser->points[i] = y;
    }
    ser->start_point = 0;
    lv_chart_refresh(chart);
}

/**
 * Set the value s of points from an array
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y_array array of 'lv_coord_t' points (with 'points count' elements )
 */
void lv_chart_set_points(lv_obj_t * chart, lv_chart_series_t * ser, lv_coord_t * y_array)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    memcpy(ser->points, y_array, ext->point_cnt * (sizeof(lv_coord_t)));
    ser->start_point = 0;
    lv_chart_refresh(chart);
}

/**
 * Shift all data left and set the rightmost data on a data line
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y the new value of the rightmost data
 */
void lv_chart_set_next(lv_obj_t * chart, lv_chart_series_t * ser, lv_coord_t y)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    ser->points[ser->start_point] = y;  /*This was the place of the former left most value, after shifting it is the rightmost*/
    ser->start_point = (ser->start_point + 1) % ext->point_cnt;

    lv_chart_refresh(chart);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the type of a chart
 * @param chart pointer to chart object
 * @return type of the chart (from 'lv_chart_t' enum)
 */
lv_chart_type_t lv_chart_get_type(const lv_obj_t * chart)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    return ext->type;
}

/**
 * Get the data point number per data line on chart
 * @param chart pointer to chart object
 * @return point number on each data line
 */
uint16_t lv_chart_get_point_cnt(const lv_obj_t * chart)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    return ext->point_cnt;
}

/**
 * Get the opacity of the data series
 * @param chart pointer to chart object
 * @return the opacity of the data series
 */
lv_opa_t lv_chart_get_series_opa(const lv_obj_t * chart)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    return ext->series.opa;
}

/**
 * Get the data series width
 * @param chart pointer to chart object
 * @return the width the data series (lines or points)
 */
lv_coord_t lv_chart_get_series_width(const lv_obj_t * chart)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    return ext->series.width;
}

/**
 * Get the dark effect level on the bottom of the points or columns
 * @param chart pointer to chart object
 * @return dark effect level (LV_OPA_TRANSP to turn off)
 */
lv_opa_t lv_chart_get_series_darking(const lv_obj_t * chart)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    return ext->series.dark;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Refresh a chart if its data line has changed
 * @param chart pointer to chart object
 */
void lv_chart_refresh(lv_obj_t * chart)
{
    lv_obj_invalidate(chart);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the chart backgrounds
 * @param chart pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_chart_design(lv_obj_t * chart, const lv_area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask_p area*/
        return ancestor_design_f(chart, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
        /*Draw the background*/
        lv_draw_rect(&chart->coords, mask, lv_obj_get_style(chart), lv_obj_get_opa_scale(chart));

        lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

        lv_chart_draw_div(chart, mask);

        if(ext->type & LV_CHART_TYPE_LINE) lv_chart_draw_lines(chart, mask);
        if(ext->type & LV_CHART_TYPE_COLUMN) lv_chart_draw_cols(chart, mask);
        if(ext->type & LV_CHART_TYPE_POINT) lv_chart_draw_points(chart, mask);
        if(ext->type & LV_CHART_TYPE_VERTICAL_LINE) lv_chart_draw_vertical_lines(chart, mask);
    }
    return true;
}

/**
 * Signal function of the chart background
 * @param chart pointer to a chart background object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
static lv_res_t lv_chart_signal(lv_obj_t * chart, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(chart, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_CLEANUP) {
        lv_coord_t ** datal;
        lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
        LL_READ(ext->series_ll, datal) {
            lv_mem_free(*datal);
        }
        lv_ll_clear(&ext->series_ll);
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_chart";
    }

    return res;
}

/**
 * Draw the division lines on chart background
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void lv_chart_draw_div(lv_obj_t * chart, const lv_area_t * mask)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    lv_style_t * style = lv_obj_get_style(chart);
    lv_opa_t opa_scale = lv_obj_get_opa_scale(chart);

    uint8_t div_i;
    uint8_t div_i_end;
    uint8_t div_i_start;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t w = lv_obj_get_width(chart);
    lv_coord_t h = lv_obj_get_height(chart);
    lv_coord_t x_ofs = chart->coords.x1;
    lv_coord_t y_ofs = chart->coords.y1;

    if(ext->hdiv_cnt != 0) {
        /*Draw slide lines if no border*/
        if(style->body.border.width != 0) {
            div_i_start = 1;
            div_i_end = ext->hdiv_cnt;
        } else {
            div_i_start = 0;
            div_i_end = ext->hdiv_cnt + 1;
        }

        p1.x = 0 + x_ofs;
        p2.x = w + x_ofs;
        for(div_i = div_i_start; div_i <= div_i_end; div_i++) {
            p1.y = (int32_t)((int32_t)h * div_i) / (ext->hdiv_cnt + 1);
            p1.y +=  y_ofs;
            if(div_i == div_i_start) p1.y += (style->line.width >> 1) + 1;  /*The first line might not be visible*/
            if(div_i == div_i_end) p1.y -= (style->line.width >> 1) + 1;  /*The last line might not be visible*/

            p2.y = p1.y;
            lv_draw_line(&p1, &p2, mask, style, opa_scale);
        }
    }

    if(ext->vdiv_cnt != 0) {
        /*Draw slide lines if no border*/
        if(style->body.border.width != 0) {
            div_i_start = 1;
            div_i_end = ext->vdiv_cnt;
        } else {
            div_i_start = 0;
            div_i_end = ext->vdiv_cnt + 1;
        }

        p1.y = 0 + y_ofs;
        p2.y = h + y_ofs;
        for(div_i = div_i_start; div_i <= div_i_end; div_i ++) {
            p1.x = (int32_t)((int32_t)w * div_i) / (ext->vdiv_cnt + 1);
            p1.x +=  x_ofs;
            if(div_i == div_i_start) p1.x += (style->line.width >> 1) + 1;  /*The first line might not be visible*/
            if(div_i == div_i_end) p1.x -= (style->line.width >> 1) + 1;  /*The last line might not be visible*/
            p2.x = p1.x;
            lv_draw_line(&p1, &p2, mask, style, opa_scale);
        }
    }
}

/**
 * Draw the data lines as lines on a chart
 * @param obj pointer to chart object
 */
static void lv_chart_draw_lines(lv_obj_t * chart, const lv_area_t * mask)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    uint16_t i;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t w = lv_obj_get_width(chart);
    lv_coord_t h = lv_obj_get_height(chart);
    lv_coord_t x_ofs = chart->coords.x1;
    lv_coord_t y_ofs = chart->coords.y1;
    int32_t y_tmp;
    lv_coord_t p_prev;
    lv_coord_t p_act;
    lv_chart_series_t * ser;
    lv_opa_t opa_scale = lv_obj_get_opa_scale(chart);
    lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.line.opa = ext->series.opa;
    style.line.width = ext->series.width;

    /*Go through all data lines*/
    LL_READ_BACK(ext->series_ll, ser) {
        style.line.color = ser->color;

        p1.x = 0 + x_ofs;
        p2.x = 0 + x_ofs;

        p_prev = ser->start_point;
        y_tmp = (int32_t)((int32_t) ser->points[p_prev] - ext->ymin) * h;
        y_tmp = y_tmp / (ext->ymax - ext->ymin);
        p2.y = h - y_tmp + y_ofs;

        for(i = 1; i < ext->point_cnt; i ++) {
            p1.x = p2.x;
            p1.y = p2.y;

            p2.x = ((w * i) / (ext->point_cnt - 1)) + x_ofs;

            p_act = (ser->start_point + i) % ext->point_cnt;

            y_tmp = (int32_t)((int32_t) ser->points[p_act] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);
            p2.y = h - y_tmp + y_ofs;

            if(ser->points[p_prev] != LV_CHART_POINT_DEF && ser->points[p_act] != LV_CHART_POINT_DEF)
                lv_draw_line(&p1, &p2, mask, &style, opa_scale);

            p_prev = p_act;
        }
    }
}

/**
 * Draw the data lines as points on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void lv_chart_draw_points(lv_obj_t * chart, const lv_area_t * mask)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    uint16_t i;
    lv_area_t cir_a;
    lv_coord_t w = lv_obj_get_width(chart);
    lv_coord_t h = lv_obj_get_height(chart);
    lv_coord_t x_ofs = chart->coords.x1;
    lv_coord_t y_ofs = chart->coords.y1;
    int32_t y_tmp;
    lv_coord_t p_act;
    lv_chart_series_t * ser;
    uint8_t series_cnt = 0;
    lv_style_t style_point;
    lv_style_copy(&style_point, &lv_style_plain);

    style_point.body.border.width = 0;
    style_point.body.empty = 0;
    style_point.body.radius = LV_RADIUS_CIRCLE;
    style_point.body.opa = ext->series.opa;
    style_point.body.radius = ext->series.width;

    /*Go through all data lines*/
    LL_READ_BACK(ext->series_ll, ser) {
        style_point.body.main_color = ser->color;
        style_point.body.grad_color = lv_color_mix(LV_COLOR_BLACK, ser->color, ext->series.dark);

        for(i = 0; i < ext->point_cnt; i ++) {
            cir_a.x1 = ((w * i) / (ext->point_cnt - 1)) + x_ofs;
            cir_a.x2 = cir_a.x1 + style_point.body.radius;
            cir_a.x1 -= style_point.body.radius;
            p_act = (ser->start_point + i) % ext->point_cnt;
            y_tmp = (int32_t)((int32_t) ser->points[p_act] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);
            cir_a.y1 = h - y_tmp + y_ofs;
            cir_a.y2 = cir_a.y1 + style_point.body.radius;
            cir_a.y1 -= style_point.body.radius;

            if(ser->points[p_act] != LV_CHART_POINT_DEF)
                lv_draw_rect(&cir_a, mask, &style_point, lv_obj_get_opa_scale(chart));
        }
        series_cnt++;
    }
}

/**
 * Draw the data lines as columns on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void lv_chart_draw_cols(lv_obj_t * chart, const lv_area_t * mask)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    uint16_t i;
    lv_area_t col_a;
    lv_area_t col_mask;
    bool mask_ret;
    lv_coord_t w = lv_obj_get_width(chart);
    lv_coord_t h = lv_obj_get_height(chart);
    int32_t y_tmp;
    lv_chart_series_t * ser;
    lv_style_t rects;
    lv_coord_t col_w = w / ((ext->series.num + 1) * ext->point_cnt); /* Suppose + 1 series as separator*/
    lv_coord_t x_ofs = col_w / 2; /*Shift with a half col.*/

    lv_style_copy(&rects, &lv_style_plain);
    rects.body.border.width = 0;
    rects.body.empty = 0;
    rects.body.radius = 0;
    rects.body.opa = ext->series.opa;

    col_a.y2 = chart->coords.y2;

    lv_coord_t x_act;

    /*Go through all points*/
    for(i = 0; i < ext->point_cnt; i ++) {
        x_act = (int32_t)((int32_t) w * i) / ext->point_cnt;
        x_act += chart->coords.x1 + x_ofs;

        /*Draw the current point of all data line*/
        LL_READ_BACK(ext->series_ll, ser) {
            rects.body.main_color = ser->color;
            rects.body.grad_color = lv_color_mix(LV_COLOR_BLACK, ser->color, ext->series.dark);
            col_a.x1 = x_act;
            col_a.x2 = col_a.x1 + col_w;
            x_act += col_w;

            lv_coord_t p_act = (ser->start_point + i) % ext->point_cnt;
            y_tmp = (int32_t)((int32_t) ser->points[p_act] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);
            col_a.y1 = h - y_tmp + chart->coords.y1;

            mask_ret = lv_area_intersect(&col_mask, mask, &col_a);
            if(mask_ret != false && ser->points[p_act] != LV_CHART_POINT_DEF) {
                lv_draw_rect(&chart->coords, &col_mask, &rects, lv_obj_get_opa_scale(chart));
            }
        }
    }
}

/**
 * Draw the data lines as vertical lines on a chart if there is only 1px between point
 * @param obj pointer to chart object
 */
static void lv_chart_draw_vertical_lines(lv_obj_t * chart, const lv_area_t * mask)
{

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    lv_coord_t w = lv_obj_get_width(chart);
    /*Vertical lines works only if the width == point count. Else use the normal line type*/
    if(ext->point_cnt != w) {
        lv_chart_draw_lines(chart, mask);
        return;
    }

    uint16_t i;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t h = lv_obj_get_height(chart);
    lv_coord_t x_ofs = chart->coords.x1;
    lv_coord_t y_ofs = chart->coords.y1;
    int32_t y_tmp;
    lv_chart_series_t * ser;
    lv_opa_t opa_scale = lv_obj_get_opa_scale(chart);
    lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.line.opa = ext->series.opa;
    style.line.width = ext->series.width;

    /*Go through all data lines*/
    LL_READ_BACK(ext->series_ll, ser) {
        style.line.color = ser->color;

        p1.x = 0 + x_ofs;
        p2.x = 0 + x_ofs;
        y_tmp = (int32_t)((int32_t) ser->points[0] - ext->ymin) * h;
        y_tmp = y_tmp / (ext->ymax - ext->ymin);
        p2.y = h - y_tmp + y_ofs;
        p1.y = p2.y;

        for(i = 0; i < ext->point_cnt; i++)
        {

            y_tmp = (int32_t)((int32_t) ser->points[i] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);
            p2.y = h - y_tmp + y_ofs;

            if(p1.y == p2.y)
            {
                p2.x++;
            }

            if(ser->points[i] != LV_CHART_POINT_DEF) {
                lv_draw_line(&p1, &p2, mask, &style, opa_scale);
            }

            p2.x = ((w * i) / (ext->point_cnt - 1)) + x_ofs;
            p1.x = p2.x;
            p1.y = p2.y;
        }
    }
}
#endif
