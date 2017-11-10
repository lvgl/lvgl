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
static lv_res_t lv_cb_signal(lv_obj_t * cb, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_func_t ancestor_bg_design;
static lv_design_func_t ancestor_bullet_design;
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

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
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_cb);
    if(ancestor_bg_design == NULL) ancestor_bg_design = lv_obj_get_design_func(new_cb);
    
    lv_cb_ext_t * ext = lv_obj_allocate_ext_attr(new_cb, sizeof(lv_cb_ext_t));
    dm_assert(ext);
    ext->bullet = NULL;
    ext->label = NULL;

    lv_obj_set_signal_func(new_cb, lv_cb_signal);
    lv_obj_set_design_func(new_cb, lv_cb_design);

    /*Init the new checkbox object*/
    if(copy == NULL) {
        ext->bullet = lv_btn_create(new_cb, NULL);
        if(ancestor_bullet_design == NULL) ancestor_bullet_design = lv_obj_get_design_func(ext->bullet);
        lv_obj_set_click(ext->bullet, false);
        lv_btn_set_style(ext->bullet, &lv_style_pretty, &lv_style_pretty_color,
                                       &lv_style_btn_on_released, &lv_style_btn_on_pressed,
                                       NULL);

        ext->label = lv_label_create(new_cb, NULL);
        lv_obj_set_style(ext->label, NULL);     /*Inherit the style of the parent*/

        lv_cb_set_style(new_cb, &lv_style_transp);
        lv_cb_set_text(new_cb, "Check box");
        lv_cont_set_layout(new_cb, LV_CONT_LAYOUT_ROW_M);
        lv_cont_set_fit(new_cb, true, true);
        lv_btn_set_toggle(new_cb, true);

        lv_obj_refresh_style(new_cb);
    } else {
    	lv_cb_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	ext->bullet = lv_btn_create(new_cb, copy_ext->bullet);
    	ext->label = lv_label_create(new_cb, copy_ext->label);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_cb);
    }

    lv_obj_set_design_func(ext->bullet, lv_bullet_design);
    
    return new_cb;
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
	lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
	lv_label_set_text(ext->label, txt);
}

/**
 * Set styles of a checkbox's bullet in each state. Use NULL for any style to leave it unchanged
 * @param cb pointer to check box object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 * @param tgl_rel pointer to a style for toggled releases state
 * @param tgl_pr pointer to a style for toggled pressed state
 * @param ina pointer to a style for inactive state
 */
void lv_cb_set_style_bullet(lv_obj_t *cb, lv_style_t *rel, lv_style_t *pr,
                                          lv_style_t *tgl_rel, lv_style_t *tgl_pr,
                                          lv_style_t *ina)
{
    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
    lv_btn_set_style(ext->bullet, rel, pr, tgl_rel, tgl_pr, ina);
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
	lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
	return lv_label_get_text(ext->label);
}

/**
 * Get styles of a checkbox's bullet in a  state.
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_cb_get_style_bullet(lv_obj_t *cb, lv_btn_state_t state)
{
    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
    return lv_btn_get_style(ext->bullet, state);
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
    	return ancestor_bg_design(cb, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN || mode == LV_DESIGN_DRAW_POST) {
        lv_cb_ext_t * cb_ext = lv_obj_get_ext_attr(cb);
        lv_btn_ext_t * bullet_ext = lv_obj_get_ext_attr(cb_ext->bullet);

        /*Be sure the state of the bullet is the same as the parent button*/
        bullet_ext->state = cb_ext->bg_btn.state;

        return ancestor_bg_design(cb, mask, mode);

    } else {
        return ancestor_bg_design(cb, mask, mode);
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
        return ancestor_bullet_design(bullet, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
#if LV_OBJ_GROUP != 0
        /* If the check box is the active in a group and
         * the background is not visible (transparent or empty)
         * then activate the style of the bullet*/
        lv_style_t * style_ori = lv_obj_get_style(bullet);
        lv_obj_t * bg = lv_obj_get_parent(bullet);
        lv_style_t * style_page = lv_obj_get_style(bg);
        lv_group_t * g = lv_obj_get_group(bg);
        if(style_page->body.empty != 0 || style_page->body.opa == OPA_TRANSP) { /*Background is visible?*/
            if(lv_group_get_focused(g) == bg) {
                lv_style_t * style_mod;
                style_mod = lv_group_mod_style(g, style_ori);
                bullet->style_p = style_mod;  /*Temporally change the style to the activated */
            }
        }
#endif
        ancestor_bullet_design(bullet, mask, mode);

#if LV_OBJ_GROUP != 0
        bullet->style_p = style_ori;  /*Revert the style*/
#endif
    } else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_bullet_design(bullet, mask, mode);
    }

    return true;
}


/**
 * Signal function of the check box
 * @param cb pointer to a check box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_cb_signal(lv_obj_t * cb, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(cb, sign, param);
    if(res != LV_RES_OK) return res;

    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
    lv_style_t * style = lv_obj_get_style(cb);

    if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_obj_set_size(ext->bullet, font_get_height(style->text.font), font_get_height(style->text.font));
        lv_btn_set_state(ext->bullet, lv_btn_get_state(cb));
    } else if(sign == LV_SIGNAL_PRESSED ||
        sign == LV_SIGNAL_RELEASED ||
        sign == LV_SIGNAL_PRESS_LOST) {
        lv_btn_set_state(ext->bullet, lv_btn_get_state(cb));
    } else if(sign == LV_SIGNAL_CONTROLL) {
        char c = *((char*)param);
        if(c == LV_GROUP_KEY_RIGHT || c == LV_GROUP_KEY_DOWN ||
           c == LV_GROUP_KEY_LEFT || c == LV_GROUP_KEY_UP ||
           c == LV_GROUP_KEY_ENTER) {
            lv_btn_set_state(ext->bullet, lv_btn_get_state(cb));
        }
    }

    return LV_RES_OK;
}

#endif
