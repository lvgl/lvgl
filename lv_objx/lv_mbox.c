/**
 * @file lv_mbox.c
 * 
 */


/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_MBOX != 0

#include "lv_mbox.h"
#include "../lv_core/lv_group.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/

#if USE_LV_ANIMATION
#  ifndef LV_MBOX_CLOSE_ANIM_TIME
#    define LV_MBOX_CLOSE_ANIM_TIME  200 /*List close animation time)  */
#  endif
#else
#  undef  LV_MBOX_CLOSE_ANIM_TIME
#  define LV_MBOX_CLOSE_ANIM_TIME	0	/*No animations*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_mbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param);
static void mbox_realign(lv_obj_t *mbox);
static lv_res_t lv_mbox_close_action(lv_obj_t *btn, const char *txt);

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
    lv_mem_assert(new_mbox);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_mbox);
    
    /*Allocate the message box type specific extended data*/
    lv_mbox_ext_t * ext = lv_obj_allocate_ext_attr(new_mbox, sizeof(lv_mbox_ext_t));
    lv_mem_assert(ext);
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

        lv_cont_set_layout(new_mbox, LV_LAYOUT_COL_M);
        lv_cont_set_fit(new_mbox, false, true);
        lv_obj_set_width(new_mbox, LV_HOR_RES / 2);
        lv_obj_align(new_mbox, NULL, LV_ALIGN_CENTER, 0, 0);

        /*Set the default styles*/
         lv_theme_t *th = lv_theme_get_current();
         if(th) {
             lv_mbox_set_style(new_mbox, LV_MBOX_STYLE_BG, th->mbox.bg);
         } else {
             lv_mbox_set_style(new_mbox, LV_MBOX_STYLE_BG, &lv_style_pretty);
         }

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

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add button to the message box
 * @param mbox pointer to message box object
 * @param btn_map button descriptor (button matrix map).
 *                E.g.  a const char *txt[] = {"ok", "close", ""} (Can not be local variable)
 * @param action a function which will be called when a button is released
 */
void lv_mbox_add_btns(lv_obj_t * mbox, const char **btn_map, lv_btnm_action_t action)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    /*Create a button matrix if not exists yet*/
    if(ext->btnm == NULL) {
        ext->btnm = lv_btnm_create(mbox, NULL);

        /*Set the default styles*/
         lv_theme_t *th = lv_theme_get_current();
         if(th) {
             lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_BG, th->mbox.btn.bg);
             lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_REL, th->mbox.btn.rel);
             lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_PR, th->mbox.btn.pr);
         } else {
             lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BG, &lv_style_transp_fit);
         }
    }

    lv_btnm_set_map(ext->btnm, btn_map);
    if(action == NULL) lv_btnm_set_action(ext->btnm, lv_mbox_close_action); /*Set a default action anyway*/
    else  lv_btnm_set_action(ext->btnm, action);

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
 * @param anim_time animation length in  milliseconds (0: no animation)
 */
void lv_mbox_set_anim_time(lv_obj_t * mbox, uint16_t anim_time)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
#if USE_LV_ANIMATION == 0
    anim_time = 0;
#endif

    ext->anim_time = anim_time;
}

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param delay a time (in milliseconds) to wait before delete the message box
 */
void lv_mbox_start_auto_close(lv_obj_t * mbox, uint16_t delay)
{
#if USE_LV_ANIMATION
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    if(ext->anim_time != 0) {
        /*Add shrinking animations*/
        lv_obj_animate(mbox, LV_ANIM_GROW_H| LV_ANIM_OUT, ext->anim_time, delay, NULL);
        lv_obj_animate(mbox, LV_ANIM_GROW_V| LV_ANIM_OUT, ext->anim_time, delay, (void (*)(lv_obj_t*))lv_obj_del);

        /*Disable fit to let shrinking work*/
        lv_cont_set_fit(mbox, false, false);
    } else {
        lv_obj_animate(mbox, LV_ANIM_NONE, ext->anim_time, delay, (void (*)(lv_obj_t*))lv_obj_del);
    }
#else
    lv_obj_del(mbox);
#endif
}

/**
 * Stop the auto. closing of message box
 * @param mbox pointer to a message box object
 */
void lv_mbox_stop_auto_close(lv_obj_t * mbox)
{
#if USE_LV_ANIMATION
    lv_anim_del(mbox, NULL);
#endif
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

    /*Translate LV_GROUP_KEY_UP/DOWN to LV_GROUP_KEY_LEFT/RIGHT */
    char c_trans = 0;
    if(sign == LV_SIGNAL_CONTROLL) {
        c_trans = *((char*)param);
        if(c_trans == LV_GROUP_KEY_DOWN) c_trans = LV_GROUP_KEY_LEFT;
        if(c_trans == LV_GROUP_KEY_UP) c_trans = LV_GROUP_KEY_RIGHT;

        param = &c_trans;
    }

    /* Include the ancient signal function */
    res = ancestor_signal(mbox, sign, param);
    if(res != LV_RES_OK) return res;

    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    if(sign == LV_SIGNAL_CORD_CHG) {
        if(lv_obj_get_width(mbox) != lv_area_get_width(param)) {
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
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_mbox";
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
    lv_coord_t w = lv_obj_get_width(mbox) - 2 * style->body.padding.hor;

    if(ext->text) {
        lv_obj_set_width(ext->text, w);
    }

    if(ext->btnm) {
        lv_style_t *btn_bg_style = lv_mbox_get_style(mbox, LV_MBOX_STYLE_BTN_BG);
        lv_style_t *btn_rel_style = lv_mbox_get_style(mbox, LV_MBOX_STYLE_BTN_REL);
        lv_coord_t font_h = lv_font_get_height(btn_rel_style->text.font);
        lv_obj_set_size(ext->btnm, w, font_h + 2 * btn_rel_style->body.padding.ver + 2 * btn_bg_style->body.padding.ver);
    }
}

static lv_res_t lv_mbox_close_action(lv_obj_t *btn, const char *txt)
{
    lv_obj_t *mbox = lv_mbox_get_from_btn(btn);

    if(txt[0] != '\0') {
        lv_mbox_start_auto_close(mbox, 0);
        return LV_RES_INV;
    }

    return LV_RES_OK;
}

#endif
