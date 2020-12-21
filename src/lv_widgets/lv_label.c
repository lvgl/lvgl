/**
 * @file lv_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_label.h"
#if LV_USE_LABEL != 0
#include "../lv_core/lv_obj.h"
#include "../lv_misc/lv_debug.h"
#include "../lv_core/lv_group.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_bidi.h"
#include "../lv_misc/lv_txt_ap.h"
#include "../lv_misc/lv_printf.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_label"

/*Test configurations*/
#ifndef LV_LABEL_DEF_SCROLL_SPEED
    #define LV_LABEL_DEF_SCROLL_SPEED (25)
#endif

#define LV_LABEL_DOT_END_INV 0xFFFFFFFF
#define LV_LABEL_HINT_HEIGHT_LIMIT 1024 /*Enable "hint" to buffer info about labels larger than this. (Speed up drawing)*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
lv_label_class_t lv_label;

static void lv_label_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_label_destructor(void * obj);
static lv_res_t lv_label_signal(lv_obj_t * label, lv_signal_t sign, void * param);
static lv_design_res_t lv_label_design(lv_obj_t * label, const lv_area_t * clip_area, lv_design_mode_t mode);
static void lv_label_revert_dots(lv_obj_t * label);

#if LV_USE_ANIMATION
    static void lv_label_set_offset_x(lv_obj_t * label, lv_coord_t x);
    static void lv_label_set_offset_y(lv_obj_t * label, lv_coord_t y);
#endif

static bool lv_label_set_dot_tmp(lv_obj_t * label, char * data, uint32_t len);
static char * lv_label_get_dot_tmp(lv_obj_t * label);
static void lv_label_dot_tmp_free(lv_obj_t * label);
static void get_txt_coords(const lv_obj_t * label, lv_area_t * area);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a label objects
 * @param parent pointer to an object, it will be the parent of the new label
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other label to copy.
 * @return pointer to the created button
 */
lv_obj_t * lv_label_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    if(!lv_label._inited) {
        LV_CLASS_INIT(lv_label, lv_obj);
        lv_label.constructor = lv_label_constructor;
        lv_label.destructor = lv_label_destructor;
        lv_label.design_cb = lv_label_design;
        lv_label.signal_cb = lv_label_signal;
    }

    lv_obj_t * obj = lv_class_new(&lv_label);
    lv_label.constructor(obj, parent, copy);

    lv_obj_create_finish(obj, parent, copy);

    return obj;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param text '\0' terminated character string. NULL to refresh with the current text.
 */
void lv_label_set_text(lv_obj_t * obj, const char * text)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_label_t * label = (lv_label_t *) obj;

    lv_obj_invalidate(obj);

    /*If text is NULL then just refresh with the current text */
    if(text == NULL) text = label->text;

    LV_ASSERT_STR(text);

    if(label->text == text && label->static_txt == 0) {
        /*If set its own text then reallocate it (maybe its size changed)*/
#if LV_USE_ARABIC_PERSIAN_CHARS
        /*Get the size of the text and process it*/
        size_t len = _lv_txt_ap_calc_bytes_cnt(text);

        label->text = lv_mem_realloc(label->text, len);
        LV_ASSERT_MEM(label->text);
        if(label->text == NULL) return;

        _lv_txt_ap_proc(label->text, label->text);
#else
        label->text = lv_mem_realloc(label->text, strlen(label->text) + 1);
#endif

        LV_ASSERT_MEM(label->text);
        if(label->text == NULL) return;
    }
    else {
        /*Free the old text*/
        if(label->text != NULL && label->static_txt == 0) {
            lv_mem_free(label->text);
            label->text = NULL;
        }

#if LV_USE_ARABIC_PERSIAN_CHARS
        /*Get the size of the text and process it*/
        size_t len = _lv_txt_ap_calc_bytes_cnt(text);

        label->text = lv_mem_alloc(len);
        LV_ASSERT_MEM(label->text);
        if(label->text == NULL) return;

        _lv_txt_ap_proc(text, label->text);
#else
        /*Get the size of the text*/
        size_t len = strlen(text) + 1;

        /*Allocate space for the new text*/
        label->text = lv_mem_alloc(len);
        LV_ASSERT_MEM(label->text);
        if(label->text == NULL) return;
        strcpy(label->text, text);
#endif

        /*Now the text is dynamically allocated*/
        label->static_txt = 0;
    }

    lv_label_refr_text(obj);
}

/**
 * Set a new formatted text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param fmt `printf`-like format
 */
void lv_label_set_text_fmt(lv_obj_t * obj, const char * fmt, ...)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_STR(fmt);

    lv_obj_invalidate(obj);
    lv_label_t * label = (lv_label_t *) obj;

    /*If text is NULL then refresh */
    if(fmt == NULL) {
        lv_label_refr_text(obj);
        return;
    }

    if(label->text != NULL && label->static_txt == 0) {
        lv_mem_free(label->text);
        label->text = NULL;
    }

    va_list args;
    va_start(args, fmt);
    label->text = _lv_txt_set_text_vfmt(fmt, args);
    va_end(args);
    label->static_txt = 0; /*Now the text is dynamically allocated*/

    lv_label_refr_text(obj);
}

/**
 * Set a static text. It will not be saved by the label so the 'text' variable
 * has to be 'alive' while the label exist.
 * @param label pointer to a label object
 * @param text pointer to a text. NULL to refresh with the current text.
 */
void lv_label_set_text_static(lv_obj_t * obj, const char * text)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_label_t * label = (lv_label_t *) obj;

    if(label->static_txt == 0 && label->text != NULL) {
        lv_mem_free(label->text);
        label->text = NULL;
    }

    if(text != NULL) {
        label->static_txt = 1;
        label->text       = (char *)text;
    }

    lv_label_refr_text(obj);
}

/**
 * Set the behavior of the label with longer text then the object size
 * @param label pointer to a label object
 * @param long_mode the new mode from 'lv_label_long_mode' enum.
 *                  In LV_LONG_BREAK/LONG/ROLL the size of the label should be set AFTER this
 * function
 */
void lv_label_set_long_mode(lv_obj_t * obj, lv_label_long_mode_t long_mode)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_label_t * label = (lv_label_t *) obj;

#if LV_USE_ANIMATION
    /*Delete the old animation (if exists)*/
    lv_anim_del(obj, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_del(obj, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_del(obj, (lv_anim_exec_xcb_t)lv_label_set_offset_x);
    lv_anim_del(obj, (lv_anim_exec_xcb_t)lv_label_set_offset_y);
#endif
    label->offset.x = 0;
    label->offset.y = 0;

    if(long_mode == LV_LABEL_LONG_SROLL || long_mode == LV_LABEL_LONG_SROLL_CIRC || long_mode == LV_LABEL_LONG_CLIP)
        label->expand = 1;
    else
        label->expand = 0;

    /*Restore the character under the dots*/
    if(label->long_mode == LV_LABEL_LONG_DOT && label->dot_end != LV_LABEL_DOT_END_INV) {
        lv_label_revert_dots(obj);
    }

    label->long_mode = long_mode;
    lv_label_refr_text(obj);
}

/**
 * Set the align of the label (left or center)
 * @param label pointer to a label object
 * @param align 'LV_LABEL_ALIGN_LEFT' or 'LV_LABEL_ALIGN_LEFT'
 */
void lv_label_set_align(lv_obj_t * obj, lv_label_align_t align)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_label_t * label = (lv_label_t *) obj;
    if(label->align == align) return;

    label->align = align;

    /*Enough to invalidate because alignment is only drawing related (lv_refr_label_text() not required)*/
    lv_obj_invalidate(obj);
}

/**
 * Enable the recoloring by in-line commands
 * @param label pointer to a label object
 * @param en true: enable recoloring, false: disable
 */
void lv_label_set_recolor(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_label_t * label = (lv_label_t *) obj;
    if(label->recolor == en) return;

    label->recolor = en == false ? 0 : 1;

    /*Refresh the text because the potential color codes in text needs to be hidden or revealed*/
    lv_label_refr_text(obj);
}

/**
 * Set the label's animation speed in LV_LABEL_LONG_SROLL/SROLL_CIRC modes
 * @param label pointer to a label object
 * @param anim_speed speed of animation in px/sec unit
 */
void lv_label_set_anim_speed(lv_obj_t * obj, uint16_t anim_speed)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_label_t * label = (lv_label_t *) obj;
    if(label->anim_speed == anim_speed) return;

    label->anim_speed = anim_speed;

    if(label->long_mode == LV_LABEL_LONG_SROLL || label->long_mode == LV_LABEL_LONG_SROLL_CIRC) {
        lv_label_refr_text(obj);
    }
#else
    (void)label;      /*Unused*/
    (void)anim_speed; /*Unused*/
#endif
}

void lv_label_set_text_sel_start(lv_obj_t * obj, uint32_t index)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_label_t * label = (lv_label_t *) obj;
    label->sel_start   = index;
    lv_obj_invalidate(obj);
#else
    (void)label;    /*Unused*/
    (void)index;    /*Unused*/
#endif
}

void lv_label_set_text_sel_end(lv_obj_t * obj, uint32_t index)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_label_t * label = (lv_label_t *) obj;
    label->sel_end     = index;
    lv_obj_invalidate(obj);
#else
    (void)label;    /*Unused*/
    (void)index;    /*Unused*/
#endif
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a label
 * @param label pointer to a label object
 * @return the text of the label
 */
char * lv_label_get_text(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_label_t * label = (lv_label_t *) obj;
    return label->text;
}

/**
 * Get the long mode of a label
 * @param label pointer to a label object
 * @return the long mode
 */
lv_label_long_mode_t lv_label_get_long_mode(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_label_t * label = (lv_label_t *) obj;
    return label->long_mode;
}

/**
 * Get the align attribute
 * @param label pointer to a label object
 * @return LV_LABEL_ALIGN_LEFT or LV_LABEL_ALIGN_CENTER
 */
lv_label_align_t lv_label_get_align(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_label_t * label = (lv_label_t *) obj;
    lv_label_align_t align = label->align;

    if(align == LV_LABEL_ALIGN_AUTO) {
#if LV_USE_BIDI
        lv_bidi_dir_t base_dir = lv_obj_get_base_dir(label);
        if(base_dir == LV_BIDI_DIR_AUTO) base_dir = _lv_bidi_detect_base_dir(label->text);

        if(base_dir == LV_BIDI_DIR_LTR) align = LV_LABEL_ALIGN_LEFT;
        else if(base_dir == LV_BIDI_DIR_RTL) align = LV_LABEL_ALIGN_RIGHT;
#else
        align = LV_LABEL_ALIGN_LEFT;
#endif
    }

    return align;
}

/**
 * Get the recoloring attribute
 * @param label pointer to a label object
 * @return true: recoloring is enabled, false: disable
 */
bool lv_label_get_recolor(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_label_t * label = (lv_label_t *) obj;
    return label->recolor == 0 ? false : true;
}

/**
 * Get the label's animation speed in LV_LABEL_LONG_ROLL and SCROLL modes
 * @param label pointer to a label object
 * @return speed of animation in px/sec unit
 */
uint16_t lv_label_get_anim_speed(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_label_t * label = (lv_label_t *) obj;
    return label->anim_speed;
#else
    (void)label;      /*Unused*/
    return 0;
#endif
}

/**
 * Get the relative x and y coordinates of a letter
 * @param label pointer to a label object
 * @param index index of the letter [0 ... text length]. Expressed in character index, not byte
 * index (different in UTF-8)
 * @param pos store the result here (E.g. index = 0 gives 0;0 coordinates)
 */
void lv_label_get_letter_pos(const lv_obj_t * obj, uint32_t char_id, lv_point_t * pos)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(pos);

    const char * txt         = lv_label_get_text(obj);
    lv_label_align_t align = lv_label_get_align(obj);

    if(txt[0] == '\0') {
        pos->y = 0;
        switch(align) {
            case LV_LABEL_ALIGN_LEFT:
                pos->x = 0;
                break;
            case LV_LABEL_ALIGN_RIGHT:
                pos->x = lv_obj_get_width_fit(obj);
                break;
            case LV_LABEL_ALIGN_CENTER:
                pos->x = lv_obj_get_width_fit(obj) / 2;
                break;
        }
        return;
    }

    lv_area_t txt_coords;
    get_txt_coords(obj, &txt_coords);

    lv_label_t * label = (lv_label_t *) obj;
    uint32_t line_start      = 0;
    uint32_t new_line_start  = 0;
    lv_coord_t max_w         = lv_area_get_width(&txt_coords);
    const lv_font_t * font   = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);
    lv_coord_t letter_height    = lv_font_get_line_height(font);
    lv_coord_t y             = 0;
    lv_txt_flag_t flag       = LV_TXT_FLAG_NONE;

    if(label->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
    if(label->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
    if(label->long_mode == LV_LABEL_LONG_EXPAND) flag |= LV_TXT_FLAG_FIT;

    if(align == LV_LABEL_ALIGN_CENTER) flag |= LV_TXT_FLAG_CENTER;
    if(align == LV_LABEL_ALIGN_RIGHT) flag |= LV_TXT_FLAG_RIGHT;

    uint32_t byte_id = _lv_txt_encoded_get_byte_id(txt, char_id);

    /*Search the line of the index letter */;
    while(txt[new_line_start] != '\0') {
        new_line_start += _lv_txt_get_next_line(&txt[line_start], font, letter_space, max_w, flag);
        if(byte_id < new_line_start || txt[new_line_start] == '\0')
            break; /*The line of 'index' letter begins at 'line_start'*/

        y += letter_height + line_space;
        line_start = new_line_start;
    }

    /*If the last character is line break then go to the nlabel line*/
    if(byte_id > 0) {
        if((txt[byte_id - 1] == '\n' || txt[byte_id - 1] == '\r') && txt[byte_id] == '\0') {
            y += letter_height + line_space;
            line_start = byte_id;
        }
    }

    const char * bidi_txt;
    uint32_t visual_byte_pos;
#if LV_USE_BIDI
    char * mutable_bidi_txt = NULL;
    /*Handle Bidi*/
    if(new_line_start == byte_id) {
        visual_byte_pos = byte_id - line_start;
        bidi_txt =  &txt[line_start];
    }
    else {
        uint32_t line_char_id = _lv_txt_encoded_get_char_id(&txt[line_start], byte_id - line_start);

        bool is_rtl;
        uint32_t visual_char_pos = _lv_bidi_get_visual_pos(&txt[line_start], &mutable_bidi_txt, new_line_start - line_start,
                                                           lv_obj_get_base_dir(label), line_char_id, &is_rtl);
        bidi_txt = mutable_bidi_txt;
        if(is_rtl) visual_char_pos++;

        visual_byte_pos = _lv_txt_encoded_get_byte_id(bidi_txt, visual_char_pos);
    }
#else
    bidi_txt = &txt[line_start];
    visual_byte_pos = byte_id - line_start;
#endif


    /*Calculate the x coordinate*/
    lv_coord_t x = _lv_txt_get_width(bidi_txt, visual_byte_pos, font, letter_space, flag);
    if(char_id != line_start) x += letter_space;

    if(align == LV_LABEL_ALIGN_CENTER) {
        lv_coord_t line_w;
        line_w = _lv_txt_get_width(bidi_txt, new_line_start - line_start, font, letter_space, flag);
        x += lv_area_get_width(&txt_coords) / 2 - line_w / 2;

    }
    else if(align == LV_LABEL_ALIGN_RIGHT) {
        lv_coord_t line_w;
        line_w = _lv_txt_get_width(bidi_txt, new_line_start - line_start, font, letter_space, flag);

        x += lv_area_get_width(&txt_coords) - line_w;
    }
    pos->x = x;
    pos->y = y;

#if LV_USE_BIDI
    if(mutable_bidi_txt) _lv_mem_buf_release(mutable_bidi_txt);
#endif
}

/**
 * Get the index of letter on a relative point of a label
 * @param label pointer to label object
 * @param pos pointer to point with coordinates on a the label
 * @return the index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter)
 * Expressed in character index and not byte index (different in UTF-8)
 */
uint32_t lv_label_get_letter_on(const lv_obj_t * obj, lv_point_t * pos_in)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(pos_in);
    lv_label_t * label = (lv_label_t *) obj;

    lv_point_t pos;
    pos.x = pos_in->x - lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    pos.y = pos_in->y - lv_obj_get_style_pad_top(obj, LV_PART_MAIN);

    lv_area_t txt_coords;
    get_txt_coords(obj, &txt_coords);
    const char * txt         = lv_label_get_text(obj);
    uint32_t line_start      = 0;
    uint32_t new_line_start  = 0;
    lv_coord_t max_w         = lv_area_get_width(&txt_coords);
    const lv_font_t * font   = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);
    lv_coord_t letter_height    = lv_font_get_line_height(font);
    lv_coord_t y             = 0;
    lv_txt_flag_t flag       = LV_TXT_FLAG_NONE;
    uint32_t logical_pos;
    char * bidi_txt;

    if(label->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
    if(label->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
    if(label->long_mode == LV_LABEL_LONG_EXPAND) flag |= LV_TXT_FLAG_FIT;

    lv_label_align_t align = lv_label_get_align(obj);
    if(align == LV_LABEL_ALIGN_CENTER) flag |= LV_TXT_FLAG_CENTER;
    if(align == LV_LABEL_ALIGN_RIGHT) flag |= LV_TXT_FLAG_RIGHT;

    /*Search the line of the index letter */;
    while(txt[line_start] != '\0') {
        new_line_start += _lv_txt_get_next_line(&txt[line_start], font, letter_space, max_w, flag);

        if(pos.y <= y + letter_height) {
            /*The line is found (stored in 'line_start')*/
            /* Include the NULL terminator in the last line */
            uint32_t tmp = new_line_start;
            uint32_t letter;
            letter = _lv_txt_encoded_prev(txt, &tmp);
            if(letter != '\n' && txt[new_line_start] == '\0') new_line_start++;
            break;
        }
        y += letter_height + line_space;

        line_start = new_line_start;
    }

#if LV_USE_BIDI
    bidi_txt = _lv_mem_buf_get(new_line_start - line_start + 1);
    uint32_t txt_len = new_line_start - line_start;
    if(new_line_start > 0 && txt[new_line_start - 1] == '\0' && txt_len > 0) txt_len--;
    _lv_bidi_process_paragraph(txt + line_start, bidi_txt, txt_len, lv_obj_get_base_dir(label), NULL, 0);
#else
    bidi_txt = (char *)txt + line_start;
#endif

    /*Calculate the x coordinate*/
    lv_coord_t x = 0;
    if(align == LV_LABEL_ALIGN_CENTER) {
        lv_coord_t line_w;
        line_w = _lv_txt_get_width(bidi_txt, new_line_start - line_start, font, letter_space, flag);
        x += lv_area_get_width(&txt_coords) / 2 - line_w / 2;
    }
    else if(align == LV_LABEL_ALIGN_RIGHT) {
        lv_coord_t line_w;
        line_w = _lv_txt_get_width(bidi_txt, new_line_start - line_start, font, letter_space, flag);
        x += lv_area_get_width(&txt_coords) - line_w;
    }

    lv_txt_cmd_state_t cmd_state = LV_TXT_CMD_STATE_WAIT;

    uint32_t i = 0;
    uint32_t i_act = i;

    if(new_line_start > 0) {
        while(i + line_start < new_line_start) {
            /* Get the current letter.*/
            uint32_t letter = _lv_txt_encoded_next(bidi_txt, &i);

            /*Get the nlabel letter too for kerning*/
            uint32_t letter_nlabel = _lv_txt_encoded_next(&bidi_txt[i], NULL);

            /*Handle the recolor command*/
            if((flag & LV_TXT_FLAG_RECOLOR) != 0) {
                if(_lv_txt_is_cmd(&cmd_state, bidi_txt[i]) != false) {
                    continue; /*Skip the letter is it is part of a command*/
                }
            }

            lv_coord_t gw = lv_font_get_glyph_width(font, letter, letter_nlabel);

            /*Finish if the x position or the last char of the nlabel line is reached*/
            if(pos.x < x + gw || i + line_start == new_line_start ||  txt[i_act + line_start] == '\0') {
                i = i_act;
                break;
            }
            x += gw;
            x += letter_space;
            i_act = i;
        }
    }

#if LV_USE_BIDI
    /*Handle Bidi*/
    uint32_t cid = _lv_txt_encoded_get_char_id(bidi_txt, i);
    if(txt[line_start + cid] == '\0') {
        logical_pos = i;
    }
    else {
        bool is_rtl;
        logical_pos = _lv_bidi_get_logical_pos(&txt[line_start], NULL,
                                               txt_len, lv_obj_get_base_dir(label), cid, &is_rtl);
        if(is_rtl) logical_pos++;
        _lv_mem_buf_release(bidi_txt);
    }
#else
    logical_pos = _lv_txt_encoded_get_char_id(bidi_txt, i);
#endif

    return  logical_pos + _lv_txt_encoded_get_char_id(txt, line_start);
}

/**
 * @brief Get the selection start index.
 * @param label pointer to a label object.
 * @return selection start index. `LV_LABEL_TXT_SEL_OFF` if nothing is selected.
 */
uint32_t lv_label_get_text_sel_start(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_label_t * label = (lv_label_t *) obj;
    return label->sel_start;

#else
    (void)label;    /*Unused*/
    return LV_LABEL_TEXT_SEL_OFF;
#endif
}

/**
 * @brief Get the selection end index.
 * @param label pointer to a label object.
 * @return selection end index. `LV_LABEL_TXT_SEL_OFF` if nothing is selected.
 */
uint32_t lv_label_get_text_sel_end(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_label_t * label = (lv_label_t *) obj;
    return label->sel_end;
#else
    (void)label; /*Unused*/
    return LV_LABEL_TEXT_SEL_OFF;
#endif
}

/**
 * Check if a character is drawn under a point.
 * @param label Label object
 * @param pos Point to check for character under
 * @return whether a character is drawn under the point
 */
//bool lv_label_is_char_under_pos(const lv_obj_t * label, lv_point_t * pos)
//{
//    LV_ASSERT_OBJ(label, LV_OBJX_NAME);
//    LV_ASSERT_NULL(pos);
//
//    lv_area_t txt_coords;
//    get_txt_coords(label, &txt_coords);
//    const char * txt         = lv_label_get_text(obj);
//    lv_label_label_t * label     = lv_obj_get_label_attr(label);
//    uint32_t line_start      = 0;
//    uint32_t new_line_start  = 0;
//    lv_coord_t max_w         = lv_area_get_width(&txt_coords);
//    const lv_font_t * font   = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
//    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
//    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);
//    lv_coord_t letter_height    = lv_font_get_line_height(font);
//    lv_coord_t y             = 0;
//    lv_txt_flag_t flag       = LV_TXT_FLAG_NONE;
//    lv_label_align_t align = lv_label_get_align(label);
//
//    if(label->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
//    if(label->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
//    if(label->long_mode == LV_LABEL_LONG_EXPAND) flag |= LV_TXT_FLAG_FIT;
//    if(align == LV_LABEL_ALIGN_CENTER) flag |= LV_TXT_FLAG_CENTER;
//
//    /*Search the line of the index letter */;
//    while(txt[line_start] != '\0') {
//        new_line_start += _lv_txt_get_next_line(&txt[line_start], font, letter_space, max_w, flag);
//
//        if(pos->y <= y + letter_height) break; /*The line is found (stored in 'line_start')*/
//        y += letter_height + line_space;
//
//        line_start = new_line_start;
//    }
//
//    /*Calculate the x coordinate*/
//    lv_coord_t x      = 0;
//    lv_coord_t last_x = 0;
//    if(align == LV_LABEL_ALIGN_CENTER) {
//        lv_coord_t line_w;
//        line_w = _lv_txt_get_width(&txt[line_start], new_line_start - line_start, font, letter_space, flag);
//        x += lv_area_get_width(&txt_coords) / 2 - line_w / 2;
//    }
//    else if(align == LV_LABEL_ALIGN_RIGHT) {
//        lv_coord_t line_w;
//        line_w = _lv_txt_get_width(&txt[line_start], new_line_start - line_start, font, letter_space, flag);
//        x += lv_area_get_width(&txt_coords) - line_w;
//    }
//
//    lv_txt_cmd_state_t cmd_state = LV_TXT_CMD_STATE_WAIT;
//
//    uint32_t i           = line_start;
//    uint32_t i_current   = i;
//    uint32_t letter      = '\0';
//    uint32_t letter_nlabel = '\0';
//
//    if(new_line_start > 0) {
//        while(i <= new_line_start - 1) {
//            /* Get the current letter
//             * Be careful 'i' already points to the nlabel character */
//            letter = _lv_txt_encoded_next(txt, &i);
//
//            /*Get the nlabel letter for kerning*/
//            letter_nlabel = _lv_txt_encoded_next(&txt[i], NULL);
//
//            /*Handle the recolor command*/
//            if((flag & LV_TXT_FLAG_RECOLOR) != 0) {
//                if(_lv_txt_is_cmd(&cmd_state, txt[i]) != false) {
//                    continue; /*Skip the letter is it is part of a command*/
//                }
//            }
//            last_x = x;
//            x += lv_font_get_glyph_width(font, letter, letter_nlabel);
//            if(pos->x < x) {
//                i = i_current;
//                break;
//            }
//            x += letter_space;
//            i_current = i;
//        }
//    }
//
//    int32_t max_diff = lv_font_get_glyph_width(font, letter, letter_nlabel) + letter_space + 1;
//    return (pos->x >= (last_x - letter_space) && pos->x <= (last_x + max_diff));
//}

/*=====================
 * Other functions
 *====================*/

/**
 * Insert a text to the label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different
 * in UTF-8) 0: before first char. LV_LABEL_POS_LAST: after last char.
 * @param txt pointer to the text to insert
 */
void lv_label_ins_text(lv_obj_t * obj, uint32_t pos, const char * txt)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_STR(txt);

    lv_label_t * label = (lv_label_t *) obj;

    /*Can not append to static text*/
    if(label->static_txt != 0) return;

    lv_obj_invalidate(obj);

    /*Allocate space for the new text*/
    size_t old_len = strlen(label->text);
    size_t ins_len = strlen(txt);
    size_t new_len = ins_len + old_len;
    label->text        = lv_mem_realloc(label->text, new_len + 1);
    LV_ASSERT_MEM(label->text);
    if(label->text == NULL) return;

    if(pos == LV_LABEL_POS_LAST) {
        pos = _lv_txt_get_encoded_length(label->text);
    }

#if LV_USE_BIDI
    char * bidi_buf = _lv_mem_buf_get(ins_len + 1);
    LV_ASSERT_MEM(bidi_buf);
    if(bidi_buf == NULL) return;

    _lv_bidi_process(txt, bidi_buf, lv_obj_get_base_dir(label));
    _lv_txt_ins(label->text, pos, bidi_buf);

    _lv_mem_buf_release(bidi_buf);
#else
    _lv_txt_ins(label->text, pos, txt);
#endif
    lv_label_set_text(obj, NULL);
}

/**
 * Delete characters from a label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different
 * in UTF-8) 0: before first char.
 * @param cnt number of characters to cut
 */
void lv_label_cut_text(lv_obj_t * obj, uint32_t pos, uint32_t cnt)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_label_t * label = (lv_label_t *) obj;

    /*Can not append to static text*/
    if(label->static_txt != 0) return;

    lv_obj_invalidate(obj);

    char * label_txt = lv_label_get_text(obj);
    /*Delete the characters*/
    _lv_txt_cut(label_txt, pos, cnt);

    /*Refresh the label*/
    lv_label_refr_text(obj);
}

/**
 * Refresh the label with its text stored in its labelended data
 * @param label pointer to a label object
 */
void lv_label_refr_text(lv_obj_t * obj)
{
    lv_label_t * label = (lv_label_t *) obj;
    if(label->text == NULL) return;
#if LV_LABEL_LONG_TXT_HINT
    label->hint.line_start = -1; /*The hint is invalid if the text changes*/
#endif

    lv_area_t txt_coords;
    get_txt_coords(obj, &txt_coords);
    lv_coord_t max_w         = lv_area_get_width(&txt_coords);
    const lv_font_t * font   = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);

    /*Calc. the height and longest line*/
    lv_point_t size;
    lv_txt_flag_t flag = LV_TXT_FLAG_NONE;
    if(label->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
    if(label->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
    if(label->long_mode == LV_LABEL_LONG_EXPAND) flag |= LV_TXT_FLAG_FIT;
    _lv_txt_get_size(&size, label->text, font, letter_space, line_space, max_w, flag);

    /*Set the full size in expand mode*/
    if(label->long_mode == LV_LABEL_LONG_EXPAND) {
        size.x += lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
        size.y += lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
        lv_obj_set_size(obj, size.x, size.y);
    }
    /*In roll mode keep the size but start offset animations*/
    else if(label->long_mode == LV_LABEL_LONG_SROLL) {
#if LV_USE_ANIMATION
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, label);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_playback_delay(&a, (((lv_font_get_glyph_width(font, ' ', ' ') + letter_space) * 1000) /
                                        label->anim_speed) *
                                   LV_LABEL_WAIT_CHAR_COUNT);
        lv_anim_set_repeat_delay(&a, a.playback_delay);

        bool hor_anim = false;
        if(size.x > lv_area_get_width(&txt_coords)) {
            lv_anim_set_values(&a, 0, lv_area_get_width(&txt_coords) - size.x);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_label_set_offset_x);
            lv_anim_set_time(&a, lv_anim_speed_to_time(label->anim_speed, a.start, a.end));
            lv_anim_set_playback_time(&a, a.time);

            lv_anim_t * anim_cur = lv_anim_get(label, (lv_anim_exec_xcb_t)lv_label_set_offset_x);
            int32_t act_time = 0;
            bool playback_now = false;
            if(anim_cur) {
                act_time = anim_cur->act_time;
                playback_now = anim_cur->playback_now;
            }
            if(act_time < a.time) {
                a.act_time = act_time;      /*To keep the old position*/
                a.early_apply = 0;
                if(playback_now) {
                    a.playback_now = 1;
                    /*Swap the start and end values*/
                    int32_t tmp;
                    tmp      = a.start;
                    a.start = a.end;
                    a.end   = tmp;
                }
            }

            lv_anim_start(&a);
            hor_anim = true;
        }
        else {
            /*Delete the offset animation if not required*/
            lv_anim_del(label, (lv_anim_exec_xcb_t)lv_label_set_offset_x);
            label->offset.x = 0;
        }

        if(size.y > lv_area_get_height(&txt_coords) && hor_anim == false) {
            lv_anim_set_values(&a, 0, lv_area_get_height(&txt_coords) - size.y - (lv_font_get_line_height(font)));
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_label_set_offset_y);
            lv_anim_set_time(&a, lv_anim_speed_to_time(label->anim_speed, a.start, a.end));
            lv_anim_set_playback_time(&a, a.time);

            lv_anim_t * anim_cur = lv_anim_get(label, (lv_anim_exec_xcb_t)lv_label_set_offset_y);
            int32_t act_time = 0;
            bool playback_now = false;
            if(anim_cur) {
                act_time = anim_cur->act_time;
                playback_now = anim_cur->playback_now;
            }
            if(act_time < a.time) {
                a.act_time = act_time;      /*To keep the old position*/
                a.early_apply = 0;
                if(playback_now) {
                    a.playback_now = 1;
                    /*Swap the start and end values*/
                    int32_t tmp;
                    tmp      = a.start;
                    a.start = a.end;
                    a.end   = tmp;
                }
            }

            lv_anim_start(&a);
        }
        else {
            /*Delete the offset animation if not required*/
            lv_anim_del(label, (lv_anim_exec_xcb_t)lv_label_set_offset_y);
            label->offset.y = 0;
        }
#endif
    }
    /*In roll inf. mode keep the size but start offset animations*/
    else if(label->long_mode == LV_LABEL_LONG_SROLL_CIRC) {
#if LV_USE_ANIMATION
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, label);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

        bool hor_anim = false;
        if(size.x > lv_area_get_width(&txt_coords)) {
            lv_anim_set_values(&a, 0, -size.x - lv_font_get_glyph_width(font, ' ', ' ') * LV_LABEL_WAIT_CHAR_COUNT);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_label_set_offset_x);
            lv_anim_set_time(&a, lv_anim_speed_to_time(label->anim_speed, a.start, a.end));

            lv_anim_t * anim_cur = lv_anim_get(label, (lv_anim_exec_xcb_t)lv_label_set_offset_x);
            int32_t act_time = anim_cur ? anim_cur->act_time : 0;
            if(act_time < a.time) {
                a.act_time = act_time;      /*To keep the old position*/
                a.early_apply = 0;
            }

            lv_anim_start(&a);
            hor_anim = true;
        }
        else {
            /*Delete the offset animation if not required*/
            lv_anim_del(label, (lv_anim_exec_xcb_t)lv_label_set_offset_x);
            label->offset.x = 0;
        }

        if(size.y > lv_area_get_height(&txt_coords) && hor_anim == false) {
            lv_anim_set_values(&a, 0, -size.y - (lv_font_get_line_height(font)));
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_label_set_offset_y);
            lv_anim_set_time(&a, lv_anim_speed_to_time(label->anim_speed, a.start, a.end));

            lv_anim_t * anim_cur = lv_anim_get(label, (lv_anim_exec_xcb_t)lv_label_set_offset_y);
            int32_t act_time = anim_cur ? anim_cur->act_time : 0;
            if(act_time < a.time) {
                a.act_time = act_time;      /*To keep the old position*/
                a.early_apply = 0;
            }

            lv_anim_start(&a);
        }
        else {
            /*Delete the offset animation if not required*/
            lv_anim_del(label, (lv_anim_exec_xcb_t)lv_label_set_offset_y);
            label->offset.y = 0;
        }
#endif
    }
    else if(label->long_mode == LV_LABEL_LONG_DOT) {
        if(size.y <= lv_area_get_height(&txt_coords)) { /*No dots are required, the text is short enough*/
            label->dot_end = LV_LABEL_DOT_END_INV;
        }
        else if(_lv_txt_get_encoded_length(label->text) <= LV_LABEL_DOT_NUM) {   /*Don't turn to dots all the characters*/
            label->dot_end = LV_LABEL_DOT_END_INV;
        }
        else {
            lv_point_t p;
            lv_coord_t y_overed;
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

            uint32_t letter_id = lv_label_get_letter_on(obj, &p);


            /*Be sure there is space for the dots*/
            size_t txt_len = strlen(label->text);
            uint32_t byte_id     = _lv_txt_encoded_get_byte_id(label->text, letter_id);
            while(byte_id + LV_LABEL_DOT_NUM > txt_len) {
                _lv_txt_encoded_prev(label->text, &byte_id);
                letter_id--;
            }

            /*Save letters under the dots and replace them with dots*/
            uint32_t byte_id_ori = byte_id;
            uint32_t i;
            uint8_t len          = 0;
            for(i = 0; i <= LV_LABEL_DOT_NUM; i++) {
                len += _lv_txt_encoded_size(&label->text[byte_id]);
                _lv_txt_encoded_next(label->text, &byte_id);
            }

            if(lv_label_set_dot_tmp(obj, &label->text[byte_id_ori], len)) {
                for(i = 0; i < LV_LABEL_DOT_NUM; i++) {
                    label->text[byte_id_ori + i] = '.';
                }
                label->text[byte_id_ori + LV_LABEL_DOT_NUM] = '\0';
                label->dot_end                              = letter_id + LV_LABEL_DOT_NUM;
            }
        }
    }
    /*In break mode only the height can change*/
    else if(label->long_mode == LV_LABEL_LONG_WRAP) {
        size.y += lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
        lv_obj_set_height(obj, size.y);
    }
    /*Do not set the size in Clip mode*/
    else if(label->long_mode == LV_LABEL_LONG_CLIP) {
        /*Do nothing*/
    }

    lv_obj_invalidate(obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void lv_label_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("label create started");

    LV_CLASS_CONSTRUCTOR_BEGIN(obj, lv_label)
    lv_label.base_p->constructor(obj, parent, copy);

    lv_label_t * label = (lv_label_t *) obj;

    label->text       = NULL;
    label->static_txt = 0;
    label->recolor    = 0;
    label->align      = LV_LABEL_ALIGN_AUTO;
    label->dot_end    = LV_LABEL_DOT_END_INV;
    label->long_mode  = LV_LABEL_LONG_EXPAND;
#if LV_USE_ANIMATION
    label->anim_speed = LV_LABEL_DEF_SCROLL_SPEED;
#endif
    label->offset.x = 0;
    label->offset.y = 0;

#if LV_LABEL_LONG_TXT_HINT
    label->hint.line_start = -1;
    label->hint.coord_y    = 0;
    label->hint.y          = 0;
#endif

#if LV_LABEL_TEXT_SEL
    label->sel_start = LV_DRAW_LABEL_NO_TXT_SEL;
    label->sel_end   = LV_DRAW_LABEL_NO_TXT_SEL;
#endif
    label->dot.tmp_ptr   = NULL;
    label->dot_tmp_alloc = 0;

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_label_set_long_mode(obj, LV_LABEL_LONG_EXPAND);
    lv_label_set_text(obj, "Text");

    LV_CLASS_CONSTRUCTOR_END(obj, lv_label)

    LV_LOG_INFO("label created");
}

static void lv_label_destructor(void * obj)
{
//    lv_label_dot_tmp_free(label->dot_...);
//    lv_label_dot_tmp_free(label->text);

//    lv_bar_t * bar = obj;
//
//    _lv_obj_reset_style_list_no_refr(obj, LV_PART_INDICATOR);
//#if LV_USE_ANIMATION
//    lv_anim_del(&bar->cur_value_anim, NULL);
//    lv_anim_del(&bar->start_value_anim, NULL);
//#endif

//    bar->class_p->base_p->destructor(obj);
}

/**
 * Handle the drawing related tasks of the labels
 * @param label pointer to a label object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_label_design(lv_obj_t * obj, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /* A label never covers an area */
    if(mode == LV_DESIGN_COVER_CHK)
        return LV_DESIGN_RES_NOT_COVER;
    else if(mode == LV_DESIGN_DRAW_MAIN) {

        lv_label_t * label = (lv_label_t *) obj;

        lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
        lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
        lv_area_t bg_coords;
        lv_area_copy(&bg_coords, &label->coords);
        bg_coords.x1 -= w;
        bg_coords.x2 += w;
        bg_coords.y1 -= h;
        bg_coords.y2 += h;

        lv_draw_rect_dsc_t draw_rect_dsc;
        lv_draw_rect_dsc_init(&draw_rect_dsc);
        lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &draw_rect_dsc);

        lv_draw_rect(&bg_coords, clip_area, &draw_rect_dsc);

        lv_area_t txt_coords;
        get_txt_coords(obj, &txt_coords);

        lv_area_t txt_clip;
        bool is_common = _lv_area_intersect(&txt_clip, clip_area, &txt_coords);
        if(!is_common) return LV_DESIGN_RES_OK;

        lv_label_align_t align = lv_label_get_align(obj);

        lv_txt_flag_t flag = LV_TXT_FLAG_NONE;
        if(label->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
        if(label->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
        if(label->long_mode == LV_LABEL_LONG_EXPAND) flag |= LV_TXT_FLAG_FIT;
        if(align == LV_LABEL_ALIGN_CENTER) flag |= LV_TXT_FLAG_CENTER;
        if(align == LV_LABEL_ALIGN_RIGHT) flag |= LV_TXT_FLAG_RIGHT;

        lv_draw_label_dsc_t label_draw_dsc;
        lv_draw_label_dsc_init(&label_draw_dsc);

        label_draw_dsc.sel_start = lv_label_get_text_sel_start(obj);
        label_draw_dsc.sel_end = lv_label_get_text_sel_end(obj);
        label_draw_dsc.ofs_x = label->offset.x;
        label_draw_dsc.ofs_y = label->offset.y;
        label_draw_dsc.flag = flag;
        lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &label_draw_dsc);

        /* In SROLL and SROLL_CIRC mode the CENTER and RIGHT are pointless so remove them.
         * (In addition they will result misalignment is this case)*/
        if((label->long_mode == LV_LABEL_LONG_SROLL || label->long_mode == LV_LABEL_LONG_SROLL_CIRC) &&
           (label->align == LV_LABEL_ALIGN_CENTER || label->align == LV_LABEL_ALIGN_RIGHT)) {
            lv_point_t size;
            _lv_txt_get_size(&size, label->text, label_draw_dsc.font, label_draw_dsc.letter_space, label_draw_dsc.line_space,
                             LV_COORD_MAX, flag);
            if(size.x > lv_area_get_width(&txt_coords)) {
                label_draw_dsc.flag &= ~LV_TXT_FLAG_RIGHT;
                label_draw_dsc.flag &= ~LV_TXT_FLAG_CENTER;
            }
        }
#if LV_LABEL_LONG_TXT_HINT
        lv_draw_label_hint_t * hint = &label->hint;
        if(label->long_mode == LV_LABEL_LONG_SROLL_CIRC || lv_area_get_height(&txt_coords) < LV_LABEL_HINT_HEIGHT_LIMIT)
            hint = NULL;

#else
        /*Just for compatibility*/
        lv_draw_label_hint_t * hint = NULL;
#endif

        lv_draw_label(&txt_coords, &txt_clip, &label_draw_dsc, label->text, hint);

        if(label->long_mode == LV_LABEL_LONG_SROLL_CIRC) {
            lv_point_t size;
            _lv_txt_get_size(&size, label->text, label_draw_dsc.font, label_draw_dsc.letter_space, label_draw_dsc.line_space,
                             LV_COORD_MAX, flag);

            /*Draw the text again nlabel to the original to make an circular effect */
            if(size.x > lv_area_get_width(&txt_coords)) {
                label_draw_dsc.ofs_x = label->offset.x + size.x +
                                       lv_font_get_glyph_width(label_draw_dsc.font, ' ', ' ') * LV_LABEL_WAIT_CHAR_COUNT;
                label_draw_dsc.ofs_y = label->offset.y;

                lv_draw_label(&txt_coords, &txt_clip, &label_draw_dsc, label->text, hint);
            }

            /*Draw the text again below the original to make an circular effect */
            if(size.y > lv_area_get_height(&txt_coords)) {
                label_draw_dsc.ofs_x = label->offset.x;
                label_draw_dsc.ofs_y = label->offset.y + size.y + lv_font_get_line_height(label_draw_dsc.font);

                lv_draw_label(&txt_coords, &txt_clip, &label_draw_dsc, label->text, hint);
            }
        }
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the label
 * @param label pointer to a label object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_label_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = lv_label.base_p->signal_cb(obj, sign, param);
    if(res != LV_RES_OK) return res;


    lv_label_t * label = (lv_label_t *) obj;

    if(sign == LV_SIGNAL_STYLE_CHG) {
        /*Revert dots for proper refresh*/
        lv_label_revert_dots(obj);
        lv_label_refr_text(obj);
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {
        if(lv_area_get_width(&label->coords) != lv_area_get_width(param) ||
           lv_area_get_height(&label->coords) != lv_area_get_height(param)) {
            lv_label_revert_dots(obj);
            lv_label_refr_text(obj);
        }
    }
    else if(sign == LV_SIGNAL_BASE_DIR_CHG) {
#if LV_USE_BIDI
        if(label->static_txt == 0) lv_label_set_text(label, NULL);
#endif
    }

    return res;
}

static void lv_label_revert_dots(lv_obj_t * obj)
{

    lv_label_t * label = (lv_label_t *) obj;

    if(label->long_mode != LV_LABEL_LONG_DOT) return;
    if(label->dot_end == LV_LABEL_DOT_END_INV) return;
    uint32_t letter_i = label->dot_end - LV_LABEL_DOT_NUM;
    uint32_t byte_i   = _lv_txt_encoded_get_byte_id(label->text, letter_i);

    /*Restore the characters*/
    uint8_t i      = 0;
    char * dot_tmp = lv_label_get_dot_tmp(obj);
    while(label->text[byte_i + i] != '\0') {
        label->text[byte_i + i] = dot_tmp[i];
        i++;
    }
    label->text[byte_i + i] = dot_tmp[i];
    lv_label_dot_tmp_free(obj);

    label->dot_end = LV_LABEL_DOT_END_INV;
}

#if LV_USE_ANIMATION
static void lv_label_set_offset_x(lv_obj_t * obj, lv_coord_t x)
{
    lv_label_t * label = (lv_label_t *) obj;
    label->offset.x        = x;
    lv_obj_invalidate(obj);
}

static void lv_label_set_offset_y(lv_obj_t * obj, lv_coord_t y)
{
    lv_label_t * label = (lv_label_t *) obj;
    label->offset.y        = y;
    lv_obj_invalidate(obj);
}
#endif

/**
 * Store `len` characters from `data`. Allocates space if necessary.
 *
 * @param label pointer to label object
 * @param len Number of characters to store.
 * @return true on success.
 */
static bool lv_label_set_dot_tmp(lv_obj_t * obj, char * data, uint32_t len)
{

    lv_label_t * label = (lv_label_t *) obj;
    lv_label_dot_tmp_free(obj); /* Deallocate any existing space */
    if(len > sizeof(char *)) {
        /* Memory needs to be allocated. Allocates an additional byte
         * for a NULL-terminator so it can be copied. */
        label->dot.tmp_ptr = lv_mem_alloc(len + 1);
        if(label->dot.tmp_ptr == NULL) {
            LV_LOG_ERROR("Failed to allocate memory for dot_tmp_ptr");
            return false;
        }
        _lv_memcpy(label->dot.tmp_ptr, data, len);
        label->dot.tmp_ptr[len] = '\0';
        label->dot_tmp_alloc    = true;
    }
    else {
        /* Characters can be directly stored in object */
        label->dot_tmp_alloc = false;
        _lv_memcpy(label->dot.tmp, data, len);
    }
    return true;
}

/**
 * Get the stored dot_tmp characters
 * @param label pointer to label object
 * @return char pointer to a stored characters. Is *not* necessarily NULL-terminated.
 */
static char * lv_label_get_dot_tmp(lv_obj_t * obj)
{
    lv_label_t * label = (lv_label_t *) obj;
    if(label->dot_tmp_alloc) {
        return label->dot.tmp_ptr;
    }
    else {
        return label->dot.tmp;
    }
}

/**
 * Free the dot_tmp_ptr field if it was previously allocated.
 * Always clears the field
 * @param label pointer to label object.
 */
static void lv_label_dot_tmp_free(lv_obj_t * obj)
{
    lv_label_t * label = (lv_label_t *) obj;
    if(label->dot_tmp_alloc && label->dot.tmp_ptr) {
        lv_mem_free(label->dot.tmp_ptr);
    }
    label->dot_tmp_alloc = false;
    label->dot.tmp_ptr   = NULL;
}

static void get_txt_coords(const lv_obj_t * obj, lv_area_t * area)
{
    lv_obj_get_coords(obj, area);

    lv_coord_t left   = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t right  = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    lv_coord_t top    = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
    area->x1 += left;
    area->x2 -= right;
    area->y1 += top;
    area->y2 -= bottom;
}

#endif
