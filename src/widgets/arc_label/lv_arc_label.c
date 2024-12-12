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

/*********************
 *      DEFINES
 *********************/

#define MY_CLASS (&lv_arc_label_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_arc_label_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void arc_label_draw_main(lv_event_t * e);
static void lv_arc_label_event(const lv_obj_class_t * class_p, lv_event_t * e);

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
    lv_obj_t * obj = lv_event_get_current_target(e);

    if((code == LV_EVENT_STYLE_CHANGED) || (code == LV_EVENT_SIZE_CHANGED)) {
        // lv_label_refr_text(obj);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        arc_label_draw_main(e);
    }
}

static void arc_label_draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_arc_label_t * arc_label = (lv_arc_label_t *)obj;

    lv_area_t coords;
    lv_obj_get_content_coords(obj, &coords);

    lv_layer_t * layer = lv_event_get_layer(e);

    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    const lv_color_t color = lv_obj_get_style_text_color(obj, LV_PART_MAIN);
    const lv_opa_t opa = lv_obj_get_style_text_opa(obj, LV_PART_MAIN);

    int32_t arc_r = arc_label->radius;

    uint32_t word_i = 0;
    uint32_t processed_word_count = 0;
    lv_value_precise_t prev_letter_w = 0;
    for(lv_value_precise_t angle_start = 0; angle_start < arc_label->angle_size;) {
        uint32_t letter;
        uint32_t letter_next;
        lv_text_encoded_letter_next_2(arc_label->text, &letter, &letter_next, &word_i);
        const lv_value_precise_t letter_w = lv_font_get_glyph_width(font, letter, letter_next);

        if(processed_word_count > 0) {
            const lv_value_precise_t angle_offset = (prev_letter_w + letter_w) * 180 / 3.141592653589f / arc_r / 2;
            angle_start += angle_offset;
            if(angle_start > arc_label->angle_size - letter_w / 2) {
                break;
            }
        }

        lv_value_precise_t curr_angle = arc_label->angle_start + (arc_label->dir == LV_ARC_LABEL_DIR_CLOCKWISE ? angle_start :
                                                                  -angle_start);

        const lv_value_precise_t x = lv_trigo_cos(curr_angle) * arc_r / (lv_value_precise_t)32767;
        const lv_value_precise_t y = lv_trigo_sin(curr_angle) * arc_r / (lv_value_precise_t)32767;

        lv_point_t point = {
            x + lv_area_get_width(&coords) / 2 + coords.x1 + arc_label->center_offset.x,
            y + lv_area_get_height(&coords) / 2 + coords.y1 + arc_label->center_offset.y,
        };

        lv_draw_letter_dsc_t dsc;
        lv_draw_letter_dsc_init(&dsc);
        dsc.font = font;
        dsc.color = color;
        dsc.opa = opa;
        dsc.rotation = (curr_angle + 90) * 10;

        dsc.unicode = letter;
        if(dsc.unicode == 0) {
            break;
        }

        lv_draw_letter(layer, &dsc, &point);

        prev_letter_w = letter_w;
        processed_word_count++;

#if DEBUG
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

#endif
