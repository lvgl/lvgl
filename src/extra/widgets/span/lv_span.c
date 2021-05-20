/**
 * @file lv_span.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_span.h"

#if LV_USE_SPAN != 0

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_spangroup_class

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_span_t * span;
    const char * txt;
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
static void lv_spangroup_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_spangroup_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_spangroup_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_main(lv_event_t * e);

static const lv_font_t * lv_span_get_style_text_font(lv_obj_t * par, lv_span_t * span);
static lv_coord_t lv_span_get_style_text_letter_space(lv_obj_t * par, lv_span_t * span);
static lv_color_t lv_span_get_style_text_color(lv_obj_t * par, lv_span_t * span);
static lv_color_t lv_span_get_style_text_color(lv_obj_t * par, lv_span_t * span);
static lv_opa_t lv_span_get_style_text_opa(lv_obj_t * par, lv_span_t * span);
static lv_opa_t lv_span_get_style_text_blend_mode(lv_obj_t * par, lv_span_t * span);
static int32_t lv_span_get_style_text_decor(lv_obj_t * par, lv_span_t * span);

static inline void span_text_check(const char ** text);
static inline bool is_break_char(uint32_t letter);
static void get_txt_coords(const lv_obj_t * span, lv_area_t * area);
static void lv_draw_span(lv_obj_t * spans, const lv_area_t * coords, const lv_area_t * mask);
static bool lv_txt_get_snippet(const char * txt, const lv_font_t * font, lv_coord_t letter_space,
                               lv_coord_t max_width, lv_text_flag_t flag, lv_coord_t * use_width,
                               uint32_t * end_ofs);

static void lv_snippet_clear(void);
static uint16_t lv_get_snippet_cnt();
static void lv_snippet_push(lv_snippet_t * item);
static lv_snippet_t * lv_get_snippet(uint16_t index);

/**********************
 *  STATIC VARIABLES
 **********************/
static struct _snippet_stack snippet_stack;

const lv_obj_class_t lv_spangroup_class  = {
    .base_class = &lv_obj_class,
    .constructor_cb = lv_spangroup_constructor,
    .destructor_cb = lv_spangroup_destructor,
    .event_cb = lv_spangroup_event,
    .instance_size = sizeof(lv_spangroup_t),
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a spangroup objects
 * @param par pointer to an object, it will be the parent of the new spangroup
 * @return pointer to the created spangroup
 */
lv_obj_t * lv_spangroup_create(lv_obj_t * par)
{
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_spangroup_class, par);
    lv_obj_class_init_obj(obj);
    return obj;
}

/**
 * Create a span string descriptor and add to spangroup.
 * @param obj pointer to a spangroup object.
 * @return pointer to the created span.
 */
lv_span_t * lv_spangroup_new_span(lv_obj_t * obj)
{
    if(obj == NULL) {
        return NULL;
    }

    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    lv_span_t * span = _lv_ll_ins_tail(&spans->child_ll);
    LV_ASSERT_MALLOC(span);

    lv_style_init(&span->style);
    span->txt = (char *)"";
    span->static_flag = 1;

    lv_obj_invalidate(obj);
    return span;
}

/**
 * Remove the span from the spangroup and free memory.
 * @param obj pointer to a spangroup object.
 * @param span pointer to a span.
 */
void lv_spangroup_del_span(lv_obj_t * obj, lv_span_t * span)
{
    if(obj == NULL) {
        return;
    }

    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    lv_span_t * cur_span;
    _LV_LL_READ(&spans->child_ll, cur_span) {
        if(cur_span == span) {
            _lv_ll_remove(&spans->child_ll, cur_span);
            if(cur_span->txt && cur_span->static_flag == 0) {
                lv_mem_free(cur_span->txt);
            }
            lv_mem_free(cur_span);
            break;
        }
    }

    lv_spangroup_refr_mode(obj);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new text for a span. Memory will be allocated to store the text by the span.
 * @param span pointer to a span.
 * @param text pointer to a text.
 */
void lv_span_set_text(lv_span_t * span, const char * text)
{
    if(span == NULL || text == NULL) {
        return;
    }

    if(span->txt == NULL || span->static_flag == 1) {
        span->txt = lv_mem_alloc(strlen(text) + 1);
    }
    else {
        lv_mem_realloc(span->txt, strlen(text) + 1);
    }
    span->static_flag = 0;
    strcpy(span->txt, text);
}

/**
 * Set a static text. It will not be saved by the span so the 'text' variable
 * has to be 'alive' while the span exist.
 * @param span pointer to a span.
 * @param text pointer to a text.
 */
void lv_span_set_text_static(lv_span_t * span, const char * text)
{
    if(span == NULL || text == NULL) {
        return;
    }

    if(span->txt && span->static_flag == 0) {
        lv_mem_free(span->txt);
    }
    span->static_flag = 1;
    span->txt = (char *)text;
}

/**
 * Set the align of the spangroup.
 * @param obj pointer to a spangroup object.
 * @param align see lv_text_align_t for details.
 */
void lv_spangroup_set_align(lv_obj_t * obj, lv_text_align_t align)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    if(spans->align == align) return;

    spans->align = align;
    lv_obj_invalidate(obj);
}

/**
 * Set the overflow of the spangroup.
 * @param obj pointer to a spangroup object.
 * @param overflow see lv_span_overflow_t for details.
 */
void lv_spangroup_set_overflow(lv_obj_t * obj, lv_span_overflow_t overflow)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    if(spans->overflow == overflow) return;

    spans->overflow = overflow;
    lv_obj_invalidate(obj);
}

/**
 * Set the indent of the spangroup.
 * @param obj pointer to a spangroup object.
 * @param indent The first line indentation
 */
void lv_spangroup_set_indent(lv_obj_t * obj, lv_coord_t indent)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    if(spans->indent == indent) return;

    spans->indent = indent;
    lv_obj_invalidate(obj);
}

/**
 * Set the mode of the spangroup.
 * @param obj pointer to a spangroup object.
 * @param mode see lv_span_mode_t for details.
 */
void lv_spangroup_set_mode(lv_obj_t * obj, lv_span_mode_t mode)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    if(spans->mode == mode) return;

    spans->mode = mode;
    lv_spangroup_refr_mode(obj);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * get the align of the spangroup.
 * @param obj pointer to a spangroup object.
 * @return the align value.
 */
lv_text_align_t lv_spangroup_get_align(lv_obj_t * obj)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    return spans->align;
}

/**
 * get the overflow of the spangroup.
 * @param obj pointer to a spangroup object.
 * @return the overflow value.
 */
lv_span_overflow_t lv_spangroup_get_overflow(lv_obj_t * obj)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    return spans->overflow;
}

/**
 * get the indent of the spangroup.
 * @param obj pointer to a spangroup object.
 * @return the indent value.
 */
lv_coord_t lv_spangroup_get_indent(lv_obj_t * obj)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    return spans->indent;
}

/**
 * Set the mode of the spangroup.
 * @param obj pointer to a spangroup object.
 * @return the mode value.
 */
lv_span_mode_t lv_spangroup_get_mode(lv_obj_t * obj)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    return spans->mode;
}

/**
 * update the mode of the spangroup.
 * @param obj pointer to a spangroup object.
 */
void lv_spangroup_refr_mode(lv_obj_t * obj)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;

    if(_lv_ll_get_head(&spans->child_ll) == NULL) {
        return;
    }

    if(spans->mode == LV_SPAN_MODE_EXPAND) {
        lv_coord_t width = lv_spangroup_get_expand_width(obj);
        lv_coord_t height = lv_spangroup_get_max_line_h(obj);
        lv_coord_t top_pad = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
        lv_coord_t bottom_pad = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
        lv_obj_set_width(obj, width + spans->indent);
        lv_obj_set_height(obj, height + top_pad + bottom_pad);
    }
    else if(spans->mode == LV_SPAN_MODE_BREAK) {
        lv_coord_t height = lv_spangroup_get_expand_height(obj, lv_obj_get_width(obj));
        lv_obj_set_height(obj, height);
    }

    lv_obj_invalidate(obj);
}

/**
 * get max line height of all span in the spangroup.
 * @param obj pointer to a spangroup object.
 */
lv_coord_t lv_spangroup_get_max_line_h(lv_obj_t * obj)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;

    lv_coord_t max_line_h = 0;
    lv_span_t * cur_span;
    _LV_LL_READ(&spans->child_ll, cur_span) {
        const lv_font_t * font = lv_span_get_style_text_font(obj, cur_span);
        lv_coord_t line_h = lv_font_get_line_height(font);
        if(line_h > max_line_h) {
            max_line_h = line_h;
        }
    }

    return max_line_h;
}

/**
 * get the width when all span of spangroup on a line. include spangroup pad.
 * @param obj pointer to a spangroup object.
 */
lv_coord_t lv_spangroup_get_expand_width(lv_obj_t * obj)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;

    if(_lv_ll_get_head(&spans->child_ll) == NULL) {
        return 0;
    }

    lv_coord_t width = 0;
    lv_span_t * cur_span;
    _LV_LL_READ(&spans->child_ll, cur_span) {
        const lv_font_t * font = lv_span_get_style_text_font(obj, cur_span);
        lv_coord_t letter_space = lv_span_get_style_text_letter_space(obj, cur_span);
        uint32_t j = 0;
        const char * cur_txt = cur_span->txt;
        span_text_check(&cur_txt);
        while(cur_txt[j] != 0) {
            uint32_t letter      = _lv_txt_encoded_next(cur_txt, &j);
            uint32_t letter_next = _lv_txt_encoded_next(&cur_txt[j], NULL);
            int32_t letter_w = lv_font_get_glyph_width(font, letter, letter_next);
            width = width + letter_w + letter_space;
        }
    }

    lv_coord_t left_pad = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t right_pad = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    width = width + left_pad + right_pad;

    return width;
}

/**
 * get the height with width fixed. the height include spangroup pad.
 * @param obj pointer to a spangroup object.
 */
lv_coord_t lv_spangroup_get_expand_height(lv_obj_t * obj, lv_coord_t width)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    if(_lv_ll_get_head(&spans->child_ll) == NULL) {
        return 0;
    }

    /* init draw variable */
    lv_text_flag_t txt_flag = LV_TEXT_FLAG_NONE;
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_coord_t left_pad = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t right_pad = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    lv_coord_t max_width = width - left_pad - right_pad;
    lv_coord_t max_w  = max_width - spans->indent; /* first line need minus indent */

    /* coords of draw span-txt */
    lv_point_t txt_pos;
    txt_pos.y = 0;
    txt_pos.x = 0 + spans->indent; /* first line need add indent */

    lv_span_t * cur_span = _lv_ll_get_head(&spans->child_ll);
    const char * cur_txt = cur_span->txt;
    span_text_check(&cur_txt);
    uint32_t cur_txt_ofs = 0;
    lv_snippet_t snippet;   /* use to save cur_span info and push it to stack */
    memset(&snippet, 0, sizeof(snippet));

    /* the loop control how many lines need to draw */
    while(cur_span) {
        int snippet_cnt = 0;
        lv_coord_t max_line_h = 0;  /* the max height of span-font when a line have a lot of span */

        /* the loop control to find a line and push the relevant span info into stack  */
        while(1) {
            /* switch to the next span when current is end */
            if(cur_txt[cur_txt_ofs] == '\0') {
                cur_span = _lv_ll_get_next(&spans->child_ll, cur_span);
                if(cur_span == NULL) break;
                cur_txt = cur_span->txt;
                span_text_check(&cur_txt);
                cur_txt_ofs = 0;
            }

            /* init span info to snippet. */
            if(cur_txt_ofs == 0) {
                snippet.span = cur_span;
                snippet.font = lv_span_get_style_text_font(obj, cur_span);
                snippet.letter_space = lv_span_get_style_text_letter_space(obj, cur_span);
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

    lv_coord_t top_pad = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t bottom_pad = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
    txt_pos.y = txt_pos.y + top_pad + bottom_pad - line_space;

    return txt_pos.y;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_spangroup_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    _lv_ll_init(&spans->child_ll, sizeof(lv_span_t));
    spans->indent = 0;
    spans->align = LV_TEXT_ALIGN_LEFT;
    spans->mode = LV_SPAN_MODE_FIXED;
    spans->overflow = LV_SPAN_OVERFLOW_CLIP;
}

static void lv_spangroup_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    lv_span_t * cur_span = _lv_ll_get_head(&spans->child_ll);
    while(cur_span) {
        _lv_ll_remove(&spans->child_ll, cur_span);
        if(cur_span->txt && cur_span->static_flag == 0) {
            lv_mem_free(cur_span->txt);
        }
        lv_mem_free(cur_span);
        cur_span = _lv_ll_get_head(&spans->child_ll);
    }
}

static void lv_spangroup_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    /* Call the ancestor's event handler */
    if(lv_obj_event_base(MY_CLASS, e) != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_DRAW_MAIN) {
        draw_main(e);
    }
    else if(code == LV_EVENT_STYLE_CHANGED) {
        lv_spangroup_refr_mode(obj);
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        lv_spangroup_refr_mode(obj);
    }
}

static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    const lv_area_t * clip_area = lv_event_get_param(e);

    lv_area_t txt_coords;
    get_txt_coords(obj, &txt_coords);

    lv_area_t txt_clip;
    bool is_common = _lv_area_intersect(&txt_clip, clip_area, &txt_coords);
    if(!is_common) return;

    lv_draw_span(obj, &txt_coords, clip_area);
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

/**
 * @return true for txt fill the max_width.
 */
static bool lv_txt_get_snippet(const char * txt, const lv_font_t * font,
                               lv_coord_t letter_space, lv_coord_t max_width, lv_text_flag_t flag,
                               lv_coord_t * use_width, uint32_t * end_ofs)
{
    uint32_t ofs = _lv_txt_get_next_line(txt, font, letter_space, max_width, flag);
    lv_coord_t width = lv_txt_get_width(txt, ofs, font, letter_space, flag);
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
    if(snippet_stack.index < LV_SPAN_SNIPPET_STACK_SIZE) {
        memcpy(&snippet_stack.stack[snippet_stack.index], item, sizeof(lv_snippet_t));
        snippet_stack.index++;
    }
    else {
        LV_LOG_ERROR("span draw stack overflow, please set LV_SPAN_SNIPPET_STACK_SIZE too larger");
    }
}

static uint16_t lv_get_snippet_cnt()
{
    return snippet_stack.index;
}

static lv_snippet_t * lv_get_snippet(uint16_t index)
{
    return &snippet_stack.stack[index];
}

static void lv_snippet_clear(void)
{
    snippet_stack.index = 0;
}

static const lv_font_t * lv_span_get_style_text_font(lv_obj_t * par, lv_span_t * span)
{
    const lv_font_t * font;
    lv_style_value_t value;
    lv_res_t res = lv_style_get_prop(&span->style, LV_STYLE_TEXT_FONT, &value);
    if(res != LV_RES_OK) {
        font = lv_obj_get_style_text_font(par, LV_PART_MAIN);
    }
    else {
        font = (const lv_font_t *)value.ptr;
    }
    return font;
}

static lv_coord_t lv_span_get_style_text_letter_space(lv_obj_t * par, lv_span_t * span)
{
    lv_coord_t letter_space;
    lv_style_value_t value;
    lv_res_t res = lv_style_get_prop(&span->style, LV_STYLE_TEXT_LETTER_SPACE, &value);
    if(res != LV_RES_OK) {
        letter_space = lv_obj_get_style_text_letter_space(par, LV_PART_MAIN);
    }
    else {
        letter_space = (lv_coord_t)value.num;
    }
    return letter_space;
}

static lv_color_t lv_span_get_style_text_color(lv_obj_t * par, lv_span_t * span)
{
    lv_style_value_t value;
    lv_res_t res = lv_style_get_prop(&span->style, LV_STYLE_TEXT_COLOR, &value);
    if(res != LV_RES_OK) {
        value.color = lv_obj_get_style_text_color(par, LV_PART_MAIN);
    }
    return value.color;
}

static lv_opa_t lv_span_get_style_text_opa(lv_obj_t * par, lv_span_t * span)
{
    lv_opa_t opa;
    lv_style_value_t value;
    lv_res_t res = lv_style_get_prop(&span->style, LV_STYLE_TEXT_OPA, &value);
    if(res != LV_RES_OK) {
        opa = (lv_opa_t)lv_obj_get_style_text_opa(par, LV_PART_MAIN);
    }
    else {
        opa = (lv_opa_t)value.num;
    }
    return opa;
}

static lv_blend_mode_t lv_span_get_style_text_blend_mode(lv_obj_t * par, lv_span_t * span)
{
    lv_blend_mode_t mode;
    lv_style_value_t value;
    lv_res_t res = lv_style_get_prop(&span->style, LV_STYLE_BLEND_MODE, &value);
    if(res != LV_RES_OK) {
        mode = (lv_blend_mode_t)lv_obj_get_style_blend_mode(par, LV_PART_MAIN);
    }
    else {
        mode = (lv_blend_mode_t)value.num;
    }
    return mode;
}

static int32_t lv_span_get_style_text_decor(lv_obj_t * par, lv_span_t * span)
{
    LV_UNUSED(par);

    int32_t decor;
    lv_style_value_t value;
    lv_res_t res = lv_style_get_prop(&span->style, LV_STYLE_TEXT_DECOR, &value);
    if(res != LV_RES_OK) {
        decor = LV_TEXT_DECOR_NONE;
    }
    else {
        decor = (int32_t)value.num;
    }
    return decor;
}

static inline void span_text_check(const char ** text)
{
    if(*text == NULL) {
        *text = "";
        LV_LOG_ERROR("occur an error that span text == NULL");
    }
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
static void lv_draw_span(lv_obj_t * obj, const lv_area_t * coords, const lv_area_t * mask)
{
    /* return if no draw area */
    lv_area_t clipped_area;
    bool clip_ok = _lv_area_intersect(&clipped_area, coords, mask);
    if(!clip_ok) return;

    lv_spangroup_t * spans = (lv_spangroup_t *)obj;

    /* return if not span */
    if(_lv_ll_get_head(&spans->child_ll) == NULL) {
        return;
    }

    /* init draw variable */
    lv_text_flag_t txt_flag = LV_TEXT_FLAG_NONE;
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);;
    lv_coord_t max_width = lv_area_get_width(coords);
    lv_coord_t max_w  = max_width - spans->indent; /* first line need minus indent */

    /* coords of draw span-txt */
    lv_point_t txt_pos;
    txt_pos.y = coords->y1;
    txt_pos.x = coords->x1 + spans->indent; /* first line need add indent */

    lv_span_t * cur_span = _lv_ll_get_head(&spans->child_ll);
    const char * cur_txt = cur_span->txt;
    span_text_check(&cur_txt);
    uint32_t cur_txt_ofs = 0;
    lv_snippet_t snippet;   /* use to save cur_span info and push it to stack */
    memset(&snippet, 0, sizeof(snippet));
    
    /* the loop control how many lines need to draw */
    while(cur_span) {
        lv_coord_t max_line_h = 0;  /* the max height of span-font when a line have a lot of span */
        lv_snippet_clear();

        /* the loop control to find a line and push the relevant span info into stack  */
        while(1) {
            /* switch to the next span when current is end */
            if(cur_txt[cur_txt_ofs] == '\0') {
                cur_span = _lv_ll_get_next(&spans->child_ll, cur_span);
                if(cur_span == NULL) break;
                cur_txt = cur_span->txt;
                span_text_check(&cur_txt);
                cur_txt_ofs = 0;
            }

            /* init span info to snippet. */
            if(cur_txt_ofs == 0) {
                snippet.span = cur_span;
                snippet.font = lv_span_get_style_text_font(obj, cur_span);
                snippet.letter_space = lv_span_get_style_text_letter_space(obj, cur_span);
                snippet.line_h = lv_font_get_line_height(snippet.font) + line_space;
            }

            if(spans->overflow == LV_SPAN_OVERFLOW_ELLIPSIS) {
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
        if(spans->overflow == LV_SPAN_OVERFLOW_ELLIPSIS) {
            lv_coord_t next_line_h = snippet.line_h;
            if(cur_txt[cur_txt_ofs] == '\0') {  /* current span deal with ok, need get next line first line height */
                next_line_h = 0;
                if(cur_span) {
                    lv_span_t * next_span = _lv_ll_get_next(&spans->child_ll, cur_span);
                    if(next_span) { /* have the next line */
                        next_line_h = lv_font_get_line_height(lv_span_get_style_text_font(obj, next_span)) + line_space;
                    }
                }
            }
            if(txt_pos.y + max_line_h + next_line_h > coords->y2  + 1) {
                ellipsis_valid = true;
            }
        }

        /* align deal with */
        if(spans->align != LV_TEXT_ALIGN_LEFT) {
            lv_coord_t align_ofs = 0;
            lv_coord_t txts_w = 0;
            for(int i = 0; i < item_cnt; i++) {
                lv_snippet_t * pinfo = lv_get_snippet(i);
                txts_w += pinfo->txt_w;
            }
            if(spans->align == LV_TEXT_ALIGN_CENTER) {
                align_ofs = (max_width - txts_w) / 2;
            }
            else if(spans->align == LV_TEXT_ALIGN_RIGHT) {
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
            lv_color_t letter_color = lv_span_get_style_text_color(obj, pinfo->span);
            lv_opa_t letter_opa = lv_span_get_style_text_opa(obj, pinfo->span);
            lv_blend_mode_t blend_mode = lv_span_get_style_text_blend_mode(obj, pinfo->span);
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
            lv_text_decor_t decor = lv_span_get_style_text_decor(obj, pinfo->span);
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
