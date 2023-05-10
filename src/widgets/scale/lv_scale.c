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

static void scale_draw_horizontal(lv_obj_t *obj, lv_event_t * event);
static void scale_draw_vertical(lv_obj_t *obj, lv_event_t * event);

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

void lv_scale_set_axis_tick(lv_obj_t * obj, lv_coord_t major_len, lv_coord_t minor_len, bool label_en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->major_len = major_len;
    scale->minor_len = minor_len;
    scale->label_enabled = label_en;

    lv_obj_invalidate(obj);
}

void lv_scale_set_range(lv_obj_t * obj, lv_coord_t min, lv_coord_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->range_min = min;
    scale->range_max = max;

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

    scale->total_tick_count = LV_SCALE_TOTAL_TICK_COUNT_DEFAULT;
    scale->major_tick_every = LV_SCALE_MAJOR_TICK_EVERY_DEFAULT;
    scale->mode = LV_SCALE_MODE_VERTICAL;
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
    lv_scale_t * scale = (lv_scale_t *) obj;

    if(event_code == LV_EVENT_DRAW_MAIN) {
        if (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode || LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
            scale_draw_horizontal(obj, event);
        }
        else if (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode || LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            scale_draw_vertical(obj, event);
        }
        else if (LV_SCALE_MODE_ROUND == scale->mode) {
            
        }
        else {
            /* Invalid mode */
        }
    }
    else if(event_code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        /* NOTE: Extend scale draw size so the first tick label can be shown */
        lv_event_set_ext_draw_size(event, 100);
    }
    else {
        /* TODO */
    }
}

static void scale_draw_vertical(lv_obj_t *obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(event);

    if (scale->total_tick_count <= 1) return;

    /* Get style properties so they can be used in the tick and label drawing */
    lv_coord_t height = (lv_coord_t) lv_obj_get_content_height(obj);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t label_gap = 0U;
    lv_coord_t x_ofs = 0U;
    
    if (scale->label_enabled && (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode)) {
        label_gap = lv_obj_get_style_pad_left(obj, LV_PART_TICKS);
        x_ofs = obj->coords.x1;
    }
    else if (scale->label_enabled && (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)) {
        label_gap = lv_obj_get_style_pad_right(obj, LV_PART_TICKS);
        x_ofs = obj->coords.x2;
    }
    else { /* Nothing to do */ }

    /* Configure both line and label draw descriptors for the tick and label drawings */
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);
    line_dsc.width = 2U; /* NOTE: Had to set up manually, otherwise the ticks were not visible */

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_TICKS, &label_dsc);

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.type = LV_SCALE_DRAW_PART_TICK_LABEL;
    part_draw_dsc.id = scale->mode;
    part_draw_dsc.part = LV_PART_TICKS;
    part_draw_dsc.label_dsc = &label_dsc;
    part_draw_dsc.line_dsc = &line_dsc;

    uint16_t total_tick_count = scale->total_tick_count;

    /* Get offset on both axis so the widget can be drawn from there */
    lv_coord_t y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);

    lv_coord_t major_len = scale->major_len;
    lv_coord_t minor_len = scale->minor_len;

    /* Draw tick lines to the right */
    if (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
        major_len *= -1;
        minor_len *= -1;
    }

    uint8_t tick_idx = 0;
    for (tick_idx = 0; tick_idx <= total_tick_count; tick_idx++)
    {
        /* The tick is represented by a vertical line. We need two points to draw it */
        lv_point_t tick_point_a;
        lv_point_t tick_point_b;
        /* A major tick is the one which has a label in it */
        bool is_major_tick = false;
        if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;

        /* Setup the tick points */
        lv_coord_t vertical_position = y_ofs + (int32_t)((int32_t)(height - line_dsc.width) * tick_idx) / total_tick_count;
        lv_coord_t tick_length = is_major_tick ? major_len : minor_len;

        tick_point_a.x = x_ofs - 1U; /* Move extra pixel out of scale boundary */
        tick_point_a.y = vertical_position;
        tick_point_b.x = tick_point_a.x - tick_length;
        tick_point_b.y = vertical_position;

        // LV_LOG_USER("Tick %d at P1 {%d, %d} P2 {%d, %d}", tick_idx, tick_point_a.x, tick_point_a.y, tick_point_b.x, tick_point_b.y);

        /* Draw label when they're enabled and the current tick is a major tick */
        if (scale->label_enabled && is_major_tick)
        {
            char text_buffer[20] = {0};
            int32_t tick_value = lv_map(total_tick_count - tick_idx, 0, total_tick_count, scale->range_min, scale->range_max);
            lv_snprintf(text_buffer, sizeof(text_buffer), "%" LV_PRId32, tick_value);
            part_draw_dsc.text = text_buffer;
            part_draw_dsc.text_length = sizeof(text_buffer);

            /* Reserve appropiate size for the tick label */
            lv_point_t size;
            lv_txt_get_size(&size, part_draw_dsc.text,
                label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

            /* Set the label draw area at some distance of the major tick */
            lv_area_t label_coords;
            label_coords.y1 = (tick_point_b.y - size.y / 2);
            label_coords.y2 = (tick_point_b.y + size.y / 2);

            if (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
                label_coords.x1 = tick_point_b.x - size.x - label_gap;
                label_coords.x2 = tick_point_b.x - label_gap;
            } else { /* LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode */
                label_coords.x1 = tick_point_b.x + label_gap;
                label_coords.x2 = tick_point_b.x + size.x + label_gap;
            }

            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);

            lv_draw_label(draw_ctx, &label_dsc, &label_coords, part_draw_dsc.text, NULL);
        }
        else {
            part_draw_dsc.label_dsc = NULL;
            part_draw_dsc.text = NULL;
            part_draw_dsc.text_length = 0;
            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
        }

        if(tick_point_a.y + line_dsc.width / 2  >= obj->coords.y1 && tick_point_b.y - line_dsc.width / 2  <= obj->coords.y2) {
            lv_draw_line(draw_ctx, &line_dsc, &tick_point_a, &tick_point_b);
        }

        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
    }
}

static void scale_draw_horizontal(lv_obj_t *obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(event);

    if (scale->total_tick_count <= 1) return;

    /* Get style properties so they can be used in the tick and label drawing */
    lv_coord_t height = (lv_coord_t) lv_obj_get_content_height(obj);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t label_gap = 0U;
    lv_coord_t y_ofs = 0U;
    
    if (scale->label_enabled && (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode)) {
        label_gap = lv_obj_get_style_pad_bottom(obj, LV_PART_TICKS);
        y_ofs = obj->coords.y2;
    }
    else if (scale->label_enabled && (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {
        label_gap = lv_obj_get_style_pad_top(obj, LV_PART_TICKS);
        y_ofs = obj->coords.y1;
    }
    else { /* Nothing to do */ }

    /* Configure both line and label draw descriptors for the tick and label drawings */
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);
    line_dsc.width = 2U; /* NOTE: Had to set up manually, otherwise the ticks were not visible */

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_TICKS, &label_dsc);

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.type = LV_SCALE_DRAW_PART_TICK_LABEL;
    part_draw_dsc.id = scale->mode;
    part_draw_dsc.part = LV_PART_TICKS;
    part_draw_dsc.label_dsc = &label_dsc;
    part_draw_dsc.line_dsc = &line_dsc;

    lv_coord_t major_len = scale->major_len;
    lv_coord_t minor_len = scale->minor_len;

    /* Draw tick lines to the top */
    if (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
        major_len *= -1;
        minor_len *= -1;
    }

    uint16_t total_tick_count = scale->total_tick_count;

    /* Get offset on both axis so the widget can be drawn from there */
    lv_coord_t x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);

    uint8_t tick_idx = 0;
    for (tick_idx = 0; tick_idx <= total_tick_count; tick_idx++)
    {
        /* The tick is represented by a vertical line. We need two points to draw it */
        lv_point_t tick_point_a;
        lv_point_t tick_point_b;
        /* A major tick is the one which has a label in it */
        bool is_major_tick = false;
        if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;

        /* Setup the tick points */
        lv_coord_t horizontal_position = x_ofs + (int32_t)((int32_t)(height - line_dsc.width) * tick_idx) / total_tick_count;
        lv_coord_t tick_length = is_major_tick ? major_len : minor_len;

        tick_point_a.x = horizontal_position;
        tick_point_a.y = y_ofs;
        tick_point_b.x = horizontal_position;
        tick_point_b.y = tick_point_a.y + tick_length;

        // LV_LOG_USER("Tick %d at P1 {%d, %d} P2 {%d, %d}", tick_idx, tick_point_a.x, tick_point_a.y, tick_point_b.x, tick_point_b.y);

        /* Draw label when they're enabled and the current tick is a major tick */
        if (scale->label_enabled && is_major_tick)
        {
            char text_buffer[20] = {0};
            int32_t tick_value = lv_map(total_tick_count - tick_idx, 0, total_tick_count, scale->range_max, scale->range_min);
            lv_snprintf(text_buffer, sizeof(text_buffer), "%" LV_PRId32, tick_value);
            part_draw_dsc.text = text_buffer;
            part_draw_dsc.text_length = sizeof(text_buffer);

            /* Reserve appropiate size for the tick label */
            lv_point_t size;
            lv_txt_get_size(&size, part_draw_dsc.text,
                label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

            /* Set the label draw area at some distance of the major tick */
            lv_area_t label_coords;
            label_coords.x1 = (tick_point_b.x - size.x / 2);
            label_coords.x2 = (tick_point_b.x + size.x / 2);
            
            if (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
                label_coords.y1 = tick_point_b.y + label_gap;
                label_coords.y2 = label_coords.y1 + size.y;
            }
            else { /* LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode */
                label_coords.y2 = tick_point_b.y - label_gap;
                label_coords.y1 = label_coords.y2 - size.y;
            }

            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);

            lv_draw_label(draw_ctx, &label_dsc, &label_coords, part_draw_dsc.text, NULL);
        }
        else {
            part_draw_dsc.label_dsc = NULL;
            part_draw_dsc.text = NULL;
            part_draw_dsc.text_length = 0;
            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
        }

        if(tick_point_a.y + line_dsc.width / 2  >= obj->coords.y1 && tick_point_b.y - line_dsc.width / 2  <= obj->coords.y2) {
            lv_draw_line(draw_ctx, &line_dsc, &tick_point_a, &tick_point_b);
        }

        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
    }
}   

#endif