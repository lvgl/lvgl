/**
 * @file lv_msgbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_msgbox.h"
#if LV_USE_MSGBOX != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_core/lv_group.h"
#include "../lv_core/lv_disp.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_msgbox"

#if LV_USE_ANIMATION
    #ifndef LV_MSGBOX_CLOSE_ANIM_TIME
        #define LV_MSGBOX_CLOSE_ANIM_TIME 200 /*List close animation time)  */
    #endif
#else
    #undef LV_MSGBOX_CLOSE_ANIM_TIME
    #define LV_MSGBOX_CLOSE_ANIM_TIME 0 /*No animations*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_msgbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param);
static void mbox_realign(lv_obj_t * mbox);
static lv_style_list_t * lv_msgbox_get_style(lv_obj_t * mbox, uint8_t part);
#if LV_USE_ANIMATION
    static void lv_msgbox_close_ready_cb(lv_anim_t * a);
#endif
static void lv_msgbox_default_event_cb(lv_obj_t * mbox, lv_event_t event);
static void lv_msgbox_btnm_event_cb(lv_obj_t * btnm, lv_event_t event);

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
lv_obj_t * lv_msgbox_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("message box create started");

    /*Create the ancestor message box*/
    lv_obj_t * mbox = lv_cont_create(par, copy);
    LV_ASSERT_MEM(mbox);
    if(mbox == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(mbox);

    /*Allocate the message box type specific extended data*/
    lv_msgbox_ext_t * ext = lv_obj_allocate_ext_attr(mbox, sizeof(lv_msgbox_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(mbox);
        return NULL;
    }

    ext->text = NULL;
    ext->btnm = NULL;
#if LV_USE_ANIMATION
    ext->anim_time = LV_MSGBOX_CLOSE_ANIM_TIME;
#endif

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(mbox, lv_msgbox_signal);

    /*Init the new message box message box*/
    if(copy == NULL) {
        ext->text = lv_label_create(mbox, NULL);
        lv_label_set_align(ext->text, LV_LABEL_ALIGN_CENTER);
        lv_label_set_long_mode(ext->text, LV_LABEL_LONG_BREAK);
        lv_label_set_text(ext->text, "Message");

        lv_cont_set_layout(mbox, LV_LAYOUT_COLUMN_MID);
        lv_cont_set_fit2(mbox, LV_FIT_NONE, LV_FIT_TIGHT);
        lv_coord_t fit_w = lv_obj_get_width_fit(par);
        if(lv_disp_get_size_category(NULL) <= LV_DISP_SIZE_SMALL) {
            lv_obj_set_width(mbox, fit_w);
        }
        else {
            lv_obj_set_width(mbox, LV_MATH_MIN(fit_w, LV_DPX(400)));
        }
        lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_event_cb(mbox, lv_msgbox_default_event_cb);

        /*Set the default styles*/
        lv_theme_apply(mbox, LV_THEME_MSGBOX);

    }
    /*Copy an existing message box*/
    else {
        lv_msgbox_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        ext->text = lv_label_create(mbox, copy_ext->text);

        /*Copy the buttons and the label on them*/
        if(copy_ext->btnm) ext->btnm = lv_btnmatrix_create(mbox, copy_ext->btnm);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(mbox, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    LV_LOG_INFO("message box created");

    return mbox;
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
void lv_msgbox_add_btns(lv_obj_t * mbox, const char * btn_map[])
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);
    LV_ASSERT_NULL(btn_map);

    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    /*Create a button matrix if not exists yet*/
    if(ext->btnm == NULL) {
        ext->btnm = lv_btnmatrix_create(mbox, NULL);

        lv_theme_apply(mbox, LV_THEME_MSGBOX_BTNS);
    }

    lv_btnmatrix_set_map(ext->btnm, btn_map);
    lv_btnmatrix_set_btn_ctrl_all(ext->btnm, LV_BTNMATRIX_CTRL_CLICK_TRIG | LV_BTNMATRIX_CTRL_NO_REPEAT);
    lv_obj_set_event_cb(ext->btnm, lv_msgbox_btnm_event_cb);

    if(lv_obj_is_focused(mbox)) {
        lv_state_t state = lv_obj_get_state(mbox, LV_MSGBOX_PART_BG);
        if(state & LV_STATE_EDITED) {
            lv_obj_set_state(ext->btnm, LV_STATE_FOCUSED | LV_STATE_EDITED);
        }
        else {
            lv_obj_set_state(ext->btnm, LV_STATE_FOCUSED);
        }

        lv_btnmatrix_set_focused_btn(ext->btnm, 0);
    }

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
void lv_msgbox_set_text(lv_obj_t * mbox, const char * txt)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);
    LV_ASSERT_STR(txt);

    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    lv_label_set_text(ext->text, txt);

    mbox_realign(mbox);
}

/**
 * Set a formatted text for the message box
 * @param mbox pointer to a message box
 * @param fmt `printf`-like format
 */
void lv_msgbox_set_text_fmt(lv_obj_t * mbox, const char * fmt, ...)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);
    LV_ASSERT_STR(fmt);

    lv_msgbox_ext_t * msgbox_ext = lv_obj_get_ext_attr(mbox);
    lv_label_ext_t * label_ext = lv_obj_get_ext_attr(msgbox_ext->text);

    /*If text is NULL then refresh */
    if(fmt == NULL) {
        lv_label_refr_text(msgbox_ext->text);
        return;
    }

    if(label_ext->text != NULL) {
        lv_mem_free(label_ext->text);
        label_ext->text = NULL;
    }

    va_list args;
    va_start(args, fmt);
    label_ext->text = _lv_txt_set_text_vfmt(fmt, args);
    va_end(args);
    lv_label_refr_text(msgbox_ext->text);
    mbox_realign(mbox);
}

/**
 * Set animation duration
 * @param mbox pointer to a message box object
 * @param anim_time animation length in  milliseconds (0: no animation)
 */
void lv_msgbox_set_anim_time(lv_obj_t * mbox, uint16_t anim_time)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
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
void lv_msgbox_start_auto_close(lv_obj_t * mbox, uint16_t delay)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    if(lv_msgbox_get_anim_time(mbox) != 0) {
        /*Add shrinking animations*/
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, mbox);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_height);
        lv_anim_set_values(&a, lv_obj_get_height(mbox), 0);
        lv_anim_set_time(&a, lv_msgbox_get_anim_time(mbox));
        lv_anim_set_delay(&a, delay);
        lv_anim_start(&a);

        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_width);
        lv_anim_set_values(&a, lv_obj_get_width(mbox), 0);
        lv_anim_set_ready_cb(&a, lv_msgbox_close_ready_cb);
        lv_anim_start(&a);

        /*Disable fit to let shrinking work*/
        lv_cont_set_fit(mbox, LV_FIT_NONE);
    }
    else {
        /*Create an animation to delete the mbox `delay` ms later*/
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, mbox);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)NULL);
        lv_anim_set_values(&a, 0, 1);
        lv_anim_set_ready_cb(&a, lv_msgbox_close_ready_cb);
        lv_anim_set_time(&a, lv_msgbox_get_anim_time(mbox));
        lv_anim_set_delay(&a, delay);
        lv_anim_start(&a);
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
void lv_msgbox_stop_auto_close(lv_obj_t * mbox)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_anim_del(mbox, NULL);
#else
    (void)mbox; /*Unused*/
#endif
}

/**
 * Set whether recoloring is enabled
 * @param mbox pointer to message box object
 * @param en whether recoloring is enabled
 */
void lv_msgbox_set_recolor(lv_obj_t * mbox, bool en)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);

    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    if(ext->btnm) lv_btnmatrix_set_recolor(ext->btnm, en);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the text of the message box
 */
const char * lv_msgbox_get_text(const lv_obj_t * mbox)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);

    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    return lv_label_get_text(ext->text);
}

/**
 * Get the index of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`.
 * @param mbox pointer to message box object
 * @return  index of the last released button (LV_BTNMATRIX_BTN_NONE: if unset)
 */
uint16_t lv_msgbox_get_active_btn(lv_obj_t * mbox)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);

    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    if(ext->btnm == NULL) return LV_BTNMATRIX_BTN_NONE;

    uint16_t id = lv_btnmatrix_get_active_btn(ext->btnm);
    if(id == LV_BTNMATRIX_BTN_NONE) {
        id = lv_btnmatrix_get_focused_btn(ext->btnm);
    }

    return id;
}

/**
 * Get the text of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`.
 * @param mbox pointer to message box object
 * @return text of the last released button (NULL: if unset)
 */
const char * lv_msgbox_get_active_btn_text(lv_obj_t * mbox)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);

    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    if(ext->btnm)
        return lv_btnmatrix_get_active_btn_text(ext->btnm);
    else
        return NULL;
}

/**
 * Get the animation duration (close animation time)
 * @param mbox pointer to a message box object
 * @return animation length in  milliseconds (0: no animation)
 */
uint16_t lv_msgbox_get_anim_time(const lv_obj_t * mbox)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    return ext->anim_time;
#else
    (void)mbox;
    return 0;
#endif
}

/**
 * Get whether recoloring is enabled
 * @param mbox pointer to a message box object
 * @return whether recoloring is enabled
 */
bool lv_msgbox_get_recolor(const lv_obj_t * mbox)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);

    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    if(!ext->btnm) return false;

    return lv_btnmatrix_get_recolor(ext->btnm);
}

/**
 * Get message box button matrix
 * @param mbox pointer to a message box object
 * @return pointer to button matrix object
 * @remarks return value will be NULL unless `lv_msgbox_add_btns` has been already called
 */
lv_obj_t * lv_msgbox_get_btnmatrix(lv_obj_t * mbox)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);

    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
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
static lv_res_t lv_msgbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param)
{
    lv_res_t res;

#if LV_USE_GROUP
    /*Translate LV_KEY_UP/DOWN to LV_KEY_LEFT/RIGHT */
    char c_trans = 0;
    if(sign == LV_SIGNAL_CONTROL) {
        c_trans = *((char *)param);
        if(c_trans == LV_KEY_DOWN) c_trans = LV_KEY_LEFT;
        if(c_trans == LV_KEY_UP) c_trans = LV_KEY_RIGHT;

        param = &c_trans;
    }
#endif

    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_msgbox_get_style(mbox, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(mbox, sign, param);
    }
    else if(sign == LV_SIGNAL_GET_STATE_DSC) {
        lv_get_state_info_t * info = param;
        lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
        if(ext->btnm && info->part == LV_MSGBOX_PART_BTN_BG) {
            info->result = lv_obj_get_state(ext->btnm, LV_BTNMATRIX_PART_BG);
        }
        else if(ext->btnm && info->part == LV_MSGBOX_PART_BTN) {
            info->result = lv_obj_get_state(ext->btnm, LV_BTNMATRIX_PART_BTN);
        }
        return LV_RES_OK;
    }

    /* Include the ancient signal function */
    res = ancestor_signal(mbox, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    if(sign == LV_SIGNAL_COORD_CHG) {
        if(lv_obj_get_width(mbox) != lv_area_get_width(param)) {
            mbox_realign(mbox);
        }
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        mbox_realign(mbox);
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        if(ext->btnm) {
            uint32_t btn_id = lv_btnmatrix_get_focused_btn(ext->btnm);
            if(btn_id != LV_BTNMATRIX_BTN_NONE) lv_event_send(mbox, LV_EVENT_VALUE_CHANGED, &btn_id);
        }
    }
    else if(
#if LV_USE_GROUP
        sign == LV_SIGNAL_CONTROL || sign == LV_SIGNAL_GET_EDITABLE ||
#endif
        sign == LV_SIGNAL_FOCUS || sign == LV_SIGNAL_DEFOCUS) {
        if(ext->btnm) {
            ext->btnm->signal_cb(ext->btnm, sign, param);
        }

        /* The button matrix with ENCODER input supposes it's in a group but in this case it isn't
         * (Only the message box's container) So so some actions here instead*/
#if LV_USE_GROUP
        if(sign == LV_SIGNAL_FOCUS) {
            lv_indev_t * indev         = lv_indev_get_act();
            lv_indev_type_t indev_type = lv_indev_get_type(indev);
            if(indev_type == LV_INDEV_TYPE_ENCODER) {
                /*In navigation mode don't select any button but in edit mode select the fist*/
                if(lv_group_get_editing(lv_obj_get_group(mbox))) lv_btnmatrix_set_focused_btn(ext->btnm, 0);
                else lv_btnmatrix_set_focused_btn(ext->btnm, LV_BTNMATRIX_BTN_NONE);
            }
        }

        if(ext->btnm && (sign == LV_SIGNAL_FOCUS || sign == LV_SIGNAL_DEFOCUS)) {
            lv_state_t state = lv_obj_get_state(mbox, LV_MSGBOX_PART_BG);
            if(state & LV_STATE_FOCUSED) {
                lv_obj_set_state(ext->btnm, LV_STATE_FOCUSED);
            }
            else {
                lv_obj_clear_state(ext->btnm, LV_STATE_FOCUSED);

            }
            if(state & LV_STATE_EDITED) {
                lv_obj_set_state(ext->btnm, LV_STATE_EDITED);
            }
            else {
                lv_obj_clear_state(ext->btnm, LV_STATE_EDITED);

            }
        }
#endif
    }

    return res;
}

/**
 * Get the style descriptor of a part of the object
 * @param mbox pointer the object
 * @param part the part from `lv_msgbox_part_t`. (LV_MSGBOX_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_list_t * lv_msgbox_get_style(lv_obj_t * mbox, uint8_t part)
{
    LV_ASSERT_OBJ(mbox, LV_OBJX_NAME);

    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_MSGBOX_PART_BG:
            style_dsc_p = &mbox->style_list;
            break;
        case LV_MSGBOX_PART_BTN_BG:
            style_dsc_p = ext->btnm ? lv_obj_get_style_list(ext->btnm, LV_BTNMATRIX_PART_BG) : NULL;
            break;
        case LV_MSGBOX_PART_BTN:
            style_dsc_p = ext->btnm ? lv_obj_get_style_list(ext->btnm, LV_BTNMATRIX_PART_BTN) : NULL;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}

/**
 * Resize the button holder to fit
 * @param mbox pointer to message box object
 */
static void mbox_realign(lv_obj_t * mbox)
{
    lv_msgbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    lv_coord_t w = lv_obj_get_width_fit(mbox);

    if(ext->text) {
        lv_obj_set_width(ext->text, w);
    }

    if(ext->btnm) {
        lv_style_int_t bg_top = lv_obj_get_style_pad_top(mbox, LV_MSGBOX_PART_BTN_BG);
        lv_style_int_t bg_bottom = lv_obj_get_style_pad_bottom(mbox, LV_MSGBOX_PART_BTN_BG);
        lv_style_int_t bg_inner = lv_obj_get_style_pad_inner(mbox, LV_MSGBOX_PART_BTN_BG);
        lv_style_int_t btn_top = lv_obj_get_style_pad_top(mbox, LV_MSGBOX_PART_BTN);
        lv_style_int_t btn_bottom = lv_obj_get_style_pad_bottom(mbox, LV_MSGBOX_PART_BTN);
        const lv_font_t * font = lv_obj_get_style_text_font(mbox, LV_MSGBOX_PART_BTN);

        uint16_t btnm_lines = 1;
        const char ** btnm_map = lv_btnmatrix_get_map_array(ext->btnm);
        uint16_t i;
        for(i = 0; btnm_map[i][0] != '\0'; i++) {
            if(btnm_map[i][0] == '\n') btnm_lines++;
        }

        lv_coord_t font_h = lv_font_get_line_height(font);
        lv_coord_t btn_h = font_h + btn_top + btn_bottom;
        lv_obj_set_size(ext->btnm, w, btn_h * btnm_lines + bg_inner * (btnm_lines - 1) + bg_top + bg_bottom);
    }
}

#if LV_USE_ANIMATION
static void lv_msgbox_close_ready_cb(lv_anim_t * a)
{
    lv_obj_del(a->var);
}
#endif

static void lv_msgbox_default_event_cb(lv_obj_t * mbox, lv_event_t event)
{
    if(event != LV_EVENT_VALUE_CHANGED) return;

    uint32_t btn_id = lv_msgbox_get_active_btn(mbox);
    if(btn_id == LV_BTNMATRIX_BTN_NONE) return;

    lv_msgbox_start_auto_close(mbox, 0);
}

static void lv_msgbox_btnm_event_cb(lv_obj_t * btnm, lv_event_t event)
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
