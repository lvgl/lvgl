/**
 * @file lv_arclabel.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_arclabel_private.h"

#if LV_USE_ARCLABEL != 0

#include "../../core/lv_obj_class_private.h"
#include "../../core/lv_obj_event_private.h"
#include "../../core/lv_obj_private.h"
#include "../../misc/lv_area_private.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_text_private.h"

#if LV_USE_FLOAT
    #include <math.h>
    #include <float.h>
    #ifndef  M_PI
        #define M_PI 3.14159265358979323846264338327950288
    #endif
#else
    /* Use fixed point math for integer only platforms $ M_PI << 8 $ */
    #define M_PI 804
#endif

/*********************
 *      DEFINES
 *********************/

#define MY_CLASS (&lv_arclabel_class)

#define LV_ARCLABEL_DEBUG 0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_arclabel_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void arclabel_draw_main(lv_event_t * e);
static void lv_arclabel_event(const lv_obj_class_t * class_p, lv_event_t * e);
static lv_value_precise_t calc_arc_text_total_angle(const char * text, const lv_font_t * font, uint32_t radius,
                                                    const lv_value_precise_t angle_size, int32_t letter_space, bool recolor);
static const char * recolor_cmd_get_next(const char * text_in, uint32_t len_in,
                                         const char ** text_out, uint32_t * len_out,
                                         lv_color_t * color_out);
static lv_value_precise_t deg_to_rad(lv_value_precise_t deg, int32_t radius);
static lv_value_precise_t rad_to_deg(lv_value_precise_t rad, int32_t radius);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_arclabel_class  = {
    .constructor_cb = lv_arclabel_constructor,
    .event_cb = lv_arclabel_event,
    .instance_size = sizeof(lv_arclabel_t),
    .editable = LV_OBJ_CLASS_EDITABLE_FALSE,
    .base_class = &lv_obj_class,
    .name = "arclabel",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_arclabel_create(lv_obj_t * parent)
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

void lv_arclabel_set_text(lv_obj_t * obj, const char * text)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arclabel = (lv_arclabel_t *)obj;

    /*If text is NULL then just refresh with the current text*/
    if(text == NULL) text = arclabel->text;

    const size_t text_len = lv_strlen(text) + 1;

    /*If set its own text then reallocate it (maybe its size changed)*/
    if(arclabel->text == text && arclabel->static_txt == 0) {
        arclabel->text = lv_realloc(arclabel->text, text_len);
        LV_ASSERT_MALLOC(arclabel->text);
        if(arclabel->text == NULL) return;
    }
    else {
        /*Free the old text*/
        if(arclabel->text != NULL && arclabel->static_txt == 0) {
            lv_free(arclabel->text);
            arclabel->text = NULL;
        }

        arclabel->text = lv_malloc(text_len);
        LV_ASSERT_MALLOC(arclabel->text);
        if(arclabel->text == NULL) return;

        lv_strcpy(arclabel->text, text);

        /*Now the text is dynamically allocated*/
        arclabel->static_txt = 0;
    }

    lv_obj_invalidate(obj);
}

void lv_arclabel_set_text_fmt(lv_obj_t * obj, const char * fmt, ...)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(fmt);

    lv_arclabel_t * arclabel = (lv_arclabel_t *)obj;

    /*If text is NULL then refresh*/
    if(fmt == NULL) {
        lv_obj_invalidate(obj);
        return;
    }

    if(arclabel->text != NULL && arclabel->static_txt == 0) {
        lv_free(arclabel->text);
        arclabel->text = NULL;
    }

    va_list args;
    va_start(args, fmt);
    arclabel->text = lv_text_set_text_vfmt(fmt, args);
    va_end(args);
    arclabel->static_txt = 0; /*Now the text is dynamically allocated*/

    lv_obj_invalidate(obj);
}

void lv_arclabel_set_text_static(lv_obj_t * obj, const char * text)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arclabel = (lv_arclabel_t *)obj;

    if(arclabel->static_txt == 0 && arclabel->text != NULL) {
        lv_free(arclabel->text);
        arclabel->text = NULL;
    }

    if(text != NULL) {
        arclabel->static_txt = 1;
        arclabel->text       = (char *)text;
    }

    lv_obj_invalidate(obj);
}

void lv_arclabel_set_angle_start(lv_obj_t * obj, lv_value_precise_t start)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arc = (lv_arclabel_t *)obj;

    arc->angle_start = start;
    lv_obj_invalidate(obj);
}

void lv_arclabel_set_angle_size(lv_obj_t * obj, lv_value_precise_t size)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arc = (lv_arclabel_t *)obj;

    arc->angle_size = size;
    lv_obj_invalidate(obj);
}

void lv_arclabel_set_offset(lv_obj_t * obj, int32_t offset)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arc = (lv_arclabel_t *)obj;

    arc->offset = offset;
    lv_obj_invalidate(obj);
}

void lv_arclabel_set_dir(lv_obj_t * obj, lv_arclabel_dir_t dir)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arc = (lv_arclabel_t *)obj;

    arc->dir = dir;
    lv_obj_invalidate(obj);
}

void lv_arclabel_set_recolor(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arc = (lv_arclabel_t *)obj;
    arc->recolor = en;
    lv_obj_invalidate(obj);
}

void lv_arclabel_set_radius(lv_obj_t * obj, uint32_t radius)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arc = (lv_arclabel_t *)obj;

    arc->radius = radius;
    lv_obj_invalidate(obj);
}

void lv_arclabel_set_center_offset_x(lv_obj_t * obj, uint32_t x)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arc = (lv_arclabel_t *)obj;

    arc->center_offset.x = x;
    lv_obj_invalidate(obj);
}

void lv_arclabel_set_center_offset_y(lv_obj_t * obj, uint32_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arc = (lv_arclabel_t *)obj;

    arc->center_offset.y = y;
    lv_obj_invalidate(obj);
}

void lv_arclabel_set_text_vertical_align(lv_obj_t * obj, lv_arclabel_text_align_t align)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arc = (lv_arclabel_t *)obj;

    arc->text_align_v = align;
    lv_obj_invalidate(obj);
}

void lv_arclabel_set_text_horizontal_align(lv_obj_t * obj, lv_arclabel_text_align_t align)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arc = (lv_arclabel_t *)obj;

    arc->text_align_h = align;
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

lv_value_precise_t lv_arclabel_get_angle_start(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arclabel_t *) obj)->angle_start;
}

lv_value_precise_t lv_arclabel_get_angle_size(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arclabel_t * arclabel = (lv_arclabel_t *)obj;
    return arclabel->angle_size;
}

lv_arclabel_dir_t lv_arclabel_get_dir(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arclabel_t *) obj)->dir;
}

bool lv_arclabel_get_recolor(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arclabel_t *) obj)->recolor;
}

uint32_t lv_arclabel_get_radius(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arclabel_t *) obj)->radius;
}

uint32_t lv_arclabel_get_center_offset_x(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arclabel_t *) obj)->center_offset.x;
}

uint32_t lv_arclabel_get_center_offset_y(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arclabel_t *) obj)->center_offset.y;
}

lv_arclabel_text_align_t lv_arclabel_get_text_vertical_align(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arclabel_t *) obj)->text_align_v;
}

lv_arclabel_text_align_t lv_arclabel_get_text_horizontal_align(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arclabel_t *) obj)->text_align_h;
}

/*=====================
 * Other functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_arclabel_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_arclabel_t * arc = (lv_arclabel_t *)obj;

    arc->angle_start = 0;
    arc->angle_size  = 360;
    arc->dir = LV_ARCLABEL_DIR_CLOCKWISE;
    arc->recolor = false;

    lv_arclabel_set_text_static(obj, LV_ARCLABEL_DEFAULT_TEXT);

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_SCROLLABLE);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_arclabel_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    /*Call the ancestor's event handler*/
    const lv_result_t res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    const lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_DRAW_MAIN) {
        arclabel_draw_main(e);
    }
}

static void arclabel_draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_arclabel_t * arclabel = (lv_arclabel_t *)obj;

    const char * text = arclabel->text;
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
    int32_t arc_r = arclabel->radius;
    lv_value_precise_t angle_start = 0;

    if(arc_r == LV_SIZE_CONTENT) arc_r = LV_PCT(100);
    if(LV_COORD_IS_PCT(arc_r)) {
        const int32_t width = lv_area_get_width(&coords);
        const int32_t height = lv_area_get_height(&coords);
        arc_r = lv_pct_to_px(arc_r, LV_MIN(width, height)) / 2;
    }

    switch(arclabel->text_align_v) {
        case LV_ARCLABEL_TEXT_ALIGN_LEADING:
            arc_r_delta = line_height - base_line;
            break;
        case LV_ARCLABEL_TEXT_ALIGN_CENTER:
            arc_r_delta = line_height / 2 - base_line;
            break;
        case LV_ARCLABEL_TEXT_ALIGN_TRAILING:
            arc_r_delta = -base_line;
            break;
        default:
            break;
    }

    arc_r += arclabel->dir == LV_ARCLABEL_DIR_CLOCKWISE ? -arc_r_delta : arc_r_delta;

    const int32_t offset = arclabel->offset;
    const lv_value_precise_t angle_offset = rad_to_deg(offset, arc_r);

    switch(arclabel->text_align_h) {
        case LV_ARCLABEL_TEXT_ALIGN_LEADING:
            angle_start = angle_offset;
            break;
        case LV_ARCLABEL_TEXT_ALIGN_CENTER:
            angle_start = (arclabel->angle_size + angle_offset - calc_arc_text_total_angle(text_start, font, arc_r,
                                                                                           arclabel->angle_size, letter_space, arclabel->recolor)) / 2;
            break;
        case LV_ARCLABEL_TEXT_ALIGN_TRAILING:
            angle_start = arclabel->angle_size - calc_arc_text_total_angle(text_start, font, arc_r, arclabel->angle_size,
                                                                           letter_space, arclabel->recolor);
            break;
        default:
            break;
    }

    uint32_t processed_word_count = 0;
    lv_value_precise_t prev_letter_w = 0;
    lv_value_precise_t total_arc_length = deg_to_rad(arclabel->angle_size, arc_r);
    lv_value_precise_t curr_total_arc_length = deg_to_rad(angle_start, arc_r);
    uint32_t letter;
    uint32_t letter_next;


    while(text) {
        uint32_t word_i = 0;
        uint32_t text_len = LV_TEXT_LEN_MAX;
        lv_color_t recolor_color = color;
        if(arclabel->recolor) text = recolor_cmd_get_next(text, LV_TEXT_LEN_MAX, &text_start, &text_len, &recolor_color);
        else text = NULL;

        while(word_i < text_len && curr_total_arc_length <= total_arc_length) {
            lv_text_encoded_letter_next_2(text_start, &letter, &letter_next, &word_i);
            const lv_value_precise_t letter_w = lv_font_get_glyph_width(font, letter, letter_next);

            if(processed_word_count > 0) {
                const lv_value_precise_t arc_offset = (prev_letter_w + letter_w + letter_space) / (lv_value_precise_t)2;
                curr_total_arc_length += arc_offset;
                if(curr_total_arc_length > total_arc_length) {
                    break;
                }
            }

            const lv_value_precise_t curr_angle = arclabel->angle_start
                                                  + rad_to_deg(arclabel->dir == LV_ARCLABEL_DIR_CLOCKWISE
                                                               ? curr_total_arc_length
                                                               : total_arc_length - curr_total_arc_length, arc_r);

#if LV_USE_FLOAT
            const lv_value_precise_t x = cosf(deg_to_rad(curr_angle, 1)) * arc_r;
            const lv_value_precise_t y = sinf(deg_to_rad(curr_angle, 1)) * arc_r;
#else
            const lv_value_precise_t x = (lv_value_precise_t)(lv_trigo_cos(curr_angle) * arc_r / 32767);
            const lv_value_precise_t y = (lv_value_precise_t)(lv_trigo_sin(curr_angle) * arc_r / 32767);
#endif

            lv_point_t point = {
                (int32_t)(x + (lv_value_precise_t)(lv_area_get_width(&coords) / 2 + coords.x1  + arclabel->center_offset.x)),
                (int32_t)(y + (lv_value_precise_t)(lv_area_get_height(&coords) / 2 + coords.y1 + arclabel->center_offset.y)),
            };

            lv_draw_letter_dsc_t dsc;
            lv_draw_letter_dsc_init(&dsc);
            dsc.font = font;
            dsc.color = arclabel->recolor ? recolor_color : color;
            dsc.opa = opa;
            if(arclabel->dir == LV_ARCLABEL_DIR_CLOCKWISE) dsc.rotation = (int32_t)((curr_angle + 90) * 10);
            else dsc.rotation = (int32_t)((curr_angle - 90) * 10);

            dsc.unicode = letter;
            if(dsc.unicode == 0) {
                break;
            }

            lv_draw_letter(layer, &dsc, &point);

            prev_letter_w = letter_w;
            processed_word_count++;

#if LV_ARCLABEL_DEBUG
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
    const lv_value_precise_t angle_size_in_arc_length = deg_to_rad(angle_size, radius);
    lv_value_precise_t total_arc_length = 0;
    uint32_t letter;
    uint32_t letter_next;

    while(text) {
        uint32_t word_i = 0;
        uint32_t text_len = LV_TEXT_LEN_MAX;
        if(recolor) text = recolor_cmd_get_next(text, LV_TEXT_LEN_MAX, &text_start, &text_len, NULL);
        else text = NULL;

        while(word_i <= text_len && total_arc_length < angle_size_in_arc_length) {
            if(total_arc_length > angle_size_in_arc_length) {
                break;
            }

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

    return rad_to_deg(total_arc_length, radius);
}

static const char * recolor_cmd_get_next(const char * text_in, uint32_t len_in,
                                         const char ** text_out, uint32_t * len_out,
                                         lv_color_t * color_out)
{
    if(!text_in || len_in == 0 || *text_in == '\0') return NULL;

    const char * text = text_in;
    uint32_t proc_len = 0;
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

        proc_len = text - text_in;
        while(proc_len < len_in && *text && *text++ != ' ') {
            proc_len++;
        }
    }

    const char * text_segment_start = text;
    while(proc_len < len_in && *text && *text != LV_TXT_COLOR_CMD[0]) {
        text++;
        proc_len++;
    };
    if(text_out) *text_out = text_segment_start;
    if(len_out)  *len_out = text - text_segment_start;
    if(*text == '\0')  return NULL;
    if(has_cmd && *text == LV_TXT_COLOR_CMD[0]) {
        text++;
        proc_len++;
    };

    return proc_len < len_in && *text ? text : NULL;
}

static lv_value_precise_t deg_to_rad(lv_value_precise_t deg, int32_t radius)
{
#if LV_USE_FLOAT
    return (lv_value_precise_t)(deg * radius * M_PI / 180);
#else
    return (lv_value_precise_t)((deg * radius * M_PI / 180) >> 8);
#endif
}

static lv_value_precise_t rad_to_deg(lv_value_precise_t rad, int32_t radius)
{
#if LV_USE_FLOAT
    return (lv_value_precise_t)(rad * 180 / M_PI / radius);
#else
    return (lv_value_precise_t)(((rad * 180) << 8) / M_PI / radius);
#endif
}

#endif
