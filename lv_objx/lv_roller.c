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
static lv_res_t lv_roller_scrl_signal(lv_obj_t * roller_scrl, lv_signal_t sign, void * param);
static lv_res_t lv_roller_signal(lv_obj_t * roller, lv_signal_t sign, void * param);
static void refr_position(lv_obj_t *roller, bool anim_en);
static void draw_bg(lv_obj_t *roller, const area_t *mask);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_signal_func_t ancestor_scrl_signal;

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
    if(ancestor_scrl_signal == NULL) ancestor_scrl_signal = lv_obj_get_signal_func(lv_page_get_scrl(new_roller));
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_roller);
    
    /*Allocate the roller type specific extended data*/
    lv_roller_ext_t * ext = lv_obj_allocate_ext_attr(new_roller, sizeof(lv_roller_ext_t));
    dm_assert(ext);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_roller, lv_roller_signal);
    lv_obj_set_design_func(new_roller, lv_roller_design);

    /*Init the new roller roller*/
    if(copy == NULL) {
        lv_obj_t * scrl = lv_page_get_scrl(new_roller);
        lv_obj_set_drag(scrl, true);                        /*In ddlist is might be disabled*/
        lv_page_set_release_action(new_roller, NULL);       /*Roller don't uses it (like ddlist)*/
        lv_page_set_scrl_fit(new_roller, true, false);      /*Height is specified directly*/
        lv_ddlist_open(new_roller, false);
        lv_style_t * style_label = lv_obj_get_style(ext->ddlist.options_label);
        lv_ddlist_set_fix_height(new_roller, (font_get_height(style_label->text.font) >> FONT_ANTIALIAS) * 3
                                                                   + style_label->text.line_space * 4);

        lv_label_set_align(ext->ddlist.options_label, LV_LABEL_ALIGN_CENTER);

        lv_obj_set_signal_func(scrl, lv_roller_scrl_signal);
        lv_obj_refresh_style(new_roller);                /*To set scrollable size automatically*/
    }
    /*Copy an existing roller*/
    else {
        lv_obj_t * scrl = lv_page_get_scrl(new_roller);
        lv_ddlist_open(new_roller, false);
        lv_obj_set_signal_func(scrl, lv_roller_scrl_signal);

        lv_obj_refresh_style(new_roller);        /*Refresh the style with new signal function*/
    }
    
    return new_roller;
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

/**
 * Set the selected option
 * @param roller pointer to a roller object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 * @param anim_en true: set with animation; false set immediately
 */
void lv_roller_set_selected(lv_obj_t *roller, uint16_t sel_opt, bool anim_en)
{
    lv_ddlist_set_selected(roller, sel_opt);
    refr_position(roller, anim_en);
}

/**
 * Enable/disable to set the width of the roller manually (by lv_obj_Set_width())
 * @param roller pointer to a roller object
 * @param fit_en: true: enable auto size; false: use manual width settings
 */
void lv_roller_set_hor_fit(lv_obj_t *roller, bool fit_en)
{
    lv_page_set_scrl_fit(roller, fit_en ,false);
    lv_cont_set_fit(roller, fit_en ,false);
}


/*=====================
 * Getter functions
 *====================*/

/**
 * Get the auto width set attribute
 * @param roller pointer to a roller object
 * @return true: auto size enabled; false: manual width settings enabled
 */
bool lv_roller_get_hor_fit(lv_obj_t *roller)
{
    return lv_page_get_scrl_hor_fit(roller);
}

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

        draw_bg(roller, mask);

        lv_style_t *style = lv_ddlist_get_style_bg(roller);
        const font_t * font = style->text.font;
        lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
        cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
        area_t rect_area;
        rect_area.y1 = roller->coords.y1 + lv_obj_get_height(roller) / 2 - font_h / 2 - style->text.line_space / 2;
        rect_area.y2 = rect_area.y1 + font_h + style->text.line_space;
        rect_area.x1 = roller->coords.x1;
        rect_area.x2 = roller->coords.x2;

        lv_draw_rect(&rect_area, mask, ext->ddlist.selected_style);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}

/**
 * Signal function of the roller
 * @param roller pointer to a roller object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_roller_signal(lv_obj_t * roller, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(roller, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(res == LV_RES_OK) {
        lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
        if(sign == LV_SIGNAL_STYLE_CHG) {
            lv_obj_set_height(lv_page_get_scrl(roller),
                                 lv_obj_get_height(ext->ddlist.options_label) + lv_obj_get_height(roller));
            lv_obj_align(ext->ddlist.options_label, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_ddlist_set_selected(roller, ext->ddlist.selected_option_id);
            refr_position(roller, false);
        } else if(sign == LV_SIGNAL_CORD_CHG) {

            if(lv_obj_get_width(roller) != area_get_width(param) ||
               lv_obj_get_height(roller) != area_get_height(param)) {

                lv_ddlist_set_fix_height(roller, lv_obj_get_height(roller));
                lv_obj_set_height(lv_page_get_scrl(roller),
                                     lv_obj_get_height(ext->ddlist.options_label) + lv_obj_get_height(roller));

                lv_obj_align(ext->ddlist.options_label, NULL, LV_ALIGN_CENTER, 0, 0);
                lv_ddlist_set_selected(roller, ext->ddlist.selected_option_id);
                refr_position(roller, false);
            }
        }
    }

    return res;
}

/**
 * Signal function of the scrollable part of the roller.
 * @param roller_scrl ointer to the scrollable part of roller (page)
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_roller_scrl_signal(lv_obj_t * roller_scrl, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_scrl_signal(roller_scrl, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(res == LV_RES_OK) {
        lv_indev_t * indev = lv_indev_get_act();
        int32_t id = -1;
        lv_obj_t * roller = lv_obj_get_parent(roller_scrl);
        lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
        lv_style_t * style_label = lv_obj_get_style(ext->ddlist.options_label);
        const font_t * font = style_label->text.font;
        cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
        if(sign == LV_SIGNAL_DRAG_END) {
            /*If dragged then align the list to there be an element in the middle*/
            cord_t label_y1 = ext->ddlist.options_label->coords.y1 - roller->coords.y1;
            cord_t label_unit = font_h + style_label->text.line_space;
            cord_t mid = (roller->coords.y2 - roller->coords.y1) / 2;
            id = (mid - label_y1 + style_label->text.line_space / 2) / label_unit;
            if(id < 0) id = 0;
            if(id >= ext->ddlist.option_cnt) id = ext->ddlist.option_cnt - 1;
            ext->ddlist.selected_option_id = id;
        }
        else if(sign == LV_SIGNAL_RELEASED) {
            /*If picked an option by clicking then set it*/
            if(!lv_indev_is_dragging(indev)) {
                point_t p;
                lv_indev_get_point(indev, &p);
                p.y = p.y - ext->ddlist.options_label->coords.y1;
                id = p.y / (font_h + style_label->text.line_space);
                if(id < 0) id = 0;
                if(id >= ext->ddlist.option_cnt) id = ext->ddlist.option_cnt - 1;
                ext->ddlist.selected_option_id = id;
            }
        }

        /*Position the scrollable according to the new selected option*/
        if(id != -1) {
            refr_position(roller, true);
        }
    }

    return res;
}

/**
 * Draw a rectangle which has gradient on its top and bottom
 * @param roller pointer to a roller object
 * @param mask pointer to the current mask (from the design function)
 */
static void draw_bg(lv_obj_t *roller, const area_t *mask)
{
    lv_style_t *style = lv_ddlist_get_style_bg(roller);
    area_t half_mask;
    area_t half_roller;
    cord_t h = lv_obj_get_height(roller);
    bool union_ok;
    area_cpy(&half_roller, &roller->coords);

    half_roller.x1 -= roller->ext_size; /*Add ext size too (e.g. because of shadow draw) */
    half_roller.x2 += roller->ext_size;
    half_roller.y1 -= roller->ext_size;
    half_roller.y2 = roller->coords.y1 + h / 2;

    union_ok = area_union(&half_mask, &half_roller, mask);

    half_roller.x1 += roller->ext_size; /*Revert ext. size adding*/
    half_roller.x2 -= roller->ext_size;
    half_roller.y1 += roller->ext_size;
    half_roller.y2 += style->body.radius;

    if(union_ok) {
        lv_draw_rect(&half_roller, &half_mask, style);
    }

    half_roller.x1 -= roller->ext_size; /*Add ext size too (e.g. because of shadow draw) */
    half_roller.x2 += roller->ext_size;
    half_roller.y2 = roller->coords.y2 + roller->ext_size;
    half_roller.y1 = roller->coords.y1 + h / 2;
    if((h & 0x1) == 0) half_roller.y1++;    /*With even height the pixels in the middle would be drawn twice*/

    union_ok = area_union(&half_mask, &half_roller, mask);

    half_roller.x1 += roller->ext_size; /*Revert ext. size adding*/
    half_roller.x2 -= roller->ext_size;
    half_roller.y2 -= roller->ext_size;
    half_roller.y1 -= style->body.radius;

    if(union_ok){
        color_t main_tmp = style->body.color_main;
        color_t grad_tmp = style->body.color_gradient;

        style->body.color_main = grad_tmp;
        style->body.color_gradient = main_tmp;
        lv_draw_rect(&half_roller, &half_mask, style);
        style->body.color_main = main_tmp;
        style->body.color_gradient = grad_tmp;
    }

}

/**
 * Refresh the position of the roller. It uses the id stored in: ext->ddlist.selected_option_id
 * @param roller pointer to a roller object
 * @param anim_en true: refresh with animation; false: without animation
 */
static void refr_position(lv_obj_t *roller, bool anim_en)
{
    lv_obj_t *roller_scrl = lv_page_get_scrl(roller);
    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    lv_style_t * style_label = lv_obj_get_style(ext->ddlist.options_label);
    const font_t * font = style_label->text.font;
    cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
    cord_t h = lv_obj_get_height(roller);
    int32_t id = ext->ddlist.selected_option_id;
    cord_t line_y1 = id * (font_h + style_label->text.line_space) + ext->ddlist.options_label->coords.y1 - roller_scrl->coords.y1;
    cord_t new_y = - line_y1 + (h - font_h) / 2;

    if(ext->ddlist.anim_time == 0 || anim_en == false) {
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

#endif
