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

#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_sw_signal(lv_obj_t * sw, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;

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
    lv_obj_t * new_sw = lv_slider_create(par, copy);
    lv_mem_assert(new_sw);
    if(new_sw == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_sw);

    /*Allocate the switch type specific extended data*/
    lv_sw_ext_t * ext = lv_obj_allocate_ext_attr(new_sw, sizeof(lv_sw_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */
    ext->changed = 0;
#if LV_USE_ANIMATION
    ext->anim_time = 0;
#endif
    ext->style_knob_off = ext->slider.style_knob;
    ext->style_knob_on  = ext->slider.style_knob;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_sw, lv_sw_signal);

    /*Init the new switch switch*/
    if(copy == NULL) {
        lv_obj_set_size(new_sw, 2 * LV_DPI / 3, LV_DPI / 3);
        lv_slider_set_knob_in(new_sw, true);
        lv_slider_set_range(new_sw, 0, LV_SW_MAX_VALUE);

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

        if(lv_sw_get_state(new_sw))
            lv_slider_set_style(new_sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_on);
        else
            lv_slider_set_style(new_sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_off);

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
#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);
    lv_slider_set_value(sw, LV_SW_MAX_VALUE, anim);
    lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_on);
}

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_sw_off(lv_obj_t * sw, lv_anim_enable_t anim)
{
#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);
    lv_slider_set_value(sw, 0, anim);
    lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_off);
}

/**
 * Toggle the position of the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 * @return resulting state of the switch.
 */
bool lv_sw_toggle(lv_obj_t * sw, lv_anim_enable_t anim)
{
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
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);

    switch(type) {
        case LV_SLIDER_STYLE_BG: lv_slider_set_style(sw, LV_SLIDER_STYLE_BG, style); break;
        case LV_SLIDER_STYLE_INDIC: lv_bar_set_style(sw, LV_SLIDER_STYLE_INDIC, style); break;
        case LV_SW_STYLE_KNOB_OFF:
            ext->style_knob_off = style;
            if(lv_sw_get_state(sw) == 0) lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB, style);
            break;
        case LV_SW_STYLE_KNOB_ON:
            ext->style_knob_on = style;
            if(lv_sw_get_state(sw) != 0) lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB, style);
            break;
    }
}

void lv_sw_set_anim_time(lv_obj_t * sw, uint16_t anim_time)
{
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
    const lv_style_t * style = NULL;
    lv_sw_ext_t * ext        = lv_obj_get_ext_attr(sw);

    switch(type) {
        case LV_SW_STYLE_BG: style = lv_slider_get_style(sw, LV_SLIDER_STYLE_BG); break;
        case LV_SW_STYLE_INDIC: style = lv_slider_get_style(sw, LV_SLIDER_STYLE_INDIC); break;
        case LV_SW_STYLE_KNOB_OFF: style = ext->style_knob_off; break;
        case LV_SW_STYLE_KNOB_ON: style = ext->style_knob_on; break;
        default: style = NULL; break;
    }

    return style;
}

uint16_t lv_sw_get_anim_time(const lv_obj_t * sw)
{

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
 * Signal function of the switch
 * @param sw pointer to a switch object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_sw_signal(lv_obj_t * sw, lv_signal_t sign, void * param)
{
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);

    /*Save the current (old) value before slider signal modifies it. It will be required in the
     * later calculations*/
    int16_t old_val;
    if(sign == LV_SIGNAL_PRESSING)
        old_val = ext->slider.drag_value;
    else
        old_val = lv_slider_get_value(sw);

    /*Don't let the slider to call the action. Switch handles it differently*/
    lv_event_cb_t event_cb = sw->event_cb;
    sw->event_cb           = NULL;

    lv_res_t res;
    /* Include the ancient signal function */

    res = ancestor_signal(sw, sign, param);
    if(res != LV_RES_OK) return res;

    sw->event_cb = event_cb;

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
        if(indev) {
            lv_point_t p = {0, 0};
            lv_indev_get_point(indev, &p);
            if(LV_MATH_ABS(p.x - ext->start_x) > LV_INDEV_DEF_DRAG_LIMIT) ext->slided = 1;
        }

        /*If didn't slide then revert the min/max value. So click without slide won't move the
         * switch as a slider*/
        if(ext->slided == 0) {
            if(lv_sw_get_state(sw))
                ext->slider.drag_value = LV_SW_MAX_VALUE;
            else
                ext->slider.drag_value = 0;
        }

        /*If explicitly changed (by slide) don't need to be toggled on release*/
        int16_t threshold = LV_SW_MAX_VALUE / 2;
        if((old_val < threshold && ext->slider.drag_value > threshold) ||
           (old_val > threshold && ext->slider.drag_value < threshold)) {
            ext->changed = 1;
        }
    } else if(sign == LV_SIGNAL_PRESS_LOST) {
        if(lv_sw_get_state(sw)) {
            lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_on);
            lv_slider_set_value(sw, LV_SW_MAX_VALUE, LV_ANIM_ON);
            if(res != LV_RES_OK) return res;
        } else {
            lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_off);
            lv_slider_set_value(sw, 0, LV_ANIM_ON);
            if(res != LV_RES_OK) return res;
        }
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
            int16_t v = lv_slider_get_value(sw);
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
            lv_slider_set_value(sw, LV_SW_MAX_VALUE, true);
            state = 1;
            res   = lv_event_send(sw, LV_EVENT_VALUE_CHANGED, &state);
            if(res != LV_RES_OK) return res;
        } else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            lv_slider_set_value(sw, 0, true);
            state = 0;
            res   = lv_event_send(sw, LV_EVENT_VALUE_CHANGED, &state);
            if(res != LV_RES_OK) return res;
        }
    } else if(sign == LV_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = false; /*The ancestor slider is editable the switch is not*/
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_sw";
    }

    return res;
}

#endif
