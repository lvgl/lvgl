/**
 * @file lv_cpicker.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_cpicker.h"
#if LV_USE_CPICKER != 0

#include "../lv_core/lv_debug.h"
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

#ifndef LV_CPICKER_DEF_HSV
#define LV_CPICKER_DEF_HSV ((lv_color_hsv_t){LV_CPICKER_DEF_HUE, LV_CPICKER_DEF_SATURATION, LV_CPICKER_DEF_VALUE})
#endif

#ifndef LV_CPICKER_DEF_INDICATOR_TYPE
#define LV_CPICKER_DEF_INDICATOR_TYPE LV_CPICKER_INDICATOR_CIRCLE
#endif

#ifndef LV_CPICKER_DEF_QF /*quantization factor*/
#define LV_CPICKER_DEF_QF 3
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
static bool lv_cpicker_design(lv_obj_t * cpicker, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_cpicker_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param);

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
    LV_ASSERT_MEM(new_cpicker);
    if(new_cpicker == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_cpicker);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(new_cpicker);

    /*Allocate the extended data*/
    lv_cpicker_ext_t * ext = lv_obj_allocate_ext_attr(new_cpicker, sizeof(lv_cpicker_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */
    ext->hsv = LV_CPICKER_DEF_HSV;
    ext->prev_hsv = ext->hsv;
    ext->indicator.style = &lv_style_plain;
    ext->indicator.type = LV_CPICKER_DEF_INDICATOR_TYPE;
    ext->color_mode = LV_CPICKER_COLOR_MODE_HUE;
    ext->color_mode_fixed = 0;
    ext->last_click = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_cpicker, lv_cpicker_signal);
    lv_obj_set_design_cb(new_cpicker, lv_cpicker_design);

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
 * Set a new type for a cpicker
 * @param cpicker pointer to a cpicker object
 * @param type new type of the cpicker (from 'lv_cpicker_type_t' enum)
 */
void lv_cpicker_set_type(lv_obj_t * cpicker, lv_cpicker_type_t type)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    if(ext->type == type) return;

    ext->type = type;

    lv_obj_invalidate(cpicker);
}

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

    ext->hsv.h = hue % 360;

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

    ext->hsv.s = saturation % 100;

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

    ext->hsv.v = val % 100;

    lv_obj_invalidate(cpicker);
}

/**
 * Set the current hsv of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param color current selected hsv
 */
void lv_cpicker_set_hsv(lv_obj_t * cpicker, lv_color_hsv_t hsv)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    ext->hsv = hsv;

    lv_obj_invalidate(cpicker);
}

/**
 * Set the current color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param color current selected color
 */
void lv_cpicker_set_color(lv_obj_t * cpicker, lv_color_t color)
{
    lv_cpicker_set_hsv(cpicker, lv_color_rgb_to_hsv(color.ch.red, color.ch.green, color.ch.blue));
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
 * Get the current selected hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return hue current selected hue
 */
uint16_t lv_cpicker_get_hue(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->hsv.h;
}

/**
 * Get the current selected saturation of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected saturation
 */
uint8_t lv_cpicker_get_saturation(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->hsv.s;
}

/**
 * Get the current selected hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected value
 */
uint8_t lv_cpicker_get_value(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->hsv.v;
}

/**
 * Get the current selected hsv of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected hsv
 */
lv_color_hsv_t lv_cpicker_get_hsv(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->hsv;
}

/**
 * Get the current selected color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return color current selected color
 */
lv_color_t lv_cpicker_get_color(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return lv_color_hsv_to_rgb(ext->hsv.h, ext->hsv.s, ext->hsv.v);
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

static void lv_cpicker_disc_design(lv_obj_t * cpicker,
                                   lv_area_t * mask, lv_style_t * style, lv_opa_t opa_scale,
                                   lv_cpicker_ext_t * ext,
                                   lv_coord_t w, lv_coord_t h,
                                   lv_coord_t cx, lv_coord_t cy, uint16_t r);
static void lv_cpicker_rect_design(lv_obj_t * cpicker,
                                   lv_area_t * mask, lv_style_t * style, lv_opa_t opa_scale,
                                   lv_cpicker_ext_t * ext,
                                   lv_coord_t w, lv_coord_t h);

/**
 * Handle the drawing related tasks of the color_picker
 * @param cpicker pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @return true/false, depends on 'mode'
 */
static bool lv_cpicker_design(lv_obj_t * cpicker, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK)
    {
        return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN)
    {
        lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
        lv_style_t * style = lv_cpicker_get_style(cpicker, LV_CPICKER_STYLE_MAIN);

        static lv_style_t styleCopy;
        lv_style_copy(&styleCopy, style);

        lv_opa_t opa_scale = lv_obj_get_opa_scale(cpicker);

        lv_coord_t w = lv_obj_get_width(cpicker);
        lv_coord_t h = lv_obj_get_height(cpicker);

        if(ext->type == LV_CPICKER_TYPE_DISC)
        {
            lv_coord_t cx = cpicker->coords.x1 + (w / 2);
            lv_coord_t cy = cpicker->coords.y1 + (h / 2);
            uint16_t r = LV_MATH_MIN(w, h) / 2;
            lv_cpicker_disc_design(cpicker, mask, &styleCopy, opa_scale, ext, w, h, cx, cy, r);
        }
        else if(ext->type == LV_CPICKER_TYPE_RECT)
        {
            lv_cpicker_rect_design(cpicker, mask, &styleCopy, opa_scale, ext, w, h);
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST)
    {
    }

    return true;
}

static lv_color_t angle_to_mode_color(lv_cpicker_ext_t * ext, uint16_t angle)
{
    lv_color_t color;
    switch(ext->color_mode)
    {
    default:
    case LV_CPICKER_COLOR_MODE_HUE:
        color = lv_color_hsv_to_rgb(angle % 360, ext->hsv.s, ext->hsv.v);
        break;
    case LV_CPICKER_COLOR_MODE_SATURATION:
        color = lv_color_hsv_to_rgb(ext->hsv.h, (angle % 360) / 360.0 * 100.0, ext->hsv.v);
        break;
    case LV_CPICKER_COLOR_MODE_VALUE:
        color = lv_color_hsv_to_rgb(ext->hsv.h, ext->hsv.s, (angle % 360) / 360.0 * 100.0);
        break;
    }
    return color;
}

static uint16_t mode_color_to_angle(lv_cpicker_ext_t * ext)
{
    uint16_t angle;
    switch(ext->color_mode)
    {
    default:
    case LV_CPICKER_COLOR_MODE_HUE:
        angle = ext->hsv.h;
        break;
    case LV_CPICKER_COLOR_MODE_SATURATION:
        angle = ext->hsv.s / 100.0 * 360.0;
        break;
    case LV_CPICKER_COLOR_MODE_VALUE:
        angle = ext->hsv.v / 100.0 * 360.0;
        break;
    }
    return angle;
}

static lv_coord_t lv_cpicker_get_indicator_coord(lv_style_t * style, lv_cpicker_ext_t * ext)
{
    lv_coord_t ind_pos = style->line.rounded ? ext->rect_gradient_h / 2 : 0;
    switch(ext->color_mode)
    {
    default:
    case LV_CPICKER_COLOR_MODE_HUE:
        ind_pos += ext->hsv.h / 360.0 * ext->rect_gradient_w;
        break;
    case LV_CPICKER_COLOR_MODE_SATURATION:
        ind_pos += ext->hsv.s / 100.0 * ext->rect_gradient_w;
        break;
    case LV_CPICKER_COLOR_MODE_VALUE:
        ind_pos += ext->hsv.v / 100.0 * ext->rect_gradient_w;
        break;
    }
    return ind_pos;
}

/**
 * Should roughly match up with `lv_cpicker_invalidate_disc_indicator_line`
 */ 
static void lv_cpicker_draw_disc_indicator_line(lv_area_t * mask, lv_style_t * style, lv_opa_t opa_scale,
                                                lv_cpicker_ext_t * ext,
                                                lv_coord_t cx, lv_coord_t cy, uint16_t r,
                                                uint16_t angle)
{
    lv_point_t start;
    lv_point_t end;
    start.x = cx + ((r - style->line.width + ext->indicator.style->body.padding.inner + ext->indicator.style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    start.y = cy + ((r - style->line.width + ext->indicator.style->body.padding.inner + ext->indicator.style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);
    end.x = cx + ((r - ext->indicator.style->body.padding.inner - ext->indicator.style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    end.y = cy + ((r - ext->indicator.style->body.padding.inner - ext->indicator.style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

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
}

/**
 * Should roughly match up with `lv_cpicker_invalidate_disc_indicator_circle`
 */ 
static void lv_cpicker_draw_disc_indicator_circle(lv_area_t * mask, lv_style_t * style, lv_opa_t opa_scale,
                                                  lv_cpicker_ext_t * ext,
                                                  lv_coord_t cx, lv_coord_t cy, uint16_t r,
                                                  uint16_t angle)
{
    uint32_t ind_cx = cx + ((r - style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    uint32_t ind_cy = cy + ((r - style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

    lv_area_t ind_area;
    ind_area.x1 = ind_cx - style->line.width/2;
    ind_area.y1 = ind_cy - style->line.width/2;
    ind_area.x2 = ind_cx + style->line.width/2;
    ind_area.y2 = ind_cy + style->line.width/2;

    lv_style_t styleCopy;
    lv_style_copy(&styleCopy, ext->indicator.style);
    styleCopy.body.radius = LV_RADIUS_CIRCLE;

    lv_draw_rect(&ind_area, mask, &styleCopy, opa_scale);
}

/**
 * Should roughly match up with `lv_cpicker_invalidate_disc_indicator_in`
 */ 
static void lv_cpicker_draw_disc_indicator_in(lv_area_t * mask, lv_style_t * style, lv_opa_t opa_scale,
                                              lv_cpicker_ext_t * ext,
                                              lv_coord_t cx, lv_coord_t cy, uint16_t r,
                                              uint16_t rin, uint16_t angle)
{
    uint16_t ind_radius = lv_sqrt((4*rin*rin)/2)/2 + 1 - style->body.padding.inner;
    ind_radius = (ind_radius + rin) / 2;

    uint32_t ind_cx = cx + ((ind_radius) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    uint32_t ind_cy = cy + ((ind_radius) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

    lv_area_t ind_area;
    ind_area.x1 = ind_cx - r;
    ind_area.y1 = ind_cy - r;
    ind_area.x2 = ind_cx + r;
    ind_area.y2 = ind_cy + r;

    lv_style_t styleCopy;
    lv_style_copy(&styleCopy, ext->indicator.style);
    styleCopy.body.radius = LV_RADIUS_CIRCLE;

    lv_draw_rect(&ind_area, mask, &styleCopy, opa_scale);
}

static void lv_cpicker_draw_disc_indicator(lv_area_t * mask, lv_style_t * style, lv_opa_t opa_scale,
                                           lv_cpicker_ext_t * ext,
                                           lv_coord_t cx, lv_coord_t cy, lv_coord_t r,
                                           uint16_t rin, uint16_t radius, lv_area_t center_ind_area)
{
    /*draw center background*/
    static lv_style_t styleCenterBackground;
    lv_theme_t * th = lv_theme_get_current();
    if (th) {
        lv_style_copy(&styleCenterBackground, th->style.bg);
    } else {
        lv_style_copy(&styleCenterBackground, &lv_style_plain);
    }

    lv_area_t center_area;
    center_area.x1 = cx - rin;
    center_area.y1 = cy - rin;
    center_area.x2 = cx + rin;
    center_area.y2 = cy + rin;
    styleCenterBackground.body.radius = LV_RADIUS_CIRCLE;
    lv_draw_rect(&center_area, mask, &styleCenterBackground, opa_scale);

    /*draw the center color indicator*/
    style->body.main_color = lv_color_hsv_to_rgb(ext->hsv.h, ext->hsv.s, ext->hsv.v);
    style->body.grad_color = style->body.main_color;
    style->body.radius = LV_RADIUS_CIRCLE;
    lv_draw_rect(&center_ind_area, mask, style, opa_scale);

    uint16_t angle = mode_color_to_angle(ext);
    /*save the angle to refresh the area later*/
    ext->prev_pos = angle;

    /*draw the current hue indicator*/
    switch(ext->indicator.type)
    {
    case LV_CPICKER_INDICATOR_NONE:
        /*no indicator*/
        break;
    case LV_CPICKER_INDICATOR_LINE:
        lv_cpicker_draw_disc_indicator_line(mask, style, opa_scale, ext, cx, cy, r, angle);
        break;
    case LV_CPICKER_INDICATOR_CIRCLE:
        lv_cpicker_draw_disc_indicator_circle(mask, style, opa_scale, ext, cx, cy, r, angle);
        break;
    case LV_CPICKER_INDICATOR_IN:
        lv_cpicker_draw_disc_indicator_in(mask, style, opa_scale, ext, cx, cy, (rin - radius) / 3, rin, angle);
        break;
    }
}

static void lv_cpicker_draw_disc_gradient(lv_area_t * mask, lv_style_t * style, lv_opa_t opa_scale,
                                          lv_cpicker_ext_t * ext,
                                          lv_coord_t cx, lv_coord_t cy, uint16_t r)
{
    int16_t start_angle = 0; /*Default*/
    int16_t end_angle = 360 - LV_CPICKER_DEF_QF; /*Default*/

    /*if the mask does not include the center of the object
     * redrawing all the wheel is not necessary;
     * only a given angular range
     */
    lv_point_t center = {cx, cy};
    if(!lv_area_is_point_on(mask, &center)
        /*
        && (mask->x1 != cpicker->coords.x1 || mask->x2 != cpicker->coords.x2
        ||  mask->y1 != cpicker->coords.y1 || mask->y2 != cpicker->coords.y2)
            */
    )
    {
        /*get angle from center of object to each corners of the area*/
        int16_t dr, ur, ul, dl;
        dr = lv_atan2(mask->x2 - cx, mask->y2 - cy);
        ur = lv_atan2(mask->x2 - cx, mask->y1 - cy);
        ul = lv_atan2(mask->x1 - cx, mask->y1 - cy);
        dl = lv_atan2(mask->x1 - cx, mask->y2 - cy);

        /*check area position from object axis*/
        uint8_t left = (mask->x2 < cx && mask->x1 < cx);
        uint8_t onYaxis = (mask->x2 > cx && mask->x1 < cx);
        uint8_t right = (mask->x2 > cx && mask->x1 > cx);
        uint8_t top = (mask->y2 < cy && mask->y1 < cy);
        uint8_t onXaxis = (mask->y2 > cy && mask->y1 < cy);
        uint8_t bottom = (mask->y2 > cy && mask->y1 > cy);

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

    lv_point_t triangle_points[3];

    if(ext->color_mode == LV_CPICKER_COLOR_MODE_HUE)
    {
        for(uint16_t i = start_angle; i <= end_angle; i+= LV_CPICKER_DEF_QF)
        {
            style->body.main_color = angle_to_mode_color(ext, i);
            style->body.grad_color = style->body.main_color;

            triangle_points[0].x = cx;
            triangle_points[0].y = cy;

            triangle_points[1].x = cx + (r * lv_trigo_sin(i) >> LV_TRIGO_SHIFT);
            triangle_points[1].y = cy + (r * lv_trigo_sin(i + 90) >> LV_TRIGO_SHIFT);

            if(i == end_angle || i == (360 - LV_CPICKER_DEF_QF))
            {
                /*the last triangle is drawn without additional overlapping pixels*/
                triangle_points[2].x = cx + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF) >> LV_TRIGO_SHIFT);
                triangle_points[2].y = cy + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + 90) >> LV_TRIGO_SHIFT);
            }
            else
            {
                triangle_points[2].x = cx + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET) >> LV_TRIGO_SHIFT);
                triangle_points[2].y = cy + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET + 90) >> LV_TRIGO_SHIFT);
            }

            lv_draw_triangle(triangle_points, mask, style, LV_OPA_COVER);
        }
    }
    else if(ext->color_mode == LV_CPICKER_COLOR_MODE_SATURATION)
    {
        for(uint16_t i = start_angle; i <= end_angle; i += LV_CPICKER_DEF_QF)
        {
            style->body.main_color = angle_to_mode_color(ext, i);
            style->body.grad_color = style->body.main_color;

            triangle_points[0].x = cx;
            triangle_points[0].y = cy;

            triangle_points[1].x = cx + (r * lv_trigo_sin(i) >> LV_TRIGO_SHIFT);
            triangle_points[1].y = cy + (r * lv_trigo_sin(i + 90) >> LV_TRIGO_SHIFT);

            if(i == end_angle || i == (360 - LV_CPICKER_DEF_QF))
            {
                /*the last triangle is drawn without additional overlapping pixels*/
                triangle_points[2].x = cx + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF) >> LV_TRIGO_SHIFT);
                triangle_points[2].y = cy + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + 90) >> LV_TRIGO_SHIFT);
            }
            else
            {
                triangle_points[2].x = cx + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET) >> LV_TRIGO_SHIFT);
                triangle_points[2].y = cy + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET + 90) >> LV_TRIGO_SHIFT);
            }

            lv_draw_triangle(triangle_points, mask, style, LV_OPA_COVER);
        }
    }
    else if(ext->color_mode == LV_CPICKER_COLOR_MODE_VALUE)
    {
        for(uint16_t i = start_angle; i <= end_angle; i += LV_CPICKER_DEF_QF)
        {
            style->body.main_color = angle_to_mode_color(ext, i);
            style->body.grad_color = style->body.main_color;

            triangle_points[0].x = cx;
            triangle_points[0].y = cy;

            triangle_points[1].x = cx + (r * lv_trigo_sin(i) >> LV_TRIGO_SHIFT);
            triangle_points[1].y = cy + (r * lv_trigo_sin(i + 90) >> LV_TRIGO_SHIFT);

            if(i == end_angle || i == (360 - LV_CPICKER_DEF_QF))
            {
                /*the last triangle is drawn without additional overlapping pixels*/
                triangle_points[2].x = cx + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF) >> LV_TRIGO_SHIFT);
                triangle_points[2].y = cy + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + 90) >> LV_TRIGO_SHIFT);
            }
            else
            {
                triangle_points[2].x = cx + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET) >> LV_TRIGO_SHIFT);
                triangle_points[2].y = cy + (r * lv_trigo_sin(i + LV_CPICKER_DEF_QF + TRI_OFFSET + 90) >> LV_TRIGO_SHIFT);
            }

            lv_draw_triangle(triangle_points, mask, style, LV_OPA_COVER);
        }
    }
}

/**
 * Should roughly match up with `lv_cpicker_invalidate_disc`
 */ 
static void lv_cpicker_disc_design(lv_obj_t * cpicker,
                                   lv_area_t * mask, lv_style_t * style, lv_opa_t opa_scale,
                                   lv_cpicker_ext_t * ext,
                                   lv_coord_t w, lv_coord_t h,
                                   lv_coord_t cx, lv_coord_t cy, uint16_t r)
{
    uint16_t rin = r - style->line.width;
    /*
    the square area (a and b being sides) should fit into the center of diameter d
    we have:
    a^2+b^2<=d^2
    2a^2 <= d^2
    a^2<=(d^2)/2
    a <= sqrt((d^2)/2)
    */
    uint16_t radius = lv_sqrt((4*rin*rin)/2)/2 - style->body.padding.inner;

    lv_area_t center_ind_area;
    center_ind_area.x1 = cx - radius;
    center_ind_area.y1 = cy - radius;
    center_ind_area.x2 = cx + radius;
    center_ind_area.y2 = cy + radius;

    /*redraw the wheel only if the mask intersect with the wheel*/
    if(mask->x1 < center_ind_area.x1 || mask->x2 > center_ind_area.x2
            || mask->y1 < center_ind_area.y1 || mask->y2 > center_ind_area.y2)
    {
        lv_cpicker_draw_disc_gradient(mask, style, opa_scale, ext, cx, cy, r);
    }

    lv_cpicker_draw_disc_indicator(mask, style, opa_scale, ext, cx, cy, r, rin, radius, center_ind_area);

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

static uint16_t lv_cpicker_calculate_rect_preview_area(lv_obj_t * cpicker,
                                                       lv_style_t * style,
                                                       lv_cpicker_ext_t * ext,
                                                       lv_coord_t w)
{
    lv_coord_t x1 = cpicker->coords.x1;
    lv_coord_t y1 = cpicker->coords.y1;
    lv_coord_t x2 = cpicker->coords.x2;
    lv_coord_t y2 = cpicker->coords.y2;

    /*prepare the color preview area*/
    uint16_t preview_offset = style->line.width;
    uint16_t style_body_padding_ver = style->body.padding.top + style->body.padding.bottom;
    uint16_t style_body_padding_hor = style->body.padding.left + style->body.padding.right;
    if(style_body_padding_ver == 0)
    {
        /*draw the color preview rect to the side of the gradient*/
        if(style_body_padding_hor >= 0)
        {
            /*draw the preview to the right*/
            ext->rect_gradient_w = w - preview_offset - (LV_MATH_ABS(style_body_padding_hor) - 1);
            ext->rect_gradient_h = y2 - y1;
            ext->rect_gradient_area.x1 = x1;
            ext->rect_gradient_area.x2 = ext->rect_gradient_area.x1 + ext->rect_gradient_w;
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
            ext->rect_gradient_w = w - preview_offset - (LV_MATH_ABS(style_body_padding_hor) - 1);
            ext->rect_gradient_h = y2 - y1;
            ext->rect_gradient_area.x1 = x2 - ext->rect_gradient_w;
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
        /*draw the color preview rect on top or below the gradient*/
        if(style_body_padding_ver >= 0)
        {
            /*draw the preview on top*/
            ext->rect_gradient_w = w;
            ext->rect_gradient_h = (y2 - y1) - preview_offset - (LV_MATH_ABS(style_body_padding_ver) - 1);
            ext->rect_gradient_area.x1 = x1;
            ext->rect_gradient_area.x2 = x2;
            ext->rect_gradient_area.y1 = y2 - ext->rect_gradient_h;
            ext->rect_gradient_area.y2 = y2;

            ext->rect_preview_area.x1 = x1;
            ext->rect_preview_area.y1 = y1;
            ext->rect_preview_area.x2 = x2;
            ext->rect_preview_area.y2 = y1 + preview_offset;
        }
        else
        {
            /*draw the preview below the gradient*/
            ext->rect_gradient_w = w;
            ext->rect_gradient_h = (y2 - y1) - preview_offset - (LV_MATH_ABS(style_body_padding_ver) - 1);
            ext->rect_gradient_area.x1 = x1;
            ext->rect_gradient_area.x2 = x2;
            ext->rect_gradient_area.y1 = y1;
            ext->rect_gradient_area.y2 = y1 + ext->rect_gradient_h;

            ext->rect_preview_area.x1 = x1;
            ext->rect_preview_area.y1 = y2 - preview_offset;
            ext->rect_preview_area.x2 = x2;
            ext->rect_preview_area.y2 = y2;
        }
    }

    return style_body_padding_hor;
}

/**
 * Should roughly match up with `lv_cpicker_invalidate_rect_indicator_line`
 */ 
static void lv_cpicker_draw_rect_indicator_line(lv_area_t * mask, lv_opa_t opa_scale,
                                                lv_cpicker_ext_t * ext, lv_coord_t ind_pos)
{
    lv_point_t p1, p2;
    p1.x = ext->rect_gradient_area.x1 + ind_pos;
    p1.y = ext->rect_gradient_area.y1;
    p2.x = p1.x;
    p2.y = ext->rect_gradient_area.y2;

    lv_draw_line(&p1, &p2, mask, ext->indicator.style, opa_scale);
}

/**
 * Should roughly match up with `lv_cpicker_invalidate_rect_indicator_circle`
 */ 
static void lv_cpicker_draw_rect_indicator_circle(lv_area_t * mask, lv_opa_t opa_scale,
                                                  lv_cpicker_ext_t * ext, lv_coord_t ind_pos)
{
    lv_area_t circle_ind_area;
    circle_ind_area.x1 = ext->rect_gradient_area.x1 + ind_pos - ext->rect_gradient_h/2;
    circle_ind_area.x2 = circle_ind_area.x1 + ext->rect_gradient_h;
    circle_ind_area.y1 = ext->rect_gradient_area.y1;
    circle_ind_area.y2 = ext->rect_gradient_area.y2;

    lv_style_t styleCopy;
    lv_style_copy(&styleCopy, ext->indicator.style);
    styleCopy.body.radius = LV_RADIUS_CIRCLE;

    lv_draw_rect(&circle_ind_area, mask, &styleCopy, opa_scale);
}

/**
 * Should roughly match up with `lv_cpicker_invalidate_rect_indicator_in`
 */ 
static void lv_cpicker_draw_rect_indicator_in(lv_area_t * mask, lv_opa_t opa_scale,
                                              lv_cpicker_ext_t * ext, lv_coord_t ind_pos)
{
    /*draw triangle at top and bottom of gradient*/
    lv_point_t triangle_points[3];

    triangle_points[0].x = ext->rect_gradient_area.x1 + ind_pos;
    triangle_points[0].y = ext->rect_gradient_area.y1 + (ext->rect_gradient_h/3);

    triangle_points[1].x = triangle_points[0].x - ext->indicator.style->line.width * 3;
    triangle_points[1].y = ext->rect_gradient_area.y1 - 1;

    triangle_points[2].x = triangle_points[0].x + ext->indicator.style->line.width * 3;
    triangle_points[2].y = triangle_points[1].y;

    lv_draw_triangle(triangle_points, mask, ext->indicator.style, LV_OPA_COVER);

    triangle_points[0].y = ext->rect_gradient_area.y2 - (ext->rect_gradient_h/3);
    triangle_points[1].y = ext->rect_gradient_area.y2;
    triangle_points[2].y = triangle_points[1].y;
    lv_draw_triangle(triangle_points, mask, ext->indicator.style, LV_OPA_COVER);
}

static void lv_cpicker_draw_rect_indicator(lv_obj_t * cpicker,
                                           lv_area_t * mask, lv_style_t * style, lv_opa_t opa_scale,
                                           lv_cpicker_ext_t * ext,
                                           uint16_t style_body_padding_hor)
{
    /*draw the color preview indicator*/
    style->body.main_color = lv_cpicker_get_color(cpicker);
    style->body.grad_color = style->body.main_color;
    if(style->line.rounded && style_body_padding_hor == 0)
    {
        style->body.radius = ext->rect_gradient_h;
    }
    lv_draw_rect(&(ext->rect_preview_area), mask, style, opa_scale);

    /*
    styleCopy.line.width = 10;
    lv_draw_arc(cpicker->coords.x1 + 3*ext->rect_gradient_h/2, cpicker->coords.y1 + ext->rect_gradient_h/2, ext->rect_gradient_h / 2 + styleCopy.line.width + 2, mask, 180, 360, &styleCopy, opa_scale);
    //lv_draw_arc(cpicker->coords.x1 + ext->rect_gradient_w - ext->rect_gradient_h/2, cpicker->coords.y1 + ext->rect_gradient_h/2, ext->rect_gradient_h / 2 + styleCopy.line.width + 2, mask, 0, 180, &styleCopy, opa_scale);
    */

    /*draw the color position indicator*/
    lv_coord_t ind_pos = lv_cpicker_get_indicator_coord(style, ext);
    /*save to refresh the area later*/
    ext->prev_pos = ind_pos;

    switch(ext->indicator.type)
    {
    case LV_CPICKER_INDICATOR_NONE:
        /*no indicator*/
        break;
    case LV_CPICKER_INDICATOR_LINE:
        lv_cpicker_draw_rect_indicator_line(mask, opa_scale, ext, ind_pos);
        break;
    case LV_CPICKER_INDICATOR_CIRCLE:
        lv_cpicker_draw_rect_indicator_circle(mask, opa_scale, ext, ind_pos);
        break;
    case LV_CPICKER_INDICATOR_IN:
        lv_cpicker_draw_rect_indicator_in(mask, opa_scale, ext, ind_pos);
        break;
    default:
        break;
    }
}

static void lv_cpicker_draw_rect_gradient(lv_area_t * mask, lv_style_t * style, lv_opa_t opa_scale,
                                          lv_cpicker_ext_t * ext)
{
    if(style->line.rounded)
    {
        /*draw rounded edges to the gradient*/
        lv_area_t rounded_edge_area;
        rounded_edge_area.x1 = ext->rect_gradient_area.x1;
        rounded_edge_area.x2 = ext->rect_gradient_area.x1 + ext->rect_gradient_h;
        rounded_edge_area.y1 = ext->rect_gradient_area.y1;
        rounded_edge_area.y2 = ext->rect_gradient_area.y2;

        ext->rect_gradient_area.x1 += ext->rect_gradient_h/2;
        ext->rect_gradient_area.x2 -= ext->rect_gradient_h/2;
        ext->rect_gradient_w -= ext->rect_gradient_h;

        style->body.main_color = angle_to_mode_color(ext, 0);
        style->body.grad_color = style->body.main_color;

        style->body.radius = LV_RADIUS_CIRCLE;

        lv_draw_rect(&rounded_edge_area, mask, style, opa_scale);

        rounded_edge_area.x1 += ext->rect_gradient_w - 1;
        rounded_edge_area.x2 += ext->rect_gradient_w - 1;

        style->body.main_color = angle_to_mode_color(ext, 360);
        style->body.grad_color = style->body.main_color;

        lv_draw_rect(&rounded_edge_area, mask, style, opa_scale);
    }

    for(uint16_t i = 0; i < 360; i += LV_MATH_MAX(LV_CPICKER_DEF_QF, 360/ext->rect_gradient_w))
    {
        style->body.main_color = angle_to_mode_color(ext, i);
        style->body.grad_color = style->body.main_color;

        /*the following attribute might need changing between index to add border, shadow, radius etc*/
        style->body.radius = 0;
        style->body.border.width = 0;
        style->body.shadow.width = 0;
        style->body.opa = LV_OPA_COVER;

        lv_area_t rect_area;

        /*scale angle (hue/sat/val) to linear coordinate*/
        lv_coord_t xi = i / 360.0 * ext->rect_gradient_w;

        rect_area.x1 = LV_MATH_MIN(ext->rect_gradient_area.x1 + xi, ext->rect_gradient_area.x1 + ext->rect_gradient_w - LV_MATH_MAX(LV_CPICKER_DEF_QF, 360/ext->rect_gradient_w));
        rect_area.y1 = ext->rect_gradient_area.y1;
        rect_area.x2 = rect_area.x1 + LV_MATH_MAX(LV_CPICKER_DEF_QF, 360/ext->rect_gradient_w);
        rect_area.y2 = ext->rect_gradient_area.y2;

        lv_draw_rect(&rect_area, mask, style, opa_scale);
    }

    if(style->line.rounded)
    {
        /*Restore gradient area to take rounded end in account*/
        ext->rect_gradient_area.x1 -= ext->rect_gradient_h/2;
        ext->rect_gradient_area.x2 += ext->rect_gradient_h/2;
        //ext->rect_gradient_w += ext->rect_gradient_h;
    }
}

/**
 * Should roughly match up with `lv_cpicker_invalidate_rect`
 */ 
static void lv_cpicker_rect_design(lv_obj_t * cpicker,
                                   lv_area_t * mask, lv_style_t * style, lv_opa_t opa_scale,
                                   lv_cpicker_ext_t * ext,
                                   lv_coord_t w, lv_coord_t h)
{
    uint16_t style_body_padding_hor = lv_cpicker_calculate_rect_preview_area(cpicker, style, ext, w);

    lv_cpicker_draw_rect_gradient(mask, style, opa_scale, ext);

    lv_cpicker_draw_rect_indicator(cpicker, mask, style, opa_scale, ext, style_body_padding_hor);
}


static void lv_cpicker_invalidate(lv_obj_t * cpicker, bool all);

static lv_res_t lv_cpicker_disc_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param,
                                       lv_style_t * style, lv_cpicker_ext_t * ext,
                                       lv_coord_t r_out, lv_coord_t r_in, lv_coord_t x, lv_coord_t y);
static lv_res_t lv_cpicker_rect_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param,
                                       lv_style_t * style, lv_cpicker_ext_t * ext);

/**
 * Signal function of the color_picker
 * @param cpicker pointer to a color_picker object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_cpicker_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param)
{
    /* Include the ancient signal function */
    lv_res_t res = ancestor_signal(cpicker, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_cpicker";
    } else {
        lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

        if(sign == LV_SIGNAL_CONTROL) {
            uint32_t c = *((uint32_t *)param); /*uint32_t because can be UTF-8*/
            if(c == LV_KEY_RIGHT)
            {
                switch(ext->color_mode)
                {
                case LV_CPICKER_COLOR_MODE_HUE:
                    ext->hsv.h = (ext->hsv.h + 1) % 360;
                    break;
                case LV_CPICKER_COLOR_MODE_SATURATION:
                    ext->hsv.s = (ext->hsv.s + 1) % 100;
                    break;
                case LV_CPICKER_COLOR_MODE_VALUE:
                    ext->hsv.v = (ext->hsv.v + 1) % 100;
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
                    ext->hsv.h = ext->hsv.h > 0?(ext->hsv.h - 1):360;
                    break;
                case LV_CPICKER_COLOR_MODE_SATURATION:
                    ext->hsv.s = ext->hsv.s > 0?(ext->hsv.s - 1):100;
                    break;
                case LV_CPICKER_COLOR_MODE_VALUE:
                    ext->hsv.v = ext->hsv.v > 0?(ext->hsv.v - 1):100;
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
                    ext->hsv.h = (ext->hsv.h + 1) % 360;
                    break;
                case LV_CPICKER_COLOR_MODE_SATURATION:
                    ext->hsv.s = (ext->hsv.s + 1) % 100;
                    break;
                case LV_CPICKER_COLOR_MODE_VALUE:
                    ext->hsv.v = (ext->hsv.v + 1) % 100;
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
                    ext->hsv.h = ext->hsv.h > 0?(ext->hsv.h - 1):360;
                    break;
                case LV_CPICKER_COLOR_MODE_SATURATION:
                    ext->hsv.s = ext->hsv.s > 0?(ext->hsv.s - 1):100;
                    break;
                case LV_CPICKER_COLOR_MODE_VALUE:
                    ext->hsv.v = ext->hsv.v > 0?(ext->hsv.v - 1):100;
                    break;
                }

                lv_cpicker_invalidate(cpicker, false);

                res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return res;
            }
        } else {
            lv_style_t * style = lv_cpicker_get_style(cpicker, LV_CPICKER_STYLE_MAIN);

            if(ext->type == LV_CPICKER_TYPE_DISC)
            {
                lv_coord_t r_out = (LV_MATH_MIN(lv_obj_get_width(cpicker), lv_obj_get_height(cpicker))) / 2;
                lv_coord_t r_in = r_out - style->line.width - style->body.padding.inner;
                lv_coord_t x = cpicker->coords.x1 + lv_obj_get_width(cpicker) / 2;
                lv_coord_t y = cpicker->coords.y1 + lv_obj_get_height(cpicker) / 2;
                res = lv_cpicker_disc_signal(cpicker, sign, param, style, ext, r_out, r_in, x, y);
                if(res != LV_RES_OK) return res;
            }
            else if(ext->type == LV_CPICKER_TYPE_RECT)
            {
                res = lv_cpicker_rect_signal(cpicker, sign, param, style, ext);
                if(res != LV_RES_OK) return res;
            }
        }
    }

    return res;
}

static lv_res_t lv_cpicker_reset_hsv_if_double_clicked(lv_obj_t * cpicker,
                                                       lv_cpicker_ext_t * ext)
{
    lv_res_t res;

    if(lv_tick_elaps(ext->last_click) < 400)
    {
        switch(ext->color_mode)
        {
        case LV_CPICKER_COLOR_MODE_HUE:
            ext->hsv.h = LV_CPICKER_DEF_HSV.h;
            break;
        case LV_CPICKER_COLOR_MODE_SATURATION:
            ext->hsv.s = LV_CPICKER_DEF_HSV.s;
            break;
        case LV_CPICKER_COLOR_MODE_VALUE:
            ext->hsv.v = LV_CPICKER_DEF_HSV.v;
            break;
        }
        ext->prev_hsv = ext->hsv;
        lv_cpicker_invalidate(cpicker, false);

        res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
    }
    ext->last_click = lv_tick_get();

    return res;
}

static void lv_cpicker_set_next_color_mode(lv_obj_t * cpicker,
                                           lv_cpicker_ext_t * ext)
{
    ext->prev_hsv = ext->hsv;
    ext->color_mode = (ext->color_mode + 1) % 3;
    lv_cpicker_invalidate(cpicker, true);
}

static lv_res_t lv_cpicker_set_hsv_percent(lv_obj_t * cpicker,
                                           lv_cpicker_ext_t * ext,
                                           float percent)
{
    lv_res_t res;

    bool changed = false;
    uint16_t hsv;
    switch(ext->color_mode)
    {
    case LV_CPICKER_COLOR_MODE_HUE:
        hsv = percent * 360.0;
        changed = hsv != ext->hsv.h;
        if (changed)
        {
            ext->hsv.h = hsv;
        }
        break;
    case LV_CPICKER_COLOR_MODE_SATURATION:
        hsv = percent * 100.0;
        changed = hsv != ext->hsv.s;
        if (changed)
        {
            ext->hsv.s = hsv;
        }
        break;
    case LV_CPICKER_COLOR_MODE_VALUE:
        hsv = percent * 100.0;
        changed = hsv != ext->hsv.v;
        if (changed)
        {
            ext->hsv.v = hsv;
        }
        break;
    }
    ext->prev_hsv = ext->hsv;

    if (changed)
    {
        lv_cpicker_invalidate(cpicker, false);

        res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
        if(res != LV_RES_OK) return res;
    }

    return res;
}

static lv_res_t lv_cpicker_disc_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param,
                                       lv_style_t * style, lv_cpicker_ext_t * ext,
                                       lv_coord_t r_out, lv_coord_t r_in, lv_coord_t x, lv_coord_t y)
{
    lv_res_t res;

    if(sign == LV_SIGNAL_PRESSED)
    {
        ext->prev_hsv = ext->hsv;

        lv_indev_t * indev = param;

        lv_coord_t xp = indev->proc.types.pointer.act_point.x - x;
        lv_coord_t yp = indev->proc.types.pointer.act_point.y - y;
        if((xp*xp + yp*yp) < (r_in*r_in))
        {
            res = lv_cpicker_reset_hsv_if_double_clicked(cpicker, ext);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_PRESSING)
    {
        lv_indev_t * indev = param;

        lv_coord_t xp = indev->proc.types.pointer.act_point.x - x;
        lv_coord_t yp = indev->proc.types.pointer.act_point.y - y;
        if((xp*xp + yp*yp) < (r_out*r_out) && (xp*xp + yp*yp) >= (r_in*r_in))
        {
            float percent = lv_atan2(xp, yp) / 360.0;

            res = lv_cpicker_set_hsv_percent(cpicker, ext, percent);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_PRESS_LOST)
    {
        ext->prev_hsv = ext->hsv;
        lv_cpicker_invalidate(cpicker, false);
    }
    else if(sign == LV_SIGNAL_RELEASED)
    {
        lv_indev_t * indev = param;

        lv_coord_t xp = indev->proc.types.pointer.act_point.x - x;
        lv_coord_t yp = indev->proc.types.pointer.act_point.y - y;
        if((xp*xp + yp*yp) < (r_out*r_out) && (xp*xp + yp*yp) >= (r_in*r_in))
        {
            float percent = lv_atan2(xp, yp) / 360.0;

            res = lv_cpicker_set_hsv_percent(cpicker, ext, percent);
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
                lv_cpicker_set_next_color_mode(cpicker, ext);
            }
        }
    }

    return res;
}

static lv_res_t lv_cpicker_rect_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param,
                                       lv_style_t * style, lv_cpicker_ext_t * ext)
{
    lv_res_t res;

    if(sign == LV_SIGNAL_PRESSED)
    {
        ext->prev_hsv = ext->hsv;

        lv_indev_t * indev = param;

        if(lv_area_is_point_on(&(ext->rect_preview_area), &indev->proc.types.pointer.act_point))
        {
            res = lv_cpicker_reset_hsv_if_double_clicked(cpicker, ext);
            if(res != LV_RES_OK) return res;
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

            res = lv_cpicker_set_hsv_percent(cpicker, ext, percent);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_PRESS_LOST)
    {
        ext->prev_hsv = ext->hsv;
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

            res = lv_cpicker_set_hsv_percent(cpicker, ext, percent);
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
                lv_cpicker_set_next_color_mode(cpicker, ext);
            }
        }
    }

    return res;
}


static void lv_cpicker_invalidate_disc(lv_disp_t * disp, lv_style_t * style,
                                       lv_cpicker_ext_t * ext,
                                       lv_coord_t w, lv_coord_t h,
                                       lv_coord_t cx, lv_coord_t cy, uint16_t r);
static void lv_cpicker_invalidate_rect(lv_disp_t * disp, lv_style_t * style,
                                       lv_cpicker_ext_t * ext,
                                       lv_coord_t w, lv_coord_t h);

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

    lv_disp_t * disp = lv_obj_get_disp(cpicker);
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    lv_style_t * style = lv_cpicker_get_style(cpicker, LV_CPICKER_STYLE_MAIN);

    lv_style_t styleCopy;
    lv_style_copy(&styleCopy, style);

    lv_coord_t w = lv_obj_get_width(cpicker);
    lv_coord_t h = lv_obj_get_height(cpicker);

    if(ext->type == LV_CPICKER_TYPE_DISC)
    {
        lv_coord_t cx = cpicker->coords.x1 + w / 2;
        lv_coord_t cy = cpicker->coords.y1 + h / 2;
        uint16_t r = LV_MATH_MIN(w, h) / 2;
        lv_cpicker_invalidate_disc(disp, &styleCopy, ext, w, h, cx, cy, r);
    }
    else if(ext->type == LV_CPICKER_TYPE_RECT)
    {
        lv_cpicker_invalidate_rect(disp, &styleCopy, ext, w, h);
    }
}

/**
 * Should roughly match up with `lv_cpicker_draw_disc_indicator_line`
 */ 
static void lv_cpicker_invalidate_disc_indicator_line(lv_disp_t * disp, lv_style_t * style,
                                                      lv_cpicker_ext_t * ext,
                                                      lv_coord_t cx, lv_coord_t cy, uint16_t r,
                                                      uint16_t angle)
{
    lv_coord_t x1 = cx + ((r - style->line.width + ext->indicator.style->body.padding.inner + ext->indicator.style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    lv_coord_t y1 = cy + ((r - style->line.width + ext->indicator.style->body.padding.inner + ext->indicator.style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);
    lv_coord_t x2 = cx + ((r - ext->indicator.style->body.padding.inner - ext->indicator.style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    lv_coord_t y2 = cy + ((r - ext->indicator.style->body.padding.inner - ext->indicator.style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

    lv_point_t point1, point2;
    point1.x = x1;
    point1.y = y1;
    point2.x = x2;
    point2.y = y2;

    lv_area_t line_area;
    //if(LV_MATH_ABS(point1.x - point2.x) > LV_MATH_ABS(point1.y - point2.y))
    //{
    /*Steps less in y than x -> rather horizontal*/
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
    /*Steps less in x than y -> rather vertical*/
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
}

/**
 * Should roughly match up with `lv_cpicker_draw_disc_indicator_circle`
 */ 
static void lv_cpicker_invalidate_disc_indicator_circle(lv_disp_t * disp, lv_style_t * style,
                                                        lv_cpicker_ext_t * ext,
                                                        lv_coord_t cx, lv_coord_t cy, uint16_t r,
                                                        uint16_t angle)
{
    uint32_t ind_cx = cx + ((r - style->line.width/2) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    uint32_t ind_cy = cy + ((r - style->line.width/2) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

    lv_area_t ind_area;
    ind_area.x1 = ind_cx - style->line.width/2;
    ind_area.y1 = ind_cy - style->line.width/2;
    ind_area.x2 = ind_cx + style->line.width/2;
    ind_area.y2 = ind_cy + style->line.width/2;

    lv_inv_area(disp, &ind_area);
}

/**
 * Should roughly match up with `lv_cpicker_draw_disc_indicator_in`
 */ 
static void lv_cpicker_invalidate_disc_indicator_in(lv_disp_t * disp, lv_style_t * style,
                                                    lv_cpicker_ext_t * ext,
                                                    lv_coord_t x, lv_coord_t y, uint16_t r,
                                                    uint16_t rin, uint16_t angle)
{
    uint16_t ind_radius = lv_sqrt((4*rin*rin)/2)/2 + 1 - style->body.padding.inner;
    ind_radius = (ind_radius + rin) / 2;

    uint16_t ind_cx = x + ((ind_radius) * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    uint16_t ind_cy = y + ((ind_radius) * lv_trigo_sin(angle + 90) >> LV_TRIGO_SHIFT);

    lv_area_t ind_area;
    ind_area.x1 = ind_cx - r;
    ind_area.y1 = ind_cy - r;
    ind_area.x2 = ind_cx + r;
    ind_area.y2 = ind_cy + r;

    lv_inv_area(disp, &ind_area);
}

/**
 * Should roughly match up with `lv_cpicker_disc_design`
 */ 
static void lv_cpicker_invalidate_disc(lv_disp_t * disp, lv_style_t * style,
                                       lv_cpicker_ext_t * ext,
                                       lv_coord_t w, lv_coord_t h,
                                       lv_coord_t cx, lv_coord_t cy, uint16_t r)
{
    /*invalidate center color area*/
    uint16_t rin = r - style->line.width;
    /*
    the square area (a and b being sides) should fit into the center of diameter d
    we have:
    a^2+b^2<=d^2
    2a^2 <= d^2
    a^2<=(d^2)/2
    a <= sqrt((d^2)/2)
    */
    uint16_t radius = lv_sqrt((4*rin*rin)/2)/2 + 1 - style->body.padding.inner;

    lv_area_t center_color_area;
    center_color_area.x1 = cx - radius;
    center_color_area.y1 = cy - radius;
    center_color_area.x2 = cx + radius;
    center_color_area.y2 = cy + radius;

    lv_inv_area(disp, &center_color_area);

    /*invalidate indicator*/
    uint16_t angle = mode_color_to_angle(ext);
    switch(ext->indicator.type)
    {
    case LV_CPICKER_INDICATOR_LINE:
        lv_cpicker_invalidate_disc_indicator_line(disp, style, ext, cx, cy, r, angle);
        lv_cpicker_invalidate_disc_indicator_line(disp, style, ext, cx, cy, r, ext->prev_pos);
        break;
    case LV_CPICKER_INDICATOR_CIRCLE:
        lv_cpicker_invalidate_disc_indicator_circle(disp, style, ext, cx, cy, r, angle);
        lv_cpicker_invalidate_disc_indicator_circle(disp, style, ext, cx, cy, r, ext->prev_pos);
        break;
    case LV_CPICKER_INDICATOR_IN:
        lv_cpicker_invalidate_disc_indicator_in(disp, style, ext, cx, cy, (rin - radius) / 3, rin, angle);
        lv_cpicker_invalidate_disc_indicator_in(disp, style, ext, cx, cy, (rin - radius) / 3, rin, ext->prev_pos);
        break;
    }
}

/**
 * Should roughly match up with `lv_cpicker_draw_rect_indicator_line`
 */ 
static void lv_cpicker_invalidate_rect_indicator_line(lv_disp_t * disp,
                                                      lv_cpicker_ext_t * ext,
                                                      lv_coord_t ind_pos)
{
    lv_area_t line_area;
    line_area.x1 = ext->rect_gradient_area.x1 + ind_pos - ext->indicator.style->line.width;
    line_area.y1 = ext->rect_gradient_area.y1;
    line_area.x2 = ext->rect_gradient_area.x1 + ind_pos + ext->indicator.style->line.width;
    line_area.y2 = ext->rect_gradient_area.y2;

    lv_inv_area(disp, &line_area);
}

/**
 * Should roughly match up with `lv_cpicker_draw_rect_indicator_circle`
 */ 
static void lv_cpicker_invalidate_rect_indicator_circle(lv_disp_t * disp,
                                                        lv_cpicker_ext_t * ext,
                                                        lv_coord_t ind_pos)
{
    lv_area_t circle_ind_area;
    circle_ind_area.x1 = ext->rect_gradient_area.x1 + ind_pos - ext->rect_gradient_h/2;
    circle_ind_area.x2 = circle_ind_area.x1 + ext->rect_gradient_h;
    circle_ind_area.y1 = ext->rect_gradient_area.y1;
    circle_ind_area.y2 = ext->rect_gradient_area.y2;

    lv_inv_area(disp, &circle_ind_area);
}

/**
 * Should roughly match up with `lv_cpicker_draw_rect_indicator_in`
 */ 
static void lv_cpicker_invalidate_rect_indicator_in(lv_disp_t * disp,
                                                    lv_cpicker_ext_t * ext,
                                                    lv_coord_t ind_pos)
{
    lv_coord_t center = ext->rect_gradient_area.x1 + ind_pos;

    lv_area_t ind_area;
    ind_area.x1 = center - ext->indicator.style->line.width * 3;
    ind_area.y1 = ext->rect_gradient_area.y1 - 1;
    ind_area.x2 = center + ext->indicator.style->line.width * 3;
    ind_area.y2 = ext->rect_gradient_area.y2;

    lv_inv_area(disp, &ind_area);
}

/**
 * Should roughly match up with `lv_cpicker_rect_design`
 */ 
static void lv_cpicker_invalidate_rect(lv_disp_t * disp, lv_style_t * style,
                                       lv_cpicker_ext_t * ext,
                                       lv_coord_t w, lv_coord_t h)
{
    /*invalidate color preview indicator*/
    lv_inv_area(disp, &ext->rect_preview_area);

    /*invalidate indicator*/
    lv_coord_t ind_pos = lv_cpicker_get_indicator_coord(style, ext);
    switch(ext->indicator.type)
    {
    case LV_CPICKER_INDICATOR_LINE:
        lv_cpicker_invalidate_rect_indicator_line(disp, ext, ind_pos);
        lv_cpicker_invalidate_rect_indicator_line(disp, ext, ext->prev_pos);
        break;
    case LV_CPICKER_INDICATOR_CIRCLE:
        lv_cpicker_invalidate_rect_indicator_circle(disp, ext, ind_pos);
        lv_cpicker_invalidate_rect_indicator_circle(disp, ext, ext->prev_pos);
        break;
    case LV_CPICKER_INDICATOR_IN:
        lv_cpicker_invalidate_rect_indicator_in(disp, ext, ind_pos);
        lv_cpicker_invalidate_rect_indicator_in(disp, ext, ext->prev_pos);
        break;
    }
}

#endif /* LV_USE_CPICKER != 0 */
