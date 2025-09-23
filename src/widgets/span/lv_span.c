/**
 * @file lv_span.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_span_private.h"
#include "../../misc/lv_area_private.h"
#include "../../draw/lv_draw_private.h"
#include "../../core/lv_obj_class_private.h"

#if LV_USE_SPAN != 0

#include "../../misc/lv_assert.h"
#include "../../misc/lv_text_private.h"
#include "../../misc/lv_bidi_private.h"
#include "../../others/observer/lv_observer_private.h"
#include "../../misc/lv_text_ap.h"
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_spangroup_class)
#define snippet_stack LV_GLOBAL_DEFAULT()->span_snippet_stack

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_span_t * span;
    const char * txt;
    const lv_font_t * font;
    uint32_t   bytes;
    int32_t txt_w;
    int32_t line_h;
    int32_t letter_space;
} lv_snippet_t;

struct _snippet_stack {
    lv_snippet_t    stack[LV_SPAN_SNIPPET_STACK_SIZE];
    uint32_t        index;
};

typedef struct {
    lv_subject_t * subject;
    void * element; /**< span of a span group*/
    const char * fmt;
} bind_element_string_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_spangroup_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_spangroup_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_spangroup_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_main(lv_event_t * e);

static const lv_font_t * lv_span_get_style_text_font(lv_obj_t * par, lv_span_t * span);
static int32_t lv_span_get_style_text_letter_space(lv_obj_t * par, lv_span_t * span);
static lv_color_t lv_span_get_style_text_color(lv_obj_t * par, lv_span_t * span);
static lv_opa_t lv_span_get_style_text_opa(lv_obj_t * par, lv_span_t * span);
static int32_t lv_span_get_style_text_decor(lv_obj_t * par, lv_span_t * span);

static inline void span_text_check(const char ** text);
static void lv_draw_span(lv_obj_t * obj, lv_layer_t * layer);
static bool lv_text_get_snippet(const char * txt, const lv_font_t * font, int32_t letter_space,
                                int32_t max_width, lv_text_flag_t flag, int32_t * use_width,
                                uint32_t * end_ofs);

static void lv_snippet_clear(void);
static uint32_t lv_get_snippet_count(void);
static void lv_snippet_push(lv_snippet_t * item);
static lv_snippet_t * lv_get_snippet(uint32_t index);
static int32_t convert_indent_pct(lv_obj_t * spans, int32_t width);

static lv_span_coords_t make_span_coords(const lv_span_t * prev_span, const lv_span_t * curr_span, int32_t width,
                                         lv_area_t padding, int32_t indent);
#if LV_USE_OBSERVER
    static void span_text_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_spangroup_class  = {
    .base_class = &lv_obj_class,
    .constructor_cb = lv_spangroup_constructor,
    .destructor_cb = lv_spangroup_destructor,
    .event_cb = lv_spangroup_event,
    .instance_size = sizeof(lv_spangroup_t),
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .name = "lv_span",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_span_stack_init(void)
{
    struct _snippet_stack * stack = snippet_stack = lv_malloc(sizeof(struct _snippet_stack));
    LV_ASSERT_MALLOC(stack);
    if(!stack) {
        LV_LOG_ERROR("malloc failed for snippet_stack");
    }
}

void lv_span_stack_deinit(void)
{
    lv_free(snippet_stack);
}

lv_obj_t * lv_spangroup_create(lv_obj_t * par)
{
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_spangroup_class, par);
    lv_obj_class_init_obj(obj);
    return obj;
}

lv_span_t * lv_spangroup_add_span(lv_obj_t * obj)
{
    if(obj == NULL) {
        return NULL;
    }

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    lv_span_t * span = lv_ll_ins_tail(&spans->child_ll);
    LV_ASSERT_MALLOC(span);

    lv_style_init(&span->style);
    span->txt = (char *)"";
    span->static_flag = 1;

    lv_spangroup_refresh(obj);

    return span;
}

void lv_spangroup_delete_span(lv_obj_t * obj, lv_span_t * span)
{
    if(obj == NULL || span == NULL) {
        return;
    }

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    lv_span_t * cur_span;
    LV_LL_READ(&spans->child_ll, cur_span) {
        if(cur_span == span) {
            lv_ll_remove(&spans->child_ll, cur_span);
            if(cur_span->txt && cur_span->static_flag == 0) {
                lv_free(cur_span->txt);
                cur_span->txt = NULL;
            }
            lv_style_reset(&cur_span->style);
            lv_free(cur_span);
            cur_span = NULL;
            break;
        }
    }

    lv_spangroup_refresh(obj);
}

/*=====================
 * Setter functions
 *====================*/

void lv_span_set_text(lv_span_t * span, const char * text)
{
    if(span == NULL || text == NULL) {
        return;
    }

    size_t text_alloc_len = 0;

#if LV_USE_ARABIC_PERSIAN_CHARS
    text_alloc_len = lv_text_ap_calc_bytes_count(text);
#else
    text_alloc_len = lv_strlen(text) + 1;
#endif

    if(span->txt == NULL || span->static_flag == 1) {
        span->txt = lv_malloc(text_alloc_len);
        LV_ASSERT_MALLOC(span->txt);
    }
    else {
        span->txt = lv_realloc(span->txt, text_alloc_len);
        LV_ASSERT_MALLOC(span->txt);
    }

    if(span->txt == NULL) return;

    span->static_flag = 0;

#if LV_USE_ARABIC_PERSIAN_CHARS
    lv_text_ap_proc(text, span->txt);
#else
    lv_memcpy(span->txt, text, text_alloc_len);
#endif
}

void lv_span_set_text_fmt(lv_span_t * span, const char * fmt, ...)
{
    if(span == NULL || fmt == NULL) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    char * text = lv_text_set_text_vfmt(fmt, args);
    LV_ASSERT_MALLOC(text);
    if(text == NULL) {
        va_end(args);
        return;
    }

    va_end(args);

    if(span->txt && !span->static_flag) {
        lv_free(span->txt);
    }

    span->static_flag = 0;
    span->txt = text;
}


void lv_spangroup_set_span_text(lv_obj_t * obj, lv_span_t * span, const char * text)
{
    lv_span_set_text(span, text);
    lv_spangroup_refresh(obj);
}

void lv_span_set_text_static(lv_span_t * span, const char * text)
{
    if(span == NULL || text == NULL) {
        return;
    }

    if(span->txt && span->static_flag == 0) {
        lv_free(span->txt);
        span->txt = NULL;
    }
    span->static_flag = 1;

#if LV_USE_ARABIC_PERSIAN_CHARS
    size_t text_alloc_len = lv_text_ap_calc_bytes_count(text);
    span->txt = lv_malloc(text_alloc_len);
    LV_ASSERT_MALLOC(span->txt)
    lv_text_ap_proc(text, span->txt);
    span->static_flag = 0;
#else
    span->txt = (char *)text;
#endif
}

void lv_spangroup_set_span_text_static(lv_obj_t * obj, lv_span_t * span, const char * text)
{
    lv_span_set_text_static(span, text);
    lv_spangroup_refresh(obj);
}

void lv_spangroup_set_span_text_fmt(lv_obj_t * obj, lv_span_t * span, const char * fmt, ...)
{
    if(span == NULL || fmt == NULL) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    char * text = lv_text_set_text_vfmt(fmt, args);
    LV_ASSERT_MALLOC(text);
    if(text == NULL) {
        va_end(args);
        return;
    }

    va_end(args);

    if(span->txt && !span->static_flag) {
        lv_free(span->txt);
    }

    span->static_flag = 0;
    span->txt = text;

    lv_spangroup_refresh(obj);
}

void lv_spangroup_set_span_style(lv_obj_t * obj, lv_span_t * span, const lv_style_t * style)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(span);

    lv_style_copy(&span->style, style);

    lv_spangroup_refresh(obj);
}

void lv_spangroup_set_align(lv_obj_t * obj, lv_text_align_t align)
{
    LV_LOG_WARN("DEPRECATED. Use the text_align style property instead");

    lv_obj_set_style_text_align(obj, align, LV_PART_MAIN);
}

void lv_spangroup_set_overflow(lv_obj_t * obj, lv_span_overflow_t overflow)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    if(spans->overflow == overflow) return;
    if(overflow >= LV_SPAN_OVERFLOW_LAST) return;
    spans->overflow = overflow;
    lv_obj_invalidate(obj);
}

void lv_spangroup_set_indent(lv_obj_t * obj, int32_t indent)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    if(spans->indent == indent) return;

    spans->indent = indent;

    lv_spangroup_refresh(obj);
}

void lv_spangroup_set_mode(lv_obj_t * obj, lv_span_mode_t mode)
{
    LV_LOG_WARN("DEPRECATED, set the width to LV_SIZE_CONTENT or fixed value to control expanding/wrapping");
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(mode >= LV_SPAN_MODE_LAST) return;

    if(mode == LV_SPAN_MODE_EXPAND) {
        lv_obj_set_width(obj, LV_SIZE_CONTENT);
        lv_obj_set_height(obj, LV_SIZE_CONTENT);
    }
    else if(mode == LV_SPAN_MODE_BREAK) {
        if(lv_obj_get_style_width(obj, LV_PART_MAIN) == LV_SIZE_CONTENT) {
            lv_obj_set_width(obj, 100);
        }
        lv_obj_set_height(obj, LV_SIZE_CONTENT);
    }
    else if(mode == LV_SPAN_MODE_FIXED) {
        /* use this mode, The user needs to set the size. */
        /* This is just to prevent an infinite loop. */
        if(lv_obj_get_style_width(obj, LV_PART_MAIN) == LV_SIZE_CONTENT) {
            lv_obj_set_width(obj, 100);
        }
        if(lv_obj_get_style_height(obj, LV_PART_MAIN) == LV_SIZE_CONTENT) {
            lv_obj_set_content_height(obj, 100);
        }
    }

    lv_spangroup_refresh(obj);
}

void lv_spangroup_set_max_lines(lv_obj_t * obj, int32_t lines)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    spans->lines = lines;

    lv_spangroup_refresh(obj);
}

/*=====================
 * Getter functions
 *====================*/

lv_style_t * lv_span_get_style(lv_span_t * span)
{
    return &span->style;
}

const char * lv_span_get_text(lv_span_t * span)
{
    return span->txt;
}

lv_span_t * lv_spangroup_get_child(const lv_obj_t * obj, int32_t id)
{
    if(obj == NULL) {
        return NULL;
    }

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    lv_ll_t * linked_list = &spans->child_ll;

    bool traverse_forwards = (id >= 0);
    int32_t cur_idx = 0;
    lv_ll_node_t * cur_node = linked_list->head;

    /*If using a negative index, start from the tail and use cur -1 to indicate the end*/
    if(!traverse_forwards) {
        cur_idx = -1;
        cur_node = linked_list->tail;
    }

    while(cur_node != NULL) {
        if(cur_idx == id) {
            return (lv_span_t *) cur_node;
        }
        if(traverse_forwards) {
            cur_node = (lv_ll_node_t *) lv_ll_get_next(linked_list, cur_node);
            cur_idx++;
        }
        else {
            cur_node = (lv_ll_node_t *) lv_ll_get_prev(linked_list, cur_node);
            cur_idx--;
        }
    }

    return NULL;
}

uint32_t lv_spangroup_get_span_count(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(obj == NULL) {
        return 0;
    }

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    return lv_ll_get_len(&(spans->child_ll));
}

lv_text_align_t lv_spangroup_get_align(lv_obj_t * obj)
{
    return lv_obj_get_style_text_align(obj, LV_PART_MAIN);
}

lv_span_overflow_t lv_spangroup_get_overflow(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    return spans->overflow;
}

int32_t lv_spangroup_get_indent(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    return spans->indent;
}

lv_span_mode_t lv_spangroup_get_mode(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(lv_obj_get_style_width(obj, LV_PART_MAIN) == LV_SIZE_CONTENT) {
        return LV_SPAN_MODE_EXPAND;
    }

    /*Width is fixed for the following cases*/
    else if(lv_obj_get_style_height(obj, LV_PART_MAIN) == LV_SIZE_CONTENT) {
        return LV_SPAN_MODE_BREAK;
    }
    /*Both fixed*/
    else {
        return LV_SPAN_MODE_FIXED;
    }
}

int32_t lv_spangroup_get_max_lines(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    return spans->lines;
}


int32_t lv_spangroup_get_max_line_height(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;

    int32_t max_line_h = 0;
    lv_span_t * cur_span;
    LV_LL_READ(&spans->child_ll, cur_span) {
        const lv_font_t * font = lv_span_get_style_text_font(obj, cur_span);
        int32_t line_h = lv_font_get_line_height(font);
        if(line_h > max_line_h) {
            max_line_h = line_h;
        }
    }

    return max_line_h;
}

uint32_t lv_spangroup_get_expand_width(lv_obj_t * obj, uint32_t max_width)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;

    if(lv_ll_get_head(&spans->child_ll) == NULL) {
        return 0;
    }

    uint32_t width = LV_COORD_IS_PCT(spans->indent) ? 0 : spans->indent;
    int32_t letter_space = 0;
    lv_span_t * cur_span;
    LV_LL_READ(&spans->child_ll, cur_span) {
        uint32_t letter;
        uint32_t letter_next;
        const lv_font_t * font = lv_span_get_style_text_font(obj, cur_span);

        letter_space = lv_span_get_style_text_letter_space(obj, cur_span);
        uint32_t j = 0;
        const char * cur_txt = cur_span->txt;
        span_text_check(&cur_txt);
        while(cur_txt[j] != '\0') {
            if(max_width > 0 && width >= max_width) {
                return max_width;
            }
            letter      = lv_text_encoded_next(cur_txt, &j);
            letter_next = lv_text_encoded_next(&cur_txt[j], NULL);
            uint32_t letter_w = lv_font_get_glyph_width(font, letter, letter_next);
            width = width + letter_w + letter_space;
        }
    }

    return width - letter_space;
}

int32_t lv_spangroup_get_expand_height(lv_obj_t * obj, int32_t width)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    if(lv_ll_get_head(&spans->child_ll) == NULL || width <= 0) {
        return 0;
    }

    /* init draw variable */
    lv_text_flag_t txt_flag = LV_TEXT_FLAG_NONE;
    int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    int32_t max_width = width;
    int32_t indent = convert_indent_pct(obj, max_width);
    int32_t max_w  = max_width - indent; /* first line need minus indent */

    /* coords of draw span-txt */
    lv_point_t txt_pos;
    lv_point_set(&txt_pos, indent, 0); /* first line need add indent */

    lv_span_t * cur_span = lv_ll_get_head(&spans->child_ll);
    const char * cur_txt = cur_span->txt;
    span_text_check(&cur_txt);
    uint32_t cur_txt_ofs = 0;
    lv_snippet_t snippet;   /* use to save cur_span info and push it to stack */
    lv_memset(&snippet, 0, sizeof(snippet));

    lv_span_t * prev_span = cur_span;
    int32_t line_cnt = 0;
    int32_t lines = spans->lines < 0 ? INT32_MAX : spans->lines;
    /* the loop control how many lines need to draw */
    while(cur_span) {
        int snippet_cnt = 0;
        int32_t max_line_h = 0;  /* the max height of span-font when a line have a lot of span */

        /* the loop control to find a line and push the relevant span info into stack  */
        while(1) {
            /* switch to the next span when current is end */
            if(cur_txt[cur_txt_ofs] == '\0') {
                cur_span->trailing_pos = txt_pos;

                cur_span = lv_ll_get_next(&spans->child_ll, cur_span);
                if(cur_span == NULL) break;
                cur_txt = cur_span->txt;
                span_text_check(&cur_txt);
                cur_txt_ofs = 0;
                /* maybe also cur_txt[cur_txt_ofs] == '\0' */
                continue;
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
            int32_t use_width = 0;
            bool isfill = lv_text_get_snippet(&cur_txt[cur_txt_ofs], snippet.font, snippet.letter_space,
                                              max_w, txt_flag, &use_width, &next_ofs);
            if(isfill) txt_pos.x = 0;
            else txt_pos.x += use_width;

            /* break word deal width */
            if(isfill && next_ofs > 0 && snippet_cnt > 0) {
                int32_t drawn_width = use_width;
                if(lv_ll_get_next(&spans->child_ll, cur_span) == NULL) {
                    drawn_width -= snippet.letter_space;
                }
                if(max_w < drawn_width) {
                    break;
                }

                uint32_t tmp_ofs = next_ofs;
                uint32_t letter = lv_text_encoded_prev(&cur_txt[cur_txt_ofs], &tmp_ofs);
                uint32_t letter_next = lv_text_encoded_next(&cur_txt[cur_txt_ofs + next_ofs], NULL);
                if(!(letter == '\0' || letter == '\n' || letter == '\r' || lv_text_is_break_char(letter) ||
                     lv_text_is_a_word(letter) || lv_text_is_a_word(letter_next))) {
                    if(!(letter_next == '\0' || letter_next == '\n'  || letter_next == '\r' || lv_text_is_break_char(letter_next))) {
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
            max_w = max_w - use_width;
            if(isfill  || max_w <= 0) {
                break;
            }
        }

        /* next line init */
        txt_pos.y += max_line_h;

        /* iterate all the spans in the current line and set the trailing height to the max line height */
        for(lv_span_t * tmp_span = prev_span;
            tmp_span && tmp_span != cur_span;
            tmp_span = lv_ll_get_next(&spans->child_ll, tmp_span))
            tmp_span->trailing_height = max_line_h;

        prev_span = cur_span;

        max_w = max_width;
        line_cnt += 1;
        if(line_cnt >= lines) {
            break;
        }
    }
    txt_pos.y -= line_space;

    return txt_pos.y;
}

lv_span_coords_t lv_spangroup_get_span_coords(lv_obj_t * obj, const lv_span_t * span)
{
    if(obj == NULL) return (lv_span_coords_t) {
        0
    };

    /* find previous span */
    const lv_spangroup_t * spangroup = (lv_spangroup_t *)obj;
    const lv_ll_t * spans = &spangroup->child_ll;
    const int32_t width = lv_obj_get_content_width(obj);
    const int32_t indent = lv_spangroup_get_indent(obj);

    if(span == NULL || lv_ll_get_head(spans) == NULL) return (lv_span_coords_t) {
        0
    };

    lv_span_t * prev_span = NULL;
    lv_span_t * curr_span;
    LV_LL_READ(spans, curr_span) {
        if(curr_span == span) break;
        prev_span = curr_span;
    }

    const uint32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    return make_span_coords(prev_span, curr_span, width, (lv_area_t) {
        .x1 = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width,
        .y1 = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width,
        .x2 = lv_obj_get_style_pad_right(obj, LV_PART_MAIN) + border_width, .y2 = 0
    },
    indent);
}

lv_span_t * lv_spangroup_get_span_by_point(lv_obj_t * obj, const lv_point_t * p)
{
    const lv_spangroup_t * spangroup = (lv_spangroup_t *)obj;
    const lv_ll_t * spans = &spangroup->child_ll;
    const int32_t width = lv_obj_get_content_width(obj);
    const int32_t indent = lv_spangroup_get_indent(obj);

    if(obj == NULL || p == NULL || lv_ll_get_head(spans) == NULL) return NULL;

    lv_point_t point;
    point.x = p->x - obj->coords.x1;
    point.y = p->y - obj->coords.y1;

    /* find previous span */

    const lv_span_t * prev_span = NULL;
    lv_span_t * curr_span;
    LV_LL_READ(spans, curr_span) {
        lv_span_coords_t coords = make_span_coords(prev_span, curr_span, width, (lv_area_t) {
            .x1 = lv_obj_get_style_pad_left(obj, LV_PART_MAIN),
            .y1 = lv_obj_get_style_pad_top(obj, LV_PART_MAIN),
            .x2 = lv_obj_get_style_pad_right(obj, LV_PART_MAIN),
            .y2 = 0
        },
        indent);
        if(lv_area_is_point_on(&coords.heading,  &point, 0) ||
           lv_area_is_point_on(&coords.middle,   &point, 0) ||
           lv_area_is_point_on(&coords.trailing, &point, 0)) {
            return curr_span;
        }
        prev_span = curr_span;
    }
    return NULL;
}


/*=====================
 * Other functions
 *====================*/

void lv_spangroup_refresh(lv_obj_t * obj)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    spans->refresh = 1;
    lv_obj_invalidate(obj);
    lv_obj_refresh_self_size(obj);
}

#if LV_USE_OBSERVER
lv_observer_t * lv_spangroup_bind_span_text(lv_obj_t * obj, lv_span_t * span, lv_subject_t * subject, const char * fmt)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(span);

    if(fmt == NULL) {
        if(subject->type == LV_SUBJECT_TYPE_INT) {
            fmt = "%d";
        }
#if LV_USE_FLOAT
        else if(subject->type == LV_SUBJECT_TYPE_FLOAT) {
            fmt = "%0.1f";
        }
#endif
        else if(subject->type != LV_SUBJECT_TYPE_STRING && subject->type != LV_SUBJECT_TYPE_POINTER) {
            LV_LOG_WARN("Incompatible subject type: %d", subject->type);
            return NULL;
        }
    }
    else {
        if(subject->type != LV_SUBJECT_TYPE_STRING && subject->type != LV_SUBJECT_TYPE_POINTER &&
           subject->type != LV_SUBJECT_TYPE_INT && subject->type != LV_SUBJECT_TYPE_FLOAT) {
            LV_LOG_WARN("Incompatible subject type: %d", subject->type);
            return NULL;
        }
    }

    bind_element_string_t * user_data = lv_zalloc(sizeof(bind_element_string_t));
    if(user_data == NULL) {
        LV_LOG_WARN("Couldn't allocate user_data");
        LV_ASSERT_MALLOC(user_data);
        return NULL;
    }

    user_data->subject = subject;
    user_data->element = span;
    user_data->fmt = fmt;

    lv_observer_t * observer = lv_subject_add_observer_obj(subject, span_text_observer_cb, obj, user_data);
    observer->auto_free_user_data = 1;

    return observer;
}
#endif /*LV_USE_OBSERVER*/


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_spangroup_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    lv_ll_init(&spans->child_ll, sizeof(lv_span_t));
    spans->indent = 0;
    spans->lines = -1;
    spans->overflow = LV_SPAN_OVERFLOW_CLIP;
    spans->cache_w = 0;
    spans->cache_h = 0;
    spans->refresh = 1;
}

static void lv_spangroup_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;
    lv_span_t * cur_span = lv_ll_get_head(&spans->child_ll);
    while(cur_span) {
        lv_ll_remove(&spans->child_ll, cur_span);
        if(cur_span->txt && cur_span->static_flag == 0) {
            lv_free(cur_span->txt);
            cur_span->txt = NULL;
        }
        lv_style_reset(&cur_span->style);
        lv_free(cur_span);
        cur_span = lv_ll_get_head(&spans->child_ll);
    }
}

static void lv_spangroup_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    /* Call the ancestor's event handler */
    if(lv_obj_event_base(MY_CLASS, e) != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;

    if(code == LV_EVENT_DRAW_MAIN) {
        draw_main(e);
    }
    else if(code == LV_EVENT_STYLE_CHANGED) {
        lv_spangroup_refresh(obj);
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        lv_spangroup_refresh(obj);
    }
    else if(code == LV_EVENT_GET_SELF_SIZE) {
        int32_t width = 0;
        int32_t height = 0;
        lv_point_t * self_size = lv_event_get_param(e);

        lv_span_mode_t mode = lv_spangroup_get_mode(obj);
        if(mode == LV_SPAN_MODE_EXPAND) {
            if(spans->refresh) {
                spans->cache_w = (int32_t)lv_spangroup_get_expand_width(obj, 0);
                spans->cache_h = lv_spangroup_get_max_line_height(obj);
                spans->refresh = 0;
            }
            width = spans->cache_w;
            height = spans->cache_h;
        }
        else if(mode == LV_SPAN_MODE_BREAK) {
            width = lv_obj_get_content_width(obj);
            if(self_size->y >= 0) {
                if(width != spans->cache_w || spans->refresh) {
                    height = lv_spangroup_get_expand_height(obj, width);
                    spans->cache_w = width;
                    spans->cache_h = height;
                    spans->refresh = 0;
                }
                else {
                    height = spans->cache_h;
                }
            }
        }
        else if(mode == LV_SPAN_MODE_FIXED) {
            width =  self_size->x >= 0 ? lv_obj_get_content_width(obj) : 0;
            height = self_size->y >= 0 ? lv_obj_get_content_height(obj) : 0;
        }
        self_size->x = LV_MAX(self_size->x, width);
        self_size->y = LV_MAX(self_size->y, height);
    }
}

static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_layer_t * layer = lv_event_get_layer(e);

    lv_draw_span(obj, layer);
}

/**
 * @return true for txt fill the max_width.
 */
static bool lv_text_get_snippet(const char * txt, const lv_font_t * font,
                                int32_t letter_space, int32_t max_width, lv_text_flag_t flag,
                                int32_t * use_width, uint32_t * end_ofs)
{
    if(txt == NULL || txt[0] == '\0') {
        *end_ofs = 0;
        *use_width = 0;
        return false;
    }

    int32_t real_max_width = max_width;
#if !LV_USE_FONT_PLACEHOLDER
    /* fix incomplete text display when disable the placeholder. */
    /* workaround by: https://github.com/lvgl/lvgl/issues/3685 */
    real_max_width++;
#endif

    lv_text_attributes_t attributes = {0};
    attributes.letter_space = letter_space;
    attributes.max_width = real_max_width;
    attributes.text_flags = flag;

    uint32_t ofs = lv_text_get_next_line(txt, LV_TEXT_LEN_MAX, font, use_width, &attributes);
    *end_ofs = ofs;

    if(txt[ofs] == '\0' && *use_width <= attributes.max_width && !(ofs && (txt[ofs - 1] == '\n' || txt[ofs - 1] == '\r'))) {
        return false;
    }
    else {
        return true;
    }
}

static void lv_snippet_push(lv_snippet_t * item)
{
    struct _snippet_stack * stack_p = snippet_stack;
    if(stack_p->index < LV_SPAN_SNIPPET_STACK_SIZE) {
        lv_memcpy(&stack_p->stack[stack_p->index], item, sizeof(lv_snippet_t));
        stack_p->index++;
    }
    else {
        LV_LOG_ERROR("span draw stack overflow, please set LV_SPAN_SNIPPET_STACK_SIZE too larger");
    }
}

static uint32_t lv_get_snippet_count(void)
{
    return snippet_stack->index;
}

static lv_snippet_t * lv_get_snippet(uint32_t index)
{
    return &snippet_stack->stack[index];
}

static void lv_snippet_clear(void)
{
    snippet_stack->index = 0;
}

static const lv_font_t * lv_span_get_style_text_font(lv_obj_t * par, lv_span_t * span)
{
    const lv_font_t * font;
    lv_style_value_t value;
    lv_style_res_t res = lv_style_get_prop(&span->style, LV_STYLE_TEXT_FONT, &value);
    if(res != LV_STYLE_RES_FOUND) {
        font = lv_obj_get_style_text_font(par, LV_PART_MAIN);
    }
    else {
        font = (const lv_font_t *)value.ptr;
    }
    return font;
}

static int32_t lv_span_get_style_text_letter_space(lv_obj_t * par, lv_span_t * span)
{
    int32_t letter_space;
    lv_style_value_t value;
    lv_style_res_t res = lv_style_get_prop(&span->style, LV_STYLE_TEXT_LETTER_SPACE, &value);
    if(res != LV_STYLE_RES_FOUND) {
        letter_space = lv_obj_get_style_text_letter_space(par, LV_PART_MAIN);
    }
    else {
        letter_space = (int32_t)value.num;
    }
    return letter_space;
}

static lv_color_t lv_span_get_style_text_color(lv_obj_t * par, lv_span_t * span)
{
    lv_style_value_t value;
    lv_style_res_t res = lv_style_get_prop(&span->style, LV_STYLE_TEXT_COLOR, &value);
    if(res != LV_STYLE_RES_FOUND) {
        value.color = lv_obj_get_style_text_color(par, LV_PART_MAIN);
    }
    return value.color;
}

static lv_opa_t lv_span_get_style_text_opa(lv_obj_t * par, lv_span_t * span)
{
    lv_opa_t opa;
    lv_style_value_t value;
    lv_style_res_t res = lv_style_get_prop(&span->style, LV_STYLE_TEXT_OPA, &value);
    if(res != LV_STYLE_RES_FOUND) {
        opa = (lv_opa_t)lv_obj_get_style_text_opa(par, LV_PART_MAIN);
    }
    else {
        opa = (lv_opa_t)value.num;
    }
    return opa;
}

static int32_t lv_span_get_style_text_decor(lv_obj_t * par, lv_span_t * span)
{
    int32_t decor;
    lv_style_value_t value;
    lv_style_res_t res = lv_style_get_prop(&span->style, LV_STYLE_TEXT_DECOR, &value);
    if(res != LV_STYLE_RES_FOUND) {
        decor = (lv_text_decor_t)lv_obj_get_style_text_decor(par, LV_PART_MAIN);
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

static int32_t convert_indent_pct(lv_obj_t * obj, int32_t width)
{
    lv_spangroup_t * spans = (lv_spangroup_t *)obj;

    int32_t indent = spans->indent;
    if(LV_COORD_IS_PCT(spans->indent)) {
        if(lv_spangroup_get_mode(obj) == LV_SPAN_MODE_EXPAND) {
            indent = 0;
        }
        else {
            indent = (width * LV_COORD_GET_PCT(spans->indent)) / 100;
        }
    }

    return indent;
}

/**
 * draw span group
 * @param spans obj handle
 * @param coords coordinates of the label
 * @param mask the label will be drawn only in this area
 */
static void lv_draw_span(lv_obj_t * obj, lv_layer_t * layer)
{

    lv_area_t coords;
    lv_obj_get_content_coords(obj, &coords);

    lv_spangroup_t * spans = (lv_spangroup_t *)obj;

    /* return if not span */
    if(lv_ll_get_head(&spans->child_ll) == NULL) {
        return;
    }

    /* return if no draw area */
    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, &coords, &layer->_clip_area))  return;
    const lv_area_t clip_area_ori = layer->_clip_area;
    layer->_clip_area = clip_area;

    /* init draw variable */
    lv_text_flag_t txt_flag = LV_TEXT_FLAG_NONE;
    int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    int32_t max_width = lv_area_get_width(&coords);
    int32_t indent = convert_indent_pct(obj, max_width);
    int32_t max_w  = max_width - indent; /* first line need minus indent */
    lv_opa_t obj_opa = lv_obj_get_style_opa_recursive(obj, LV_PART_MAIN);

    /* coords of draw span-txt */
    lv_point_t txt_pos;
    txt_pos.y = coords.y1;
    txt_pos.x = coords.x1 + indent; /* first line need add indent */

    lv_span_t * cur_span = lv_ll_get_head(&spans->child_ll);
    const char * cur_txt = cur_span->txt;
    span_text_check(&cur_txt);

    lv_text_align_t align = lv_obj_get_style_text_align(obj, LV_PART_MAIN);
#if LV_USE_BIDI
    lv_base_dir_t base_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);
    if(base_dir == LV_BASE_DIR_AUTO) {
        base_dir = lv_bidi_detect_base_dir(cur_txt) == LV_BASE_DIR_RTL ? LV_BASE_DIR_RTL : LV_BASE_DIR_AUTO;
    }

    while(cur_span) {
        cur_span = lv_ll_get_next(&spans->child_ll, cur_span);
        if(cur_span == NULL) break;
        cur_txt = cur_span->txt;
        span_text_check(&cur_txt);

        if(base_dir == LV_BASE_DIR_AUTO) {
            base_dir = lv_bidi_detect_base_dir(cur_txt) == LV_BASE_DIR_RTL ? LV_BASE_DIR_RTL : LV_BASE_DIR_AUTO;
        }
    }
    cur_span = lv_ll_get_head(&spans->child_ll);
    cur_txt = cur_span->txt;
#endif

    uint32_t cur_txt_ofs = 0;
    lv_snippet_t snippet;   /* use to save cur_span info and push it to stack */
    lv_memzero(&snippet, sizeof(snippet));

    lv_draw_label_dsc_t label_draw_dsc;
    lv_draw_label_dsc_init(&label_draw_dsc);

    bool is_first_line = true;
    /* the loop control how many lines need to draw */
    while(cur_span) {
        bool is_end_line = false;
        bool ellipsis_valid = false;
        int32_t max_line_h = 0;  /* the max height of span-font when a line have a lot of span */
        int32_t max_baseline = 0; /*baseline of the highest span*/
        lv_snippet_clear();

        /* the loop control to find a line and push the relevant span info into stack  */
        while(1) {
            /* switch to the next span when current is end */
            if(cur_txt[cur_txt_ofs] == '\0') {
                cur_span = lv_ll_get_next(&spans->child_ll, cur_span);
                if(cur_span == NULL) break;
                cur_txt = cur_span->txt;
                span_text_check(&cur_txt);
                cur_txt_ofs = 0;
                /* maybe also cur_txt[cur_txt_ofs] == '\0' */
                continue;
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
            int32_t use_width = 0;
            bool isfill = lv_text_get_snippet(&cur_txt[cur_txt_ofs], snippet.font, snippet.letter_space,
                                              max_w, txt_flag, &use_width, &next_ofs);

            if(isfill) {
                if(next_ofs > 0 && lv_get_snippet_count() > 0) {
                    int32_t drawn_width = use_width;
                    if(lv_ll_get_next(&spans->child_ll, cur_span) == NULL) {
                        drawn_width -= snippet.letter_space;
                    }
                    /* To prevent infinite loops, the lv_text_get_next_line() may return incomplete words, */
                    /* This phenomenon should be avoided when lv_get_snippet_count() > 0 */
                    if(max_w < drawn_width) {
                        break;
                    }
                    uint32_t tmp_ofs = next_ofs;
                    uint32_t letter = lv_text_encoded_prev(&cur_txt[cur_txt_ofs], &tmp_ofs);
                    uint32_t letter_next = lv_text_encoded_next(&cur_txt[cur_txt_ofs + next_ofs], NULL);
                    if(!(letter == '\0' || letter == '\n' || letter == '\r' || lv_text_is_break_char(letter) ||
                         lv_text_is_a_word(letter) || lv_text_is_a_word(letter_next))) {
                        if(!(letter_next == '\0' || letter_next == '\n'  || letter_next == '\r' || lv_text_is_break_char(letter_next))) {
                            break;
                        }
                    }
                }
            }

            snippet.txt = &cur_txt[cur_txt_ofs];
            snippet.bytes = next_ofs;
            snippet.txt_w = use_width;
            cur_txt_ofs += next_ofs;
            if(max_line_h < snippet.line_h) {
                max_line_h = snippet.line_h;
                max_baseline = snippet.font->base_line;
            }

            lv_snippet_push(&snippet);
            max_w = max_w - use_width;
            if(isfill || max_w <= 0) {
                break;
            }
        }

        /* start current line deal with */

        uint32_t item_cnt = lv_get_snippet_count();
        if(item_cnt == 0) {     /* break if stack is empty */
            break;
        }

        /* Whether the current line is the end line and does overflow processing */
        {
            lv_snippet_t * last_snippet = lv_get_snippet(item_cnt - 1);
            int32_t next_line_h = last_snippet->line_h;
            if(last_snippet->txt[last_snippet->bytes] == '\0') {
                next_line_h = 0;
                lv_span_t * next_span = lv_ll_get_next(&spans->child_ll, last_snippet->span);
                if(next_span && next_span->txt && next_span->txt[0]) { /* have the next line */
                    next_line_h = lv_font_get_line_height(lv_span_get_style_text_font(obj, next_span)) + line_space;
                }
            }
            if(txt_pos.y + max_line_h + next_line_h - line_space > coords.y2 + 1) { /* for overflow if is end line. */
                ellipsis_valid = spans->overflow == LV_SPAN_OVERFLOW_ELLIPSIS;
                is_end_line = true;
            }
        }

        /*Go the first visible line*/
        if(txt_pos.y + max_line_h < clip_area.y1) {
            goto Next_line_init;
        }

        /* align deal with */
#if LV_USE_BIDI
        if(base_dir == LV_BASE_DIR_AUTO) {
            base_dir = LV_BASE_DIR_LTR;
        }

        if(align == LV_TEXT_ALIGN_AUTO) {
            if(base_dir == LV_BASE_DIR_RTL) align = LV_TEXT_ALIGN_RIGHT;
            else align = LV_TEXT_ALIGN_LEFT;
        }
#endif
        int32_t align_ofs = 0;
        int32_t txts_w = is_first_line ? indent : 0;
        uint32_t i_item;
        for(i_item = 0; i_item < item_cnt; i_item++) {
            lv_snippet_t * pinfo = lv_get_snippet(i_item);
            if(ellipsis_valid && i_item == item_cnt - 1) {
                uint32_t n_ofs = 0;
                ellipsis_valid = lv_text_get_snippet(pinfo->txt, pinfo->font, pinfo->letter_space, max_width - txts_w,
                                                     LV_TEXT_FLAG_BREAK_ALL, &pinfo->txt_w, &n_ofs);
                pinfo->bytes = n_ofs;
            }
            txts_w = txts_w + pinfo->txt_w;
        }
        txts_w -= lv_get_snippet(item_cnt - 1)->letter_space;
        align_ofs = max_width > txts_w ? max_width - txts_w : 0;
        if(align == LV_TEXT_ALIGN_CENTER) {
            align_ofs = align_ofs >> 1;
        }
        if(align == LV_TEXT_ALIGN_CENTER || align == LV_TEXT_ALIGN_RIGHT) {
            txt_pos.x += align_ofs;
        }

#if LV_USE_BIDI
        int32_t first_txt_pos_x = txt_pos.x;
        bool is_draw_rtl = false;
        lv_snippet_t * pinfo0 = lv_get_snippet(0);
        lv_base_dir_t bidi_dir = lv_bidi_detect_base_dir(pinfo0->txt);
        if(bidi_dir == LV_BASE_DIR_RTL && base_dir == LV_BASE_DIR_RTL) {
            is_draw_rtl = true;
            if(align == LV_TEXT_ALIGN_LEFT || align == LV_TEXT_ALIGN_CENTER) {
                txt_pos.x = coords.x2 - align_ofs;
            }
            else if(align == LV_TEXT_ALIGN_RIGHT) {
                txt_pos.x = coords.x2;
            }
        }
#endif
        /* draw line letters */
        uint32_t i;
        for(i = 0; i < item_cnt; i++) {
            lv_snippet_t * pinfo = lv_get_snippet(i);

#if LV_USE_BIDI
            char * bidi_txt;
            if(base_dir == LV_BASE_DIR_RTL) {
                bidi_txt = lv_malloc(pinfo->bytes + 1);
                LV_ASSERT_MALLOC(bidi_txt);
                lv_memcpy(bidi_txt, pinfo->txt, (size_t)pinfo->bytes);
                label_draw_dsc.bidi_dir = base_dir;
                label_draw_dsc.has_bided = true;
                label_draw_dsc.text_local = true;
                lv_bidi_process_paragraph(pinfo->txt, bidi_txt, pinfo->bytes, label_draw_dsc.bidi_dir, NULL, 0);
            }
            else {
                bidi_txt = (char *)pinfo->txt;
            }
#else
            const char * bidi_txt = pinfo->txt;
#endif

            lv_point_t pos;
            pos.x = txt_pos.x;
            pos.y = txt_pos.y + max_line_h - pinfo->line_h - (max_baseline - pinfo->font->base_line);
            label_draw_dsc.color = lv_span_get_style_text_color(obj, pinfo->span);
            label_draw_dsc.opa = lv_span_get_style_text_opa(obj, pinfo->span);
            label_draw_dsc.font = lv_span_get_style_text_font(obj, pinfo->span);
            if(obj_opa < LV_OPA_MAX) {
                label_draw_dsc.opa = LV_OPA_MIX2(label_draw_dsc.opa, obj_opa);
            }
            uint32_t txt_bytes = pinfo->bytes;

            if(pos.x > clip_area.x2) {
                continue;
            }

            label_draw_dsc.text = bidi_txt;
            label_draw_dsc.text_length = txt_bytes;
            label_draw_dsc.letter_space = pinfo->letter_space;
            label_draw_dsc.line_space = line_space;
            label_draw_dsc.decor = lv_span_get_style_text_decor(obj, pinfo->span);
            lv_area_t a;
            a.x1 = pos.x;
            a.y1 = pos.y;
            a.x2 = a.x1 + pinfo->txt_w;
            a.y2 = a.y1 + pinfo->line_h;

#if LV_USE_BIDI
            if(is_draw_rtl) {
                a.x1 = pos.x - pinfo->txt_w;
                a.x2 = pos.x;
            }
#endif

            bool need_draw_ellipsis = false;
            int32_t dot_width = 0;
            /* deal overflow */
            if(ellipsis_valid) {
                int32_t dot_letter_w = lv_font_get_glyph_width(pinfo->font, '.', '.');
                dot_width = dot_letter_w * 3;

                label_draw_dsc.flag = LV_TEXT_FLAG_BREAK_ALL;
                int32_t text_width = coords.x2 - a.x1;
                uint32_t next_ofs;
                text_width = text_width > dot_width ? text_width - dot_width : text_width;
                need_draw_ellipsis = lv_text_get_snippet(pinfo->txt, pinfo->font, pinfo->letter_space, text_width,
                                                         label_draw_dsc.flag, &pinfo->txt_w, &next_ofs);
                a.x2 = a.x1 + pinfo->txt_w;
                label_draw_dsc.text_length = next_ofs;
#if LV_USE_BIDI
                if(base_dir == LV_BASE_DIR_RTL) {
                    if(txt_bytes > label_draw_dsc.text_length) {
                        char * tmp_txt = lv_malloc(label_draw_dsc.text_length + 1);
                        LV_ASSERT_MALLOC(tmp_txt);

                        if(lv_bidi_detect_base_dir(bidi_txt) == LV_BASE_DIR_RTL) {
                            lv_memcpy(tmp_txt, bidi_txt + (txt_bytes - label_draw_dsc.text_length), (size_t)label_draw_dsc.text_length);
                        }
                        else {
                            lv_memcpy(tmp_txt, bidi_txt, (size_t)label_draw_dsc.text_length);
                        }

                        label_draw_dsc.text = tmp_txt;
                        lv_free(bidi_txt);
                    }
                    if(i == 0) {
                        a.x1 = a.x1 + dot_width;
                        a.x2 = a.x2 + dot_width;
                    }
                }
#endif
            }

            lv_draw_label(layer, &label_draw_dsc, &a);
#if LV_USE_BIDI
            if(label_draw_dsc.has_bided) {
                lv_free((void *)label_draw_dsc.text);
            }
#endif

            if(need_draw_ellipsis) {
                label_draw_dsc.text = "...";
                label_draw_dsc.text_length = LV_TEXT_LEN_MAX;

#if LV_USE_BIDI
                if(label_draw_dsc.bidi_dir == LV_BASE_DIR_RTL) {
                    a.x1 = first_txt_pos_x;
                    a.x2 = a.x1 + dot_width;
                }
                else {
                    a.x1 = a.x2;
                    a.x2 = a.x1 + dot_width;
                }
                label_draw_dsc.text_local = false;
#else
                a.x1 = a.x2;
                a.x2 = a.x1 + dot_width;
#endif

                lv_draw_label(layer, &label_draw_dsc, &a);
            }

            txt_pos.x = a.x2;

#if LV_USE_BIDI
            if(is_draw_rtl) {
                txt_pos.x = a.x1;
            }
#endif
        }

Next_line_init:
        /* next line init */
        is_first_line = false;
        txt_pos.x = coords.x1;
        txt_pos.y += max_line_h;
        if(is_end_line || txt_pos.y > clip_area.y2 + 1) {
            layer->_clip_area = clip_area_ori;
            return;
        }
        max_w = max_width;
    }
    layer->_clip_area = clip_area_ori;
}

static lv_span_coords_t make_span_coords(const lv_span_t * prev_span, const lv_span_t * curr_span, const int32_t width,
                                         const lv_area_t padding, const int32_t indent)
{
    lv_span_coords_t coords = { 0 };

    if(curr_span == NULL) return coords;

    /* first line */
    if(prev_span == NULL) {
        lv_area_set(&coords.heading, padding.x1 + indent, padding.y1, width + padding.x1,
                    curr_span->trailing_pos.y + padding.y1);
        lv_area_set(&coords.middle, coords.heading.x1, coords.heading.y2, curr_span->trailing_pos.x + padding.x1,
                    coords.heading.y2 + curr_span->trailing_height);
        lv_area_set(&coords.trailing, 0, 0, 0, 0);

        return coords;
    }

    /* start and end on the same line */
    const bool is_same_line = prev_span->trailing_pos.y == curr_span->trailing_pos.y;
    if(is_same_line == true) {
        lv_area_set(&coords.heading,
                    prev_span->trailing_pos.x + padding.x1, prev_span->trailing_pos.y + padding.y1,
                    curr_span->trailing_pos.x + padding.x1, curr_span->trailing_pos.y + curr_span->trailing_height + padding.y1);
        return coords;
    }

    /* common case */
    const lv_point_t pre_trailing_pos = prev_span->trailing_pos;
    const int32_t pre_trailing_height = prev_span->trailing_height;

    lv_area_set(&coords.heading,
                pre_trailing_pos.x + padding.x1, pre_trailing_pos.y + padding.y1,
                width + padding.x1, pre_trailing_pos.y + pre_trailing_height + padding.y1);
    /* When it happens to be two lines of text,
    * the y2 of the middle area is exactly the y1 + line height of the first line of text,
    * so the area of the middle area is empty.
    * */
    lv_area_set(&coords.middle,
                padding.x1, coords.heading.y2,
                width + padding.x1, curr_span->trailing_pos.y + padding.y1);
    lv_area_set(&coords.trailing,
                coords.middle.x1, coords.middle.y2,
                curr_span->trailing_pos.x + padding.x1, curr_span->trailing_pos.y + curr_span->trailing_height + padding.y1);

    return coords;
}

#if LV_USE_OBSERVER

static void span_text_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    bind_element_string_t * user_data = observer->user_data;

    if(user_data->fmt == NULL) {
        lv_spangroup_set_span_text(observer->target, user_data->element, subject->value.pointer);
    }
    else {
        switch(subject->type) {

            case LV_SUBJECT_TYPE_INT:
                lv_spangroup_set_span_text_fmt(observer->target, user_data->element, user_data->fmt, subject->value.num);
                break;
#if LV_USE_FLOAT
            case LV_SUBJECT_TYPE_FLOAT:
                lv_spangroup_set_span_text_fmt(observer->target, user_data->element, user_data->fmt, subject->value.float_v);
                break;
#endif
            case LV_SUBJECT_TYPE_STRING:
            case LV_SUBJECT_TYPE_POINTER:
                lv_spangroup_set_span_text_fmt(observer->target, user_data->element, user_data->fmt, subject->value.pointer);
                break;
            default:
                return;
        }
    }
}

#endif /*LV_USE_OBSERVER*/

#endif
