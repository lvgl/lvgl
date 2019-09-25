/**
 * @file lv_cpicker.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_cpicker.h"
#if LV_USE_CPICKER != 0

#include "../lv_draw/lv_draw_arc.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_core/lv_indev.h"
#include "../lv_core/lv_refr.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_CPICKER_DEF_TYPE
#define LV_CPICKER_DEF_TYPE LV_CPICKER_TYPE_DISC
#endif

#ifndef LV_CPICKER_DEF_HUE
#define LV_CPICKER_DEF_HUE 0
#endif

#ifndef LV_CPICKER_DEF_SATURATION
#define LV_CPICKER_DEF_SATURATION 100
#endif

#ifndef LV_CPICKER_DEF_VALUE
#define LV_CPICKER_DEF_VALUE 100
#endif

#ifndef LV_CPICKER_DEF_INDICATOR_TYPE
#define LV_CPICKER_DEF_INDICATOR_TYPE LV_CPICKER_INDICATOR_CIRCLE
#endif

#ifndef LV_CPICKER_DEF_QF /*quantization factor*/
#define LV_CPICKER_DEF_QF 1
#endif

/*for rectangular mode the QF can be down to 1*/
/*
#define LV_CPICKER_MINIMUM_QF 4
#if LV_CPICKER_DEF_QF < LV_CPICKER_MINIMUM_QF
#undef LV_CPICKER_DEF_QF
#define LV_CPICKER_DEF_QF LV_CPICKER_MINIMUM_QF
#endif
 */

#ifndef LV_CPICKER_USE_TRI /*Use triangle approximation instead of arc*/
#define LV_CPICKER_USE_TRI 1
#endif

#define TRI_OFFSET 4

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_cpicker_disc_design(lv_obj_t * cpicker, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_cpicker_disc_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param);

static bool lv_cpicker_rect_design(lv_obj_t * cpicker, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_cpicker_rect_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param);

static void lv_cpicker_invalidate(lv_obj_t * cpicker, bool all);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a color_picker object
 * @param par pointer to an object, it will be the parent of the new color_picker
 * @param copy pointer to a color_picker object, if not NULL then the new object will be copied from it
 * @return pointer to the created color_picker
 */
lv_obj_t * lv_cpicker_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("color_picker create started");

    lv_obj_t * new_cpicker = lv_obj_create(par, copy);
    lv_mem_assert(new_cpicker);
    if(new_cpicker == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_cpicker);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(new_cpicker);

    /*Allocate the extended data*/
    lv_cpicker_ext_t * ext = lv_obj_allocate_ext_attr(new_cpicker, sizeof(lv_cpicker_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */
    ext->hue = LV_CPICKER_DEF_HUE;
    ext->prev_hue = ext->hue;
    ext->saturation = LV_CPICKER_DEF_SATURATION;
    ext->value = LV_CPICKER_DEF_VALUE;
    ext->indicator.style = &lv_style_plain;
    ext->indicator.type = LV_CPICKER_DEF_INDICATOR_TYPE;
    ext->color_mode = LV_CPICKER_COLOR_MODE_HUE;
    ext->color_mode_fixed = 0;
    ext->last_click = 0;
    ext->type = LV_CPICKER_DEF_TYPE;

    /*The signal and design functions are not copied so set them here*/
    if(ext->type == LV_CPICKER_TYPE_DISC)
    {
        lv_obj_set_signal_cb(new_cpicker, lv_cpicker_disc_signal);
        lv_obj_set_design_cb(new_cpicker, lv_cpicker_disc_design);
    }
    else if(ext->type == LV_CPICKER_TYPE_RECT)
    {
        lv_obj_set_signal_cb(new_cpicker, lv_cpicker_rect_signal);
        lv_obj_set_design_cb(new_cpicker, lv_cpicker_rect_design);
    }

    /*If no copy do the basic initialization*/
    if(copy == NULL) {
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_cpicker_set_style(new_cpicker, LV_CPICKER_STYLE_MAIN, th->style.bg);
        } else {
            lv_cpicker_set_style(new_cpicker, LV_CPICKER_STYLE_MAIN, &lv_style_plain);
        }
    }
    /*Copy 'copy'*/
    else {
        lv_cpicker_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_cpicker);
    }

    LV_LOG_INFO("color_picker created");

    return new_cpicker;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_cpicker_set_style(lv_obj_t * cpicker, lv_cpicker_style_t type, lv_style_t * style)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    switch(type) {
    case LV_CPICKER_STYLE_MAIN:
        lv_obj_set_style(cpicker, style);
        break;
    case LV_CPICKER_STYLE_INDICATOR:
        ext->indicator.style = style;
        lv_obj_invalidate(cpicker);
        break;
    }
}

/**
 * Set a type of a colorpicker indicator.
 * @param cpicker pointer to colorpicker object
 * @param type indicator type
 */
void lv_cpicker_set_indicator_type(lv_obj_t * cpicker, lv_cpicker_indicator_type_t type)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    ext->indicator.type = type;
    lv_obj_invalidate(cpicker);
}

/**
 * Set the current hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param hue current selected hue [0..360]
 */
void lv_cpicker_set_hue(lv_obj_t * cpicker, uint16_t hue)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    ext->hue = hue % 360;

    lv_obj_invalidate(cpicker);
}

/**
 * Set the current saturation of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param saturation current selected saturation [0..100]
 */
void lv_cpicker_set_saturation(lv_obj_t * cpicker, uint8_t saturation)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    ext->saturation = saturation % 100;

    lv_obj_invalidate(cpicker);
}

/**
 * Set the current value of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param val current selected value [0..100]
 */
void lv_cpicker_set_value(lv_obj_t * cpicker, uint8_t val)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    ext->value = val % 100;

    lv_obj_invalidate(cpicker);
}

/**
 * Set the current color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param color current selected color
 */
void lv_cpicker_set_color(lv_obj_t * cpicker, lv_color_t color)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    lv_color_hsv_t hsv = lv_color_rgb_to_hsv(color.ch.red, color.ch.green, color.ch.blue);
    ext->hue = hsv.h;

    lv_obj_invalidate(cpicker);
}

/**
 * Set the current color mode.
 * @param cpicker pointer to colorpicker object
 * @param mode color mode (hue/sat/val)
 */
void lv_cpicker_set_color_mode(lv_obj_t * cpicker, lv_cpicker_color_mode_t mode)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    ext->color_mode = mode;
}

/**
 * Set if the color mode is changed on long press on center
 * @param cpicker pointer to colorpicker object
 * @param fixed color mode cannot be changed on long press
 */
void lv_cpicker_set_color_mode_fixed(lv_obj_t * cpicker, bool fixed)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    ext->color_mode_fixed = fixed;
}

/*=====================
 * Getter functions
 *====================*/

/** 
 * Get the current color mode.
 * @param cpicker pointer to colorpicker object
 * @return color mode (hue/sat/val)
 */
lv_cpicker_color_mode_t lv_cpicker_get_color_mode(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->color_mode;
}

/**
 * Get if the color mode is changed on long press on center
 * @param cpicker pointer to colorpicker object
 * @return mode cannot be changed on long press
 */
bool lv_cpicker_get_color_mode_fixed(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->color_mode_fixed;
}

/**
 * Get style of a color_picker.
 * @param cpicker pointer to color_picker object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_cpicker_get_style(const lv_obj_t * cpicker, lv_cpicker_style_t type)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    switch(type) {
    case LV_CPICKER_STYLE_MAIN:
        return lv_obj_get_style(cpicker);
    case LV_CPICKER_STYLE_INDICATOR:
        return ext->indicator.style;
    default:
        return NULL;
    }

    /*To avoid warning*/
    return NULL;
}

/**
 * Get the current hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return hue current selected hue
 */
uint16_t lv_cpicker_get_hue(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->hue;
}

/**
 * Get the current saturation of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected saturation
 */
uint8_t lv_cpicker_get_saturation(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->saturation;
}

/**
 * Get the current hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected value
 */
uint8_t lv_cpicker_get_value(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->value;
}

/**
 * Get the current selected color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return color current selected color
 */
lv_color_t lv_cpicker_get_color(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return lv_color_hsv_to_rgb(ext->hue, ext->saturation, ext->value);
}

/*=====================
 * Other functions
 *====================*/

/*
 * New object specific "other" functions come here
 */

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the color_pickerwhen when wheel type
 * @param cpicker pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_cpicker_disc_design(lv_obj_t * cpicker, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {

        lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
        lv_style_t * style = lv_cpicker_get_style(cpicker, LV_CPICKER_STYLE_MAIN);

        static lv_style_t styleCopy;
        lv_style_copy(&styleCopy, style);

        static lv_style_t styleCenterBackground;
        lv_theme_t * th = lv_theme_get_current();
        if (th) {
            lv_style_copy(&styleCenterBackground, th->style.bg);
        } else {
            lv_style_copy(&styleCenterBackground, &lv_style_plain);
        }

        lv_coord_t r = (LV_MATH_MIN(lv_obj_get_width(cpicker), lv_obj_get_height(cpicker))) / 2;
        lv_coord_t x = cpicker->coords.x1 + lv_obj_get_width(cpicker) / 2;
        lv_coord_t y = cpicker->coords.y1 + lv_obj_get_height(cpicker) / 2;
        lv_opa_t opa_scale = lv_obj_get_opa_scale(cpicker);

        uint8_t redraw_wheel = 0;

        lv_area_t center_ind_area;

        uint32_t rin = r - styleCopy.line.width;
        //the square area (a and b being sides) should fit into the center of diameter d
        //we have:
        //a^2+b^2<=d^2
        //2a^2 <= d^2
        //a^2<=(d^2)/2
        //a <= sqrt((d^2)/2)
        uint16_t radius = lv_sqrt((4*rin*rin)/2)/2 - style->body.padding.inner;

        center_ind_area.x1 = x - radius;
        center_ind_area.y1 = y - radius;
        center_ind_area.x2 = x + radius;
        center_ind_area.y2 = y + radius;

        /*redraw the wheel only if the mask intersect with the wheel*/
        if(mask->x1 < center_ind_area.x1 || mask->x2 > center_ind_area.x2
                || mask->y1 < center_ind_area.y1 || mask->y2 > center_ind_area.y2)
        {
            redraw_wheel = 1;
        }

        lv_point_t triangle_points[3];

        int16_t start_angle, end_angle;
        start_angle = 0; //Default
        end_angle = 360 - LV_CPICKER_DEF_QF; //Default

        if(redraw_wheel)
        {
            /*if the mask does not include the center of the object
             * redrawing all the wheel is not necessary;
             * only a given angular range
             */
            lv_point_t center = {x, y};
            if(!lv_area_is_point_on(mask, &center)
                    /*
              && (mask->x1 != cpicker->coords.x1 || mask->x2 != cpicker->coords.x2
              ||  mask->y1 != cpicker->coords.y1 || mask->y2 != cpicker->coords.y2)
                     */
            )
            {
                /*get angle from center of object to each corners of the area*/
                int16_t dr, ur, ul, dl;
                dr = lv_atan2(mask->x2 - x, mask->y2 - y);
                ur = lv_atan2(mask->x2 - x, mask->y1 - y);
                ul = lv_atan2(mask->x1 - x, mask->y1 - y);
                dl = lv_atan2(mask->x1 - x, mask->y2 - y);

                /* check area position from object axis*/
                uint8_t left = (mask->x2 < x && mask->x1 < x);
                uint8_t onYaxis = (mask->x2 > x && mask->x1 < x);
                uint8_t right = (mask->x2 > x && mask->x1 > x);
                uint8_t top = (mask->y2 < y && mask->y1 < y);
                uint8_t onXaxis = (mask->y2 > y && mask->y1 < y);
                uint8_t bottom = (mask->y2 > y && mask->y1 > x);

                /*store angular range*/
                if(right && bottom)
                {
                    start_angle = dl;
                    end_angle = ur;
                }
                else if(right && onXaxis)
                {
                    start_angle = dl;
                    end_angle = ul;
                }
                else if(right && top)
                {
                    start_angle = dr;
                    end_angle = ul;
                }
                else if(onYaxis && top)
                {
                    start_angle = dr;
                    end_angle = dl;
                }
                else if(left && top)
                {
                    start_angle = ur;
                    end_angle = dl;
                }
                else if(left && onXaxis)
                {
                    start_angle = ur;
                    end_angle = dr;
                }
                else if(left && bottom)
                {
                    start_angle = ul;
                    end_angle = dr;
                }
                else if(onYaxis && bottom)
                {
                    start_angle = ul;
                    end_angle = ur;
                }

                /*rollover angle*/
                if(start_angle > end_angle)
                {
                    end_angle +=  360;
                }

                /*round to QF factor*/
                start_angle = start_angle/LV_CPICKER_DEF_QF*LV_CPICKER_DEF_QF;
                end_angle = end_angle/LV_CPICKER_DEF_QF*LV_CPICKER_DEF_QF;;

                /*shift angle if necessary before adding offset*/
                if((start_angle - LV_CPICKER_DEF_QF) < 0)
                {
                    start_angle += 360;
                    end_angle += 360;
                }

                /*ensure overlapping by adding offset*/
                start_angle -= LV_CPICKER_DEF_QF;
                end_angle += LV_CPICKER_DEF_QF;
            }

            if(ext->color_mode == LV_CPICKER_COLOR_MODE_HUE)
            {
                for(uint16_t i = start_angle; i <= end_angle; i+= LV_CPICKER_DEF_QF)
                {
                    styleCopy.body.main_color = lv_color_hsv_to_rgb(i%360, ext->saturation, ext->value);

                    triangle_points[0].x = x;
                    triangle_points[0].y = y;

                    triangle_points[1].x = x + (r * lv_trigo_sin(i) >> LV_TRIGO_SHIFT);
                    triangle_points[1].y = y + (r * lv_trigo_sin(i + 90) >> LV_TRIGO_SHIFT);


                    if(i == end_angle || i == (360 - LV_CPICKER_DEF_QF))
                    {
                        /*the last triangle is drawn without additional overlapping pixels*/
                        triangle_points[2].x = x + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF) >> LV_TRIGO_SHIFT);
                        triangle_points[2].y = y + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + 90) >> LV_TRIGO_SHIFT);

                    }
                    else
                    {
                        triangle_points[2].x = x + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET) >> LV_TRIGO_SHIFT);
                        triangle_points[2].y = y + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET + 90) >> LV_TRIGO_SHIFT);

                    }

                    lv_draw_triangle(triangle_points, mask, &styleCopy, LV_OPA_COVER);
                }
            }
            else if(ext->color_mode == LV_CPICKER_COLOR_MODE_SATURATION)
            {
                for(uint16_t i = start_angle; i <= end_angle; i += LV_CPICKER_DEF_QF)
                {
                    styleCopy.body.main_color = lv_color_hsv_to_rgb(ext->hue, (i%360)*100/360, ext->value);

                    triangle_points[0].x = x;
                    triangle_points[0].y = y;

                    triangle_points[1].x = x + (r * lv_trigo_sin(i) >> LV_TRIGO_SHIFT);
                    triangle_points[1].y = y + (r * lv_trigo_sin(i + 90) >> LV_TRIGO_SHIFT);

                    if(i == end_angle || i == (360 - LV_CPICKER_DEF_QF))
                    {
                        /*the last triangle is drawn without additional overlapping pixels*/
                        triangle_points[2].x = x + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF) >> LV_TRIGO_SHIFT);
                        triangle_points[2].y = y + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + 90) >> LV_TRIGO_SHIFT);
                    }
                    else
                    {
                        triangle_points[2].x = x + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET) >> LV_TRIGO_SHIFT);
                        triangle_points[2].y = y + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET + 90) >> LV_TRIGO_SHIFT);
                    }

                    lv_draw_triangle(triangle_points, mask, &styleCopy, LV_OPA_COVER);
                }
            }
            else if(ext->color_mode == LV_CPICKER_COLOR_MODE_VALUE)
            {
                for(uint16_t i = start_angle; i <= end_angle; i += LV_CPICKER_DEF_QF)
                {
                    styleCopy.body.main_color = lv_color_hsv_to_rgb(ext->hue, ext->saturation, (i%360)*100/360);

                    triangle_points[0].x = x;
                    triangle_points[0].y = y;

                    triangle_points[1].x = x + (r * lv_trigo_sin(i) >> LV_TRIGO_SHIFT);
                    triangle_points[1].y = y + (r * lv_trigo_sin(i + 90) >> LV_TRIGO_SHIFT);

                    if(i == end_angle || i == (360 - LV_CPICKER_DEF_QF))
                    {
                        /*the last triangle is drawn without additional overlapping pixels*/
                        triangle_points[2].x = x + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF) >> LV_TRIGO_SHIFT);
                        triangle_points[2].y = y + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + 90) >> LV_TRIGO_SHIFT);

                    }
                    else
                    {
                        triangle_points[2].x = x + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET) >> LV_TRIGO_SHIFT);
                        triangle_points[2].y = y + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET + 90) >> LV_TRIGO_SHIFT);
                    }

                    lv_draw_triangle(triangle_points, mask, &styleCopy, LV_OPA_COVER);
                }
            }
        }

        //draw center background
        lv_area_t center_area;
        uint16_t wradius = r - styleCopy.line.width;
        center_area.x1 = x - wradius;
        center_area.y1 = y - wradius;
        center_area.x2 = x + wradius;
        center_area.y2 = y + wradius;
        styleCenterBackground.body.radius = LV_RADIUS_CIRCLE;
        lv_draw_rect(&center_area, mask, &styleCenterBackground, opa_scale);

        //draw the center color indicator
        styleCopy.body.main_color = lv_color_hsv_to_rgb(ext->hue, ext->saturation, ext->value);
        styleCopy.body.grad_color = styleCopy.body.main_color;
        styleCopy.body.radius = LV_RADIUS_CIRCLE;
        lv_draw_rect(&center_ind_area, mask, &styleCopy, opa_scale);

        //Draw the current hue indicator
        switch(ext->indicator.type)
        {
        case LV_CPICKER_INDICATOR_NONE:
            break;
        case LV_CPICKER_INDICATOR_LINE:
        {
            lv_point_t start;
            lv_point_t end;

            uint16_t angle;

            switch(ext->color_mode)
            {
            default:
            case LV_CPICKER_COLOR_MODE_HUE:
                angle = ext->hue;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                angle = ext->saturation * 360 / 100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                angle = ext->value * 360 / 100;
                break;
            }

            /*save the angle to refresh the area later*/
            ext->prev_pos = angle;

            start.x = x + ((r - style->line.width + ext->indicator.style->body.padding.inner + ext->indicator.style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            start.y = y + ((r - style->line.width + ext->indicator.style->body.padding.inner + ext->indicator.style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

            end.x = x + ((r - ext->indicator.style->body.padding.inner - ext->indicator.style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            end.y = y + ((r - ext->indicator.style->body.padding.inner - ext->indicator.style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

            lv_draw_line(&start, &end, mask, ext->indicator.style, opa_scale);
            if(ext->indicator.style->line.rounded)
            {
                lv_area_t circle_area;
                circle_area.x1 = start.x - ((ext->indicator.style->line.width - 1) >> 1) - ((ext->indicator.style->line.width - 1) & 0x1);
                circle_area.y1 = start.y - ((ext->indicator.style->line.width - 1) >> 1) - ((ext->indicator.style->line.width - 1) & 0x1);
                circle_area.x2 = start.x + ((ext->indicator.style->line.width - 1) >> 1);
                circle_area.y2 = start.y + ((ext->indicator.style->line.width - 1) >> 1);
                lv_draw_rect(&circle_area, mask, ext->indicator.style, opa_scale);

                circle_area.x1 = end.x - ((ext->indicator.style->line.width - 1) >> 1) - ((ext->indicator.style->line.width - 1) & 0x1);
                circle_area.y1 = end.y - ((ext->indicator.style->line.width - 1) >> 1) - ((ext->indicator.style->line.width - 1) & 0x1);
                circle_area.x2 = end.x + ((ext->indicator.style->line.width - 1) >> 1);
                circle_area.y2 = end.y + ((ext->indicator.style->line.width - 1) >> 1);
                lv_draw_rect(&circle_area, mask, ext->indicator.style, opa_scale);
            }
            break;
        }
        case LV_CPICKER_INDICATOR_CIRCLE:
        {
            lv_area_t circle_ind_area;
            uint32_t cx, cy;

            uint16_t angle;

            switch(ext->color_mode)
            {
            default:
            case LV_CPICKER_COLOR_MODE_HUE:
                angle = ext->hue;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                angle = ext->saturation * 360 / 100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                angle = ext->value * 360 / 100;
                break;
            }

            /*save the angle to refresh the area later*/
            ext->prev_pos = angle;

            cx = x + ((r - style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            cy = y + ((r - style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

            circle_ind_area.x1 = cx - style->line.width/2;
            circle_ind_area.y1 = cy - style->line.width/2;
            circle_ind_area.x2 = cx + style->line.width/2;
            circle_ind_area.y2 = cy + style->line.width/2;

            lv_style_copy(&styleCopy, ext->indicator.style);
            styleCopy.body.radius = LV_RADIUS_CIRCLE;

            lv_draw_rect(&circle_ind_area, mask, &styleCopy, opa_scale);
            break;
        }
        case LV_CPICKER_INDICATOR_IN:
        {
            lv_area_t circle_ind_area;
            uint32_t cx, cy;

            uint16_t angle;

            switch(ext->color_mode)
            {
            default:
            case LV_CPICKER_COLOR_MODE_HUE:
                angle = ext->hue;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                angle = ext->saturation * 360 / 100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                angle = ext->value * 360 / 100;
                break;
            }

            /*save the angle to refresh the area later*/
            ext->prev_pos = angle;

            uint16_t ind_radius = lv_sqrt((4*rin*rin)/2)/2 + 1 - style->body.padding.inner;
            ind_radius = (ind_radius + rin) / 2;

            cx = x + ((ind_radius) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            cy = y + ((ind_radius) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

            circle_ind_area.x1 = cx - ((wradius - radius) / 3);
            circle_ind_area.y1 = cy - ((wradius - radius) / 3);
            circle_ind_area.x2 = cx + ((wradius - radius) / 3);
            circle_ind_area.y2 = cy + ((wradius - radius) / 3);

            lv_style_copy(&styleCopy, ext->indicator.style);
            styleCopy.body.radius = LV_RADIUS_CIRCLE;

            lv_draw_rect(&circle_ind_area, mask, &styleCopy, opa_scale);
            break;
        }
        } // switch

        /*
        //code to color the drawn area
        static uint32_t c = 0;
        lv_style_t style2;
        lv_style_copy(&style2, &lv_style_plain);
        style2.body.main_color.full = c;
        style2.body.grad_color.full = c;
        c += 0x123445678;
        lv_draw_rect(mask, mask, &style2, opa_scale);
        */
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}

/**
 * Handle the drawing related tasks of the color_pickerwhen of rectangle type
 * @param cpicker pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_cpicker_rect_design(lv_obj_t * cpicker, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {

        lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
        lv_style_t * style = lv_cpicker_get_style(cpicker, LV_CPICKER_STYLE_MAIN);

        static lv_style_t styleCopy;
        lv_style_copy(&styleCopy, style);

        lv_coord_t w = lv_obj_get_width(cpicker);
        lv_coord_t h = lv_obj_get_height(cpicker);

        lv_coord_t gradient_w, gradient_h;

        lv_coord_t x1 = cpicker->coords.x1;
        lv_coord_t y1 = cpicker->coords.y1;
        lv_coord_t x2 = cpicker->coords.x2;
        lv_coord_t y2 = cpicker->coords.y2;
        lv_opa_t opa_scale = lv_obj_get_opa_scale(cpicker);

        /* prepare the color preview area */
        uint16_t preview_offset = style->line.width;
        uint16_t style_body_padding_ver = style->body.padding.top + style->body.padding.bottom;
        uint16_t style_body_padding_hor = style->body.padding.left + style->body.padding.right;
        if(style_body_padding_ver == 0)
        {
            /* draw the color preview rect to the side of the gradient*/
            if(style_body_padding_hor >= 0)
            {
                /*draw the preview to the right*/
                gradient_w = w - preview_offset - (LV_MATH_ABS(style_body_padding_hor) - 1);
                gradient_h = y2 - y1;
                ext->rect_gradient_area.x1 = x1;
                ext->rect_gradient_area.x2 = ext->rect_gradient_area.x1 + gradient_w;
                ext->rect_gradient_area.y1 = y1;
                ext->rect_gradient_area.y2 = y2;

                ext->rect_preview_area.x1 = x2 - preview_offset;
                ext->rect_preview_area.y1 = y1;
                ext->rect_preview_area.x2 = x2 ;
                ext->rect_preview_area.y2 = y2;
            }
            else
            {
                /*draw the preview to the left*/
                gradient_w = w - preview_offset - (LV_MATH_ABS(style_body_padding_hor) - 1);
                gradient_h = y2 - y1;
                ext->rect_gradient_area.x1 = x2 - gradient_w;
                ext->rect_gradient_area.x2 = x2;
                ext->rect_gradient_area.y1 = y1;
                ext->rect_gradient_area.y2 = y2;

                ext->rect_preview_area.x1 = x1;
                ext->rect_preview_area.y1 = y1;
                ext->rect_preview_area.x2 = x1 + preview_offset;
                ext->rect_preview_area.y2 = y2;
            }
        }
        else
        {
            /* draw the color preview rect on top or below the gradient*/
            if(style_body_padding_ver >= 0)
            {
                /*draw the preview on top*/
                gradient_w = w;
                gradient_h = (y2 - y1) - preview_offset - (LV_MATH_ABS(style_body_padding_ver) - 1);
                ext->rect_gradient_area.x1 = x1;
                ext->rect_gradient_area.x2 = x2;
                ext->rect_gradient_area.y1 = y2 - gradient_h;
                ext->rect_gradient_area.y2 = y2;

                ext->rect_preview_area.x1 = x1;
                ext->rect_preview_area.y1 = y1;
                ext->rect_preview_area.x2 = x2;
                ext->rect_preview_area.y2 = y1 + preview_offset;
            }
            else
            {
                /*draw the preview below the gradient*/
                gradient_w = w;
                gradient_h = (y2 - y1) - preview_offset - (LV_MATH_ABS(style_body_padding_ver) - 1);
                ext->rect_gradient_area.x1 = x1;
                ext->rect_gradient_area.x2 = x2;
                ext->rect_gradient_area.y1 = y1;
                ext->rect_gradient_area.y2 = y1 + gradient_h;

                ext->rect_preview_area.x1 = x1;
                ext->rect_preview_area.y1 = y2 - preview_offset;
                ext->rect_preview_area.x2 = x2;
                ext->rect_preview_area.y2 = y2;
            }
        }

        if(style->line.rounded)
        {
            /*draw rounded edges to the gradient*/
            lv_area_t rounded_edge_area;
            rounded_edge_area.x1 = ext->rect_gradient_area.x1;
            rounded_edge_area.x2 = ext->rect_gradient_area.x1 + gradient_h;
            rounded_edge_area.y1 = ext->rect_gradient_area.y1;
            rounded_edge_area.y2 = ext->rect_gradient_area.y2;

            ext->rect_gradient_area.x1 += gradient_h/2;
            ext->rect_gradient_area.x2 -= gradient_h/2;
            gradient_w -= gradient_h;

            switch(ext->color_mode)
            {
            default:
            case LV_CPICKER_COLOR_MODE_HUE:
                styleCopy.body.main_color = lv_color_hsv_to_rgb(0, ext->saturation, ext->value);
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                styleCopy.body.main_color = lv_color_hsv_to_rgb(ext->hue, 0, ext->value);
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                styleCopy.body.main_color = lv_color_hsv_to_rgb(ext->hue, ext->saturation, 0);
                break;
            }
            styleCopy.body.grad_color = styleCopy.body.main_color;

            styleCopy.body.radius = LV_RADIUS_CIRCLE;

            lv_draw_rect(&rounded_edge_area, mask, &styleCopy, opa_scale);

            rounded_edge_area.x1 += gradient_w - 1;
            rounded_edge_area.x2 += gradient_w - 1;

            switch(ext->color_mode)
            {
            default:
            case LV_CPICKER_COLOR_MODE_HUE:
                styleCopy.body.main_color = lv_color_hsv_to_rgb(360, ext->saturation, ext->value);
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                styleCopy.body.main_color = lv_color_hsv_to_rgb(ext->hue, 100, ext->value);
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                styleCopy.body.main_color = lv_color_hsv_to_rgb(ext->hue, ext->saturation, 100);
                break;
            }
            styleCopy.body.grad_color = styleCopy.body.main_color;

            lv_draw_rect(&rounded_edge_area, mask, &styleCopy, opa_scale);
        }

        for(uint16_t i = 0; i < 360; i += LV_MATH_MAX(LV_CPICKER_DEF_QF, 360/gradient_w))
        {
            switch(ext->color_mode)
            {
            default:
            case LV_CPICKER_COLOR_MODE_HUE:
                styleCopy.body.main_color = lv_color_hsv_to_rgb(i%360, ext->saturation, ext->value);
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                styleCopy.body.main_color = lv_color_hsv_to_rgb(ext->hue, (i%360)*100/360, ext->value);
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                styleCopy.body.main_color = lv_color_hsv_to_rgb(ext->hue, ext->saturation, (i%360)*100/360);
                break;
            }

            styleCopy.body.grad_color = styleCopy.body.main_color;

            /*the following attribute might need changing between index to add border, shadow, radius etc*/
            styleCopy.body.radius = 0;
            styleCopy.body.border.width = 0;
            styleCopy.body.shadow.width = 0;
            styleCopy.body.opa = LV_OPA_COVER;

            lv_area_t rect_area;

            /*scale angle (hue/sat/val) to linear coordinate*/
            lv_coord_t xi = i*gradient_w/360;

            rect_area.x1 = LV_MATH_MIN(ext->rect_gradient_area.x1 + xi, ext->rect_gradient_area.x1 + gradient_w - LV_MATH_MAX(LV_CPICKER_DEF_QF, 360/gradient_w));
            rect_area.y1 = ext->rect_gradient_area.y1;
            rect_area.x2 = rect_area.x1 + LV_MATH_MAX(LV_CPICKER_DEF_QF, 360/gradient_w);
            rect_area.y2 = ext->rect_gradient_area.y2;

            lv_draw_rect(&rect_area, mask, &styleCopy, opa_scale);
        }

        if(style->line.rounded)
        {
            /*Restore gradient area to take rounded end in account*/
            ext->rect_gradient_area.x1 -= gradient_h/2;
            ext->rect_gradient_area.x2 += gradient_h/2;
            //gradient_w += gradient_h;
        }

        /*draw the color preview indicator*/
        styleCopy.body.main_color = lv_cpicker_get_color(cpicker);
        styleCopy.body.grad_color = styleCopy.body.main_color;
        if(style->line.rounded && style_body_padding_hor == 0)
        {
            styleCopy.body.radius = gradient_h;
        }
        lv_draw_rect(&(ext->rect_preview_area), mask, &styleCopy, opa_scale);

        /*
        styleCopy.line.width = 10;
        lv_draw_arc(cpicker->coords.x1 + 3*gradient_h/2, cpicker->coords.y1 + gradient_h/2, gradient_h / 2 + styleCopy.line.width + 2, mask, 180, 360, &styleCopy, opa_scale);
        //lv_draw_arc(cpicker->coords.x1 + gradient_w - gradient_h/2, cpicker->coords.y1 + gradient_h/2, gradient_h / 2 + styleCopy.line.width + 2, mask, 0, 180, &styleCopy, opa_scale);
        */

        /*draw the color position indicator*/
        lv_coord_t ind_pos = style->line.rounded ? gradient_h / 2 : 0;
        switch(ext->color_mode)
        {
        default:
        case LV_CPICKER_COLOR_MODE_HUE:
            ind_pos += ext->hue * gradient_w / 360;
            break;
        case LV_CPICKER_COLOR_MODE_SATURATION:
            ind_pos += ext->saturation * gradient_w / 100;
            break;
        case LV_CPICKER_COLOR_MODE_VALUE:
            ind_pos += ext->value * gradient_w / 100;
            break;
        }

        /*save to refresh the area later*/
        ext->prev_pos = ind_pos;

        switch(ext->indicator.type)
        {
        case LV_CPICKER_INDICATOR_NONE:
            /*no indicator*/
            break;
        case LV_CPICKER_INDICATOR_LINE:
        {
            lv_point_t p1, p2;
            p1.x = ext->rect_gradient_area.x1 + ind_pos;
            p1.y = ext->rect_gradient_area.y1;
            p2.x = p1.x;
            p2.y = ext->rect_gradient_area.y2;

            lv_draw_line(&p1, &p2, mask, ext->indicator.style, opa_scale);
            break;
        }
        case LV_CPICKER_INDICATOR_CIRCLE:
        {
            lv_area_t circle_ind_area;
            circle_ind_area.x1 = ext->rect_gradient_area.x1 + ind_pos - gradient_h/2;
            circle_ind_area.x2 = circle_ind_area.x1 + gradient_h;
            circle_ind_area.y1 = ext->rect_gradient_area.y1;
            circle_ind_area.y2 = ext->rect_gradient_area.y2;

            lv_style_copy(&styleCopy, ext->indicator.style);
            styleCopy.body.radius = LV_RADIUS_CIRCLE;

            lv_draw_rect(&circle_ind_area, mask, &styleCopy, opa_scale);
            break;
        }
        case LV_CPICKER_INDICATOR_IN:
        {
            /*draw triangle under the gradient*/
            lv_point_t triangle_points[3];

            triangle_points[0].x = ext->rect_gradient_area.x1 + ind_pos;
            triangle_points[0].y = ext->rect_gradient_area.y1 + (gradient_h/3);

            triangle_points[1].x = triangle_points[0].x - ext->indicator.style->line.width * 3;
            triangle_points[1].y = ext->rect_gradient_area.y1 - 1;

            triangle_points[2].x = triangle_points[0].x + ext->indicator.style->line.width * 3;
            triangle_points[2].y = triangle_points[1].y;

            lv_draw_triangle(triangle_points, mask, ext->indicator.style, LV_OPA_COVER);

            triangle_points[0].y = ext->rect_gradient_area.y2 - (gradient_h/3);
            triangle_points[1].y = ext->rect_gradient_area.y2;
            triangle_points[2].y = triangle_points[1].y;
            lv_draw_triangle(triangle_points, mask, ext->indicator.style, LV_OPA_COVER);
            break;
        }
        default:
            break;
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}

/**
 * Signal function of the color_picker of wheel type
 * @param cpicker pointer to a color_picker object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_cpicker_disc_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(cpicker, sign, param);
    if(res != LV_RES_OK) return res;

    lv_style_t * style = lv_cpicker_get_style(cpicker, LV_CPICKER_STYLE_MAIN);

    lv_coord_t r_out = (LV_MATH_MIN(lv_obj_get_width(cpicker), lv_obj_get_height(cpicker))) / 2;
    lv_coord_t r_in = r_out - style->line.width - style->body.padding.inner;

    lv_coord_t x = cpicker->coords.x1 + lv_obj_get_width(cpicker) / 2;
    lv_coord_t y = cpicker->coords.y1 + lv_obj_get_height(cpicker) / 2;

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_cpicker";
    }
    else if(sign == LV_SIGNAL_PRESSED)
    {
        switch(ext->color_mode)
        {
        case LV_CPICKER_COLOR_MODE_HUE:
            ext->prev_hue = ext->hue;
            break;
        case LV_CPICKER_COLOR_MODE_SATURATION:
            ext->prev_saturation = ext->saturation;
            break;
        case LV_CPICKER_COLOR_MODE_VALUE:
            ext->prev_value = ext->value;
            break;
        }

        lv_indev_t * indev = param;
        lv_coord_t xp = indev->proc.types.pointer.act_point.x - x;
        lv_coord_t yp = indev->proc.types.pointer.act_point.y - y;

        if((xp*xp + yp*yp) < (r_in*r_in))
        {
            if(lv_tick_elaps(ext->last_click) < 400)
            {
                switch(ext->color_mode)
                {
                case LV_CPICKER_COLOR_MODE_HUE:
                    ext->hue = 0;
                    ext->prev_hue = ext->hue;
                    break;
                case LV_CPICKER_COLOR_MODE_SATURATION:
                    ext->saturation = 100;
                    ext->prev_saturation = ext->saturation;
                    break;
                case LV_CPICKER_COLOR_MODE_VALUE:
                    ext->value = 100;
                    ext->prev_value = ext->value;
                    break;
                }
                //lv_cpicker_invalidate(cpicker, false);
            }
            ext->last_click = lv_tick_get();
        }
    }
    else if(sign == LV_SIGNAL_PRESSING)
    {
        lv_indev_t * indev = param;
        lv_coord_t xp = indev->proc.types.pointer.act_point.x - x;
        lv_coord_t yp = indev->proc.types.pointer.act_point.y - y;

        if((xp*xp + yp*yp) < (r_out*r_out) && (xp*xp + yp*yp) >= (r_in*r_in))
        {
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = lv_atan2(xp, yp);
                ext->prev_hue = ext->hue;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = lv_atan2(xp, yp) * 100.0 / 360.0;
                ext->prev_saturation = ext->saturation;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = lv_atan2(xp, yp) * 100.0 / 360.0;
                ext->prev_value = ext->value;
                break;
            }
            lv_cpicker_invalidate(cpicker, false);
        }
    }
    else if(sign == LV_SIGNAL_PRESS_LOST)
    {
        switch(ext->color_mode)
        {
        case LV_CPICKER_COLOR_MODE_HUE:
            ext->prev_hue = ext->hue;
            break;
        case LV_CPICKER_COLOR_MODE_SATURATION:
            ext->prev_saturation = ext->saturation;
            break;
        case LV_CPICKER_COLOR_MODE_VALUE:
            ext->prev_value = ext->value;
            break;
        }
        lv_cpicker_invalidate(cpicker, false);
    }
    else if(sign == LV_SIGNAL_RELEASED)
    {
        lv_indev_t * indev = param;
        lv_coord_t xp = indev->proc.types.pointer.act_point.x - x;
        lv_coord_t yp = indev->proc.types.pointer.act_point.y - y;

        if((xp*xp + yp*yp) < (r_out*r_out) && (xp*xp + yp*yp) >= (r_in*r_in))
        {
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = lv_atan2(xp, yp);
                ext->prev_hue = ext->hue;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = lv_atan2(xp, yp) * 100.0 / 360.0;
                ext->prev_saturation = ext->saturation;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = lv_atan2(xp, yp) * 100.0 / 360.0;
                ext->prev_value = ext->value;
                break;
            }

            lv_cpicker_invalidate(cpicker, false);

            res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_LONG_PRESS)
    {
        if(!ext->color_mode_fixed)
        {
            lv_indev_t * indev = param;
            lv_coord_t xp = indev->proc.types.pointer.act_point.x - x;
            lv_coord_t yp = indev->proc.types.pointer.act_point.y - y;

            if((xp*xp + yp*yp) < (r_in*r_in))
            {
                switch(ext->color_mode)
                {
                case LV_CPICKER_COLOR_MODE_HUE:
                    ext->prev_hue = ext->hue;
                    break;
                case LV_CPICKER_COLOR_MODE_SATURATION:
                    ext->prev_saturation = ext->saturation;
                    break;
                case LV_CPICKER_COLOR_MODE_VALUE:
                    ext->prev_value = ext->value;
                    break;
                }

                ext->color_mode = (ext->color_mode + 1) % 3;

                lv_cpicker_invalidate(cpicker, true);
            }
        }
    }
    else if(sign == LV_SIGNAL_CONTROL)
    {
        uint32_t c = *((uint32_t *)param);      /*uint32_t because can be UTF-8*/
        if(c == LV_KEY_RIGHT)
        {
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = (ext->hue + 1) % 360;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = (ext->saturation + 1) % 100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = (ext->value + 1) % 100;
                break;
            }

            lv_cpicker_invalidate(cpicker, false);

            res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
        else if(c == LV_KEY_LEFT)
        {
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = ext->hue > 0?(ext->hue - 1):360;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = ext->saturation > 0?(ext->saturation - 1):100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = ext->value > 0?(ext->value - 1):100;
                break;
            }

            lv_cpicker_invalidate(cpicker, false);

            res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
        else if(c == LV_KEY_UP)
        {
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = (ext->hue + 1) % 360;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = (ext->saturation + 1) % 100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = (ext->value + 1) % 100;
                break;
            }

            lv_cpicker_invalidate(cpicker, false);

            res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
        else if(c == LV_KEY_DOWN)
        {
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = ext->hue > 0?(ext->hue - 1):360;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = ext->saturation > 0?(ext->saturation - 1):100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = ext->value > 0?(ext->value - 1):100;
                break;
            }

            lv_cpicker_invalidate(cpicker, false);

            res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    }

    return res;
}

/**
 * Signal function of the color_picker of rectangle type
 * @param cpicker pointer to a color_picker object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_cpicker_rect_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(cpicker, sign, param);
    if(res != LV_RES_OK) return res;

    lv_style_t * style = lv_cpicker_get_style(cpicker, LV_CPICKER_STYLE_MAIN);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_cpicker";
    }
    else if(sign == LV_SIGNAL_PRESSED)
    {
        switch(ext->color_mode)
        {
        case LV_CPICKER_COLOR_MODE_HUE:
            ext->prev_hue = ext->hue;
            break;
        case LV_CPICKER_COLOR_MODE_SATURATION:
            ext->prev_saturation = ext->saturation;
            break;
        case LV_CPICKER_COLOR_MODE_VALUE:
            ext->prev_value = ext->value;
            break;
        }

        lv_indev_t * indev = param;

        if(lv_area_is_point_on(&(ext->rect_preview_area), &indev->proc.types.pointer.act_point))
        {
            if(lv_tick_elaps(ext->last_click) < 400)
            {
                switch(ext->color_mode)
                {
                case LV_CPICKER_COLOR_MODE_HUE:
                    ext->hue = 0;
                    ext->prev_hue = ext->hue;
                    break;
                case LV_CPICKER_COLOR_MODE_SATURATION:
                    ext->saturation = 100;
                    ext->prev_saturation = ext->saturation;
                    break;
                case LV_CPICKER_COLOR_MODE_VALUE:
                    ext->value = 100;
                    ext->prev_value = ext->value;
                    break;
                }
                //lv_cpicker_invalidate(cpicker, false);
            }
            ext->last_click = lv_tick_get();
        }
    }
    else if(sign == LV_SIGNAL_PRESSING)
    {
        lv_indev_t * indev = param;

        if(lv_area_is_point_on(&(ext->rect_gradient_area), &indev->proc.types.pointer.act_point))
        {
            uint16_t width = ext->rect_gradient_area.x2 - ext->rect_gradient_area.x1;
            uint16_t distance = indev->proc.types.pointer.act_point.x - ext->rect_gradient_area.x1;
            float percent = distance / (float) width;
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = percent * 360;
                ext->prev_hue = ext->hue;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = percent * 100.0;
                ext->prev_saturation = ext->saturation;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = percent * 100.0;
                ext->prev_value = ext->value;
                break;
            }
            lv_cpicker_invalidate(cpicker, false);
        }
    }
    else if(sign == LV_SIGNAL_PRESS_LOST)
    {
        switch(ext->color_mode)
        {
        case LV_CPICKER_COLOR_MODE_HUE:
            ext->prev_hue = ext->hue;
            break;
        case LV_CPICKER_COLOR_MODE_SATURATION:
            ext->prev_saturation = ext->saturation;
            break;
        case LV_CPICKER_COLOR_MODE_VALUE:
            ext->prev_value = ext->value;
            break;
        }
        lv_cpicker_invalidate(cpicker, false);
    }
    else if(sign == LV_SIGNAL_RELEASED)
    {
        lv_indev_t * indev = param;

        if(lv_area_is_point_on(&(ext->rect_gradient_area), &indev->proc.types.pointer.act_point))
        {
            uint16_t width = ext->rect_gradient_area.x2 - ext->rect_gradient_area.x1;
            uint16_t distance = indev->proc.types.pointer.act_point.x - ext->rect_gradient_area.x1;
            float percent = distance / (float) width;
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = percent * 360;
                ext->prev_hue = ext->hue;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = percent * 100;
                ext->prev_saturation = ext->saturation;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = percent * 100;
                ext->prev_value = ext->value;
                break;
            }

            lv_cpicker_invalidate(cpicker, false);

            res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_LONG_PRESS)
    {
        if(!ext->color_mode_fixed)
        {
            lv_indev_t * indev = param;

            if(lv_area_is_point_on(&(ext->rect_preview_area), &indev->proc.types.pointer.act_point))
            {
                switch(ext->color_mode)
                {
                case LV_CPICKER_COLOR_MODE_HUE:
                    ext->prev_hue = ext->hue;
                    break;
                case LV_CPICKER_COLOR_MODE_SATURATION:
                    ext->prev_saturation = ext->saturation;
                    break;
                case LV_CPICKER_COLOR_MODE_VALUE:
                    ext->prev_value = ext->value;
                    break;
                }

                ext->color_mode = (ext->color_mode + 1) % 3;

                lv_cpicker_invalidate(cpicker, true);
            }
        }
    }
    else if(sign == LV_SIGNAL_CONTROL)
    {
        uint32_t c = *((uint32_t *)param);      /*uint32_t because can be UTF-8*/
        if(c == LV_KEY_RIGHT)
        {
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = (ext->hue + 1) % 360;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = (ext->saturation + 1) % 100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = (ext->value + 1) % 100;
                break;
            }

            lv_cpicker_invalidate(cpicker, false);

            res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
        else if(c == LV_KEY_LEFT)
        {
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = ext->hue > 0?(ext->hue - 1):360;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = ext->saturation > 0?(ext->saturation - 1):100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = ext->value > 0?(ext->value - 1):100;
                break;
            }

            lv_cpicker_invalidate(cpicker, false);

            res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
        else if(c == LV_KEY_UP)
        {
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = (ext->hue + 1) % 360;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = (ext->saturation + 1) % 100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = (ext->value + 1) % 100;
                break;
            }

            lv_cpicker_invalidate(cpicker, false);

            res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
        else if(c == LV_KEY_DOWN)
        {
            switch(ext->color_mode)
            {
            case LV_CPICKER_COLOR_MODE_HUE:
                ext->hue = ext->hue > 0?(ext->hue - 1):360;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ext->saturation = ext->saturation > 0?(ext->saturation - 1):100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ext->value = ext->value > 0?(ext->value - 1):100;
                break;
            }

            lv_cpicker_invalidate(cpicker, false);

            res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    }

    return res;
}

/**
 * Indicator points need to match those set in lv_cpicker_disc_design/lv_cpicker_rect_design
 */ 
static void lv_cpicker_invalidate(lv_obj_t * cpicker, bool all)
{
    if (all)
    {
        lv_obj_invalidate(cpicker);
        return;
    }

    lv_disp_t * disp = lv_disp_get_default();

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    lv_style_t * style = lv_cpicker_get_style(cpicker, LV_CPICKER_STYLE_MAIN);

    static lv_style_t styleCopy;
    lv_style_copy(&styleCopy, style);

    lv_coord_t w = lv_obj_get_width(cpicker);
    lv_coord_t h = lv_obj_get_height(cpicker);

    if(ext->type == LV_CPICKER_TYPE_DISC)
    {
        lv_coord_t r = LV_MATH_MIN(w, h) / 2;
        lv_coord_t x = cpicker->coords.x1 + w / 2;
        lv_coord_t y = cpicker->coords.y1 + h / 2;
        
        /*invalidate center color area*/
        lv_area_t center_color_area;

        uint32_t rin = r - styleCopy.line.width;

        uint16_t radius = lv_sqrt((4*rin*rin)/2)/2 + 1 - style->body.padding.inner;

        center_color_area.x1 = x - radius;
        center_color_area.y1 = y - radius;
        center_color_area.x2 = x + radius;
        center_color_area.y2 = y + radius;

        lv_inv_area(disp, &center_color_area);

        /*invalidate indicator*/

        uint16_t angle;

        switch(ext->color_mode)
            {
            default:
            case LV_CPICKER_COLOR_MODE_HUE:
                angle = ext->hue;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                angle = ext->saturation * 360 / 100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                angle = ext->value * 360 / 100;
            break;
        }

        switch(ext->indicator.type)
        {
        case LV_CPICKER_INDICATOR_LINE:
        {
            lv_area_t line_area;
            lv_point_t point1, point2;
            lv_coord_t x1, y1, x2, y2;

            x1 = x + ((r - style->line.width + ext->indicator.style->body.padding.inner + ext->indicator.style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            y1 = y + ((r - style->line.width + ext->indicator.style->body.padding.inner + ext->indicator.style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);
            x2 = x + ((r - ext->indicator.style->body.padding.inner - ext->indicator.style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            y2 = y + ((r - ext->indicator.style->body.padding.inner - ext->indicator.style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

            point1.x = x1;
            point1.y = y1;
            point2.x = x2;
            point2.y = y2;

            //if(LV_MATH_ABS(point1.x - point2.x) > LV_MATH_ABS(point1.y - point2.y))
            //{
            /*Steps less in y then x -> rather horizontal*/
            if(point1.x < point2.x) {
                line_area.x1 = point1.x;
                //line_area.y1 = point1.y;
                line_area.x2 = point2.x;
                //line_area.y2 = point2.y;
            } else {
                line_area.x1 = point2.x;
                //line_area.y1 = point2.y;
                line_area.x2 = point1.x;
                //line_area.y2 = point1.y;
            }
            //} else {
            /*Steps less in x then y -> rather vertical*/
            if(point1.y < point2.y) {
                //line_area.x1 = point1.x;
                line_area.y1 = point1.y;
                //line_area.x2 = point2.x;
                line_area.y2 = point2.y;
            } else {
                //line_area.x1 = point2.x;
                line_area.y1 = point2.y;
                line_area.x2 = point1.x;
                //line_area.y2 = point1.y;
            }
            //}

            line_area.x1 -= 2*ext->indicator.style->line.width;
            line_area.y1 -= 2*ext->indicator.style->line.width;
            line_area.x2 += 2*ext->indicator.style->line.width;
            line_area.y2 += 2*ext->indicator.style->line.width;

            lv_inv_area(disp, &line_area);

            /* invalidate last postion */
            angle = ext->prev_pos;

            x1 = x + ((r - style->line.width + ext->indicator.style->body.padding.inner + ext->indicator.style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            y1 = y + ((r - style->line.width + ext->indicator.style->body.padding.inner + ext->indicator.style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

            x2 = x + ((r - ext->indicator.style->body.padding.inner - ext->indicator.style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            y2 = y + ((r - ext->indicator.style->body.padding.inner - ext->indicator.style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

            point1.x = x1;
            point1.y = y1;
            point2.x = x2;
            point2.y = y2;

            //if(LV_MATH_ABS(point1.x - point2.x) > LV_MATH_ABS(point1.y - point2.y))
            //{
            /*rather horizontal*/
            if(point1.x < point2.x) {
                line_area.x1 = point1.x;
                //line_area.y1 = point1.y;
                line_area.x2 = point2.x;
                //line_area.y2 = point2.y;
            } else {
                line_area.x1 = point2.x;
                //line_area.y1 = point2.y;
                line_area.x2 = point1.x;
                //line_area.y2 = point1.y;
            }
            //} else {
            /*rather vertical*/
            if(point1.y < point2.y) {
                //line_area.x1 = point1.x;
                line_area.y1 = point1.y;
                //line_area.x2 = point2.x;
                line_area.y2 = point2.y;
            } else {
                //line_area.x1 = point2.x;
                line_area.y1 = point2.y;
                //line_area.x2 = point1.x;
                line_area.y2 = point1.y;
            }
            //}

            line_area.x1 -= 2*ext->indicator.style->line.width;
            line_area.y1 -= 2*ext->indicator.style->line.width;
            line_area.x2 += 2*ext->indicator.style->line.width;
            line_area.y2 += 2*ext->indicator.style->line.width;

            lv_inv_area(disp, &line_area);

            break;
        }
        case LV_CPICKER_INDICATOR_CIRCLE:
        {
            lv_area_t circle_ind_area;
            uint32_t cx, cy;

            cx = x + ((r - style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            cy = y + ((r - style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

            circle_ind_area.x1 = cx - style->line.width/2;
            circle_ind_area.y1 = cy - style->line.width/2;
            circle_ind_area.x2 = cx + style->line.width/2;
            circle_ind_area.y2 = cy + style->line.width/2;

            lv_inv_area(disp, &circle_ind_area);

            /* invalidate last position*/
            angle = ext->prev_pos;

            cx = x + ((r - style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            cy = y + ((r - style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

            circle_ind_area.x1 = cx - style->line.width/2;
            circle_ind_area.y1 = cy - style->line.width/2;
            circle_ind_area.x2 = cx + style->line.width/2;
            circle_ind_area.y2 = cy + style->line.width/2;

            lv_inv_area(disp, &circle_ind_area);
            break;
        }
        case LV_CPICKER_INDICATOR_IN:
        {
            uint16_t wradius = r - style->line.width;

            lv_area_t circle_ind_area;
            uint32_t cx, cy;

            uint16_t ind_radius = lv_sqrt((4*rin*rin)/2)/2 + 1 - style->body.padding.inner;
            ind_radius = (ind_radius + rin) / 2;

            cx = x + ((ind_radius) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            cy = y + ((ind_radius) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

            circle_ind_area.x1 = cx - ((wradius - radius) / 3);
            circle_ind_area.y1 = cy - ((wradius - radius) / 3);
            circle_ind_area.x2 = cx + ((wradius - radius) / 3);
            circle_ind_area.y2 = cy + ((wradius - radius) / 3);

            lv_inv_area(disp, &circle_ind_area);

            /* invalidate last position*/
            angle = ext->prev_pos;

            cx = x + ((ind_radius) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
            cy = y + ((ind_radius) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

            circle_ind_area.x1 = cx - ((wradius - radius) / 3);
            circle_ind_area.y1 = cy - ((wradius - radius) / 3);
            circle_ind_area.x2 = cx + ((wradius - radius) / 3);
            circle_ind_area.y2 = cy + ((wradius - radius) / 3);

            lv_inv_area(disp, &circle_ind_area);
            break;
        }
        }
    }
    else if(ext->type == LV_CPICKER_TYPE_RECT)
    {
        /*invalidate color preview area*/
        lv_inv_area(disp, &ext->rect_preview_area);

        lv_coord_t gradient_w, gradient_h;

        lv_coord_t x1 = cpicker->coords.x1;
        lv_coord_t y1 = cpicker->coords.y1;
        lv_coord_t x2 = cpicker->coords.x2;
        lv_coord_t y2 = cpicker->coords.y2;

        uint16_t preview_offset = style->line.width;

        uint16_t style_body_padding_ver = style->body.padding.top + style->body.padding.bottom;
        uint16_t style_body_padding_hor = style->body.padding.left + style->body.padding.right;
        if(style_body_padding_ver == 0)
        {
            if(style_body_padding_hor >= 0)
            {
                gradient_w = w - preview_offset - (LV_MATH_ABS(style_body_padding_hor) - 1);
                gradient_h = y2 - y1;
            }
            else
            {
                gradient_w = w - preview_offset - (LV_MATH_ABS(style_body_padding_hor) - 1);
                gradient_h = y2 - y1;
            }
        }
        else
        {
            if(style_body_padding_ver >= 0)
            {
                gradient_w = w;
                gradient_h = (y2 - y1) - preview_offset - (LV_MATH_ABS(style_body_padding_ver) - 1);
            }
            else
            {
                gradient_w = w;
                gradient_h = (y2 - y1) - preview_offset - (LV_MATH_ABS(style_body_padding_ver) - 1);
            }
        }

        lv_coord_t ind_pos = style->line.rounded ? gradient_h / 2 : 0;
        switch(ext->color_mode)
        {
        default:
        case LV_CPICKER_COLOR_MODE_HUE:
            ind_pos += ext->hue * gradient_w / 360;
            break;
        case LV_CPICKER_COLOR_MODE_SATURATION:
            ind_pos += ext->saturation * gradient_w / 100;
            break;
        case LV_CPICKER_COLOR_MODE_VALUE:
            ind_pos += ext->value * gradient_w / 100;
            break;
        }
        lv_coord_t prev_pos = ext->prev_pos;

        switch(ext->indicator.type)
        {
        case LV_CPICKER_INDICATOR_LINE:
        {
            lv_area_t line_area;

            lv_point_t p1, p2;
            p1.x = ext->rect_gradient_area.x1 + ind_pos;
            p1.y = ext->rect_gradient_area.y1;
            p2.x = p1.x;
            p2.y = ext->rect_gradient_area.y2;

            line_area.x1 = p1.x;
            line_area.y1 = p1.y;
            line_area.x2 = p2.x;
            line_area.y2 = p2.x;

            line_area.x1 -= 2*ext->indicator.style->line.width;
            line_area.y1 -= 2*ext->indicator.style->line.width;
            line_area.x2 += 2*ext->indicator.style->line.width;
            line_area.y2 += 2*ext->indicator.style->line.width;

            lv_inv_area(disp, &line_area);

            /* invalidate last postion */
            p1.x = ext->rect_gradient_area.x1 + prev_pos;
            //p1.y = ext->rect_gradient_area.y1;
            p2.x = p1.x;
            //p2.y = ext->rect_gradient_area.y2;

            line_area.x1 = p1.x;
            line_area.y1 = p1.y;
            line_area.x2 = p2.x;
            line_area.y2 = p2.x;

            line_area.x1 -= 2*ext->indicator.style->line.width;
            line_area.y1 -= 2*ext->indicator.style->line.width;
            line_area.x2 += 2*ext->indicator.style->line.width;
            line_area.y2 += 2*ext->indicator.style->line.width;

            lv_inv_area(disp, &line_area);
            break;
        }
        case LV_CPICKER_INDICATOR_CIRCLE:
        {
            lv_area_t circle_ind_area;

            circle_ind_area.x1 = ext->rect_gradient_area.x1 + ind_pos - gradient_h/2;
            circle_ind_area.x2 = circle_ind_area.x1 + gradient_h;
            circle_ind_area.y1 = ext->rect_gradient_area.y1;
            circle_ind_area.y2 = ext->rect_gradient_area.y2;

            lv_inv_area(disp, &circle_ind_area);

            /* invalidate last postion */
            circle_ind_area.x1 = ext->rect_gradient_area.x1 + prev_pos - gradient_h/2;
            circle_ind_area.x2 = circle_ind_area.x1 + gradient_h;
            //circle_ind_area.y1 = ext->rect_gradient_area.y1;
            //circle_ind_area.y2 = ext->rect_gradient_area.y2;

            lv_inv_area(disp, &circle_ind_area);
            break;
        }
        case LV_CPICKER_INDICATOR_IN:
        {
            lv_coord_t center;
            lv_area_t ind_area;
            
            center = ext->rect_gradient_area.x1 + ind_pos;
            ind_area.x1 = center - ext->indicator.style->line.width * 3;
            ind_area.y1 = ext->rect_gradient_area.y1 - 1;
            ind_area.x2 = center + ext->indicator.style->line.width * 3;
            ind_area.y2 = ext->rect_gradient_area.y2;
            lv_inv_area(disp, &ind_area);

            /* invalidate last postion */
            center = ext->rect_gradient_area.x1 + prev_pos;
            ind_area.x1 = center - ext->indicator.style->line.width * 3;
            //ind_area.y1 = ext->rect_gradient_area.y1 - 1;
            ind_area.x2 = center + ext->indicator.style->line.width * 3;
            //ind_area.y2 = ext->rect_gradient_area.y2;
            lv_inv_area(disp, &ind_area);
            break;
        }
        }
    }
}

#endif
