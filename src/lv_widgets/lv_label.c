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

#define LV_LABEL_DOT_END_INV 0xFFFF
#define LV_LABEL_HINT_HEIGHT_LIMIT                                                                                     \
    1024 /*Enable "hint" to buffer info about labels larger than this. (Speed up their drawing)*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
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
static lv_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a label objects
 * @param par pointer to an object, it will be the parent of the new label
 * @param copy pointer to a label object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t * lv_label_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("label create started");

    /*Create a basic object*/
    lv_obj_t * new_label = lv_obj_create(par, copy);
    LV_ASSERT_MEM(new_label);
    if(new_label == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_label);

    /*Extend the basic object to a label object*/
    lv_obj_allocate_ext_attr(new_label, sizeof(lv_label_ext_t));

    lv_label_ext_t * ext = lv_obj_get_ext_attr(new_label);
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(new_label);
        return NULL;
    }

    ext->text       = NULL;
    ext->static_txt = 0;
    ext->recolor    = 0;
    ext->align      = LV_LABEL_ALIGN_AUTO;
    ext->dot_end    = LV_LABEL_DOT_END_INV;
    ext->long_mode  = LV_LABEL_LONG_EXPAND;
#if LV_USE_ANIMATION
    ext->anim_speed = LV_LABEL_DEF_SCROLL_SPEED;
#endif
    ext->offset.x = 0;
    ext->offset.y = 0;

#if LV_LABEL_LONG_TXT_HINT
    ext->hint.line_start = -1;
    ext->hint.coord_y    = 0;
    ext->hint.y          = 0;
#endif

#if LV_LABEL_TEXT_SEL
    ext->sel_start = LV_DRAW_LABEL_NO_TXT_SEL;
    ext->sel_end   = LV_DRAW_LABEL_NO_TXT_SEL;
#endif
    ext->dot.tmp_ptr   = NULL;
    ext->dot_tmp_alloc = 0;

    lv_obj_set_design_cb(new_label, lv_label_design);
    lv_obj_set_signal_cb(new_label, lv_label_signal);

    /*Init the new label*/
    if(copy == NULL) {
        lv_theme_apply(new_label, LV_THEME_LABEL);
        lv_obj_set_click(new_label, false);
        lv_label_set_long_mode(new_label, LV_LABEL_LONG_EXPAND);
        lv_label_set_text(new_label, "Text");
    }
    /*Copy 'copy' if not NULL*/
    else {
        lv_label_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        lv_label_set_long_mode(new_label, lv_label_get_long_mode(copy));
        lv_label_set_recolor(new_label, lv_label_get_recolor(copy));
        lv_label_set_align(new_label, lv_label_get_align(copy));
        if(copy_ext->static_txt == 0)
            lv_label_set_text(new_label, lv_label_get_text(copy));
        else
            lv_label_set_text_static(new_label, lv_label_get_text(copy));

        /*In DOT mode save the text byte-to-byte because a '\0' can be in the middle*/
        if(copy_ext->long_mode == LV_LABEL_LONG_DOT) {
            ext->text = lv_mem_realloc(ext->text, _lv_mem_get_size(copy_ext->text));
            LV_ASSERT_MEM(ext->text);
            if(ext->text == NULL) return NULL;
            _lv_memcpy(ext->text, copy_ext->text, _lv_mem_get_size(copy_ext->text));
        }

        if(copy_ext->dot_tmp_alloc && copy_ext->dot.tmp_ptr) {
            uint32_t len = (uint32_t)strlen(copy_ext->dot.tmp_ptr);
            lv_label_set_dot_tmp(new_label, ext->dot.tmp_ptr, len);
        }
        else {
            _lv_memcpy(ext->dot.tmp, copy_ext->dot.tmp, sizeof(ext->dot.tmp));
        }
        ext->dot_tmp_alloc = copy_ext->dot_tmp_alloc;
        ext->dot_end       = copy_ext->dot_end;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_label, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    LV_LOG_INFO("label created");

    return new_label;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param text '\0' terminated character string. NULL to refresh with the current text.
 */
void lv_label_set_text(lv_obj_t * label, const char * text)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

    lv_obj_invalidate(label);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    /*If text is NULL then just refresh with the current text */
    if(text == NULL) text = ext->text;

    LV_ASSERT_STR(text);

    if(ext->text == text && ext->static_txt == 0) {
        /*If set its own text then reallocate it (maybe its size changed)*/
#if LV_USE_ARABIC_PERSIAN_CHARS
        /*Get the size of the text and process it*/
        size_t len = _lv_txt_ap_calc_bytes_cnt(text);

        ext->text = lv_mem_realloc(ext->text, len);
        LV_ASSERT_MEM(ext->text);
        if(ext->text == NULL) return;

        _lv_txt_ap_proc(ext->text, ext->text);
#else
        ext->text = lv_mem_realloc(ext->text, strlen(ext->text) + 1);
#endif

        LV_ASSERT_MEM(ext->text);
        if(ext->text == NULL) return;
    }
    else {
        /*Free the old text*/
        if(ext->text != NULL && ext->static_txt == 0) {
            lv_mem_free(ext->text);
            ext->text = NULL;
        }

#if LV_USE_ARABIC_PERSIAN_CHARS
        /*Get the size of the text and process it*/
        size_t len = _lv_txt_ap_calc_bytes_cnt(text);

        ext->text = lv_mem_alloc(len);
        LV_ASSERT_MEM(ext->text);
        if(ext->text == NULL) return;

        _lv_txt_ap_proc(text, ext->text);
#else
        /*Get the size of the text*/
        size_t len = strlen(text) + 1;

        /*Allocate space for the new text*/
        ext->text = lv_mem_alloc(len);
        LV_ASSERT_MEM(ext->text);
        if(ext->text == NULL) return;
        strcpy(ext->text, text);
#endif

        /*Now the text is dynamically allocated*/
        ext->static_txt = 0;
    }

    lv_label_refr_text(label);
}

/**
 * Set a new formatted text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param fmt `printf`-like format
 */
void lv_label_set_text_fmt(lv_obj_t * label, const char * fmt, ...)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);
    LV_ASSERT_STR(fmt);

    lv_obj_invalidate(label);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    /*If text is NULL then refresh */
    if(fmt == NULL) {
        lv_label_refr_text(label);
        return;
    }

    if(ext->text != NULL && ext->static_txt == 0) {
        lv_mem_free(ext->text);
        ext->text = NULL;
    }

    va_list args;
    va_start(args, fmt);
    ext->text = _lv_txt_set_text_vfmt(fmt, args);
    va_end(args);
    ext->static_txt = 0; /*Now the text is dynamically allocated*/

    lv_label_refr_text(label);
}

/**
 * Set a static text. It will not be saved by the label so the 'text' variable
 * has to be 'alive' while the label exist.
 * @param label pointer to a label object
 * @param text pointer to a text. NULL to refresh with the current text.
 */
void lv_label_set_text_static(lv_obj_t * label, const char * text)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->static_txt == 0 && ext->text != NULL) {
        lv_mem_free(ext->text);
        ext->text = NULL;
    }

    if(text != NULL) {
        ext->static_txt = 1;
        ext->text       = (char *)text;
    }

    lv_label_refr_text(label);
}

/**
 * Set the behavior of the label with longer text then the object size
 * @param label pointer to a label object
 * @param long_mode the new mode from 'lv_label_long_mode' enum.
 *                  In LV_LONG_BREAK/LONG/ROLL the size of the label should be set AFTER this
 * function
 */
void lv_label_set_long_mode(lv_obj_t * label, lv_label_long_mode_t long_mode)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

#if LV_USE_ANIMATION
    /*Delete the old animation (if exists)*/
    lv_anim_del(label, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_del(label, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_del(label, (lv_anim_exec_xcb_t)lv_label_set_offset_x);
    lv_anim_del(label, (lv_anim_exec_xcb_t)lv_label_set_offset_y);
#endif
    ext->offset.x = 0;
    ext->offset.y = 0;

    if(long_mode == LV_LABEL_LONG_SROLL || long_mode == LV_LABEL_LONG_SROLL_CIRC || long_mode == LV_LABEL_LONG_CROP)
        ext->expand = 1;
    else
        ext->expand = 0;

    /*Restore the character under the dots*/
    if(ext->long_mode == LV_LABEL_LONG_DOT && ext->dot_end != LV_LABEL_DOT_END_INV) {
        lv_label_revert_dots(label);
    }

    ext->long_mode = long_mode;
    lv_label_refr_text(label);
}

/**
 * Set the align of the label (left or center)
 * @param label pointer to a label object
 * @param align 'LV_LABEL_ALIGN_LEFT' or 'LV_LABEL_ALIGN_LEFT'
 */
void lv_label_set_align(lv_obj_t * label, lv_label_align_t align)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->align == align) return;

    ext->align = align;

    lv_obj_invalidate(label); /*Enough to invalidate because alignment is only drawing related
                                 (lv_refr_label_text() not required)*/
}

/**
 * Enable the recoloring by in-line commands
 * @param label pointer to a label object
 * @param en true: enable recoloring, false: disable
 */
void lv_label_set_recolor(lv_obj_t * label, bool en)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->recolor == en) return;

    ext->recolor = en == false ? 0 : 1;

    lv_label_refr_text(label); /*Refresh the text because the potential color codes in text needs to
                                  be hidden or revealed*/
}

/**
 * Set the label's animation speed in LV_LABEL_LONG_SROLL/SROLL_CIRC modes
 * @param label pointer to a label object
 * @param anim_speed speed of animation in px/sec unit
 */
void lv_label_set_anim_speed(lv_obj_t * label, uint16_t anim_speed)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->anim_speed == anim_speed) return;

    ext->anim_speed = anim_speed;

    if(ext->long_mode == LV_LABEL_LONG_SROLL || ext->long_mode == LV_LABEL_LONG_SROLL_CIRC) {
        lv_label_refr_text(label);
    }
#else
    (void)label;      /*Unused*/
    (void)anim_speed; /*Unused*/
#endif
}

void lv_label_set_text_sel_start(lv_obj_t * label, uint32_t index)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    ext->sel_start   = index;
    lv_obj_invalidate(label);
#else
    (void)label;    /*Unused*/
    (void)index;    /*Unused*/
#endif
}

void lv_label_set_text_sel_end(lv_obj_t * label, uint32_t index)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    ext->sel_end     = index;
    lv_obj_invalidate(label);
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
char * lv_label_get_text(const lv_obj_t * label)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    return ext->text;
}

/**
 * Get the long mode of a label
 * @param label pointer to a label object
 * @return the long mode
 */
lv_label_long_mode_t lv_label_get_long_mode(const lv_obj_t * label)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    return ext->long_mode;
}

/**
 * Get the align attribute
 * @param label pointer to a label object
 * @return LV_LABEL_ALIGN_LEFT or LV_LABEL_ALIGN_CENTER
 */
lv_label_align_t lv_label_get_align(const lv_obj_t * label)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    lv_label_align_t align = ext->align;

    if(align == LV_LABEL_ALIGN_AUTO) {
#if LV_USE_BIDI
        lv_bidi_dir_t base_dir = lv_obj_get_base_dir(label);
        if(base_dir == LV_BIDI_DIR_AUTO) base_dir = _lv_bidi_detect_base_dir(ext->text);

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
bool lv_label_get_recolor(const lv_obj_t * label)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    return ext->recolor == 0 ? false : true;
}

/**
 * Get the label's animation speed in LV_LABEL_LONG_ROLL and SCROLL modes
 * @param label pointer to a label object
 * @return speed of animation in px/sec unit
 */
uint16_t lv_label_get_anim_speed(const lv_obj_t * label)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    return ext->anim_speed;
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
void lv_label_get_letter_pos(const lv_obj_t * label, uint32_t char_id, lv_point_t * pos)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);
    LV_ASSERT_NULL(pos);

    const char * txt         = lv_label_get_text(label);
    lv_label_align_t align = lv_label_get_align(label);

    if(txt[0] == '\0') {
        pos->y = 0;
        switch(align) {
            case LV_LABEL_ALIGN_LEFT:
                pos->x = 0;
                break;
            case LV_LABEL_ALIGN_RIGHT:
                pos->x = lv_obj_get_width(label);
                break;
            case LV_LABEL_ALIGN_CENTER:
                pos->x = lv_obj_get_width(label) / 2;
                break;
        }
        return;
    }

    lv_area_t txt_coords;
    get_txt_coords(label, &txt_coords);

    lv_label_ext_t * ext     = lv_obj_get_ext_attr(label);
    uint32_t line_start      = 0;
    uint32_t new_line_start  = 0;
    lv_coord_t max_w         = lv_area_get_width(&txt_coords);
    const lv_font_t * font   = lv_obj_get_style_text_font(label, LV_LABEL_PART_MAIN);
    lv_style_int_t line_space = lv_obj_get_style_text_line_space(label, LV_LABEL_PART_MAIN);
    lv_style_int_t letter_space = lv_obj_get_style_text_letter_space(label, LV_LABEL_PART_MAIN);
    lv_coord_t letter_height    = lv_font_get_line_height(font);
    lv_coord_t y             = 0;
    lv_txt_flag_t flag       = LV_TXT_FLAG_NONE;

    if(ext->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
    if(ext->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
    if(ext->long_mode == LV_LABEL_LONG_EXPAND) flag |= LV_TXT_FLAG_FIT;

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
uint32_t lv_label_get_letter_on(const lv_obj_t * label, lv_point_t * pos_in)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);
    LV_ASSERT_NULL(pos_in);

    lv_point_t pos;
    pos.x = pos_in->x - lv_obj_get_style_pad_left(label, LV_LABEL_PART_MAIN);
    pos.y = pos_in->y - lv_obj_get_style_pad_top(label, LV_LABEL_PART_MAIN);

    lv_area_t txt_coords;
    get_txt_coords(label, &txt_coords);
    const char * txt         = lv_label_get_text(label);
    lv_label_ext_t * ext     = lv_obj_get_ext_attr(label);
    uint32_t line_start      = 0;
    uint32_t new_line_start  = 0;
    lv_coord_t max_w         = lv_area_get_width(&txt_coords);
    const lv_font_t * font   = lv_obj_get_style_text_font(label, LV_LABEL_PART_MAIN);
    lv_style_int_t line_space = lv_obj_get_style_text_line_space(label, LV_LABEL_PART_MAIN);
    lv_style_int_t letter_space = lv_obj_get_style_text_letter_space(label, LV_LABEL_PART_MAIN);
    lv_coord_t letter_height    = lv_font_get_line_height(font);
    lv_coord_t y             = 0;
    lv_txt_flag_t flag       = LV_TXT_FLAG_NONE;
    uint32_t logical_pos;
    char * bidi_txt;

    if(ext->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
    if(ext->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
    if(ext->long_mode == LV_LABEL_LONG_EXPAND) flag |= LV_TXT_FLAG_FIT;

    lv_label_align_t align = lv_label_get_align(label);
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

            /*Get the next letter too for kerning*/
            uint32_t letter_next = _lv_txt_encoded_next(&bidi_txt[i], NULL);

            /*Handle the recolor command*/
            if((flag & LV_TXT_FLAG_RECOLOR) != 0) {
                if(_lv_txt_is_cmd(&cmd_state, bidi_txt[i]) != false) {
                    continue; /*Skip the letter is it is part of a command*/
                }
            }

            lv_coord_t gw = lv_font_get_glyph_width(font, letter, letter_next);

            /*Finish if the x position or the last char of the next line is reached*/
            if(pos.x < x + gw || i + line_start == new_line_start || txt[i_act + line_start] == '\0') {
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

    return logical_pos + _lv_txt_encoded_get_char_id(txt, line_start);
}

/**
 * @brief Get the selection start index.
 * @param label pointer to a label object.
 * @return selection start index. `LV_LABEL_TXT_SEL_OFF` if nothing is selected.
 */
uint32_t lv_label_get_text_sel_start(const lv_obj_t * label)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    return ext->sel_start;

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
uint32_t lv_label_get_text_sel_end(const lv_obj_t * label)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

#if LV_LABEL_TEXT_SEL
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    return ext->sel_end;
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
bool lv_label_is_char_under_pos(const lv_obj_t * label, lv_point_t * pos)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);
    LV_ASSERT_NULL(pos);

    lv_area_t txt_coords;
    get_txt_coords(label, &txt_coords);
    const char * txt         = lv_label_get_text(label);
    lv_label_ext_t * ext     = lv_obj_get_ext_attr(label);
    uint32_t line_start      = 0;
    uint32_t new_line_start  = 0;
    lv_coord_t max_w         = lv_area_get_width(&txt_coords);
    const lv_font_t * font   = lv_obj_get_style_text_font(label, LV_LABEL_PART_MAIN);
    lv_style_int_t line_space = lv_obj_get_style_text_line_space(label, LV_LABEL_PART_MAIN);
    lv_style_int_t letter_space = lv_obj_get_style_text_letter_space(label, LV_LABEL_PART_MAIN);
    lv_coord_t letter_height    = lv_font_get_line_height(font);
    lv_coord_t y             = 0;
    lv_txt_flag_t flag       = LV_TXT_FLAG_NONE;
    lv_label_align_t align = lv_label_get_align(label);

    if(ext->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
    if(ext->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
    if(ext->long_mode == LV_LABEL_LONG_EXPAND) flag |= LV_TXT_FLAG_FIT;
    if(align == LV_LABEL_ALIGN_CENTER) flag |= LV_TXT_FLAG_CENTER;

    /*Search the line of the index letter */;
    while(txt[line_start] != '\0') {
        new_line_start += _lv_txt_get_next_line(&txt[line_start], font, letter_space, max_w, flag);

        if(pos->y <= y + letter_height) break; /*The line is found (stored in 'line_start')*/
        y += letter_height + line_space;

        line_start = new_line_start;
    }

    /*Calculate the x coordinate*/
    lv_coord_t x      = 0;
    lv_coord_t last_x = 0;
    if(align == LV_LABEL_ALIGN_CENTER) {
        lv_coord_t line_w;
        line_w = _lv_txt_get_width(&txt[line_start], new_line_start - line_start, font, letter_space, flag);
        x += lv_area_get_width(&txt_coords) / 2 - line_w / 2;
    }
    else if(align == LV_LABEL_ALIGN_RIGHT) {
        lv_coord_t line_w;
        line_w = _lv_txt_get_width(&txt[line_start], new_line_start - line_start, font, letter_space, flag);
        x += lv_area_get_width(&txt_coords) - line_w;
    }

    lv_txt_cmd_state_t cmd_state = LV_TXT_CMD_STATE_WAIT;

    uint32_t i           = line_start;
    uint32_t i_current   = i;
    uint32_t letter      = '\0';
    uint32_t letter_next = '\0';

    if(new_line_start > 0) {
        while(i <= new_line_start - 1) {
            /* Get the current letter
             * Be careful 'i' already points to the next character */
            letter = _lv_txt_encoded_next(txt, &i);

            /*Get the next letter for kerning*/
            letter_next = _lv_txt_encoded_next(&txt[i], NULL);

            /*Handle the recolor command*/
            if((flag & LV_TXT_FLAG_RECOLOR) != 0) {
                if(_lv_txt_is_cmd(&cmd_state, txt[i]) != false) {
                    continue; /*Skip the letter is it is part of a command*/
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

    int32_t max_diff = lv_font_get_glyph_width(font, letter, letter_next) + letter_space + 1;
    return (pos->x >= (last_x - letter_space) && pos->x <= (last_x + max_diff));
}

lv_style_list_t * lv_label_get_style(lv_obj_t * label, uint8_t type)
{
    lv_style_list_t * style_dsc_p;
    switch(type) {
        case LV_LABEL_PART_MAIN:
            style_dsc_p = &label->style_list;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}
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
void lv_label_ins_text(lv_obj_t * label, uint32_t pos, const char * txt)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);
    LV_ASSERT_STR(txt);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    /*Can not append to static text*/
    if(ext->static_txt != 0) return;

    lv_obj_invalidate(label);

    /*Allocate space for the new text*/
    size_t old_len = strlen(ext->text);
    size_t ins_len = strlen(txt);
    size_t new_len = ins_len + old_len;
    ext->text        = lv_mem_realloc(ext->text, new_len + 1);
    LV_ASSERT_MEM(ext->text);
    if(ext->text == NULL) return;

    if(pos == LV_LABEL_POS_LAST) {
        pos = _lv_txt_get_encoded_length(ext->text);
    }

#if LV_USE_BIDI
    char * bidi_buf = _lv_mem_buf_get(ins_len + 1);
    LV_ASSERT_MEM(bidi_buf);
    if(bidi_buf == NULL) return;

    _lv_bidi_process(txt, bidi_buf, lv_obj_get_base_dir(label));
    _lv_txt_ins(ext->text, pos, bidi_buf);

    _lv_mem_buf_release(bidi_buf);
#else
    _lv_txt_ins(ext->text, pos, txt);
#endif
    lv_label_set_text(label, NULL);
}

/**
 * Delete characters from a label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different
 * in UTF-8) 0: before first char.
 * @param cnt number of characters to cut
 */
void lv_label_cut_text(lv_obj_t * label, uint32_t pos, uint32_t cnt)
{
    LV_ASSERT_OBJ(label, LV_OBJX_NAME);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    /*Can not append to static text*/
    if(ext->static_txt != 0) return;

    lv_obj_invalidate(label);

    char * label_txt = lv_label_get_text(label);
    /*Delete the characters*/
    _lv_txt_cut(label_txt, pos, cnt);

    /*Refresh the label*/
    lv_label_refr_text(label);
}

/**
 * Refresh the label with its text stored in its extended data
 * @param label pointer to a label object
 */
void lv_label_refr_text(lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    if(ext->text == NULL) return;
#if LV_LABEL_LONG_TXT_HINT
    ext->hint.line_start = -1; /*The hint is invalid if the text changes*/
#endif

    lv_area_t txt_coords;
    get_txt_coords(label, &txt_coords);
    lv_coord_t max_w         = lv_area_get_width(&txt_coords);
    const lv_font_t * font   = lv_obj_get_style_text_font(label, LV_LABEL_PART_MAIN);
    lv_style_int_t line_space = lv_obj_get_style_text_line_space(label, LV_LABEL_PART_MAIN);
    lv_style_int_t letter_space = lv_obj_get_style_text_letter_space(label, LV_LABEL_PART_MAIN);

    /*Calc. the height and longest line*/
    lv_point_t size;
    lv_txt_flag_t flag = LV_TXT_FLAG_NONE;
    if(ext->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
    if(ext->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
    if(ext->long_mode == LV_LABEL_LONG_EXPAND) flag |= LV_TXT_FLAG_FIT;
    _lv_txt_get_size(&size, ext->text, font, letter_space, line_space, max_w, flag);

    /*Set the full size in expand mode*/
    if(ext->long_mode == LV_LABEL_LONG_EXPAND) {
        size.x += lv_obj_get_style_pad_left(label, LV_LABEL_PART_MAIN) + lv_obj_get_style_pad_right(label, LV_LABEL_PART_MAIN);
        size.y += lv_obj_get_style_pad_top(label, LV_LABEL_PART_MAIN) + lv_obj_get_style_pad_bottom(label, LV_LABEL_PART_MAIN);
        lv_obj_set_size(label, size.x, size.y);
    }
    /*In roll mode keep the size but start offset animations*/
    else if(ext->long_mode == LV_LABEL_LONG_SROLL) {
#if LV_USE_ANIMATION
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, label);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_playback_delay(&a, (((lv_font_get_glyph_width(font, ' ', ' ') + letter_space) * 1000) /
                                        ext->anim_speed) *
                                   LV_LABEL_WAIT_CHAR_COUNT);
        lv_anim_set_repeat_delay(&a, a.playback_delay);

        bool hor_anim = false;
        if(size.x > lv_area_get_width(&txt_coords)) {
#if LV_USE_BIDI
            lv_anim_value_t start, end;
            lv_bidi_dir_t base_dir = lv_obj_get_base_dir(label);

            if(base_dir == LV_BIDI_DIR_AUTO)
                base_dir = _lv_bidi_detect_base_dir(ext->text);

            if(base_dir == LV_BIDI_DIR_RTL) {
                start = lv_area_get_width(&txt_coords) - size.x;
                end = 0;
            }
            else {
                start = 0;
                end = lv_area_get_width(&txt_coords) - size.x;
            }

            lv_anim_set_values(&a, start, end);
#else
            lv_anim_set_values(&a, 0, lv_area_get_width(&txt_coords) - size.x);
#endif
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_label_set_offset_x);
            lv_anim_set_time(&a, lv_anim_speed_to_time(ext->anim_speed, a.start, a.end));
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
            ext->offset.x = 0;
        }

        if(size.y > lv_area_get_height(&txt_coords) && hor_anim == false) {
            lv_anim_set_values(&a, 0, lv_area_get_height(&txt_coords) - size.y - (lv_font_get_line_height(font)));
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_label_set_offset_y);
            lv_anim_set_time(&a, lv_anim_speed_to_time(ext->anim_speed, a.start, a.end));
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
            ext->offset.y = 0;
        }
#endif
    }
    /*In roll inf. mode keep the size but start offset animations*/
    else if(ext->long_mode == LV_LABEL_LONG_SROLL_CIRC) {
#if LV_USE_ANIMATION
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, label);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

        bool hor_anim = false;
        if(size.x > lv_area_get_width(&txt_coords)) {
#if LV_USE_BIDI
            lv_anim_value_t start, end;
            lv_bidi_dir_t base_dir = lv_obj_get_base_dir(label);

            if(base_dir == LV_BIDI_DIR_AUTO)
                base_dir = _lv_bidi_detect_base_dir(ext->text);

            if(base_dir == LV_BIDI_DIR_RTL) {
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

            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_label_set_offset_x);
            lv_anim_set_time(&a, lv_anim_speed_to_time(ext->anim_speed, a.start, a.end));

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
            ext->offset.x = 0;
        }

        if(size.y > lv_area_get_height(&txt_coords) && hor_anim == false) {
            lv_anim_set_values(&a, 0, -size.y - (lv_font_get_line_height(font)));
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_label_set_offset_y);
            lv_anim_set_time(&a, lv_anim_speed_to_time(ext->anim_speed, a.start, a.end));

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
            ext->offset.y = 0;
        }
#endif
    }
    else if(ext->long_mode == LV_LABEL_LONG_DOT) {
        if(size.y <= lv_area_get_height(&txt_coords)) { /*No dots are required, the text is short enough*/
            ext->dot_end = LV_LABEL_DOT_END_INV;
        }
        else if(_lv_txt_get_encoded_length(ext->text) <= LV_LABEL_DOT_NUM) {   /*Don't turn to dots all the characters*/
            ext->dot_end = LV_LABEL_DOT_END_INV;
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

            uint32_t letter_id = lv_label_get_letter_on(label, &p);

            /*Be sure there is space for the dots*/
            size_t txt_len = strlen(ext->text);
            uint32_t byte_id     = _lv_txt_encoded_get_byte_id(ext->text, letter_id);
            while(byte_id + LV_LABEL_DOT_NUM > txt_len) {
                _lv_txt_encoded_prev(ext->text, &byte_id);
                letter_id--;
            }

            /*Save letters under the dots and replace them with dots*/
            uint32_t byte_id_ori = byte_id;
            uint32_t i;
            uint8_t len          = 0;
            for(i = 0; i <= LV_LABEL_DOT_NUM; i++) {
                len += _lv_txt_encoded_size(&ext->text[byte_id]);
                _lv_txt_encoded_next(ext->text, &byte_id);
            }

            if(lv_label_set_dot_tmp(label, &ext->text[byte_id_ori], len)) {
                for(i = 0; i < LV_LABEL_DOT_NUM; i++) {
                    ext->text[byte_id_ori + i] = '.';
                }
                ext->text[byte_id_ori + LV_LABEL_DOT_NUM] = '\0';
                ext->dot_end                              = letter_id + LV_LABEL_DOT_NUM;
            }
        }
    }
    /*In break mode only the height can change*/
    else if(ext->long_mode == LV_LABEL_LONG_BREAK) {
        size.y += lv_obj_get_style_pad_top(label, LV_LABEL_PART_MAIN) + lv_obj_get_style_pad_bottom(label, LV_LABEL_PART_MAIN);
        lv_obj_set_height(label, size.y);
    }
    /*Do not set the size in Clip mode*/
    else if(ext->long_mode == LV_LABEL_LONG_CROP) {
        /*Do nothing*/
    }

    lv_obj_invalidate(label);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

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
static lv_design_res_t lv_label_design(lv_obj_t * label, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /* A label never covers an area */
    if(mode == LV_DESIGN_COVER_CHK)
        return LV_DESIGN_RES_NOT_COVER;
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

        lv_coord_t w = lv_obj_get_style_transform_width(label, LV_LABEL_PART_MAIN);
        lv_coord_t h = lv_obj_get_style_transform_height(label, LV_LABEL_PART_MAIN);
        lv_area_t bg_coords;
        lv_area_copy(&bg_coords, &label->coords);
        bg_coords.x1 -= w;
        bg_coords.x2 += w;
        bg_coords.y1 -= h;
        bg_coords.y2 += h;

        lv_draw_rect_dsc_t draw_rect_dsc;
        lv_draw_rect_dsc_init(&draw_rect_dsc);
        lv_obj_init_draw_rect_dsc(label, LV_LABEL_PART_MAIN, &draw_rect_dsc);

        lv_draw_rect(&bg_coords, clip_area, &draw_rect_dsc);

        lv_area_t txt_coords;
        get_txt_coords(label, &txt_coords);

        lv_area_t txt_clip;
        bool is_common = _lv_area_intersect(&txt_clip, clip_area, &txt_coords);
        if(!is_common) return LV_DESIGN_RES_OK;

        lv_label_align_t align = lv_label_get_align(label);

        lv_txt_flag_t flag = LV_TXT_FLAG_NONE;
        if(ext->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
        if(ext->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
        if(ext->long_mode == LV_LABEL_LONG_EXPAND) flag |= LV_TXT_FLAG_FIT;
        if(align == LV_LABEL_ALIGN_CENTER) flag |= LV_TXT_FLAG_CENTER;
        if(align == LV_LABEL_ALIGN_RIGHT) flag |= LV_TXT_FLAG_RIGHT;

        lv_draw_label_dsc_t label_draw_dsc;
        lv_draw_label_dsc_init(&label_draw_dsc);

        label_draw_dsc.sel_start = lv_label_get_text_sel_start(label);
        label_draw_dsc.sel_end = lv_label_get_text_sel_end(label);
        label_draw_dsc.ofs_x = ext->offset.x;
        label_draw_dsc.ofs_y = ext->offset.y;
        label_draw_dsc.flag = flag;
        lv_obj_init_draw_label_dsc(label, LV_LABEL_PART_MAIN, &label_draw_dsc);

        /* In SROLL and SROLL_CIRC mode the CENTER and RIGHT are pointless so remove them.
         * (In addition they will result misalignment is this case)*/
        if((ext->long_mode == LV_LABEL_LONG_SROLL || ext->long_mode == LV_LABEL_LONG_SROLL_CIRC) &&
           (ext->align == LV_LABEL_ALIGN_CENTER || ext->align == LV_LABEL_ALIGN_RIGHT)) {
            lv_point_t size;
            _lv_txt_get_size(&size, ext->text, label_draw_dsc.font, label_draw_dsc.letter_space, label_draw_dsc.line_space,
                             LV_COORD_MAX, flag);
            if(size.x > lv_area_get_width(&txt_coords)) {
                label_draw_dsc.flag &= ~LV_TXT_FLAG_RIGHT;
                label_draw_dsc.flag &= ~LV_TXT_FLAG_CENTER;
            }
        }
#if LV_LABEL_LONG_TXT_HINT
        lv_draw_label_hint_t * hint = &ext->hint;
        if(ext->long_mode == LV_LABEL_LONG_SROLL_CIRC || lv_area_get_height(&txt_coords) < LV_LABEL_HINT_HEIGHT_LIMIT)
            hint = NULL;

#else
        /*Just for compatibility*/
        lv_draw_label_hint_t * hint = NULL;
#endif

        lv_draw_label(&txt_coords, &txt_clip, &label_draw_dsc, ext->text, hint);

        if(ext->long_mode == LV_LABEL_LONG_SROLL_CIRC) {
            lv_point_t size;
            _lv_txt_get_size(&size, ext->text, label_draw_dsc.font, label_draw_dsc.letter_space, label_draw_dsc.line_space,
                             LV_COORD_MAX, flag);

            /*Draw the text again next to the original to make an circular effect */
            if(size.x > lv_area_get_width(&txt_coords)) {
                label_draw_dsc.ofs_x = ext->offset.x + size.x +
                                       lv_font_get_glyph_width(label_draw_dsc.font, ' ', ' ') * LV_LABEL_WAIT_CHAR_COUNT;
                label_draw_dsc.ofs_y = ext->offset.y;

                lv_draw_label(&txt_coords, &txt_clip, &label_draw_dsc, ext->text, hint);
            }

            /*Draw the text again below the original to make an circular effect */
            if(size.y > lv_area_get_height(&txt_coords)) {
                label_draw_dsc.ofs_x = ext->offset.x;
                label_draw_dsc.ofs_y = ext->offset.y + size.y + lv_font_get_line_height(label_draw_dsc.font);

                lv_draw_label(&txt_coords, &txt_clip, &label_draw_dsc, ext->text, hint);
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
static lv_res_t lv_label_signal(lv_obj_t * label, lv_signal_t sign, void * param)
{
    lv_res_t res;

    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_label_get_style(label, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(label, sign, param);
    }

    /* Include the ancient signal function */
    res = ancestor_signal(label, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(sign == LV_SIGNAL_CLEANUP) {
        if(ext->static_txt == 0) {
            lv_mem_free(ext->text);
            ext->text = NULL;
        }
        lv_label_dot_tmp_free(label);
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        /*Revert dots for proper refresh*/
        lv_label_revert_dots(label);
        lv_label_refr_text(label);
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {
        if(lv_area_get_width(&label->coords) != lv_area_get_width(param) ||
           lv_area_get_height(&label->coords) != lv_area_get_height(param)) {
            lv_label_revert_dots(label);
            lv_label_refr_text(label);
        }
    }
    else if(sign == LV_SIGNAL_BASE_DIR_CHG) {
#if LV_USE_BIDI
        if(ext->static_txt == 0) lv_label_set_text(label, NULL);
#endif
    }

    return res;
}

static void lv_label_revert_dots(lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->long_mode != LV_LABEL_LONG_DOT) return;
    if(ext->dot_end == LV_LABEL_DOT_END_INV) return;
    uint32_t letter_i = ext->dot_end - LV_LABEL_DOT_NUM;
    uint32_t byte_i   = _lv_txt_encoded_get_byte_id(ext->text, letter_i);

    /*Restore the characters*/
    uint8_t i      = 0;
    char * dot_tmp = lv_label_get_dot_tmp(label);
    while(ext->text[byte_i + i] != '\0') {
        ext->text[byte_i + i] = dot_tmp[i];
        i++;
    }
    ext->text[byte_i + i] = dot_tmp[i];
    lv_label_dot_tmp_free(label);

    ext->dot_end = LV_LABEL_DOT_END_INV;
}

#if LV_USE_ANIMATION
static void lv_label_set_offset_x(lv_obj_t * label, lv_coord_t x)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    ext->offset.x        = x;
    lv_obj_invalidate(label);
}

static void lv_label_set_offset_y(lv_obj_t * label, lv_coord_t y)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    ext->offset.y        = y;
    lv_obj_invalidate(label);
}
#endif

/**
 * Store `len` characters from `data`. Allocates space if necessary.
 *
 * @param label pointer to label object
 * @param len Number of characters to store.
 * @return true on success.
 */
static bool lv_label_set_dot_tmp(lv_obj_t * label, char * data, uint32_t len)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    lv_label_dot_tmp_free(label); /* Deallocate any existing space */
    if(len > sizeof(char *)) {
        /* Memory needs to be allocated. Allocates an additional byte
         * for a NULL-terminator so it can be copied. */
        ext->dot.tmp_ptr = lv_mem_alloc(len + 1);
        if(ext->dot.tmp_ptr == NULL) {
            LV_LOG_ERROR("Failed to allocate memory for dot_tmp_ptr");
            return false;
        }
        _lv_memcpy(ext->dot.tmp_ptr, data, len);
        ext->dot.tmp_ptr[len] = '\0';
        ext->dot_tmp_alloc    = true;
    }
    else {
        /* Characters can be directly stored in object */
        ext->dot_tmp_alloc = false;
        _lv_memcpy(ext->dot.tmp, data, len);
    }
    return true;
}

/**
 * Get the stored dot_tmp characters
 * @param label pointer to label object
 * @return char pointer to a stored characters. Is *not* necessarily NULL-terminated.
 */
static char * lv_label_get_dot_tmp(lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->dot_tmp_alloc) {
        return ext->dot.tmp_ptr;
    }
    else {
        return ext->dot.tmp;
    }
}

/**
 * Free the dot_tmp_ptr field if it was previously allocated.
 * Always clears the field
 * @param label pointer to label object.
 */
static void lv_label_dot_tmp_free(lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->dot_tmp_alloc && ext->dot.tmp_ptr) {
        lv_mem_free(ext->dot.tmp_ptr);
    }
    ext->dot_tmp_alloc = false;
    ext->dot.tmp_ptr   = NULL;
}

static void get_txt_coords(const lv_obj_t * label, lv_area_t * area)
{
    lv_obj_get_coords(label, area);

    lv_coord_t left   = lv_obj_get_style_pad_left(label, LV_LABEL_PART_MAIN);
    lv_coord_t right  = lv_obj_get_style_pad_right(label, LV_LABEL_PART_MAIN);
    lv_coord_t top    = lv_obj_get_style_pad_top(label, LV_LABEL_PART_MAIN);
    lv_coord_t bottom = lv_obj_get_style_pad_bottom(label, LV_LABEL_PART_MAIN);
    area->x1 += left;
    area->x2 -= right;
    area->y1 += top;
    area->y2 -= bottom;
}

#endif
