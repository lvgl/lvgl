/**
 * @file lv_ta.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_textarea.h"
#if LV_USE_TEXTAREA != 0

#include <string.h>
#include "../lv_misc/lv_debug.h"
#include "../lv_core/lv_group.h"
#include "../lv_core/lv_refr.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_textarea"

/*Test configuration*/
#ifndef LV_TEXTAREA_DEF_CURSOR_BLINK_TIME
    #define LV_TEXTAREA_DEF_CURSOR_BLINK_TIME 400 /*ms*/
#endif

#ifndef LV_TEXTAREA_DEF_PWD_SHOW_TIME
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500 /*ms*/
#endif

#define LV_TEXTAREA_DEF_WIDTH (2 * LV_DPI)
#define LV_TEXTAREA_DEF_HEIGHT (1 * LV_DPI)

#define LV_TEXTAREA_PWD_BULLET_UNICODE      0x2022

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_textarea_design(lv_obj_t * ta, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_design_res_t lv_textarea_scrollable_design(lv_obj_t * scrl, const lv_area_t * clip_area,
                                                     lv_design_mode_t mode);
static lv_res_t lv_textarea_signal(lv_obj_t * ta, lv_signal_t sign, void * param);
static lv_res_t lv_textarea_scrollable_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);
static lv_style_list_t * lv_textarea_get_style(lv_obj_t * ta, uint8_t part);
#if LV_USE_ANIMATION
    static void cursor_blink_anim(lv_obj_t * ta, lv_anim_value_t show);
    static void pwd_char_hider_anim(lv_obj_t * ta, lv_anim_value_t x);
    static void pwd_char_hider_anim_ready(lv_anim_t * a);
#endif
static void pwd_char_hider(lv_obj_t * ta);
static bool char_is_accepted(lv_obj_t * ta, uint32_t c);
static void refr_cursor_area(lv_obj_t * ta);
static void update_cursor_position_on_click(lv_obj_t * ta, lv_signal_t sign, lv_indev_t * click_source);
static lv_res_t insert_handler(lv_obj_t * ta, const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_cb_t ancestor_design;
static lv_design_cb_t scrl_design;
static lv_signal_cb_t ancestor_signal;
static lv_signal_cb_t scrl_signal;
static const char * ta_insert_replace;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a text area objects
 * @param par pointer to an object, it will be the parent of the new text area
 * @param copy pointer to a text area object, if not NULL then the new object will be copied from it
 * @return pointer to the created text area
 */
lv_obj_t * lv_textarea_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("text area create started");

    /*Create the ancestor object*/
    lv_obj_t * ta = lv_page_create(par, copy);
    LV_ASSERT_MEM(ta);
    if(ta == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(ta);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(ta);
    if(scrl_signal == NULL) scrl_signal = lv_obj_get_signal_cb(lv_page_get_scrollable(ta));
    if(scrl_design == NULL) scrl_design = lv_obj_get_design_cb(lv_page_get_scrollable(ta));

    /*Allocate the object type specific extended data*/
    lv_textarea_ext_t * ext = lv_obj_allocate_ext_attr(ta, sizeof(lv_textarea_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(ta);
        return NULL;
    }

    ext->pwd_mode          = 0;
    ext->pwd_tmp           = NULL;
    ext->pwd_show_time     = LV_TEXTAREA_DEF_PWD_SHOW_TIME;
    ext->accepted_chars    = NULL;
    ext->max_length        = 0;
    ext->cursor.state      = 1;
    ext->cursor.hidden     = 0;
    ext->cursor.blink_time = LV_TEXTAREA_DEF_CURSOR_BLINK_TIME;
    ext->cursor.pos        = 0;
    ext->cursor.click_pos  = 1;
    ext->cursor.valid_x    = 0;
    ext->one_line          = 0;
#if LV_LABEL_TEXT_SEL
    ext->text_sel_en = 0;
#endif
    ext->label       = NULL;
    ext->placeholder_txt = NULL;

    lv_style_list_init(&ext->cursor.style);
    lv_style_list_init(&ext->style_placeholder);

#if LV_USE_ANIMATION == 0
    ext->pwd_show_time     = 0;
    ext->cursor.blink_time = 0;
#endif

    lv_obj_set_signal_cb(ta, lv_textarea_signal);
    lv_obj_set_signal_cb(lv_page_get_scrollable(ta), lv_textarea_scrollable_signal);
    lv_obj_set_design_cb(ta, lv_textarea_design);

    /*Init the new text area object*/
    if(copy == NULL) {
        lv_page_set_scrollable_fit2(ta, LV_FIT_PARENT, LV_FIT_TIGHT);

        ext->label = lv_label_create(ta, NULL);

        lv_obj_set_design_cb(ext->page.scrl, lv_textarea_scrollable_design);

        lv_label_set_long_mode(ext->label, LV_LABEL_LONG_BREAK);
        lv_label_set_text(ext->label, "Text area");
        lv_obj_set_click(ext->label, false);
        lv_obj_set_size(ta, LV_TEXTAREA_DEF_WIDTH, LV_TEXTAREA_DEF_HEIGHT);
        lv_textarea_set_scrollbar_mode(ta, LV_SCROLLBAR_MODE_DRAG);

        lv_obj_reset_style_list(ta, LV_PAGE_PART_SCROLLABLE);
        lv_theme_apply(ta, LV_THEME_TEXTAREA);

    }
    /*Copy an existing object*/
    else {
        lv_obj_set_design_cb(ext->page.scrl, lv_textarea_scrollable_design);
        lv_textarea_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->label             = lv_label_create(ta, copy_ext->label);
        ext->pwd_mode          = copy_ext->pwd_mode;
        ext->accepted_chars    = copy_ext->accepted_chars;
        ext->max_length        = copy_ext->max_length;
        ext->cursor.pos        = copy_ext->cursor.pos;
        ext->cursor.valid_x    = copy_ext->cursor.valid_x;
        ext->cursor.hidden    = copy_ext->cursor.hidden;

        lv_style_list_copy(&ext->cursor.style, &copy_ext->cursor.style);
        lv_style_list_copy(&ext->style_placeholder, &copy_ext->style_placeholder);

        if(ext->pwd_mode != 0) pwd_char_hider(ta);

        if(copy_ext->placeholder_txt) {
            lv_textarea_set_placeholder_text(ta, copy_ext->placeholder_txt);
        }

        if(copy_ext->pwd_tmp) {
            uint32_t len = _lv_mem_get_size(copy_ext->pwd_tmp);
            ext->pwd_tmp = lv_mem_alloc(len);
            LV_ASSERT_MEM(ext->pwd_tmp);
            if(ext->pwd_tmp == NULL) return NULL;

            _lv_memcpy(ext->pwd_tmp, copy_ext->pwd_tmp, len);
        }

        if(copy_ext->one_line) lv_textarea_set_one_line(ta, true);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(ta, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

#if LV_USE_ANIMATION
    if(ext->cursor.blink_time) {
        /*Create a cursor blinker animation*/
        lv_anim_path_t path;
        lv_anim_path_init(&path);
        lv_anim_path_set_cb(&path, lv_anim_path_step);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, ta);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)cursor_blink_anim);
        lv_anim_set_time(&a, ext->cursor.blink_time);
        lv_anim_set_playback_time(&a, ext->cursor.blink_time);
        lv_anim_set_values(&a, 0, 1);
        lv_anim_set_path(&a, &path);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_start(&a);
    }
#endif

    LV_LOG_INFO("text area created");

    return ta;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Insert a character to the current cursor position.
 * To add a wide char, e.g. 'Á' use `_lv_txt_encoded_conv_wc('Á')`
 * @param ta pointer to a text area object
 * @param c a character (e.g. 'a')
 */
void lv_textarea_add_char(lv_obj_t * ta, uint32_t c)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    const char * letter_buf;

    uint32_t u32_buf[2];
    u32_buf[0] = c;
    u32_buf[1] = 0;

    letter_buf = (char *)&u32_buf;

#if LV_BIG_ENDIAN_SYSTEM
    if(c != 0) while(*letter_buf == 0) ++letter_buf;
#endif

    lv_res_t res = insert_handler(ta, letter_buf);
    if(res != LV_RES_OK) return;

    if(ext->one_line && (c == '\n' || c == '\r')) {
        LV_LOG_INFO("Text area: line break ignored in one-line mode");
        return;
    }

    uint32_t c_uni = _lv_txt_encoded_next((const char *)&c, NULL);

    if(char_is_accepted(ta, c_uni) == false) {
        LV_LOG_INFO("Character is no accepted by the text area (too long text or not in the "
                    "accepted list)");
        return;
    }

    /*If a new line was added it shouldn't show edge flash effect*/
    bool edge_flash_en = lv_textarea_get_edge_flash(ta);
    lv_textarea_set_edge_flash(ta, false);

    if(ext->pwd_mode != 0) pwd_char_hider(ta); /*Make sure all the current text contains only '*'*/

    /*If the textarea is empty, invalidate it to hide the placeholder*/
    if(ext->placeholder_txt) {
        const char * txt = lv_label_get_text(ext->label);
        if(txt[0] == '\0') lv_obj_invalidate(ta);
    }

    lv_label_ins_text(ext->label, ext->cursor.pos, letter_buf); /*Insert the character*/
    lv_textarea_clear_selection(ta);                                                /*Clear selection*/

    if(ext->pwd_mode != 0) {
        ext->pwd_tmp = lv_mem_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + strlen(letter_buf) + 1); /*+2: the new char + \0 */
        LV_ASSERT_MEM(ext->pwd_tmp);
        if(ext->pwd_tmp == NULL) return;

        _lv_txt_ins(ext->pwd_tmp, ext->cursor.pos, (const char *)letter_buf);

#if LV_USE_ANIMATION
        /*Auto hide characters*/
        if(ext->pwd_show_time == 0) {
            pwd_char_hider(ta);
        }
        else {
            lv_anim_path_t path;
            lv_anim_path_init(&path);
            lv_anim_path_set_cb(&path, lv_anim_path_step);

            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, ta);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)pwd_char_hider_anim);
            lv_anim_set_time(&a, ext->pwd_show_time);
            lv_anim_set_values(&a, 0, 1);
            lv_anim_set_path(&a, &path);
            lv_anim_set_ready_cb(&a, pwd_char_hider_anim_ready);
            lv_anim_start(&a);
        }

#else
        pwd_char_hider(ta);
#endif
    }

    /*Move the cursor after the new character*/
    lv_textarea_set_cursor_pos(ta, lv_textarea_get_cursor_pos(ta) + 1);

    /*Revert the original edge flash state*/
    lv_textarea_set_edge_flash(ta, edge_flash_en);

    lv_event_send(ta, LV_EVENT_VALUE_CHANGED, NULL);
}

/**
 * Insert a text to the current cursor position
 * @param ta pointer to a text area object
 * @param txt a '\0' terminated string to insert
 */
void lv_textarea_add_text(lv_obj_t * ta, const char * txt)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);
    LV_ASSERT_NULL(txt);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    if(ext->pwd_mode != 0) pwd_char_hider(ta); /*Make sure all the current text contains only '*'*/

    /*Add the character one-by-one if not all characters are accepted or there is character limit.*/
    if(lv_textarea_get_accepted_chars(ta) || lv_textarea_get_max_length(ta)) {
        uint32_t i = 0;
        while(txt[i] != '\0') {
            uint32_t c = _lv_txt_encoded_next(txt, &i);
            lv_textarea_add_char(ta, _lv_txt_unicode_to_encoded(c));
        }
        return;
    }

    lv_res_t res = insert_handler(ta, txt);
    if(res != LV_RES_OK) return;

    /*If a new line was added it shouldn't show edge flash effect*/
    bool edge_flash_en = lv_textarea_get_edge_flash(ta);
    lv_textarea_set_edge_flash(ta, false);

    /*If the textarea is empty, invalidate it to hide the placeholder*/
    if(ext->placeholder_txt) {
        const char * txt_act = lv_label_get_text(ext->label);
        if(txt_act[0] == '\0') lv_obj_invalidate(ta);
    }

    /*Insert the text*/
    lv_label_ins_text(ext->label, ext->cursor.pos, txt);
    lv_textarea_clear_selection(ta);

    if(ext->pwd_mode != 0) {
        ext->pwd_tmp = lv_mem_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + strlen(txt) + 1);
        LV_ASSERT_MEM(ext->pwd_tmp);
        if(ext->pwd_tmp == NULL) return;

        _lv_txt_ins(ext->pwd_tmp, ext->cursor.pos, txt);

#if LV_USE_ANIMATION
        /*Auto hide characters*/
        if(ext->pwd_show_time == 0) {
            pwd_char_hider(ta);
        }
        else {
            lv_anim_path_t path;
            lv_anim_path_init(&path);
            lv_anim_path_set_cb(&path, lv_anim_path_step);

            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, ta);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)pwd_char_hider_anim);
            lv_anim_set_time(&a, ext->pwd_show_time);
            lv_anim_set_values(&a, 0, 1);
            lv_anim_set_path(&a, &path);
            lv_anim_set_ready_cb(&a, pwd_char_hider_anim_ready);
            lv_anim_start(&a);
        }
#else
        pwd_char_hider(ta);
#endif
    }

    /*Move the cursor after the new text*/
    lv_textarea_set_cursor_pos(ta, lv_textarea_get_cursor_pos(ta) + _lv_txt_get_encoded_length(txt));

    /*Revert the original edge flash state*/
    lv_textarea_set_edge_flash(ta, edge_flash_en);

    lv_event_send(ta, LV_EVENT_VALUE_CHANGED, NULL);
}

/**
 * Delete a the left character from the current cursor position
 * @param ta pointer to a text area object
 */
void lv_textarea_del_char(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    uint32_t cur_pos  = ext->cursor.pos;

    if(cur_pos == 0) return;

    char del_buf[2]   = {LV_KEY_DEL, '\0'};

    lv_res_t res = insert_handler(ta, del_buf);
    if(res != LV_RES_OK) return;

    char * label_txt = lv_label_get_text(ext->label);

    /*Delete a character*/
    _lv_txt_cut(label_txt, ext->cursor.pos - 1, 1);
    /*Refresh the label*/
    lv_label_set_text(ext->label, label_txt);
    lv_textarea_clear_selection(ta);

    /*If the textarea became empty, invalidate it to hide the placeholder*/
    if(ext->placeholder_txt) {
        const char * txt = lv_label_get_text(ext->label);
        if(txt[0] == '\0') lv_obj_invalidate(ta);
    }

    /*Don't let 'width == 0' because cursor will not be visible*/
    if(lv_obj_get_width(ext->label) == 0) {
        lv_style_int_t border_width = lv_obj_get_style_border_width(ta, LV_TEXTAREA_PART_CURSOR);
        lv_obj_set_width(ext->label, border_width == 0 ? 1 : border_width);
    }

    if(ext->pwd_mode != 0) {
        uint32_t byte_pos = _lv_txt_encoded_get_byte_id(ext->pwd_tmp, ext->cursor.pos - 1);
        _lv_txt_cut(ext->pwd_tmp, ext->cursor.pos - 1, _lv_txt_encoded_size(&ext->pwd_tmp[byte_pos]));

        ext->pwd_tmp = lv_mem_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + 1);
        LV_ASSERT_MEM(ext->pwd_tmp);
        if(ext->pwd_tmp == NULL) return;
    }

    /*Move the cursor to the place of the deleted character*/
    lv_textarea_set_cursor_pos(ta, ext->cursor.pos - 1);

    lv_event_send(ta, LV_EVENT_VALUE_CHANGED, NULL);

}

/**
 * Delete the right character from the current cursor position
 * @param ta pointer to a text area object
 */
void lv_textarea_del_char_forward(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    uint32_t cp = lv_textarea_get_cursor_pos(ta);
    lv_textarea_set_cursor_pos(ta, cp + 1);
    if(cp != lv_textarea_get_cursor_pos(ta)) lv_textarea_del_char(ta);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void lv_textarea_set_text(lv_obj_t * ta, const char * txt)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);
    LV_ASSERT_NULL(txt);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    /*Clear the existing selection*/
    lv_textarea_clear_selection(ta);

    /*Add the character one-by-one if not all characters are accepted or there is character limit.*/
    if(lv_textarea_get_accepted_chars(ta) || lv_textarea_get_max_length(ta)) {
        lv_label_set_text(ext->label, "");
        lv_textarea_set_cursor_pos(ta, LV_TEXTAREA_CURSOR_LAST);
        if(ext->pwd_mode != 0) {
            ext->pwd_tmp[0] = '\0'; /*Clear the password too*/
        }
        uint32_t i = 0;
        while(txt[i] != '\0') {
            uint32_t c = _lv_txt_encoded_next(txt, &i);
            lv_textarea_add_char(ta, _lv_txt_unicode_to_encoded(c));
        }
    }
    else {
        lv_label_set_text(ext->label, txt);
        lv_textarea_set_cursor_pos(ta, LV_TEXTAREA_CURSOR_LAST);
    }

    /*If the textarea is empty, invalidate it to hide the placeholder*/
    if(ext->placeholder_txt) {
        const char * txt_act = lv_label_get_text(ext->label);
        if(txt_act[0] == '\0') lv_obj_invalidate(ta);
    }

    /*Don't let 'width == 0' because the cursor will not be visible*/
    if(lv_obj_get_width(ext->label) == 0) {
        lv_style_int_t border_width = lv_obj_get_style_border_width(ta, LV_TEXTAREA_PART_CURSOR);
        lv_obj_set_width(ext->label, border_width == 0 ? 1 : border_width);
    }

    if(ext->pwd_mode != 0) {
        ext->pwd_tmp = lv_mem_realloc(ext->pwd_tmp, strlen(txt) + 1);
        LV_ASSERT_MEM(ext->pwd_tmp);
        if(ext->pwd_tmp == NULL) return;
        strcpy(ext->pwd_tmp, txt);

#if LV_USE_ANIMATION
        /*Auto hide characters*/
        if(ext->pwd_show_time == 0) {
            pwd_char_hider(ta);
        }
        else {
            lv_anim_path_t path;
            lv_anim_path_init(&path);
            lv_anim_path_set_cb(&path, lv_anim_path_step);

            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, ta);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)pwd_char_hider_anim);
            lv_anim_set_time(&a, ext->pwd_show_time);
            lv_anim_set_values(&a, 0, 1);
            lv_anim_set_path(&a, &path);
            lv_anim_set_ready_cb(&a, pwd_char_hider_anim_ready);
            lv_anim_start(&a);
        }
#else
        pwd_char_hider(ta);
#endif
    }

    lv_event_send(ta, LV_EVENT_VALUE_CHANGED, NULL);
}

/**
 * Set the placeholder_txt text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void lv_textarea_set_placeholder_text(lv_obj_t * ta, const char * txt)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);
    LV_ASSERT_NULL(txt);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    size_t txt_len = strlen(txt);

    if(txt_len == 0) {
        if(ext->placeholder_txt) {
            lv_mem_free(ext->placeholder_txt);
            ext->placeholder_txt = NULL;
        }
    }
    else {

        /*Allocate memory for the placeholder_txt text*/
        if(ext->placeholder_txt == NULL) {
            ext->placeholder_txt = lv_mem_alloc(txt_len + 1);
        }
        else {
            ext->placeholder_txt = lv_mem_realloc(ext->placeholder_txt, txt_len + 1);

        }
        LV_ASSERT_MEM(ext->placeholder_txt);
        if(ext->placeholder_txt == NULL) {
            LV_LOG_ERROR("lv_textarea_set_placeholder_text: couldn't allocate memory for placeholder");
            return;
        }
        strcpy(ext->placeholder_txt, txt);
    }

    lv_obj_invalidate(ta);
}

/**
 * Set the cursor position
 * @param obj pointer to a text area object
 * @param pos the new cursor position in character index
 *             < 0 : index from the end of the text
 *             LV_TEXTAREA_CURSOR_LAST: go after the last character
 */
void lv_textarea_set_cursor_pos(lv_obj_t * ta, int32_t pos)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    if((uint32_t)ext->cursor.pos == (uint32_t)pos) return;

    uint32_t len = _lv_txt_get_encoded_length(lv_label_get_text(ext->label));

    if(pos < 0) pos = len + pos;

    if(pos > (int32_t)len || pos == LV_TEXTAREA_CURSOR_LAST) pos = len;

    ext->cursor.pos = pos;

    /*Position the label to make the cursor visible*/
    lv_obj_t * label_par = lv_obj_get_parent(ext->label);
    lv_point_t cur_pos;
    const lv_font_t * font = lv_obj_get_style_text_font(ta, LV_TEXTAREA_PART_BG);
    lv_style_int_t top = lv_obj_get_style_pad_top(ta, LV_TEXTAREA_PART_BG);
    lv_style_int_t bottom = lv_obj_get_style_pad_bottom(ta, LV_TEXTAREA_PART_BG);
    lv_area_t label_cords;
    lv_area_t ta_cords;
    lv_label_get_letter_pos(ext->label, pos, &cur_pos);
    lv_obj_get_coords(ta, &ta_cords);
    lv_obj_get_coords(ext->label, &label_cords);

    /*Check the top*/
    lv_coord_t font_h = lv_font_get_line_height(font);
    if(lv_obj_get_y(label_par) + cur_pos.y < 0) {
        lv_obj_set_y(label_par, -cur_pos.y + top);
    }

    /*Check the bottom*/
    if(label_cords.y1 + cur_pos.y + font_h + bottom > ta_cords.y2) {
        lv_obj_set_y(label_par, -(cur_pos.y - lv_obj_get_height(ta) + font_h + top + bottom));
    }
    /*Check the left (use the font_h as general unit)*/
    if(lv_obj_get_x(label_par) + cur_pos.x < font_h) {
        lv_obj_set_x(label_par, -cur_pos.x + font_h);
    }

    lv_style_int_t right = lv_obj_get_style_pad_right(ta, LV_TEXTAREA_PART_BG);
    lv_style_int_t left = lv_obj_get_style_pad_left(ta, LV_TEXTAREA_PART_BG);
    /*Check the right (use the font_h as general unit)*/
    if(label_cords.x1 + cur_pos.x + font_h + right > ta_cords.x2) {
        lv_obj_set_x(label_par, -(cur_pos.x - lv_obj_get_width(ta) + font_h + left + right));
    }

    ext->cursor.valid_x = cur_pos.x;

#if LV_USE_ANIMATION
    if(ext->cursor.blink_time) {
        /*Reset cursor blink animation*/
        lv_anim_path_t path;
        lv_anim_path_init(&path);
        lv_anim_path_set_cb(&path, lv_anim_path_step);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, ta);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)cursor_blink_anim);
        lv_anim_set_time(&a, ext->cursor.blink_time);
        lv_anim_set_playback_time(&a, ext->cursor.blink_time);
        lv_anim_set_values(&a, 1, 0);
        lv_anim_set_path(&a, &path);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_start(&a);
    }
#endif

    refr_cursor_area(ta);
}

/**
 * Hide/Unhide the cursor.
 * @param ta pointer to a text area object
 * @param hide: true: hide the cursor
 */
void lv_textarea_set_cursor_hidden(lv_obj_t * ta, bool hide)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    ext->cursor.hidden = hide ? 1 : 0;

    refr_cursor_area(ta);
}

/**
 * Enable/Disable the positioning of the cursor by clicking the text on the text area.
 * @param ta pointer to a text area object
 * @param en true: enable click positions; false: disable
 */
void lv_textarea_set_cursor_click_pos(lv_obj_t * ta, bool en)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext     = lv_obj_get_ext_attr(ta);
    ext->cursor.click_pos = en ? 1 : 0;
}

/**
 * Enable/Disable password mode
 * @param ta pointer to a text area object
 * @param en true: enable, false: disable
 */
void lv_textarea_set_pwd_mode(lv_obj_t * ta, bool en)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    if(ext->pwd_mode == en) return;

    ext->pwd_mode = en == false ? 0 : 1;
    /*Pwd mode is now enabled*/
    if(en != false) {
        char * txt   = lv_label_get_text(ext->label);
        size_t len = strlen(txt);
        ext->pwd_tmp = lv_mem_alloc(len + 1);
        LV_ASSERT_MEM(ext->pwd_tmp);
        if(ext->pwd_tmp == NULL) return;

        strcpy(ext->pwd_tmp, txt);

        pwd_char_hider(ta);

        lv_textarea_clear_selection(ta);
    }
    /*Pwd mode is now disabled*/
    else {
        lv_textarea_clear_selection(ta);
        lv_label_set_text(ext->label, ext->pwd_tmp);
        lv_mem_free(ext->pwd_tmp);
        ext->pwd_tmp = NULL;
    }

    refr_cursor_area(ta);
}

/**
 * Configure the text area to one line or back to normal
 * @param ta pointer to a Text area object
 * @param en true: one line, false: normal
 */
void lv_textarea_set_one_line(lv_obj_t * ta, bool en)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    if(ext->one_line == en) return;
    lv_label_align_t old_align = lv_label_get_align(ext->label);

    if(en) {
        lv_style_int_t top = lv_obj_get_style_pad_top(ta, LV_TEXTAREA_PART_BG);
        lv_style_int_t bottom = lv_obj_get_style_pad_bottom(ta, LV_TEXTAREA_PART_BG);
        lv_style_int_t left = lv_obj_get_style_pad_left(ta, LV_TEXTAREA_PART_BG);
        const lv_font_t * font = lv_obj_get_style_text_font(ta, LV_TEXTAREA_PART_BG);
        lv_coord_t font_h              = lv_font_get_line_height(font);

        ext->one_line = 1;
        lv_page_set_scrollable_fit2(ta, LV_FIT_MAX, LV_FIT_PARENT);
        lv_obj_set_height(ta, font_h + top + bottom);
        lv_label_set_long_mode(ext->label, LV_LABEL_LONG_EXPAND);
        lv_obj_set_pos(lv_page_get_scrollable(ta), left, top);
    }
    else {
        lv_style_int_t top = lv_obj_get_style_pad_top(ta, LV_TEXTAREA_PART_BG);
        lv_style_int_t left = lv_obj_get_style_pad_left(ta, LV_TEXTAREA_PART_BG);
        ext->one_line = 0;
        lv_page_set_scrollable_fit2(ta, LV_FIT_PARENT, LV_FIT_TIGHT);
        lv_label_set_long_mode(ext->label, LV_LABEL_LONG_BREAK);

        lv_obj_set_height(ta, LV_TEXTAREA_DEF_HEIGHT);
        lv_obj_set_pos(lv_page_get_scrollable(ta), left, top);
    }

    /* `refr_cursor_area` is called at the end of lv_ta_set_text_align */
    lv_textarea_set_text_align(ta, old_align);
}

/**
 * Set the alignment of the text area.
 * In one line mode the text can be scrolled only with `LV_LABEL_ALIGN_LEFT`.
 * This function should be called if the size of text area changes.
 * @param ta pointer to a text are object
 * @param align the desired alignment from `lv_label_align_t`. (LV_LABEL_ALIGN_LEFT/CENTER/RIGHT)
 */
void lv_textarea_set_text_align(lv_obj_t * ta, lv_label_align_t align)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    lv_obj_t * label  = lv_textarea_get_label(ta);
    if(!ext->one_line) {
        lv_label_set_align(label, align);
    }
    else {
        /*Normal left align. Just let the text expand*/
        if(align == LV_LABEL_ALIGN_LEFT) {
            lv_label_set_long_mode(label, LV_LABEL_LONG_EXPAND);
            lv_page_set_scrollable_fit2(ta, LV_FIT_MAX, LV_FIT_PARENT);
            lv_label_set_align(label, align);
        }
        /*Else use fix label width equal to the Text area width*/
        else {
            lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
            lv_obj_set_width(label, lv_page_get_width_fit(ta));
            lv_label_set_align(label, align);
            lv_page_set_scrollable_fit2(ta, LV_FIT_PARENT, LV_FIT_PARENT);
        }
    }

    refr_cursor_area(ta);
}

/**
 * Set a list of characters. Only these characters will be accepted by the text area
 * @param ta pointer to Text Area
 * @param list list of characters. Only the pointer is saved. E.g. "+-.,0123456789"
 */
void lv_textarea_set_accepted_chars(lv_obj_t * ta, const char * list)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    ext->accepted_chars = list;
}

/**
 * Set max length of a Text Area.
 * @param ta pointer to Text Area
 * @param num the maximal number of characters can be added (`lv_textarea_set_text` ignores it)
 */
void lv_textarea_set_max_length(lv_obj_t * ta, uint32_t num)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    ext->max_length = num;
}

/**
 * In `LV_EVENT_INSERT` the text which planned to be inserted can be replaced by an other text.
 * It can be used to add automatic formatting to the text area.
 * @param ta pointer to a text area.
 * @param txt pointer to a new string to insert. If `""` no text will be added.
 *            The variable must be live after the `event_cb` exists. (Should be `global` or
 * `static`)
 */
void lv_textarea_set_insert_replace(lv_obj_t * ta, const char * txt)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    (void)ta; /*Unused*/
    ta_insert_replace = txt;
}

/**
 * Enable/disable selection mode.
 * @param ta pointer to a text area object
 * @param en true or false to enable/disable selection mode
 */
void lv_textarea_set_text_sel(lv_obj_t * ta, bool en)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    ext->text_sel_en = en;

    if(!en) lv_textarea_clear_selection(ta);
#else
    (void)ta; /*Unused*/
    (void)en; /*Unused*/
#endif
}

/**
 * Set how long show the password before changing it to '*'
 * @param ta pointer to Text area
 * @param time show time in milliseconds. 0: hide immediately.
 */
void lv_textarea_set_pwd_show_time(lv_obj_t * ta, uint16_t time)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    time = 0;
#endif

    lv_textarea_ext_t * ext  = lv_obj_get_ext_attr(ta);
    ext->pwd_show_time = time;
}

/**
 * Set cursor blink animation time
 * @param ta pointer to Text area
 * @param time blink period. 0: disable blinking
 */
void lv_textarea_set_cursor_blink_time(lv_obj_t * ta, uint16_t time)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    time = 0;
#endif

    lv_textarea_ext_t * ext      = lv_obj_get_ext_attr(ta);
    ext->cursor.blink_time = time;

#if LV_USE_ANIMATION
    if(ext->cursor.blink_time) {
        /*Reset cursor blink animation*/
        lv_anim_path_t path;
        lv_anim_path_init(&path);
        lv_anim_path_set_cb(&path, lv_anim_path_step);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, ta);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)cursor_blink_anim);
        lv_anim_set_time(&a, ext->cursor.blink_time);
        lv_anim_set_playback_time(&a, ext->cursor.blink_time);
        lv_anim_set_values(&a, 1, 0);
        lv_anim_set_path(&a, &path);
        lv_anim_start(&a);
    }
    else {
        lv_anim_del(ta, (lv_anim_exec_xcb_t)cursor_blink_anim);
        ext->cursor.state = 1;
    }
#else
    ext->cursor.state = 1;
#endif
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a text area. In password mode it gives the real text (not '*'s).
 * @param ta pointer to a text area object
 * @return pointer to the text
 */
const char * lv_textarea_get_text(const lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    const char * txt;
    if(ext->pwd_mode == 0) {
        txt = lv_label_get_text(ext->label);
    }
    else {
        txt = ext->pwd_tmp;
    }

    return txt;
}

/**
 * Get the placeholder_txt text of a text area
 * @param ta pointer to a text area object
 * @return pointer to the text
 */
const char * lv_textarea_get_placeholder_text(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    if(ext->placeholder_txt) return ext->placeholder_txt;
    else return "";
}

/**
 * Get the label of a text area
 * @param ta pointer to a text area object
 * @return pointer to the label object
 */
lv_obj_t * lv_textarea_get_label(const lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->label;
}

/**
 * Get the current cursor position in character index
 * @param ta pointer to a text area object
 * @return the cursor position
 */
uint32_t lv_textarea_get_cursor_pos(const lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->cursor.pos;
}

/**
 * Get whether the cursor is hidden or not
 * @param ta pointer to a text area object
 * @return true: the cursor is hidden
 */
bool lv_textarea_get_cursor_hidden(const lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->cursor.hidden ? true : false;
}

/**
 * Get whether the cursor click positioning is enabled or not.
 * @param ta pointer to a text area object
 * @return true: enable click positions; false: disable
 */
bool lv_textarea_get_cursor_click_pos(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->cursor.click_pos ? true : false;
}

/**
 * Get the password mode attribute
 * @param ta pointer to a text area object
 * @return true: password mode is enabled, false: disabled
 */
bool lv_textarea_get_pwd_mode(const lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->pwd_mode == 0 ? false : true;
}

/**
 * Get the one line configuration attribute
 * @param ta pointer to a text area object
 * @return true: one line configuration is enabled, false: disabled
 */
bool lv_textarea_get_one_line(const lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->one_line == 0 ? false : true;
}

/**
 * Get a list of accepted characters.
 * @param ta pointer to Text Area
 * @return list of accented characters.
 */
const char * lv_textarea_get_accepted_chars(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    return ext->accepted_chars;
}

/**
 * Set max length of a Text Area.
 * @param ta pointer to Text Area
 * @return the maximal number of characters to be add
 */
uint32_t lv_textarea_get_max_length(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->max_length;
}

/**
 * Find whether text is selected or not.
 * @param ta Text area object
 * @return whether text is selected or not
 */
bool lv_textarea_text_is_selected(const lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    if((lv_label_get_text_sel_start(ext->label) == LV_DRAW_LABEL_NO_TXT_SEL ||
        lv_label_get_text_sel_end(ext->label) == LV_DRAW_LABEL_NO_TXT_SEL)) {
        return true;
    }
    else {
        return false;
    }
#else
    (void)ta; /*Unused*/
    return false;
#endif
}

/**
 * Find whether selection mode is enabled.
 * @param ta pointer to a text area object
 * @return true: selection mode is enabled, false: disabled
 */
bool lv_textarea_get_text_sel_en(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->text_sel_en;
#else
    (void)ta; /*Unused*/
    return false;
#endif
}

/**
 * Set how long show the password before changing it to '*'
 * @param ta pointer to Text area
 * @return show time in milliseconds. 0: hide immediately.
 */
uint16_t lv_textarea_get_pwd_show_time(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    return ext->pwd_show_time;
}

/**
 * Set cursor blink animation time
 * @param ta pointer to Text area
 * @return time blink period. 0: disable blinking
 */
uint16_t lv_textarea_get_cursor_blink_time(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->cursor.blink_time;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Clear the selection on the text area.
 * @param ta Text area object
 */
void lv_textarea_clear_selection(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    if(lv_label_get_text_sel_start(ext->label) != LV_DRAW_LABEL_NO_TXT_SEL ||
       lv_label_get_text_sel_end(ext->label) != LV_DRAW_LABEL_NO_TXT_SEL) {
        lv_label_set_text_sel_start(ext->label, LV_DRAW_LABEL_NO_TXT_SEL);
        lv_label_set_text_sel_end(ext->label, LV_DRAW_LABEL_NO_TXT_SEL);
    }
#else
    (void)ta; /*Unused*/
#endif
}

/**
 * Move the cursor one character right
 * @param ta pointer to a text area object
 */
void lv_textarea_cursor_right(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    uint32_t cp = lv_textarea_get_cursor_pos(ta);
    cp++;
    lv_textarea_set_cursor_pos(ta, cp);
}

/**
 * Move the cursor one character left
 * @param ta pointer to a text area object
 */
void lv_textarea_cursor_left(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    uint32_t cp = lv_textarea_get_cursor_pos(ta);
    if(cp > 0) {
        cp--;
        lv_textarea_set_cursor_pos(ta, cp);
    }
}

/**
 * Move the cursor one line down
 * @param ta pointer to a text area object
 */
void lv_textarea_cursor_down(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    lv_point_t pos;

    /*Get the position of the current letter*/
    lv_label_get_letter_pos(ext->label, lv_textarea_get_cursor_pos(ta), &pos);

    /*Increment the y with one line and keep the valid x*/

    lv_style_int_t line_space = lv_obj_get_style_text_line_space(ta, LV_TEXTAREA_PART_BG);
    const lv_font_t * font = lv_obj_get_style_text_font(ta, LV_TEXTAREA_PART_BG);
    lv_coord_t font_h              = lv_font_get_line_height(font);
    pos.y += font_h + line_space + 1;
    pos.x = ext->cursor.valid_x;

    /*Do not go below the last line*/
    if(pos.y < lv_obj_get_height(ext->label)) {
        /*Get the letter index on the new cursor position and set it*/
        uint32_t new_cur_pos = lv_label_get_letter_on(ext->label, &pos);

        lv_coord_t cur_valid_x_tmp = ext->cursor.valid_x; /*Cursor position set overwrites the valid position */
        lv_textarea_set_cursor_pos(ta, new_cur_pos);
        ext->cursor.valid_x = cur_valid_x_tmp;
    }
}

/**
 * Move the cursor one line up
 * @param ta pointer to a text area object
 */
void lv_textarea_cursor_up(lv_obj_t * ta)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    lv_point_t pos;

    /*Get the position of the current letter*/
    lv_label_get_letter_pos(ext->label, lv_textarea_get_cursor_pos(ta), &pos);

    /*Decrement the y with one line and keep the valid x*/
    lv_style_int_t line_space = lv_obj_get_style_text_line_space(ta, LV_TEXTAREA_PART_BG);
    const lv_font_t * font = lv_obj_get_style_text_font(ta, LV_TEXTAREA_PART_BG);
    lv_coord_t font_h              = lv_font_get_line_height(font);
    pos.y -= font_h + line_space - 1;
    pos.x = ext->cursor.valid_x;

    /*Get the letter index on the new cursor position and set it*/
    uint32_t new_cur_pos       = lv_label_get_letter_on(ext->label, &pos);
    lv_coord_t cur_valid_x_tmp = ext->cursor.valid_x; /*Cursor position set overwrites the valid position */
    lv_textarea_set_cursor_pos(ta, new_cur_pos);
    ext->cursor.valid_x = cur_valid_x_tmp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the text areas
 * @param ta pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_textarea_design(lv_obj_t * ta, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask_p area*/
        return ancestor_design(ta, clip_area, mode);
    }
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        /*Draw the object*/
        ancestor_design(ta, clip_area, mode);

    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(ta, clip_area, mode);
    }
    return LV_DESIGN_RES_OK;
}

/**
 * An extended scrollable design of the page. Calls the normal design function and draws a cursor.
 * @param scrl pointer to the scrollable part of the Text area
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @return return true/false, depends on 'mode'
 */
static lv_design_res_t lv_textarea_scrollable_design(lv_obj_t * scrl, const lv_area_t * clip_area,
                                                     lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask_p area*/
        return scrl_design(scrl, clip_area, mode);
    }
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        /*Draw the object*/
        scrl_design(scrl, clip_area, mode);
    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        scrl_design(scrl, clip_area, mode);

        lv_obj_t * ta     = lv_obj_get_parent(scrl);
        lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
        const char * txt = lv_label_get_text(ext->label);

        /*Draw the place holder*/
        if(txt[0] == '\0' && ext->placeholder_txt && ext->placeholder_txt[0] != 0) {
            lv_draw_label_dsc_t ph_dsc;
            lv_draw_label_dsc_init(&ph_dsc);
            lv_obj_init_draw_label_dsc(ta, LV_TEXTAREA_PART_PLACEHOLDER, &ph_dsc);
            switch(lv_label_get_align(ext->label)) {
                case LV_LABEL_ALIGN_CENTER:
                    ph_dsc.flag |= LV_TXT_FLAG_CENTER;
                    break;
                case LV_LABEL_ALIGN_RIGHT:
                    ph_dsc.flag |= LV_TXT_FLAG_RIGHT;
                    break;
                default:
                    break;
            }

            if(ext->one_line) ph_dsc.flag |= LV_TXT_FLAG_EXPAND;

            lv_draw_label(&scrl->coords, clip_area, &ph_dsc, ext->placeholder_txt, NULL);
        }

        /*Draw the cursor*/

        if(ext->cursor.hidden || ext->cursor.state == 0) {
            return LV_DESIGN_RES_OK; /*The cursor is not visible now*/
        }

        lv_draw_rect_dsc_t cur_dsc;
        lv_draw_rect_dsc_init(&cur_dsc);
        lv_obj_init_draw_rect_dsc(ta, LV_TEXTAREA_PART_CURSOR, &cur_dsc);

        /*Draw he cursor according to the type*/
        lv_area_t cur_area;
        lv_area_copy(&cur_area, &ext->cursor.area);

        cur_area.x1 += ext->label->coords.x1;
        cur_area.y1 += ext->label->coords.y1;
        cur_area.x2 += ext->label->coords.x1;
        cur_area.y2 += ext->label->coords.y1;

        lv_draw_rect(&cur_area, clip_area, &cur_dsc);

        char letter_buf[8] = {0};
        _lv_memcpy(letter_buf, &txt[ext->cursor.txt_byte_pos], _lv_txt_encoded_size(&txt[ext->cursor.txt_byte_pos]));

        if(cur_dsc.bg_opa == LV_OPA_COVER) {
            lv_style_int_t left = lv_obj_get_style_pad_left(ta, LV_TEXTAREA_PART_CURSOR);
            lv_style_int_t top = lv_obj_get_style_pad_top(ta, LV_TEXTAREA_PART_CURSOR);
            cur_area.x1 += left;
            cur_area.y1 += top;

            lv_draw_label_dsc_t cur_label_dsc;
            lv_draw_label_dsc_init(&cur_label_dsc);
            lv_obj_init_draw_label_dsc(ta, LV_TEXTAREA_PART_CURSOR, &cur_label_dsc);
            lv_draw_label(&cur_area, clip_area, &cur_label_dsc, letter_buf, NULL);
        }
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the text area
 * @param ta pointer to a text area object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_textarea_signal(lv_obj_t * ta, lv_signal_t sign, void * param)
{
    lv_res_t res;
    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_textarea_get_style(ta, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(ta, sign, param);
    }
    else if(sign == LV_SIGNAL_GET_STATE_DSC) {
        return ancestor_signal(ta, sign, param);
    }

    /* Include the ancient signal function */
    res = ancestor_signal(ta, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    if(sign == LV_SIGNAL_CLEANUP) {
        if(ext->pwd_tmp != NULL) lv_mem_free(ext->pwd_tmp);
        if(ext->placeholder_txt != NULL) lv_mem_free(ext->placeholder_txt);

        ext->pwd_tmp = NULL;
        ext->placeholder_txt = NULL;

        lv_obj_clean_style_list(ta, LV_TEXTAREA_PART_CURSOR);
        lv_obj_clean_style_list(ta, LV_TEXTAREA_PART_PLACEHOLDER);

        /* (The created label will be deleted automatically) */

    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        if(ext->label) {
            if(ext->one_line) {
                lv_style_int_t top = lv_obj_get_style_pad_top(ta, LV_TEXTAREA_PART_BG);
                lv_style_int_t bottom = lv_obj_get_style_pad_bottom(ta, LV_TEXTAREA_PART_BG);
                const lv_font_t * font = lv_obj_get_style_text_font(ta, LV_TEXTAREA_PART_BG);

                /*In one line mode refresh the Text Area height because 'vpad' can modify it*/
                lv_coord_t font_h              = lv_font_get_line_height(font);
                lv_obj_set_height(ext->label, font_h);
                lv_obj_set_height(ta, font_h + top + bottom);
            }
            else {
                /*In not one line mode refresh the Label width because 'hpad' can modify it*/
                lv_obj_set_width(ext->label, lv_page_get_width_fit(ta));
                lv_obj_set_pos(ext->label, 0, 0); /*Be sure the Label is in the correct position*/

            }
            lv_label_set_text(ext->label, NULL);
            refr_cursor_area(ta);
        }
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {
        /*Set the label width according to the text area width*/
        if(ext->label) {
            if(lv_obj_get_width(ta) != lv_area_get_width(param) || lv_obj_get_height(ta) != lv_area_get_height(param)) {
                lv_obj_set_width(ext->label, lv_page_get_width_fit(ta));
                lv_obj_set_pos(ext->label, 0, 0);
                lv_label_set_text(ext->label, NULL); /*Refresh the label*/

                refr_cursor_area(ta);
            }
        }
    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        uint32_t c = *((uint32_t *)param); /*uint32_t because can be UTF-8*/
        if(c == LV_KEY_RIGHT)
            lv_textarea_cursor_right(ta);
        else if(c == LV_KEY_LEFT)
            lv_textarea_cursor_left(ta);
        else if(c == LV_KEY_UP)
            lv_textarea_cursor_up(ta);
        else if(c == LV_KEY_DOWN)
            lv_textarea_cursor_down(ta);
        else if(c == LV_KEY_BACKSPACE)
            lv_textarea_del_char(ta);
        else if(c == LV_KEY_DEL)
            lv_textarea_del_char_forward(ta);
        else if(c == LV_KEY_HOME)
            lv_textarea_set_cursor_pos(ta, 0);
        else if(c == LV_KEY_END)
            lv_textarea_set_cursor_pos(ta, LV_TEXTAREA_CURSOR_LAST);
        else if(c == LV_KEY_ENTER && lv_textarea_get_one_line(ta))
            lv_event_send(ta, LV_EVENT_APPLY, NULL);
        else {
            lv_textarea_add_char(ta, c);
        }
#endif
    }
    else if(sign == LV_SIGNAL_GET_EDITABLE) {
#if LV_USE_GROUP
        bool * editable = (bool *)param;
        *editable       = true;
#endif
    }
    else if(sign == LV_SIGNAL_PRESSED || sign == LV_SIGNAL_PRESSING || sign == LV_SIGNAL_PRESS_LOST ||
            sign == LV_SIGNAL_RELEASED) {
        update_cursor_position_on_click(ta, sign, (lv_indev_t *)param);
    }
    return res;
}

/**
 * Signal function of the scrollable part of the text area
 * @param scrl pointer to scrollable part of a text area object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_textarea_scrollable_signal(lv_obj_t * scrl, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = scrl_signal(scrl, sign, param);

    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, "");

    lv_obj_t * ta     = lv_obj_get_parent(scrl);
    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        /*Set ext. size because the cursor might be out of this object*/
        lv_style_int_t line_space = lv_obj_get_style_text_line_space(ta, LV_TEXTAREA_PART_BG);
        const lv_font_t * font = lv_obj_get_style_text_font(ta, LV_TEXTAREA_PART_BG);
        lv_coord_t font_h              = lv_font_get_line_height(font);
        scrl->ext_draw_pad             = LV_MATH_MAX(scrl->ext_draw_pad, line_space + font_h);
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {
        /*Set the label width according to the text area width*/
        if(ext->label) {
            if(lv_obj_get_width(scrl) != lv_area_get_width(param) ||
               lv_obj_get_height(scrl) != lv_area_get_height(param)) {

                lv_obj_set_width(ext->label, lv_page_get_width_fit(ta));
                lv_obj_set_pos(ext->label, 0, 0);

                lv_label_set_text(ext->label, NULL); /*Refresh the label*/
                refr_cursor_area(ta);
            }

        }

    }
    else if(sign == LV_SIGNAL_PRESSING || sign == LV_SIGNAL_PRESSED || sign == LV_SIGNAL_PRESS_LOST ||
            sign == LV_SIGNAL_RELEASED) {
        update_cursor_position_on_click(ta, sign, (lv_indev_t *)param);
    }

    return res;
}

/**
 * Get the style descriptor of a part of the object
 * @param page pointer the object
 * @param part the part from `lv_textarea_part_t`. (LV_TEXTAREA_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_list_t * lv_textarea_get_style(lv_obj_t * ta, uint8_t part)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_TEXTAREA_PART_BG:
            style_dsc_p = &ta->style_list;
            break;
        case LV_TEXTAREA_PART_SCROLLBAR:
            style_dsc_p = &ext->page.scrlbar.style;
            break;
        case LV_TEXTAREA_PART_CURSOR:
            style_dsc_p = &ext->cursor.style;
            break;
#if LV_USE_ANIMATION
        case LV_TEXTAREA_PART_EDGE_FLASH:
            style_dsc_p = &ext->page.edge_flash.style;
            break;
#endif
        case LV_TEXTAREA_PART_PLACEHOLDER:
            style_dsc_p = &ext->style_placeholder;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}

#if LV_USE_ANIMATION

/**
 * Called to blink the cursor
 * @param ta pointer to a text area
 * @param hide 1: hide the cursor, 0: show it
 */
static void cursor_blink_anim(lv_obj_t * ta, lv_anim_value_t show)
{
    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    if(show != ext->cursor.state) {
        ext->cursor.state = show == 0 ? 0 : 1;
        if(ext->cursor.hidden == 0) {
            lv_area_t area_tmp;
            lv_area_copy(&area_tmp, &ext->cursor.area);
            area_tmp.x1 += ext->label->coords.x1;
            area_tmp.y1 += ext->label->coords.y1;
            area_tmp.x2 += ext->label->coords.x1;
            area_tmp.y2 += ext->label->coords.y1;
            lv_obj_invalidate_area(ta, &area_tmp);
        }
    }
}

/**
 * Dummy function to animate char hiding in pwd mode.
 * Does nothing, but a function is required in car hiding anim.
 * (pwd_char_hider callback do the real job)
 * @param ta unused
 * @param x unused
 */
static void pwd_char_hider_anim(lv_obj_t * ta, lv_anim_value_t x)
{
    (void)ta;
    (void)x;
}

/**
 * Call when an animation is ready to convert all characters to '*'
 * @param a pointer to the animation
 */
static void pwd_char_hider_anim_ready(lv_anim_t * a)
{
    lv_obj_t * ta = a->var;
    pwd_char_hider(ta);
}
#endif

/**
 * Hide all characters (convert them to '*')
 * @param ta: pointer to text area object
 */
static void pwd_char_hider(lv_obj_t * ta)
{
    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    if(ext->pwd_mode != 0) {
        char * txt  = lv_label_get_text(ext->label);
        int32_t enc_len = _lv_txt_get_encoded_length(txt);
        if(enc_len == 0) return;

        /*If the textarea's font has "bullet" character use it else fallback to "*"*/
        const lv_font_t * font = lv_obj_get_style_text_font(ta, LV_TEXTAREA_PART_BG);
        lv_font_glyph_dsc_t g;
        bool has_bullet;
        has_bullet = lv_font_get_glyph_dsc(font, &g, LV_TEXTAREA_PWD_BULLET_UNICODE, 0);
        const char * bullet;
        if(has_bullet) bullet = LV_SYMBOL_BULLET;
        else bullet = "*";

        size_t bullet_len = strlen(bullet);
        char * txt_tmp = _lv_mem_buf_get(enc_len * bullet_len + 1);
        int32_t i;
        for(i = 0; i < enc_len; i++) {
            _lv_memcpy(&txt_tmp[i * bullet_len], bullet, bullet_len);
        }

        txt_tmp[i * bullet_len] = '\0';

        lv_label_set_text(ext->label, txt_tmp);
        _lv_mem_buf_release(txt_tmp);
        refr_cursor_area(ta);
    }
}

/**
 * Test an unicode character if it is accepted or not. Checks max length and accepted char list.
 * @param ta pointer to a test area object
 * @param c an unicode character
 * @return true: accepted; false: rejected
 */
static bool char_is_accepted(lv_obj_t * ta, uint32_t c)
{
    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    /*If no restriction accept it*/
    if(ext->accepted_chars == NULL && ext->max_length == 0) return true;

    /*Too many characters?*/
    if(ext->max_length > 0 && _lv_txt_get_encoded_length(lv_textarea_get_text(ta)) >= ext->max_length) {
        return false;
    }

    /*Accepted character?*/
    if(ext->accepted_chars) {
        uint32_t i = 0;

        while(ext->accepted_chars[i] != '\0') {
            uint32_t a = _lv_txt_encoded_next(ext->accepted_chars, &i);
            if(a == c) return true; /*Accepted*/
        }

        return false; /*The character wasn't in the list*/
    }
    else {
        return true; /*If the accepted char list in not specified the accept the character*/
    }
}

static void refr_cursor_area(lv_obj_t * ta)
{
    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);

    const lv_font_t * font = lv_obj_get_style_text_font(ta, LV_TEXTAREA_PART_BG);
    lv_style_int_t line_space = lv_obj_get_style_text_line_space(ta, LV_TEXTAREA_PART_BG);

    uint32_t cur_pos = lv_textarea_get_cursor_pos(ta);
    const char * txt = lv_label_get_text(ext->label);

    uint32_t byte_pos;
    byte_pos = _lv_txt_encoded_get_byte_id(txt, cur_pos);

    uint32_t letter = _lv_txt_encoded_next(&txt[byte_pos], NULL);

    lv_coord_t letter_h = lv_font_get_line_height(font);

    /*Set letter_w (set not 0 on non printable but valid chars)*/
    lv_coord_t letter_w;
    if(letter == '\0' || letter == '\n' || letter == '\r') {
        letter_w = lv_font_get_glyph_width(font, ' ', '\0');
    }
    else {
        /*`letter_next` parameter is '\0' to ignore kerning*/
        letter_w = lv_font_get_glyph_width(font, letter, '\0');
    }

    lv_point_t letter_pos;
    lv_label_get_letter_pos(ext->label, cur_pos, &letter_pos);

    /*If the cursor is out of the text (most right) draw it to the next line*/
    if(letter_pos.x + ext->label->coords.x1 + letter_w > ext->label->coords.x2 && ext->one_line == 0 &&
       lv_label_get_align(ext->label) != LV_LABEL_ALIGN_RIGHT) {
        letter_pos.x = 0;
        letter_pos.y += letter_h + line_space;

        if(letter != '\0') {
            byte_pos += _lv_txt_encoded_size(&txt[byte_pos]);
            letter = _lv_txt_encoded_next(&txt[byte_pos], NULL);
        }

        if(letter == '\0' || letter == '\n' || letter == '\r') {
            letter_w = lv_font_get_glyph_width(font, ' ', '\0');
        }
        else {
            letter_w = lv_font_get_glyph_width(font, letter, '\0');
        }
    }

    /*Save the byte position. It is required to draw `LV_CURSOR_BLOCK`*/
    ext->cursor.txt_byte_pos = byte_pos;

    /*Calculate the cursor according to its type*/
    lv_style_int_t top = lv_obj_get_style_pad_top(ta, LV_TEXTAREA_PART_CURSOR);
    lv_style_int_t bottom = lv_obj_get_style_pad_bottom(ta, LV_TEXTAREA_PART_CURSOR);
    lv_style_int_t left = lv_obj_get_style_pad_left(ta, LV_TEXTAREA_PART_CURSOR);
    lv_style_int_t right = lv_obj_get_style_pad_right(ta, LV_TEXTAREA_PART_CURSOR);

    lv_area_t cur_area;
    cur_area.x1 = letter_pos.x - left;
    cur_area.y1 = letter_pos.y - top;
    cur_area.x2 = letter_pos.x + right + letter_w;
    cur_area.y2 = letter_pos.y + bottom + letter_h;

    /*Save the new area*/
    lv_area_t area_tmp;
    lv_area_copy(&area_tmp, &ext->cursor.area);
    area_tmp.x1 += ext->label->coords.x1;
    area_tmp.y1 += ext->label->coords.y1;
    area_tmp.x2 += ext->label->coords.x1;
    area_tmp.y2 += ext->label->coords.y1;
    lv_obj_invalidate_area(ta, &area_tmp);

    lv_area_copy(&ext->cursor.area, &cur_area);

    lv_area_copy(&area_tmp, &ext->cursor.area);
    area_tmp.x1 += ext->label->coords.x1;
    area_tmp.y1 += ext->label->coords.y1;
    area_tmp.x2 += ext->label->coords.x1;
    area_tmp.y2 += ext->label->coords.y1;
    lv_obj_invalidate_area(ta, &area_tmp);
}

static void update_cursor_position_on_click(lv_obj_t * ta, lv_signal_t sign, lv_indev_t * click_source)
{

    if(click_source == NULL) return;

    lv_textarea_ext_t * ext = lv_obj_get_ext_attr(ta);
    if(ext->cursor.click_pos == 0) return;
    if(ext->cursor.hidden) return;

    if(lv_indev_get_type(click_source) == LV_INDEV_TYPE_KEYPAD ||
       lv_indev_get_type(click_source) == LV_INDEV_TYPE_ENCODER) {
        return;
    }

    lv_area_t label_coords;
    lv_obj_get_coords(ext->label, &label_coords);

    lv_point_t point_act, vect_act;
    lv_indev_get_point(click_source, &point_act);
    lv_indev_get_vect(click_source, &vect_act);

    if(point_act.x < 0 || point_act.y < 0) return; /*Ignore event from keypad*/
    lv_point_t rel_pos;
    rel_pos.x = point_act.x - label_coords.x1;
    rel_pos.y = point_act.y - label_coords.y1;

    lv_coord_t label_width = lv_obj_get_width(ext->label);

    uint16_t char_id_at_click;

#if LV_LABEL_TEXT_SEL
    lv_label_ext_t * ext_label = lv_obj_get_ext_attr(ext->label);
    bool click_outside_label;
    /*Check if the click happened on the left side of the area outside the label*/
    if(rel_pos.x < 0) {
        char_id_at_click = 0;
        click_outside_label       = true;
    }
    /*Check if the click happened on the right side of the area outside the label*/
    else if(rel_pos.x >= label_width) {
        char_id_at_click = LV_TEXTAREA_CURSOR_LAST;
        click_outside_label       = true;
    }
    else {
        char_id_at_click = lv_label_get_letter_on(ext->label, &rel_pos);
        click_outside_label       = !lv_label_is_char_under_pos(ext->label, &rel_pos);
    }

    if(ext->text_sel_en) {
        if(!ext->text_sel_in_prog && !click_outside_label && sign == LV_SIGNAL_PRESSED) {
            /*Input device just went down. Store the selection start position*/
            ext->sel_start    = char_id_at_click;
            ext->sel_end      = LV_LABEL_TEXT_SEL_OFF;
            ext->text_sel_in_prog = 1;
            lv_obj_set_drag(lv_page_get_scrollable(ta), false);
        }
        else if(ext->text_sel_in_prog && sign == LV_SIGNAL_PRESSING) {
            /*Input device may be moving. Store the end position */
            ext->sel_end = char_id_at_click;
        }
        else if(ext->text_sel_in_prog && (sign == LV_SIGNAL_PRESS_LOST || sign == LV_SIGNAL_RELEASED)) {
            /*Input device is released. Check if anything was selected.*/
            lv_obj_set_drag(lv_page_get_scrollable(ta), true);
        }
    }

    if(ext->text_sel_in_prog || sign == LV_SIGNAL_PRESSED) lv_textarea_set_cursor_pos(ta, char_id_at_click);

    if(ext->text_sel_in_prog) {
        /*If the selected area has changed then update the real values and*/

        /*Invalidate the text area.*/
        if(ext->sel_start > ext->sel_end) {
            if(ext_label->sel_start != ext->sel_end || ext_label->sel_end != ext->sel_start) {
                ext_label->sel_start = ext->sel_end;
                ext_label->sel_end   = ext->sel_start;
                lv_obj_invalidate(ta);
            }
        }
        else if(ext->sel_start < ext->sel_end) {
            if(ext_label->sel_start != ext->sel_start || ext_label->sel_end != ext->sel_end) {
                ext_label->sel_start = ext->sel_start;
                ext_label->sel_end   = ext->sel_end;
                lv_obj_invalidate(ta);
            }
        }
        else {
            if(ext_label->sel_start != LV_DRAW_LABEL_NO_TXT_SEL || ext_label->sel_end != LV_DRAW_LABEL_NO_TXT_SEL) {
                ext_label->sel_start = LV_DRAW_LABEL_NO_TXT_SEL;
                ext_label->sel_end   = LV_DRAW_LABEL_NO_TXT_SEL;
                lv_obj_invalidate(ta);
            }
        }
        /*Finish selection if necessary */
        if(sign == LV_SIGNAL_PRESS_LOST || sign == LV_SIGNAL_RELEASED) {
            ext->text_sel_in_prog = 0;
        }
    }
#else
    /*Check if the click happened on the left side of the area outside the label*/
    if(rel_pos.x < 0) {
        char_id_at_click = 0;
    }
    /*Check if the click happened on the right side of the area outside the label*/
    else if(rel_pos.x >= label_width) {
        char_id_at_click = LV_TEXTAREA_CURSOR_LAST;
    }
    else {
        char_id_at_click = lv_label_get_letter_on(ext->label, &rel_pos);
    }

    if(sign == LV_SIGNAL_PRESSED) lv_textarea_set_cursor_pos(ta, char_id_at_click);
#endif
}

static lv_res_t insert_handler(lv_obj_t * ta, const char * txt)
{
    ta_insert_replace = NULL;
    lv_event_send(ta, LV_EVENT_INSERT, txt);
    if(ta_insert_replace) {
        if(ta_insert_replace[0] == '\0') return LV_RES_INV; /*Drop this text*/

        /*Add the replaced text directly it's different from the original*/
        if(strcmp(ta_insert_replace, txt)) {
            lv_textarea_add_text(ta, ta_insert_replace);
            return LV_RES_INV;
        }
    }

    return LV_RES_OK;
}

#endif
