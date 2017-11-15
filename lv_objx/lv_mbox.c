/**
 * @file lv_mbox.c
 * 
 */


/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_MBOX != 0

#include "lv_mbox.h"
#include "../lv_obj/lv_group.h"
#include "misc/gfx/anim.h"
#include "misc/math/math_base.h"

/*********************
 *      DEFINES
 *********************/
#define LV_MBOX_CLOSE_ANIM_TIME     200 /*Default close anim. time [ms]*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_mbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param);
static void mbox_realign(lv_obj_t *mbox);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a message box objects
 * @param par pointer to an object, it will be the parent of the new message box
 * @param copy pointer to a message box object, if not NULL then the new object will be copied from it
 * @return pointer to the created message box
 */
lv_obj_t * lv_mbox_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor message box*/
	lv_obj_t * new_mbox = lv_cont_create(par, copy);
    dm_assert(new_mbox);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_mbox);
    
    /*Allocate the message box type specific extended data*/
    lv_mbox_ext_t * ext = lv_obj_allocate_ext_attr(new_mbox, sizeof(lv_mbox_ext_t));
    dm_assert(ext);
    ext->text = NULL;
    ext->btnm = NULL;
    ext->anim_time = LV_MBOX_CLOSE_ANIM_TIME;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_mbox, lv_mbox_signal);

    /*Init the new message box message box*/
    if(copy == NULL) {
    	ext->text = lv_label_create(new_mbox, NULL);
    	lv_label_set_align(ext->text, LV_LABEL_ALIGN_CENTER);
    	lv_label_set_long_mode(ext->text, LV_LABEL_LONG_BREAK);
    	lv_label_set_text(ext->text, "Message");

        lv_cont_set_layout(new_mbox, LV_CONT_LAYOUT_COL_M);
        lv_cont_set_fit(new_mbox, false, true);
        lv_obj_set_width(new_mbox, LV_HOR_RES / 3);

    	lv_mbox_set_style(new_mbox, LV_MBOX_STYLE_BG, &lv_style_pretty);
    }
    /*Copy an existing message box*/
    else {
        lv_mbox_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        ext->text = lv_label_create(new_mbox, copy_ext->text);

        /*Copy the buttons and the label on them*/
        if(copy_ext->btnm) ext->btnm = lv_btnm_create(new_mbox, copy_ext->btnm);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_mbox);
    }
    
    return new_mbox;
}

/**
 * Set  button to the message box
 * @param mbox pointer to message box object
 * @param btn_map button descriptor (button matrix map).
 *                E.g.  a const char *txt[] = {"ok", "close", ""} (Can not be local variable)
 * @param action a function which will be called when a button is released
 */
void lv_mbox_set_btns(lv_obj_t * mbox, const char **btn_map, lv_btnm_action_t action)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    /*Create a button matrix if not exists yet*/
    if(ext->btnm == NULL) {
        ext->btnm = lv_btnm_create(mbox, NULL);
        lv_obj_set_height(ext->btnm, LV_DPI / 2);
        lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BG, &lv_style_transp_fit);
    }

    lv_btnm_set_map(ext->btnm, btn_map);
    lv_btnm_set_action(ext->btnm, action);

    mbox_realign(mbox);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of the message box
 * @param mbox pointer to a message box
 * @param txt a '\0' terminated character string which will be the message box text
 */
void lv_mbox_set_text(lv_obj_t * mbox, const char * txt)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    lv_label_set_text(ext->text, txt);

    mbox_realign(mbox);
}


/**
 * Stop the action to call when button is released
 * @param mbox pointer to a message box object
 * @param pointer to an 'lv_btnm_action_t' action
 */
void lv_mbox_set_action(lv_obj_t * mbox, lv_btnm_action_t action)
{
    lv_mbox_ext_t *ext = lv_obj_get_ext_attr(mbox);
    lv_btnm_set_action(ext->btnm, action);
}


/**
 * Set animation duration
 * @param mbox pointer to a message box object
 * @param time animation length in  milliseconds (0: no animation)
 */
void lv_mbox_set_anim_time(lv_obj_t * mbox, uint16_t time)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    ext->anim_time = time;
}

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param delay a time (in milliseconds) to wait before delete the message box
 */
void lv_mbox_start_auto_close(lv_obj_t * mbox, uint16_t delay)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    if(ext->anim_time != 0) {
        /*Add shrinking animations*/
        lv_obj_animate(mbox, LV_ANIM_GROW_H| ANIM_OUT, ext->anim_time, delay, NULL);
        lv_obj_animate(mbox, LV_ANIM_GROW_V| ANIM_OUT, ext->anim_time, delay, (anim_cb_t)lv_obj_del);

        /*Disable fit to let shrinking work*/
        lv_cont_set_fit(mbox, false, false);
    } else {
        lv_obj_animate(mbox, LV_ANIM_NONE, ext->anim_time, delay, (anim_cb_t)lv_obj_del);
    }
}

/**
 * Stop the auto. closing of message box
 * @param mbox pointer to a message box object
 */
void lv_mbox_stop_auto_close(lv_obj_t * mbox)
{
    anim_del(mbox, NULL);
}

/**
 * Set a style of a message box
 * @param mbox pointer to a message box object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_mbox_set_style(lv_obj_t *mbox, lv_mbox_style_t type, lv_style_t *style)
{
    lv_mbox_ext_t *ext = lv_obj_get_ext_attr(mbox);

    switch (type) {
        case LV_MBOX_STYLE_BG:
            lv_obj_set_style(mbox, style);
            break;
        case LV_MBOX_STYLE_BTN_BG:
            lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BG, style);
            break;
        case LV_MBOX_STYLE_BTN_REL:
            lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BTN_REL, style);
            break;
        case LV_MBOX_STYLE_BTN_PR:
            lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BTN_PR, style);
            break;
        case LV_MBOX_STYLE_BTN_TGL_REL:
            lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BTN_TGL_REL, style);
            break;
        case LV_MBOX_STYLE_BTN_TGL_PR:
            lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BTN_TGL_PR, style);
            break;
        case LV_MBOX_STYLE_BTN_INA:
            lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BTN_INA, style);
            break;
    }

}


/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the text of the message box
 */
const char * lv_mbox_get_text(lv_obj_t * mbox)
{
	lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

	return lv_label_get_text(ext->text);
}

/**
 * Get the message box object from one of its button.
 * It is useful in the button release actions where only the button is known
 * @param btn pointer to a button of a message box
 * @return pointer to the button's message box
 */
lv_obj_t * lv_mbox_get_from_btn(lv_obj_t * btn)
{
	lv_obj_t * mbox = lv_obj_get_parent(btn);

	return mbox;
}

/**
 * Get the animation duration (close animation time)
 * @param mbox pointer to a message box object
 * @return animation length in  milliseconds (0: no animation)
 */
uint16_t lv_mbox_get_anim_time(lv_obj_t * mbox )
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    return ext->anim_time;
}

/**
 * Get a style of a message box
 * @param mbox pointer to a message box object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_mbox_get_style(lv_obj_t *mbox, lv_mbox_style_t type)
{
    lv_mbox_ext_t *ext = lv_obj_get_ext_attr(mbox);

    switch (type) {
        case LV_MBOX_STYLE_BG:          return lv_obj_get_style(mbox);
        case LV_MBOX_STYLE_BTN_BG:      return lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BG);
        case LV_MBOX_STYLE_BTN_REL:     return lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BTN_REL);
        case LV_MBOX_STYLE_BTN_PR:      return lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BTN_PR);
        case LV_MBOX_STYLE_BTN_TGL_REL: return lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BTN_TGL_REL);
        case LV_MBOX_STYLE_BTN_TGL_PR:  return lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BTN_TGL_PR);
        case LV_MBOX_STYLE_BTN_INA:     return lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BTN_INA);
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the message box
 * @param mbox pointer to a message box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_mbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(mbox, sign, param);
    if(res != LV_RES_OK) return res;

    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    if(sign == LV_SIGNAL_CORD_CHG) {
        if(lv_obj_get_width(mbox) != area_get_width(param)) {
            mbox_realign(mbox);

        }
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        mbox_realign(mbox);

    }
    else if(sign == LV_SIGNAL_FOCUS || sign == LV_SIGNAL_DEFOCUS || sign == LV_SIGNAL_CONTROLL) {
        if(ext->btnm) {
            ext->btnm->signal_func(ext->btnm, sign, param);
        }
    }

    return res;
}

/**
 * Resize the button holder to fit
 * @param mbox pointer to message box object
 */
static void mbox_realign(lv_obj_t *mbox)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    lv_style_t *style = lv_mbox_get_style(mbox, LV_MBOX_STYLE_BG);
    cord_t w = lv_obj_get_width(mbox) - 2 * style->body.padding.hor;
    if(ext->btnm) lv_obj_set_width(ext->btnm, w);
    if(ext->text) lv_obj_set_width(ext->text, w);
}

#endif
