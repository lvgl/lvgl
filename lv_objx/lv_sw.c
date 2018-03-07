/**
 * @file lv_sw.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_SW != 0

/*Testing of dependencies*/
#if USE_LV_SLIDER == 0
#error "lv_sw: lv_slider is required. Enable it in lv_conf.h (USE_LV_SLIDER  1) "
#endif

#include "lv_sw.h"
#include "../lv_themes/lv_theme.h"

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
static lv_signal_func_t ancestor_signal;

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
lv_obj_t * lv_sw_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of switch*/
    lv_obj_t * new_sw = lv_slider_create(par, copy);
    lv_mem_assert(new_sw);

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_sw);
    
    /*Allocate the switch type specific extended data*/
    lv_sw_ext_t * ext = lv_obj_allocate_ext_attr(new_sw, sizeof(lv_sw_ext_t));
    lv_mem_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->changed = 0;
    ext->style_knob_off = ext->slider.style_knob;
    ext->style_knob_on = ext->slider.style_knob;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_sw, lv_sw_signal);

    /*Init the new switch switch*/
    if(copy == NULL) {
        lv_slider_set_range(new_sw, 0, 1);
        lv_obj_set_size(new_sw, 2 * LV_DPI / 3, LV_DPI / 3);
        lv_slider_set_knob_in(new_sw, true);

        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            lv_sw_set_style(new_sw, LV_SW_STYLE_BG, th->sw.bg);
            lv_sw_set_style(new_sw, LV_SW_STYLE_INDIC, th->sw.indic);
            lv_sw_set_style(new_sw, LV_SW_STYLE_KNOB_OFF, th->sw.knob_off);
            lv_sw_set_style(new_sw, LV_SW_STYLE_KNOB_ON, th->sw.knob_on);
        } else {
            /*Let the slider' style*/
        }

    }
    /*Copy an existing switch*/
    else {
        lv_sw_ext_t *copy_ext = lv_obj_get_ext_attr(copy);
        ext->style_knob_off = copy_ext->style_knob_off;
        ext->style_knob_on = copy_ext->style_knob_on;

        if(lv_sw_get_state(new_sw)) lv_slider_set_style(new_sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_on);
        else lv_slider_set_style(new_sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_off);
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_sw);
    }
    
    return new_sw;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Turn ON the switch
 * @param sw pointer to a switch object
 */
void lv_sw_on(lv_obj_t *sw)
{
    lv_sw_ext_t *ext = lv_obj_get_ext_attr(sw);
    lv_slider_set_value(sw, 1);
    lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB,ext->style_knob_on);
}

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 */
void lv_sw_off(lv_obj_t *sw)
{
    lv_sw_ext_t *ext = lv_obj_get_ext_attr(sw);
    lv_slider_set_value(sw, 0);
    lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB,ext->style_knob_off);
}

/**
 * Set a style of a switch
 * @param sw pointer to a switch object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_sw_set_style(lv_obj_t *sw, lv_sw_style_t type, lv_style_t *style)
{
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);

    switch (type) {
        case LV_SLIDER_STYLE_BG:
            lv_slider_set_style(sw, LV_SLIDER_STYLE_BG, style);
            break;
        case LV_SLIDER_STYLE_INDIC:
            lv_bar_set_style(sw, LV_SLIDER_STYLE_INDIC, style);
            break;
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

/*=====================
 * Getter functions
 *====================*/

/**
 * Get a style of a switch
 * @param sw pointer to a  switch object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_sw_get_style(lv_obj_t *sw, lv_sw_style_t type)
{
    lv_sw_ext_t *ext = lv_obj_get_ext_attr(sw);

    switch (type) {
        case LV_SW_STYLE_BG:    return lv_slider_get_style(sw, LV_SLIDER_STYLE_BG);
        case LV_SW_STYLE_INDIC: return lv_slider_get_style(sw, LV_SLIDER_STYLE_INDIC);
        case LV_SW_STYLE_KNOB_OFF:  return ext->style_knob_off;
        case LV_SW_STYLE_KNOB_ON:  return ext->style_knob_on;
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
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

    /*Save the current (old) value before slider signal modifies it*/
    int16_t old_val;
    if(sign == LV_SIGNAL_PRESSING) old_val = ext->slider.drag_value;
    else old_val = lv_slider_get_value(sw);

    /*Do not let the slider to call the callback. The Switch will do it if required*/
    lv_action_t slider_action = ext->slider.action;
    ext->slider.action = NULL;

    lv_res_t res;
    /* Include the ancient signal function */
    res = ancestor_signal(sw, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }
    else if(sign == LV_SIGNAL_PRESSING) {
        int16_t act_val = ext->slider.drag_value;
        if(act_val != old_val) ext->changed = 1;
    }
    else if(sign == LV_SIGNAL_PRESS_LOST) {
        ext->changed = 0;
        if(lv_sw_get_state(sw)) lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_on);
        else lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_off);
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        if(ext->changed == 0) {
            int16_t v = lv_slider_get_value(sw);
            if(v == 0) lv_slider_set_value(sw, 1);
            else lv_slider_set_value(sw, 0);
        }

        if(lv_sw_get_state(sw)) lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_on);
        else lv_slider_set_style(sw, LV_SLIDER_STYLE_KNOB, ext->style_knob_off);

        if(slider_action != NULL) slider_action(sw);

        ext->changed = 0;
    }
    else if(sign == LV_SIGNAL_CONTROLL) {

        char c = *((char*)param);
        if(c == LV_GROUP_KEY_ENTER || c == LV_GROUP_KEY_ENTER_LONG) {
            if(lv_sw_get_state(sw)) lv_sw_off(sw);
            else lv_sw_on(sw);

            if(slider_action) slider_action(sw);
        }
        else if(c == LV_GROUP_KEY_UP || c== LV_GROUP_KEY_RIGHT) {
            lv_sw_on(sw);
            if(slider_action) slider_action(sw);
        }
        else if(c == LV_GROUP_KEY_DOWN || c== LV_GROUP_KEY_LEFT) {
            lv_sw_off(sw);
            if(slider_action) slider_action(sw);
        }
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_sw";
    }

    /*Restore the callback*/
    ext->slider.action = slider_action;

    return res;
}

#endif
