/**
 * @file lv_span.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_span.h"
#if LV_USE_SPAN != 0
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
#include "../lv_draw/lv_draw_label.h"
/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_spangroup"

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_span_t * span;
    char * txt;
    const lv_font_t * font;
    uint16_t   bytes;
    lv_coord_t txt_w;
    lv_coord_t line_h;
    lv_coord_t letter_space;
} lv_snippet_t;

struct _snippet_stack {
    lv_snippet_t    stack[LV_SPAN_SNIPPET_STACK_SIZE];
    uint16_t        index;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static inline bool is_break_char(uint32_t letter);
static void get_txt_coords(const lv_obj_t * span, lv_area_t * area);
static LV_ATTRIBUTE_FAST_MEM void lv_draw_span(lv_obj_t * spans, const lv_area_t * coords, const lv_area_t * mask);
static bool lv_txt_get_snippet(const char * txt, const lv_font_t * font, lv_coord_t letter_space,
                               lv_coord_t max_width, lv_txt_flag_t flag, lv_coord_t * use_width, uint32_t * end_ofs);

void lv_snippet_clear(void);
static uint16_t lv_get_snippet_cnt();
static void lv_snippet_push(lv_snippet_t * item);
static lv_snippet_t * lv_get_snippet(uint16_t index);

const lv_font_t * lv_span_get_style_text_font(lv_obj_t * par, lv_span_t * span);
lv_style_int_t lv_span_get_style_text_letter_space(lv_obj_t * par, lv_span_t * span);
lv_color_t lv_span_get_style_text_color(lv_obj_t * par, lv_span_t * span);
lv_color_t lv_span_get_style_text_color(lv_obj_t * par, lv_span_t * span);
lv_opa_t lv_span_get_style_text_opa(lv_obj_t * par, lv_span_t * span);
lv_opa_t lv_span_get_style_text_blend_mode(lv_obj_t * par, lv_span_t * span);
lv_style_int_t lv_span_get_style_text_decor(lv_obj_t * par, lv_span_t * span);

static lv_res_t lv_spangroup_signal(lv_obj_t * spangroup, lv_signal_t sign, void * param);
static lv_design_res_t lv_spangroup_design(lv_obj_t * spangroup, const lv_area_t * clip_area, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
struct _snippet_stack snippet_stack;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a spangroup objects
 * @param par pointer to an object, it will be the parent of the new spangroup
 * @param copy pointer to a spangroup object, if not NULL then the new object will be copied from it
 * @return pointer to the created spangroup
 */
lv_obj_t * lv_spangroup_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("spangroup create started");

    /*Create a basic object*/
    lv_obj_t * spans = lv_obj_create(par, copy);
    LV_ASSERT_MEM(spans);
    if(spans == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(spans);

    /*Extend the basic object to a label object*/
    lv_obj_allocate_ext_attr(spans, sizeof(lv_span_ext_t));
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(spans);
        return NULL;
    }

    _lv_ll_init(&ext->child_ll, sizeof(lv_span_t));
    ext->indent = 0;
    ext->align = LV_SPAN_ALIGN_LEFT;
    ext->mode = LV_SPAN_MODE_FIXED;
    ext->overflow = LV_SPAN_OVERFLOW_CLIP;

    lv_obj_set_design_cb(spans, lv_spangroup_design);
    lv_obj_set_signal_cb(spans, lv_spangroup_signal);

    /*Init the new label*/
    if(copy == NULL) {
        lv_obj_set_click(spans, false);
    }
    else {
    }

    LV_LOG_INFO("spangroup created");

    return spans;
}

/**
 * Create a span string descriptor and add to spangroup.
 * @param spans pointer to a spangroup object.
 * @param text '\0' terminated character string.
 * @return pointer to the created span.
 */
lv_span_t * lv_span_create(lv_obj_t * spans, const char * text)
{
    if(spans == NULL || text == NULL) {
        return NULL;
    }

    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);
    lv_span_t * span = _lv_ll_ins_tail(&ext->child_ll);
    span->txt = lv_mem_alloc(strlen(text) + 1);
    strcpy(span->txt, text);
    lv_style_init(&span->style);
    lv_span_refr_mode(spans);

    return span;
}

/**
 * Remove the span from the spangroup and free memory.
 * @param spans pointer to a spangroup object.
 * @param span pointer to a span.
 */
void lv_span_del(lv_obj_t * spans, lv_span_t * span)
{
    if(spans == NULL) {
        return;
    }

    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);

    lv_span_t * cur_span;
    _LV_LL_READ(ext->child_ll, cur_span) {
        if(cur_span == span) {
            _lv_ll_remove(&ext->child_ll, cur_span);
            lv_mem_free(cur_span->txt);
            lv_mem_free(cur_span);
        }
    }

    lv_span_refr_mode(spans);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the align of the spangroup.
 * @param spans pointer to a spangroup object.
 * @param align see lv_span_align_t for details.
 */
void lv_span_set_align(lv_obj_t * spans, lv_span_align_t align)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);

    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);
    if(ext->align == align) return;

    ext->align = align;
    lv_obj_invalidate(spans);
}

/**
 * Set the overflow of the spangroup.
 * @param spans pointer to a spangroup object.
 * @param overflow see lv_span_overflow_t for details.
 */
void lv_span_set_overflow(lv_obj_t * spans, lv_span_overflow_t overflow)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);
    if(ext->overflow == overflow) return;

    ext->overflow = overflow;
    lv_obj_invalidate(spans);
}

/**
 * Set the indent of the spangroup.
 * @param spans pointer to a spangroup object.
 * @param indent The first line indentation
 */
void lv_span_set_indent(lv_obj_t * spans, lv_coord_t indent)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);
    if(ext->indent == indent) return;

    ext->indent = indent;
    lv_obj_invalidate(spans);
}

/**
 * Set the mode of the spangroup.
 * @param spans pointer to a spangroup object.
 * @param mode see lv_span_mode_t for details.
 */
void lv_span_set_mode(lv_obj_t * spans, lv_span_mode_t mode)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);
    if(ext->mode == mode) return;

    ext->mode = mode;
    lv_span_refr_mode(spans);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * get the align of the spangroup.
 * @param spans pointer to a spangroup object.
 * @return the align value.
 */
lv_span_align_t lv_span_get_align(lv_obj_t * spans)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);
    return ext->align;
}

/**
 * get the overflow of the spangroup.
 * @param spans pointer to a spangroup object.
 * @return the overflow value.
 */
lv_span_overflow_t lv_span_get_overflow(lv_obj_t * spans)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);
    return ext->overflow;
}

/**
 * get the indent of the spangroup.
 * @param spans pointer to a spangroup object.
 * @return the indent value.
 */
lv_coord_t lv_span_get_indent(lv_obj_t * spans)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);

    return ext->indent;
}

/**
 * Set the mode of the spangroup.
 * @param spans pointer to a spangroup object.
 * @return the mode value.
 */
lv_span_mode_t lv_span_get_mode(lv_obj_t * spans)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);

    return ext->mode;
}

/**
 * update the mode of the spangroup.
 * @param spans pointer to a spangroup object.
 */
void lv_span_refr_mode(lv_obj_t * spans)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);

    if(_lv_ll_get_head(&ext->child_ll) == NULL) {
        return;
    }

    if(ext->mode == LV_SPAN_MODE_EXPAND) {
        lv_coord_t width = lv_span_get_expend_width(spans);
        lv_coord_t height = lv_span_get_max_line_h(spans);
        lv_coord_t top_pad = lv_obj_get_style_pad_top(spans, LV_SPAN_PART_MAIN);
        lv_coord_t bottom_pad = lv_obj_get_style_pad_bottom(spans, LV_SPAN_PART_MAIN);
        lv_obj_set_width(spans, width + ext->indent);
        lv_obj_set_height(spans, height + top_pad + bottom_pad);
    }
    else if(ext->mode == LV_SPAN_MODE_BREAK) {
        lv_coord_t height = lv_span_get_expend_height(spans, lv_obj_get_width(spans));
        lv_obj_set_height(spans, height);
    }

    lv_obj_invalidate(spans);
}

/**
 * get max line height of all span in the spangroup.
 * @param spans pointer to a spangroup object.
 */
lv_coord_t lv_span_get_max_line_h(lv_obj_t * spans)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);

    lv_coord_t max_line_h = 0;
    lv_span_t * cur_span;
    _LV_LL_READ(ext->child_ll, cur_span) {
        const lv_font_t * font = lv_span_get_style_text_font(spans, cur_span);
        lv_coord_t line_h = lv_font_get_line_height(font);
        if(line_h > max_line_h) {
            max_line_h = line_h;
        }
    }

    return max_line_h;
}

/**
 * get the width when all span of spangroup on a line. include spangroup pad.
 * @param spans pointer to a spangroup object.
 */
lv_coord_t lv_span_get_expend_width(lv_obj_t * spans)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);

    if(_lv_ll_get_head(&ext->child_ll) == NULL) {
        return 0;
    }

    lv_coord_t width = 0;
    lv_span_t * cur_span;
    _LV_LL_READ(ext->child_ll, cur_span) {
        const lv_font_t * font = lv_span_get_style_text_font(spans, cur_span);
        lv_coord_t letter_space = lv_span_get_style_text_letter_space(spans, cur_span);
        uint32_t j = 0;
        while(cur_span->txt[j] != 0) {
            uint32_t letter      = _lv_txt_encoded_next(cur_span->txt, &j);
            uint32_t letter_next = _lv_txt_encoded_next(&cur_span->txt[j], NULL);
            int32_t letter_w = lv_font_get_glyph_width(font, letter, letter_next);
            width = width + letter_w + letter_space;
        }
    }

    lv_coord_t left_pad = lv_obj_get_style_pad_left(spans, LV_SPAN_PART_MAIN);
    lv_coord_t right_pad = lv_obj_get_style_pad_right(spans, LV_SPAN_PART_MAIN);
    width = width + left_pad + right_pad;

    return width;
}

/**
 * get the height with width fixed. the height include spangroup pad.
 * @param spans pointer to a spangroup object.
 */
lv_coord_t lv_span_get_expend_height(lv_obj_t * spans, lv_coord_t width)
{
    LV_ASSERT_OBJ(spans, LV_OBJX_NAME);
    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);

    if(_lv_ll_get_head(&ext->child_ll) == NULL) {
        return 0;
    }

    /* init draw variable */

    lv_txt_flag_t txt_flag = LV_TXT_FLAG_NONE;
    lv_coord_t line_space = lv_obj_get_style_text_line_space(spans, LV_SPAN_PART_MAIN);
    lv_coord_t left_pad = lv_obj_get_style_pad_left(spans, LV_SPAN_PART_MAIN);
    lv_coord_t right_pad = lv_obj_get_style_pad_right(spans, LV_SPAN_PART_MAIN);
    lv_coord_t max_width = width - left_pad - right_pad;
    lv_coord_t max_w  = max_width - ext->indent; /* first line need minus indent */

    /* coords of draw span-txt */
    lv_point_t txt_pos;
    txt_pos.y = 0;
    txt_pos.x = 0 + ext->indent; /* first line need add indent */

    lv_span_t * cur_span = _lv_ll_get_head(&ext->child_ll);
    char * cur_txt = cur_span->txt;
    uint32_t cur_txt_ofs = 0;

    /* the loop control how many lines need to draw */
    while(cur_span) {
        int snippet_cnt = 0;
        lv_coord_t max_line_h = 0;  /* the max height of span-font when a line have a lot of span */
        lv_snippet_t snippet;       /* use to save cur_span info and push it to stack */

        /* the loop control to find a line and push the relevant span info into stack  */
        while(1) {
            /* switch to the next span when current is end */
            if(cur_txt[cur_txt_ofs] == '\0') {
                cur_span = _lv_ll_get_next(&ext->child_ll, cur_span);
                if(cur_span == NULL) break;
                cur_txt = cur_span->txt;
                cur_txt_ofs = 0;
            }

            /* init span info to snippet. */
            if(cur_txt_ofs == 0) {
                snippet.span = cur_span;
                snippet.font = lv_span_get_style_text_font(spans, cur_span);
                snippet.letter_space = lv_span_get_style_text_letter_space(spans, cur_span);
                snippet.line_h = lv_font_get_line_height(snippet.font) + line_space;
            }

            /* get current span text line info */
            uint32_t next_ofs = 0;
            lv_coord_t use_width = 0;
            bool isfill = lv_txt_get_snippet(&cur_txt[cur_txt_ofs], snippet.font, snippet.letter_space,
                                             max_w, txt_flag, &use_width, &next_ofs);

            /* break word deal width */
            if(isfill && next_ofs > 0 && snippet_cnt > 0) {
                uint32_t letter = (uint32_t)cur_txt[cur_txt_ofs + next_ofs - 1];
                if(!(letter == '\0' || letter == '\n' || letter == '\r' || is_break_char(letter))) {
                    letter = (uint32_t)cur_txt[cur_txt_ofs + next_ofs];
                    if(!(letter == '\0' || letter == '\n'  || letter == '\r' || is_break_char(letter))) {
                        break;
                    }
                }
            }

            snippet.txt = &cur_txt[cur_txt_ofs];
            snippet.bytes = next_ofs;
            snippet.txt_w = use_width;
            cur_txt_ofs += next_ofs;
            if(max_line_h < snippet.line_h) {
                max_line_h = snippet.line_h;
            }
            snippet_cnt ++;
            if(isfill) {
                break;
            }
            max_w -= use_width;
        }

        /* next line init */
        txt_pos.x = 0;
        txt_pos.y += max_line_h;
        max_w = max_width;
    }

    lv_coord_t top_pad = lv_obj_get_style_pad_top(spans, LV_SPAN_PART_MAIN);
    lv_coord_t bottom_pad = lv_obj_get_style_pad_bottom(spans, LV_SPAN_PART_MAIN);
    txt_pos.y = txt_pos.y + top_pad + bottom_pad - line_space;

    return txt_pos.y;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the labels
 * @param spans pointer to a label object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_spangroup_design(lv_obj_t * spans, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /* A span never covers an area */
    if(mode == LV_DESIGN_COVER_CHK)
        return LV_DESIGN_RES_NOT_COVER;
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_coord_t w = lv_obj_get_style_transform_width(spans, LV_SPAN_PART_MAIN);
        lv_coord_t h = lv_obj_get_style_transform_height(spans, LV_SPAN_PART_MAIN);
        lv_area_t bg_coords;
        lv_area_copy(&bg_coords, &spans->coords);
        bg_coords.x1 -= w;
        bg_coords.x2 += w;
        bg_coords.y1 -= h;
        bg_coords.y2 += h;

        lv_draw_rect_dsc_t draw_rect_dsc;
        lv_draw_rect_dsc_init(&draw_rect_dsc);
        lv_obj_init_draw_rect_dsc(spans, LV_SPAN_PART_MAIN, &draw_rect_dsc);
        lv_draw_rect(&bg_coords, clip_area, &draw_rect_dsc);

        lv_area_t txt_clip;
        lv_area_t txt_coords;
        get_txt_coords(spans, &txt_coords);
        bool is_common = _lv_area_intersect(&txt_clip, clip_area, &txt_coords);
        if(!is_common) return LV_DESIGN_RES_OK;
        lv_draw_span(spans, &txt_coords, clip_area);
    }
    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the label
 * @param spans pointer to a label object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_spangroup_signal(lv_obj_t * spans, lv_signal_t sign, void * param)
{
    lv_res_t res;

    if(sign == LV_SIGNAL_GET_STYLE) {
        return ancestor_signal(spans, sign, param);
    }

    /* Include the ancient signal function */
    res = ancestor_signal(spans, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);

    if(sign == LV_SIGNAL_CLEANUP) {
        lv_span_t * cur_span = _lv_ll_get_head(&ext->child_ll);
        while(cur_span) {
            _lv_ll_remove(&ext->child_ll, cur_span);
            lv_mem_free(cur_span->txt);
            lv_mem_free(cur_span);
            cur_span = _lv_ll_get_head(&ext->child_ll);
        }
    }
    else if(sign == LV_SIGNAL_BASE_DIR_CHG) {
    }

    return res;
}

static void get_txt_coords(const lv_obj_t * span, lv_area_t * area)
{
    lv_obj_get_coords(span, area);

    lv_coord_t left   = lv_obj_get_style_pad_left(span, LV_SPAN_PART_MAIN);
    lv_coord_t right  = lv_obj_get_style_pad_right(span, LV_SPAN_PART_MAIN);
    lv_coord_t top    = lv_obj_get_style_pad_top(span, LV_SPAN_PART_MAIN);
    lv_coord_t bottom = lv_obj_get_style_pad_bottom(span, LV_SPAN_PART_MAIN);
    area->x1 += left;
    area->x2 -= right;
    area->y1 += top;
    area->y2 -= bottom;
}

/**
 * @return true for txt fill the max_width.
 */
static bool lv_txt_get_snippet(const char * txt, const lv_font_t * font,
                               lv_coord_t letter_space, lv_coord_t max_width, lv_txt_flag_t flag,
                               lv_coord_t * use_width, uint32_t * end_ofs)
{
    uint32_t ofs = _lv_txt_get_next_line(txt, font, letter_space, max_width, flag);
    lv_coord_t width = _lv_txt_get_width(txt, ofs, font, letter_space, flag);
    *end_ofs = ofs;
    *use_width = width;

    if(txt[ofs] == '\0' && width < max_width) {
        return false;
    }
    else {
        return true;
    }
}

static void lv_snippet_push(lv_snippet_t * item)
{
    memcpy(&snippet_stack.stack[snippet_stack.index], item, sizeof(lv_snippet_t));
    snippet_stack.index++;
}

static uint16_t lv_get_snippet_cnt()
{
    return snippet_stack.index;
}

static lv_snippet_t * lv_get_snippet(uint16_t index)
{
    return &snippet_stack.stack[index];
}

void lv_snippet_clear(void)
{
    snippet_stack.index = 0;
}

const lv_font_t * lv_span_get_style_text_font(lv_obj_t * par, lv_span_t * span)
{
    const void * font = NULL;
    int res = _lv_style_get_ptr(&span->style, LV_STYLE_TEXT_FONT, &font);
    if(res < 0) {
        font = lv_obj_get_style_text_font(par, LV_OBJ_PART_MAIN);
    }
    return (const lv_font_t *)font;
}

lv_style_int_t lv_span_get_style_text_letter_space(lv_obj_t * par, lv_span_t * span)
{
    lv_style_int_t letter_space;
    int res = _lv_style_get_int(&span->style, LV_STYLE_VALUE_LETTER_SPACE, &letter_space);
    if(res < 0) {
        letter_space = lv_obj_get_style_text_letter_space(par, LV_OBJ_PART_MAIN);
    }
    return letter_space;
}

lv_color_t lv_span_get_style_text_color(lv_obj_t * par, lv_span_t * span)
{
    lv_color_t color;
    int res = _lv_style_get_color(&span->style, LV_STYLE_TEXT_COLOR, &color);
    if(res < 0) {
        color = lv_obj_get_style_text_color(par, LV_OBJ_PART_MAIN);
    }
    return color;
}

lv_opa_t lv_span_get_style_text_opa(lv_obj_t * par, lv_span_t * span)
{
    lv_opa_t opa;
    int res = _lv_style_get_opa(&span->style, LV_STYLE_TEXT_OPA, &opa);
    if(res < 0) {
        opa = lv_obj_get_style_text_opa(par, LV_OBJ_PART_MAIN);
    }
    return opa;
}

lv_blend_mode_t lv_span_get_style_text_blend_mode(lv_obj_t * par, lv_span_t * span)
{
    lv_style_int_t mode;
    int res = _lv_style_get_int(&span->style, LV_STYLE_TEXT_BLEND_MODE, &mode);
    if(res < 0) {
        mode = lv_obj_get_style_text_blend_mode(par, LV_OBJ_PART_MAIN);
    }
    return (lv_blend_mode_t)mode;
}

lv_style_int_t lv_span_get_style_text_decor(lv_obj_t * par, lv_span_t * span)
{
    LV_UNUSED(par);

    lv_style_int_t decor;
    int res = _lv_style_get_int(&span->style, LV_STYLE_TEXT_DECOR, &decor);
    if(res < 0) {
        decor = LV_TEXT_DECOR_NONE;
    }
    return decor;
}

static inline bool is_break_char(uint32_t letter)
{
    uint8_t i;
    bool ret = false;

    /*Compare the letter to TXT_BREAK_CHARS*/
    for(i = 0; LV_TXT_BREAK_CHARS[i] != '\0'; i++) {
        if(letter == (uint32_t)LV_TXT_BREAK_CHARS[i]) {
            ret = true; /*If match then it is break char*/
            break;
        }
    }

    return ret;
}

/**
 * draw span group
 * @param spans obj handle
 * @param coords coordinates of the label
 * @param mask the label will be drawn only in this area
 */
static LV_ATTRIBUTE_FAST_MEM void lv_draw_span(lv_obj_t * spans, const lv_area_t * coords, const lv_area_t * mask)
{
    /* return if no draw area */
    lv_area_t clipped_area;
    bool clip_ok = _lv_area_intersect(&clipped_area, coords, mask);
    if(!clip_ok) return;

    lv_span_ext_t * ext = lv_obj_get_ext_attr(spans);

    /* return if not span */
    if(_lv_ll_get_head(&ext->child_ll) == NULL) {
        return;
    }

    /* init draw variable */
    lv_txt_flag_t txt_flag = LV_TXT_FLAG_NONE;
    lv_coord_t line_space = lv_obj_get_style_text_line_space(spans, LV_SPAN_PART_MAIN);;
    lv_coord_t max_width = lv_area_get_width(coords);
    lv_coord_t max_w  = max_width - ext->indent; /* first line need minus indent */

    /* coords of draw span-txt */
    lv_point_t txt_pos;
    txt_pos.y = coords->y1;
    txt_pos.x = coords->x1 + ext->indent; /* first line need add indent */

    lv_span_t * cur_span = _lv_ll_get_head(&ext->child_ll);
    char * cur_txt = cur_span->txt;
    uint32_t cur_txt_ofs = 0;

    /* the loop control how many lines need to draw */
    while(cur_span) {
        lv_coord_t max_line_h = 0;  /* the max height of span-font when a line have a lot of span */
        lv_snippet_t snippet;       /* use to save cur_span info and push it to stack */
        lv_snippet_clear();

        /* the loop control to find a line and push the relevant span info into stack  */
        while(1) {
            /* switch to the next span when current is end */
            if(cur_txt[cur_txt_ofs] == '\0') {
                cur_span = _lv_ll_get_next(&ext->child_ll, cur_span);
                if(cur_span == NULL) break;
                cur_txt = cur_span->txt;
                cur_txt_ofs = 0;
            }

            /* init span info to snippet. */
            if(cur_txt_ofs == 0) {
                snippet.span = cur_span;
                snippet.font = lv_span_get_style_text_font(spans, cur_span);
                snippet.letter_space = lv_span_get_style_text_letter_space(spans, cur_span);
                snippet.line_h = lv_font_get_line_height(snippet.font) + line_space;
            }

            if(ext->overflow == LV_SPAN_OVERFLOW_ELLIPSIS) {
                /* span txt overflow, don't push */
                if(txt_pos.y + snippet.line_h - line_space > coords->y2 + 1) {
                    break;
                }
            }

            /* get current span text line info */
            uint32_t next_ofs = 0;
            lv_coord_t use_width = 0;
            bool isfill = lv_txt_get_snippet(&cur_txt[cur_txt_ofs], snippet.font, snippet.letter_space,
                                             max_w, txt_flag, &use_width, &next_ofs);

            /* break word deal width */
            if(isfill && next_ofs > 0 && lv_get_snippet_cnt() > 0) {
                uint32_t letter = (uint32_t)cur_txt[cur_txt_ofs + next_ofs - 1];
                if(!(letter == '\0' || letter == '\n' || letter == '\r' || is_break_char(letter))) {
                    letter = (uint32_t)cur_txt[cur_txt_ofs + next_ofs];
                    if(!(letter == '\0' || letter == '\n'  || letter == '\r' || is_break_char(letter))) {
                        break;
                    }
                }
            }

            snippet.txt = &cur_txt[cur_txt_ofs];
            snippet.bytes = next_ofs;
            snippet.txt_w = use_width;
            cur_txt_ofs += next_ofs;
            if(max_line_h < snippet.line_h) {
                max_line_h = snippet.line_h;
            }

            lv_snippet_push(&snippet);
            if(isfill) {
                break;
            }
            max_w -= use_width;
        }

        /* start current line deal width */

        uint16_t item_cnt = lv_get_snippet_cnt();
        if(item_cnt == 0) {     /* break if stack is empty */
            break;
        }

        /*Go the first visible line*/
        if(txt_pos.y + max_line_h < mask->y1) {
            goto Next_line_init;
        }

        /* overflow deal width */
        bool ellipsis_valid = false;
        if(ext->overflow == LV_SPAN_OVERFLOW_ELLIPSIS) {
            lv_coord_t next_line_h = snippet.line_h;
            if(cur_txt[cur_txt_ofs] == '\0') {  /* current span deal with ok, need get next line first line height */
                next_line_h = 0;
                if(cur_span) {
                    lv_span_t * next_span = _lv_ll_get_next(&ext->child_ll, cur_span);
                    if(next_span) { /* have the next line */
                        next_line_h = lv_font_get_line_height(lv_span_get_style_text_font(spans, next_span)) + line_space;
                    }
                }
            }
            if(txt_pos.y + max_line_h + next_line_h > coords->y2  + 1) {
                ellipsis_valid = true;
            }
        }

        /* align deal with */
        if(ext->align != LV_SPAN_ALIGN_LEFT) {
            lv_coord_t align_ofs = 0;
            lv_coord_t txts_w = 0;
            for(int i = 0; i < item_cnt; i++) {
                lv_snippet_t * pinfo = lv_get_snippet(i);
                txts_w += pinfo->txt_w;
            }
            if(ext->align == LV_SPAN_ALIGN_CENTER) {
                align_ofs = (max_width - txts_w) / 2;
            }
            else if(ext->align == LV_SPAN_ALIGN_RIGHT) {
                align_ofs = max_width - txts_w;
            }
            txt_pos.x += align_ofs;
        }

        /* draw line letters */
        for(int i = 0; i < item_cnt; i++) {
            lv_snippet_t * pinfo = lv_get_snippet(i);

            /* bidi deal with:todo */
            const char * bidi_txt = pinfo->txt;

            lv_point_t pos;
            pos.x = txt_pos.x;
            pos.y = txt_pos.y + max_line_h - pinfo->line_h;
            lv_color_t letter_color = lv_span_get_style_text_color(spans, pinfo->span);
            lv_opa_t letter_opa = lv_span_get_style_text_opa(spans, pinfo->span);
            lv_blend_mode_t blend_mode = lv_span_get_style_text_blend_mode(spans, pinfo->span);
            uint32_t txt_bytes = pinfo->bytes;

            /* overflow */
            uint16_t dot_letter_w = 0;
            uint16_t dot_width = 0;
            if(ellipsis_valid) {
                txt_bytes = strlen(bidi_txt);
                dot_letter_w = lv_font_get_glyph_width(pinfo->font, '.', '.');
                dot_width = dot_letter_w * 3;
            }
            lv_coord_t ellipsis_width = coords->x1 + max_width - dot_width;

            uint32_t j = 0;
            while(j < txt_bytes) {
                /* skip invalid fields */
                if(pos.x > clipped_area.x2) {
                    break;
                }
                uint32_t letter      = _lv_txt_encoded_next(bidi_txt, &j);
                uint32_t letter_next = _lv_txt_encoded_next(&bidi_txt[j], NULL);
                int32_t letter_w = lv_font_get_glyph_width(pinfo->font, letter, letter_next);

                /* skip invalid fields */
                if(pos.x + letter_w + pinfo->letter_space < clipped_area.x1) {
                    if(letter_w > 0) {
                        pos.x = pos.x + letter_w + pinfo->letter_space;
                    }
                    continue;
                }

                if(ellipsis_valid && pos.x + letter_w + pinfo->letter_space > ellipsis_width) {
                    for(int ell = 0; ell < 3; ell++) {
                        lv_draw_letter(&pos, &clipped_area, pinfo->font, '.', letter_color, letter_opa, blend_mode);
                        pos.x = pos.x + dot_letter_w + pinfo->letter_space;
                    }
                    break;
                }
                else {
                    lv_draw_letter(&pos, &clipped_area, pinfo->font, letter, letter_color, letter_opa, blend_mode);
                    if(letter_w > 0) {
                        pos.x = pos.x + letter_w + pinfo->letter_space;
                    }
                }
            }

            if(ellipsis_valid && i == item_cnt - 1 && pos.x <= ellipsis_width) {
                for(int ell = 0; ell < 3; ell++) {
                    lv_draw_letter(&pos, &clipped_area, pinfo->font, '.', letter_color, letter_opa, blend_mode);
                    pos.x = pos.x + dot_letter_w + pinfo->letter_space;
                }
            }

            /* draw decor */
            lv_text_decor_t decor = lv_span_get_style_text_decor(spans, pinfo->span);
            if(decor != LV_TEXT_DECOR_NONE) {
                lv_draw_line_dsc_t line_dsc;
                lv_draw_line_dsc_init(&line_dsc);
                line_dsc.color = letter_color;
                line_dsc.width = pinfo->font->underline_thickness ? pinfo->font->underline_thickness : 1;
                line_dsc.opa = letter_opa;
                line_dsc.blend_mode = blend_mode;

                if(decor & LV_TEXT_DECOR_STRIKETHROUGH) {
                    lv_point_t p1;
                    lv_point_t p2;
                    p1.x = txt_pos.x;
                    p1.y = pos.y + (pinfo->line_h / 2)  + line_dsc.width / 2;
                    p2.x = pos.x;
                    p2.y = p1.y;
                    lv_draw_line(&p1, &p2, mask, &line_dsc);
                }

                if(decor & LV_TEXT_DECOR_UNDERLINE) {
                    lv_point_t p1;
                    lv_point_t p2;
                    p1.x = txt_pos.x;
                    p1.y = pos.y + pinfo->line_h + line_dsc.width / 2;
                    p2.x = pos.x;
                    p2.y = p1.y;
                    lv_draw_line(&p1, &p2, &clipped_area, &line_dsc);
                }
            }
            txt_pos.x = pos.x;
        }

Next_line_init:
        /* next line init */
        txt_pos.x = coords->x1;
        txt_pos.y += max_line_h;
        if(txt_pos.y > clipped_area.y2 + 1) {
            return;
        }
        max_w = max_width;
    }
}

#endif