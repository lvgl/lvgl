/**
 * @file lv_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_scale.h"
#if LV_USE_SCALE != 0

#include "../../core/lv_group.h"
#include "../../core/lv_indev.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_math.h"
#include "../../draw/lv_draw_arc.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_scale_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_scale_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_scale_event(const lv_obj_class_t * class_p, lv_event_t * event);

static void draw_horizontal_scale(lv_obj_t *obj, lv_event_t * event);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_scale_class  = {
    .constructor_cb = lv_scale_constructor,
    .event_cb = lv_scale_event,
    .instance_size = sizeof(lv_scale_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_scale_create(lv_obj_t * parent)
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

void lv_scale_mode(lv_obj_t * obj, lv_scale_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->mode = mode;

    lv_obj_invalidate(obj);
}

void lv_scale_set_total_tick_count(lv_obj_t * obj, lv_coord_t total_tick_count)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->total_tick_count = total_tick_count;

    lv_obj_invalidate(obj);
}

void lv_scale_set_major_tick_every(lv_obj_t * obj, lv_coord_t major_tick_every)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->major_tick_every = major_tick_every;

    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_scale_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_scale_t * scale = (lv_scale_t *)obj;

    // scale->total_tick_count = LV_SCALE_TOTAL_TICK_COUNT_DEFAULT;
    scale->total_tick_count = 4;
    // scale->major_tick_every = LV_SCALE_MAJOR_TICK_EVERY_DEFAULT;
    scale->major_tick_every = 2;
    scale->mode = LV_SCALE_MODE_HORIZONTAL;
    scale->label_enabled = LV_SCALE_LABEL_ENABLED_DEFAULT;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_scale_event(const lv_obj_class_t * class_p, lv_event_t * event)
{
    LV_UNUSED(class_p);

    /*Call the ancestor's event handler*/
    lv_res_t res = lv_obj_event_base(MY_CLASS, event);
    if(res != LV_RES_OK) return;

    lv_event_code_t event_code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);
    lv_scale_t * scale = (lv_scale_t *)lv_event_get_target(event);

    if(event_code == LV_EVENT_DRAW_MAIN) {
        if (LV_SCALE_MODE_HORIZONTAL == scale->mode) {
            draw_horizontal_scale(obj, event);
        }
        else if (LV_SCALE_MODE_VERTICAL == scale->mode) {

        }
        else if (LV_SCALE_MODE_ROUND == scale->mode) {
            
        }
        else {
            /* Invalid mode */
        }
    }
    else {
        /* TODO */
    }
}

static void draw_horizontal_scale(lv_obj_t *obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(event);

    lv_point_t point1;
    lv_point_t point2;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_TICKS, &label_dsc);

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_TICKS, &line_dsc);

    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t w = (int32_t)lv_obj_get_content_width(obj);

    lv_coord_t x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
    lv_coord_t y_ofs = obj->coords.y2;
    lv_coord_t label_gap;
    label_gap = scale->label_enabled ? lv_obj_get_style_pad_bottom(obj, LV_PART_TICKS) : 0;

    /* Line descriptor customization */
    line_dsc.dash_gap = 0;
    line_dsc.dash_width = 0;

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.type = LV_SCALE_DRAW_PART_TICK_LABEL;
    part_draw_dsc.id = LV_SCALE_MODE_HORIZONTAL;
    part_draw_dsc.part = LV_PART_TICKS;
    part_draw_dsc.label_dsc = &label_dsc;
    part_draw_dsc.line_dsc = &line_dsc;

    point1.y = y_ofs;
    uint8_t tick_idx = 0;
    for (tick_idx = 0; tick_idx < scale->total_tick_count; tick_idx++)
    {
        /*draw a line at moving x position*/
        point2.x = point1.x = x_ofs + (int32_t)((int32_t)(w - line_dsc.width) * tick_idx) / 5;
        point2.y = point1.y - 10; /* TODO */

        LV_LOG_USER("Drawing tick %d at X: %d", tick_idx, point1.x);

        part_draw_dsc.p1 = &point1;
        part_draw_dsc.p2 = &point2;

        part_draw_dsc.label_dsc = &label_dsc;
        char text_buffer[20];
        lv_snprintf(text_buffer, sizeof(text_buffer), "Hi %" LV_PRId32, tick_idx);
        part_draw_dsc.label_dsc = &label_dsc;
        part_draw_dsc.text = text_buffer;
        part_draw_dsc.text_length = 10;

        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);

        /*reserve appropriate area*/
        lv_point_t size;
        lv_txt_get_size(&size, part_draw_dsc.text, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX,
                        LV_TEXT_FLAG_NONE);

        lv_area_t a;
        a.x1 = (point2.x - size.x / 2);
        a.x2 = (point2.x + size.x / 2);
        a.y1 = point2.y + label_gap;
        a.y2 = a.y1 + size.y;

        LV_LOG_USER("Drawing line P1 {%d,%d} P1 {%d,%d}", point1.x, point1.y, point2.x, point2.y);

        lv_draw_label(draw_ctx, &label_dsc, &a, part_draw_dsc.text, NULL);
        lv_draw_line(draw_ctx, &line_dsc, &point1, &point2);

        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
    }
}   

#endif