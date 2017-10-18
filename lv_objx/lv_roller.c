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
static bool roller_scrl_signal(lv_obj_t * roller_scrl, lv_signal_t sign, void * indev);

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
        lv_obj_set_drag(scrl, true);                    /*In ddlist is might be disabled*/
        lv_page_set_rel_action(new_roller, NULL);       /*Handle roller specific actions*/
        lv_cont_set_fit(lv_page_get_scrl(new_roller), true, false); /*Height is specified directly*/
        lv_obj_set_signal_f(scrl, roller_scrl_signal);
        lv_ddlist_open(new_roller, true, 0);

        lv_style_t * style_label = lv_obj_get_style(ext->ddlist.opt_label);
        lv_ddlist_set_fix_height(new_roller, (font_get_height(style_label->txt.font)  >> FONT_ANTIALIAS) * 3
                                      + style_label->txt.space_line * 4);
        lv_obj_refr_style(new_roller);                /*To set scrollable size automatically*/
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
    valid = lv_ddlist_signal(roller, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        lv_roller_ext_t * ext = lv_obj_get_ext(roller);
    	if(sign == LV_SIGNAL_STYLE_CHG) {
            lv_obj_set_height(lv_page_get_scrl(roller),
                                 lv_obj_get_height(ext->ddlist.opt_label) + lv_obj_get_height(roller));
            lv_obj_align(ext->ddlist.opt_label, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_ddlist_set_selected(roller, ext->ddlist.sel_opt);
    	} else if(sign == LV_SIGNAL_CORD_CHG) {
    	    lv_ddlist_set_fix_height(roller, lv_obj_get_height(roller));
            lv_obj_set_height(lv_page_get_scrl(roller),
                                 lv_obj_get_height(ext->ddlist.opt_label) + lv_obj_get_height(roller));
            lv_obj_align(ext->ddlist.opt_label, NULL, LV_ALIGN_CENTER, 0, 0);
    	    lv_ddlist_set_selected(roller, ext->ddlist.sel_opt);
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

        const font_t * font = style->txt.font;
        lv_roller_ext_t * ext = lv_obj_get_ext(roller);
        cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
        area_t rect_area;
        rect_area.y1 = roller->cords.y1 + lv_obj_get_height(roller) / 2 - font_h / 2 - style->txt.space_line - 2;
        rect_area.y2 = rect_area.y1 + font_h + style->txt.space_line;
        rect_area.x1 = ext->ddlist.opt_label->cords.x1 - style->body.pad_hor;
        rect_area.x2 = rect_area.x1 + lv_obj_get_width(lv_page_get_scrl(roller));

        lv_draw_rect(&rect_area, mask, ext->ddlist.style_sel);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}

/**
 * Signal function of the scrollable part of the roller.
 * @param roller_scrl ointer to the scrollable part of roller (page)
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
static bool roller_scrl_signal(lv_obj_t * roller_scrl, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = ancestor_scr_signal_f(roller_scrl, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        lv_indev_t * indev = lv_indev_get_act();
        int32_t id = -1;
        lv_obj_t * roller = lv_obj_get_parent(roller_scrl);
        lv_roller_ext_t * ext = lv_obj_get_ext(roller);
        lv_style_t * style_label = lv_obj_get_style(ext->ddlist.opt_label);
        const font_t * font = style_label->txt.font;
        cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
        if(sign == LV_SIGNAL_DRAG_END) {
            /*If dragged then align the list to there be an element in the middle*/
            cord_t label_y1 = ext->ddlist.opt_label->cords.y1 - roller->cords.y1;
            cord_t label_unit = (font_get_height(style_label->txt.font) >> FONT_ANTIALIAS) + style_label->txt.space_line / 2;
            cord_t mid = (roller->cords.y2 - roller->cords.y1) / 2;
            id = (mid - label_y1) / label_unit;
            if(id < 0) id = 0;
            if(id >= ext->ddlist.num_opt) id = ext->ddlist.num_opt - 1;
            ext->ddlist.sel_opt = id;
        }
        else if(sign == LV_SIGNAL_RELEASED) {
            /*If picked an option by clicking then set it*/
            if(!lv_indev_is_dragging(indev)) {
                point_t p;
                lv_indev_get_point(indev, &p);
                p.y = p.y - ext->ddlist.opt_label->cords.y1;
                id = p.y / (font_h + style_label->txt.space_line);
                if(id < 0) id = 0;
                if(id >= ext->ddlist.num_opt) id = ext->ddlist.num_opt - 1;
                ext->ddlist.sel_opt = id;
            }
        }

        /*Position the scrollable according to the new selected option*/
        if(id != -1) {
            cord_t h = lv_obj_get_height(roller);
            cord_t line_y1 = id * (font_h + style_label->txt.space_line) + ext->ddlist.opt_label->cords.y1 - roller_scrl->cords.y1;
            cord_t new_y = - line_y1 + (h - font_h) / 2;

            if(ext->ddlist.anim_time == 0) {
                lv_obj_set_y(roller_scrl, new_y);
            } else {
                anim_t a;
                a.var = roller_scrl;
                a.start = lv_obj_get_y(roller_scrl);
                a.end = new_y;
                a.fp = (anim_fp_t)lv_obj_set_y;
                a.path = anim_get_path(ANIM_PATH_LIN);
                a.end_cb = NULL;
                a.act_time = 0;
                a.time = ext->ddlist.anim_time;
                a.playback = 0;
                a.playback_pause = 0;
                a.repeat = 0;
                a.repeat_pause = 0;
                anim_create(&a);
            }
        }
    }

    return valid;
}



#endif
