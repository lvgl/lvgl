
/**
 * @file lv_kb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_KB != 0

#include "lv_kb.h"
#include "lv_ta.h"
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
static lv_res_t lv_kb_signal(lv_obj_t * kb, lv_signal_t sign, void * param);
static lv_res_t lv_app_kb_action(lv_obj_t * kb, const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;

static const char * kb_map_lc[] = {
"\2051#", "\204q", "\204w", "\204e", "\204r", "\204t", "\204y", "\204u", "\204i", "\204o", "\204p", "\207Del", "\n",
"\226ABC", "\203a", "\203s", "\203d", "\203f", "\203g", "\203h", "\203j", "\203k", "\203l", "\207Enter", "\n",
"_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
"\202"SYMBOL_CLOSE, "\202"SYMBOL_LEFT, "\206 ", "\202"SYMBOL_RIGHT, "\202"SYMBOL_OK, ""
};

static const char * kb_map_uc[] = {
"\2051#", "\204Q", "\204W", "\204E", "\204R", "\204T", "\204Y", "\204U", "\204I", "\204O", "\204P", "\207Del", "\n",
"\226abc", "\203A", "\203S", "\203D", "\203F", "\203G", "\203H", "\203J", "\203K", "\203L", "\207Enter", "\n",
"_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
"\202"SYMBOL_CLOSE, "\202"SYMBOL_LEFT, "\206 ", "\202"SYMBOL_RIGHT, "\202"SYMBOL_OK, ""
};

static const char * kb_map_spec[] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "\202Del", "\n",
"\222abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
"\\", "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
"\202"SYMBOL_CLOSE, "\202"SYMBOL_LEFT, "\206 ", "\202"SYMBOL_RIGHT, "\202"SYMBOL_OK, ""
};

static const char * kb_map_num[] = {
"1", "2", "3", "\202"SYMBOL_CLOSE,"\n",
"4", "5", "6", "\202"SYMBOL_OK, "\n",
"7", "8", "9", "\202Del", "\n",
"+/-", "0", ".", SYMBOL_LEFT, SYMBOL_RIGHT, ""
};
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a keyboard objects
 * @param par pointer to an object, it will be the parent of the new keyboard
 * @param copy pointer to a keyboard object, if not NULL then the new object will be copied from it
 * @return pointer to the created keyboard
 */
lv_obj_t * lv_kb_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of keyboard*/
    lv_obj_t * new_kb = lv_btnm_create(par, copy);
    lv_mem_assert(new_kb);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_kb);

    /*Allocate the keyboard type specific extended data*/
    lv_kb_ext_t * ext = lv_obj_allocate_ext_attr(new_kb, sizeof(lv_kb_ext_t));
    lv_mem_assert(ext);

    /*Initialize the allocated 'ext' */

    ext->ta = NULL;
    ext->mode = LV_KB_MODE_TEXT;
    ext->cursor_mng = 0;
    ext->hide_action = NULL;
    ext->ok_action = NULL;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_kb, lv_kb_signal);

    /*Init the new keyboard keyboard*/
    if(copy == NULL) {
        lv_obj_set_size(new_kb, LV_HOR_RES, LV_VER_RES / 2);
        lv_obj_align(new_kb, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
        lv_btnm_set_action(new_kb, lv_app_kb_action);
        lv_btnm_set_map(new_kb, kb_map_lc);

        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            lv_kb_set_style(new_kb, LV_KB_STYLE_BG, th->kb.bg);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_REL, th->kb.btn.rel);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_PR, th->kb.btn.pr);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_TGL_REL, th->kb.btn.tgl_rel);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_TGL_PR, th->kb.btn.tgl_pr);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_INA, th->kb.btn.ina);
        } else {
            /*Let the button matrix's styles*/
        }
    }
    /*Copy an existing keyboard*/
    else {
    	lv_kb_ext_t * copy_ext = lv_obj_get_ext_attr(copy);ext->ta = NULL;
        ext->ta = copy_ext->ta;
        ext->mode = copy_ext->mode;
        ext->cursor_mng = copy_ext->cursor_mng;
        ext->hide_action = copy_ext->hide_action;
        ext->ok_action = copy_ext->ok_action;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_kb);
    }

    return new_kb;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @param ta pointer to a Text Area object to write there
 */
void lv_kb_set_ta(lv_obj_t * kb, lv_obj_t * ta)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    lv_cursor_type_t cur_type;

    /*Hide the cursor of the old Text area if cursor management is enabled*/
    if(ext->ta && ext->cursor_mng) {
        cur_type = lv_ta_get_cursor_type(ext->ta);
        lv_ta_set_cursor_type(ext->ta,  cur_type | LV_CURSOR_HIDDEN);
    }

    ext->ta = ta;

    /*Show the cursor of the new Text area if cursor management is enabled*/
    if(ext->ta && ext->cursor_mng) {
        cur_type = lv_ta_get_cursor_type(ext->ta);
        lv_ta_set_cursor_type(ext->ta,  cur_type & (~LV_CURSOR_HIDDEN));
    }
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @param mode the mode from 'lv_kb_mode_t'
 */
void lv_kb_set_mode(lv_obj_t * kb, lv_kb_mode_t mode)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    if(ext->mode == mode) return;

    ext->mode = mode;
    if(mode == LV_KB_MODE_TEXT) lv_btnm_set_map(kb, kb_map_lc);
    else if(mode == LV_KB_MODE_NUM) lv_btnm_set_map(kb, kb_map_num);
}


/**
 * Automatically hide or show the cursor of Text Area
 * @param kb pointer to a Keyboard object
 * @param en true: show cursor on the current text area, false: hide cursor
 */
void lv_kb_set_cursor_manage(lv_obj_t * kb, bool en)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    if(ext->cursor_mng == en) return;

    ext->cursor_mng = en == false? 0 : 1;

    if(ext->ta) {
        lv_cursor_type_t cur_type;
        cur_type = lv_ta_get_cursor_type(ext->ta);

        if(ext->cursor_mng){
            lv_ta_set_cursor_type(ext->ta,  cur_type & (~LV_CURSOR_HIDDEN));
        }else{
            lv_ta_set_cursor_type(ext->ta,  cur_type | LV_CURSOR_HIDDEN);
        }
    }
}

/**
 * Set call back to call when the "Ok" button is pressed
 * @param kb pointer to Keyboard object
 * @param action a callback with 'lv_action_t' type
 */
void lv_kb_set_ok_action(lv_obj_t * kb, lv_action_t action)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    ext->ok_action = action;
}

/**
 * Set call back to call when the "Hide" button is pressed
 * @param kb pointer to Keyboard object
 * @param action a callback with 'lv_action_t' type
 */
void lv_kb_set_hide_action(lv_obj_t * kb, lv_action_t action)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    ext->hide_action = action;
}

/**
 * Set a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_kb_set_style(lv_obj_t *kb, lv_kb_style_t type, lv_style_t *style)
{
    switch (type) {
        case LV_KB_STYLE_BG:
            lv_btnm_set_style(kb, LV_BTNM_STYLE_BG, style);
            break;
        case LV_KB_STYLE_BTN_REL:
            lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_REL, style);
            break;
        case LV_KB_STYLE_BTN_PR:
            lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_PR, style);
            break;
        case LV_KB_STYLE_BTN_TGL_REL:
            lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_TGL_REL, style);
            break;
        case LV_KB_STYLE_BTN_TGL_PR:
            lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_TGL_PR, style);
            break;
        case LV_KB_STYLE_BTN_INA:
            lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_INA, style);
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @return pointer to the assigned Text Area object
 */
lv_obj_t * lv_kb_get_ta(lv_obj_t * kb)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->ta;
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'lv_kb_mode_t'
 */
lv_kb_mode_t lv_kb_get_mode(lv_obj_t * kb)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->mode;
}


/**
 * Get the current cursor manage mode.
 * @param kb pointer to a Keyboard object
 * @return true: show cursor on the current text area, false: hide cursor
 */
bool lv_kb_get_cursor_manage(lv_obj_t * kb)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->cursor_mng == 0 ? false : true;
}

/**
 * Get the callback to call when the "Ok" button is pressed
 * @param kb pointer to Keyboard object
 * @return the ok callback
 */
lv_action_t lv_kb_get_ok_action(lv_obj_t * kb)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->ok_action;
}

/**
 * Get the callback to call when the "Hide" button is pressed
 * @param kb pointer to Keyboard object
 * @return the close callback
 */
lv_action_t lv_kb_get_hide_action(lv_obj_t * kb)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->hide_action;
}

/**
 * Get a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_kb_get_style(lv_obj_t *kb, lv_kb_style_t type)
{
    switch (type) {
        case LV_KB_STYLE_BG:          return lv_btnm_get_style(kb, LV_BTNM_STYLE_BG);
        case LV_KB_STYLE_BTN_REL:     return lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_REL);
        case LV_KB_STYLE_BTN_PR:      return lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_PR);
        case LV_KB_STYLE_BTN_TGL_REL: return lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_TGL_REL);
        case LV_KB_STYLE_BTN_TGL_PR:  return lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_TGL_PR);
        case LV_KB_STYLE_BTN_INA:     return lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_INA);
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the keyboard
 * @param kb pointer to a keyboard object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_kb_signal(lv_obj_t * kb, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(kb, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_kb";
    }

    return res;
}

/**
 * Called when a button of 'kb_btnm' is released
 * @param btnm pointer to 'kb_btnm'
 * @param i the index of the released button from the current btnm map
 * @return LV_ACTION_RES_INV if the btnm is deleted else LV_ACTION_RES_OK
 */
static lv_res_t lv_app_kb_action(lv_obj_t * kb, const char * txt)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);

    /*Do the corresponding action according to the text of the button*/
    if(strcmp(txt, "abc") == 0) {
        lv_btnm_set_map(kb, kb_map_lc);
        return LV_RES_OK;
    }
    else if(strcmp(txt, "ABC") == 0) {
        lv_btnm_set_map(kb, kb_map_uc);
        return LV_RES_OK;
    }
    else if(strcmp(txt, "1#") == 0) {
        lv_btnm_set_map(kb, kb_map_spec);
        return LV_RES_OK;
    }
    else if(strcmp(txt, SYMBOL_CLOSE) == 0) {
        if(ext->hide_action) ext->hide_action(kb);
        else {
            lv_kb_set_ta(kb, NULL);         /*De-assign the text area  to hide it cursor if needed*/
            lv_obj_del(kb);
        }
        return LV_RES_INV;
    } else if(strcmp(txt, SYMBOL_OK) == 0) {
        if(ext->ok_action) ext->ok_action(kb);
        else {
            lv_kb_set_ta(kb, NULL);         /*De-assign the text area to hide it cursor if needed*/
            lv_obj_del(kb);
        }
        return LV_RES_INV;
    }

    if(ext->ta == NULL) return LV_RES_OK;

    if(strcmp(txt, "Enter") == 0)lv_ta_add_char(ext->ta, '\n');
    else if(strcmp(txt, SYMBOL_LEFT) == 0) lv_ta_cursor_left(ext->ta);
    else if(strcmp(txt, SYMBOL_RIGHT) == 0) lv_ta_cursor_right(ext->ta);
    else if(strcmp(txt, "Del") == 0)  lv_ta_del_char(ext->ta);
    else if(strcmp(txt, "+/-") == 0) {
        uint16_t cur = lv_ta_get_cursor_pos(ext->ta);
        const char * ta_txt = lv_ta_get_text(ext->ta);
        if(ta_txt[0] == '-') {
            lv_ta_set_cursor_pos(ext->ta, 1);
            lv_ta_del_char(ext->ta);
            lv_ta_add_char(ext->ta, '+');
            lv_ta_set_cursor_pos(ext->ta, cur);
        } else if(ta_txt[0] == '+') {
            lv_ta_set_cursor_pos(ext->ta, 1);
            lv_ta_del_char(ext->ta);
            lv_ta_add_char(ext->ta, '-');
            lv_ta_set_cursor_pos(ext->ta, cur);
        } else {
            lv_ta_set_cursor_pos(ext->ta, 0);
            lv_ta_add_char(ext->ta, '-');
            lv_ta_set_cursor_pos(ext->ta, cur + 1);
        }
    } else {
        lv_ta_add_text(ext->ta, txt);
    }
    return LV_RES_OK;
}

#endif
