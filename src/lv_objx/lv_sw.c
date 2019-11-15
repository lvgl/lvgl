/**
 * @file lv_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_sw.h"

#if LV_USE_SW != 0

/*Testing of dependencies*/
#if LV_USE_SLIDER == 0
#error "lv_sw: lv_slider is required. Enable it in lv_conf.h (LV_USE_SLIDER  1) "
#endif

#include "../lv_core/lv_debug.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_math.h"
#include "../lv_core/lv_indev.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_sw"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_sw_design(lv_obj_t * slider, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_sw_signal(lv_obj_t * sw, lv_signal_t sign, void * param);

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
 * Create a switch objects
 * @param par pointer to an object, it will be the parent of the new switch
 * @param copy pointer to a switch object, if not NULL then the new object will be copied from it
 * @return pointer to the created switch
 */
lv_obj_t * lv_sw_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("switch create started");

    /*Create the ancestor of switch*/
    lv_obj_t * new_sw = lv_bar_create(par, copy);
    LV_ASSERT_MEM(new_sw);

    if(new_sw == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_sw);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(new_sw);

    /*Allocate the switch type specific extended data*/
    lv_sw_ext_t * ext = lv_obj_allocate_ext_attr(new_sw, sizeof(lv_sw_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */
    ext->changed = 0;
#if LV_USE_ANIMATION
    ext->anim_time = 0;
#endif
    ext->style_knob_off = &lv_style_pretty;
    ext->style_knob_on  = &lv_style_pretty;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_sw, lv_sw_signal);
    lv_obj_set_design_cb(new_sw, lv_sw_design);

    /*Init the new switch switch*/
    if(copy == NULL) {
        lv_obj_set_click(new_sw, true);
        lv_obj_set_protect(new_sw, LV_PROTECT_PRESS_LOST);
        lv_obj_set_size(new_sw, 2 * LV_DPI / 3, LV_DPI / 3);
        lv_bar_set_range(new_sw, 0, LV_SW_MAX_VALUE);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_sw_set_style(new_sw, LV_SW_STYLE_BG, th->style.sw.bg);
            lv_sw_set_style(new_sw, LV_SW_STYLE_INDIC, th->style.sw.indic);
            lv_sw_set_style(new_sw, LV_SW_STYLE_KNOB_OFF, th->style.sw.knob_off);
            lv_sw_set_style(new_sw, LV_SW_STYLE_KNOB_ON, th->style.sw.knob_on);
        } else {
            /*Let the slider' style*/
        }
    }
    /*Copy an existing switch*/
    else {
        lv_sw_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->style_knob_off    = copy_ext->style_knob_off;
        ext->style_knob_on     = copy_ext->style_knob_on;
#if LV_USE_ANIMATION
        ext->anim_time = copy_ext->anim_time;
#endif

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_sw);
    }

    LV_LOG_INFO("switch created");

    return new_sw;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Turn ON the switch
 * @param sw pointer to a switch objec
 * @param anim LV_ANOM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_sw_on(lv_obj_t * sw, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);
    ext->state = 1;
    lv_bar_set_value(sw, LV_SW_MAX_VALUE, anim);
    lv_obj_invalidate(sw);
}

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_sw_off(lv_obj_t * sw, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);
    ext->state = 0;
    lv_bar_set_value(sw, 0, anim);
    lv_obj_invalidate(sw);
}

/**
 * Toggle the position of the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 * @return resulting state of the switch.
 */
bool lv_sw_toggle(lv_obj_t * sw, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif

    bool state = lv_sw_get_state(sw);
    if(state)
        lv_sw_off(sw, anim);
    else
        lv_sw_on(sw, anim);

    return !state;
}

/**
 * Set a style of a switch
 * @param sw pointer to a switch object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_sw_set_style(lv_obj_t * sw, lv_sw_style_t type, const lv_style_t * style)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);

    switch(type) {
        case LV_SW_STYLE_BG: lv_bar_set_style(sw, LV_BAR_STYLE_BG, style); break;
        case LV_SW_STYLE_INDIC: lv_bar_set_style(sw, LV_BAR_STYLE_INDIC, style); break;
        case LV_SW_STYLE_KNOB_OFF:
            ext->style_knob_off = style;
            lv_obj_invalidate(sw);
            break;
        case LV_SW_STYLE_KNOB_ON:
            ext->style_knob_on = style;
            lv_obj_invalidate(sw);
            break;
    }
}

void lv_sw_set_anim_time(lv_obj_t * sw, uint16_t anim_time)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);
    ext->anim_time    = anim_time;
#else
    (void)sw;
    (void)anim_time;
#endif
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get a style of a switch
 * @param sw pointer to a  switch object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_sw_get_style(const lv_obj_t * sw, lv_sw_style_t type)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

    const lv_style_t * style = NULL;
    lv_sw_ext_t * ext        = lv_obj_get_ext_attr(sw);

    switch(type) {
        case LV_SW_STYLE_BG: style = lv_bar_get_style(sw, LV_BAR_STYLE_BG); break;
        case LV_SW_STYLE_INDIC: style = lv_bar_get_style(sw, LV_BAR_STYLE_INDIC); break;
        case LV_SW_STYLE_KNOB_OFF: style = ext->style_knob_off; break;
        case LV_SW_STYLE_KNOB_ON: style = ext->style_knob_on; break;
        default: style = NULL; break;
    }

    return style;
}

uint16_t lv_sw_get_anim_time(const lv_obj_t * sw)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);
    return ext->anim_time;
#else
    (void)sw; /*Unused*/
    return 0;
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the sliders
 * @param sw pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_sw_design(lv_obj_t * sw, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return LV_DESIGN_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        /*The ancestor design function will draw the background and the indicator.
         * It also sets ext->bar.indic_area*/
        ancestor_design(sw, clip_area, mode);

        lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);
        lv_opa_t opa_scale = lv_obj_get_opa_scale(sw);
        const lv_style_t * style_knob  = lv_sw_get_style(sw, ext->state ?
                                            LV_SW_STYLE_KNOB_ON : LV_SW_STYLE_KNOB_OFF);

        const lv_style_t * style_indic = lv_sw_get_style(sw, LV_SW_STYLE_INDIC);

        lv_coord_t objw = lv_obj_get_width(sw);
        lv_coord_t objh = lv_obj_get_height(sw);
        lv_coord_t indic_maxw  = objw - style_indic->body.padding.left - style_indic->body.padding.right;
        lv_coord_t knob_size = objh;

        lv_coord_t indic_p = (lv_area_get_width(&ext->bar.indic_area) * 256) / (indic_maxw);
        lv_area_t knob_area;
        knob_area.x2 = ext->bar.indic_area.x2;
        knob_area.x2 += (knob_size * (256 - indic_p)) >> 8;
        if(knob_area.x2 < sw->coords.x1 + knob_size)  knob_area.x2 = sw->coords.x1 + knob_size;
        knob_area.x1 = knob_area.x2 - knob_size;
        knob_area.y1 = sw->coords.y1;
        knob_area.y2 = sw->coords.y2;

        lv_draw_rect(&knob_area, clip_area, style_knob, opa_scale);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
    }

    return LV_DESIGN_RES_OK;
}


/**
 * Signal function of the switch
 * @param sw pointer to a switch object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_sw_signal(lv_obj_t * sw, lv_signal_t sign, void * param)
{
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);

    lv_res_t res;
    /* Include the ancient signal function */
    res = ancestor_signal(sw, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_PRESSED) {

        /*Save the x coordinate of the pressed point to see if the switch was slid*/
        lv_indev_t * indev = lv_indev_get_act();
        if(indev) {
            lv_point_t p;
            lv_indev_get_point(indev, &p);
            ext->start_x = p.x;
        }
        ext->slided  = 0;
        ext->changed = 0;
    } else if(sign == LV_SIGNAL_PRESSING) {
        /*See if the switch was slid (moved at least a little)*/
        lv_indev_t * indev = lv_indev_get_act();
        lv_point_t p;
        if(indev) {
            lv_indev_get_point(indev, &p);
            if(LV_MATH_ABS(p.x - ext->start_x) > indev->driver.drag_limit) ext->slided = 1;

            /*If slid set the value accordingly*/
            if(ext->slided) {
                lv_coord_t w = lv_obj_get_width(sw);
                const lv_style_t * indic_style = lv_sw_get_style(sw, LV_SW_STYLE_INDIC);
                int16_t new_val = 0;

                lv_coord_t indic_w = w - indic_style->body.padding.left - indic_style->body.padding.right;
                int32_t range = ext->bar.max_value - ext->bar.min_value;
                new_val = p.x - (sw->coords.x1 + indic_style->body.padding.left); /*Make the point relative to the indicator*/
                new_val = (new_val * range) / indic_w;
                new_val += ext->bar.min_value;

                if(new_val < ext->bar.min_value) new_val = ext->bar.min_value;
                else if(new_val > ext->bar.max_value) new_val = ext->bar.max_value;

                /*If explicitly changed (by slide) don't need to be toggled on release*/
                int16_t threshold = LV_SW_MAX_VALUE / 2;
                if((new_val < threshold && ext->bar.cur_value > threshold) ||
                   (new_val > threshold && ext->bar.cur_value < threshold)) {
                    ext->changed = 1;
                }

                if(new_val != ext->bar.cur_value) {
                    ext->bar.cur_value = new_val;
                    lv_obj_invalidate(sw);
                }
            }
        }
    } else if(sign == LV_SIGNAL_PRESS_LOST) {
        if(lv_sw_get_state(sw)) lv_sw_on(sw, LV_ANIM_ON);
        else lv_sw_off(sw, LV_ANIM_ON);
    } else if(sign == LV_SIGNAL_RELEASED) {
        /*If not dragged then toggle the switch*/
        if(ext->changed == 0) {
            int32_t state;
            if(lv_sw_get_state(sw)) {
                lv_sw_off(sw, LV_ANIM_ON);
                state = 0;
            } else {
                lv_sw_on(sw, LV_ANIM_ON);
                state = 1;
            }

            res = lv_event_send(sw, LV_EVENT_VALUE_CHANGED, &state);
            if(res != LV_RES_OK) return res;
        }
        /*If the switch was dragged then calculate the new state based on the current position*/
        else {
            int16_t v = lv_bar_get_value(sw);
            int32_t state;
            if(v > LV_SW_MAX_VALUE / 2) {
                lv_sw_on(sw, LV_ANIM_ON);
                state = 1;
            } else {
                lv_sw_off(sw, LV_ANIM_ON);
                state = 0;
            }
            res = lv_event_send(sw, LV_EVENT_VALUE_CHANGED, &state);
            if(res != LV_RES_OK) return res;
        }
    } else if(sign == LV_SIGNAL_CONTROL) {
        char c = *((char *)param);
        uint32_t state;
        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            lv_bar_set_value(sw, LV_SW_MAX_VALUE, LV_ANIM_ON);
            state = 1;
            res   = lv_event_send(sw, LV_EVENT_VALUE_CHANGED, &state);
            if(res != LV_RES_OK) return res;
        } else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            lv_bar_set_value(sw, 0, LV_ANIM_ON);
            state = 0;
            res   = lv_event_send(sw, LV_EVENT_VALUE_CHANGED, &state);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        const lv_style_t * bg_style      = lv_sw_get_style(sw, LV_SW_STYLE_BG);
        const lv_style_t * indic_style      = lv_sw_get_style(sw, LV_SW_STYLE_INDIC);
        const lv_style_t * knob_on_style = lv_sw_get_style(sw, LV_SW_STYLE_KNOB_OFF);
        const lv_style_t * knob_off_style = lv_sw_get_style(sw, LV_SW_STYLE_KNOB_ON);
        /* The smaller size is the knob diameter*/
        lv_coord_t knob_on_size = LV_MATH_MIN(lv_obj_get_width(sw), lv_obj_get_height(sw)) >> 1;
        knob_on_size += LV_MATH_MAX(
                        LV_MATH_MAX(knob_on_style->body.padding.left, knob_on_style->body.padding.right),
                        LV_MATH_MAX(knob_on_style->body.padding.bottom, knob_on_style->body.padding.top));

        knob_on_size += knob_on_style->body.shadow.width + knob_on_style->body.shadow.spread;
        knob_on_size += LV_MATH_MAX(LV_MATH_ABS(knob_on_style->body.shadow.offset.x), LV_MATH_ABS(knob_on_style->body.shadow.offset.y));

        lv_coord_t knob_off_size = LV_MATH_MIN(lv_obj_get_width(sw), lv_obj_get_height(sw)) >> 1;
        knob_off_size += LV_MATH_MAX(
                       LV_MATH_MAX(knob_off_style->body.padding.left, knob_off_style->body.padding.right),
                       LV_MATH_MAX(knob_off_style->body.padding.bottom, knob_off_style->body.padding.top));

        knob_off_size += knob_off_style->body.shadow.width + knob_off_style->body.shadow.spread;
        knob_off_size += LV_MATH_MAX(LV_MATH_ABS(knob_off_style->body.shadow.offset.x), LV_MATH_ABS(knob_off_style->body.shadow.offset.y));

        lv_coord_t bg_size = bg_style->body.shadow.width + bg_style->body.shadow.spread;
        bg_size += LV_MATH_MAX(LV_MATH_ABS(bg_style->body.shadow.offset.x), LV_MATH_ABS(bg_style->body.shadow.offset.y));

        lv_coord_t indic_size = indic_style->body.shadow.width + indic_style->body.shadow.spread;
        indic_size += LV_MATH_MAX(LV_MATH_ABS(indic_style->body.shadow.offset.x), LV_MATH_ABS(indic_style->body.shadow.offset.y));

        sw->ext_draw_pad = LV_MATH_MAX(sw->ext_draw_pad, knob_on_size);
        sw->ext_draw_pad = LV_MATH_MAX(sw->ext_draw_pad, knob_off_size);
        sw->ext_draw_pad = LV_MATH_MAX(sw->ext_draw_pad, bg_size);
        sw->ext_draw_pad = LV_MATH_MAX(sw->ext_draw_pad, indic_size);

    }
    else if(sign == LV_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = false; /*The ancestor slider is editable the switch is not*/
    }

    return res;
}

#endif
