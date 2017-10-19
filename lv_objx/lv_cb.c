/**
 * @file lv_cb.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_CB != 0

#include "lv_cb.h"
#include "../lv_obj/lv_group.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_cb_design(lv_obj_t * cb, const area_t * mask, lv_design_mode_t mode);
static bool lv_bullet_design(lv_obj_t * bullet, const area_t * mask, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_f_t ancestor_bg_design_f;
static lv_design_f_t ancestor_bullet_design_f;

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
 * Create a check box objects
 * @param par pointer to an object, it will be the parent of the new check box
 * @param copy pointer to a check box object, if not NULL then the new object will be copied from it
 * @return pointer to the created check box
 */
lv_obj_t * lv_cb_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor basic object*/
    lv_obj_t * new_cb = lv_btn_create(par, copy);
    dm_assert(new_cb);
    
    lv_cb_ext_t * ext = lv_obj_alloc_ext(new_cb, sizeof(lv_cb_ext_t));
    dm_assert(ext);
    ext->bullet = NULL;
    ext->label = NULL;

    if(ancestor_bg_design_f == NULL) ancestor_bg_design_f = lv_obj_get_design_f(new_cb);

    lv_obj_set_signal_f(new_cb, lv_cb_signal);
    lv_obj_set_design_f(new_cb, lv_cb_design);

    /*Init the new checkbox object*/
    if(copy == NULL) {
        ext->bullet = lv_btn_create(new_cb, NULL);
        if(ancestor_bullet_design_f == NULL) ancestor_bullet_design_f = lv_obj_get_design_f(ext->bullet);
        lv_btn_set_styles(new_cb, lv_style_get(LV_STYLE_TRANSPARENT, NULL), lv_style_get(LV_STYLE_TRANSPARENT, NULL),
                                  lv_style_get(LV_STYLE_TRANSPARENT, NULL), lv_style_get(LV_STYLE_TRANSPARENT, NULL),
                                  lv_style_get(LV_STYLE_TRANSPARENT, NULL));
        lv_cont_set_layout(new_cb, LV_CONT_LAYOUT_ROW_M);
        lv_cont_set_fit(new_cb, true, true);
        lv_btn_set_tgl(new_cb, true);

        lv_obj_set_click(ext->bullet, false);
        lv_btn_set_styles(ext->bullet, lv_style_get(LV_STYLE_PRETTY, NULL), lv_style_get(LV_STYLE_PRETTY_COLOR, NULL),
                                       lv_style_get(LV_STYLE_BUTTON_ON_RELEASED, NULL), lv_style_get(LV_STYLE_BUTTON_ON_PRESSED, NULL),
                                       lv_style_get(LV_STYLE_BUTTON_INACTIVE, NULL));

        ext->label = lv_label_create(new_cb, NULL);
        lv_obj_set_style(ext->label, NULL);     /*Inherit the style of the parent*/
        lv_label_set_text(ext->label, "Check box");
    } else {
    	lv_cb_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->bullet = lv_btn_create(new_cb, copy_ext->bullet);
    	ext->label = lv_label_create(new_cb, copy_ext->label);

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_cb);
    }

    lv_obj_set_design_f(ext->bullet, lv_bullet_design);
    
    return new_cb;
}


/**
 * Signal function of the check box
 * @param cb pointer to a check box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_cb_signal(lv_obj_t * cb, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_btn_signal(cb, sign, param);

    lv_cb_ext_t * ext = lv_obj_get_ext(cb);
    lv_style_t * style = lv_obj_get_style(cb);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	if(sign == LV_SIGNAL_STYLE_CHG) {
    		lv_obj_set_size(ext->bullet, font_get_height(style->text.font), font_get_height(style->text.font));
    	} else if(sign == LV_SIGNAL_PRESSED ||
            sign == LV_SIGNAL_RELEASED ||
            sign == LV_SIGNAL_PRESS_LOST) {
            lv_btn_set_state(lv_cb_get_bullet(cb), lv_btn_get_state(cb));
        } else if(sign == LV_SIGNAL_CONTROLL) {
            char c = *((char*)param);
            if(c == LV_GROUP_KEY_RIGHT || c == LV_GROUP_KEY_DOWN ||
               c == LV_GROUP_KEY_LEFT || c == LV_GROUP_KEY_UP ||
               c == LV_GROUP_KEY_ENTER) {
                lv_btn_set_state(lv_cb_get_bullet(cb), lv_btn_get_state(cb));
            }
        }
    }
    
    return valid;
}


/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a check box
 * @param cb pointer to a check box
 * @param txt the text of the check box
 */
void lv_cb_set_text(lv_obj_t * cb, const char * txt)
{
	lv_cb_ext_t * ext = lv_obj_get_ext(cb);
	lv_label_set_text(ext->label, txt);
}


/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param cb pointer to check box object
 * @return pointer to the text of the check box
 */
const char * lv_cb_get_text(lv_obj_t * cb)
{
	lv_cb_ext_t * ext = lv_obj_get_ext(cb);
	return lv_label_get_text(ext->label);
}

/**
 * Get the bullet (lv_btn) of a check box
 * @param cb pointer to check box object
 * @return pointer to the bullet of the check box (lv_btn)
 */
lv_obj_t *  lv_cb_get_bullet(lv_obj_t * cb)
{
    lv_cb_ext_t * ext = lv_obj_get_ext(cb);
    return ext->bullet;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the check boxes
 * @param cb pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_cb_design(lv_obj_t * cb, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return ancestor_bg_design_f(cb, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN || mode == LV_DESIGN_DRAW_POST) {
        lv_cb_ext_t * cb_ext = lv_obj_get_ext(cb);
        lv_btn_ext_t * bullet_ext = lv_obj_get_ext(cb_ext->bullet);

        /*Be sure the state of the bullet is the same as the parent button*/
        bullet_ext->state = cb_ext->bg_btn.state;

        return ancestor_bg_design_f(cb, mask, mode);

    } else {
        return ancestor_bg_design_f(cb, mask, mode);
    }

    return true;
}

/**
 * Handle the drawing related tasks of the check boxes
 * @param bullet pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_bullet_design(lv_obj_t * bullet, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_bullet_design_f(bullet, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
#if LV_OBJ_GROUP != 0
        /* If the check box is the active in a group and
         * the background is not visible (transparent or empty)
         * then activate the style of the bullet*/
        lv_style_t * style_ori = lv_obj_get_style(bullet);
        lv_obj_t * bg = lv_obj_get_parent(bullet);
        lv_style_t * style_page = lv_obj_get_style(bg);
        lv_group_t * g = lv_obj_get_group(bg);
        if(style_page->body.empty != 0 || style_page->opacity == OPA_TRANSP) { /*Background is visible?*/
            if(lv_group_get_focused(g) == bg) {
                lv_style_t * style_mod;
                style_mod = lv_group_mod_style(g, style_ori);
                bullet->style_p = style_mod;  /*Temporally change the style to the activated */
            }
        }
#endif
        ancestor_bullet_design_f(bullet, mask, mode);

#if LV_OBJ_GROUP != 0
        bullet->style_p = style_ori;  /*Revert the style*/
#endif
    } else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_bullet_design_f(bullet, mask, mode);
    }

    return true;
}

#endif
