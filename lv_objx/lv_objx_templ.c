/**
 * @file lv_templ.c
 * 
 */

/*Search an replace: templ -> object short name with lower case(e.g. btn, label etc)
 *                   TEMPL -> object short name with upper case (e.g. BTN, LABEL etc.)
 *                   template -> object normal name with lower case (e.g. button, label etc.)
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
static bool lv_templ_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_templs_t lv_templs_def =
{ /*Create a default style*/ };

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
 * @param par_dp pointer to an object, it will be the parent of the new template
 * @param copy_dp pointer to a template object, if not NULL then the new object will be copied from it
 * @return pointer to the created template
 */
lv_obj_t* lv_templ_create(lv_obj_t* par_dp, , lv_obj_t * copy_dp);
{
    /*Create the ancestor basic object*/
    lv_obj_t* new_obj_dp = lv_obj_create(par_dp);
    dm_assert(new_obj_dp);
    
    /*Init the new template object*/
    
    return new_obj_dp;
}

/**
 * Signal function of the template
 * @param obj_dp pointer to a template object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_templ_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    /* TODO update it to the ancient signal function*/
    valid = lv_obj_signal(obj_dp, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    		default:
    			break;
    	}
    }
    
    return valid;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_templs_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_templs_t style
 */
lv_templs_t * lv_templs_get(lv_templs_builtin_t style, lv_templs_t * copy_p)
{
	lv_templs_t  *style_p;

	switch(style) {
		case LV_TEMPLS_DEF:
			style_p = &lv_templs_def;
			break;
		default:
			style_p = &lv_templs_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_templs_t));
		else memcpy(copy_p, &lv_templs_def, sizeof(lv_templs_t));
	}

	return style_p;
}
/*=====================
 * Setter functions 
 *====================*/


/*=====================
 * Getter functions 
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Handle the drawing related tasks of the templates
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static bool lv_templ_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return false;
    }


    /*Draw the object*/

    return true;
}


#endif
