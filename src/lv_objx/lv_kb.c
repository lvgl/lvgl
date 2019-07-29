
/**
 * @file lv_kb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_kb.h"
#if LV_USE_KB != 0

#include "lv_ta.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#define LV_KB_CTRL_BTN_FLAGS (LV_BTNM_CTRL_NO_REPEAT | LV_BTNM_CTRL_CLICK_TRIG)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_kb_signal(lv_obj_t * kb, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
/* clang-format off */
static const char * kb_map_lc[] = {"1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "Bksp", "\n",
                                   "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", "Enter", "\n",
                                   "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                   LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""};

static const lv_btnm_ctrl_t kb_ctrl_lc_map[] = {
    LV_KB_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KB_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KB_CTRL_BTN_FLAGS | 2};

static const char * kb_map_uc[] = {"1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "Bksp", "\n",
                                   "abc", "A", "S", "D", "F", "G", "H", "J", "K", "L", "Enter", "\n",
                                   "_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
                                   LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""};

static const lv_btnm_ctrl_t kb_ctrl_uc_map[] = {
    LV_KB_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KB_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KB_CTRL_BTN_FLAGS | 2};

static const char * kb_map_spec[] = {"0", "1", "2", "3", "4" ,"5", "6", "7", "8", "9", "Bksp", "\n",
                                     "abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
                                     "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
                                     LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""};

static const lv_btnm_ctrl_t kb_ctrl_spec_map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 2,
    LV_KB_CTRL_BTN_FLAGS | 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KB_CTRL_BTN_FLAGS | 2, LV_KB_CTRL_BTN_FLAGS | 2, 6, 2, 2};

static const char * kb_map_num[] = {"1", "2", "3", LV_SYMBOL_CLOSE, "\n",
                                    "4", "5", "6", LV_SYMBOL_OK, "\n",
                                    "7", "8", "9", "Bksp", "\n",
                                    "+/-", "0", ".", LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, ""};

static const lv_btnm_ctrl_t kb_ctrl_num_map[] = {
        1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 2,
        1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 2,
        1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 2,
        LV_KB_CTRL_BTN_FLAGS | 1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 1, LV_KB_CTRL_BTN_FLAGS | 1};
/* clang-format on */

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
lv_obj_t * lv_kb_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("keyboard create started");

    /*Create the ancestor of keyboard*/
    lv_obj_t * new_kb = lv_btnm_create(par, copy);
    lv_mem_assert(new_kb);
    if(new_kb == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_kb);

    /*Allocate the keyboard type specific extended data*/
    lv_kb_ext_t * ext = lv_obj_allocate_ext_attr(new_kb, sizeof(lv_kb_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */

    ext->ta         = NULL;
    ext->mode       = LV_KB_MODE_TEXT;
    ext->cursor_mng = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_kb, lv_kb_signal);

    /*Init the new keyboard keyboard*/
    if(copy == NULL) {
        /* Set a size which fits into the parent.
         * Don't use `par` directly because if the window is created on a page it is moved to the
         * scrollable so the parent has changed */
        lv_obj_set_size(new_kb, lv_obj_get_width_fit(lv_obj_get_parent(new_kb)),
                        lv_obj_get_height_fit(lv_obj_get_parent(new_kb)) / 2);

        lv_obj_align(new_kb, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
        lv_obj_set_event_cb(new_kb, lv_kb_def_event_cb);
        lv_btnm_set_map(new_kb, kb_map_lc);
        lv_btnm_set_ctrl_map(new_kb, kb_ctrl_lc_map);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_kb_set_style(new_kb, LV_KB_STYLE_BG, th->style.kb.bg);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_REL, th->style.kb.btn.rel);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_PR, th->style.kb.btn.pr);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_TGL_REL, th->style.kb.btn.tgl_rel);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_TGL_PR, th->style.kb.btn.tgl_pr);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_INA, th->style.kb.btn.ina);
        } else {
            /*Let the button matrix's styles*/
        }
    }
    /*Copy an existing keyboard*/
    else {
        lv_kb_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->ta                = NULL;
        ext->ta                = copy_ext->ta;
        ext->mode              = copy_ext->mode;
        ext->cursor_mng        = copy_ext->cursor_mng;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_kb);
    }

    LV_LOG_INFO("keyboard created");

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
        lv_ta_set_cursor_type(ext->ta, cur_type | LV_CURSOR_HIDDEN);
    }

    ext->ta = ta;

    /*Show the cursor of the new Text area if cursor management is enabled*/
    if(ext->ta && ext->cursor_mng) {
        cur_type = lv_ta_get_cursor_type(ext->ta);
        lv_ta_set_cursor_type(ext->ta, cur_type & (~LV_CURSOR_HIDDEN));
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
    if(mode == LV_KB_MODE_TEXT) {
        lv_btnm_set_map(kb, kb_map_lc);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_lc_map);
    } else if(mode == LV_KB_MODE_NUM) {
        lv_btnm_set_map(kb, kb_map_num);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_num_map);
    }
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

    ext->cursor_mng = en == false ? 0 : 1;

    if(ext->ta) {
        lv_cursor_type_t cur_type;
        cur_type = lv_ta_get_cursor_type(ext->ta);

        if(ext->cursor_mng) {
            lv_ta_set_cursor_type(ext->ta, cur_type & (~LV_CURSOR_HIDDEN));
        } else {
            lv_ta_set_cursor_type(ext->ta, cur_type | LV_CURSOR_HIDDEN);
        }
    }
}

/**
 * Set a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_kb_set_style(lv_obj_t * kb, lv_kb_style_t type, const lv_style_t * style)
{
    switch(type) {
        case LV_KB_STYLE_BG: lv_btnm_set_style(kb, LV_BTNM_STYLE_BG, style); break;
        case LV_KB_STYLE_BTN_REL: lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_REL, style); break;
        case LV_KB_STYLE_BTN_PR: lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_PR, style); break;
        case LV_KB_STYLE_BTN_TGL_REL: lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_TGL_REL, style); break;
        case LV_KB_STYLE_BTN_TGL_PR: lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_TGL_PR, style); break;
        case LV_KB_STYLE_BTN_INA: lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_INA, style); break;
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
lv_obj_t * lv_kb_get_ta(const lv_obj_t * kb)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->ta;
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'lv_kb_mode_t'
 */
lv_kb_mode_t lv_kb_get_mode(const lv_obj_t * kb)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->mode;
}

/**
 * Get the current cursor manage mode.
 * @param kb pointer to a Keyboard object
 * @return true: show cursor on the current text area, false: hide cursor
 */
bool lv_kb_get_cursor_manage(const lv_obj_t * kb)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->cursor_mng == 0 ? false : true;
}

/**
 * Get a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_kb_get_style(const lv_obj_t * kb, lv_kb_style_t type)
{
    const lv_style_t * style = NULL;

    switch(type) {
        case LV_KB_STYLE_BG: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BG); break;
        case LV_KB_STYLE_BTN_REL: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_REL); break;
        case LV_KB_STYLE_BTN_PR: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_PR); break;
        case LV_KB_STYLE_BTN_TGL_REL: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_TGL_REL); break;
        case LV_KB_STYLE_BTN_TGL_PR: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_TGL_PR); break;
        case LV_KB_STYLE_BTN_INA: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_INA); break;
        default: style = NULL; break;
    }

    return style;
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
void lv_kb_def_event_cb(lv_obj_t * kb, lv_event_t event)
{
    if(event != LV_EVENT_VALUE_CHANGED && event != LV_EVENT_LONG_PRESSED_REPEAT) return;

    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    uint16_t btn_id   = lv_btnm_get_active_btn(kb);
    if(btn_id == LV_BTNM_BTN_NONE) return;
    if(lv_btnm_get_btn_ctrl(kb, btn_id, LV_BTNM_CTRL_HIDDEN | LV_BTNM_CTRL_INACTIVE)) return;
    if(lv_btnm_get_btn_ctrl(kb, btn_id, LV_BTNM_CTRL_NO_REPEAT) && event == LV_EVENT_LONG_PRESSED_REPEAT) return;

    const char * txt = lv_btnm_get_active_btn_text(kb);
    if(txt == NULL) return;

    /*Do the corresponding action according to the text of the button*/
    if(strcmp(txt, "abc") == 0) {
        lv_btnm_set_map(kb, kb_map_lc);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_lc_map);
        return;
    } else if(strcmp(txt, "ABC") == 0) {
        lv_btnm_set_map(kb, kb_map_uc);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_uc_map);
        return;
    } else if(strcmp(txt, "1#") == 0) {
        lv_btnm_set_map(kb, kb_map_spec);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_spec_map);
        return;
    } else if(strcmp(txt, LV_SYMBOL_CLOSE) == 0) {
        if(kb->event_cb != lv_kb_def_event_cb) {
            lv_res_t res = lv_event_send(kb, LV_EVENT_CANCEL, NULL);
            if(res != LV_RES_OK) return;
        } else {
            lv_kb_set_ta(kb, NULL); /*De-assign the text area  to hide it cursor if needed*/
            lv_obj_del(kb);
            return;
        }
        return;
    } else if(strcmp(txt, LV_SYMBOL_OK) == 0) {
        if(kb->event_cb != lv_kb_def_event_cb) {
            lv_res_t res = lv_event_send(kb, LV_EVENT_APPLY, NULL);
            if(res != LV_RES_OK) return;
        } else {
            lv_kb_set_ta(kb, NULL); /*De-assign the text area to hide it cursor if needed*/
        }
        return;
    }

    /*Add the characters to the text area if set*/
    if(ext->ta == NULL) return;

    if(strcmp(txt, "Enter") == 0)
        lv_ta_add_char(ext->ta, '\n');
    else if(strcmp(txt, LV_SYMBOL_LEFT) == 0)
        lv_ta_cursor_left(ext->ta);
    else if(strcmp(txt, LV_SYMBOL_RIGHT) == 0)
        lv_ta_cursor_right(ext->ta);
    else if(strcmp(txt, "Bksp") == 0)
        lv_ta_del_char(ext->ta);
    else if(strcmp(txt, "+/-") == 0) {
        uint16_t cur        = lv_ta_get_cursor_pos(ext->ta);
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
    } else if(sign == LV_SIGNAL_FOCUS) {
        lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
        /*Show the cursor of the new Text area if cursor management is enabled*/
        if(ext->ta && ext->cursor_mng) {
            lv_cursor_type_t cur_type = lv_ta_get_cursor_type(ext->ta);
            lv_ta_set_cursor_type(ext->ta, cur_type & (~LV_CURSOR_HIDDEN));
        }
    } else if(sign == LV_SIGNAL_DEFOCUS) {
        lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
        /*Show the cursor of the new Text area if cursor management is enabled*/
        if(ext->ta && ext->cursor_mng) {
            lv_cursor_type_t cur_type = lv_ta_get_cursor_type(ext->ta);
            lv_ta_set_cursor_type(ext->ta, cur_type | LV_CURSOR_HIDDEN);
        }
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_kb";
    }

    return res;
}

#endif
