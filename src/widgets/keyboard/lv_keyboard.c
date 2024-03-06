
/**
 * @file lv_keyboard.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_keyboard.h"
#if LV_USE_KEYBOARD

#include "../textarea/lv_textarea.h"
#include "../../misc/lv_assert.h"
#include "../../stdlib/lv_string.h"
#if LV_USE_KEYBOARD_PINYIN
    #include "lv_keyboard_pinyin.h"
    #include "../button/lv_button.h"
    #include "../image/lv_image.h"
#endif

#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_keyboard_class)
#define LV_KB_BTN(width) LV_BUTTONMATRIX_CTRL_POPOVER | width

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_keyboard_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

static void lv_keyboard_update_map(lv_obj_t * obj);

static void lv_keyboard_update_ctrl_map(lv_obj_t * obj);

#if LV_USE_KEYBOARD_PINYIN
    static void lv_keyboard_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

    static void lv_keyboard_proc_pinyin(lv_keyboard_t * keyboard);

    static void lv_keyboard_hidden_candidate(lv_keyboard_t * keyboard);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_keyboard_class = {
    .constructor_cb = lv_keyboard_constructor,
    .destructor_cb = lv_keyboard_destructor,
    .width_def = LV_PCT(100),
    .height_def = LV_PCT(50),
    .instance_size = sizeof(lv_keyboard_t),
    .editable = 1,
    .base_class = &lv_buttonmatrix_class,
    .name = "keyboard",
};

#if LV_USE_KEYBOARD_PINYIN
static const char * const default_en_kb_map_lc[] = {" ", "\n", /*candidate content*/
                                                    "1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
                                                    "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
                                                    "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                                    LV_SYMBOL_KEYBOARD,  "Zh",
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                    "أب",
#endif
                                                    LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                   };

static const char * const default_zh_kb_map_lc[] = {" ", "\n", /*candidate content*/
                                                    "1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
                                                    "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
                                                    "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                                    LV_SYMBOL_KEYBOARD, "En",
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                    "أب",
#endif
                                                    LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                   };

static const lv_buttonmatrix_ctrl_t default_kb_ctrl_lc_map[] = {
    LV_BUTTONMATRIX_CTRL_HIDDEN,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 5, LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_BUTTONMATRIX_CTRL_CHECKED | 7,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 6, LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_BUTTONMATRIX_CTRL_CHECKED | 7,
    LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
#endif
    LV_BUTTONMATRIX_CTRL_CHECKED | 2, 6, LV_BUTTONMATRIX_CTRL_CHECKED | 2, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2
};

static const char * const default_kb_map_uc[] = {" ", "\n", /*candidate content*/
                                                 "1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
                                                 "abc", "A", "S", "D", "F", "G", "H", "J", "K", "L", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
                                                 LV_SYMBOL_CLOSE,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                 "أب",
#endif
                                                 LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                };

static const lv_buttonmatrix_ctrl_t default_kb_ctrl_uc_map[] = {
    LV_BUTTONMATRIX_CTRL_HIDDEN,
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
    " ", "\n", /*candidate content*/
    "1#", "ض", "ص", "ث", "ق", "ف", "غ", "ع", "ه", "خ", "ح", "ج", "\n",
    "ش", "س", "ي", "ب", "ل", "ا", "ت", "ن", "م", "ك", "ط", LV_SYMBOL_BACKSPACE, "\n",
    "ذ", "ء", "ؤ", "ر", "ى", "ة", "و", "ز", "ظ", "د", "ز", "ظ", "د", "\n",
    LV_SYMBOL_CLOSE, "abc", LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_NEW_LINE, LV_SYMBOL_OK, ""
};

static const lv_buttonmatrix_ctrl_t default_kb_ctrl_ar_map[] = {
    LV_BUTTONMATRIX_CTRL_HIDDEN,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2, 2, 6, 2, 3, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2
};
#endif

static const char * const default_kb_map_spec[] = {" ", "\n", /*candidate content*/
                                                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", LV_SYMBOL_BACKSPACE, "\n",
                                                   "abc", "+", "&", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
                                                   "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
                                                   LV_SYMBOL_KEYBOARD,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                   "أب",
#endif
                                                   LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                  };

static const lv_buttonmatrix_ctrl_t default_kb_ctrl_spec_map[] = {
    LV_BUTTONMATRIX_CTRL_HIDDEN,
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_BUTTONMATRIX_CTRL_CHECKED | 2,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2, LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
#endif
    LV_BUTTONMATRIX_CTRL_CHECKED | 2, 6, LV_BUTTONMATRIX_CTRL_CHECKED | 2, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2
};
#else /*LV_USE_KEYBOARD_PINYIN*/
static const char * const default_kb_map_lc[] = {"1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
                                                 "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                                 LV_SYMBOL_KEYBOARD,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                 "أب",
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

static const char * const default_kb_map_uc[] = {"1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
                                                 "abc", "A", "S", "D", "F", "G", "H", "J", "K", "L", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
                                                 LV_SYMBOL_CLOSE,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                 "أب",
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
    "1#", "ض", "ص", "ث", "ق", "ف", "غ", "ع", "ه", "خ", "ح", "ج", "\n",
    "ش", "س", "ي", "ب", "ل", "ا", "ت", "ن", "م", "ك", "ط", LV_SYMBOL_BACKSPACE, "\n",
    "ذ", "ء", "ؤ", "ر", "ى", "ة", "و", "ز", "ظ", "د", "ز", "ظ", "د", "\n",
    LV_SYMBOL_CLOSE, "abc", LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_NEW_LINE, LV_SYMBOL_OK, ""
};

static const lv_buttonmatrix_ctrl_t default_kb_ctrl_ar_map[] = {
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2, 2, 6, 2, 3, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2
};
#endif

static const char * const default_kb_map_spec[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", LV_SYMBOL_BACKSPACE, "\n",
                                                   "abc", "+", "&", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
                                                   "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
                                                   LV_SYMBOL_KEYBOARD,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                   "أب",
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
#endif /*LV_USE_KEYBOARD_PINYIN*/

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

#if LV_USE_KEYBOARD_PINYIN
static const char * * kb_map[11] = {
    (const char * *)default_en_kb_map_lc,
    (const char * *)default_kb_map_uc,
    (const char * *)default_kb_map_spec,
    (const char * *)default_kb_map_num,
    (const char * *)default_zh_kb_map_lc,
    (const char * *)default_en_kb_map_lc,
    (const char * *)default_en_kb_map_lc,
    (const char * *)default_en_kb_map_lc,
    (const char * *)default_en_kb_map_lc,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    (const char * *)default_kb_map_ar,
#endif
    (const char * *)NULL
};
static const lv_buttonmatrix_ctrl_t * kb_ctrl[11] = {
    default_kb_ctrl_lc_map,
    default_kb_ctrl_uc_map,
    default_kb_ctrl_spec_map,
    default_kb_ctrl_num_map,
    default_kb_ctrl_lc_map,
    default_kb_ctrl_lc_map,
    default_kb_ctrl_lc_map,
    default_kb_ctrl_lc_map,
    default_kb_ctrl_lc_map,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    default_kb_ctrl_ar_map,
#endif
    NULL
};
#else /*LV_USE_KEYBOARD_PINYIN*/
static const char * * kb_map[10] = {
    (const char * *)default_kb_map_lc,
    (const char * *)default_kb_map_uc,
    (const char * *)default_kb_map_spec,
    (const char * *)default_kb_map_num,
    (const char * *)default_kb_map_lc,
    (const char * *)default_kb_map_lc,
    (const char * *)default_kb_map_lc,
    (const char * *)default_kb_map_lc,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    (const char * *)default_kb_map_ar,
#endif
    (const char * *)NULL
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
#endif /*LV_USE_KEYBOARD_PINYIN*/

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
        lv_obj_remove_state(obj, LV_STATE_FOCUSED);
    }

    keyboard->ta = ta;

    /*Show the cursor of the new Text area if cursor management is enabled*/
    if(keyboard->ta) {
        lv_obj_add_flag(obj, LV_STATE_FOCUSED);
    }
    /*clear pinyin and hidden candidate content when the text area defocused*/
    else {
        lv_label_set_text(keyboard->pinyin_label, "");
        lv_obj_add_flag(keyboard->candidate_cont, LV_OBJ_FLAG_HIDDEN);
#if LV_KEYBOARD_PINYIN_USE_EXT_BUTTONMATRIX
        lv_obj_add_flag(keyboard->ext_candidate_btnm_cont, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_state(keyboard->ext_btn, LV_STATE_CHECKED);
        lv_image_set_src(keyboard->ext_img, LV_SYMBOL_DOWN);
#endif
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

void lv_keyboard_set_map(lv_obj_t * obj, lv_keyboard_mode_t mode, const char * map[],
                         const lv_buttonmatrix_ctrl_t ctrl_map[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    kb_map[mode] = map;
    kb_ctrl[mode] = ctrl_map;
    lv_keyboard_update_map(obj);
}

#if LV_USE_KEYBOARD_PINYIN
void lv_keyboard_set_pinyin_dict(lv_obj_t * obj, const lv_keyboard_pinyin_dict_t ** dict)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;

    keyboard->dict = dict;
}
#endif

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

bool lv_buttonmatrix_get_popovers(const lv_obj_t * obj)
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

    if(lv_strcmp(txt, "abc") == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_TEXT_LOWER;
        lv_buttonmatrix_set_map(obj, kb_map[LV_KEYBOARD_MODE_TEXT_LOWER]);
        lv_keyboard_update_ctrl_map(obj);
        goto update_candidate;
    }
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    else if(lv_strcmp(txt, "أب") == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_TEXT_ARABIC;
        lv_buttonmatrix_set_map(obj, kb_map[LV_KEYBOARD_MODE_TEXT_ARABIC]);
        lv_keyboard_update_ctrl_map(obj);
        goto update_candidate;
    }
#endif
    else if(lv_strcmp(txt, "ABC") == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_TEXT_UPPER;
        lv_buttonmatrix_set_map(obj, kb_map[LV_KEYBOARD_MODE_TEXT_UPPER]);
        lv_keyboard_update_ctrl_map(obj);
        goto update_candidate;
    }
    else if(lv_strcmp(txt, "1#") == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_SPECIAL;
        lv_buttonmatrix_set_map(obj, kb_map[LV_KEYBOARD_MODE_SPECIAL]);
        lv_keyboard_update_ctrl_map(obj);
        goto update_candidate;
    }
    else if(lv_strcmp(txt, LV_SYMBOL_CLOSE) == 0 || lv_strcmp(txt, LV_SYMBOL_KEYBOARD) == 0) {
        lv_result_t res = lv_obj_send_event(obj, LV_EVENT_CANCEL, NULL);
        if(res != LV_RESULT_OK) goto update_candidate;

        if(keyboard->ta) {
            res = lv_obj_send_event(keyboard->ta, LV_EVENT_CANCEL, NULL);
            if(res != LV_RESULT_OK) goto update_candidate;
        }
        goto update_candidate;;
    }
    else if(lv_strcmp(txt, LV_SYMBOL_OK) == 0) {
        lv_result_t res = lv_obj_send_event(obj, LV_EVENT_READY, NULL);
        if(res != LV_RESULT_OK) goto update_candidate;

        if(keyboard->ta) {
            res = lv_obj_send_event(keyboard->ta, LV_EVENT_READY, NULL);
            if(res != LV_RESULT_OK) goto update_candidate;
        }
        goto update_candidate;
    }
#if LV_USE_KEYBOARD_PINYIN
    else if(lv_strcmp(txt, "Zh") == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_PINYIN;
        lv_buttonmatrix_set_map(obj, kb_map[LV_KEYBOARD_MODE_PINYIN]);
        lv_keyboard_update_ctrl_map(obj);
        goto update_candidate;
    }
    else if(lv_strcmp(txt, "En") == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_TEXT_LOWER;
        lv_buttonmatrix_set_map(obj, kb_map[LV_KEYBOARD_MODE_TEXT_LOWER]);
        lv_keyboard_update_ctrl_map(obj);
        goto update_candidate;
    }
#endif

    /*Add the characters to the text area if set*/
    if(keyboard->ta == NULL) return;

    if(lv_strcmp(txt, "Enter") == 0 || lv_strcmp(txt, LV_SYMBOL_NEW_LINE) == 0) {
#if LV_USE_KEYBOARD_PINYIN
        lv_textarea_add_text(keyboard->ta, lv_label_get_text(keyboard->pinyin_label));
        lv_label_set_text(keyboard->pinyin_label, "");
        lv_obj_add_flag(keyboard->candidate_cont, LV_OBJ_FLAG_HIDDEN);
#else
        lv_textarea_add_char(keyboard->ta, '\n');
        if(lv_textarea_get_one_line(keyboard->ta)) {
            lv_result_t res = lv_obj_send_event(keyboard->ta, LV_EVENT_READY, NULL);
            if(res != LV_RESULT_OK) goto update_candidate;
        }
#endif
    }
    else if(lv_strcmp(txt, LV_SYMBOL_LEFT) == 0) {
        lv_textarea_cursor_left(keyboard->ta);
    }
    else if(lv_strcmp(txt, LV_SYMBOL_RIGHT) == 0) {
        lv_textarea_cursor_right(keyboard->ta);
    }
    else if(lv_strcmp(txt, LV_SYMBOL_BACKSPACE) == 0) {
#if LV_USE_KEYBOARD_PINYIN
        if(keyboard->mode != LV_KEYBOARD_MODE_PINYIN) {
            lv_textarea_delete_char(keyboard->ta);
        }
        else {
            char * input_pinyin = lv_label_get_text(keyboard->pinyin_label);
            size_t input_pinyin_len = lv_strlen(input_pinyin);
            if(input_pinyin_len > 0) {
                lv_label_cut_text(keyboard->pinyin_label, (input_pinyin_len - 1), 1);

                if(input_pinyin_len - 1 == 0) {
                    lv_obj_add_flag(keyboard->candidate_cont, LV_OBJ_FLAG_HIDDEN);

                    goto update_candidate;
                }

                lv_keyboard_proc_pinyin(keyboard);
            }
            else {
                lv_textarea_delete_char(keyboard->ta);
            }
        }
#else
        lv_textarea_delete_char(keyboard->ta);
#endif
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
#if LV_USE_KEYBOARD_PINYIN
        if(keyboard->mode != LV_KEYBOARD_MODE_PINYIN) {
            lv_textarea_add_text(keyboard->ta, txt);
        }
        else {
            if(*txt < 'a' || *txt > 'z') goto update_candidate;

            if(lv_obj_has_flag(keyboard->candidate_cont, LV_OBJ_FLAG_HIDDEN)) {
                lv_obj_remove_flag(keyboard->candidate_cont, LV_OBJ_FLAG_HIDDEN);
            }

            if(lv_obj_has_flag(keyboard->pinyin_label, LV_OBJ_FLAG_HIDDEN)) {
                lv_obj_remove_flag(keyboard->pinyin_label, LV_OBJ_FLAG_HIDDEN);
            }

            if(lv_obj_has_flag(keyboard->candidate_btnm, LV_OBJ_FLAG_HIDDEN)) {
                lv_obj_remove_flag(keyboard->candidate_btnm, LV_OBJ_FLAG_HIDDEN);
            }

            lv_label_ins_text(keyboard->pinyin_label, LV_LABEL_POS_LAST, txt);

            lv_keyboard_proc_pinyin(keyboard);
        }
#else
        lv_textarea_add_text(keyboard->ta, txt);
#endif
    }

update_candidate:
#if LV_USE_KEYBOARD_PINYIN
    lv_keyboard_hidden_candidate(keyboard);
#else
    return;
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_USE_KEYBOARD_PINYIN
static void lv_keyboard_free_candidate_btnm_map(lv_keyboard_t * keyboard)
{
    if(keyboard->candidate_num != 0) {
        for(uint16_t i = 0; i < keyboard->candidate_num; i++) {
            lv_free(keyboard->candidate_btnm_map[i]);
        }
        lv_free(keyboard->candidate_btnm_map);
        keyboard->candidate_num = 0;
    }
}

#if LV_KEYBOARD_PINYIN_USE_EXT_BUTTONMATRIX
static void lv_keyboard_free_ext_candidate_btnm_map(lv_keyboard_t * keyboard)
{
    if(keyboard->ext_candidate_num != 0) {
        for(uint16_t i = 0; i < keyboard->ext_candidate_num; i++) {
            lv_free(keyboard->ext_candidate_btnm_map[i]);
        }
        lv_free(keyboard->ext_candidate_btnm_map);
        keyboard->ext_candidate_num = 0;
    }
}
#endif

static char * lv_keyboard_search_chinese(size_t * candidate_num, lv_keyboard_t * keyboard, const char * pinyin_str)
{
    /*illegal data*/
    if(*pinyin_str == '\0')    return NULL;
    if(*pinyin_str == 'i')     return NULL;
    if(*pinyin_str == 'u')     return NULL;
    if(*pinyin_str == 'v')     return NULL;
    if(*pinyin_str == ' ')     return NULL;

    size_t pinyin_str_len = strlen(pinyin_str);
    char new_pinyin_str[pinyin_str_len + 1];

    /*pinyin to lower case*/
    for(size_t i = 0; i < pinyin_str_len; i++) {
        if((pinyin_str[i] >= 'A') && (pinyin_str[i] <= 'Z')) {
            new_pinyin_str[i] = pinyin_str[i] + 32;
        }
        else if((pinyin_str[i] >= 'a') && (pinyin_str[i] <= 'z')) {
            new_pinyin_str[i] = pinyin_str[i];
        }
        else {
            return NULL;
        }
    }

    if(keyboard->dict == NULL) {
        LV_LOG_ERROR("keyboard pinyin dict is NULL");
        return NULL;
    }

    lv_keyboard_pinyin_dict_t * pinyin_head = (lv_keyboard_pinyin_dict_t *)keyboard->dict[*new_pinyin_str - 'a'];
    lv_keyboard_pinyin_dict_t * pinyin_tail = (lv_keyboard_pinyin_dict_t *)keyboard->dict[*new_pinyin_str - 'a' + 1];

    size_t i = 0;
    for(; pinyin_head < pinyin_tail; pinyin_head++) {
        for(i = 0; i < pinyin_str_len - 1; i++) {
            if((*pinyin_head).pinyin_list == NULL) return NULL;

            if(*((*pinyin_head).pinyin_list + i) != new_pinyin_str[i + 1]) break;
        }

        /*perfect match*/
        if(i == (pinyin_str_len - 1)) {
            /*The Chinese character in UTF-8 encoding format is 3 bytes*/
            *candidate_num = strlen((const char *)(*pinyin_head).pinyin_mb_list) / 3;

            return (char *)(*pinyin_head).pinyin_mb_list;
        }
    }

    return NULL;
}

static void lv_keyboard_proc_pinyin(lv_keyboard_t * keyboard)
{
    char * input_pinyin = lv_label_get_text(keyboard->pinyin_label);
    if(lv_strlen(input_pinyin) == 0) {
        return;
    }

    size_t candidate_num = 0;
    char * candidate_str = lv_keyboard_search_chinese(&candidate_num, keyboard, input_pinyin);

    lv_keyboard_free_candidate_btnm_map(keyboard);
    keyboard->candidate_num = candidate_num;

    keyboard->candidate_btnm_map = (char **)lv_malloc((candidate_num + 1) * sizeof(char *));
    if(keyboard->candidate_btnm_map == NULL) {
        return;
    }

    for(size_t i = 0; i < candidate_num; i++) {
        keyboard->candidate_btnm_map[i] = (char *)lv_malloc(4 * sizeof(char));
        if(keyboard->candidate_btnm_map[i] == NULL) {
            lv_keyboard_free_candidate_btnm_map(keyboard);
            return;
        }

        lv_memcpy(keyboard->candidate_btnm_map[i], candidate_str + i * 3, 3);
        keyboard->candidate_btnm_map[i][3] = '\0';
    }

    keyboard->candidate_btnm_map[keyboard->candidate_num] = "";

    lv_buttonmatrix_set_map(keyboard->candidate_btnm, (const char **)keyboard->candidate_btnm_map);

    int32_t height = lv_obj_get_height(keyboard->candidate_btnm);
    lv_obj_set_width(keyboard->candidate_btnm, height * keyboard->candidate_num);
}

#if LV_KEYBOARD_PINYIN_USE_EXT_BUTTONMATRIX
static void lv_keyboard_proc_ext_candidate_btnm_map(lv_keyboard_t * keyboard)
{
    int32_t cont_width = lv_obj_get_width(keyboard->candidate_btnm_cont);
    int32_t btnm_width = lv_obj_get_width(keyboard->candidate_btnm);
    if(btnm_width <= cont_width) {
        lv_obj_add_flag(keyboard->ext_btn, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_remove_flag(keyboard->ext_btn, LV_OBJ_FLAG_HIDDEN);

    int32_t width = (btnm_width / keyboard->candidate_num);
    int32_t keyboard_width = lv_obj_get_width((lv_obj_t *)keyboard);

    uint8_t col_num = keyboard_width / width;
    uint8_t row_num = 0;
    if(keyboard->candidate_num % col_num == 0) {
        row_num = keyboard->candidate_num / col_num;
    }
    else {
        row_num = (keyboard->candidate_num / col_num) + 1;
    }

    lv_obj_set_height(keyboard->ext_candidate_btnm, row_num * width);

    lv_keyboard_free_ext_candidate_btnm_map(keyboard);

    keyboard->ext_candidate_num = (col_num + 1) * row_num;

    keyboard->ext_candidate_btnm_map = (char **)lv_malloc((keyboard->ext_candidate_num + 1) * sizeof(char *));
    if(keyboard->ext_candidate_btnm_map == NULL) {
        return;
    }

    for(uint16_t i = 0; i < keyboard->ext_candidate_num; i++) {
        keyboard->ext_candidate_btnm_map[i] = (char *)lv_malloc(4 * sizeof(char));
        if(keyboard->ext_candidate_btnm_map[i] == NULL) {
            lv_keyboard_free_ext_candidate_btnm_map(keyboard);
            return;
        }
    }

    for(uint8_t i = 0; i < (row_num - 1); i++) {
        lv_strcpy(keyboard->ext_candidate_btnm_map[col_num * (i + 1) + i], "\n");
    }

    lv_strcpy(keyboard->ext_candidate_btnm_map[keyboard->ext_candidate_num - 1], "");

    bool has_hidden_btn = false;
    for(uint16_t i = 0; i < (keyboard->ext_candidate_num - 1); i++) {
        if(lv_strcmp(keyboard->ext_candidate_btnm_map[i], "\n") == 0) {
            continue;
        }

        if(i < keyboard->candidate_num + row_num - 1) {
            lv_strcpy(keyboard->ext_candidate_btnm_map[i], keyboard->candidate_btnm_map[i - i / (col_num + 1)]);
        }
        else {
            lv_strcpy(keyboard->ext_candidate_btnm_map[i], " ");
            has_hidden_btn = true;
        }
    }

    lv_buttonmatrix_set_map(keyboard->ext_candidate_btnm, (const char **)keyboard->ext_candidate_btnm_map);

    if(has_hidden_btn) {
        for(uint16_t i = 0; i < (keyboard->ext_candidate_num - 1); i++) {
            if(lv_strcmp(keyboard->ext_candidate_btnm_map[i], " ") == 0) {
                lv_buttonmatrix_set_button_ctrl(keyboard->ext_candidate_btnm, (i - (row_num - 1)),
                                                LV_BUTTONMATRIX_CTRL_DISABLED);
            }
        }
    }
}
#endif

static void lv_keyboard_candidate_btnm_event_cb(lv_event_t * e)
{
    lv_keyboard_t * keyboard = (lv_keyboard_t *)lv_event_get_user_data(e);

    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        uint32_t id = lv_buttonmatrix_get_selected_button(keyboard->candidate_btnm);
        const char * txt = lv_buttonmatrix_get_button_text(keyboard->candidate_btnm, id);
        if((txt == NULL) || (lv_strcmp(txt, " ") == 0)) return;
        lv_textarea_add_text(keyboard->ta, txt);

        lv_label_set_text(keyboard->pinyin_label, "");
        lv_obj_add_flag(keyboard->candidate_cont, LV_OBJ_FLAG_HIDDEN);
    }
}

#if LV_KEYBOARD_PINYIN_USE_EXT_BUTTONMATRIX
static void lv_keyboard_ext_candidate_btnm_event_cb(lv_event_t * e)
{
    lv_keyboard_t * keyboard = (lv_keyboard_t *)lv_event_get_user_data(e);

    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        uint32_t id = lv_buttonmatrix_get_selected_button(keyboard->ext_candidate_btnm);
        const char * txt = lv_buttonmatrix_get_button_text(keyboard->ext_candidate_btnm, id);
        if((txt == NULL) || (lv_strcmp(txt, " ") == 0)) return;
        lv_textarea_add_text(keyboard->ta, txt);

        lv_label_set_text(keyboard->pinyin_label, "");
        lv_obj_add_flag(keyboard->candidate_cont, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(keyboard->ext_candidate_btnm_cont, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_state(keyboard->ext_btn, LV_STATE_CHECKED);
        lv_image_set_src(keyboard->ext_img, LV_SYMBOL_DOWN);
    }
}
#endif

static void lv_keyboard_hidden_candidate(lv_keyboard_t * keyboard)
{
    if(keyboard->mode != LV_KEYBOARD_MODE_PINYIN) {
        if(!lv_obj_has_flag(keyboard->candidate_cont, LV_OBJ_FLAG_HIDDEN)) {
            lv_label_set_text(keyboard->pinyin_label, "");
            lv_obj_add_flag(keyboard->candidate_cont, LV_OBJ_FLAG_HIDDEN);
        }
    }
    /*hide the keyboard when entering pinyin*/
    else {
        if(lv_obj_has_flag((lv_obj_t *)keyboard, LV_OBJ_FLAG_HIDDEN)) {
            lv_label_set_text(keyboard->pinyin_label, "");
            lv_obj_add_flag(keyboard->candidate_cont, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

#if LV_KEYBOARD_PINYIN_USE_EXT_BUTTONMATRIX
static void lv_keyboard_ext_btn_event_cb(lv_event_t * e)
{
    lv_keyboard_t * keyboard = (lv_keyboard_t *)lv_event_get_user_data(e);

    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        if(lv_obj_has_state(keyboard->ext_btn, LV_STATE_CHECKED)) {
            lv_image_set_src(keyboard->ext_img, LV_SYMBOL_UP);
            lv_obj_remove_flag(keyboard->ext_candidate_btnm_cont, LV_OBJ_FLAG_HIDDEN);
            lv_buttonmatrix_clear_button_ctrl_all(keyboard->ext_candidate_btnm, LV_BUTTONMATRIX_CTRL_DISABLED);
            lv_keyboard_proc_ext_candidate_btnm_map(keyboard);
        }
        else {
            lv_image_set_src(keyboard->ext_img, LV_SYMBOL_DOWN);
            lv_obj_add_flag(keyboard->ext_candidate_btnm_cont, LV_OBJ_FLAG_HIDDEN);
        }
    }
}
#endif
#endif

static void lv_keyboard_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    keyboard->ta         = NULL;
    keyboard->mode       = LV_KEYBOARD_MODE_TEXT_LOWER;
    keyboard->popovers   = 0;
#if LV_USE_KEYBOARD_PINYIN
    keyboard->dict = NULL;
    keyboard->candidate_num = 0;
    keyboard->candidate_btnm_map = NULL;
#if LV_KEYBOARD_PINYIN_USE_EXT_BUTTONMATRIX
    keyboard->ext_candidate_num = 0;
    keyboard->ext_candidate_btnm_map = NULL;
#endif
#endif

    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(obj, lv_keyboard_def_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_style_base_dir(obj, LV_BASE_DIR_LTR, 0);

#if LV_USE_KEYBOARD_PINYIN
#if LV_KEYBOARD_PINYIN_USE_DEFAULT_DICT
    keyboard->dict = lv_def_pinyin_dict;
#endif

    lv_opa_t keyboard_bg_opa = lv_obj_get_style_bg_opa(obj, 0);
    lv_color_t keyboard_bg_color = lv_obj_get_style_bg_color(obj, 0);

    static lv_style_t pinyin_default_style;
    lv_style_init(&pinyin_default_style);
    lv_style_set_bg_opa(&pinyin_default_style, keyboard_bg_opa);
    lv_style_set_bg_color(&pinyin_default_style, keyboard_bg_color);
    lv_style_set_border_width(&pinyin_default_style, 0);
    lv_style_set_pad_all(&pinyin_default_style, 0);
    lv_style_set_pad_gap(&pinyin_default_style, 0);
    lv_style_set_radius(&pinyin_default_style, 0);
#if LV_FONT_SIMSUN_16_CJK
    lv_style_set_text_font(&pinyin_default_style, &lv_font_simsun_16_cjk);
#endif

    keyboard->candidate_cont = lv_obj_create(obj);
    lv_obj_set_size(keyboard->candidate_cont, LV_PCT(100), LV_PCT(15));
    lv_obj_add_flag(keyboard->candidate_cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(keyboard->candidate_cont, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_remove_flag(keyboard->candidate_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(keyboard->candidate_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(keyboard->candidate_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_add_style(keyboard->candidate_cont, &pinyin_default_style, 0);
    lv_obj_set_style_pad_gap(keyboard->candidate_cont, 4, 0);

    keyboard->pinyin_label = lv_label_create(keyboard->candidate_cont);
    lv_label_set_text(keyboard->pinyin_label, "");
    lv_obj_set_size(keyboard->pinyin_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_remove_flag(keyboard->pinyin_label, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_add_style(keyboard->pinyin_label, &pinyin_default_style, 0);

    keyboard->candidate_btnm_cont = lv_obj_create(keyboard->candidate_cont);
    lv_obj_set_height(keyboard->candidate_btnm_cont, LV_PCT(100));
    lv_obj_remove_flag(keyboard->candidate_btnm_cont, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_flex_flow(keyboard->candidate_btnm_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(keyboard->candidate_btnm_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_flex_grow(keyboard->candidate_btnm_cont, 1);

    lv_obj_add_style(keyboard->candidate_btnm_cont, &pinyin_default_style, 0);
    lv_obj_set_style_bg_opa(keyboard->candidate_btnm_cont, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(keyboard->candidate_btnm_cont, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);

    keyboard->candidate_btnm = lv_buttonmatrix_create(keyboard->candidate_btnm_cont);
    lv_buttonmatrix_set_one_checked(keyboard->candidate_btnm, true);
    lv_obj_set_height(keyboard->candidate_btnm, LV_PCT(100));
    lv_obj_remove_flag(keyboard->candidate_btnm, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_add_style(keyboard->candidate_btnm, &pinyin_default_style, 0);
    lv_obj_add_style(keyboard->candidate_btnm, &pinyin_default_style, LV_PART_ITEMS);
    lv_obj_add_style(keyboard->candidate_btnm, &pinyin_default_style, LV_PART_ITEMS | LV_STATE_CHECKED);

#if LV_KEYBOARD_PINYIN_USE_EXT_BUTTONMATRIX
    keyboard->ext_btn = lv_button_create(keyboard->candidate_cont);
    lv_obj_set_size(keyboard->ext_btn, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_add_flag(keyboard->ext_btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_remove_flag(keyboard->ext_btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_remove_style_all(keyboard->ext_btn);

    keyboard->ext_img = lv_image_create(keyboard->ext_btn);
    lv_image_set_src(keyboard->ext_img, LV_SYMBOL_DOWN);
    lv_obj_center(keyboard->ext_img);

    keyboard->ext_candidate_btnm_cont = lv_obj_create(obj);
    lv_obj_set_size(keyboard->ext_candidate_btnm_cont, LV_PCT(100), LV_PCT(85));
    lv_obj_align_to(keyboard->ext_candidate_btnm_cont, keyboard->candidate_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    lv_obj_add_flag(keyboard->ext_candidate_btnm_cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(keyboard->ext_candidate_btnm_cont, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_flex_flow(keyboard->ext_candidate_btnm_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(keyboard->ext_candidate_btnm_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_START);
    lv_obj_set_flex_grow(keyboard->ext_candidate_btnm_cont, 1);

    lv_obj_add_style(keyboard->ext_candidate_btnm_cont, &pinyin_default_style, 0);
    lv_obj_set_style_bg_opa(keyboard->ext_candidate_btnm_cont, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(keyboard->ext_candidate_btnm_cont, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);

    keyboard->ext_candidate_btnm = lv_buttonmatrix_create(keyboard->ext_candidate_btnm_cont);
    lv_buttonmatrix_set_one_checked(keyboard->ext_candidate_btnm, true);
    lv_obj_set_width(keyboard->ext_candidate_btnm, LV_PCT(100));
    lv_obj_remove_flag(keyboard->ext_candidate_btnm, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_add_style(keyboard->ext_candidate_btnm, &pinyin_default_style, 0);
    lv_obj_set_style_border_width(keyboard->ext_candidate_btnm, 1, 0);
    lv_obj_set_style_border_color(keyboard->ext_candidate_btnm, lv_color_black(), 0);
    lv_obj_set_style_border_side(keyboard->ext_candidate_btnm, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT, 0);
    lv_obj_add_style(keyboard->ext_candidate_btnm, &pinyin_default_style, LV_PART_ITEMS);
    lv_obj_set_style_border_width(keyboard->ext_candidate_btnm, 1, LV_PART_ITEMS);
    lv_obj_set_style_border_color(keyboard->ext_candidate_btnm, lv_color_black(), LV_PART_ITEMS);
    lv_obj_set_style_border_side(keyboard->ext_candidate_btnm, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT, LV_PART_ITEMS);
    lv_obj_add_style(keyboard->ext_candidate_btnm, &pinyin_default_style, LV_PART_ITEMS | LV_STATE_CHECKED);
#endif

    lv_obj_add_event_cb(keyboard->candidate_btnm, lv_keyboard_candidate_btnm_event_cb, LV_EVENT_CLICKED, obj);
#if LV_KEYBOARD_PINYIN_USE_EXT_BUTTONMATRIX
    lv_obj_add_event_cb(keyboard->ext_btn, lv_keyboard_ext_btn_event_cb, LV_EVENT_VALUE_CHANGED, obj);
    lv_obj_add_event_cb(keyboard->ext_candidate_btnm, lv_keyboard_ext_candidate_btnm_event_cb, LV_EVENT_CLICKED, obj);
#endif
#endif /*LV_USE_KEYBOARD_PINYIN*/

    lv_keyboard_update_map(obj);
}

#if LV_USE_KEYBOARD_PINYIN
static void lv_keyboard_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;

    if(lv_obj_is_valid(keyboard->candidate_cont))
        lv_obj_delete(keyboard->candidate_cont);

    lv_keyboard_free_candidate_btnm_map(keyboard);

#if LV_KEYBOARD_PINYIN_USE_EXT_BUTTONMATRIX
    lv_keyboard_free_ext_candidate_btnm_map(keyboard);
#endif
}
#endif

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
