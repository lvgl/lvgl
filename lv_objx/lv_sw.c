/**
 * @file lv_templ.c
 * 
 */

/*Search an replace: template -> object normal name with lower case (e.g. button, label etc.)
 * 					 templ -> object short name with lower case(e.g. btn, label etc)
 *                   TEMPL -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_TEMPL != 0

#include "lv_templ.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_templ_design(lv_obj_t * templ, const area_t * mask, lv_design_mode_t mode);

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
 * Create a template objects
 * @param par pointer to an object, it will be the parent of the new template
 * @param copy pointer to a template object, if not NULL then the new object will be copied from it
 * @return pointer to the created template
 */
lv_obj_t * lv_templ_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of template*/
	/*TODO modify it to the ancestor create function */
    lv_obj_t * new_templ = lv_ANCESTOR_create(par, copy);
    dm_assert(new_templ);
    
    /*Allocate the template type specific extended data*/
    lv_templ_ext_t * ext = lv_obj_alloc_ext(new_templ, sizeof(lv_templ_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->xyz = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_templ, lv_templ_signal);
    lv_obj_set_design_f(new_templ, lv_templ_design);

    /*Init the new template template*/
    if(copy == NULL) {
        lv_obj_set_style(new_templ, lv_style_get(LV_STYLE_PRETTY, NULL));
    }
    /*Copy an existing template*/
    else {
    	lv_templ_ext_t * copy_ext = lv_obj_get_ext(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_templ);
    }
    
    return new_templ;
}

/**
 * Signal function of the template
 * @param templ pointer to a template object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_templ_signal(lv_obj_t * templ, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    /* TODO update it to the ancestor's signal function*/
    valid = lv_ANCESTOR_signal(templ, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	if(sign == LV_SIGNAL_CLEANUP) {
            /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    	}
    }
    
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


/**
 * Handle the drawing related tasks of the templates
 * @param templ pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_templ_design(lv_obj_t * templ, const area_t * mask, lv_design_mode_t mode)
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
