/**
 * @file lv_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_label_private.h"
#include "../../misc/lv_area_private.h"
#include "../../misc/lv_anim_private.h"
#include "../../draw/lv_draw_label_private.h"
#include "../../core/lv_obj_class_private.h"
#if LV_USE_LABEL != 0
#include "../../core/lv_obj_private.h"
#include "../../misc/lv_assert.h"
#include "../../core/lv_group.h"
#include "../../display/lv_display.h"
#include "../../draw/lv_draw_private.h"
#include "../../misc/lv_color.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_bidi_private.h"
#include "../../misc/lv_text_ap.h"
#include "../../misc/lv_text_private.h"
#include "../../stdlib/lv_sprintf.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_label_class)

#define LV_LABEL_DEF_SCROLL_SPEED   lv_anim_speed_clamped(40, 300, 10000)
#define LV_LABEL_SCROLL_DELAY       300
#define LV_LABEL_DOT_BEGIN_INV 0xFFFFFFFF
#define LV_LABEL_HINT_HEIGHT_LIMIT 1024 /*Enable "hint" to buffer info about labels larger than this. (Speed up drawing)*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_label_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_label_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_label_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_main(lv_event_t * e);

static void lv_label_refr_text(lv_obj_t * obj);
static void lv_label_revert_dots(lv_obj_t * label);
static void lv_label_set_dots(lv_obj_t * label, uint32_t dot_begin);

static void set_ofs_x_anim(void * obj, int32_t v);
static void set_ofs_y_anim(void * obj, int32_t v);
static size_t get_text_length(const char * text);
static void copy_text_to_label(lv_label_t * label, const char * text);
static lv_text_flag_t get_label_flags(lv_label_t * label);
static void calculate_x_coordinate(int32_t * x, const lv_text_align_t align, const char * txt,
                                   uint32_t length, const lv_font_t * font, int32_t letter_space, lv_area_t * txt_coords, lv_text_flag_t flags);

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_OBJ_PROPERTY
static const lv_property_ops_t properties[] = {
    {
        .id = LV_PROPERTY_LABEL_TEXT,
        .setter = lv_label_set_text,
        .getter = lv_label_get_text,
    },
    {
        .id = LV_PROPERTY_LABEL_LONG_MODE,
        .setter = lv_label_set_long_mode,
        .getter = lv_label_get_long_mode,
    },
    {
        .id = LV_PROPERTY_LABEL_TEXT_SELECTION_START,
        .setter = lv_label_set_text_selection_start,
        .getter = lv_label_get_text_selection_start,
    },
    {
        .id = LV_PROPERTY_LABEL_TEXT_SELECTION_END,
        .setter = lv_label_set_text_selection_end,
        .getter = lv_label_get_text_selection_end,
    },
};
#endif

const lv_obj_class_t lv_label_class = {
    .constructor_cb = lv_label_constructor,
    .destructor_cb = lv_label_destructor,
    .event_cb = lv_label_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_label_t),
    .base_class = &lv_obj_class,
    .name = "label",
#if LV_USE_OBJ_PROPERTY
    .prop_index_start = LV_PROPERTY_LABEL_START,
    .prop_index_end = LV_PROPERTY_LABEL_END,
    .properties = properties,
    .properties_count = sizeof(properties) / sizeof(properties[0]),

#if LV_USE_OBJ_PROPERTY_NAME
    .property_names = lv_label_property_names,
    .names_count = sizeof(lv_label_property_names) / sizeof(lv_property_name_t),
#endif

#endif
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_label_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_label_set_text(lv_obj_t * obj, const char * text)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_label_t * label = (lv_label_t *)obj;

    /*If text is NULL then just refresh with the current text*/
    if(text == NULL) text = label->text;

    lv_label_revert_dots(obj); /*In case text == label->text*/
    const size_t text_len = get_text_length(text);

    /*If set its own text then reallocate it (maybe its size changed)*/
    if(label->text == text && label->static_txt == 0) {
        label->text = lv_realloc(label->text, text_len);
        LV_ASSERT_MALLOC(label->text);
        if(label->text == NULL) return;

#if LV_USE_ARABIC_PERSIAN_CHARS
        lv_text_ap_proc(label->text, label->text);
#endif

    }
    else {
        /*Free the old text*/
        if(label->text != NULL && label->static_txt == 0) {
            lv_free(label->text);
            label->text = NULL;
        }

        label->text = lv_malloc(text_len);
        LV_ASSERT_MALLOC(label->text);
        if(label->text == NULL) return;

        copy_text_to_label(label, text);

        /*Now the text is dynamically allocated*/
        label->static_txt = 0;
    }

    lv_label_refr_text(obj);
}

void lv_label_set_text_fmt(lv_obj_t * obj, const char * fmt, ...)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(fmt);

    lv_obj_invalidate(obj);
    lv_label_t * label = (lv_label_t *)obj;

    /*If text is NULL then refresh*/
    if(fmt == NULL) {
        lv_label_refr_text(obj);
        return;
    }

    if(label->text != NULL && label->static_txt == 0) {
        lv_free(label->text);
        label->text = NULL;
    }

    va_list args;
    va_start(args, fmt);
    label->text = lv_text_set_text_vfmt(fmt, args);
    va_end(args);
    label->static_txt = 0; /*Now the text is dynamically allocated*/

    lv_label_refr_text(obj);
}

void lv_label_set_text_static(lv_obj_t * obj, const char * text)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_label_t * label = (lv_label_t *)obj;

    if(label->static_txt == 0 && label->text != NULL) {
        lv_free(label->text);
        label->text = NULL;
    }

    if(text != NULL) {
        label->static_txt = 1;
        label->text       = (char *)text;
    }

    lv_label_refr_text(obj);
}

void lv_label_set_long_mode(lv_obj_t * obj, lv_label_long_mode_t long_mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_label_t * label = (lv_label_t *)obj;

    /*Delete the old animation (if exists)*/
    lv_anim_delete(obj, set_ofs_x_anim);
    lv_anim_delete(obj, set_ofs_y_anim);
    lv_point_set(&label->offset, 0, 0);

    if(long_mode == LV_LABEL_LONG_MODE_SCROLL || long_mode == LV_LABEL_LONG_MODE_SCROLL_CIRCULAR ||
       long_mode == LV_LABEL_LONG_MODE_CLIP)
        label->expand = 1;
    else
        label->expand = 0;

    label->long_mode = long_mode;
    lv_label_refr_text(obj);
}

void lv_label_set_text_selection_start(lv_obj_t * obj, uint32_t index)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

#if LV_LABEL_TEXT_SELECTION
    lv_label_t * label = (lv_label_t *)obj;
    label->sel_start   = index;
    lv_obj_invalidate(obj);
#else
    LV_UNUSED(obj);    /*Unused*/
    LV_UNUSED(index);  /*Unused*/
#endif
}

void lv_label_set_text_selection_end(lv_obj_t * obj, uint32_t index)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

#if LV_LABEL_TEXT_SELECTION
    lv_label_t * label = (lv_label_t *)obj;
    label->sel_end     = index;
    lv_obj_invalidate(obj);
#else
    LV_UNUSED(obj);   /*Unused*/
    LV_UNUSED(index); /*Unused*/
#endif
}

void lv_label_set_recolor(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_label_t * label = (lv_label_t *)obj;
    if(label->recolor == en) return;

    label->recolor = en == false ? 0 : 1;

    /*Refresh the text because the potential color codes in text needs to be hidden or revealed*/
    lv_label_refr_text(obj);
}

/*=====================
 * Getter functions
 *====================*/

char * lv_label_get_text(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_label_t * label = (lv_label_t *)obj;
    return label->text;
}

lv_label_long_mode_t lv_label_get_long_mode(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_label_t * label = (lv_label_t *)obj;
    return label->long_mode;
}

void lv_label_get_letter_pos(const lv_obj_t * obj, uint32_t char_id, lv_point_t * pos)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(pos);

    lv_label_t * label = (lv_label_t *)obj;
    const char * txt = lv_label_get_text(obj);
    const lv_text_align_t align = lv_obj_calculate_style_text_align(obj, LV_PART_MAIN, txt);

    if(txt[0] == '\0') {
        pos->y = 0;
        switch(align) {
            case LV_TEXT_ALIGN_LEFT:
                pos->x = 0;
                break;
            case LV_TEXT_ALIGN_RIGHT:
                pos->x = lv_obj_get_content_width(obj);
                break;
            case LV_TEXT_ALIGN_CENTER:
                pos->x = lv_obj_get_content_width(obj) / 2;
                break;
            default:
                break;
        }
        return;
    }

    lv_text_flag_t flag = get_label_flags(label);

    const uint32_t byte_id = lv_text_encoded_get_byte_id(txt, char_id);
    /*Search the line of the index letter*/
    const int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    const int32_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);

    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    const int32_t letter_height = lv_font_get_line_height(font);

    lv_area_t txt_coords;
    lv_obj_get_content_coords(obj, &txt_coords);
    const int32_t max_w = lv_area_get_width(&txt_coords);
    const int32_t max_h = lv_area_get_height(&txt_coords);

    int32_t y = 0;
    uint32_t line_start = 0;
    uint32_t new_line_start = 0;
    while(txt[new_line_start] != '\0') {
        bool last_line = y + letter_height + line_space + letter_height > max_h;
        if(last_line && label->long_mode == LV_LABEL_LONG_MODE_DOTS) flag |= LV_TEXT_FLAG_BREAK_ALL;

        new_line_start += lv_text_get_next_line(&txt[line_start], LV_TEXT_LEN_MAX, font, letter_space, max_w, NULL, flag);
        if(byte_id < new_line_start || txt[new_line_start] == '\0')
            break; /*The line of 'index' letter begins at 'line_start'*/

        y += letter_height + line_space;
        line_start = new_line_start;
    }

    /*If the last character is line break then go to the next line*/
    if(byte_id > 0) {
        if((txt[byte_id - 1] == '\n' || txt[byte_id - 1] == '\r') && txt[byte_id] == '\0') {
            y += letter_height + line_space;
            line_start = byte_id;
        }
    }

    const char * bidi_txt;
    uint32_t visual_byte_pos;
#if LV_USE_BIDI
    lv_base_dir_t base_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);
    if(base_dir == LV_BASE_DIR_AUTO) base_dir = lv_bidi_detect_base_dir(txt);

    char * mutable_bidi_txt = NULL;
    /*Handle Bidi*/
    if(new_line_start == byte_id) {
        visual_byte_pos = base_dir == LV_BASE_DIR_RTL ? 0 : byte_id - line_start;
        bidi_txt = &txt[line_start];
    }
    else {
        uint32_t line_char_id = lv_text_encoded_get_char_id(&txt[line_start], byte_id - line_start);

        bool is_rtl;
        uint32_t visual_char_pos = lv_bidi_get_visual_pos(&txt[line_start], &mutable_bidi_txt, new_line_start - line_start,
                                                          base_dir, line_char_id, &is_rtl);
        bidi_txt = mutable_bidi_txt;
        if(is_rtl) visual_char_pos++;

        visual_byte_pos = lv_text_encoded_get_byte_id(bidi_txt, visual_char_pos);
    }
#else
    bidi_txt = &txt[line_start];
    visual_byte_pos = byte_id - line_start;
#endif

    /*Calculate the x coordinate*/
    int32_t x = lv_text_get_width_with_flags(bidi_txt, visual_byte_pos, font, letter_space, flag);
    if(char_id != line_start) x += letter_space;

    uint32_t length = new_line_start - line_start;
    calculate_x_coordinate(&x, align, bidi_txt, length, font, letter_space, &txt_coords, flag);
    pos->x = x;
    pos->y = y;

#if LV_USE_BIDI
    if(mutable_bidi_txt) lv_free(mutable_bidi_txt);
#endif
}

uint32_t lv_label_get_letter_on(const lv_obj_t * obj, lv_point_t * pos_in, bool bidi)
{
    LV_UNUSED(bidi);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(pos_in);
    lv_label_t * label = (lv_label_t *)obj;

    lv_point_t pos;
    pos.x = pos_in->x - lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    pos.y = pos_in->y - lv_obj_get_style_pad_top(obj, LV_PART_MAIN);

    lv_area_t txt_coords;
    lv_obj_get_content_coords(obj, &txt_coords);
    const char * txt         = lv_label_get_text(obj);
    uint32_t line_start      = 0;
    uint32_t new_line_start  = 0;
    int32_t max_w = lv_area_get_width(&txt_coords);
    int32_t max_h = lv_area_get_height(&txt_coords);
    const lv_font_t * font   = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    const int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    const int32_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);
    const int32_t letter_height = lv_font_get_line_height(font);
    int32_t y = 0;

    lv_text_flag_t flag = get_label_flags(label);

    /*Search the line of the index letter*/;
    while(txt[line_start] != '\0') {
        /*If dots will be shown, break the last visible line anywhere,
         *not only at word boundaries.*/
        bool last_line = y + letter_height + line_space + letter_height > max_h;
        if(last_line && label->long_mode == LV_LABEL_LONG_MODE_DOTS) flag |= LV_TEXT_FLAG_BREAK_ALL;

        new_line_start += lv_text_get_next_line(&txt[line_start], LV_TEXT_LEN_MAX, font, letter_space, max_w, NULL, flag);

        if(pos.y <= y + letter_height) {
            /*The line is found (stored in 'line_start')*/
            /*Include the NULL terminator in the last line*/
            uint32_t tmp = new_line_start;
            uint32_t letter;
            letter = lv_text_encoded_prev(txt, &tmp);
            if(letter != '\n' && txt[new_line_start] == '\0') new_line_start++;
            break;
        }
        y += letter_height + line_space;

        line_start = new_line_start;
    }

    char * bidi_txt;

#if LV_USE_BIDI
    uint32_t txt_len = 0;
    if(bidi) {
        bidi_txt = lv_malloc(new_line_start - line_start + 1);
        txt_len = new_line_start - line_start;
        if(new_line_start > 0 && txt[new_line_start - 1] == '\0' && txt_len > 0) txt_len--;
        lv_bidi_process_paragraph(txt + line_start, bidi_txt, txt_len, lv_obj_get_style_base_dir(obj, LV_PART_MAIN), NULL, 0);
    }
    else
#endif
    {
        bidi_txt = (char *)txt + line_start;
    }

    /*Calculate the x coordinate*/
    int32_t x = 0;
    const lv_text_align_t align = lv_obj_calculate_style_text_align(obj, LV_PART_MAIN, label->text);
    uint32_t length = new_line_start - line_start;
    calculate_x_coordinate(&x, align, bidi_txt, length, font, letter_space, &txt_coords, flag);

    lv_text_cmd_state_t cmd_state = LV_TEXT_CMD_STATE_WAIT;

    uint32_t i = 0;
    uint32_t i_act = i;

    if(new_line_start > 0) {
        while(i + line_start < new_line_start) {
            /*Get the current letter and the next letter for kerning*/
            /*Be careful 'i' already points to the next character*/
            uint32_t letter;
            uint32_t letter_next;
            lv_text_encoded_letter_next_2(bidi_txt, &letter, &letter_next, &i);

            if((flag & LV_TEXT_FLAG_RECOLOR) != 0) {
                if(lv_text_is_cmd(&cmd_state, bidi_txt[i]) != false) {
                    continue; /*Skip the letter if it is part of a command*/
                }
            }

            int32_t gw = lv_font_get_glyph_width(font, letter, letter_next);

            /*Finish if the x position or the last char of the next line is reached*/
            if(pos.x < x + gw || i + line_start == new_line_start ||  txt[i_act + line_start] == '\0') {
                i = i_act;
                break;
            }
            x += gw;
            x += letter_space;
            i_act = i;
        }
    }

    uint32_t logical_pos;
#if LV_USE_BIDI
    if(bidi) {
        /*Handle Bidi*/
        uint32_t cid = lv_text_encoded_get_char_id(bidi_txt, i);
        if(txt[line_start + i] == '\0') {
            logical_pos = i;
        }
        else {
            bool is_rtl;
            logical_pos = lv_bidi_get_logical_pos(&txt[line_start], NULL,
                                                  txt_len, lv_obj_get_style_base_dir(obj, LV_PART_MAIN), cid, &is_rtl);
            if(is_rtl) logical_pos++;
        }
        lv_free(bidi_txt);
    }
    else
#endif
    {
        logical_pos = lv_text_encoded_get_char_id(bidi_txt, i);
    }

    return  logical_pos + lv_text_encoded_get_char_id(txt, line_start);
}

bool lv_label_is_char_under_pos(const lv_obj_t * obj, lv_point_t * pos)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(pos);

    lv_area_t txt_coords;
    lv_obj_get_content_coords(obj, &txt_coords);
    const char * txt         = lv_label_get_text(obj);
    lv_label_t * label     = (lv_label_t *)obj;
    uint32_t line_start      = 0;
    uint32_t new_line_start  = 0;
    const int32_t max_w = lv_area_get_width(&txt_coords);
    const int32_t max_h = lv_area_get_height(&txt_coords);
    const lv_font_t * font   = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    const int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    const int32_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);
    const int32_t letter_height    = lv_font_get_line_height(font);

    lv_text_flag_t flag = get_label_flags(label);

    /*Search the line of the index letter*/
    int32_t y = 0;
    while(txt[line_start] != '\0') {
        bool last_line = y + letter_height + line_space + letter_height > max_h;
        if(last_line && label->long_mode == LV_LABEL_LONG_MODE_DOTS) flag |= LV_TEXT_FLAG_BREAK_ALL;

        new_line_start += lv_text_get_next_line(&txt[line_start], LV_TEXT_LEN_MAX, font, letter_space, max_w, NULL, flag);

        if(pos->y <= y + letter_height) break; /*The line is found (stored in 'line_start')*/
        y += letter_height + line_space;

        line_start = new_line_start;
    }

    /*Calculate the x coordinate*/
    const lv_text_align_t align = lv_obj_calculate_style_text_align(obj, LV_PART_MAIN, label->text);

    int32_t x = 0;
    if(align == LV_TEXT_ALIGN_CENTER) {
        const int32_t line_w = lv_text_get_width_with_flags(&txt[line_start], new_line_start - line_start, font, letter_space,
                                                            flag);
        x += lv_area_get_width(&txt_coords) / 2 - line_w / 2;
    }
    else if(align == LV_TEXT_ALIGN_RIGHT) {
        const int32_t line_w = lv_text_get_width_with_flags(&txt[line_start], new_line_start - line_start, font, letter_space,
                                                            flag);
        x += lv_area_get_width(&txt_coords) - line_w;
    }

    lv_text_cmd_state_t cmd_state = LV_TEXT_CMD_STATE_WAIT;

    int32_t last_x = 0;
    uint32_t i           = line_start;
    uint32_t i_current   = i;
    uint32_t letter      = '\0';
    uint32_t letter_next = '\0';

    if(new_line_start > 0) {
        while(i <= new_line_start - 1) {
            /*Get the current letter and the next letter for kerning*/
            /*Be careful 'i' already points to the next character*/
            lv_text_encoded_letter_next_2(txt, &letter, &letter_next, &i);

            if((flag & LV_TEXT_FLAG_RECOLOR) != 0) {
                if(lv_text_is_cmd(&cmd_state, txt[i]) != false) {
                    continue; /*Skip the letter if it is part of a command*/
                }
            }

            last_x = x;
            x += lv_font_get_glyph_width(font, letter, letter_next);
            if(pos->x < x) {
                i = i_current;
                break;
            }
            x += letter_space;
            i_current = i;
        }
    }

    const int32_t max_diff = lv_font_get_glyph_width(font, letter, letter_next) + letter_space + 1;
    return (pos->x >= (last_x - letter_space) && pos->x <= (last_x + max_diff));
}

uint32_t lv_label_get_text_selection_start(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

#if LV_LABEL_TEXT_SELECTION
    lv_label_t * label = (lv_label_t *)obj;
    return label->sel_start;
#else
    LV_UNUSED(obj); /*Unused*/
    return LV_LABEL_TEXT_SELECTION_OFF;
#endif
}

uint32_t lv_label_get_text_selection_end(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

#if LV_LABEL_TEXT_SELECTION
    lv_label_t * label = (lv_label_t *)obj;
    return label->sel_end;
#else
    LV_UNUSED(obj); /*Unused*/
    return LV_LABEL_TEXT_SELECTION_OFF;
#endif
}

bool lv_label_get_recolor(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_label_t * label = (lv_label_t *)obj;
    return label->recolor == 0 ? false : true;
}

/*=====================
 * Other functions
 *====================*/

void lv_label_ins_text(lv_obj_t * obj, uint32_t pos, const char * txt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(txt);

    lv_label_t * label = (lv_label_t *)obj;

    /*Cannot append to static text*/
    if(label->static_txt != 0) return;

    lv_obj_invalidate(obj);

    /*Allocate space for the new text*/
    size_t old_len = lv_strlen(label->text);
    size_t ins_len = lv_strlen(txt);
    size_t new_len = ins_len + old_len;
    label->text        = lv_realloc(label->text, new_len + 1);
    LV_ASSERT_MALLOC(label->text);
    if(label->text == NULL) return;

    if(pos == LV_LABEL_POS_LAST) {
        pos = lv_text_get_encoded_length(label->text);
    }

    lv_text_ins(label->text, pos, txt);
    lv_label_set_text(obj, NULL);
}

void lv_label_cut_text(lv_obj_t * obj, uint32_t pos, uint32_t cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_label_t * label = (lv_label_t *)obj;

    /*Cannot append to static text*/
    if(label->static_txt) return;

    lv_obj_invalidate(obj);

    char * label_txt = lv_label_get_text(obj);
    /*Delete the characters*/
    lv_text_cut(label_txt, pos, cnt);

    /*Refresh the label*/
    lv_label_refr_text(obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_label_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_label_t * label = (lv_label_t *)obj;

    label->text       = NULL;
    label->recolor    = 0;
    label->static_txt = 0;
    label->dot_begin  = LV_LABEL_DOT_BEGIN_INV;
    label->long_mode  = LV_LABEL_LONG_MODE_WRAP;
    lv_point_set(&label->offset, 0, 0);

#if LV_LABEL_LONG_TXT_HINT
    label->hint.line_start = -1;
    label->hint.coord_y    = 0;
    label->hint.y          = 0;
#endif

#if LV_LABEL_TEXT_SELECTION
    label->sel_start = LV_DRAW_LABEL_NO_TXT_SEL;
    label->sel_end   = LV_DRAW_LABEL_NO_TXT_SEL;
#endif

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_label_set_long_mode(obj, LV_LABEL_LONG_MODE_WRAP);
    lv_label_set_text(obj, LV_LABEL_DEFAULT_TEXT);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_label_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_label_t * label = (lv_label_t *)obj;

    if(!label->static_txt) lv_free(label->text);
    label->text = NULL;
}

static void lv_label_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    /*Call the ancestor's event handler*/
    const lv_result_t res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    const lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    if((code == LV_EVENT_STYLE_CHANGED) || (code == LV_EVENT_SIZE_CHANGED)) {
        lv_label_refr_text(obj);
    }
    else if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        /* Italic or other non-typical letters can be drawn of out of the object.
         * It happens if box_w + ofs_x > adw_w in the glyph.
         * To avoid this add some extra draw area.
         * font_h / 4 is an empirical value. */
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        const int32_t font_h = lv_font_get_line_height(font);
        lv_event_set_ext_draw_size(e, font_h / 4);
    }
    else if(code == LV_EVENT_GET_SELF_SIZE) {
        lv_label_t * label = (lv_label_t *)obj;
        if(label->invalid_size_cache) {
            const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
            int32_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);
            int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
            lv_text_flag_t flag = LV_TEXT_FLAG_NONE;
            if(label->recolor != 0) flag |= LV_TEXT_FLAG_RECOLOR;
            if(label->expand != 0) flag |= LV_TEXT_FLAG_EXPAND;

            int32_t w;
            if(lv_obj_get_style_width(obj, LV_PART_MAIN) == LV_SIZE_CONTENT && !obj->w_layout) w = LV_COORD_MAX;
            else w = lv_obj_get_content_width(obj);
            w = LV_MIN(w, lv_obj_get_style_max_width(obj, 0));

            uint32_t dot_begin = label->dot_begin;
            lv_label_revert_dots(obj);
            lv_text_get_size(&label->size_cache, label->text, font, letter_space, line_space, w, flag);
            lv_label_set_dots(obj, dot_begin);

            label->invalid_size_cache = false;
        }

        lv_point_t * self_size = lv_event_get_param(e);
        self_size->x = LV_MAX(self_size->x, label->size_cache.x);
        self_size->y = LV_MAX(self_size->y, label->size_cache.y);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_main(e);
    }
}

static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_label_t * label = (lv_label_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(e);

    lv_area_t txt_coords;
    lv_obj_get_content_coords(obj, &txt_coords);

    lv_text_flag_t flag = get_label_flags(label);

    lv_draw_label_dsc_t label_draw_dsc;
    lv_draw_label_dsc_init(&label_draw_dsc);
    label_draw_dsc.text = label->text;
    label_draw_dsc.text_static = label->static_txt;
    label_draw_dsc.ofs_x = label->offset.x;
    label_draw_dsc.ofs_y = label->offset.y;
#if LV_LABEL_LONG_TXT_HINT
    if(label->long_mode != LV_LABEL_LONG_MODE_SCROLL_CIRCULAR &&
       lv_area_get_height(&txt_coords) >= LV_LABEL_HINT_HEIGHT_LIMIT) {
        label_draw_dsc.hint = &label->hint;
    }
#endif

    label_draw_dsc.flag = flag;
    lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &label_draw_dsc);
    lv_bidi_calculate_align(&label_draw_dsc.align, &label_draw_dsc.bidi_dir, label->text);

    label_draw_dsc.sel_start = lv_label_get_text_selection_start(obj);
    label_draw_dsc.sel_end = lv_label_get_text_selection_end(obj);
    if(label_draw_dsc.sel_start != LV_DRAW_LABEL_NO_TXT_SEL && label_draw_dsc.sel_end != LV_DRAW_LABEL_NO_TXT_SEL) {
        label_draw_dsc.sel_color = lv_obj_get_style_text_color_filtered(obj, LV_PART_SELECTED);
        label_draw_dsc.sel_bg_color = lv_obj_get_style_bg_color(obj, LV_PART_SELECTED);
    }

    /* In SCROLL and SCROLL_CIRCULAR mode the CENTER and RIGHT are pointless, so remove them.
     * (In addition, they will create misalignment in this situation)*/
    if((label->long_mode == LV_LABEL_LONG_MODE_SCROLL || label->long_mode == LV_LABEL_LONG_MODE_SCROLL_CIRCULAR) &&
       (label_draw_dsc.align == LV_TEXT_ALIGN_CENTER || label_draw_dsc.align == LV_TEXT_ALIGN_RIGHT)) {
        lv_point_t size;
        lv_text_get_size(&size, label->text, label_draw_dsc.font, label_draw_dsc.letter_space, label_draw_dsc.line_space,
                         LV_COORD_MAX, flag);
        if(size.x > lv_area_get_width(&txt_coords)) {
            label_draw_dsc.align = LV_TEXT_ALIGN_LEFT;
        }
    }

    lv_area_t txt_clip;
    bool is_common = lv_area_intersect(&txt_clip, &txt_coords, &layer->_clip_area);
    if(!is_common) {
        return;
    }

    if(label->long_mode == LV_LABEL_LONG_MODE_WRAP) {
        int32_t s = lv_obj_get_scroll_top(obj);
        lv_area_move(&txt_coords, 0, -s);
        txt_coords.y2 = obj->coords.y2;
    }
    if(label->long_mode == LV_LABEL_LONG_MODE_SCROLL || label->long_mode == LV_LABEL_LONG_MODE_SCROLL_CIRCULAR) {
        const lv_area_t clip_area_ori = layer->_clip_area;
        layer->_clip_area = txt_clip;
        lv_draw_label(layer, &label_draw_dsc, &txt_coords);
        layer->_clip_area = clip_area_ori;
    }
    else {
        lv_draw_label(layer, &label_draw_dsc, &txt_coords);
    }

    lv_area_t clip_area_ori = layer->_clip_area;
    layer->_clip_area = txt_clip;

    if(label->long_mode == LV_LABEL_LONG_MODE_SCROLL_CIRCULAR) {
        lv_point_t size;
        lv_text_get_size(&size, label->text, label_draw_dsc.font, label_draw_dsc.letter_space, label_draw_dsc.line_space,
                         LV_COORD_MAX, flag);

        /*Draw the text again on label to the original to make a circular effect */
        if(size.x > lv_area_get_width(&txt_coords)) {
            label_draw_dsc.ofs_x = label->offset.x + size.x +
                                   lv_font_get_glyph_width(label_draw_dsc.font, ' ', ' ') * LV_LABEL_WAIT_CHAR_COUNT;
            label_draw_dsc.ofs_y = label->offset.y;

            lv_draw_label(layer, &label_draw_dsc, &txt_coords);
        }

        /*Draw the text again below the original to make a circular effect */
        if(size.y > lv_area_get_height(&txt_coords)) {
            label_draw_dsc.ofs_x = label->offset.x;
            label_draw_dsc.ofs_y = label->offset.y + size.y + lv_font_get_line_height(label_draw_dsc.font);

            lv_draw_label(layer, &label_draw_dsc, &txt_coords);
        }
    }

    layer->_clip_area = clip_area_ori;
}

static void overwrite_anim_property(lv_anim_t * dest, const lv_anim_t * src, lv_label_long_mode_t mode)
{
    switch(mode) {
        case LV_LABEL_LONG_MODE_SCROLL:
            /** If the dest animation is already running, overwrite is not allowed */
            if(dest->act_time <= 0)
                dest->act_time = src->act_time;
            dest->repeat_cnt = src->repeat_cnt;
            dest->repeat_delay = src->repeat_delay;
            dest->completed_cb = src->completed_cb;
            dest->reverse_delay = src->reverse_delay;
            break;
        case LV_LABEL_LONG_MODE_SCROLL_CIRCULAR:
            /** If the dest animation is already running, overwrite is not allowed */
            if(dest->act_time <= 0)
                dest->act_time = src->act_time;
            dest->repeat_cnt = src->repeat_cnt;
            dest->repeat_delay = src->repeat_delay;
            dest->completed_cb = src->completed_cb;
            break;
        default:
            break;
    }
}

/**
 * Refresh the label with its text stored in its extended data
 * @param label pointer to a label object
 */
static void lv_label_refr_text(lv_obj_t * obj)
{
    lv_label_t * label = (lv_label_t *)obj;
    if(label->text == NULL) return;
#if LV_LABEL_LONG_TXT_HINT
    label->hint.line_start = -1; /*The hint is invalid if the text changes*/
#endif
    label->invalid_size_cache = true;

    lv_area_t txt_coords;
    lv_obj_get_content_coords(obj, &txt_coords);
    int32_t max_w         = lv_area_get_width(&txt_coords);
    const lv_font_t * font   = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    int32_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);

    /*Calc. the height and longest line*/
    lv_point_t size;
    lv_text_flag_t flag = get_label_flags(label);

    lv_label_revert_dots(obj);
    lv_text_get_size(&size, label->text, font, letter_space, line_space, max_w, flag);

    lv_obj_refresh_self_size(obj);

    /*In scroll mode start an offset animation*/
    if(label->long_mode == LV_LABEL_LONG_MODE_SCROLL) {
        const lv_anim_t * anim_template = lv_obj_get_style_anim(obj, LV_PART_MAIN);
        uint32_t anim_time = lv_obj_get_style_anim_duration(obj, LV_PART_MAIN);
        if(anim_time == 0) anim_time = LV_LABEL_DEF_SCROLL_SPEED;
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_reverse_delay(&a, LV_LABEL_SCROLL_DELAY);
        lv_anim_set_repeat_delay(&a, a.reverse_delay);

        bool hor_anim = false;
        if(size.x > lv_area_get_width(&txt_coords)) {
            int32_t start = 0;
            int32_t end = 0;

#if LV_USE_BIDI
            lv_base_dir_t base_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);

            if(base_dir == LV_BASE_DIR_AUTO)
                base_dir = lv_bidi_detect_base_dir(label->text);

            if(base_dir == LV_BASE_DIR_RTL) {
                start = lv_area_get_width(&txt_coords) - size.x;
                end = 0;
            }
            else {
                start = 0;
                end = lv_area_get_width(&txt_coords) - size.x;
            }
#else
            end = lv_area_get_width(&txt_coords) - size.x;
#endif

            lv_anim_set_values(&a, start, end);
            lv_anim_set_exec_cb(&a, set_ofs_x_anim);

            lv_anim_t * anim_cur = lv_anim_get(obj, set_ofs_x_anim);
            int32_t act_time = 0;
            bool reverse_play_in_progress = false;
            if(anim_cur) {
                act_time = anim_cur->act_time;
                reverse_play_in_progress = anim_cur->reverse_play_in_progress;
            }
            if(act_time < a.duration) {
                a.act_time = act_time;      /*To keep the old position*/
                a.early_apply = 0;
                if(reverse_play_in_progress) {
                    a.reverse_play_in_progress = 1;
                    /*Swap the start and end values*/
                    int32_t tmp;
                    tmp      = a.start_value;
                    a.start_value = a.end_value;
                    a.end_value   = tmp;
                }
            }

            lv_anim_set_duration(&a, anim_time);
            lv_anim_set_reverse_duration(&a, a.duration);

            /*If a template animation exists, overwrite some property*/
            if(anim_template)
                overwrite_anim_property(&a, anim_template, label->long_mode);
            lv_anim_start(&a);
            hor_anim = true;
        }
        else {
            /*Delete the offset animation if not required*/
            lv_anim_delete(obj, set_ofs_x_anim);
            label->offset.x = 0;
        }

        if(size.y > lv_area_get_height(&txt_coords) && hor_anim == false) {
            lv_anim_set_values(&a, 0, lv_area_get_height(&txt_coords) - size.y - (lv_font_get_line_height(font)));
            lv_anim_set_exec_cb(&a, set_ofs_y_anim);

            lv_anim_t * anim_cur = lv_anim_get(obj, set_ofs_y_anim);
            int32_t act_time = 0;
            bool reverse_play_in_progress = false;
            if(anim_cur) {
                act_time = anim_cur->act_time;
                reverse_play_in_progress = anim_cur->reverse_play_in_progress;
            }
            if(act_time < a.duration) {
                a.act_time = act_time;      /*To keep the old position*/
                a.early_apply = 0;
                if(reverse_play_in_progress) {
                    a.reverse_play_in_progress = 1;
                    /*Swap the start and end values*/
                    int32_t tmp;
                    tmp      = a.start_value;
                    a.start_value = a.end_value;
                    a.end_value   = tmp;
                }
            }

            lv_anim_set_duration(&a, anim_time);
            lv_anim_set_reverse_duration(&a, a.duration);

            /*If a template animation exists, overwrite some property*/
            if(anim_template)
                overwrite_anim_property(&a, anim_template, label->long_mode);
            lv_anim_start(&a);
        }
        else {
            /*Delete the offset animation if not required*/
            lv_anim_delete(obj, set_ofs_y_anim);
            label->offset.y = 0;
        }
    }
    /*In roll inf. mode keep the size but start offset animations*/
    else if(label->long_mode == LV_LABEL_LONG_MODE_SCROLL_CIRCULAR) {
        const lv_anim_t * anim_template = lv_obj_get_style_anim(obj, LV_PART_MAIN);
        uint32_t anim_time = lv_obj_get_style_anim_duration(obj, LV_PART_MAIN);
        if(anim_time == 0) anim_time = LV_LABEL_DEF_SCROLL_SPEED;
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

        bool hor_anim = false;
        if(size.x > lv_area_get_width(&txt_coords)) {
#if LV_USE_BIDI
            int32_t start, end;
            lv_base_dir_t base_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);

            if(base_dir == LV_BASE_DIR_AUTO)
                base_dir = lv_bidi_detect_base_dir(label->text);

            if(base_dir == LV_BASE_DIR_RTL) {
                start = -size.x - lv_font_get_glyph_width(font, ' ', ' ') * LV_LABEL_WAIT_CHAR_COUNT;
                end = 0;
            }
            else {
                start = 0;
                end = -size.x - lv_font_get_glyph_width(font, ' ', ' ') * LV_LABEL_WAIT_CHAR_COUNT;
            }

            lv_anim_set_values(&a, start, end);
#else
            lv_anim_set_values(&a, 0, -size.x - lv_font_get_glyph_width(font, ' ', ' ') * LV_LABEL_WAIT_CHAR_COUNT);
#endif
            lv_anim_set_exec_cb(&a, set_ofs_x_anim);
            lv_anim_set_duration(&a, anim_time);

            lv_anim_t * anim_cur = lv_anim_get(obj, set_ofs_x_anim);
            int32_t act_time = anim_cur ? anim_cur->act_time : 0;

            /*If a template animation exists, overwrite some property*/
            if(anim_template) {
                overwrite_anim_property(&a, anim_template, label->long_mode);
            }
            else if(act_time < a.duration) {
                a.act_time = act_time;      /*To keep the old position when the label text is updated mid-scrolling*/
                a.early_apply = 0;
            }

            lv_anim_start(&a);
            hor_anim = true;
        }
        else {
            /*Delete the offset animation if not required*/
            lv_anim_delete(obj, set_ofs_x_anim);
            label->offset.x = 0;
        }

        if(size.y > lv_area_get_height(&txt_coords) && hor_anim == false) {
            lv_anim_set_values(&a, 0, -size.y - (lv_font_get_line_height(font)));
            lv_anim_set_exec_cb(&a, set_ofs_y_anim);
            lv_anim_set_duration(&a, anim_time);

            lv_anim_t * anim_cur = lv_anim_get(obj, set_ofs_y_anim);
            int32_t act_time = anim_cur ? anim_cur->act_time : 0;

            /*If a template animation exists, overwrite some property*/
            if(anim_template) {
                overwrite_anim_property(&a, anim_template, label->long_mode);
            }
            else if(act_time < a.duration) {
                a.act_time = act_time;      /*To keep the old position when the label text is updated mid-scrolling*/
                a.early_apply = 0;
            }

            lv_anim_start(&a);
        }
        else {
            /*Delete the offset animation if not required*/
            lv_anim_delete(obj, set_ofs_y_anim);
            label->offset.y = 0;
        }
    }
    else if(label->long_mode == LV_LABEL_LONG_MODE_DOTS) {
        if(size.y > lv_area_get_height(&txt_coords) && /*Text overflows available area*/
           size.y > lv_font_get_line_height(font) && /*No break requested, so no dots required*/
           lv_text_get_encoded_length(label->text) > LV_LABEL_DOT_NUM) { /*Do not turn all characters into dots*/
            lv_point_t p;
            int32_t y_overed;
            p.x = lv_area_get_width(&txt_coords) -
                  (lv_font_get_glyph_width(font, '.', '.') + letter_space) *
                  LV_LABEL_DOT_NUM; /*Shrink with dots*/
            p.y = lv_area_get_height(&txt_coords);
            y_overed = p.y %
                       (lv_font_get_line_height(font) + line_space); /*Round down to the last line*/
            if(y_overed >= lv_font_get_line_height(font)) {
                p.y -= y_overed;
                p.y += lv_font_get_line_height(font);
            }
            else {
                p.y -= y_overed;
                p.y -= line_space;
            }

            uint32_t letter_id = lv_label_get_letter_on(obj, &p, false);

            /*Be sure there is space for the dots*/
            size_t txt_len = lv_strlen(label->text);
            uint32_t byte_id     = lv_text_encoded_get_byte_id(label->text, letter_id);
            while(byte_id + LV_LABEL_DOT_NUM > txt_len) {
                lv_text_encoded_prev(label->text, &byte_id);
                letter_id--;
            }

            /*Save letters under the dots and replace them with dots*/
            lv_label_set_dots(obj, byte_id);
        }
    }
    else if(label->long_mode == LV_LABEL_LONG_MODE_CLIP || label->long_mode == LV_LABEL_LONG_MODE_WRAP) {
        /*Do nothing*/
    }

    lv_obj_invalidate(obj);
}

static void lv_label_revert_dots(lv_obj_t * obj)
{
    lv_label_t * label = (lv_label_t *)obj;
    if(label->dot_begin != LV_LABEL_DOT_BEGIN_INV) {
        for(int i = 0; i < LV_LABEL_DOT_NUM + 1 && label->dot[i]; i++) {
            label->text[label->dot_begin + i] = label->dot[i];
        }
        label->dot_begin = LV_LABEL_DOT_BEGIN_INV;
    }
}

static void lv_label_set_dots(lv_obj_t * obj, uint32_t dot_begin)
{
    lv_label_t * label = (lv_label_t *)obj;
    LV_ASSERT_MSG(label->dot_begin == LV_LABEL_DOT_BEGIN_INV, "Label dots already set");
    if(dot_begin != LV_LABEL_DOT_BEGIN_INV) {
        /*Save characters*/
        lv_strncpy(label->dot, &label->text[dot_begin], LV_LABEL_DOT_NUM + 1);
        label->dot_begin = dot_begin;

        /*Overwrite up to LV_LABEL_DOT_NUM + 1 characters with dots and null terminator*/
        int i = 0;
        for(; i < LV_LABEL_DOT_NUM && label->text[dot_begin + i]; i++) {
            label->text[dot_begin + i] = '.';
        }
        label->text[dot_begin + i] = '\0';
    }
}

static void set_ofs_x_anim(void * obj, int32_t v)
{
    lv_label_t * label = (lv_label_t *)obj;
    label->offset.x    = v;
    lv_obj_invalidate(obj);
}

static void set_ofs_y_anim(void * obj, int32_t v)
{
    lv_label_t * label = (lv_label_t *)obj;
    label->offset.y    = v;
    lv_obj_invalidate(obj);
}

static size_t get_text_length(const char * text)
{
    size_t len = 0;
#if LV_USE_ARABIC_PERSIAN_CHARS
    len = lv_text_ap_calc_bytes_count(text);
#else
    len = lv_strlen(text) + 1;
#endif

    return len;
}

static void copy_text_to_label(lv_label_t * label, const char * text)
{
#if LV_USE_ARABIC_PERSIAN_CHARS
    lv_text_ap_proc(text, label->text);
#else
    lv_strcpy(label->text, text);
#endif
}

static lv_text_flag_t get_label_flags(lv_label_t * label)
{
    lv_text_flag_t flag = LV_TEXT_FLAG_NONE;

    if(label->recolor) flag |= LV_TEXT_FLAG_RECOLOR;
    if(label->expand) flag |= LV_TEXT_FLAG_EXPAND;

    lv_obj_t * obj = (lv_obj_t *) label;
    if(lv_obj_get_style_width(obj, LV_PART_MAIN) == LV_SIZE_CONTENT &&
       lv_obj_get_style_max_width(obj, LV_PART_MAIN) == LV_COORD_MAX &&
       !obj->w_layout) {
        flag |= LV_TEXT_FLAG_FIT;
    }

    return flag;
}

/* Function created because of this pattern be used in multiple functions */
static void calculate_x_coordinate(int32_t * x, const lv_text_align_t align, const char * txt, uint32_t length,
                                   const lv_font_t * font, int32_t letter_space, lv_area_t * txt_coords, lv_text_flag_t flags)
{
    if(align == LV_TEXT_ALIGN_CENTER) {
        const int32_t line_w = lv_text_get_width_with_flags(txt, length, font, letter_space, flags);
        *x += lv_area_get_width(txt_coords) / 2 - line_w / 2;
    }
    else if(align == LV_TEXT_ALIGN_RIGHT) {
        const int32_t line_w = lv_text_get_width_with_flags(txt, length, font, letter_space, flags);
        *x += lv_area_get_width(txt_coords) - line_w;
    }
    else {
        /* Nothing to do */
    }
}

#endif
