/**
 * @file lv_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_label.h"
#if USE_LV_LABEL != 0

#include "../lv_core/lv_obj.h"
#include "../lv_core/lv_group.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
/*Test configurations*/
#ifndef LV_LABEL_SCROLL_SPEED
#define LV_LABEL_SCROLL_SPEED       (25) /*Hor, or ver. scroll speed (px/sec) in 'LV_LABEL_LONG_SCROLL/ROLL' mode*/
#endif

#define ANIM_WAIT_CHAR_COUNT 3

#define LV_LABEL_DOT_END_INV 0xFFFF

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_label_signal(lv_obj_t * label, lv_signal_t sign, void * param);
static bool lv_label_design(lv_obj_t * label, const lv_area_t * mask, lv_design_mode_t mode);
static void lv_label_refr_text(lv_obj_t * label);
static void lv_label_revert_dots(lv_obj_t * label);

#if USE_LV_ANIMATION
static void lv_label_set_offset_x(lv_obj_t * label, lv_coord_t x);
static void lv_label_set_offset_y(lv_obj_t * label, lv_coord_t y);
#endif
/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a label objects
 * @param par pointer to an object, it will be the parent of the new label
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t * lv_label_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("label create started");

    /*Create a basic object*/
    lv_obj_t * new_label = lv_obj_create(par, copy);
    lv_mem_assert(new_label);
    if(new_label == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_label);

    /*Extend the basic object to a label object*/
    lv_obj_allocate_ext_attr(new_label, sizeof(lv_label_ext_t));

    lv_label_ext_t * ext = lv_obj_get_ext_attr(new_label);
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->text = NULL;
    ext->static_txt = 0;
    ext->recolor = 0;
    ext->body_draw = 0;
    ext->align = LV_LABEL_ALIGN_LEFT;
    ext->dot_end = LV_LABEL_DOT_END_INV;
    ext->long_mode = LV_LABEL_LONG_EXPAND;
    ext->anim_speed = LV_LABEL_SCROLL_SPEED;
    ext->offset.x = 0;
    ext->offset.y = 0;
    lv_obj_set_design_func(new_label, lv_label_design);
    lv_obj_set_signal_func(new_label, lv_label_signal);

    /*Init the new label*/
    if(copy == NULL) {
        lv_obj_set_click(new_label, false);
        lv_label_set_long_mode(new_label, LV_LABEL_LONG_EXPAND);
        lv_label_set_text(new_label, "Text");
        lv_label_set_style(new_label, NULL);        /*Inherit parent's style*/
    }
    /*Copy 'copy' if not NULL*/
    else {
        lv_label_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        lv_label_set_long_mode(new_label, lv_label_get_long_mode(copy));
        lv_label_set_recolor(new_label, lv_label_get_recolor(copy));
        lv_label_set_body_draw(new_label, lv_label_get_body_draw(copy));
        lv_label_set_align(new_label, lv_label_get_align(copy));
        if(copy_ext->static_txt == 0) lv_label_set_text(new_label, lv_label_get_text(copy));
        else lv_label_set_static_text(new_label, lv_label_get_text(copy));

        /*In DOT mode save the text byte-to-byte because a '\0' can be in the middle*/
        if(copy_ext->long_mode == LV_LABEL_LONG_DOT) {
            ext->text = lv_mem_realloc(ext->text, lv_mem_get_size(copy_ext->text));
            lv_mem_assert(ext->text);
            if(ext->text == NULL) return NULL;
            memcpy(ext->text, copy_ext->text, lv_mem_get_size(copy_ext->text));
        }

        memcpy(ext->dot_tmp, copy_ext->dot_tmp, sizeof(ext->dot_tmp));
        ext->dot_end = copy_ext->dot_end;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_label);
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
    lv_obj_invalidate(label);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    /*If text is NULL then refresh */
    if(text == NULL) {
        lv_label_refr_text(label);
        return;
    }

    if(ext->text == text) {
        /*If set its own text then reallocate it (maybe its size changed)*/
        ext->text = lv_mem_realloc(ext->text, strlen(ext->text) + 1);
        lv_mem_assert(ext->text);
        if(ext->text == NULL) return;
    } else {
        /*Allocate space for the new text*/
        uint32_t len = strlen(text) + 1;
        if(ext->text != NULL && ext->static_txt == 0) {
            lv_mem_free(ext->text);
            ext->text = NULL;
        }

        ext->text = lv_mem_alloc(len);
        lv_mem_assert(ext->text);
        if(ext->text == NULL) return;

        strcpy(ext->text, text);
        ext->static_txt = 0;    /*Now the text is dynamically allocated*/
    }

    lv_label_refr_text(label);
}
/**
 * Set a new text for a label from a character array. The array don't has to be '\0' terminated.
 * Memory will be allocated to store the array by the label.
 * @param label pointer to a label object
 * @param array array of characters or NULL to refresh the label
 * @param size the size of 'array' in bytes
 */
void lv_label_set_array_text(lv_obj_t * label, const char * array, uint16_t size)
{
    lv_obj_invalidate(label);

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    /*If trying to set its own text or the array is NULL then refresh */
    if(array == ext->text || array == NULL) {
        lv_label_refr_text(label);
        return;
    }

    /*Allocate space for the new text*/
    if(ext->text != NULL && ext->static_txt == 0) {
        lv_mem_free(ext->text);
        ext->text = NULL;
    }
    ext->text = lv_mem_alloc(size + 1);
    lv_mem_assert(ext->text);
    if(ext->text == NULL) return;

    memcpy(ext->text, array, size);
    ext->text[size] = '\0';
    ext->static_txt = 0;    /*Now the text is dynamically allocated*/

    lv_label_refr_text(label);
}

/**
 * Set a static text. It will not be saved by the label so the 'text' variable
 * has to be 'alive' while the label exist.
 * @param label pointer to a label object
 * @param text pointer to a text. NULL to refresh with the current text.
 */
void lv_label_set_static_text(lv_obj_t * label, const char * text)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->static_txt == 0 && ext->text != NULL) {
        lv_mem_free(ext->text);
        ext->text = NULL;
    }

    if(text != NULL) {
        ext->static_txt = 1;
        ext->text = (char *) text;
    }

    lv_label_refr_text(label);
}

/**
 * Set the behavior of the label with longer text then the object size
 * @param label pointer to a label object
 * @param long_mode the new mode from 'lv_label_long_mode' enum.
 *                  In LV_LONG_BREAK/LONG/ROLL the size of the label should be set AFTER this function
 */
void lv_label_set_long_mode(lv_obj_t * label, lv_label_long_mode_t long_mode)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

#if USE_LV_ANIMATION
    /*Delete the old animation (if exists)*/
    lv_anim_del(label, (lv_anim_fp_t) lv_obj_set_x);
    lv_anim_del(label, (lv_anim_fp_t) lv_obj_set_y);
    lv_anim_del(label, (lv_anim_fp_t) lv_label_set_offset_x);
    lv_anim_del(label, (lv_anim_fp_t) lv_label_set_offset_y);
#endif
    ext->offset.x = 0;
    ext->offset.y = 0;

    if(long_mode == LV_LABEL_LONG_ROLL || long_mode == LV_LABEL_LONG_CROP) ext->expand = 1;
    else ext->expand = 0;

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
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->align == align) return;

    ext->align = align;

    lv_obj_invalidate(label);       /*Enough to invalidate because alignment is only drawing related (lv_refr_label_text() not required)*/

}

/**
 * Enable the recoloring by in-line commands
 * @param label pointer to a label object
 * @param recolor_en true: enable recoloring, false: disable
 */
void lv_label_set_recolor(lv_obj_t * label, bool recolor_en)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->recolor == recolor_en) return;

    ext->recolor = recolor_en == false ? 0 : 1;

    lv_label_refr_text(label);  /*Refresh the text because the potential colo codes in text needs to be hided or revealed*/
}

/**
 * Set the label to draw (or not draw) background specified in its style's body
 * @param label pointer to a label object
 * @param body_en true: draw body; false: don't draw body
 */
void lv_label_set_body_draw(lv_obj_t * label, bool body_en)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->body_draw == body_en) return;

    ext->body_draw = body_en == false ? 0 : 1;

    lv_obj_refresh_ext_size(label);

    lv_obj_invalidate(label);
}

/**
 * Set the label's animation speed in LV_LABEL_LONG_ROLL and SCROLL modes
 * @param label pointer to a label object
 * @param anim_speed speed of animation in px/sec unit
 */
void lv_label_set_anim_speed(lv_obj_t * label, uint16_t anim_speed)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->anim_speed == anim_speed) return;

    ext->anim_speed = anim_speed;

    if(ext->long_mode == LV_LABEL_LONG_ROLL || ext->long_mode == LV_LABEL_LONG_SCROLL) {
        lv_label_refr_text(label);
    }
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
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    return ext->align;
}

/**
 * Get the recoloring attribute
 * @param label pointer to a label object
 * @return true: recoloring is enabled, false: disable
 */
bool lv_label_get_recolor(const lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    return ext->recolor == 0 ? false : true;
}

/**
 * Get the body draw attribute
 * @param label pointer to a label object
 * @return true: draw body; false: don't draw body
 */
bool lv_label_get_body_draw(const lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    return ext->body_draw == 0 ? false : true;
}

/**
 * Get the label's animation speed in LV_LABEL_LONG_ROLL and SCROLL modes
 * @param label pointer to a label object
 * @return speed of animation in px/sec unit
 */
uint16_t lv_label_get_anim_speed(const lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    return ext->anim_speed;
}

/**
 * Get the relative x and y coordinates of a letter
 * @param label pointer to a label object
 * @param index index of the letter [0 ... text length]. Expressed in character index, not byte index (different in UTF-8)
 * @param pos store the result here (E.g. index = 0 gives 0;0 coordinates)
 */
void lv_label_get_letter_pos(const lv_obj_t * label, uint16_t index, lv_point_t * pos)
{
    const char * txt = lv_label_get_text(label);
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    uint32_t line_start = 0;
    uint32_t new_line_start = 0;
    lv_coord_t max_w = lv_obj_get_width(label);
    lv_style_t * style = lv_obj_get_style(label);
    const lv_font_t * font = style->text.font;
    uint8_t letter_height = lv_font_get_height(font);
    lv_coord_t y = 0;
    lv_txt_flag_t flag = LV_TXT_FLAG_NONE;

    if(ext->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
    if(ext->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
    if(ext->align == LV_LABEL_ALIGN_CENTER) flag |= LV_TXT_FLAG_CENTER;

    /*If the width will be expanded  the set the max length to very big */
    if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
        max_w = LV_COORD_MAX;
    }

    index = lv_txt_encoded_get_byte_id(txt, index);

    /*Search the line of the index letter */;
    while(txt[new_line_start] != '\0') {
        new_line_start += lv_txt_get_next_line(&txt[line_start], font, style->text.letter_space, max_w, flag);
        if(index < new_line_start || txt[new_line_start] == '\0') break; /*The line of 'index' letter begins at 'line_start'*/

        y += letter_height + style->text.line_space;
        line_start = new_line_start;
    }

    /*If the last character is line break then go to the next line*/
    if((txt[index - 1] == '\n' || txt[index - 1] == '\r') && txt[index] == '\0') {
        y += letter_height + style->text.line_space;
        line_start = index;
    }

    /*Calculate the x coordinate*/
    lv_coord_t x = 0;
    uint32_t i = line_start;
    uint32_t cnt = line_start;                      /*Count the letter (in UTF-8 1 letter not 1 byte)*/
    lv_txt_cmd_state_t cmd_state = LV_TXT_CMD_STATE_WAIT;
    uint32_t letter;
    while(cnt < index) {
        cnt += lv_txt_encoded_size(&txt[i]);
        letter = lv_txt_encoded_next(txt, &i);
        /*Handle the recolor command*/
        if((flag & LV_TXT_FLAG_RECOLOR) != 0) {
            if(lv_txt_is_cmd(&cmd_state, txt[i]) != false) {
                continue; /*Skip the letter is it is part of a command*/
            }
        }
        x += lv_font_get_width(font, letter) + style->text.letter_space;
    }

    if(ext->align == LV_LABEL_ALIGN_CENTER) {
        lv_coord_t line_w;
        line_w = lv_txt_get_width(&txt[line_start], new_line_start - line_start,
                                  font, style->text.letter_space, flag);
        x += lv_obj_get_width(label) / 2 - line_w / 2;

    } else if(ext->align == LV_LABEL_ALIGN_RIGHT) {
        lv_coord_t line_w;
        line_w = lv_txt_get_width(&txt[line_start], new_line_start - line_start,
                                  font, style->text.letter_space, flag);

        x += lv_obj_get_width(label) - line_w;
    }

    pos->x = x;
    pos->y = y;
}

/**
 * Get the index of letter on a relative point of a label
 * @param label pointer to label object
 * @param pos pointer to point with coordinates on a the label
 * @return the index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter)
 * Expressed in character index and not byte index (different in UTF-8)
 */
uint16_t lv_label_get_letter_on(const lv_obj_t * label, lv_point_t * pos)
{
    const char * txt = lv_label_get_text(label);
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    uint32_t line_start = 0;
    uint32_t new_line_start = 0;
    lv_coord_t max_w = lv_obj_get_width(label);
    lv_style_t * style = lv_obj_get_style(label);
    const lv_font_t * font = style->text.font;
    uint8_t letter_height = lv_font_get_height(font);
    lv_coord_t y = 0;
    lv_txt_flag_t flag = LV_TXT_FLAG_NONE;

    if(ext->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
    if(ext->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
    if(ext->align == LV_LABEL_ALIGN_CENTER) flag |= LV_TXT_FLAG_CENTER;

    /*If the width will be expanded set the max length to very big */
    if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
        max_w = LV_COORD_MAX;
    }

    /*Search the line of the index letter */;
    while(txt[line_start] != '\0') {
        new_line_start += lv_txt_get_next_line(&txt[line_start], font, style->text.letter_space, max_w, flag);

        if(pos->y <= y + letter_height) break; /*The line is found (stored in 'line_start')*/
        y += letter_height + style->text.line_space;

        line_start = new_line_start;
    }

    /*Calculate the x coordinate*/
    lv_coord_t x = 0;
    if(ext->align == LV_LABEL_ALIGN_CENTER) {
        lv_coord_t line_w;
        line_w = lv_txt_get_width(&txt[line_start], new_line_start - line_start,
                                  font, style->text.letter_space, flag);
        x += lv_obj_get_width(label) / 2 - line_w / 2;
    }

    lv_txt_cmd_state_t cmd_state = LV_TXT_CMD_STATE_WAIT;
    uint32_t i = line_start;
    uint32_t i_current = i;
    uint32_t letter;
    while(i < new_line_start - 1) {
        letter = lv_txt_encoded_next(txt, &i);    /*Be careful 'i' already points to the next character*/
        /*Handle the recolor command*/
        if((flag & LV_TXT_FLAG_RECOLOR) != 0) {
            if(lv_txt_is_cmd(&cmd_state, txt[i]) != false) {
                continue; /*Skip the letter is it is part of a command*/
            }
        }

        x += lv_font_get_width(font, letter);
        if(pos->x < x) {
            i = i_current;
            break;
        }
        x += style->text.letter_space;
        i_current = i;
    }

    return lv_encoded_get_char_id(txt, i);
}


/*=====================
 * Other functions
 *====================*/

/**
 * Insert a text to the label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different in UTF-8)
 *            0: before first char.
 *            LV_LABEL_POS_LAST: after last char.
 * @param txt pointer to the text to insert
 */
void lv_label_ins_text(lv_obj_t * label, uint32_t pos,  const char * txt)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    /*Can not append to static text*/
    if(ext->static_txt != 0) return;

    lv_obj_invalidate(label);

    /*Allocate space for the new text*/
    uint32_t old_len = strlen(ext->text);
    uint32_t ins_len = strlen(txt);
    uint32_t new_len = ins_len + old_len;
    ext->text = lv_mem_realloc(ext->text, new_len + 1);
    lv_mem_assert(ext->text);
    if(ext->text == NULL) return;

    if(pos == LV_LABEL_POS_LAST) {
#if LV_TXT_UTF8 == 0
        pos = old_len;
#else
        pos = lv_txt_get_encoded_length(ext->text);
#endif
    }

    lv_txt_ins(ext->text, pos, txt);

    lv_label_refr_text(label);
}

/**
 * Delete characters from a label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different in UTF-8)
 *            0: before first char.
 * @param cnt number of characters to cut
 */
void lv_label_cut_text(lv_obj_t * label, uint32_t pos,  uint32_t cnt)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    /*Can not append to static text*/
    if(ext->static_txt != 0) return;

    lv_obj_invalidate(label);

    char * label_txt = lv_label_get_text(label);
    /*Delete the characters*/
    lv_txt_cut(label_txt, pos, cnt);

    /*Refresh the label*/
    lv_label_refr_text(label);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the labels
 * @param label pointer to a label object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_label_design(lv_obj_t * label, const lv_area_t * mask, lv_design_mode_t mode)
{
    /* A label never covers an area */
    if(mode == LV_DESIGN_COVER_CHK) return false;
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_area_t coords;
        lv_style_t * style = lv_obj_get_style(label);
        lv_opa_t opa_scale = lv_obj_get_opa_scale(label);
        lv_obj_get_coords(label, &coords);

#if USE_LV_GROUP
        lv_group_t * g = lv_obj_get_group(label);
        if(lv_group_get_focused(g) == label) {
            lv_draw_rect(&coords, mask, style, opa_scale);
        }
#endif

        lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

        if(ext->body_draw) {
            lv_area_t bg;
            lv_obj_get_coords(label, &bg);
            bg.x1 -= style->body.padding.hor;
            bg.x2 += style->body.padding.hor;
            bg.y1 -= style->body.padding.ver;
            bg.y2 += style->body.padding.ver;

            lv_draw_rect(&bg, mask, style, lv_obj_get_opa_scale(label));
        }

        /*TEST: draw a background for the label*/
        //lv_draw_rect(&label->coords, mask, &lv_style_plain_color, LV_OPA_COVER);

        lv_txt_flag_t flag = LV_TXT_FLAG_NONE;
        if(ext->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
        if(ext->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
        if(ext->align == LV_LABEL_ALIGN_CENTER) flag |= LV_TXT_FLAG_CENTER;
        if(ext->align == LV_LABEL_ALIGN_RIGHT) flag |= LV_TXT_FLAG_RIGHT;

        /* In ROLL mode the CENTER and RIGHT are pointless so remove them.
         * (In addition they will result mis-alignment is this case)*/
        if((ext->long_mode == LV_LABEL_LONG_ROLL) &&
                (ext->align == LV_LABEL_ALIGN_CENTER || ext->align == LV_LABEL_ALIGN_RIGHT)) {
            lv_point_t size;
            lv_txt_get_size(&size, ext->text, style->text.font, style->text.letter_space, style->text.line_space, LV_COORD_MAX, flag);
            if(size.x > lv_obj_get_width(label)) {
                flag &= ~LV_TXT_FLAG_RIGHT;
                flag &= ~LV_TXT_FLAG_CENTER;
            }
        }

        lv_draw_label(&coords, mask, style, opa_scale, ext->text, flag, &ext->offset);
    }
    return true;
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

    /* Include the ancient signal function */
    res = ancestor_signal(label, sign, param);
    if(res != LV_RES_OK) return res;

    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(sign ==  LV_SIGNAL_CLEANUP) {
        if(ext->static_txt == 0) {
            lv_mem_free(ext->text);
            ext->text = NULL;
        }
    } else if(sign == LV_SIGNAL_STYLE_CHG) {
        /*Revert dots for proper refresh*/
        lv_label_revert_dots(label);

        lv_label_refr_text(label);
    } else if(sign == LV_SIGNAL_CORD_CHG) {
        if(lv_area_get_width(&label->coords) != lv_area_get_width(param) ||
                lv_area_get_height(&label->coords) != lv_area_get_height(param)) {
            lv_label_revert_dots(label);
            lv_label_refr_text(label);
        }
    } else if(sign == LV_SIGNAL_REFR_EXT_SIZE) {
        if(ext->body_draw) {
            lv_style_t * style = lv_label_get_style(label);
            label->ext_size = LV_MATH_MAX(label->ext_size, style->body.padding.hor);
            label->ext_size = LV_MATH_MAX(label->ext_size, style->body.padding.ver);
        }
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_label";
    }

    return res;
}

/**
 * Refresh the label with its text stored in its extended data
 * @param label pointer to a label object
 */
static void lv_label_refr_text(lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);

    if(ext->text == NULL) return;

    lv_coord_t max_w = lv_obj_get_width(label);
    lv_style_t * style = lv_obj_get_style(label);
    const lv_font_t * font = style->text.font;

    /*If the width will be expanded set the max length to very big */
    if(ext->long_mode == LV_LABEL_LONG_EXPAND ||
            ext->long_mode == LV_LABEL_LONG_SCROLL) {
        max_w = LV_COORD_MAX;
    }

    /*Calc. the height and longest line*/
    lv_point_t size;
    lv_txt_flag_t flag = LV_TXT_FLAG_NONE;
    if(ext->recolor != 0) flag |= LV_TXT_FLAG_RECOLOR;
    if(ext->expand != 0) flag |= LV_TXT_FLAG_EXPAND;
    lv_txt_get_size(&size, ext->text, font, style->text.letter_space, style->text.line_space, max_w, flag);

    /*Set the full size in expand mode*/
    if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
        lv_obj_set_size(label, size.x, size.y);

        /*Start scrolling if the label is greater then its parent*/
        if(ext->long_mode == LV_LABEL_LONG_SCROLL) {
#if USE_LV_ANIMATION
            lv_obj_t * parent = lv_obj_get_parent(label);

            /*Delete the potential previous scroller animations*/
            lv_anim_del(label, (lv_anim_fp_t) lv_obj_set_x);
            lv_anim_del(label, (lv_anim_fp_t) lv_obj_set_y);

            lv_anim_t anim;
            anim.var = label;
            anim.repeat = 1;
            anim.playback = 1;
            anim.start = lv_font_get_width(font, ' ');
            anim.act_time = 0;
            anim.end_cb = NULL;
            anim.path = lv_anim_path_linear;

            anim.playback_pause = (((lv_font_get_width(style->text.font, ' ') +
                                     style->text.letter_space) * 1000) / ext->anim_speed) * ANIM_WAIT_CHAR_COUNT;
            anim.repeat_pause = anim.playback_pause;

            if(lv_obj_get_width(label) > lv_obj_get_width(parent)) {
                anim.end = lv_obj_get_width(parent) - lv_obj_get_width(label) - lv_font_get_width(font, ' ');
                anim.fp = (lv_anim_fp_t) lv_obj_set_x;
                anim.time = lv_anim_speed_to_time(ext->anim_speed, anim.start, anim.end);
                lv_anim_create(&anim);
            } else if(lv_obj_get_height(label) > lv_obj_get_height(parent)) {
                anim.end =  lv_obj_get_height(parent) - lv_obj_get_height(label) - lv_font_get_height(font);
                anim.fp = (lv_anim_fp_t)lv_obj_set_y;
                anim.time = lv_anim_speed_to_time(ext->anim_speed, anim.start, anim.end);
                lv_anim_create(&anim);
            }
#endif
        }
    }
    /*In roll mode keep the size but start offset animations*/
    else if(ext->long_mode == LV_LABEL_LONG_ROLL) {
#if USE_LV_ANIMATION
        lv_anim_t anim;
        anim.var = label;
        anim.repeat = 1;
        anim.playback = 1;
        anim.start = lv_font_get_width(font, ' ');
        anim.act_time = 0;
        anim.end_cb = NULL;
        anim.path = lv_anim_path_linear;
        anim.playback_pause = (((lv_font_get_width(style->text.font, ' ') + style->text.letter_space) * 1000) / ext->anim_speed) * ANIM_WAIT_CHAR_COUNT;;
        anim.repeat_pause =  anim.playback_pause;

        bool hor_anim = false;
        if(size.x > lv_obj_get_width(label)) {
            anim.end = lv_obj_get_width(label) - size.x - lv_font_get_width(font, ' ');
            anim.fp = (lv_anim_fp_t) lv_label_set_offset_x;
            anim.time = lv_anim_speed_to_time(ext->anim_speed, anim.start, anim.end);
            lv_anim_create(&anim);
            hor_anim = true;
        } else {
            /*Delete the offset animation if not required*/
            lv_anim_del(label, (lv_anim_fp_t) lv_label_set_offset_x);
            ext->offset.x = 0;
        }

        if(size.y > lv_obj_get_height(label) && hor_anim == false) {
            anim.end =  lv_obj_get_height(label) - size.y - (lv_font_get_height(font));
            anim.fp = (lv_anim_fp_t)lv_label_set_offset_y;
            anim.time = lv_anim_speed_to_time(ext->anim_speed, anim.start, anim.end);
            lv_anim_create(&anim);
        } else {
            /*Delete the offset animation if not required*/
            lv_anim_del(label, (lv_anim_fp_t) lv_label_set_offset_y);
            ext->offset.y = 0;
        }
#endif
    } else if(ext->long_mode == LV_LABEL_LONG_DOT) {
        if(size.y <= lv_obj_get_height(label)) {                /*No dots are required, the text is short enough*/
            ext->dot_end = LV_LABEL_DOT_END_INV;
        } else if(lv_txt_get_encoded_length(ext->text) <= LV_LABEL_DOT_NUM) {     /*Don't turn to dots all the characters*/
            ext->dot_end = LV_LABEL_DOT_END_INV;
        } else {
            lv_point_t p;
            p.x = lv_obj_get_width(label) - (lv_font_get_width(style->text.font, '.') + style->text.letter_space) * LV_LABEL_DOT_NUM; /*Shrink with dots*/
            p.y = lv_obj_get_height(label);
            p.y -= p.y  % (lv_font_get_height(style->text.font) + style->text.line_space);   /*Round down to the last line*/
            p.y -= style->text.line_space;                                                      /*Trim the last line space*/
            uint32_t letter_id = lv_label_get_letter_on(label, &p);


#if LV_TXT_UTF8 == 0
            /*Save letters under the dots and replace them with dots*/
            uint8_t i;
            for(i = 0; i < LV_LABEL_DOT_NUM; i++)  {
                ext->dot_tmp[i] = ext->text[letter_id + i];
                ext->text[letter_id + i] = '.';
            }

            ext->dot_tmp[LV_LABEL_DOT_NUM] = ext->text[letter_id + LV_LABEL_DOT_NUM];
            ext->text[letter_id + LV_LABEL_DOT_NUM] = '\0';

            ext->dot_end = letter_id + LV_LABEL_DOT_NUM;
#else
            /*Save letters under the dots and replace them with dots*/
            uint32_t i;
            uint32_t byte_id = lv_txt_encoded_get_byte_id(ext->text, letter_id);
            uint32_t byte_id_ori = byte_id;
            uint8_t len = 0;
            for(i = 0; i <= LV_LABEL_DOT_NUM; i++)  {
                len += lv_txt_encoded_size(&ext->text[byte_id]);
                lv_txt_encoded_next(ext->text, &byte_id);
            }

            memcpy(ext->dot_tmp, &ext->text[byte_id_ori], len);
            ext->dot_tmp[len] = '\0';       /*Close with a zero*/

            for(i = 0; i < LV_LABEL_DOT_NUM; i++) {
                ext->text[byte_id_ori + i] = '.';
            }
            ext->text[byte_id_ori + LV_LABEL_DOT_NUM] = '\0';

            ext->dot_end = letter_id + LV_LABEL_DOT_NUM;
#endif

        }
    }
    /*In break mode only the height can change*/
    else if(ext->long_mode == LV_LABEL_LONG_BREAK) {
        lv_obj_set_height(label, size.y);
    }
    /*Do not set the size in Clip mode*/
    else if(ext->long_mode == LV_LABEL_LONG_CROP) {
        /*Do nothing*/
    }


    lv_obj_invalidate(label);
}

static void lv_label_revert_dots(lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    if(ext->long_mode != LV_LABEL_LONG_DOT) return;
    if(ext->dot_end == LV_LABEL_DOT_END_INV) return;
#if LV_TXT_UTF8 == 0
    uint32_t i;
    for(i = 0; i <= LV_LABEL_DOT_NUM; i++) {
        ext->text[ext->dot_end - i] = ext->dot_tmp[LV_LABEL_DOT_NUM - i];
    }
#else
    uint32_t letter_i = ext->dot_end - LV_LABEL_DOT_NUM;
    uint32_t byte_i = lv_txt_encoded_get_byte_id(ext->text, letter_i);

    /*Restore the characters*/
    uint8_t i = 0;
    while(ext->dot_tmp[i] != '\0') {
        ext->text[byte_i + i] = ext->dot_tmp[i];
        i++;
    }
#endif

    ext->dot_end = LV_LABEL_DOT_END_INV;
}

#if USE_LV_ANIMATION
static void lv_label_set_offset_x(lv_obj_t * label, lv_coord_t x)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    ext->offset.x = x;
    lv_obj_invalidate(label);
}

static void lv_label_set_offset_y(lv_obj_t * label, lv_coord_t y)
{
    lv_label_ext_t * ext = lv_obj_get_ext_attr(label);
    ext->offset.y = y;
    lv_obj_invalidate(label);
}
#endif
#endif
