/**
 * @file lv_scale.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_scale.h"
#if LV_USE_SCALE != 0

#include "../../core/lv_group.h"
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

static void scale_draw_main(lv_obj_t * obj, lv_event_t * event);
static void scale_draw_items(lv_obj_t * obj, lv_event_t * event);
static void scale_draw_indicator(lv_obj_t * obj, lv_event_t * event);

static void scale_draw_main_round(lv_obj_t * obj, lv_event_t * event);
static void scale_draw_items_round(lv_obj_t * obj, lv_event_t * event);
static void scale_draw_indicator_round(lv_obj_t * obj, lv_event_t * event);

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

    scale->txt_src = &txt_src;

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
    if(NULL == section) return;

    section->minor_range = minor_range;
    section->major_range = major_range;
}

void lv_scale_section_set_style(lv_scale_section_t * section, uint32_t part, lv_style_t * section_part_style)
{
    if(NULL == section) return;

    switch(part) {
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
    scale->mode = LV_SCALE_MODE_HORIZONTAL_BOTTOM;
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
        if(((LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode))
           || ((LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) || (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode))) {
            scale_draw_main(obj, event);
            scale_draw_indicator(obj, event);
            scale_draw_items(obj, event);
        }
        else if((LV_SCALE_MODE_ROUND_INNER == scale->mode) || (LV_SCALE_MODE_ROUND_OUTTER == scale->mode)) {
            scale_draw_main_round(obj, event);
            scale_draw_indicator_round(obj, event);
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

static void scale_draw_items(lv_obj_t * obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    if(scale->total_tick_count <= 1) return;

    /* Configure line draw descriptor for the minor tick drawing */
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &line_dsc);

    /* Get style properties so they can be used in the tick and label drawing */
    lv_coord_t height = (lv_coord_t) lv_obj_get_content_height(obj);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t x_ofs = 0U;
    lv_coord_t y_ofs = 0U;

    if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
        x_ofs = obj->coords.x1;
        y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    }
    else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
        x_ofs = obj->coords.x2;
        y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    }
    else if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y2;
    }
    else if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y1;
    }
    else { /* Nothing to do */ }

    uint16_t total_tick_count = scale->total_tick_count;

    lv_coord_t major_len = scale->major_len;
    lv_coord_t minor_len = lv_obj_get_style_width(obj, LV_PART_ITEMS);

    /* Draw tick lines to the right */
    if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
        major_len *= -1;
        minor_len *= -1;
    }
    /* Draw tick lines to the top */
    else if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
        major_len *= -1;
        minor_len *= -1;
    }
    else { /* Nothing to do */ }

    uint8_t tick_idx = 0;
    for(tick_idx = 0; tick_idx <= total_tick_count; tick_idx++) {
        /* The tick is represented by a vertical line. We need two points to draw it */
        lv_point_t tick_point_a;
        lv_point_t tick_point_b;
        /* A major tick is the one which has a label in it */
        bool is_major_tick = false;
        if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;
        lv_coord_t tick_length = is_major_tick ? major_len : minor_len;

        if(is_major_tick) continue;

        /* Setup the tick points */
        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            lv_coord_t vertical_position = y_ofs + (int32_t)((int32_t)(height - line_dsc.width) *
                                                             (total_tick_count - tick_idx)) / total_tick_count;

            tick_point_a.x = x_ofs - 1U; /* Move extra pixel out of scale boundary */
            tick_point_a.y = vertical_position;
            tick_point_b.x = tick_point_a.x - tick_length;
            tick_point_b.y = vertical_position;
        }
        else {
            lv_coord_t horizontal_position = x_ofs + (int32_t)((int32_t)(height - line_dsc.width) * tick_idx) / total_tick_count;

            tick_point_a.x = horizontal_position;
            tick_point_a.y = y_ofs;
            tick_point_b.x = horizontal_position;
            tick_point_b.y = tick_point_a.y + tick_length;
        }

        int32_t tick_value = 0U;
        int32_t min_out = 0U;
        int32_t max_out = 0U;

        if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
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

                if(section->indicator_style) {
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

        line_dsc.p1 = tick_point_a;
        line_dsc.p2 = tick_point_b;
        lv_draw_line(layer, &line_dsc);
    }
}

static void scale_draw_indicator(lv_obj_t * obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    if(!scale->label_enabled) return;

    /* Get style properties so they can be used in the tick and label drawing */
    lv_coord_t height = (lv_coord_t) lv_obj_get_content_height(obj);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t label_gap = 0U;
    /* Get offset on both axis so the widget can be drawn from there */
    lv_coord_t x_ofs = 0U;
    lv_coord_t y_ofs = 0U;

    if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
        label_gap = lv_obj_get_style_pad_bottom(obj, LV_PART_INDICATOR);
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y2;
    }
    else if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
        label_gap = lv_obj_get_style_pad_top(obj, LV_PART_INDICATOR);
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y1;
    }
    else if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
        label_gap = lv_obj_get_style_pad_left(obj, LV_PART_TICKS);
        x_ofs = obj->coords.x1;
        y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    }
    else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
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

    lv_coord_t major_len = scale->major_len;

    /* Handle tick length being drawn backwards */
    if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
        major_len *= -1;
    }
    else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
        major_len *= -1;
    }
    else { /* Nothing to do */ }

    uint16_t total_tick_count = scale->total_tick_count;
    uint8_t tick_idx = 0;
    uint16_t major_tick_idx = 0;
    for(tick_idx = 0; tick_idx <= total_tick_count; tick_idx++) {
        /* The tick is represented by a vertical line. We need two points to draw it */
        lv_point_t tick_point_a;
        lv_point_t tick_point_b;
        /* A major tick is the one which has a label in it */
        bool is_major_tick = false;
        if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;
        if(false == is_major_tick) continue;

        lv_coord_t tick_length = major_len;

        /* Setup the tick points */
        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            lv_coord_t vertical_position = y_ofs + (int32_t)((int32_t)(height - line_dsc.width) *
                                                             (total_tick_count - tick_idx)) / total_tick_count;

            tick_point_a.x = x_ofs - 1U; /* Move extra pixel out of scale boundary */
            tick_point_a.y = vertical_position;
            tick_point_b.x = tick_point_a.x - tick_length;
            tick_point_b.y = vertical_position;
        }
        else if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
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
        if(scale->txt_src) {
            if(scale->txt_src[major_tick_idx]) {
            	label_dsc.text = scale->txt_src[major_tick_idx];
            }
            else {
            	label_dsc.text = NULL;
            }
        }
        else { /* Add label with mapped values */
            int32_t tick_value = 0U;
            int32_t min_out = 0U;
            int32_t max_out = 0U;

            if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
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

                    if(section->indicator_style) {
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
            label_dsc.text = text_buffer;
        }

        /* Reserve appropiate size for the tick label */
        lv_point_t size;
        lv_txt_get_size(&size, label_dsc.text,
                        label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

        /* Set the label draw area at some distance of the major tick */
        lv_area_t label_coords;

        if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
            label_coords.x1 = (tick_point_b.x - size.x / 2);
            label_coords.x2 = (tick_point_b.x + size.x / 2);
            label_coords.y1 = tick_point_b.y + label_gap;
            label_coords.y2 = label_coords.y1 + size.y;
        }
        else if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
            label_coords.x1 = (tick_point_b.x - size.x / 2);
            label_coords.x2 = (tick_point_b.x + size.x / 2);
            label_coords.y2 = tick_point_b.y - label_gap;
            label_coords.y1 = label_coords.y2 - size.y;
        }
        else if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            label_coords.x1 = tick_point_b.x - size.x - label_gap;
            label_coords.x2 = tick_point_b.x - label_gap;
            label_coords.y1 = (tick_point_b.y - size.y / 2);
            label_coords.y2 = (tick_point_b.y + size.y / 2);
        }
        else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            label_coords.x1 = tick_point_b.x + label_gap;
            label_coords.x2 = tick_point_b.x + size.x + label_gap;
            label_coords.y1 = (tick_point_b.y - size.y / 2);
            label_coords.y2 = (tick_point_b.y + size.y / 2);
        }
        else { /* Nothing to do */ }

        line_dsc.p1 = tick_point_a;
        line_dsc.p2 = tick_point_b;
        lv_draw_line(layer, &line_dsc);
        lv_draw_label(layer, &label_dsc, &label_coords);

        major_tick_idx++;
    }
}

static void scale_draw_main(lv_obj_t * obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    if(scale->total_tick_count <= 1) return;

    /* Get style properties so they can be used in the tick and label drawing */
    lv_coord_t height = (lv_coord_t) lv_obj_get_content_height(obj);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t x_ofs = 0U;
    lv_coord_t y_ofs = 0U;

    if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
        x_ofs = obj->coords.x1;
        y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    }
    else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
        x_ofs = obj->coords.x2;
        y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    }
    if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y2;
    }
    else if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
        x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
        y_ofs = obj->coords.y1;
    }
    else { /* Nothing to do */ }

    /* Configure both line and label draw descriptors for the tick and label drawings */
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

    lv_point_t main_line_point_a;
    lv_point_t main_line_point_b;

    uint8_t tick_idx = 0;
    for(tick_idx = 0; tick_idx <= scale->total_tick_count; tick_idx++) {
        /* Setup the tick origin */
        lv_point_t tick_point_a;

        /* Setup the tick points */
        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            lv_coord_t vertical_position = y_ofs + (int32_t)((int32_t)(height - line_dsc.width) * tick_idx) /
                                           scale->total_tick_count;
            tick_point_a.x = x_ofs - 1U; /* Move extra pixel out of scale boundary */
            tick_point_a.y = vertical_position;
        }
        else {
            lv_coord_t horizontal_position = x_ofs + (int32_t)((int32_t)(height - line_dsc.width) * tick_idx) /
                                             scale->total_tick_count;
            tick_point_a.x = horizontal_position;
            tick_point_a.y = y_ofs;
        }

        if(0 == tick_idx) {
            main_line_point_a.x = tick_point_a.x;
            main_line_point_a.y = tick_point_a.y;
        }
        else if(scale->total_tick_count == tick_idx) {
            main_line_point_b.x = tick_point_a.x;
            main_line_point_b.y = tick_point_a.y;
        }
    }

    /* Draw vertical line that covers all the ticks */
    line_dsc.p1 = main_line_point_a;
    line_dsc.p2 = main_line_point_b;
    lv_draw_line(layer, &line_dsc);
}

static void get_center(const lv_obj_t * obj, lv_point_t * center, lv_coord_t * arc_r)
{
    lv_coord_t left_bg = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t right_bg = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    lv_coord_t top_bg = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t bottom_bg = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

    lv_coord_t r = (LV_MIN(lv_obj_get_width(obj) - left_bg - right_bg,
                           lv_obj_get_height(obj) - top_bg - bottom_bg)) / 2U;

    center->x = obj->coords.x1 + r + left_bg;
    center->y = obj->coords.y1 + r + top_bg;

    if(arc_r) *arc_r = r;
}

static void scale_draw_main_round(lv_obj_t * obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    if(scale->total_tick_count <= 1) return;

    /* Get style properties so they can be used in the tick and label drawing */
    lv_coord_t height = (lv_coord_t) lv_obj_get_content_height(obj);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t x_ofs = obj->coords.x1;
    lv_coord_t y_ofs = obj->coords.y1;

    /* Configure arc draw descriptors for the main part */
    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);
    lv_obj_init_draw_arc_dsc(obj, LV_PART_MAIN, &arc_dsc);

    lv_point_t arc_center;
    lv_coord_t arc_radius;
    get_center(obj, &arc_center, &arc_radius);

    // LV_LOG_USER("Arc center: {X:%d, Y:%d}, radius: %d", arc_center.x, arc_center.y, arc_radius);

    arc_dsc.center = arc_center;
	arc_dsc.radius = arc_radius;
	/* TODO: Set as properties? */
	arc_dsc.start_angle = 0U;
	arc_dsc.end_angle = 270U;

    lv_draw_arc(layer, &arc_dsc);
}

static void scale_draw_items_round(lv_obj_t * obj, lv_event_t * event)
{

}

static void scale_draw_indicator_round(lv_obj_t * obj, lv_event_t * event)
{

    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    lv_area_t scale_area;
    lv_obj_get_content_coords(obj, &scale_area);

    /* Find the center of the scale */
    lv_point_t center_point;
    lv_coord_t radius_edge = LV_MIN(lv_area_get_width(&scale_area) / 2U, lv_area_get_height(&scale_area) / 2U);
    center_point.x = scale_area.x1 + radius_edge;
    center_point.y = scale_area.y1 + radius_edge;

    // LV_LOG_USER("Center at {%d:%d}. Edge: %d", center_point.x, center_point.y, radius_edge);

    /* Major tick */
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &line_dsc);
    line_dsc.raw_end = 1;

    /* Label */
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_INDICATOR, &label_dsc);

    lv_coord_t radius_out = radius_edge;
    lv_coord_t radius_in_major = radius_out - scale->major_len;

    uint32_t angular_range = 270U; /* TODO: Add property to scale. How big (from 0° to 360°) is the arc */
    uint32_t rotation = 0U; /* TODO: Add property to scale. Rotation in clock wise from arc 0 to value 0 */
    uint16_t label_gap = 15U; /* TODO: Add property to scale. Gap between major ticks and labels */
    uint8_t tick_idx = 0;
    for(tick_idx = 0; tick_idx <= scale->total_tick_count; tick_idx++) {
		/* A major tick is the one which has a label in it */
		bool is_major_tick = false;
		if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;
		if(false == is_major_tick) continue;

		int32_t angle_upscale = ((tick_idx * angular_range) * 10U) / (scale->total_tick_count - 1U);
		angle_upscale += rotation * 10U;

        /*Draw a little bit longer lines to be sure the mask will clip them correctly
         *and to get a better precision*/
        lv_point_t p_outer;
        p_outer.x = center_point.x + radius_out;
        p_outer.y = center_point.y;
        lv_point_transform(&p_outer, angle_upscale, 256U, &center_point); /* TODO: What is 256? */

        lv_point_t p_inner;
        p_inner.x = center_point.x + radius_in_major;
        p_inner.y = center_point.y;
        lv_point_transform(&p_inner, angle_upscale, 256U, &center_point);  /* TODO: What is 256? */

		uint32_t radius_text = radius_in_major - label_gap;

		lv_point_t point;
		point.x = center_point.x + radius_text;
		point.y = center_point.y;
		lv_point_transform(&point, angle_upscale, 256, &center_point);

        int32_t tick_value = 0U;
        int32_t min_out = scale->range_min;
        int32_t max_out = scale->range_max;

        tick_value = lv_map(tick_idx, 0U, scale->total_tick_count, min_out, max_out);

        /* TODO: Support custom labels */
        char buf[16] = {0U};
        lv_snprintf(buf, sizeof(buf), "%" LV_PRId32, tick_value);

        lv_point_t label_size;
        label_dsc.text = buf;
        lv_txt_get_size(&label_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space,
                        LV_COORD_MAX, LV_TEXT_FLAG_NONE);

        lv_area_t label_cord;
        label_cord.x1 = point.x - label_size.x / 2;
        label_cord.y1 = point.y - label_size.y / 2;
        label_cord.x2 = label_cord.x1 + label_size.x;
        label_cord.y2 = label_cord.y1 + label_size.y;

        LV_LOG_USER("Label %s: X1: %d, Y1: %d, X2: %d, Y2: %d", buf, label_cord.x1, label_cord.y1, label_cord.x2, label_cord.y2);

        lv_draw_label(layer, &label_dsc, &label_cord);

		line_dsc.p1 = p_outer;
		line_dsc.p2 = p_inner;
		lv_draw_line(layer, &line_dsc);
    }

#if 0
    lv_meter_scale_t * scale;

    _LV_LL_READ_BACK(&meter->scale_ll, scale) {
        part_draw_dsc.sub_part_ptr = scale;

        lv_coord_t r_out = r_edge;
        lv_coord_t r_in_minor = r_out - scale->tick_length;
        lv_coord_t r_in_major = r_out - scale->tick_major_length;

        lv_area_t area_inner_minor;
        area_inner_minor.x1 = p_center.x - r_in_minor;
        area_inner_minor.y1 = p_center.y - r_in_minor;
        area_inner_minor.x2 = p_center.x + r_in_minor;
        area_inner_minor.y2 = p_center.y + r_in_minor;
        lv_draw_mask_radius_init(&inner_minor_mask, &area_inner_minor, LV_RADIUS_CIRCLE, true);

        lv_area_t area_inner_major;
        area_inner_major.x1 = p_center.x - r_in_major;
        area_inner_major.y1 = p_center.y - r_in_major;
        area_inner_major.x2 = p_center.x + r_in_major - 1;
        area_inner_major.y2 = p_center.y + r_in_major - 1;
        lv_draw_mask_radius_init(&inner_major_mask, &area_inner_major, LV_RADIUS_CIRCLE, true);

        lv_area_t area_outer;
        area_outer.x1 = p_center.x - r_out;
        area_outer.y1 = p_center.y - r_out;
        area_outer.x2 = p_center.x + r_out - 1;
        area_outer.y2 = p_center.y + r_out - 1;
        lv_draw_mask_radius_init(&outer_mask, &area_outer, LV_RADIUS_CIRCLE, false);
        int16_t outer_mask_id = lv_draw_mask_add(&outer_mask, NULL);

        int16_t inner_act_mask_id = LV_MASK_ID_INV; /*Will be added later*/

        uint32_t minor_cnt = scale->tick_major_nth ? scale->tick_major_nth - 1 : 0xFFFF;
        uint16_t i;
        for(i = 0; i < scale->tick_cnt; i++) {
            minor_cnt++;
            bool major = false;
            if(minor_cnt == scale->tick_major_nth) {
                minor_cnt = 0;
                major = true;
            }

            int32_t value_of_line = lv_map(i, 0, scale->tick_cnt - 1, scale->min, scale->max);
            part_draw_dsc.value = value_of_line;

            lv_color_t line_color = major ? scale->tick_major_color : scale->tick_color;
            lv_color_t line_color_ori = line_color;

            lv_coord_t line_width_ori = major ? scale->tick_major_width : scale->tick_width;
            lv_coord_t line_width = line_width_ori;

            lv_meter_indicator_t * indic;
            _LV_LL_READ_BACK(&meter->indicator_ll, indic) {
                if(indic->type != LV_METER_INDICATOR_TYPE_SCALE_LINES) continue;
                if(value_of_line >= indic->start_value && value_of_line <= indic->end_value) {
                    line_width += indic->type_data.scale_lines.width_mod;

                    if(indic->type_data.scale_lines.color_start.full == indic->type_data.scale_lines.color_end.full) {
                        line_color = indic->type_data.scale_lines.color_start;
                    }
                    else {
                        lv_opa_t ratio;
                        if(indic->type_data.scale_lines.local_grad) {
                            ratio = lv_map(value_of_line, indic->start_value, indic->end_value, LV_OPA_TRANSP, LV_OPA_COVER);
                        }
                        else {
                            ratio = lv_map(value_of_line, scale->min, scale->max, LV_OPA_TRANSP, LV_OPA_COVER);
                        }
                        line_color = lv_color_mix(indic->type_data.scale_lines.color_end, indic->type_data.scale_lines.color_start, ratio);
                    }
                }
            }

            int32_t angle_upscale = ((i * scale->angle_range) * 10) / (scale->tick_cnt - 1) +  + scale->rotation * 10;

            line_dsc.color = line_color;
            line_dsc.width = line_width;

            /*Draw a little bit longer lines to be sure the mask will clip them correctly
             *and to get a better precision*/
            lv_point_t p_outer;
            p_outer.x = p_center.x + r_out + LV_MAX(LV_DPI_DEF, r_out);
            p_outer.y = p_center.y;
            lv_point_transform(&p_outer, angle_upscale, 256, &p_center);

            part_draw_dsc.p1 = &p_center;
            part_draw_dsc.p2 = &p_outer;
            part_draw_dsc.id = i;
            part_draw_dsc.label_dsc = &label_dsc;

            /*Draw the text*/
            if(major) {
                lv_draw_mask_remove_id(outer_mask_id);
                uint32_t r_text = r_in_major - scale->label_gap;
                lv_point_t p;
                p.x = p_center.x + r_text;
                p.y = p_center.y;
                lv_point_transform(&p, angle_upscale, 256, &p_center);

                lv_draw_label_dsc_t label_dsc_tmp;
                lv_memcpy(&label_dsc_tmp, &label_dsc, sizeof(label_dsc_tmp));

                part_draw_dsc.label_dsc = &label_dsc_tmp;
                char buf[16];

                lv_snprintf(buf, sizeof(buf), "%" LV_PRId32, value_of_line);
                part_draw_dsc.text = buf;

                lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);

                lv_point_t label_size;
                lv_txt_get_size(&label_size, part_draw_dsc.text, label_dsc.font, label_dsc.letter_space, label_dsc.line_space,
                                LV_COORD_MAX, LV_TEXT_FLAG_NONE);

                lv_area_t label_cord;
                label_cord.x1 = p.x - label_size.x / 2;
                label_cord.y1 = p.y - label_size.y / 2;
                label_cord.x2 = label_cord.x1 + label_size.x;
                label_cord.y2 = label_cord.y1 + label_size.y;

                lv_draw_label(draw_ctx, part_draw_dsc.label_dsc, &label_cord, part_draw_dsc.text, NULL);

                outer_mask_id = lv_draw_mask_add(&outer_mask, NULL);
            }
            else {
                part_draw_dsc.label_dsc = NULL;
                part_draw_dsc.text = NULL;
                lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
            }

            inner_act_mask_id = lv_draw_mask_add(major ? &inner_major_mask : &inner_minor_mask, NULL);
            lv_draw_line(draw_ctx, &line_dsc, &p_outer, &p_center);
            lv_draw_mask_remove_id(inner_act_mask_id);
            lv_event_send(obj, LV_EVENT_DRAW_MAIN_END, &part_draw_dsc);

            line_dsc.color = line_color_ori;
            line_dsc.width = line_width_ori;

        }
        lv_draw_mask_free_param(&inner_minor_mask);
        lv_draw_mask_free_param(&inner_major_mask);
        lv_draw_mask_free_param(&outer_mask);
        lv_draw_mask_remove_id(outer_mask_id);
    }
#endif
}

#endif
