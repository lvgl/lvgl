/**
 * @file lv_sw.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_SW != 0

#include "lv_sw.h"

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

/*----------------- 
 * Create function
 *-----------------*/

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
    dm_assert(new_sw);

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_sw);
    
    /*Allocate the switch type specific extended data*/
    lv_sw_ext_t * ext = lv_obj_allocate_ext_attr(new_sw, sizeof(lv_sw_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->changed = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_sw, lv_sw_signal);

    /*Init the new switch switch*/
    if(copy == NULL) {
        lv_slider_set_range(new_sw, 0, 1);
        lv_obj_set_size(new_sw, 2 * LV_DPI / 3, LV_DPI / 3);
        lv_slider_set_knob_in(new_sw, true);
    }
    /*Copy an existing switch*/
    else {
        lv_sw_ext_t *copy_ext = lv_obj_get_ext_attr(copy);
        ext->knob_off_style = copy_ext->knob_off_style;
        ext->knob_on_style = copy_ext->knob_on_style;

        if(lv_sw_get_state(new_sw)) lv_slider_set_style(new_sw, NULL, NULL, ext->knob_on_style);
        else lv_slider_set_style(new_sw, NULL, NULL, ext->knob_off_style);
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
void lv_sw_set_on(lv_obj_t *sw)
{
    lv_sw_ext_t *ext = lv_obj_get_ext_attr(sw);
    lv_slider_set_value(sw, 1);
    lv_slider_set_style(sw, NULL, NULL,ext->knob_on_style);
}

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 */
void lv_sw_set_off(lv_obj_t *sw)
{
    lv_sw_ext_t *ext = lv_obj_get_ext_attr(sw);
    lv_slider_set_value(sw, 0);
    lv_slider_set_style(sw, NULL, NULL,ext->knob_off_style);
}

/**
 * Set the styles of a switch
 * @param sw pointer to a switch object
 * @param bg pointer to the background's style
 * @param indic pointer to the indicator's style
 * @param knob_off pointer to the knob's style when the switch is OFF
 * @param knob_on pointer to the knob's style when the switch is ON
 */
void lv_sw_set_style(lv_obj_t * sw, lv_style_t *bg, lv_style_t *indic, lv_style_t *knob_off, lv_style_t *knob_on)
{
    lv_sw_ext_t *ext = lv_obj_get_ext_attr(sw);

    ext->knob_on_style = knob_on;
    ext->knob_off_style = knob_off;

    if(lv_sw_get_state(sw)) lv_slider_set_style(sw, bg, indic, knob_on);
    else lv_slider_set_style(sw, bg, indic, knob_off);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the style of the switch's knob when the switch is OFF
 * @param sw pointer to a switch object
 * @return pointer to the switch's knob OFF style
 */
lv_style_t * lv_sw_get_style_knob_off(lv_obj_t *sw)
{
    lv_sw_ext_t *ext = lv_obj_get_ext_attr(sw);
    return ext->knob_off_style;
}


/**
 * Get the style of the switch's knob when the switch is ON
 * @param sw pointer to a switch object
 * @return pointer to the switch's knob ON style
 */
lv_style_t * lv_sw_get_style_knob_on(lv_obj_t *sw)
{
    lv_sw_ext_t *ext = lv_obj_get_ext_attr(sw);
    return ext->knob_on_style;
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
    lv_action_t slider_cb = ext->slider.action;
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
        if(lv_sw_get_state(sw)) lv_slider_set_style(sw, NULL, NULL, ext->knob_on_style);
        else lv_slider_set_style(sw, NULL, NULL, ext->knob_off_style);
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        if(ext->changed == 0) {
            int16_t v = lv_slider_get_value(sw);
            if(v == 0) lv_slider_set_value(sw, 1);
            else lv_slider_set_value(sw, 0);
        }

        if(lv_sw_get_state(sw)) lv_slider_set_style(sw, NULL, NULL, ext->knob_on_style);
        else lv_slider_set_style(sw, NULL, NULL, ext->knob_off_style);

        if(slider_cb != NULL) slider_cb(sw);

        ext->changed = 0;
    }

    /*Restore the callback*/
    ext->slider.action = slider_cb;

    return res;
}

#endif
