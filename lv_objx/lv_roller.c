/**
 * @file lv_roller.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_ROLLER != 0

#include "lv_roller.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_roller_design(lv_obj_t * roller, const area_t * mask, lv_design_mode_t mode);
bool roller_scrl_signal(lv_obj_t * roller_scrl, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_f_t ancestor_scr_signal_f;

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
 * Create a roller object
 * @param par pointer to an object, it will be the parent of the new roller
 * @param copy pointer to a roller object, if not NULL then the new object will be copied from it
 * @return pointer to the created roller
 */
lv_obj_t * lv_roller_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of roller*/
	/*TODO modify it to the ancestor create function */
    lv_obj_t * new_roller = lv_ddlist_create(par, copy);
    dm_assert(new_roller);
    
    /*Allocate the roller type specific extended data*/
    lv_roller_ext_t * ext = lv_obj_alloc_ext(new_roller, sizeof(lv_roller_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_roller, lv_roller_signal);
    lv_obj_set_design_f(new_roller, lv_roller_design);

    if(ancestor_scr_signal_f == NULL) ancestor_scr_signal_f = lv_obj_get_signal_f(lv_page_get_scrl(new_roller));

    /*Init the new roller roller*/
    if(copy == NULL) {
        lv_obj_t * scrl = lv_page_get_scrl(new_roller);
        lv_obj_set_drag(scrl, true);
        lv_page_set_rel_action(new_roller, NULL);
        lv_obj_set_signal_f(scrl, roller_scrl_signal);
       // lv_ddlist_open(new_roller, true, false);
        lv_obj_set_style(new_roller, lv_style_get(LV_STYLE_PRETTY, NULL));
        lv_cont_set_fit(lv_page_get_scrl(new_roller), true, false);
        lv_ddlist_set_options_str(new_roller, "alma\nkorte\ncitrom\nbanan\neper\ndinnye");

        lv_style_t * style_label = lv_obj_get_style(ext->ddlist.opt_label);
        lv_obj_set_height(new_roller, (font_get_height(style_label->font)  >> FONT_ANTIALIAS) * 3 + style_label->line_space * 4);

        lv_obj_set_height(lv_page_get_scrl(new_roller), lv_obj_get_height(ext->ddlist.opt_label) + lv_obj_get_height(new_roller));
        lv_obj_align(ext->ddlist.opt_label, NULL, LV_ALIGN_CENTER, 0, 0);

    }
    /*Copy an existing roller*/
    else {
    	lv_roller_ext_t * copy_ext = lv_obj_get_ext(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_roller);
    }
    
    return new_roller;
}

/**
 * Signal function of the roller
 * @param roller pointer to a roller object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_roller_signal(lv_obj_t * roller, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    /* TODO update it to the ancestor's signal function*/
    valid = lv_ddlist_signal(roller, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	if(sign == LV_SIGNAL_CLEANUP) {
            /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    	}
    }
    
    return valid;
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


/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" functions come here
 */

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
 * Handle the drawing related tasks of the rollers
 * @param roller pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_roller_design(lv_obj_t * roller, const area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {


        lv_style_t * style = lv_obj_get_style(roller);
        lv_draw_rect(&roller->cords, mask, style);

        const font_t * font = style->font;
        lv_roller_ext_t * ext = lv_obj_get_ext(roller);
        cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
        area_t rect_area;
        rect_area.y1 = lv_obj_get_height(roller) / 2 - font_h / 2 - style->line_space - 2;

        rect_area.y2 = rect_area.y1 + font_h + style->line_space;
        rect_area.x1 = ext->ddlist.opt_label->cords.x1 - style->hpad;
        rect_area.x2 = rect_area.x1 + lv_obj_get_width(lv_page_get_scrl(roller));

        lv_draw_rect(&rect_area, mask, ext->ddlist.style_sel);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}


bool roller_scrl_signal(lv_obj_t * roller_scrl, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = ancestor_scr_signal_f(roller_scrl, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        if(sign == LV_SIGNAL_RELEASED) {
            lv_obj_t * roller = lv_obj_get_parent(roller_scrl);
            lv_roller_ext_t * ext = lv_obj_get_ext(roller);
            lv_style_t * style_label = lv_obj_get_style(ext->ddlist.opt_label);

            cord_t label_y1 = ext->ddlist.opt_label->cords.y1;
            cord_t label_unit = (font_get_height(style_label->font) >> FONT_ANTIALIAS) + style_label->line_space / 2;
            cord_t mid = (roller->cords.y2 - roller->cords.y1) / 2;

            int32_t id = (mid - label_y1) / label_unit;

            printf("roller diff: %d , unit: %d, id: %d\n", mid-label_y1, label_unit, id);
        }
    }

    return valid;
}



#endif
