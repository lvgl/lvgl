/**
 * @file lv_mbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_mbox.h"
#if LV_USE_MBOX != 0

#include "../lv_core/lv_group.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/

#if LV_USE_ANIMATION
#ifndef LV_MBOX_CLOSE_ANIM_TIME
#define LV_MBOX_CLOSE_ANIM_TIME 200 /*List close animation time)  */
#endif
#else
#undef LV_MBOX_CLOSE_ANIM_TIME
#define LV_MBOX_CLOSE_ANIM_TIME 0 /*No animations*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_mbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param);
static void mbox_realign(lv_obj_t * mbox);
#if LV_USE_ANIMATION
static void lv_mbox_close_ready_cb(lv_anim_t * a);
#endif
static void lv_mbox_default_event_cb(lv_obj_t * mbox, lv_event_t event);
static void lv_mbox_btnm_event_cb(lv_obj_t * btnm, lv_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a message box objects
 * @param par pointer to an object, it will be the parent of the new message box
 * @param copy pointer to a message box object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created message box
 */
lv_obj_t * lv_mbox_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("mesasge box create started");

    /*Create the ancestor message box*/
    lv_obj_t * new_mbox = lv_cont_create(par, copy);
    lv_mem_assert(new_mbox);
    if(new_mbox == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_mbox);

    /*Allocate the message box type specific extended data*/
    lv_mbox_ext_t * ext = lv_obj_allocate_ext_attr(new_mbox, sizeof(lv_mbox_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->text = NULL;
    ext->btnm = NULL;
#if LV_USE_ANIMATION
    ext->anim_time = LV_MBOX_CLOSE_ANIM_TIME;
#endif

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_mbox, lv_mbox_signal);

    /*Init the new message box message box*/
    if(copy == NULL) {
        ext->text = lv_label_create(new_mbox, NULL);
        lv_label_set_align(ext->text, LV_LABEL_ALIGN_CENTER);
        lv_label_set_long_mode(ext->text, LV_LABEL_LONG_BREAK);
        lv_label_set_text(ext->text, "Message");

        lv_cont_set_layout(new_mbox, LV_LAYOUT_COL_M);
        lv_cont_set_fit2(new_mbox, LV_FIT_NONE, LV_FIT_TIGHT);
        lv_obj_set_width(new_mbox, LV_DPI * 2);
        lv_obj_align(new_mbox, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_event_cb(new_mbox, lv_mbox_default_event_cb);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_mbox_set_style(new_mbox, LV_MBOX_STYLE_BG, th->style.mbox.bg);
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

    LV_LOG_INFO("mesasge box created");

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
 */
void lv_mbox_add_btns(lv_obj_t * mbox, const char ** btn_map)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    /*Create a button matrix if not exists yet*/
    if(ext->btnm == NULL) {
        ext->btnm = lv_btnm_create(mbox, NULL);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_BG, th->style.mbox.btn.bg);
            lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_REL, th->style.mbox.btn.rel);
            lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_PR, th->style.mbox.btn.pr);
        } else {
            lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BG, &lv_style_transp_fit);
        }
    }

    lv_btnm_set_map(ext->btnm, btn_map);
    lv_btnm_set_btn_ctrl_all(ext->btnm, LV_BTNM_CTRL_CLICK_TRIG | LV_BTNM_CTRL_NO_REPEAT);
    lv_obj_set_event_cb(ext->btnm, lv_mbox_btnm_event_cb);

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
 * Set animation duration
 * @param mbox pointer to a message box object
 * @param anim_time animation length in  milliseconds (0: no animation)
 */
void lv_mbox_set_anim_time(lv_obj_t * mbox, uint16_t anim_time)
{
#if LV_USE_ANIMATION
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    anim_time           = 0;
    ext->anim_time      = anim_time;
#else
    (void)mbox;
    (void)anim_time;
#endif
}

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param delay a time (in milliseconds) to wait before delete the message box
 */
void lv_mbox_start_auto_close(lv_obj_t * mbox, uint16_t delay)
{
#if LV_USE_ANIMATION
    if(lv_mbox_get_anim_time(mbox) != 0) {
        /*Add shrinking animations*/
        lv_anim_t a;
        a.var            = mbox;
        a.start          = lv_obj_get_height(mbox);
        a.end            = 0;
        a.exec_cb        = (lv_anim_exec_xcb_t)lv_obj_set_height;
        a.path_cb        = lv_anim_path_linear;
        a.ready_cb       = NULL;
        a.act_time       = -delay;
        a.time           = lv_mbox_get_anim_time(mbox);
        a.playback       = 0;
        a.playback_pause = 0;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        lv_anim_create(&a);

        a.start    = lv_obj_get_width(mbox);
        a.exec_cb  = (lv_anim_exec_xcb_t)lv_obj_set_width;
        a.ready_cb = lv_mbox_close_ready_cb;
        lv_anim_create(&a);

        /*Disable fit to let shrinking work*/
        lv_cont_set_fit(mbox, LV_FIT_NONE);
    } else {
        /*Create an animation to delete the mbox `delay` ms later*/
        lv_anim_t a;
        a.var            = mbox;
        a.start          = 0;
        a.end            = 1;
        a.exec_cb        = (lv_anim_exec_xcb_t)NULL;
        a.path_cb        = lv_anim_path_linear;
        a.ready_cb       = lv_mbox_close_ready_cb;
        a.act_time       = -delay;
        a.time           = 0;
        a.playback       = 0;
        a.playback_pause = 0;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        lv_anim_create(&a);
    }
#else
    (void)delay; /*Unused*/
    lv_obj_del(mbox);
#endif
}

/**
 * Stop the auto. closing of message box
 * @param mbox pointer to a message box object
 */
void lv_mbox_stop_auto_close(lv_obj_t * mbox)
{
#if LV_USE_ANIMATION
    lv_anim_del(mbox, NULL);
#else
    (void)mbox; /*Unused*/
#endif
}

/**
 * Set a style of a message box
 * @param mbox pointer to a message box object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_mbox_set_style(lv_obj_t * mbox, lv_mbox_style_t type, const lv_style_t * style)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    switch(type) {
        case LV_MBOX_STYLE_BG: lv_obj_set_style(mbox, style); break;
        case LV_MBOX_STYLE_BTN_BG: lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BG, style); break;
        case LV_MBOX_STYLE_BTN_REL: lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BTN_REL, style); break;
        case LV_MBOX_STYLE_BTN_PR: lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BTN_PR, style); break;
        case LV_MBOX_STYLE_BTN_TGL_REL: lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BTN_TGL_REL, style); break;
        case LV_MBOX_STYLE_BTN_TGL_PR: lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BTN_TGL_PR, style); break;
        case LV_MBOX_STYLE_BTN_INA: lv_btnm_set_style(ext->btnm, LV_BTNM_STYLE_BTN_INA, style); break;
    }

    mbox_realign(mbox);
}

/**
 * Set whether recoloring is enabled
 * @param btnm pointer to button matrix object
 * @param en whether recoloring is enabled
 */
void lv_mbox_set_recolor(lv_obj_t * mbox, bool en)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    if(ext->btnm) lv_btnm_set_recolor(ext->btnm, en);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the text of the message box
 */
const char * lv_mbox_get_text(const lv_obj_t * mbox)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    return lv_label_get_text(ext->text);
}

/**
 * Get the index of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`.
 * @param btnm pointer to button matrix object
 * @return  index of the last released button (LV_BTNM_BTN_NONE: if unset)
 */
uint16_t lv_mbox_get_active_btn(lv_obj_t * mbox)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    if(ext->btnm)
        return lv_btnm_get_active_btn(ext->btnm);
    else
        return LV_BTNM_BTN_NONE;
}

/**
 * Get the text of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`.
 * @param btnm pointer to button matrix object
 * @return text of the last released button (NULL: if unset)
 */
const char * lv_mbox_get_active_btn_text(lv_obj_t * mbox)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    if(ext->btnm)
        return lv_btnm_get_active_btn_text(ext->btnm);
    else
        return NULL;
}

/**
 * Get the animation duration (close animation time)
 * @param mbox pointer to a message box object
 * @return animation length in  milliseconds (0: no animation)
 */
uint16_t lv_mbox_get_anim_time(const lv_obj_t * mbox)
{
#if LV_USE_ANIMATION
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    return ext->anim_time;
#else
    (void)mbox;
    return 0;
#endif
}

/**
 * Get a style of a message box
 * @param mbox pointer to a message box object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_mbox_get_style(const lv_obj_t * mbox, lv_mbox_style_t type)
{
    const lv_style_t * style = NULL;
    lv_mbox_ext_t * ext      = lv_obj_get_ext_attr(mbox);

    switch(type) {
        case LV_MBOX_STYLE_BG: style = lv_obj_get_style(mbox); break;
        case LV_MBOX_STYLE_BTN_BG: style = lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BG); break;
        case LV_MBOX_STYLE_BTN_REL: style = lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BTN_REL); break;
        case LV_MBOX_STYLE_BTN_PR: style = lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BTN_PR); break;
        case LV_MBOX_STYLE_BTN_TGL_REL: style = lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BTN_TGL_REL); break;
        case LV_MBOX_STYLE_BTN_TGL_PR: style = lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BTN_TGL_PR); break;
        case LV_MBOX_STYLE_BTN_INA: style = lv_btnm_get_style(ext->btnm, LV_BTNM_STYLE_BTN_INA); break;
        default: style = NULL; break;
    }

    return style;
}

/**
 * Get whether recoloring is enabled
 * @param mbox pointer to a message box object
 * @return whether recoloring is enabled
 */
bool lv_mbox_get_recolor(const lv_obj_t * mbox)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    if(!ext->btnm) return false;

    return lv_btnm_get_recolor(ext->btnm);
}

/**
 * Get message box button matrix
 * @param mbox pointer to a message box object
 * @return pointer to button matrix object
 * @remarks return value will be NULL unless `lv_mbox_add_btns` has been already called
 */
lv_obj_t * lv_mbox_get_btnm(lv_obj_t * mbox)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    return ext->btnm;
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

    /*Translate LV_KEY_UP/DOWN to LV_KEY_LEFT/RIGHT */
    char c_trans = 0;
    if(sign == LV_SIGNAL_CONTROL) {
        c_trans = *((char *)param);
        if(c_trans == LV_KEY_DOWN) c_trans = LV_KEY_LEFT;
        if(c_trans == LV_KEY_UP) c_trans = LV_KEY_RIGHT;

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
    } else if(sign == LV_SIGNAL_STYLE_CHG) {
        mbox_realign(mbox);
    } else if(sign == LV_SIGNAL_RELEASED) {
        if(ext->btnm) {
            uint32_t btn_id = lv_btnm_get_active_btn(ext->btnm);
            if(btn_id != LV_BTNM_BTN_NONE) lv_event_send(mbox, LV_EVENT_VALUE_CHANGED, &btn_id);
        }
    } else if(sign == LV_SIGNAL_FOCUS || sign == LV_SIGNAL_DEFOCUS || sign == LV_SIGNAL_CONTROL ||
              sign == LV_SIGNAL_GET_EDITABLE) {
        if(ext->btnm) {
            ext->btnm->signal_cb(ext->btnm, sign, param);
        }

        /* The button matrix with ENCODER input supposes it's in a group but in this case it isn't
         * (Only the message box's container) So so some actions here instead*/
        if(sign == LV_SIGNAL_FOCUS) {
#if LV_USE_GROUP
            lv_indev_t * indev         = lv_indev_get_act();
            lv_indev_type_t indev_type = lv_indev_get_type(indev);
            if(indev_type == LV_INDEV_TYPE_ENCODER) {
                /*In navigation mode don't select any button but in edit mode select the fist*/
                lv_btnm_ext_t * btnm_ext = lv_obj_get_ext_attr(ext->btnm);
                if(lv_group_get_editing(lv_obj_get_group(mbox)))
                    btnm_ext->btn_id_pr = 0;
                else
                    btnm_ext->btn_id_pr = LV_BTNM_BTN_NONE;
            }
#endif
        }
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
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
static void mbox_realign(lv_obj_t * mbox)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    const lv_style_t * style = lv_mbox_get_style(mbox, LV_MBOX_STYLE_BG);
    lv_coord_t w             = lv_obj_get_width(mbox) - style->body.padding.left - style->body.padding.right;

    if(ext->text) {
        lv_obj_set_width(ext->text, w);
    }

    if(ext->btnm) {
        const lv_style_t * btn_bg_style  = lv_mbox_get_style(mbox, LV_MBOX_STYLE_BTN_BG);
        const lv_style_t * btn_rel_style = lv_mbox_get_style(mbox, LV_MBOX_STYLE_BTN_REL);
        lv_coord_t font_h                = lv_font_get_line_height(btn_rel_style->text.font);
        lv_obj_set_size(ext->btnm, w,
                        font_h + btn_rel_style->body.padding.top + btn_rel_style->body.padding.bottom +
                            btn_bg_style->body.padding.top + btn_bg_style->body.padding.bottom);
    }
}

#if LV_USE_ANIMATION
static void lv_mbox_close_ready_cb(lv_anim_t * a)
{
    lv_obj_del(a->var);
}
#endif

static void lv_mbox_default_event_cb(lv_obj_t * mbox, lv_event_t event)
{
    if(event != LV_EVENT_VALUE_CHANGED) return;

    uint32_t btn_id = lv_mbox_get_active_btn(mbox);
    if(btn_id == LV_BTNM_BTN_NONE) return;

    lv_mbox_start_auto_close(mbox, 0);
}

static void lv_mbox_btnm_event_cb(lv_obj_t * btnm, lv_event_t event)
{
    lv_obj_t * mbox = lv_obj_get_parent(btnm);

    /*clang-format off*/
    if(event == LV_EVENT_PRESSED || event == LV_EVENT_PRESSING || event == LV_EVENT_PRESS_LOST ||
       event == LV_EVENT_RELEASED || event == LV_EVENT_SHORT_CLICKED || event == LV_EVENT_CLICKED ||
       event == LV_EVENT_LONG_PRESSED || event == LV_EVENT_LONG_PRESSED_REPEAT ||
       event == LV_EVENT_VALUE_CHANGED) {
        lv_event_send(mbox, event, lv_event_get_data());
    }
    /*clang-format on*/
}

#endif
