
/**
 * @file lv_kb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_keyboard.h"
#if LV_USE_KEYBOARD != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_themes/lv_theme.h"
#include "lv_textarea.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_keyboard"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_keyboard_signal(lv_obj_t * kb, lv_signal_t sign, void * param);
static void lv_keyboard_update_map(lv_obj_t * kb);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
/* clang-format off */
static const char * const default_kb_map_lc[] = {"1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
                                                 "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                                 LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                };

static const lv_btnmatrix_ctrl_t default_kb_ctrl_lc_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static const char * const default_kb_map_uc[] = {"1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
                                                 "abc", "A", "S", "D", "F", "G", "H", "J", "K", "L", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
                                                 LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                };

static const lv_btnmatrix_ctrl_t default_kb_ctrl_uc_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static const char * const default_kb_map_spec[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", LV_SYMBOL_BACKSPACE, "\n",
                                                   "abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
                                                   "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
                                                   LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                  };

static const lv_btnmatrix_ctrl_t default_kb_ctrl_spec_map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static const char * const default_kb_map_num[] = {"1", "2", "3", LV_SYMBOL_CLOSE, "\n",
                                                  "4", "5", "6", LV_SYMBOL_OK, "\n",
                                                  "7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
                                                  "+/-", "0", ".", LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, ""
                                                 };

static const lv_btnmatrix_ctrl_t default_kb_ctrl_num_map[] = {
    1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
    1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
    1, 1, 1, 2,
    1, 1, 1, 1, 1
};
/* clang-format on */

static const char * * kb_map[4] = {
    (const char * *)default_kb_map_lc,
    (const char * *)default_kb_map_uc,
    (const char * *)default_kb_map_spec,
    (const char * *)default_kb_map_num
};
static const lv_btnmatrix_ctrl_t * kb_ctrl[4] = {
    default_kb_ctrl_lc_map,
    default_kb_ctrl_uc_map,
    default_kb_ctrl_spec_map,
    default_kb_ctrl_num_map
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
lv_obj_t * lv_keyboard_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("keyboard create started");

    /*Create the ancestor of keyboard*/
    lv_obj_t * kb = lv_btnmatrix_create(par, copy);
    LV_ASSERT_MEM(kb);
    if(kb == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(kb);

    /*Allocate the keyboard type specific extended data*/
    lv_keyboard_ext_t * ext = lv_obj_allocate_ext_attr(kb, sizeof(lv_keyboard_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(kb);
        return NULL;
    }

    /*Initialize the allocated 'ext' */
    ext->ta         = NULL;
    ext->mode       = LV_KEYBOARD_MODE_TEXT_LOWER;
    ext->cursor_mng = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(kb, lv_keyboard_signal);

    /*Init the new keyboard keyboard*/
    if(copy == NULL) {
        /* Set a size which fits into the parent.
         * Don't use `par` directly because if the window is created on a page it is moved to the
         * scrollable so the parent has changed */
        lv_obj_set_size(kb, lv_obj_get_width_fit(lv_obj_get_parent(kb)),
                        lv_obj_get_height_fit(lv_obj_get_parent(kb)) / 2);
        lv_obj_align(kb, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
        lv_obj_set_event_cb(kb, lv_keyboard_def_event_cb);
        lv_obj_set_base_dir(kb, LV_BIDI_DIR_LTR);
        lv_obj_add_protect(kb, LV_PROTECT_CLICK_FOCUS);

        lv_btnmatrix_set_map(kb, kb_map[ext->mode]);
        lv_btnmatrix_set_ctrl_map(kb, kb_ctrl[ext->mode]);

        lv_theme_apply(kb, LV_THEME_KEYBOARD);
    }
    /*Copy an existing keyboard*/
    else {
        lv_keyboard_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->ta                = copy_ext->ta;
        ext->mode              = copy_ext->mode;
        ext->cursor_mng        = copy_ext->cursor_mng;

        lv_btnmatrix_set_map(kb, kb_map[ext->mode]);
        lv_btnmatrix_set_ctrl_map(kb, kb_ctrl[ext->mode]);

        /*Refresh the style with new signal function*/
        //        lv_obj_refresh_style(new_kb);
    }

    LV_LOG_INFO("keyboard created");

    return kb;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @param ta pointer to a Text Area object to write there
 */
void lv_keyboard_set_textarea(lv_obj_t * kb, lv_obj_t * ta)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);
    if(ta) {
        LV_ASSERT_OBJ(ta, "lv_textarea");
    }

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);

    /*Hide the cursor of the old Text area if cursor management is enabled*/
    if(ext->ta && ext->cursor_mng) {
        lv_textarea_set_cursor_hidden(ext->ta, true);
    }

    ext->ta = ta;

    /*Show the cursor of the new Text area if cursor management is enabled*/
    if(ext->ta && ext->cursor_mng) {
        lv_textarea_set_cursor_hidden(ext->ta, false);
    }
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @param mode the mode from 'lv_keyboard_mode_t'
 */
void lv_keyboard_set_mode(lv_obj_t * kb, lv_keyboard_mode_t mode)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    if(ext->mode == mode) return;

    ext->mode = mode;
    lv_btnmatrix_set_map(kb, kb_map[mode]);
    lv_btnmatrix_set_ctrl_map(kb, kb_ctrl[mode]);
}

/**
 * Automatically hide or show the cursor of Text Area
 * @param kb pointer to a Keyboard object
 * @param en true: show cursor on the current text area, false: hide cursor
 */
void lv_keyboard_set_cursor_manage(lv_obj_t * kb, bool en)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    if(ext->cursor_mng == en) return;

    ext->cursor_mng = en == false ? 0 : 1;

    if(ext->ta) {

        if(ext->cursor_mng) {
            lv_textarea_set_cursor_hidden(ext->ta, false);
        }
        else {
            lv_textarea_set_cursor_hidden(ext->ta, true);
        }
    }
}

/**
 * Set a new map for the keyboard
 * @param kb pointer to a Keyboard object
 * @param mode keyboard map to alter 'lv_keyboard_mode_t'
 * @param map pointer to a string array to describe the map.
 *            See 'lv_btnmatrix_set_map()' for more info.
 */
void lv_keyboard_set_map(lv_obj_t * kb, lv_keyboard_mode_t mode, const char * map[])
{
    kb_map[mode] = map;
    lv_keyboard_update_map(kb);
}

/**
 * Set the button control map (hidden, disabled etc.) for the keyboard. The
 * control map array will be copied and so may be deallocated after this
 * function returns.
 * @param kb pointer to a keyboard object
 * @param mode keyboard ctrl map to alter 'lv_keyboard_mode_t'
 * @param ctrl_map pointer to an array of `lv_btn_ctrl_t` control bytes.
 *                 See: `lv_btnmatrix_set_ctrl_map` for more details.
 */
void lv_keyboard_set_ctrl_map(lv_obj_t * kb, lv_keyboard_mode_t mode, const lv_btnmatrix_ctrl_t ctrl_map[])
{
    kb_ctrl[mode] = ctrl_map;
    lv_keyboard_update_map(kb);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @return pointer to the assigned Text Area object
 */
lv_obj_t * lv_keyboard_get_textarea(const lv_obj_t * kb)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->ta;
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'lv_keyboard_mode_t'
 */
lv_keyboard_mode_t lv_keyboard_get_mode(const lv_obj_t * kb)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->mode;
}

/**
 * Get the current cursor manage mode.
 * @param kb pointer to a Keyboard object
 * @return true: show cursor on the current text area, false: hide cursor
 */
bool lv_keyboard_get_cursor_manage(const lv_obj_t * kb)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->cursor_mng == 0 ? false : true;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Default keyboard event to add characters to the Text area and change the map.
 * If a custom `event_cb` is added to the keyboard this function be called from it to handle the
 * button clicks
 * @param kb pointer to a  keyboard
 * @param event the triggering event
 */
void lv_keyboard_def_event_cb(lv_obj_t * kb, lv_event_t event)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    if(event != LV_EVENT_VALUE_CHANGED) return;

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    uint16_t btn_id   = lv_btnmatrix_get_active_btn(kb);
    if(btn_id == LV_BTNMATRIX_BTN_NONE) return;
    if(lv_btnmatrix_get_btn_ctrl(kb, btn_id, LV_BTNMATRIX_CTRL_HIDDEN | LV_BTNMATRIX_CTRL_DISABLED)) return;
    if(lv_btnmatrix_get_btn_ctrl(kb, btn_id, LV_BTNMATRIX_CTRL_NO_REPEAT) && event == LV_EVENT_LONG_PRESSED_REPEAT) return;

    const char * txt = lv_btnmatrix_get_active_btn_text(kb);
    if(txt == NULL) return;

    /*Do the corresponding action according to the text of the button*/
    if(strcmp(txt, "abc") == 0) {
        ext->mode = LV_KEYBOARD_MODE_TEXT_LOWER;
        lv_btnmatrix_set_map(kb, kb_map[LV_KEYBOARD_MODE_TEXT_LOWER]);
        lv_btnmatrix_set_ctrl_map(kb, kb_ctrl[LV_KEYBOARD_MODE_TEXT_LOWER]);
        return;
    }
    else if(strcmp(txt, "ABC") == 0) {
        ext->mode = LV_KEYBOARD_MODE_TEXT_UPPER;
        lv_btnmatrix_set_map(kb, kb_map[LV_KEYBOARD_MODE_TEXT_UPPER]);
        lv_btnmatrix_set_ctrl_map(kb, kb_ctrl[LV_KEYBOARD_MODE_TEXT_UPPER]);
        return;
    }
    else if(strcmp(txt, "1#") == 0) {
        ext->mode = LV_KEYBOARD_MODE_SPECIAL;
        lv_btnmatrix_set_map(kb, kb_map[LV_KEYBOARD_MODE_SPECIAL]);
        lv_btnmatrix_set_ctrl_map(kb, kb_ctrl[LV_KEYBOARD_MODE_SPECIAL]);
        return;
    }
    else if(strcmp(txt, LV_SYMBOL_CLOSE) == 0) {
        if(kb->event_cb != lv_keyboard_def_event_cb) {
            lv_res_t res = lv_event_send(kb, LV_EVENT_CANCEL, NULL);
            if(res != LV_RES_OK) return;
        }
        else {
            lv_keyboard_set_textarea(kb, NULL); /*De-assign the text area  to hide it cursor if needed*/
            lv_obj_del(kb);
            return;
        }
        return;
    }
    else if(strcmp(txt, LV_SYMBOL_OK) == 0) {
        if(kb->event_cb != lv_keyboard_def_event_cb) {
            lv_res_t res = lv_event_send(kb, LV_EVENT_APPLY, NULL);
            if(res != LV_RES_OK) return;
        }
        else {
            lv_keyboard_set_textarea(kb, NULL); /*De-assign the text area to hide it cursor if needed*/
        }
        return;
    }

    /*Add the characters to the text area if set*/
    if(ext->ta == NULL) return;

    if(strcmp(txt, "Enter") == 0 || strcmp(txt, LV_SYMBOL_NEW_LINE) == 0)
        lv_textarea_add_char(ext->ta, '\n');
    else if(strcmp(txt, LV_SYMBOL_LEFT) == 0)
        lv_textarea_cursor_left(ext->ta);
    else if(strcmp(txt, LV_SYMBOL_RIGHT) == 0)
        lv_textarea_cursor_right(ext->ta);
    else if(strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
        lv_textarea_del_char(ext->ta);
    else if(strcmp(txt, "+/-") == 0) {
        uint16_t cur        = lv_textarea_get_cursor_pos(ext->ta);
        const char * ta_txt = lv_textarea_get_text(ext->ta);
        if(ta_txt[0] == '-') {
            lv_textarea_set_cursor_pos(ext->ta, 1);
            lv_textarea_del_char(ext->ta);
            lv_textarea_add_char(ext->ta, '+');
            lv_textarea_set_cursor_pos(ext->ta, cur);
        }
        else if(ta_txt[0] == '+') {
            lv_textarea_set_cursor_pos(ext->ta, 1);
            lv_textarea_del_char(ext->ta);
            lv_textarea_add_char(ext->ta, '-');
            lv_textarea_set_cursor_pos(ext->ta, cur);
        }
        else {
            lv_textarea_set_cursor_pos(ext->ta, 0);
            lv_textarea_add_char(ext->ta, '-');
            lv_textarea_set_cursor_pos(ext->ta, cur + 1);
        }
    }
    else {
        lv_textarea_add_text(ext->ta, txt);
    }
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
static lv_res_t lv_keyboard_signal(lv_obj_t * kb, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(kb, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }
    else if(sign == LV_SIGNAL_FOCUS) {
        lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
        /*Show the cursor of the Text area if cursor management is enabled*/
        if(ext->ta && ext->cursor_mng) {
            lv_textarea_set_cursor_hidden(ext->ta, false);
        }
    }
    else if(sign == LV_SIGNAL_DEFOCUS) {
        lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
        /*Show the cursor of the Text area if cursor management is enabled*/
        if(ext->ta && ext->cursor_mng) {
            lv_textarea_set_cursor_hidden(ext->ta, true);
        }
    }

    return res;
}

/**
 * Update the key map for the current mode
 * @param kb pointer to a keyboard object
 */
static void lv_keyboard_update_map(lv_obj_t * kb)
{
    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    lv_btnmatrix_set_map(kb, kb_map[ext->mode]);
    lv_btnmatrix_set_ctrl_map(kb, kb_ctrl[ext->mode]);
}

#endif
