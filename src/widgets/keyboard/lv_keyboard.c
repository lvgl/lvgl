
/**
 * @file lv_keyboard.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_keyboard_private.h"
#include "../../core/lv_obj_class_private.h"
#if LV_USE_KEYBOARD

#include "../textarea/lv_textarea.h"
#include "../../misc/lv_assert.h"
#include "../../stdlib/lv_string.h"

/*Testing of dependencies*/
#if LV_USE_BUTTONMATRIX == 0
    #error "lv_buttonmatrix is required. Enable it in lv_conf.h (LV_USE_BUTTONMATRIX  1) "
#endif

#if LV_USE_TEXTAREA == 0
    #error "lv_textarea is required. Enable it in lv_conf.h (LV_USE_TEXTAREA  1) "
#endif

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_keyboard_class)
#define LV_KB_BTN(width) LV_BUTTONMATRIX_CTRL_POPOVER | width

#ifndef LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_LOWER
    #define LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_LOWER     "abc"
#endif
#ifndef LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_UPPER
    #define LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_UPPER     "ABC"
#endif
#ifndef LV_KEYBOARD_CTRL_BUTTON_MODE_SPECIAL
    #define LV_KEYBOARD_CTRL_BUTTON_MODE_SPECIAL        "1#"
#endif
#ifndef LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_ARABIC
    #define LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_ARABIC    "أب"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_keyboard_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

static void lv_keyboard_update_map(lv_obj_t * obj);

static void lv_keyboard_update_ctrl_map(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_OBJ_PROPERTY
static const lv_property_ops_t lv_keyboard_properties[] = {
    {
        .id = LV_PROPERTY_KEYBOARD_TEXTAREA,
        .setter = lv_keyboard_set_textarea,
        .getter = lv_keyboard_get_textarea,
    },
    {
        .id = LV_PROPERTY_KEYBOARD_MODE,
        .setter = lv_keyboard_set_mode,
        .getter = lv_keyboard_get_mode,
    },
    {
        .id = LV_PROPERTY_KEYBOARD_POPOVERS,
        .setter = lv_keyboard_set_popovers,
        .getter = lv_keyboard_get_popovers,
    },
    {
        .id = LV_PROPERTY_KEYBOARD_SELECTED_BUTTON,
        .setter = lv_buttonmatrix_set_selected_button,
        .getter = lv_keyboard_get_selected_button,
    },
};
#endif

const lv_obj_class_t lv_keyboard_class = {
    .constructor_cb = lv_keyboard_constructor,
    .width_def = LV_PCT(100),
    .height_def = LV_PCT(50),
    .instance_size = sizeof(lv_keyboard_t),
    .editable = 1,
    .base_class = &lv_buttonmatrix_class,
    .name = "lv_keyboard",
    LV_PROPERTY_CLASS_FIELDS(keyboard, KEYBOARD)
};

static const char * const default_kb_map_lc[] = {LV_KEYBOARD_CTRL_BUTTON_MODE_SPECIAL, "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
                                                 LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_UPPER, "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                                 LV_SYMBOL_KEYBOARD,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                 LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_ARABIC,
#endif
                                                 LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                };

static const lv_buttonmatrix_ctrl_t default_kb_ctrl_lc_map[] = {
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 5, LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_BUTTONMATRIX_CTRL_CHECKED | 7,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 6, LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_BUTTONMATRIX_CTRL_CHECKED | 7,
    LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
#endif
    LV_BUTTONMATRIX_CTRL_CHECKED | 2, 6, LV_BUTTONMATRIX_CTRL_CHECKED | 2, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2
};

static const char * const default_kb_map_uc[] = {LV_KEYBOARD_CTRL_BUTTON_MODE_SPECIAL, "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
                                                 LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_LOWER, "A", "S", "D", "F", "G", "H", "J", "K", "L", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
                                                 LV_SYMBOL_CLOSE,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                 LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_ARABIC,
#endif
                                                 LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                };

static const lv_buttonmatrix_ctrl_t default_kb_ctrl_uc_map[] = {
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 5, LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_BUTTONMATRIX_CTRL_CHECKED | 7,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 6, LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_BUTTONMATRIX_CTRL_CHECKED | 7,
    LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
#endif
    LV_BUTTONMATRIX_CTRL_CHECKED | 2, 6, LV_BUTTONMATRIX_CTRL_CHECKED | 2, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2
};

#if LV_USE_ARABIC_PERSIAN_CHARS == 1
static const char * const default_kb_map_ar[] = {
    LV_KEYBOARD_CTRL_BUTTON_MODE_SPECIAL, "ض", "ص", "ث", "ق", "ف", "غ", "ع", "ه", "خ", "ح", "ج", "\n",
    "ش", "س", "ي", "ب", "ل", "ا", "ت", "ن", "م", "ك", "ط", LV_SYMBOL_BACKSPACE, "\n",
    "ذ", "ء", "ؤ", "ر", "ى", "ة", "و", "ز", "ظ", "د", "ز", "ظ", "د", "\n",
    LV_SYMBOL_CLOSE, LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_LOWER, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_NEW_LINE, LV_SYMBOL_OK, ""
};

static const lv_buttonmatrix_ctrl_t default_kb_ctrl_ar_map[] = {
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2, 2, 6, 2, 3, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2
};
#endif

static const char * const default_kb_map_spec[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", LV_SYMBOL_BACKSPACE, "\n",
                                                   LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_LOWER, "+", "&", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
                                                   "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
                                                   LV_SYMBOL_KEYBOARD,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                   LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_ARABIC,
#endif
                                                   LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                  };

static const lv_buttonmatrix_ctrl_t default_kb_ctrl_spec_map[] = {
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | 2,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2, LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
#endif
    LV_BUTTONMATRIX_CTRL_CHECKED | 2, 6, LV_BUTTONMATRIX_CTRL_CHECKED | 2, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2
};

static const char * const default_kb_map_num[] = {"1", "2", "3", LV_SYMBOL_KEYBOARD, "\n",
                                                  "4", "5", "6", LV_SYMBOL_OK, "\n",
                                                  "7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
                                                  "+/-", "0", ".", LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, ""
                                                 };

static const lv_buttonmatrix_ctrl_t default_kb_ctrl_num_map[] = {
    1, 1, 1, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
    1, 1, 1, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
    1, 1, 1, 2,
    1, 1, 1, 1, 1
};

static const char * const * kb_map[10] = {
    default_kb_map_lc,
    default_kb_map_uc,
    default_kb_map_spec,
    default_kb_map_num,
    default_kb_map_lc,
    default_kb_map_lc,
    default_kb_map_lc,
    default_kb_map_lc,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    default_kb_map_ar,
#endif
    NULL
};
static const lv_buttonmatrix_ctrl_t * kb_ctrl[10] = {
    default_kb_ctrl_lc_map,
    default_kb_ctrl_uc_map,
    default_kb_ctrl_spec_map,
    default_kb_ctrl_num_map,
    default_kb_ctrl_lc_map,
    default_kb_ctrl_lc_map,
    default_kb_ctrl_lc_map,
    default_kb_ctrl_lc_map,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    default_kb_ctrl_ar_map,
#endif
    NULL
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_keyboard_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_keyboard_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_keyboard_set_textarea(lv_obj_t * obj, lv_obj_t * ta)
{
    if(ta) {
        LV_ASSERT_OBJ(ta, &lv_textarea_class);
    }

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;

    /*Hide the cursor of the old Text area if cursor management is enabled*/
    if(keyboard->ta) {
        lv_obj_remove_state(keyboard->ta, LV_STATE_FOCUSED);
    }

    keyboard->ta = ta;

    /*Show the cursor of the new Text area if cursor management is enabled*/
    if(keyboard->ta) {
        lv_obj_add_state(keyboard->ta, LV_STATE_FOCUSED);
    }
}

void lv_keyboard_set_mode(lv_obj_t * obj, lv_keyboard_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    if(keyboard->mode == mode) return;

    keyboard->mode = mode;
    lv_keyboard_update_map(obj);
}

void lv_keyboard_set_popovers(lv_obj_t * obj, bool en)
{
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;

    if(keyboard->popovers == en) {
        return;
    }

    keyboard->popovers = en;
    lv_keyboard_update_ctrl_map(obj);
}

void lv_keyboard_set_map(lv_obj_t * obj, lv_keyboard_mode_t mode, const char * const map[],
                         const lv_buttonmatrix_ctrl_t ctrl_map[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    kb_map[mode] = map;
    kb_ctrl[mode] = ctrl_map;
    lv_keyboard_update_map(obj);
}

/*=====================
 * Getter functions
 *====================*/

lv_obj_t * lv_keyboard_get_textarea(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    return keyboard->ta;
}

lv_keyboard_mode_t lv_keyboard_get_mode(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    return keyboard->mode;
}

bool lv_keyboard_get_popovers(const lv_obj_t * obj)
{
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    return keyboard->popovers;
}

/*=====================
 * Other functions
 *====================*/

void lv_keyboard_def_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    uint32_t btn_id = lv_buttonmatrix_get_selected_button(obj);
    if(btn_id == LV_BUTTONMATRIX_BUTTON_NONE) return;

    const char * txt = lv_buttonmatrix_get_button_text(obj, btn_id);
    if(txt == NULL) return;

    if(lv_strcmp(txt, LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_LOWER) == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_TEXT_LOWER;
        lv_buttonmatrix_set_map(obj, kb_map[LV_KEYBOARD_MODE_TEXT_LOWER]);
        lv_keyboard_update_ctrl_map(obj);
        return;
    }
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    else if(lv_strcmp(txt, LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_ARABIC) == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_TEXT_ARABIC;
        lv_buttonmatrix_set_map(obj, kb_map[LV_KEYBOARD_MODE_TEXT_ARABIC]);
        lv_keyboard_update_ctrl_map(obj);
        return;
    }
#endif
    else if(lv_strcmp(txt, LV_KEYBOARD_CTRL_BUTTON_MODE_TEXT_UPPER) == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_TEXT_UPPER;
        lv_buttonmatrix_set_map(obj, kb_map[LV_KEYBOARD_MODE_TEXT_UPPER]);
        lv_keyboard_update_ctrl_map(obj);
        return;
    }
    else if(lv_strcmp(txt, LV_KEYBOARD_CTRL_BUTTON_MODE_SPECIAL) == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_SPECIAL;
        lv_buttonmatrix_set_map(obj, kb_map[LV_KEYBOARD_MODE_SPECIAL]);
        lv_keyboard_update_ctrl_map(obj);
        return;
    }
    else if(lv_strcmp(txt, LV_SYMBOL_CLOSE) == 0 || lv_strcmp(txt, LV_SYMBOL_KEYBOARD) == 0) {
        lv_result_t res = lv_obj_send_event(obj, LV_EVENT_CANCEL, NULL);
        if(res != LV_RESULT_OK) return;

        if(keyboard->ta) {
            res = lv_obj_send_event(keyboard->ta, LV_EVENT_CANCEL, NULL);
            if(res != LV_RESULT_OK) return;
        }
        return;
    }
    else if(lv_strcmp(txt, LV_SYMBOL_OK) == 0) {
        lv_result_t res = lv_obj_send_event(obj, LV_EVENT_READY, NULL);
        if(res != LV_RESULT_OK) return;

        if(keyboard->ta) {
            res = lv_obj_send_event(keyboard->ta, LV_EVENT_READY, NULL);
            if(res != LV_RESULT_OK) return;
        }
        return;
    }

    /*Add the characters to the text area if set*/
    if(keyboard->ta == NULL) return;

    if(lv_strcmp(txt, "Enter") == 0 || lv_strcmp(txt, LV_SYMBOL_NEW_LINE) == 0) {
        lv_textarea_add_char(keyboard->ta, '\n');
        if(lv_textarea_get_one_line(keyboard->ta)) {
            lv_result_t res = lv_obj_send_event(keyboard->ta, LV_EVENT_READY, NULL);
            if(res != LV_RESULT_OK) return;
        }
    }
    else if(lv_strcmp(txt, LV_SYMBOL_LEFT) == 0) {
        lv_textarea_cursor_left(keyboard->ta);
    }
    else if(lv_strcmp(txt, LV_SYMBOL_RIGHT) == 0) {
        lv_textarea_cursor_right(keyboard->ta);
    }
    else if(lv_strcmp(txt, LV_SYMBOL_BACKSPACE) == 0) {
        lv_textarea_delete_char(keyboard->ta);
    }
    else if(lv_strcmp(txt, "+/-") == 0) {
        uint32_t cur        = lv_textarea_get_cursor_pos(keyboard->ta);
        const char * ta_txt = lv_textarea_get_text(keyboard->ta);
        if(ta_txt[0] == '-') {
            lv_textarea_set_cursor_pos(keyboard->ta, 1);
            lv_textarea_delete_char(keyboard->ta);
            lv_textarea_add_char(keyboard->ta, '+');
            lv_textarea_set_cursor_pos(keyboard->ta, cur);
        }
        else if(ta_txt[0] == '+') {
            lv_textarea_set_cursor_pos(keyboard->ta, 1);
            lv_textarea_delete_char(keyboard->ta);
            lv_textarea_add_char(keyboard->ta, '-');
            lv_textarea_set_cursor_pos(keyboard->ta, cur);
        }
        else {
            lv_textarea_set_cursor_pos(keyboard->ta, 0);
            lv_textarea_add_char(keyboard->ta, '-');
            lv_textarea_set_cursor_pos(keyboard->ta, cur + 1);
        }
    }
    else {
        lv_textarea_add_text(keyboard->ta, txt);
    }
}

const char * const * lv_keyboard_get_map_array(const lv_obj_t * kb)
{
    return lv_buttonmatrix_get_map(kb);
}

uint32_t lv_keyboard_get_selected_button(const lv_obj_t * obj)
{
    return lv_buttonmatrix_get_selected_button(obj);
}

const char * lv_keyboard_get_button_text(const lv_obj_t * obj, uint32_t btn_id)
{
    return lv_buttonmatrix_get_button_text(obj, btn_id);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_keyboard_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    keyboard->ta         = NULL;
    keyboard->mode       = LV_KEYBOARD_MODE_TEXT_LOWER;
    keyboard->popovers   = 0;

    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(obj, lv_keyboard_def_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_style_base_dir(obj, LV_BASE_DIR_LTR, 0);

    lv_keyboard_update_map(obj);
}

/**
 * Update the key and control map for the current mode
 * @param obj pointer to a keyboard object
 */
static void lv_keyboard_update_map(lv_obj_t * obj)
{
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    lv_buttonmatrix_set_map(obj, kb_map[keyboard->mode]);
    lv_keyboard_update_ctrl_map(obj);
}

/**
 * Update the control map for the current mode
 * @param obj pointer to a keyboard object
 */
static void lv_keyboard_update_ctrl_map(lv_obj_t * obj)
{
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;

    if(keyboard->popovers) {
        /*Apply the current control map (already includes LV_BUTTONMATRIX_CTRL_POPOVER flags)*/
        lv_buttonmatrix_set_ctrl_map(obj, kb_ctrl[keyboard->mode]);
    }
    else {
        /*Make a copy of the current control map*/
        lv_buttonmatrix_t * btnm = (lv_buttonmatrix_t *)obj;
        lv_buttonmatrix_ctrl_t * ctrl_map = lv_malloc(btnm->btn_cnt * sizeof(lv_buttonmatrix_ctrl_t));
        lv_memcpy(ctrl_map, kb_ctrl[keyboard->mode], sizeof(lv_buttonmatrix_ctrl_t) * btnm->btn_cnt);

        /*Remove all LV_BUTTONMATRIX_CTRL_POPOVER flags*/
        uint32_t i;
        for(i = 0; i < btnm->btn_cnt; i++) {
            ctrl_map[i] &= (~LV_BUTTONMATRIX_CTRL_POPOVER);
        }

        /*Apply new control map and clean up*/
        lv_buttonmatrix_set_ctrl_map(obj, ctrl_map);
        lv_free(ctrl_map);
    }
}

#endif  /*LV_USE_KEYBOARD*/
