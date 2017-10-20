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
#if 0 /*Slider design is used*/
static bool lv_sw_design(lv_obj_t * sw, const area_t * mask, lv_design_mode_t mode);
#endif
/**********************
 *  STATIC VARIABLES
 **********************/

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
    
    /*Allocate the switch type specific extended data*/
    lv_sw_ext_t * ext = lv_obj_allocate_ext_attr(new_sw, sizeof(lv_sw_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->changed = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_sw, lv_sw_signal);

    /*Init the new switch switch*/
    if(copy == NULL) {
        lv_bar_set_range(new_sw, 0, 1);
        lv_obj_set_size(new_sw, LV_DPI, LV_DPI / 2);
        lv_slider_set_knob_in(new_sw, true);
    }
    /*Copy an existing switch*/
    else {
        /*Nothing to copy*/

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_sw);
    }
    
    return new_sw;
}

/**
 * Signal function of the switch
 * @param sw pointer to a switch object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_sw_signal(lv_obj_t * sw, lv_signal_t sign, void * param)
{
    bool valid;

    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);
    int16_t old_val = lv_bar_get_value(sw);

    lv_action_t slider_cb = ext->slider.cb;
    ext->slider.cb = NULL;  /*Do not let the slider to call the callback. The Switch will do it*/

    /* Include the ancient signal function */
    valid = lv_slider_signal(sw, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	if(sign == LV_SIGNAL_CLEANUP) {
            /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    	}
        else if(sign == LV_SIGNAL_PRESSING) {
            int16_t act_val = lv_bar_get_value(sw);
            if(act_val != old_val) ext->changed = 1;
        }
        else if(sign == LV_SIGNAL_PRESS_LOST) {
            ext->changed = 0;
        }
    	else if(sign == LV_SIGNAL_RELEASED) {
    	    if(ext->changed == 0) {
                int16_t v = lv_bar_get_value(sw);
                if(v == 0) lv_bar_set_value(sw, 1);
                else lv_bar_set_value(sw, 0);
    	    }
    	    if(slider_cb != NULL) slider_cb(sw);

            ext->changed = 0;
    	}
    }
    
    /*Restore the callback*/
    ext->slider.cb = slider_cb;

    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/*
 * New object specific "set" function comes here
 */


/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" function comes here
 */


/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0 /*Slider design is used*/
/**
 * Handle the drawing related tasks of the switchs
 * @param sw pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_sw_design(lv_obj_t * sw, const area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}
#endif

#endif
