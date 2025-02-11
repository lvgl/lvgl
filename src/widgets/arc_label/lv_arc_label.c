/**
 * @file lv_arc_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_arc_label_private.h"

#if LV_USE_ARC_LABEL != 0

#include "../../core/lv_obj_class_private.h"
#include "../../core/lv_obj_event_private.h"
#include "../../core/lv_obj_private.h"
#include "../../misc/lv_area_private.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_text_private.h"

#if LV_USE_FLOAT
    #include <math.h>
#endif

#ifndef  M_PI
    #define M_PI 3.14159265358979323846
#endif

/*********************
 *      DEFINES
 *********************/

#define MY_CLASS (&lv_arc_label_class)

#define LV_ARC_LABEL_DEBUG 0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_arc_label_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void arc_label_draw_main(lv_event_t * e);
static void lv_arc_label_event(const lv_obj_class_t * class_p, lv_event_t * e);
static lv_value_precise_t calc_arc_text_total_angle(const char * text, const lv_font_t * font, uint32_t radius,
                                                    const lv_value_precise_t angle_size, int32_t letter_space, bool recolor);
static const char * recolor_cmd_get_next(const char * text_in, uint32_t len_in,
                                         const char ** text_out, uint32_t * len_out,
                                         lv_color_t * color_out);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_arc_label_class  = {
    .constructor_cb = lv_arc_label_constructor,
    .event_cb = lv_arc_label_event,
    .instance_size = sizeof(lv_arc_label_t),
    .editable = LV_OBJ_CLASS_EDITABLE_FALSE,
    .base_class = &lv_obj_class,
    .name = "arc_label",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_arc_label_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*======================
 * Add/remove functions
 *=====================*/

/*
 * New object specific "add" or "remove" functions come here
 */

/*=====================
 * Setter functions
 *====================*/

void lv_arc_label_set_text(lv_obj_t * obj, const char * text)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc_label = (lv_arc_label_t *)obj;

    /*If text is NULL then just refresh with the current text*/
    if(text == NULL) text = arc_label->text;

    const size_t text_len = lv_strlen(text) + 1;

    /*If set its own text then reallocate it (maybe its size changed)*/
    if(arc_label->text == text && arc_label->static_txt == 0) {
        arc_label->text = lv_realloc(arc_label->text, text_len);
        LV_ASSERT_MALLOC(arc_label->text);
        if(arc_label->text == NULL) return;
    }
    else {
        /*Free the old text*/
        if(arc_label->text != NULL && arc_label->static_txt == 0) {
            lv_free(arc_label->text);
            arc_label->text = NULL;
        }

        arc_label->text = lv_malloc(text_len);
        LV_ASSERT_MALLOC(arc_label->text);
        if(arc_label->text == NULL) return;

        lv_strcpy(arc_label->text, text);

        /*Now the text is dynamically allocated*/
        arc_label->static_txt = 0;
    }

    lv_obj_invalidate(obj);
}

void lv_arc_label_set_text_fmt(lv_obj_t * obj, const char * fmt, ...)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(fmt);

    lv_obj_invalidate(obj);
    lv_arc_label_t * arc_label = (lv_arc_label_t *)obj;

    /*If text is NULL then refresh*/
    if(fmt == NULL) {
        lv_obj_invalidate(obj);
        return;
    }

    if(arc_label->text != NULL && arc_label->static_txt == 0) {
        lv_free(arc_label->text);
        arc_label->text = NULL;
    }

    va_list args;
    va_start(args, fmt);
    arc_label->text = lv_text_set_text_vfmt(fmt, args);
    va_end(args);
    arc_label->static_txt = 0; /*Now the text is dynamically allocated*/

    lv_obj_invalidate(obj);
}

void lv_arc_label_set_text_static(lv_obj_t * obj, const char * text)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc_label = (lv_arc_label_t *)obj;

    if(arc_label->static_txt == 0 && arc_label->text != NULL) {
        lv_free(arc_label->text);
        arc_label->text = NULL;
    }

    if(text != NULL) {
        arc_label->static_txt = 1;
        arc_label->text       = (char *)text;
    }

    lv_obj_invalidate(obj);
}

void lv_arc_label_set_angle_start(lv_obj_t * obj, lv_value_precise_t start)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc = (lv_arc_label_t *)obj;

    arc->angle_start = start;
    lv_obj_invalidate(obj);
}

void lv_arc_label_set_angle_size(lv_obj_t * obj, lv_value_precise_t size)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc = (lv_arc_label_t *)obj;

    arc->angle_size = size;
    lv_obj_invalidate(obj);
}

void lv_arc_label_set_offset(lv_obj_t * obj, int32_t offset)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc = (lv_arc_label_t *)obj;

    arc->offset = offset;
    lv_obj_invalidate(obj);
}

void lv_arc_label_set_dir(lv_obj_t * obj, lv_arc_label_dir_t dir)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc = (lv_arc_label_t *)obj;

    arc->dir = dir;
    lv_obj_invalidate(obj);
}

void lv_arc_label_set_recolor(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc = (lv_arc_label_t *)obj;
    arc->recolor = en;
    lv_obj_invalidate(obj);
}

void lv_arc_label_set_radius(lv_obj_t * obj, uint32_t radius)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc = (lv_arc_label_t *)obj;

    arc->radius = radius;
    lv_obj_invalidate(obj);
}

void lv_arc_label_set_center_offset_x(lv_obj_t * obj, uint32_t x)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc = (lv_arc_label_t *)obj;

    arc->center_offset.x = x;
    lv_obj_invalidate(obj);
}

void lv_arc_label_set_center_offset_y(lv_obj_t * obj, uint32_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc = (lv_arc_label_t *)obj;

    arc->center_offset.y = y;
    lv_obj_invalidate(obj);
}

void lv_arc_label_set_text_vertical_align(lv_obj_t * obj, lv_arc_label_text_align_t align)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc = (lv_arc_label_t *)obj;

    arc->text_align_v = align;
    lv_obj_invalidate(obj);
}

void lv_arc_label_set_text_horizontal_align(lv_obj_t * obj, lv_arc_label_text_align_t align)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc = (lv_arc_label_t *)obj;

    arc->text_align_h = align;
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

lv_value_precise_t lv_arc_label_get_angle_start(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_label_t *) obj)->angle_start;
}

lv_value_precise_t lv_arc_label_get_angle_size(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_label_t * arc_label = (lv_arc_label_t *)obj;
    return arc_label->angle_size;
}

lv_arc_label_dir_t lv_arc_label_get_dir(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_label_t *) obj)->dir;
}

bool lv_arc_label_get_recolor(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_label_t *) obj)->recolor;
}

uint32_t lv_arc_label_get_radius(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_label_t *) obj)->radius;
}

uint32_t lv_arc_label_get_center_offset_x(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_label_t *) obj)->center_offset.x;
}

uint32_t lv_arc_label_get_center_offset_y(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_label_t *) obj)->center_offset.y;
}

lv_arc_label_text_align_t lv_arc_label_get_text_vertical_align(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_label_t *) obj)->text_align_v;
}

lv_arc_label_text_align_t lv_arc_label_get_text_horizontal_align(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_label_t *) obj)->text_align_h;
}

/*=====================
 * Other functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_arc_label_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_arc_label_t * arc = (lv_arc_label_t *)obj;

    arc->angle_start = 0;
    arc->angle_size  = 360;
    arc->dir = LV_ARC_LABEL_DIR_CLOCKWISE;
    arc->recolor = false;

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_arc_label_set_text(obj, LV_ARC_LABEL_DEFAULT_TEXT);

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_SCROLLABLE);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_arc_label_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    /*Call the ancestor's event handler*/
    const lv_result_t res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    const lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_DRAW_MAIN) {
        arc_label_draw_main(e);
    }
}

static void arc_label_draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_arc_label_t * arc_label = (lv_arc_label_t *)obj;

    const char * text = arc_label->text;
    const char * text_start = text;

    lv_area_t coords;
    lv_obj_get_content_coords(obj, &coords);

    lv_layer_t * layer = lv_event_get_layer(e);

    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    const lv_color_t color = lv_obj_get_style_text_color(obj, LV_PART_MAIN);
    const lv_opa_t opa = lv_obj_get_style_text_opa(obj, LV_PART_MAIN);
    const int32_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);

    const int32_t line_height = font->line_height;
    const int32_t base_line = font->base_line;
    int32_t arc_r_delta = 0;
    int32_t arc_r = arc_label->radius;
    lv_value_precise_t angle_start = 0;

    switch(arc_label->text_align_v) {
        case LV_ARC_LABEL_TEXT_ALIGN_LEADING:
            arc_r_delta = line_height - base_line;
            break;
        case LV_ARC_LABEL_TEXT_ALIGN_CENTER:
            arc_r_delta = line_height / 2 - base_line;
            break;
        case LV_ARC_LABEL_TEXT_ALIGN_TRAILING:
            arc_r_delta = -base_line;
            break;
        default:
            break;
    }

    arc_r += arc_label->dir == LV_ARC_LABEL_DIR_CLOCKWISE ? -arc_r_delta : arc_r_delta;

    const int32_t offset = arc_label->offset;
    const int32_t angle_offset = offset / M_PI * 180 / arc_r;

    switch(arc_label->text_align_h) {
        case LV_ARC_LABEL_TEXT_ALIGN_LEADING:
            angle_start = angle_offset;
            break;
        case LV_ARC_LABEL_TEXT_ALIGN_CENTER:
            angle_start = (arc_label->angle_size + angle_offset - calc_arc_text_total_angle(text_start, font, arc_r,
                                                                                            arc_label->angle_size, letter_space, arc_label->recolor)) / 2;
            break;
        case LV_ARC_LABEL_TEXT_ALIGN_TRAILING:
            angle_start = arc_label->angle_size - calc_arc_text_total_angle(text_start, font, arc_r, arc_label->angle_size,
                                                                            letter_space, arc_label->recolor);
            break;
        default:
            break;
    }

    uint32_t processed_word_count = 0;
    lv_value_precise_t prev_letter_w = 0;
    lv_value_precise_t total_arc_length = arc_label->angle_size * M_PI / 180 * arc_r;
    lv_value_precise_t curr_total_arc_length = angle_start * M_PI / 180 * arc_r;

    while(text) {
        uint32_t word_i = 0;
        uint32_t text_len = LV_TEXT_LEN_MAX;
        lv_color_t recolor_color = color;
        if(arc_label->recolor) text = recolor_cmd_get_next(text, LV_TEXT_LEN_MAX, &text_start, &text_len, &recolor_color);
        else text = NULL;

        while(word_i < text_len && curr_total_arc_length <= total_arc_length) {
            uint32_t letter;
            uint32_t letter_next;
            lv_text_encoded_letter_next_2(text_start, &letter, &letter_next, &word_i);
            const lv_value_precise_t letter_w = lv_font_get_glyph_width(font, letter, letter_next);

            if(processed_word_count > 0) {
                const lv_value_precise_t arc_offset = (prev_letter_w + letter_w + letter_space) / (lv_value_precise_t)2;
                curr_total_arc_length += arc_offset;
                if(curr_total_arc_length > total_arc_length) {
                    break;
                }
            }

            const lv_value_precise_t curr_angle = arc_label->angle_start + (arc_label->dir == LV_ARC_LABEL_DIR_CLOCKWISE ?
                                                                            curr_total_arc_length : total_arc_length - curr_total_arc_length) * 180 / M_PI / arc_r;

#if LV_USE_FLOAT
            const lv_value_precise_t x = cos(curr_angle * M_PI / 180) * arc_r;
            const lv_value_precise_t y = sin(curr_angle * M_PI / 180) * arc_r;
#else
            const lv_value_precise_t x = lv_trigo_cos(curr_angle) * arc_r / 32767;
            const lv_value_precise_t y = lv_trigo_sin(curr_angle) * arc_r / 32767;
#endif

            lv_point_t point = {
                x + lv_area_get_width(&coords) / 2 + coords.x1 + arc_label->center_offset.x,
                y + lv_area_get_height(&coords) / 2 + coords.y1 + arc_label->center_offset.y,
            };

            lv_draw_letter_dsc_t dsc;
            lv_draw_letter_dsc_init(&dsc);
            dsc.font = font;
            dsc.color = arc_label->recolor ? recolor_color : color;
            dsc.opa = opa;
            if(arc_label->dir == LV_ARC_LABEL_DIR_CLOCKWISE) dsc.rotation = (curr_angle + 90) * 10;
            else dsc.rotation = (curr_angle - 90) * 10;

            dsc.unicode = letter;
            if(dsc.unicode == 0) {
                break;
            }

            lv_draw_letter(layer, &dsc, &point);

            prev_letter_w = letter_w;
            processed_word_count++;

#if LV_ARC_LABEL_DEBUG
            lv_draw_line_dsc_t line_dsc;
            lv_draw_line_dsc_init(&line_dsc);
            line_dsc.color = lv_color_make(0x11, 0x45, 0x14);
            line_dsc.opa = LV_OPA_30;
            line_dsc.width = 2;
            line_dsc.p1 = (lv_point_precise_t) {
                .x = point.x,
                .y = point.y
            };
            line_dsc.p2 = (lv_point_precise_t) {
                .x = lv_area_get_width(&coords) / 2 + coords.x1,
                .y = lv_area_get_height(&coords) / 2 + coords.y1
            };

            lv_draw_line(layer, &line_dsc);
#endif
        }
    }
}

static lv_value_precise_t calc_arc_text_total_angle(const char * text, const lv_font_t * font, const uint32_t radius,
                                                    const lv_value_precise_t angle_size, const int32_t letter_space, const bool recolor)
{
    const char * text_start = text;
    uint32_t processed_letter_count = 0;
    lv_value_precise_t prev_letter_w = 0;
    const lv_value_precise_t angle_size_in_arc_length = angle_size * M_PI / 180 * radius;
    lv_value_precise_t total_arc_length = 0;

    while(text) {
        uint32_t word_i = 0;
        uint32_t text_len = LV_TEXT_LEN_MAX;
        if(recolor) text = recolor_cmd_get_next(text, LV_TEXT_LEN_MAX, &text_start, &text_len, NULL);
        else text = NULL;

        while(word_i <= text_len && total_arc_length < angle_size_in_arc_length) {
            if(total_arc_length > angle_size_in_arc_length) {
                break;
            }

            uint32_t letter;
            uint32_t letter_next;
            lv_text_encoded_letter_next_2(text_start, &letter, &letter_next, &word_i);
            const lv_value_precise_t letter_w = lv_font_get_glyph_width(font, letter, letter_next);

            if(processed_letter_count == 0) {
                processed_letter_count++;
                continue;
            }
            const lv_value_precise_t arc_offset = (prev_letter_w + letter_w + letter_space) / (lv_value_precise_t)2;

            total_arc_length += arc_offset;

            if(letter == 0) {
                break;
            }

            prev_letter_w = letter_w;
            processed_letter_count++;
        }
    }

    return total_arc_length * 180 / M_PI / radius;
}

static const char * recolor_cmd_get_next(const char * text_in, uint32_t len_in,
                                         const char ** text_out, uint32_t * len_out,
                                         lv_color_t * color_out)
{
    if(!text_in || len_in == 0 || *text_in == '\0') return NULL;

    const char * text = text_in;
    const char * text_end = text_in + len_in;
    bool has_cmd = false;

    if(*text == LV_TXT_COLOR_CMD[0]) {
        if(len_in < 8) {
            if(text_out) *text_out = text_in;
            if(len_out) *len_out = len_in;
            return NULL;
        }
        has_cmd = true;
        text++;

        int32_t index = 0;
        uint8_t color_buf[6];
        while(*text && index < 6) {
            uint8_t ch = text[index];
            if(!((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))) break;
            if(ch >= 'a' && ch <= 'f')      ch -= 'a' - 10;
            else if(ch >= 'A' && ch <= 'F') ch -= 'A' - 10;
            else                            ch -= '0';
            color_buf[index] = ch;
            index++;
        }

        const bool has_valid_param = index == 6 && text[index] == ' ';

        text += index;
        if(has_valid_param && color_out)
            *color_out = lv_color_make(color_buf[0] << 4 | color_buf[1],
                                       color_buf[2] << 4 | color_buf[3],
                                       color_buf[4] << 4 | color_buf[5]);

        while(text < text_end && *text && *text++ != ' ') { }
    }

    const char * text_segment_start = text;
    while(text < text_end && *text && *text != LV_TXT_COLOR_CMD[0]) text++;
    if(text_out) *text_out = text_segment_start;
    if(len_out)  *len_out = text - text_segment_start;
    if(*text == '\0')  return NULL;
    if(has_cmd && *text == LV_TXT_COLOR_CMD[0]) text++;

    return text < text_end && *text ? text : NULL;
}

#endif
