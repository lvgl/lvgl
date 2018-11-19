/**
 * @file lv_cpicker.c
 *
 */

/* TODO Remove these instructions
 * Search an replace: color_picker -> object normal name with lower case (e.g. button, label etc.)
 *                    cpicker -> object short name with lower case(e.g. btn, label etc)
 *                    CPICKER -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_cpicker.h"
#include "../lv_misc/lv_math.h"
#include "../lv_draw/lv_draw_arc.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_core/lv_indev.h"

#if USE_LV_CPICKER != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_cpicker_design(lv_obj_t * cpicker, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_cpicker_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param);
static uint16_t fast_atan2(int x, int y);
static uint16_t fast_sqrt(unsigned int x);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_design_func_t ancestor_design;

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

    /*Create the ancestor of color_picker*/
    /*TODO modify it to the ancestor create function */
    lv_obj_t * new_cpicker = lv_obj_create(par, copy);
    lv_mem_assert(new_cpicker);
    if(new_cpicker == NULL) return NULL;

    /*Allocate the colorpicker type specific extended data*/
    lv_cpicker_ext_t * ext = lv_obj_allocate_ext_attr(new_cpicker, sizeof(lv_cpicker_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_cpicker);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_cpicker);

    /*Initialize the allocated 'ext' */
    ext->hue = 0;
    ext->prev_hue = 0;
    ext->saturation = 100;
    ext->value = 100;
    ext->ind.style = &lv_style_plain;
    ext->ind.type = LV_CPICKER_IND_CIRCLE;
    ext->value_changed = NULL;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_cpicker, lv_cpicker_signal);
    lv_obj_set_design_func(new_cpicker, lv_cpicker_design);

    /*Init the new cpicker color_picker*/
    if(copy == NULL) {

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            if(par == NULL) { /*Different styles if it is screen*/
                lv_cpicker_set_style(new_cpicker, LV_CPICKER_STYLE_MAIN, th->bg);
            } else {
                lv_cpicker_set_style(new_cpicker, LV_CPICKER_STYLE_MAIN, th->page.bg);
            }
        } else {
            lv_cpicker_set_style(new_cpicker, LV_CPICKER_STYLE_MAIN, &lv_style_plain);
        }
    }

    /*Copy an existing color_picker*/
    else {
        lv_cpicker_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_cpicker);
    }

    LV_LOG_INFO("colorpicker created");

    return new_cpicker;
}

/*======================
 * Add/remove functions
 *=====================*/

/*
 * New object specific "add" or "remove" functions come here
 */


/*=====================
 * Setter functions
 *====================*/

/*
 * New object specific "set" functions come here
 */


/**
 * Set a style of a color_picker.
 * @param cpicker pointer to color_picker object
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
    case LV_CPICKER_STYLE_IND:
        ext->ind.style = style;
        lv_obj_invalidate(cpicker);
        break;
    }
}

/**
 * Set the current hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param hue current selected hue
 */
void lv_cpicker_set_hue(lv_obj_t * cpicker, uint16_t hue)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    ext->hue = hue;

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

    lv_color_hsv_t hsv = lv_color_rgb_to_hsv(color.red, color.green, color.blue);
    ext->hue = hsv.h;

    lv_obj_invalidate(cpicker);
}

/**
 * Set the action callback on value change event.
 * @param cpicker pointer to colorpicker object
 * @param action callback function
 */
void lv_cpicker_set_action(lv_obj_t * cpicker, lv_action_t action)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    ext->value_changed = action;
}

/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" functions come here
 */

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
    case LV_CPICKER_STYLE_IND:
        return ext->ind.style;
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
 * Get the current selected color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return color current selected color
 */
lv_color_t lv_cpicker_get_color(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return lv_color_hsv_to_rgb(ext->hue, ext->saturation, ext->value);
}

/**
 * Get the action callback called on value change event.
 * @param cpicker pointer to colorpicker object
 * @return action callback function
 */
lv_action_t lv_cpicker_get_action(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->value_changed;
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
 * Handle the drawing related tasks of the color_picker
 * @param cpicker pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_cpicker_design(lv_obj_t * cpicker, const lv_area_t * mask, lv_design_mode_t mode)
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

        lv_coord_t r = (LV_MATH_MIN(lv_obj_get_width(cpicker), lv_obj_get_height(cpicker))) / 2;
        lv_coord_t x = cpicker->coords.x1 + lv_obj_get_width(cpicker) / 2;
        lv_coord_t y = cpicker->coords.y1 + lv_obj_get_height(cpicker) / 2;
        lv_opa_t opa_scale = lv_obj_get_opa_scale(cpicker);

        for(uint16_t i = 0; i < 360; i++)
        {
            styleCopy.line.color = lv_color_hsv_to_rgb(i, ext->saturation, ext->value);
            lv_draw_arc(x, y, r, mask, i, i + 1, &styleCopy, opa_scale);
        }

        //draw the center color indicator
        lv_area_t center_area;

        uint16_t radius = r - styleCopy.line.width - style->body.padding.inner;
        center_area.x1 = x - radius;
        center_area.y1 = y - radius;
        center_area.x2 = x + radius;
        center_area.y2 = y + radius;

        styleCopy.body.main_color = lv_color_hsv_to_rgb(ext->hue, ext->saturation, ext->value);
        styleCopy.body.grad_color = styleCopy.body.main_color;
        styleCopy.body.radius = LV_RADIUS_CIRCLE;
        lv_draw_rect(&center_area, mask, &styleCopy, opa_scale);


        //Draw the current hue indicator
        switch(ext->ind.type)
        {
        case LV_CPICKER_IND_LINE:
        {
            lv_point_t start;
            lv_point_t end;

            start.x = x + ((r - style->line.width + ext->ind.style->body.padding.inner + ext->ind.style->line.width/2) * lv_trigo_sin(ext->hue) >> LV_TRIGO_SHIFT);
            start.y = y + ((r - style->line.width + ext->ind.style->body.padding.inner + ext->ind.style->line.width/2) * lv_trigo_sin(ext->hue + 90) >> LV_TRIGO_SHIFT);

            end.x = x + ((r - ext->ind.style->body.padding.inner - ext->ind.style->line.width/2) * lv_trigo_sin(ext->hue) >> LV_TRIGO_SHIFT);
            end.y = y + ((r - ext->ind.style->body.padding.inner - ext->ind.style->line.width/2) * lv_trigo_sin(ext->hue + 90) >> LV_TRIGO_SHIFT);

            lv_draw_line(&start, &end, mask, ext->ind.style, opa_scale);
            if(ext->ind.style->line.rounded)
            {
                lv_area_t circle_area;
                circle_area.x1 = start.x - ((ext->ind.style->line.width - 1) >> 1) - ((ext->ind.style->line.width - 1) & 0x1);
                circle_area.y1 = start.y - ((ext->ind.style->line.width - 1) >> 1) - ((ext->ind.style->line.width - 1) & 0x1);
                circle_area.x2 = start.x + ((ext->ind.style->line.width - 1) >> 1);
                circle_area.y2 = start.y + ((ext->ind.style->line.width - 1) >> 1);
                lv_draw_rect(&circle_area, mask, ext->ind.style, opa_scale);

                circle_area.x1 = end.x - ((ext->ind.style->line.width - 1) >> 1) - ((ext->ind.style->line.width - 1) & 0x1);
                circle_area.y1 = end.y - ((ext->ind.style->line.width - 1) >> 1) - ((ext->ind.style->line.width - 1) & 0x1);
                circle_area.x2 = end.x + ((ext->ind.style->line.width - 1) >> 1);
                circle_area.y2 = end.y + ((ext->ind.style->line.width - 1) >> 1);
                lv_draw_rect(&circle_area, mask, ext->ind.style, opa_scale);
            }
            break;
        }
        case LV_CPICKER_IND_CIRCLE:
        {
            lv_area_t circle_area;
            uint32_t cx, cy;
            cx = x + ((r - style->line.width/2) * lv_trigo_sin(ext->hue) >> LV_TRIGO_SHIFT);
            cy = y + ((r - style->line.width/2) * lv_trigo_sin(ext->hue + 90) >> LV_TRIGO_SHIFT);

            circle_area.x1 = cx - style->line.width/2;
            circle_area.y1 = cy - style->line.width/2;
            circle_area.x2 = cx + style->line.width/2;
            circle_area.y2 = cy + style->line.width/2;


            ext->ind.style->body.radius = LV_RADIUS_CIRCLE;
            lv_draw_rect(&circle_area, mask, ext->ind.style, opa_scale);
        }
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}

/**
 * Signal function of the color_picker
 * @param cpicker pointer to a color_picker object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_cpicker_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(cpicker, sign, param);
    if(res != LV_RES_OK) return res;

    lv_coord_t r = (LV_MATH_MIN(lv_obj_get_width(cpicker), lv_obj_get_height(cpicker))) / 2;
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
        ext->prev_hue = ext->hue;
    }
    else if(sign == LV_SIGNAL_PRESSING)
    {
        lv_indev_t * indev = param;
        lv_coord_t xp = indev->proc.act_point.x - x;
        lv_coord_t yp = indev->proc.act_point.y - y;

        if(fast_sqrt(xp*xp + yp+yp) < r)
        {
            ext->hue = fast_atan2(xp, yp);
            lv_obj_invalidate(cpicker);
        }
    }
    else if(sign == LV_SIGNAL_PRESS_LOST)
    {
        ext->hue = ext->prev_hue;
        lv_obj_invalidate(cpicker);
    }
    else if(sign == LV_SIGNAL_RELEASED)
    {
        lv_indev_t * indev = param;
        lv_coord_t xp = indev->proc.act_point.x - x;
        lv_coord_t yp = indev->proc.act_point.y - y;

        if(fast_sqrt(xp*xp + yp+yp) < r)
        {
            ext->hue = fast_atan2(xp, yp);
            lv_obj_invalidate(cpicker);

            ext->prev_hue = ext->hue;

            if(ext->value_changed != NULL)
                ext->value_changed(cpicker);
        }
    }
    else if(sign == LV_SIGNAL_CONTROLL)
    {
        uint32_t c = *((uint32_t *)param);      /*uint32_t because can be UTF-8*/
        if(c == LV_GROUP_KEY_RIGHT)
        {
            ext->hue++;
            lv_obj_invalidate(cpicker);
            if(ext->value_changed != NULL)
                ext->value_changed(cpicker);
        }
        else if(c == LV_GROUP_KEY_LEFT)
        {
            ext->hue--;
            lv_obj_invalidate(cpicker);
            if(ext->value_changed != NULL)
                ext->value_changed(cpicker);
        }
        else if(c == LV_GROUP_KEY_UP)
        {
            ext->hue++;
            lv_obj_invalidate(cpicker);
            if(ext->value_changed != NULL)
                ext->value_changed(cpicker);
        }
        else if(c == LV_GROUP_KEY_DOWN)
        {
            ext->hue--;
            lv_obj_invalidate(cpicker);
            if(ext->value_changed != NULL)
                ext->value_changed(cpicker);
        }
    }

    return LV_RES_OK;
}

static uint16_t fast_atan2(int x, int y)
{
    // Fast XY vector to integer degree algorithm - Jan 2011 www.RomanBlack.com
    // Converts any XY values including 0 to a degree value that should be
    // within +/- 1 degree of the accurate value without needing
    // large slow trig functions like ArcTan() or ArcCos().
    // NOTE! at least one of the X or Y values must be non-zero!
    // This is the full version, for all 4 quadrants and will generate
    // the angle in integer degrees from 0-360.
    // Any values of X and Y are usable including negative values provided
    // they are between -1456 and 1456 so the 16bit multiply does not overflow.

    unsigned char negflag;
    unsigned char tempdegree;
    unsigned char comp;
    unsigned int degree;     // this will hold the result
    //signed int x;            // these hold the XY vector at the start
    //signed int y;            // (and they will be destroyed)
    unsigned int ux;
    unsigned int uy;

    // Save the sign flags then remove signs and get XY as unsigned ints
    negflag = 0;
    if(x < 0) {
        negflag += 0x01;    // x flag bit
        x = (0 - x);        // is now +
    }
    ux = x;                // copy to unsigned var before multiply
    if(y < 0) {
        negflag += 0x02;    // y flag bit
        y = (0 - y);        // is now +
    }
    uy = y;                // copy to unsigned var before multiply

    // 1. Calc the scaled "degrees"
    if(ux > uy) {
        degree = (uy * 45) / ux;   // degree result will be 0-45 range
        negflag += 0x10;    // octant flag bit
    } else {
        degree = (ux * 45) / uy;   // degree result will be 0-45 range
    }

    // 2. Compensate for the 4 degree error curve
    comp = 0;
    tempdegree = degree;    // use an unsigned char for speed!
    if(tempdegree > 22) {    // if top half of range
        if(tempdegree <= 44) comp++;
        if(tempdegree <= 41) comp++;
        if(tempdegree <= 37) comp++;
        if(tempdegree <= 32) comp++;  // max is 4 degrees compensated
    } else { // else is lower half of range
        if(tempdegree >= 2) comp++;
        if(tempdegree >= 6) comp++;
        if(tempdegree >= 10) comp++;
        if(tempdegree >= 15) comp++;  // max is 4 degrees compensated
    }
    degree += comp;   // degree is now accurate to +/- 1 degree!

    // Invert degree if it was X>Y octant, makes 0-45 into 90-45
    if(negflag & 0x10) degree = (90 - degree);

    // 3. Degree is now 0-90 range for this quadrant,
    // need to invert it for whichever quadrant it was in
    if(negflag & 0x02) { // if -Y
        if(negflag & 0x01)   // if -Y -X
            degree = (180 + degree);
        else        // else is -Y +X
            degree = (180 - degree);
    } else { // else is +Y
        if(negflag & 0x01)   // if +Y -X
            degree = (360 - degree);
    }
    return degree;
}

/*IAR Application Note AN-G002*/
static uint16_t fast_sqrt(unsigned int x)
{
    uint16_t a,b;
    b = x;
    a = x = 0x3f;
    x = b/x;
    a = x = (x+a)>>1;
    x = b/x;
    a = x = (x+a)>>1;
    x = b/x;
    x = (x+a)>>1;
    return(x);
}

#endif
