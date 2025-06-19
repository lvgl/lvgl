/**
 * @file lv_textarea.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_textarea_private.h"
#include "../label/lv_label_private.h"
#include "../../core/lv_obj_class_private.h"
#if LV_USE_TEXTAREA != 0

#include "../../core/lv_group.h"
#include "../../core/lv_refr.h"
#include "../../indev/lv_indev.h"
#include "../../draw/lv_draw.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_anim_private.h"
#include "../../misc/lv_text_private.h"
#include "../../misc/lv_math.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_textarea_class)

/*Test configuration*/
#ifndef LV_TEXTAREA_DEF_CURSOR_BLINK_TIME
    #define LV_TEXTAREA_DEF_CURSOR_BLINK_TIME 400 /*ms*/
#endif

#ifndef LV_TEXTAREA_DEF_PWD_SHOW_TIME
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500 /*ms*/
#endif

#define LV_TEXTAREA_PWD_BULLET_UNICODE      0x2022
#define IGNORE_KERNING                      '\0'

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_textarea_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_textarea_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_textarea_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void label_event_cb(lv_event_t * e);
static void cursor_blink_anim_cb(void * obj, int32_t show);
static void pwd_char_hider_anim(void * obj, int32_t x);
static void pwd_char_hider_anim_completed(lv_anim_t * a);
static void pwd_char_hider(lv_obj_t * obj);
static bool char_is_accepted(lv_obj_t * obj, uint32_t c);
static void start_cursor_blink(lv_obj_t * obj);
static void refr_cursor_area(lv_obj_t * obj);
static void update_cursor_position_on_click(lv_event_t * e);
static lv_result_t insert_handler(lv_obj_t * obj, const char * txt);
static void draw_placeholder(lv_event_t * e);
static void draw_cursor(lv_event_t * e);
static void auto_hide_characters(lv_obj_t * obj);
static void auto_hide_characters_cancel(lv_obj_t * obj);
static inline bool is_valid_but_non_printable_char(const uint32_t letter);

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_OBJ_PROPERTY
static const lv_property_ops_t properties[] = {
    {
        .id = LV_PROPERTY_TEXTAREA_TEXT,
        .setter = lv_textarea_set_text,
        .getter = lv_textarea_get_text,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_PLACEHOLDER_TEXT,
        .setter = lv_textarea_set_placeholder_text,
        .getter = lv_textarea_get_placeholder_text,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_CURSOR_POS,
        .setter = lv_textarea_set_cursor_pos,
        .getter = lv_textarea_get_cursor_pos,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_CURSOR_CLICK_POS,
        .setter = lv_textarea_set_cursor_click_pos,
        .getter = lv_textarea_get_cursor_click_pos,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_PASSWORD_MODE,
        .setter = lv_textarea_set_password_mode,
        .getter = lv_textarea_get_password_mode,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_PASSWORD_BULLET,
        .setter = lv_textarea_set_password_bullet,
        .getter = lv_textarea_get_password_bullet,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_ONE_LINE,
        .setter = lv_textarea_set_one_line,
        .getter = lv_textarea_get_one_line,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_ACCEPTED_CHARS,
        .setter = lv_textarea_set_accepted_chars,
        .getter = lv_textarea_get_accepted_chars,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_MAX_LENGTH,
        .setter = lv_textarea_set_max_length,
        .getter = lv_textarea_get_max_length,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_INSERT_REPLACE,
        .setter = lv_textarea_set_insert_replace,
        .getter = NULL,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_TEXT_SELECTION,
        .setter = lv_textarea_set_text_selection,
        .getter = lv_textarea_get_text_selection,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_PASSWORD_SHOW_TIME,
        .setter = lv_textarea_set_password_show_time,
        .getter = lv_textarea_get_password_show_time,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_LABEL,
        .setter = NULL,
        .getter = lv_textarea_get_label,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_TEXT_IS_SELECTED,
        .setter = NULL,
        .getter = lv_textarea_text_is_selected,
    },
    {
        .id = LV_PROPERTY_TEXTAREA_CURRENT_CHAR,
        .setter = NULL,
        .getter = lv_textarea_get_current_char,
    },
};
#endif

const lv_obj_class_t lv_textarea_class = {
    .constructor_cb = lv_textarea_constructor,
    .destructor_cb = lv_textarea_destructor,
    .event_cb = lv_textarea_event,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .width_def = LV_DPI_DEF * 2,
    .height_def = LV_DPI_DEF,
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .instance_size = sizeof(lv_textarea_t),
    .base_class = &lv_obj_class,
    .name = "lv_textarea",
#if LV_USE_OBJ_PROPERTY
    .prop_index_start = LV_PROPERTY_TEXTAREA_START,
    .prop_index_end = LV_PROPERTY_TEXTAREA_END,
    .properties = properties,
    .properties_count = sizeof(properties) / sizeof(properties[0]),

#if LV_USE_OBJ_PROPERTY_NAME
    .property_names = lv_textarea_property_names,
    .names_count = sizeof(lv_textarea_property_names) / sizeof(lv_property_name_t),
#endif

#endif
};

static const char * ta_insert_replace;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_textarea_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*======================
 * Add/remove functions
 *=====================*/

void lv_textarea_add_char(lv_obj_t * obj, uint32_t c)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    if(ta->one_line && (c == '\n' || c == '\r')) {
        LV_LOG_INFO("Text area: line break ignored in one-line mode");
        return;
    }

    uint32_t u32_buf[2];
    u32_buf[0] = c;
    u32_buf[1] = 0;

    const char * letter_buf = (char *)&u32_buf;

    uint32_t c2 = c;
#if LV_BIG_ENDIAN_SYSTEM
    if(c != 0) while(*letter_buf == 0) ++letter_buf;

    /*The byte order may or may not need to be swapped here to get correct c_uni below,
      since lv_textarea_add_text is ordering bytes correctly before calling lv_textarea_add_char.
      Assume swapping is needed if MSB is zero. May not be foolproof. */
    if((c != 0) && ((c & 0xff000000) == 0)) {
        c2 = ((c >> 24) & 0xff) | /*move byte 3 to byte 0*/
             ((c << 8) & 0xff0000) | /*move byte 1 to byte 2*/
             ((c >> 8) & 0xff00) | /*move byte 2 to byte 1*/
             ((c << 24) & 0xff000000); /*byte 0 to byte 3*/
    }
#endif

    lv_result_t res = insert_handler(obj, letter_buf);
    if(res != LV_RESULT_OK) return;

    uint32_t c_uni = lv_text_encoded_next((const char *)&c2, NULL);

    if(char_is_accepted(obj, c_uni) == false) {
        LV_LOG_INFO("Character is not accepted by the text area (too long text or not in the accepted list)");
        return;
    }

    if(ta->pwd_mode) pwd_char_hider(obj); /*Make sure all the current text contains only '*'*/

    /*If the textarea is empty, invalidate it to hide the placeholder*/
    if(ta->placeholder_txt) {
        const char * txt = lv_label_get_text(ta->label);
        if(txt[0] == '\0') lv_obj_invalidate(obj);
    }

    lv_label_ins_text(ta->label, ta->cursor.pos, letter_buf); /*Insert the character*/
    lv_textarea_clear_selection(obj); /*Clear selection*/

    if(ta->pwd_mode) {
        /*+2: the new char + \0*/
        size_t realloc_size = lv_strlen(ta->pwd_tmp) + lv_strlen(letter_buf) + 1;
        ta->pwd_tmp = lv_realloc(ta->pwd_tmp, realloc_size);
        LV_ASSERT_MALLOC(ta->pwd_tmp);
        if(ta->pwd_tmp == NULL) return;

        lv_text_ins(ta->pwd_tmp, ta->cursor.pos, (const char *)letter_buf);

        /*Auto hide characters*/
        auto_hide_characters(obj);
    }

    /*Move the cursor after the new character*/
    lv_textarea_set_cursor_pos(obj, lv_textarea_get_cursor_pos(obj) + 1);

    lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
}

void lv_textarea_add_text(lv_obj_t * obj, const char * txt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(txt);

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    if(ta->pwd_mode) pwd_char_hider(obj); /*Make sure all the current text contains only '*'*/

    /*Add the character one-by-one if not all characters are accepted or there is character limit.*/
    if(lv_textarea_get_accepted_chars(obj) || lv_textarea_get_max_length(obj)) {
        uint32_t i = 0;
        while(txt[i] != '\0') {
            uint32_t c = lv_text_encoded_next(txt, &i);
            lv_textarea_add_char(obj, lv_text_unicode_to_encoded(c));
        }
        return;
    }

    lv_result_t res = insert_handler(obj, txt);
    if(res != LV_RESULT_OK) return;

    /*If the textarea is empty, invalidate it to hide the placeholder*/
    if(ta->placeholder_txt) {
        const char * txt_act = lv_label_get_text(ta->label);
        if(txt_act[0] == '\0') lv_obj_invalidate(obj);
    }

    /*Insert the text*/
    lv_label_ins_text(ta->label, ta->cursor.pos, txt);
    lv_textarea_clear_selection(obj);

    if(ta->pwd_mode) {
        size_t realloc_size = lv_strlen(ta->pwd_tmp) + lv_strlen(txt) + 1;
        ta->pwd_tmp = lv_realloc(ta->pwd_tmp, realloc_size);
        LV_ASSERT_MALLOC(ta->pwd_tmp);
        if(ta->pwd_tmp == NULL) return;

        lv_text_ins(ta->pwd_tmp, ta->cursor.pos, txt);

        /*Auto hide characters*/
        auto_hide_characters(obj);
    }

    /*Move the cursor after the new text*/
    lv_textarea_set_cursor_pos(obj, lv_textarea_get_cursor_pos(obj) + lv_text_get_encoded_length(txt));

    lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
}

void lv_textarea_delete_char(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    uint32_t cur_pos  = ta->cursor.pos;

    if(cur_pos == 0) return;

    char del_buf[2]   = {LV_KEY_DEL, '\0'};

    lv_result_t res = insert_handler(obj, del_buf);
    if(res != LV_RESULT_OK) return;

    char * label_txt = lv_label_get_text(ta->label);

    /*Delete a character*/
    lv_text_cut(label_txt, ta->cursor.pos - 1, 1);

    /*Refresh the label*/
    lv_label_set_text(ta->label, label_txt);
    lv_textarea_clear_selection(obj);

    /*If the textarea became empty, invalidate it to hide the placeholder*/
    if(ta->placeholder_txt) {
        const char * txt = lv_label_get_text(ta->label);
        if(txt[0] == '\0') lv_obj_invalidate(obj);
    }

    if(ta->pwd_mode) {
        lv_text_cut(ta->pwd_tmp, ta->cursor.pos - 1, 1);

        ta->pwd_tmp = lv_realloc(ta->pwd_tmp, lv_strlen(ta->pwd_tmp) + 1);
        LV_ASSERT_MALLOC(ta->pwd_tmp);
        if(ta->pwd_tmp == NULL) return;
    }

    /*Move the cursor to the place of the deleted character*/
    lv_textarea_set_cursor_pos(obj, ta->cursor.pos - 1);

    lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);

}

void lv_textarea_delete_char_forward(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    uint32_t cp = lv_textarea_get_cursor_pos(obj);
    lv_textarea_set_cursor_pos(obj, cp + 1);
    if(cp != lv_textarea_get_cursor_pos(obj)) lv_textarea_delete_char(obj);
}

/*=====================
 * Setter functions
 *====================*/

void lv_textarea_set_text(lv_obj_t * obj, const char * txt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(txt);

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    /*Clear the existing selection*/
    lv_textarea_clear_selection(obj);

    /*Add the character one-by-one if not all characters are accepted or there is character limit.*/
    if(lv_textarea_get_accepted_chars(obj) || lv_textarea_get_max_length(obj)) {
        lv_label_set_text(ta->label, "");
        lv_textarea_set_cursor_pos(obj, LV_TEXTAREA_CURSOR_LAST);
        if(ta->pwd_mode) {
            ta->pwd_tmp[0] = '\0'; /*Clear the password too*/
        }
        uint32_t i = 0;
        while(txt[i] != '\0') {
            uint32_t c = lv_text_encoded_next(txt, &i);
            lv_textarea_add_char(obj, lv_text_unicode_to_encoded(c));
        }
    }
    else {
        lv_label_set_text(ta->label, txt);
        lv_textarea_set_cursor_pos(obj, LV_TEXTAREA_CURSOR_LAST);
    }

    /*If the textarea is empty, invalidate it to hide the placeholder*/
    if(ta->placeholder_txt) {
        const char * txt_act = lv_label_get_text(ta->label);
        if(txt_act[0] == '\0') lv_obj_invalidate(obj);
    }

    if(ta->pwd_mode) {
        lv_free(ta->pwd_tmp);
        ta->pwd_tmp = lv_strdup(txt);
        LV_ASSERT_MALLOC(ta->pwd_tmp);
        if(ta->pwd_tmp == NULL) return;

        pwd_char_hider(obj);
    }

    lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
}

void lv_textarea_set_placeholder_text(lv_obj_t * obj, const char * txt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(txt);

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    size_t txt_len = lv_strlen(txt);
    if((txt_len == 0) && (ta->placeholder_txt)) {
        lv_free(ta->placeholder_txt);
        ta->placeholder_txt = NULL;
    }
    else {
        /*Allocate memory for the placeholder_txt text*/
        /*NOTE: Using special realloc behavior, malloc-like when data_p is NULL*/
        ta->placeholder_txt = lv_realloc(ta->placeholder_txt, txt_len + 1);
        LV_ASSERT_MALLOC(ta->placeholder_txt);
        if(ta->placeholder_txt == NULL) {
            LV_LOG_ERROR("couldn't allocate memory for placeholder");
            return;
        }

        lv_strcpy(ta->placeholder_txt, txt);
        ta->placeholder_txt[txt_len] = '\0';
    }

    lv_obj_invalidate(obj);
}

void lv_textarea_set_cursor_pos(lv_obj_t * obj, int32_t pos)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    if((uint32_t)ta->cursor.pos == (uint32_t)pos) return;

    uint32_t len = lv_text_get_encoded_length(lv_label_get_text(ta->label));

    if(pos < 0) pos = len + pos;

    if(pos > (int32_t)len || pos == LV_TEXTAREA_CURSOR_LAST) pos = len;

    ta->cursor.pos = pos;

    /*Position the label to make the cursor visible*/
    lv_obj_update_layout(obj);

    lv_point_t cur_pos;
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    lv_label_get_letter_pos(ta->label, pos, &cur_pos);

    /*The text area needs to have it's final size to see if the cursor is out of the area or not*/

    /*Check the top*/
    int32_t font_h = lv_font_get_line_height(font);
    if(cur_pos.y < lv_obj_get_scroll_top(obj)) {
        lv_obj_scroll_to_y(obj, cur_pos.y, LV_ANIM_ON);
    }
    /*Check the bottom*/
    int32_t h = lv_obj_get_content_height(obj);
    if(cur_pos.y + font_h - lv_obj_get_scroll_top(obj) > h) {
        lv_obj_scroll_to_y(obj, cur_pos.y - h + font_h, LV_ANIM_ON);
    }

    /*Check the left*/
    if(cur_pos.x < lv_obj_get_scroll_left(obj)) {
        lv_obj_scroll_to_x(obj, cur_pos.x, LV_ANIM_ON);
    }
    /*Check the right*/
    int32_t w = lv_obj_get_content_width(obj);
    if(cur_pos.x + font_h - lv_obj_get_scroll_left(obj) > w) {
        lv_obj_scroll_to_x(obj, cur_pos.x - w + font_h, LV_ANIM_ON);
    }

    ta->cursor.valid_x = cur_pos.x;

    start_cursor_blink(obj);

    refr_cursor_area(obj);
}

void lv_textarea_set_cursor_click_pos(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    ta->cursor.click_pos = en ? 1U : 0U;
}

void lv_textarea_set_password_mode(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    if(ta->pwd_mode == en) return;

    ta->pwd_mode = en ? 1U : 0U;
    /*Pwd mode is now enabled*/
    if(en) {
        char * txt = lv_label_get_text(ta->label);
        lv_free(ta->pwd_tmp);
        ta->pwd_tmp = lv_strdup(txt);
        LV_ASSERT_MALLOC(ta->pwd_tmp);
        if(ta->pwd_tmp == NULL) return;

        pwd_char_hider(obj);

        lv_textarea_clear_selection(obj);
    }
    /*Pwd mode is now disabled*/
    else {
        lv_textarea_clear_selection(obj);
        lv_label_set_text(ta->label, ta->pwd_tmp);
        lv_free(ta->pwd_tmp);
        ta->pwd_tmp = NULL;
    }

    refr_cursor_area(obj);
}

void lv_textarea_set_password_bullet(lv_obj_t * obj, const char * bullet)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(bullet);

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    if(!bullet && (ta->pwd_bullet)) {
        lv_free(ta->pwd_bullet);
        ta->pwd_bullet = NULL;
    }
    else {
        size_t txt_len = lv_strlen(bullet);

        /*Allocate memory for the pwd_bullet text*/
        /*NOTE: Using special realloc behavior, malloc-like when data_p is NULL*/
        ta->pwd_bullet = lv_realloc(ta->pwd_bullet, txt_len + 1);
        LV_ASSERT_MALLOC(ta->pwd_bullet);
        if(ta->pwd_bullet == NULL) {
            LV_LOG_ERROR("couldn't allocate memory for bullet");
            return;
        }

        lv_memcpy(ta->pwd_bullet, bullet, txt_len);
        ta->pwd_bullet[txt_len] = '\0';
    }

    pwd_char_hider(obj);
}

void lv_textarea_set_one_line(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    if(ta->one_line == en) return;

    ta->one_line = en ? 1U : 0U;
    int32_t width = en ? LV_SIZE_CONTENT : lv_pct(100);
    int32_t min_width_value = en ? lv_pct(100) : 0;

    lv_obj_set_width(ta->label, width);
    lv_obj_set_style_min_width(ta->label, min_width_value, 0);

    if(en) {
        lv_obj_set_height(obj, LV_SIZE_CONTENT);
    }
    else {
        lv_obj_remove_local_style_prop(obj, LV_STYLE_HEIGHT, LV_PART_MAIN);
    }

    lv_obj_scroll_to(obj, 0, 0, LV_ANIM_OFF);
}

void lv_textarea_set_accepted_chars(lv_obj_t * obj, const char * list)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    ta->accepted_chars = list;
}

void lv_textarea_set_max_length(lv_obj_t * obj, uint32_t num)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    ta->max_length = num;
}

void lv_textarea_set_insert_replace(lv_obj_t * obj, const char * txt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    LV_UNUSED(obj);
    ta_insert_replace = txt;
}

void lv_textarea_set_text_selection(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

#if LV_LABEL_TEXT_SELECTION
    lv_textarea_t * ta = (lv_textarea_t *)obj;

    ta->text_sel_en = en;

    if(!en) lv_textarea_clear_selection(obj);
#else
    LV_UNUSED(obj); /*Unused*/
    LV_UNUSED(en);  /*Unused*/
#endif
}

void lv_textarea_set_password_show_time(lv_obj_t * obj, uint32_t time)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    ta->pwd_show_time = time;
    pwd_char_hider(obj);
}

void lv_textarea_set_align(lv_obj_t * obj, lv_text_align_t align)
{
    LV_LOG_WARN("Deprecated: use the normal text_align style property instead");
    lv_obj_set_style_text_align(obj, align, 0);

    switch(align) {
        default:
        case LV_TEXT_ALIGN_LEFT:
            lv_obj_align(lv_textarea_get_label(obj), LV_ALIGN_TOP_LEFT, 0, 0);
            break;
        case LV_TEXT_ALIGN_RIGHT:
            lv_obj_align(lv_textarea_get_label(obj), LV_ALIGN_TOP_RIGHT, 0, 0);
            break;
        case LV_TEXT_ALIGN_CENTER:
            lv_obj_align(lv_textarea_get_label(obj), LV_ALIGN_TOP_MID, 0, 0);
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

const char * lv_textarea_get_text(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    const char * txt;
    if(ta->pwd_mode == 0) {
        txt = lv_label_get_text(ta->label);
    }
    else {
        txt = ta->pwd_tmp;
    }

    return txt;
}

const char * lv_textarea_get_placeholder_text(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    if(ta->placeholder_txt) return ta->placeholder_txt;
    else return "";
}

lv_obj_t * lv_textarea_get_label(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    return ta->label;
}

uint32_t lv_textarea_get_cursor_pos(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    return ta->cursor.pos;
}

bool lv_textarea_get_cursor_click_pos(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    return ta->cursor.click_pos;
}

bool lv_textarea_get_password_mode(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    return ta->pwd_mode == 1U;
}

const char * lv_textarea_get_password_bullet(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    if(ta->pwd_bullet) return ta->pwd_bullet;

    lv_font_glyph_dsc_t g;
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);

    /*If the textarea's font has the bullet character use it else fallback to "*"*/
    if(lv_font_get_glyph_dsc(font, &g, LV_TEXTAREA_PWD_BULLET_UNICODE, 0))
        return LV_SYMBOL_BULLET;
    return "*";
}

bool lv_textarea_get_one_line(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    return ta->one_line == 1U;
}

const char * lv_textarea_get_accepted_chars(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    return ta->accepted_chars;
}

uint32_t lv_textarea_get_max_length(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    return ta->max_length;
}

bool lv_textarea_text_is_selected(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

#if LV_LABEL_TEXT_SELECTION
    lv_textarea_t * ta = (lv_textarea_t *)obj;

    if((lv_label_get_text_selection_start(ta->label) != LV_DRAW_LABEL_NO_TXT_SEL ||
        lv_label_get_text_selection_end(ta->label) != LV_DRAW_LABEL_NO_TXT_SEL)) {
        return true;
    }
    else {
        return false;
    }
#else
    LV_UNUSED(obj); /*Unused*/
    return false;
#endif
}

bool lv_textarea_get_text_selection(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

#if LV_LABEL_TEXT_SELECTION
    lv_textarea_t * ta = (lv_textarea_t *)obj;
    return ta->text_sel_en;
#else
    LV_UNUSED(obj); /*Unused*/
    return false;
#endif
}

uint32_t lv_textarea_get_password_show_time(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    return ta->pwd_show_time;
}

uint32_t lv_textarea_get_current_char(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    const char * txt = lv_textarea_get_text(obj);
    lv_textarea_t * ta = (lv_textarea_t *)obj;
    uint32_t pos = ta->cursor.pos;
    if(lv_text_get_encoded_length(txt) >= pos && pos > 0)
        return lv_text_encoded_prev(txt, &pos);
    else
        return 0;
}

/*=====================
 * Other functions
 *====================*/

void lv_textarea_clear_selection(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

#if LV_LABEL_TEXT_SELECTION
    lv_textarea_t * ta = (lv_textarea_t *)obj;

    if(lv_label_get_text_selection_start(ta->label) != LV_DRAW_LABEL_NO_TXT_SEL ||
       lv_label_get_text_selection_end(ta->label) != LV_DRAW_LABEL_NO_TXT_SEL) {
        lv_label_set_text_selection_start(ta->label, LV_DRAW_LABEL_NO_TXT_SEL);
        lv_label_set_text_selection_end(ta->label, LV_DRAW_LABEL_NO_TXT_SEL);
    }
#else
    LV_UNUSED(obj); /*Unused*/
#endif
}

void lv_textarea_cursor_right(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    uint32_t cp = lv_textarea_get_cursor_pos(obj);
    cp++;
    lv_textarea_set_cursor_pos(obj, cp);
}

void lv_textarea_cursor_left(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    uint32_t cp = lv_textarea_get_cursor_pos(obj);
    if(cp > 0) {
        cp--;
        lv_textarea_set_cursor_pos(obj, cp);
    }
}

void lv_textarea_cursor_down(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    lv_point_t pos;

    /*Get the position of the current letter*/
    lv_label_get_letter_pos(ta->label, lv_textarea_get_cursor_pos(obj), &pos);

    /*Increment the y with one line and keep the valid x*/

    int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    int32_t font_h              = lv_font_get_line_height(font);
    pos.y += font_h + line_space + 1;
    pos.x = ta->cursor.valid_x;

    /*Do not go below the last line*/
    if(pos.y < lv_obj_get_height(ta->label)) {
        /*Get the letter index on the new cursor position and set it*/
        uint32_t new_cur_pos = lv_label_get_letter_on(ta->label, &pos, true);

        int32_t cur_valid_x_tmp = ta->cursor.valid_x; /*Cursor position set overwrites the valid position*/
        lv_textarea_set_cursor_pos(obj, new_cur_pos);
        ta->cursor.valid_x = cur_valid_x_tmp;
    }
}

void lv_textarea_cursor_up(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    lv_point_t pos;

    /*Get the position of the current letter*/
    lv_label_get_letter_pos(ta->label, lv_textarea_get_cursor_pos(obj), &pos);

    /*Decrement the y with one line and keep the valid x*/
    int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    int32_t font_h              = lv_font_get_line_height(font);
    pos.y -= font_h + line_space - 1;
    pos.x = ta->cursor.valid_x;

    /*Get the letter index on the new cursor position and set it*/
    uint32_t new_cur_pos       = lv_label_get_letter_on(ta->label, &pos, true);
    int32_t cur_valid_x_tmp = ta->cursor.valid_x; /*Cursor position set overwrites the valid position*/
    lv_textarea_set_cursor_pos(obj, new_cur_pos);
    ta->cursor.valid_x = cur_valid_x_tmp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_textarea_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_textarea_t * ta = (lv_textarea_t *)obj;

    ta->pwd_mode          = 0;
    ta->pwd_tmp           = NULL;
    ta->pwd_bullet        = NULL;
    ta->pwd_show_time     = LV_TEXTAREA_DEF_PWD_SHOW_TIME;
    ta->accepted_chars    = NULL;
    ta->max_length        = 0;
    ta->cursor.show      = 1;
    /*It will be set to zero later (with zero value lv_textarea_set_cursor_pos(obj, 0); wouldn't do anything as there is no difference)*/
    ta->cursor.pos        = 1;
    ta->cursor.click_pos  = 1;
    ta->cursor.valid_x    = 0;
    ta->one_line          = 0;
#if LV_LABEL_TEXT_SELECTION
    ta->text_sel_en = 0;
#endif
    ta->label       = NULL;
    ta->placeholder_txt = NULL;

    ta->label = lv_label_create(obj);
    lv_obj_set_width(ta->label, lv_pct(100));
    lv_label_set_text(ta->label, "");
    lv_obj_add_event_cb(ta->label, label_event_cb, LV_EVENT_STYLE_CHANGED, NULL);
    lv_obj_add_event_cb(ta->label, label_event_cb, LV_EVENT_SIZE_CHANGED, NULL);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);

    lv_textarea_set_cursor_pos(obj, 0);

    start_cursor_blink(obj);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_textarea_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_textarea_t * ta = (lv_textarea_t *)obj;
    if(ta->pwd_tmp != NULL) {
        lv_free(ta->pwd_tmp);
        ta->pwd_tmp = NULL;
    }
    if(ta->pwd_bullet != NULL) {
        lv_free(ta->pwd_bullet);
        ta->pwd_bullet = NULL;
    }
    if(ta->placeholder_txt != NULL) {
        lv_free(ta->placeholder_txt);
        ta->placeholder_txt = NULL;
    }
}

static void lv_textarea_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;
    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    if(code == LV_EVENT_FOCUSED) {
        start_cursor_blink(obj);
    }
    else if(code == LV_EVENT_KEY) {
        uint32_t c = *((uint32_t *)lv_event_get_param(e)); /*uint32_t because can be UTF-8*/
        if(c == LV_KEY_RIGHT)
            lv_textarea_cursor_right(obj);
        else if(c == LV_KEY_LEFT)
            lv_textarea_cursor_left(obj);
        else if(c == LV_KEY_UP)
            lv_textarea_cursor_up(obj);
        else if(c == LV_KEY_DOWN)
            lv_textarea_cursor_down(obj);
        else if(c == LV_KEY_BACKSPACE)
            lv_textarea_delete_char(obj);
        else if(c == LV_KEY_DEL)
            lv_textarea_delete_char_forward(obj);
        else if(c == LV_KEY_HOME)
            lv_textarea_set_cursor_pos(obj, 0);
        else if(c == LV_KEY_END)
            lv_textarea_set_cursor_pos(obj, LV_TEXTAREA_CURSOR_LAST);
        else if(c == LV_KEY_ENTER && lv_textarea_get_one_line(obj))
            lv_obj_send_event(obj, LV_EVENT_READY, NULL);
        else {
            lv_textarea_add_char(obj, c);
        }
    }
    else if(code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING || code == LV_EVENT_PRESS_LOST ||
            code == LV_EVENT_RELEASED) {
        update_cursor_position_on_click(e);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_placeholder(e);
    }
    else if(code == LV_EVENT_DRAW_POST) {
        draw_cursor(e);
    }
}

static void label_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * label = lv_event_get_current_target(e);
    lv_obj_t * ta = lv_obj_get_parent(label);

    if(code == LV_EVENT_STYLE_CHANGED || code == LV_EVENT_SIZE_CHANGED) {
        lv_label_set_text(label, NULL);
        refr_cursor_area(ta);
        start_cursor_blink(ta);
    }
}

/**
 * Called to blink the cursor
 * @param ta pointer to a text area
 * @param hide 1: hide the cursor, 0: show it
 */
static void cursor_blink_anim_cb(void * obj, int32_t show)
{
    lv_textarea_t * ta = (lv_textarea_t *)obj;
    if(show != ta->cursor.show) {
        ta->cursor.show = show ? 1U : 0U;
        lv_area_t area_tmp;
        lv_area_copy(&area_tmp, &ta->cursor.area);
        area_tmp.x1 += ta->label->coords.x1;
        area_tmp.y1 += ta->label->coords.y1;
        area_tmp.x2 += ta->label->coords.x1;
        area_tmp.y2 += ta->label->coords.y1;
        lv_obj_invalidate_area(obj, &area_tmp);
    }
}

/**
 * Dummy function to animate char hiding in pwd mode.
 * Does nothing, but a function is required in car hiding anim.
 * (pwd_char_hider callback do the real job)
 * @param ta unused
 * @param x unused
 */
static void pwd_char_hider_anim(void * obj, int32_t x)
{
    LV_UNUSED(obj);
    LV_UNUSED(x);
}

/**
 * Call when an animation is ready to convert all characters to '*'
 * @param a pointer to the animation
 */
static void pwd_char_hider_anim_completed(lv_anim_t * a)
{
    lv_obj_t * obj = a->var;
    pwd_char_hider(obj);
}

/**
 * Hide all characters (convert them to '*')
 * @param ta pointer to text area object
 */
static void pwd_char_hider(lv_obj_t * obj)
{
    lv_textarea_t * ta = (lv_textarea_t *)obj;
    if(ta->pwd_mode == 0) {
        return;
    }

    /* When ta->label is empty we get 0 back */
    char * txt = lv_label_get_text(ta->label);
    uint32_t enc_len = lv_text_get_encoded_length(txt);
    if(enc_len == 0) return;

    const char * bullet = lv_textarea_get_password_bullet(obj);
    const size_t bullet_len = lv_strlen(bullet);
    char * txt_tmp = lv_malloc(enc_len * bullet_len + 1);

    uint32_t i;
    for(i = 0; i < enc_len; i++) {
        lv_memcpy(&txt_tmp[i * bullet_len], bullet, bullet_len);
    }
    txt_tmp[i * bullet_len] = '\0';

    lv_label_set_text(ta->label, txt_tmp);
    lv_free(txt_tmp);

    auto_hide_characters_cancel(obj);

    refr_cursor_area(obj);
}

/**
 * Test a unicode character if it is accepted or not. Checks max length and accepted char list.
 * @param ta pointer to a test area object
 * @param c a unicode character
 * @return true: accepted; false: rejected
 */
static bool char_is_accepted(lv_obj_t * obj, uint32_t c)
{
    lv_textarea_t * ta = (lv_textarea_t *)obj;

    /*Too many characters?*/
    if(ta->max_length > 0 && lv_text_get_encoded_length(lv_textarea_get_text(obj)) >= ta->max_length) {
        return false;
    }

    if(ta->accepted_chars == NULL || ta->accepted_chars[0] == '\0') return true;
    /*Accepted character?*/
    uint32_t i = 0;

    while(ta->accepted_chars[i] != '\0') {
        uint32_t a = lv_text_encoded_next(ta->accepted_chars, &i);
        if(a == c) return true; /*Accepted*/
    }

    return false; /*The character wasn't in the list*/
}

static void start_cursor_blink(lv_obj_t * obj)
{
    lv_textarea_t * ta = (lv_textarea_t *)obj;
    uint32_t blink_time = lv_obj_get_style_anim_duration(obj, LV_PART_CURSOR);
    if(blink_time == 0) {
        lv_anim_delete(obj, cursor_blink_anim_cb);
        ta->cursor.show = 1;
    }
    else {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, ta);
        lv_anim_set_exec_cb(&a, cursor_blink_anim_cb);
        lv_anim_set_duration(&a, blink_time);
        lv_anim_set_reverse_duration(&a, blink_time);
        lv_anim_set_values(&a, 1, 0);
        lv_anim_set_path_cb(&a, lv_anim_path_step);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_start(&a);
    }
}

static void refr_cursor_area(lv_obj_t * obj)
{
    lv_textarea_t * ta = (lv_textarea_t *)obj;

    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);

    uint32_t cur_pos = lv_textarea_get_cursor_pos(obj);
    const char * txt = lv_label_get_text(ta->label);

    uint32_t byte_pos = lv_text_encoded_get_byte_id(txt, cur_pos);
    uint32_t letter = lv_text_encoded_next(&txt[byte_pos], NULL);

    /* Letter height and width */
    const int32_t letter_h = lv_font_get_line_height(font);
    /*Set letter_w (set not 0 on non printable but valid chars)*/
    uint32_t letter_space = letter;
    if(is_valid_but_non_printable_char(letter)) {
        letter_space = ' ';
    }
    int32_t letter_w = lv_font_get_glyph_width(font, letter_space, IGNORE_KERNING);

    lv_point_t letter_pos;
    lv_label_get_letter_pos(ta->label, cur_pos, &letter_pos);

    lv_text_align_t align = lv_obj_calculate_style_text_align(ta->label, LV_PART_MAIN, lv_label_get_text(ta->label));

    /*If the cursor is out of the text (most right) draw it to the next line*/
    if(((letter_pos.x + ta->label->coords.x1) + letter_w > ta->label->coords.x2) &&
       (ta->one_line == 0 && align != LV_TEXT_ALIGN_RIGHT)) {

        letter_pos.x = 0;
        letter_pos.y += letter_h + line_space;

        if(letter != '\0') {
            byte_pos += lv_text_encoded_size(&txt[byte_pos]);
            letter = lv_text_encoded_next(&txt[byte_pos], NULL);
        }

        uint32_t tmp = letter;
        if(is_valid_but_non_printable_char(letter)) {
            tmp = ' ';
        }
        letter_w = lv_font_get_glyph_width(font, tmp, IGNORE_KERNING);
    }

    /*Save the byte position. It is required to draw `LV_CURSOR_BLOCK`*/
    ta->cursor.txt_byte_pos = byte_pos;

    /*Calculate the cursor according to its type*/
    int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_CURSOR);
    int32_t top = lv_obj_get_style_pad_top(obj, LV_PART_CURSOR) + border_width;
    int32_t bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_CURSOR) + border_width;
    int32_t left = lv_obj_get_style_pad_left(obj, LV_PART_CURSOR) + border_width;
    int32_t right = lv_obj_get_style_pad_right(obj, LV_PART_CURSOR) + border_width;

    lv_area_t cur_area;
    cur_area.x1 = letter_pos.x - left;
    cur_area.y1 = letter_pos.y - top;
    cur_area.x2 = letter_pos.x + right + letter_w - 1;
    cur_area.y2 = letter_pos.y + bottom + letter_h - 1;

    /*Save the new area*/
    lv_area_t area_tmp;
    lv_area_copy(&area_tmp, &ta->cursor.area);
    area_tmp.x1 += ta->label->coords.x1;
    area_tmp.y1 += ta->label->coords.y1;
    area_tmp.x2 += ta->label->coords.x1;
    area_tmp.y2 += ta->label->coords.y1;
    lv_obj_invalidate_area(obj, &area_tmp);

    lv_area_copy(&ta->cursor.area, &cur_area);

    lv_area_copy(&area_tmp, &ta->cursor.area);
    area_tmp.x1 += ta->label->coords.x1;
    area_tmp.y1 += ta->label->coords.y1;
    area_tmp.x2 += ta->label->coords.x1;
    area_tmp.y2 += ta->label->coords.y1;
    lv_obj_invalidate_area(obj, &area_tmp);
}

static void update_cursor_position_on_click(lv_event_t * e)
{
    lv_indev_t * click_source = lv_indev_active();
    if(click_source == NULL) return;

    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_textarea_t * ta = (lv_textarea_t *)obj;
    if(ta->cursor.click_pos == 0) return;

    if(lv_indev_get_type(click_source) == LV_INDEV_TYPE_KEYPAD ||
       lv_indev_get_type(click_source) == LV_INDEV_TYPE_ENCODER) {
        return;
    }

    lv_area_t label_coords;
    lv_obj_get_coords(ta->label, &label_coords);

    lv_point_t point_act, vect_act;
    lv_indev_get_point(click_source, &point_act);
    lv_indev_get_vect(click_source, &vect_act);

    if(point_act.x < 0 || point_act.y < 0) return; /*Ignore event from keypad*/
    lv_point_t rel_pos;
    rel_pos.x = point_act.x - label_coords.x1;
    rel_pos.y = point_act.y - label_coords.y1;

    const lv_event_code_t code = lv_event_get_code(e);

    int32_t label_width = lv_obj_get_width(ta->label);
    uint32_t char_id_at_click = 0;

#if LV_LABEL_TEXT_SELECTION
    lv_label_t * label_data = (lv_label_t *)ta->label;
    bool click_outside_label = false;
    /*Check if the click happened on the left side of the area outside the label*/
    if(rel_pos.x < 0) {
        char_id_at_click = 0;
        click_outside_label = true;
    }
    /*Check if the click happened on the right side of the area outside the label*/
    else if(rel_pos.x >= label_width) {
        char_id_at_click = LV_TEXTAREA_CURSOR_LAST;
        click_outside_label = true;
    }
    else {
        char_id_at_click = lv_label_get_letter_on(ta->label, &rel_pos, true);
        click_outside_label = !lv_label_is_char_under_pos(ta->label, &rel_pos);
    }

    if(ta->text_sel_en) {
        if(!ta->text_sel_in_prog && !click_outside_label && code == LV_EVENT_PRESSED) {
            /*Input device just went down. Store the selection start position*/
            ta->sel_start    = char_id_at_click;
            ta->sel_end      = LV_LABEL_TEXT_SELECTION_OFF;
            ta->text_sel_in_prog = 1;
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
        }
        else if(ta->text_sel_in_prog && code == LV_EVENT_PRESSING) {
            /*Input device may be moving. Store the end position*/
            ta->sel_end = char_id_at_click;
        }
        else if(ta->text_sel_in_prog && (code == LV_EVENT_PRESS_LOST || code == LV_EVENT_RELEASED)) {
            /*Input device is released. Check if anything was selected.*/
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
        }
    }

    if(ta->text_sel_in_prog || code == LV_EVENT_PRESSED) lv_textarea_set_cursor_pos(obj, char_id_at_click);

    if(ta->text_sel_in_prog) {
        /*If the selected area has changed then update the real values and*/

        /*Invalidate the text area.*/
        if(ta->sel_start > ta->sel_end) {
            if(label_data->sel_start != ta->sel_end || label_data->sel_end != ta->sel_start) {
                label_data->sel_start = ta->sel_end;
                label_data->sel_end   = ta->sel_start;
                lv_obj_invalidate(obj);
            }
        }
        else if(ta->sel_start < ta->sel_end) {
            if(label_data->sel_start != ta->sel_start || label_data->sel_end != ta->sel_end) {
                label_data->sel_start = ta->sel_start;
                label_data->sel_end   = ta->sel_end;
                lv_obj_invalidate(obj);
            }
        }
        else {
            if(label_data->sel_start != LV_DRAW_LABEL_NO_TXT_SEL || label_data->sel_end != LV_DRAW_LABEL_NO_TXT_SEL) {
                label_data->sel_start = LV_DRAW_LABEL_NO_TXT_SEL;
                label_data->sel_end   = LV_DRAW_LABEL_NO_TXT_SEL;
                lv_obj_invalidate(obj);
            }
        }
        /*Finish selection if necessary*/
        if(code == LV_EVENT_PRESS_LOST || code == LV_EVENT_RELEASED) {
            ta->text_sel_in_prog = 0;
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
        char_id_at_click = lv_label_get_letter_on(ta->label, &rel_pos, true);
    }

    if(code == LV_EVENT_PRESSED) lv_textarea_set_cursor_pos(obj, char_id_at_click);
#endif
}

/* Returns LV_RESULT_OK when no operation were performed
 * Returns LV_RESULT_INVALID when a user defined text was inserted */
static lv_result_t insert_handler(lv_obj_t * obj, const char * txt)
{
    ta_insert_replace = NULL;
    lv_obj_send_event(obj, LV_EVENT_INSERT, (char *)txt);

    /* Drop txt if insert replace is set to '\0' */
    if(ta_insert_replace && ta_insert_replace[0] == '\0')
        return LV_RESULT_INVALID;

    if(ta_insert_replace) {
        /*Add the replaced text directly it's different from the original*/
        if(lv_strcmp(ta_insert_replace, txt)) {
            lv_textarea_add_text(obj, ta_insert_replace);
            return LV_RESULT_INVALID;
        }
    }

    return LV_RESULT_OK;
}

static void draw_placeholder(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_textarea_t * ta = (lv_textarea_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(e);
    const char * txt = lv_label_get_text(ta->label);

    /*Draw the place holder*/
    if(txt[0] == '\0' && ta->placeholder_txt && ta->placeholder_txt[0] != 0) {
        lv_draw_label_dsc_t ph_dsc;
        lv_draw_label_dsc_init(&ph_dsc);
        ph_dsc.base.layer = layer;
        lv_obj_init_draw_label_dsc(obj, LV_PART_TEXTAREA_PLACEHOLDER, &ph_dsc);

        if(ta->one_line) ph_dsc.flag |= LV_TEXT_FLAG_EXPAND;

        int32_t left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        int32_t right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
        int32_t top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
        int32_t bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
        int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
        lv_area_t ph_coords;
        lv_area_copy(&ph_coords, &obj->coords);
        ph_coords.x1 += left + border_width;
        ph_coords.x2 -= right + border_width;
        ph_coords.y1 += top + border_width;
        ph_coords.y2 -= bottom + border_width;
        ph_dsc.text = ta->placeholder_txt;
        lv_draw_label(layer, &ph_dsc, &ph_coords);
    }
}

static void draw_cursor(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_textarea_t * ta = (lv_textarea_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(e);
    const char * txt = lv_label_get_text(ta->label);

    if(ta->cursor.show == 0) return;

    lv_draw_rect_dsc_t cur_dsc;
    lv_draw_rect_dsc_init(&cur_dsc);
    cur_dsc.base.layer = layer;
    lv_obj_init_draw_rect_dsc(obj, LV_PART_CURSOR, &cur_dsc);

    /*Draw he cursor according to the type*/
    lv_area_t cur_area;
    lv_area_copy(&cur_area, &ta->cursor.area);

    cur_area.x1 += ta->label->coords.x1;
    cur_area.y1 += ta->label->coords.y1;
    cur_area.x2 += ta->label->coords.x1;
    cur_area.y2 += ta->label->coords.y1;

    lv_draw_rect(layer, &cur_dsc, &cur_area);

    int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_CURSOR);
    int32_t left = lv_obj_get_style_pad_left(obj, LV_PART_CURSOR) + border_width;
    int32_t top = lv_obj_get_style_pad_top(obj, LV_PART_CURSOR) + border_width;
    char letter_buf[8] = {0};
    lv_memcpy(letter_buf, &txt[ta->cursor.txt_byte_pos], lv_text_encoded_size(&txt[ta->cursor.txt_byte_pos]));

    cur_area.x1 += left;
    cur_area.y1 += top;

    /*Draw the letter over the cursor only if
     *the cursor has background or the letter has different color than the original.
     *Else the original letter is drawn twice which makes it look bolder*/
    lv_color_t label_color = lv_obj_get_style_text_color(ta->label, 0);
    lv_draw_label_dsc_t cur_label_dsc;
    lv_draw_label_dsc_init(&cur_label_dsc);
    cur_label_dsc.base.layer = layer;
    lv_obj_init_draw_label_dsc(obj, LV_PART_CURSOR, &cur_label_dsc);
    if(cur_dsc.bg_opa > LV_OPA_MIN || !lv_color_eq(cur_label_dsc.color, label_color)) {
        cur_label_dsc.text = letter_buf;
        cur_label_dsc.text_local = true;
        lv_draw_label(layer, &cur_label_dsc, &cur_area);
    }
}

static void auto_hide_characters(lv_obj_t * obj)
{
    lv_textarea_t * ta = (lv_textarea_t *) obj;

    if(ta->pwd_show_time == 0) {
        pwd_char_hider(obj);
    }
    else {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, ta);
        lv_anim_set_exec_cb(&a, pwd_char_hider_anim);
        lv_anim_set_duration(&a, ta->pwd_show_time);
        lv_anim_set_values(&a, 0, 1);
        lv_anim_set_path_cb(&a, lv_anim_path_step);
        lv_anim_set_completed_cb(&a, pwd_char_hider_anim_completed);
        lv_anim_start(&a);
    }
}

static void auto_hide_characters_cancel(lv_obj_t * obj)
{
    lv_anim_delete(obj, pwd_char_hider_anim);
}

static inline bool is_valid_but_non_printable_char(const uint32_t letter)
{
    if(letter == '\0' || letter == '\n' || letter == '\r') {
        return true;
    }

    return false;
}

#endif
