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
static void lv_temps_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_templs_t lv_templs_def;	/*Default template style*/

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
    /*Create the ancestor template*/
	/*TODO modify it to the ancestor create function */
    lv_obj_t * new_templ = lv_obj_create(par, copy);
    dm_assert(new_templ);
    
    /*Allocate the template type specific extended data*/
    lv_templ_ext_t * ext = lv_obj_alloc_ext(new_templ, sizeof(lv_templ_ext_t));
    dm_assert(ext);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_templ, lv_templ_signal);
    lv_obj_set_design_f(new_templ, lv_templ_design);

    /*Init the new template template*/
    if(copy == NULL) {

    }
    /*Copy an existing template*/
    else {
    	lv_templ_ext_t * copy_ext = lv_obj_get_ext(copy);
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
    valid = lv_obj_signal(templ, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			/*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    			break;
    		default:
    			break;
    	}
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/


/*=====================
 * Getter functions
 *====================*/

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_templs_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_templs_t style
 */
lv_templs_t * lv_templs_get(lv_templs_builtin_t style, lv_templs_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_temps_init();
		style_inited = true;
	}

	lv_templs_t  *style_p;

	switch(style) {
		case LV_TEMPLS_DEF:
			style_p = &lv_templs_def;
			break;
		default:
			style_p = &lv_templs_def;
	}

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_templs_t));
		else memcpy(copy, &lv_templs_def, sizeof(lv_templs_t));
	}

	return style_p;
}

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
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return false;
    }


    /*Draw the object*/

    return true;
}


/**
 * Initialize the template styles
 */
static void lv_temps_init(void)
{
	/*Default style*/
}

#endif
