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
static void lv_scale_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_scale_event(const lv_obj_class_t * class_p, lv_event_t * event);

static void scale_draw_main(lv_obj_t *obj, lv_event_t * event);
static void scale_draw_items(lv_obj_t *obj, lv_event_t * event);
static void scale_draw_indicator(lv_obj_t *obj, lv_event_t * event);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_scale_class  = {
    .constructor_cb = lv_scale_constructor,
    .destructor_cb = lv_scale_destructor,
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

void lv_scale_set_major_tick_length(lv_obj_t * obj, lv_coord_t major_len)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->major_len = major_len;

    lv_obj_invalidate(obj);
}

void lv_scale_set_minor_tick_length(lv_obj_t * obj, lv_coord_t minor_len)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->minor_len = minor_len;

    lv_obj_invalidate(obj);
}

void lv_scale_set_label_show(lv_obj_t * obj, bool show_label)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->label_enabled = show_label;

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

void lv_scale_set_text_src(lv_obj_t * obj, char * txt_src)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->txt_src = txt_src;

    lv_obj_invalidate(obj);
}

lv_scale_section_t * lv_scale_add_section(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_scale_section_t * section = _lv_ll_ins_head(&scale->section_ll);
    LV_ASSERT_MALLOC(section);
    if(section == NULL) return NULL;

    /* Section default values */
    section->main_style = NULL;
    section->indicator_style = NULL;
    section->items_style = NULL;
    section->minor_range = 0U;
    section->major_range = 0U;

    return section;
}

void lv_scale_section_set_range(lv_scale_section_t * section, lv_coord_t minor_range, lv_coord_t major_range)
{
    if (NULL == section) return;

    section->minor_range = minor_range;
    section->major_range = major_range;
}

void lv_scale_section_set_style(lv_scale_section_t * section, uint32_t part, lv_style_t * section_part_style)
{
    if (NULL == section) return;

    switch (part) {
        case LV_PART_MAIN:
            section->main_style = section_part_style;
            break;
        case LV_PART_INDICATOR:
            section->indicator_style = section_part_style;
            break;
        case LV_PART_ITEMS:
            section->items_style = section_part_style;
            break;
        default:
            /* Invalid part */
            break;
    }
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

    _lv_ll_init(&scale->section_ll, sizeof(lv_scale_section_t));

    scale->total_tick_count = LV_SCALE_TOTAL_TICK_COUNT_DEFAULT;
    scale->major_tick_every = LV_SCALE_MAJOR_TICK_EVERY_DEFAULT;
    scale->mode = LV_SCALE_MODE_VERTICAL_LEFT;
    scale->label_enabled = LV_SCALE_LABEL_ENABLED_DEFAULT;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_scale_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_scale_section_t * section;
    while(scale->section_ll.head) {
        section = _lv_ll_get_head(&scale->section_ll);
        _lv_ll_remove(&scale->section_ll, section);
        lv_free(section);
    }
    _lv_ll_clear(&scale->section_ll);

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
        if (((LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode))
            || ((LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) || (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode))) {
            scale_draw_main(obj, event);
            scale_draw_indicator(obj, event);
            scale_draw_items(obj, event);
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

static void scale_draw_items(lv_obj_t *obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(event);

    if (scale->total_tick_count <= 1) return;

    /* Configure line draw descriptor for the minor tick drawing */
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &line_dsc);

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.id = scale->mode;
    part_draw_dsc.part = LV_PART_ITEMS;
    part_draw_dsc.line_dsc = &line_dsc;

    /* Get style properties so they can be used in the tick and label drawing */
    lv_coord_t height = (lv_coord_t) lv_obj_get_content_height(obj);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t x_ofs = 0U;
    lv_coord_t y_ofs = 0U;

    if (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
        x_ofs = obj->coords.x1;
        y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    }
    else if (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
        x_ofs = obj->coords.x2;
        y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    }
    else if (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y2;
    }
    else if (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y1;
    }
    else { /* Nothing to do */ }

    uint16_t total_tick_count = scale->total_tick_count;

    lv_coord_t major_len = scale->major_len;
    lv_coord_t minor_len = lv_obj_get_style_width(obj, LV_PART_ITEMS);

    /* Draw tick lines to the right */
    if (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
        major_len *= -1;
        minor_len *= -1;
    }
    /* Draw tick lines to the top */
    else if (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
        major_len *= -1;
        minor_len *= -1;
    }
    else { /* Nothing to do */ }

    uint8_t tick_idx = 0;
    for (tick_idx = 0; tick_idx <= total_tick_count; tick_idx++)
    {
        /* The tick is represented by a vertical line. We need two points to draw it */
        lv_point_t tick_point_a;
        lv_point_t tick_point_b;
        /* A major tick is the one which has a label in it */
        bool is_major_tick = false;
        if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;
        lv_coord_t tick_length = is_major_tick ? major_len : minor_len;

        if (is_major_tick) continue;

        /* Setup the tick points */
        if (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            lv_coord_t vertical_position = y_ofs + (int32_t)((int32_t)(height - line_dsc.width) * (total_tick_count - tick_idx)) / total_tick_count;

            tick_point_a.x = x_ofs - 1U; /* Move extra pixel out of scale boundary */
            tick_point_a.y = vertical_position;
            tick_point_b.x = tick_point_a.x - tick_length;
            tick_point_b.y = vertical_position;
        } else {
            lv_coord_t horizontal_position = x_ofs + (int32_t)((int32_t)(height - line_dsc.width) * tick_idx) / total_tick_count;

            tick_point_a.x = horizontal_position;
            tick_point_a.y = y_ofs;
            tick_point_b.x = horizontal_position;
            tick_point_b.y = tick_point_a.y + tick_length;
        }

        int32_t tick_value = 0U;
        int32_t min_out = 0U;
        int32_t max_out = 0U;

        if ((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
            || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {
            min_out = scale->range_min;
            max_out = scale->range_max;
        }
        else {
            /* Circular mode */
        }

        tick_value = lv_map(tick_idx, 0U, total_tick_count, min_out, max_out);

        /* Overwrite label properties if tick value is within section range */
        lv_scale_section_t * section;
        _LV_LL_READ_BACK(&scale->section_ll, section) {
            if(section->minor_range <= tick_value && section->major_range >= tick_value) {

                if (section->indicator_style) {
                    lv_style_value_t value;
                    lv_res_t res;

                    /* Tick width */
                    res = lv_style_get_prop(section->items_style, LV_STYLE_LINE_WIDTH, &value);
                    if(res == LV_RES_OK) {
                        line_dsc.width = (lv_coord_t)value.num;
                    }
                    else {
                        line_dsc.width = lv_obj_get_style_line_width(obj, LV_PART_ITEMS);
                    }

                    /* Tick color */
                    res = lv_style_get_prop(section->items_style, LV_STYLE_LINE_COLOR, &value);
                    if(res == LV_RES_OK) {
                        line_dsc.color = value.color;
                    }
                    else {
                        line_dsc.color = lv_obj_get_style_line_color(obj, LV_PART_ITEMS);
                    }

                    /* Tick opa */
                    res = lv_style_get_prop(section->items_style, LV_STYLE_LINE_OPA, &value);
                    if(res == LV_RES_OK) {
                        line_dsc.opa = (lv_opa_t)value.num;
                    }
                    else {
                        line_dsc.opa = lv_obj_get_style_line_opa(obj, LV_PART_ITEMS);
                    }

                    /* Tick gap */

                }
            }
            else {
                line_dsc.color = lv_obj_get_style_line_color(obj, LV_PART_ITEMS);
                line_dsc.opa = lv_obj_get_style_line_opa(obj, LV_PART_ITEMS);
                line_dsc.width = lv_obj_get_style_line_width(obj, LV_PART_ITEMS);
            }
        }

        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
        lv_draw_line(draw_ctx, &line_dsc, &tick_point_a, &tick_point_b);
        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
    }
}

static void scale_draw_indicator(lv_obj_t *obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(event);

    if (!scale->label_enabled) return;

    /* Get style properties so they can be used in the tick and label drawing */
    lv_coord_t height = (lv_coord_t) lv_obj_get_content_height(obj);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t label_gap = 0U;
    /* Get offset on both axis so the widget can be drawn from there */
    lv_coord_t x_ofs = 0U;
    lv_coord_t y_ofs = 0U;

    if (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
        label_gap = lv_obj_get_style_pad_bottom(obj, LV_PART_INDICATOR);
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y2;
    }
    else if (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
        label_gap = lv_obj_get_style_pad_top(obj, LV_PART_INDICATOR);
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y1;
    }
    else if (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
        label_gap = lv_obj_get_style_pad_left(obj, LV_PART_TICKS);
        x_ofs = obj->coords.x1;
        y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    }
    else if (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
        label_gap = lv_obj_get_style_pad_right(obj, LV_PART_TICKS);
        x_ofs = obj->coords.x2;
        y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    }
    else {
        /* Mode not handled */
        return;
    }

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    /* Formatting the labels with the configured style for LV_PART_INDICATOR */
    lv_obj_init_draw_label_dsc(obj, LV_PART_INDICATOR, &label_dsc);

    /* Major tick style */
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &line_dsc);

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.id = scale->mode;
    part_draw_dsc.part = LV_PART_INDICATOR;
    part_draw_dsc.label_dsc = &label_dsc;
    part_draw_dsc.line_dsc = &line_dsc;

    lv_coord_t major_len = scale->major_len;

    /* Handle tick length being drawn backwards */
    if (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
        major_len *= -1;
    }
    else if (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
        major_len *= -1;
    }
    else { /* Nothing to do */ }

    uint16_t total_tick_count = scale->total_tick_count;
    uint8_t tick_idx = 0;
    uint16_t major_tick_idx = 0;
    for (tick_idx = 0; tick_idx <= total_tick_count; tick_idx++)
    {
        /* The tick is represented by a vertical line. We need two points to draw it */
        lv_point_t tick_point_a;
        lv_point_t tick_point_b;
        /* A major tick is the one which has a label in it */
        bool is_major_tick = false;
        if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;
        if(false == is_major_tick) continue;

        lv_coord_t tick_length = major_len;

        /* Setup the tick points */
        if (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            lv_coord_t vertical_position = y_ofs + (int32_t)((int32_t)(height - line_dsc.width) * (total_tick_count - tick_idx)) / total_tick_count;

            tick_point_a.x = x_ofs - 1U; /* Move extra pixel out of scale boundary */
            tick_point_a.y = vertical_position;
            tick_point_b.x = tick_point_a.x - tick_length;
            tick_point_b.y = vertical_position;
        } else if (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
            lv_coord_t horizontal_position = x_ofs + (int32_t)((int32_t)(height - line_dsc.width) * tick_idx) / total_tick_count;

            tick_point_a.x = horizontal_position;
            tick_point_a.y = y_ofs;
            tick_point_b.x = horizontal_position;
            tick_point_b.y = tick_point_a.y + tick_length;
        }
        else {
            /* Circular mode */
        }

        /* Label text setup */
        char text_buffer[20] = {0};

        /* Check if the custom text array has element for this major tick index */
        if (scale->txt_src)
        {
            if (scale->txt_src[major_tick_idx]) {
                part_draw_dsc.text = scale->txt_src[major_tick_idx];
                part_draw_dsc.text_length = strlen(scale->txt_src[major_tick_idx]);
            }
            else {
                part_draw_dsc.text = NULL;
                part_draw_dsc.text_length = 0;
            }
        }
        else /* Add label with mapped values */
        {
            int32_t tick_value = 0U;
            int32_t min_out = 0U;
            int32_t max_out = 0U;

            if ((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
                || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {
                min_out = scale->range_min;
                max_out = scale->range_max;
            }
            else {
                /* Circular mode */
            }

            tick_value = lv_map(tick_idx, 0U, total_tick_count, min_out, max_out);

            /* Overwrite label properties if tick value is within section range */
            lv_scale_section_t * section;
            _LV_LL_READ_BACK(&scale->section_ll, section) {
                if(section->minor_range <= tick_value && section->major_range >= tick_value) {

                    if (section->indicator_style) {
                        lv_style_value_t value;
                        lv_res_t res;

                        /* Text color */
                        res = lv_style_get_prop(section->indicator_style, LV_STYLE_TEXT_COLOR, &value);
                        if(res == LV_RES_OK) {
                            label_dsc.color = value.color;
                        }
                        else {
                            label_dsc.color = lv_obj_get_style_text_color(obj, LV_PART_INDICATOR);
                        }

                        /* Text opa */
                        res = lv_style_get_prop(section->indicator_style, LV_STYLE_TEXT_OPA, &value);
                        if(res == LV_RES_OK) {
                            label_dsc.opa = (lv_opa_t)value.num;
                        }
                        else {
                            label_dsc.opa = lv_obj_get_style_text_opa(obj, LV_PART_INDICATOR);
                        }

                        /* Text letter space */
                        res = lv_style_get_prop(section->indicator_style, LV_STYLE_TEXT_LETTER_SPACE, &value);
                        if(res == LV_RES_OK) {
                            label_dsc.letter_space = (lv_coord_t)value.num;
                        }
                        else {
                            label_dsc.letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_INDICATOR);
                        }

                        /* Text font */
                        res = lv_style_get_prop(section->indicator_style, LV_STYLE_TEXT_FONT, &value);
                        if(res == LV_RES_OK) {
                            label_dsc.font = (const lv_font_t *)value.ptr;
                        }
                        else {
                            label_dsc.font = lv_obj_get_style_text_font(obj, LV_PART_INDICATOR);
                        }

                        /* Tick width */
                        res = lv_style_get_prop(section->indicator_style, LV_STYLE_LINE_WIDTH, &value);
                        if(res == LV_RES_OK) {
                            line_dsc.width = (lv_coord_t)value.num;
                        }
                        else {
                            line_dsc.width = lv_obj_get_style_line_width(obj, LV_PART_INDICATOR);
                        }

                        /* Tick color */
                        res = lv_style_get_prop(section->indicator_style, LV_STYLE_LINE_COLOR, &value);
                        if(res == LV_RES_OK) {
                            line_dsc.color = value.color;
                        }
                        else {
                            line_dsc.color = lv_obj_get_style_line_color(obj, LV_PART_INDICATOR);
                        }

                        /* Tick opa */
                        res = lv_style_get_prop(section->indicator_style, LV_STYLE_LINE_OPA, &value);
                        if(res == LV_RES_OK) {
                            line_dsc.opa = (lv_opa_t)value.num;
                        }
                        else {
                            line_dsc.opa = lv_obj_get_style_line_opa(obj, LV_PART_INDICATOR);
                        }

                        /* Tick gap */

                    }
                }
                else {
                    /* If label is not within a range then get the indicator style */
                    label_dsc.color = lv_obj_get_style_text_color(obj, LV_PART_INDICATOR);
                    label_dsc.opa = lv_obj_get_style_text_opa(obj, LV_PART_INDICATOR);
                    label_dsc.letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_INDICATOR);
                    label_dsc.font = lv_obj_get_style_text_font(obj, LV_PART_INDICATOR);

                    line_dsc.color = lv_obj_get_style_line_color(obj, LV_PART_INDICATOR);
                    line_dsc.opa = lv_obj_get_style_line_opa(obj, LV_PART_INDICATOR);
                    line_dsc.width = lv_obj_get_style_line_width(obj, LV_PART_INDICATOR);
                }
            }

            lv_snprintf(text_buffer, sizeof(text_buffer), "%" LV_PRId32, tick_value);
            part_draw_dsc.text = text_buffer;
            part_draw_dsc.text_length = sizeof(text_buffer);
        }

        /* Reserve appropiate size for the tick label */
        lv_point_t size;
        lv_txt_get_size(&size, part_draw_dsc.text,
            label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

        /* Set the label draw area at some distance of the major tick */
        lv_area_t label_coords;

        if (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
            label_coords.x1 = (tick_point_b.x - size.x / 2);
            label_coords.x2 = (tick_point_b.x + size.x / 2);
            label_coords.y1 = tick_point_b.y + label_gap;
            label_coords.y2 = label_coords.y1 + size.y;
        }
        else if (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
            label_coords.x1 = (tick_point_b.x - size.x / 2);
            label_coords.x2 = (tick_point_b.x + size.x / 2);
            label_coords.y2 = tick_point_b.y - label_gap;
            label_coords.y1 = label_coords.y2 - size.y;
        }
        else if (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            label_coords.x1 = tick_point_b.x - size.x - label_gap;
            label_coords.x2 = tick_point_b.x - label_gap;
            label_coords.y1 = (tick_point_b.y - size.y / 2);
            label_coords.y2 = (tick_point_b.y + size.y / 2);
        } else if (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            label_coords.x1 = tick_point_b.x + label_gap;
            label_coords.x2 = tick_point_b.x + size.x + label_gap;
            label_coords.y1 = (tick_point_b.y - size.y / 2);
            label_coords.y2 = (tick_point_b.y + size.y / 2);
        }
        else { /* Nothing to do */ }

        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
        lv_draw_line(draw_ctx, &line_dsc, &tick_point_a, &tick_point_b);
        lv_draw_label(draw_ctx, &label_dsc, &label_coords, part_draw_dsc.text, NULL);
        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);

        major_tick_idx++;
    }
}

static void scale_draw_main(lv_obj_t *obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(event);

    if (scale->total_tick_count <= 1) return;

    /* Get style properties so they can be used in the tick and label drawing */
    lv_coord_t height = (lv_coord_t) lv_obj_get_content_height(obj);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t x_ofs = 0U;
    lv_coord_t y_ofs = 0U;

    if (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
        x_ofs = obj->coords.x1;
        y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    }
    else if (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
        x_ofs = obj->coords.x2;
        y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    }
    if (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y2;
    }
    else if (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y1;
    }
    else { /* Nothing to do */ }

    /* Configure both line and label draw descriptors for the tick and label drawings */
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.id = scale->mode;
    part_draw_dsc.part = LV_PART_MAIN;
    part_draw_dsc.line_dsc = &line_dsc;

    lv_point_t main_line_point_a;
    lv_point_t main_line_point_b;

    uint8_t tick_idx = 0;
    for (tick_idx = 0; tick_idx <= scale->total_tick_count; tick_idx++)
    {
        /* Setup the tick origin */
        lv_point_t tick_point_a;

        /* Setup the tick points */
        if (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            lv_coord_t vertical_position = y_ofs + (int32_t)((int32_t)(height - line_dsc.width) * tick_idx) / scale->total_tick_count;
            tick_point_a.x = x_ofs - 1U; /* Move extra pixel out of scale boundary */
            tick_point_a.y = vertical_position;
        }
        else {
            lv_coord_t horizontal_position = x_ofs + (int32_t)((int32_t)(height - line_dsc.width) * tick_idx) / scale->total_tick_count;
            tick_point_a.x = horizontal_position;
            tick_point_a.y = y_ofs;
        }

        if (0 == tick_idx) {
            main_line_point_a.x = tick_point_a.x;
            main_line_point_a.y = tick_point_a.y;
        } else if (scale->total_tick_count == tick_idx) {
            main_line_point_b.x = tick_point_a.x;
            main_line_point_b.y = tick_point_a.y;
        }
    }

    /* Draw vertical line that covers all the ticks */
    lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
    lv_draw_line(draw_ctx, &line_dsc, &main_line_point_a, &main_line_point_b);
    lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
}

#endif
