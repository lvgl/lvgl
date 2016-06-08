/**
 * @file lv_temp.c
 * 
 */

/*Search an replace: templ -> object short name (e.g. btn, label etc)
 *                   TEMPLATE -> object normal name (e.g. button, label etc.)
 *Modify USE_LV_TEMPL by hand */

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
 * Create a TEMPLATE objects
 * @param par_dp pointer to an object, it will be the parent of the new label
 * @return pointer to the created label
 */
lv_obj_t* lv_templ_create(lv_obj_t* par_dp)
{
    /*Create a basic object*/
    lv_obj_t* new_obj = lv_obj_create(par_dp);
    dm_assert(new_obj);
    
    /*Init the new TEMPLATE object*/
    
    return new_obj;
}

/**
 * Signal function of the TEMPLATE
 * @param obj_dp pointer to a TEMPLATE object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_rect_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
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
 * @param style a style name from lv_rects_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_templs_t style
 */
lv_templs_t * lv_rects_get(lv_templs_builtin_t style, lv_templs_t * copy_p)
{
	lv_rects_t  *style_p;

	switch(style) {
		case LV_RECTS_DEF:
			style_p = &lv_rects_def;
			break;
		case LV_RECTS_BORDER:
			style_p = &lv_rects_border;
			break;
		case LV_RECTS_TRANSP:
			style_p = &lv_rects_transp;
			break;
		default:
			style_p = NULL;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_templs_t));
		else memcpy(copy_p, &lv_rects_def, sizeof(lv_templs_t));
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
 * Handle the drawing related tasks of the TEMPLATEs
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static bool lv_rect_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {

    	/*Return false if the object is not covers the mask_p area*/
    	return false;
    }


    /*Draw the object*/

    return true;
}


#endif
