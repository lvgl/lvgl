/**
 * @file lv_cb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_cb.h"
#if LV_USE_CB != 0

#include "../lv_core/lv_group.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_cb_design(lv_obj_t * cb, const lv_area_t * mask, lv_design_mode_t mode);
static bool lv_bullet_design(lv_obj_t * bullet, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_cb_signal(lv_obj_t * cb, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_cb_t ancestor_bg_design;
static lv_design_cb_t ancestor_bullet_design;
static lv_signal_cb_t ancestor_signal;

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
lv_obj_t * lv_cb_create(lv_obj_t * par, const lv_obj_t * copy)
{

    LV_LOG_TRACE("check box create started");

    /*Create the ancestor basic object*/
    lv_obj_t * new_cb = lv_btn_create(par, copy);
    lv_mem_assert(new_cb);
    if(new_cb == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_cb);
    if(ancestor_bg_design == NULL) ancestor_bg_design = lv_obj_get_design_cb(new_cb);

    lv_cb_ext_t * ext = lv_obj_allocate_ext_attr(new_cb, sizeof(lv_cb_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->bullet = NULL;
    ext->label  = NULL;

    lv_obj_set_signal_cb(new_cb, lv_cb_signal);
    lv_obj_set_design_cb(new_cb, lv_cb_design);

    /*Init the new checkbox object*/
    if(copy == NULL) {
        ext->bullet = lv_btn_create(new_cb, NULL);
        if(ancestor_bullet_design == NULL) ancestor_bullet_design = lv_obj_get_design_cb(ext->bullet);
        lv_obj_set_click(ext->bullet, false);

        ext->label = lv_label_create(new_cb, NULL);

        lv_cb_set_text(new_cb, "Check box");
        lv_btn_set_layout(new_cb, LV_LAYOUT_ROW_M);
        lv_btn_set_fit(new_cb, LV_FIT_TIGHT);
        lv_btn_set_toggle(new_cb, true);
        lv_obj_set_protect(new_cb, LV_PROTECT_PRESS_LOST);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_cb_set_style(new_cb, LV_CB_STYLE_BG, th->style.cb.bg);
            lv_cb_set_style(new_cb, LV_CB_STYLE_BOX_REL, th->style.cb.box.rel);
            lv_cb_set_style(new_cb, LV_CB_STYLE_BOX_PR, th->style.cb.box.pr);
            lv_cb_set_style(new_cb, LV_CB_STYLE_BOX_TGL_REL, th->style.cb.box.tgl_rel);
            lv_cb_set_style(new_cb, LV_CB_STYLE_BOX_TGL_PR, th->style.cb.box.tgl_pr);
            lv_cb_set_style(new_cb, LV_CB_STYLE_BOX_INA, th->style.cb.box.ina);
        } else {
            lv_cb_set_style(new_cb, LV_CB_STYLE_BG, &lv_style_transp);
            lv_cb_set_style(new_cb, LV_CB_STYLE_BOX_REL, &lv_style_pretty);
        }
    } else {
        lv_cb_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->bullet            = lv_btn_create(new_cb, copy_ext->bullet);
        ext->label             = lv_label_create(new_cb, copy_ext->label);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_cb);
    }

    lv_obj_set_design_cb(ext->bullet, lv_bullet_design);

    LV_LOG_INFO("check box created");

    return new_cb;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a check box. `txt` will be copied and may be deallocated
 * after this function returns.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void lv_cb_set_text(lv_obj_t * cb, const char * txt)
{
    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
    lv_label_set_text(ext->label, txt);
}

/**
 * Set the text of a check box. `txt` must not be deallocated during the life
 * of this checkbox.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void lv_cb_set_static_text(lv_obj_t * cb, const char * txt)
{
    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
    lv_label_set_static_text(ext->label, txt);
}

/**
 * Set a style of a check box
 * @param cb pointer to check box object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_cb_set_style(lv_obj_t * cb, lv_cb_style_t type, const lv_style_t * style)
{
    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);

    switch(type) {
        case LV_CB_STYLE_BG:
            lv_btn_set_style(cb, LV_BTN_STYLE_REL, style);
            lv_btn_set_style(cb, LV_BTN_STYLE_PR, style);
            lv_btn_set_style(cb, LV_BTN_STYLE_TGL_REL, style);
            lv_btn_set_style(cb, LV_BTN_STYLE_TGL_PR, style);
            lv_btn_set_style(cb, LV_BTN_STYLE_INA, style);
            break;
        case LV_CB_STYLE_BOX_REL: lv_btn_set_style(ext->bullet, LV_BTN_STYLE_REL, style); break;
        case LV_CB_STYLE_BOX_PR: lv_btn_set_style(ext->bullet, LV_BTN_STYLE_PR, style); break;
        case LV_CB_STYLE_BOX_TGL_REL: lv_btn_set_style(ext->bullet, LV_BTN_STYLE_TGL_REL, style); break;
        case LV_CB_STYLE_BOX_TGL_PR: lv_btn_set_style(ext->bullet, LV_BTN_STYLE_TGL_PR, style); break;
        case LV_CB_STYLE_BOX_INA: lv_btn_set_style(ext->bullet, LV_BTN_STYLE_INA, style); break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param cb pointer to check box object
 * @return pointer to the text of the check box
 */
const char * lv_cb_get_text(const lv_obj_t * cb)
{
    lv_cb_ext_t * ext = lv_obj_get_ext_attr(cb);
    return lv_label_get_text(ext->label);
}

/**
 * Get a style of a button
 * @param cb pointer to check box object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
const lv_style_t * lv_cb_get_style(const lv_obj_t * cb, lv_cb_style_t type)
{
    const lv_style_t * style = NULL;
    lv_cb_ext_t * ext        = lv_obj_get_ext_attr(cb);

    switch(type) {
        case LV_CB_STYLE_BOX_REL: style = lv_btn_get_style(ext->bullet, LV_BTN_STYLE_REL); break;
        case LV_CB_STYLE_BOX_PR: style = lv_btn_get_style(ext->bullet, LV_BTN_STYLE_PR); break;
        case LV_CB_STYLE_BOX_TGL_REL: style = lv_btn_get_style(ext->bullet, LV_BTN_STYLE_TGL_REL); break;
        case LV_CB_STYLE_BOX_TGL_PR: style = lv_btn_get_style(ext->bullet, LV_BTN_STYLE_TGL_PR); break;
        case LV_CB_STYLE_BOX_INA: style = lv_btn_get_style(ext->bullet, LV_BTN_STYLE_INA); break;
        default: style = NULL; break;
    }

    return style;
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
static bool lv_cb_design(lv_obj_t * cb, const lv_area_t * mask, lv_design_mode_t mode)
{
    bool result = true;

    if(mode == LV_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask_p area*/
        result = ancestor_bg_design(cb, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN || mode == LV_DESIGN_DRAW_POST) {
        lv_cb_ext_t * cb_ext      = lv_obj_get_ext_attr(cb);
        lv_btn_ext_t * bullet_ext = lv_obj_get_ext_attr(cb_ext->bullet);

        /*Be sure the state of the bullet is the same as the parent button*/
        bullet_ext->state = cb_ext->bg_btn.state;

        result = ancestor_bg_design(cb, mask, mode);

    } else {
        result = ancestor_bg_design(cb, mask, mode);
    }

    return result;
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
static bool lv_bullet_design(lv_obj_t * bullet, const lv_area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_bullet_design(bullet, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
#if LV_USE_GROUP
        /* If the check box is the active in a group and
         * the background is not visible (transparent)
         * then activate the style of the bullet*/
        const lv_style_t * style_ori  = lv_obj_get_style(bullet);
        lv_obj_t * bg                 = lv_obj_get_parent(bullet);
        const lv_style_t * style_page = lv_obj_get_style(bg);
        lv_group_t * g                = lv_obj_get_group(bg);
        if(style_page->body.opa == LV_OPA_TRANSP) { /*Is the Background visible?*/
            if(lv_group_get_focused(g) == bg) {
                lv_style_t * style_mod;
                style_mod       = lv_group_mod_style(g, style_ori);
                bullet->style_p = style_mod; /*Temporally change the style to the activated */
            }
        }
#endif
        ancestor_bullet_design(bullet, mask, mode);

#if LV_USE_GROUP
        bullet->style_p = style_ori; /*Revert the style*/
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

    if(sign == LV_SIGNAL_STYLE_CHG) {
        const lv_style_t * label_style = lv_label_get_style(ext->label, LV_LABEL_STYLE_MAIN);
        lv_obj_set_size(ext->bullet, lv_font_get_line_height(label_style->text.font),
                        lv_font_get_line_height(label_style->text.font));
        lv_btn_set_state(ext->bullet, lv_btn_get_state(cb));
    } else if(sign == LV_SIGNAL_PRESSED || sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        lv_btn_set_state(ext->bullet, lv_btn_get_state(cb));
    } else if(sign == LV_SIGNAL_CONTROL) {
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT || c == LV_KEY_DOWN || c == LV_KEY_LEFT || c == LV_KEY_UP) {
            /*Follow the backgrounds state with the bullet*/
            lv_btn_set_state(ext->bullet, lv_btn_get_state(cb));
        }
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_cb";
    }

    return res;
}

#endif
