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

#define LV_SCALE_LABEL_TXT_LEN  (20U)

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

static void scale_get_center(const lv_obj_t * obj, lv_point_t * center, lv_coord_t * arc_r);
static void scale_get_tick_points(lv_obj_t * obj, const uint16_t tick_idx, bool is_major_tick,
                                        lv_point_t * tick_point_a, lv_point_t * tick_point_b);
static void scale_get_label_coords(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc, lv_point_t * tick_point, lv_area_t * label_coords);
static void scale_set_items_properties(lv_obj_t * obj, lv_draw_line_dsc_t * line_dsc, lv_style_t * items_section_style);
static void scale_set_indicator_label_properties(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc,
                                                 lv_style_t * indicator_section_style);
static void scale_set_indicator_line_properties(lv_obj_t * obj, lv_draw_line_dsc_t * line_dsc,
                                                lv_style_t * indicator_section_style);
static void scale_find_section_tick_idx(lv_obj_t * obj);

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

void lv_scale_set_mode(lv_obj_t * obj, lv_scale_mode_t mode)
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

void lv_scale_set_round_props(lv_obj_t * obj, uint16_t angle_range, int16_t rotation)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->angle_range = angle_range;
    scale->rotation = rotation;

    lv_obj_invalidate(obj);
}

void lv_scale_set_text_src(lv_obj_t * obj, char * txt_src[])
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
    section->first_tick_idx_in_section = 255U;
    section->last_tick_idx_in_section = 255U;
    section->first_tick_idx_is_major = 0U;
    section->last_tick_idx_is_major = 0U;

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
    scale->angle_range = 270U; /* TODO: Replace with symbol? */
    scale->rotation = 90 + (360 - scale->angle_range) / 2; /* TODO: Replace magic numbers */
    scale->range_min = 0U;
    scale->range_max = 100U;
    scale->major_len = 10U;
    scale->minor_len = 5u;
    scale->last_tick_width = 0U;
    scale->first_tick_width = 0U;

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
    LV_UNUSED(scale);

    if(event_code == LV_EVENT_DRAW_MAIN) {
        scale_find_section_tick_idx(obj);
        scale_draw_items(obj, event);
        scale_draw_indicator(obj, event);
        scale_draw_main(obj, event);
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
    lv_draw_line_dsc_t minor_tick_dsc;
    lv_draw_line_dsc_init(&minor_tick_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &minor_tick_dsc);

    const uint16_t total_tick_count = scale->total_tick_count;

    uint8_t tick_idx = 0;
    for(tick_idx = 0; tick_idx <= total_tick_count; tick_idx++) {
        /* A major tick is the one which has a label in it */
        bool is_major_tick = false;
        if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;
        if(is_major_tick) continue;

        const int32_t tick_value = lv_map(tick_idx, 0U, total_tick_count, scale->range_min, scale->range_max);

        /* Overwrite label properties if tick value is within section range */
        lv_scale_section_t * section;
        _LV_LL_READ_BACK(&scale->section_ll, section) {
            if(section->minor_range <= tick_value && section->major_range >= tick_value) {
                scale_set_items_properties(obj, &minor_tick_dsc, section->items_style);
            }
        }

        lv_point_t tick_point_a;
        lv_point_t tick_point_b;
        scale_get_tick_points(obj, tick_idx, is_major_tick, &tick_point_a, &tick_point_b);

        minor_tick_dsc.p1 = tick_point_a;
        minor_tick_dsc.p2 = tick_point_b;
        lv_draw_line(layer, &minor_tick_dsc);
    }
}

static void scale_draw_indicator(lv_obj_t * obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    // if(!scale->label_enabled) return;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    /* Formatting the labels with the configured style for LV_PART_INDICATOR */
    lv_obj_init_draw_label_dsc(obj, LV_PART_INDICATOR, &label_dsc);

    /* Major tick style */
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &line_dsc);

    /* Main line style */
    lv_draw_line_dsc_t main_line_dsc;
    lv_draw_line_dsc_init(&main_line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &main_line_dsc);

    if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
       || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {

        uint16_t total_tick_count = scale->total_tick_count;
        uint8_t tick_idx = 0;
        uint16_t major_tick_idx = 0;
        for(tick_idx = 0; tick_idx <= total_tick_count; tick_idx++) {
            /* A major tick is the one which has a label in it */
            bool is_major_tick = false;
            if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;
            if(false == is_major_tick) continue;
            major_tick_idx++;

            const int32_t tick_value = lv_map(tick_idx, 0U, total_tick_count, scale->range_min, scale->range_max);

            /* Overwrite label properties if tick value is within section range */
            lv_scale_section_t * section;
            _LV_LL_READ_BACK(&scale->section_ll, section) {
                if(section->minor_range <= tick_value && section->major_range >= tick_value) {
                    scale_set_indicator_label_properties(obj, &label_dsc, section->indicator_style);
                    scale_set_indicator_line_properties(obj, &line_dsc, section->indicator_style);
                }
            }

            /* The tick is represented by a vertical line. We need two points to draw it */
            lv_point_t tick_point_a;
            lv_point_t tick_point_b;
            scale_get_tick_points(obj, tick_idx, is_major_tick, &tick_point_a, &tick_point_b);

            if(scale->label_enabled) {
                /* Label text setup */
                char text_buffer[LV_SCALE_LABEL_TXT_LEN] = {0};
                lv_area_t label_coords;

                /* Check if the custom text array has element for this major tick index */
                if(scale->txt_src) {
                    if(scale->txt_src[major_tick_idx - 1U]) {
                        label_dsc.text = scale->txt_src[major_tick_idx - 1U];
                    }
                    else {
                        label_dsc.text = NULL;
                    }
                }
                else { /* Add label with mapped values */
                    lv_snprintf(text_buffer, sizeof(text_buffer), "%" LV_PRId32, tick_value);
                    label_dsc.text = text_buffer;
                }

                scale_get_label_coords(obj, &label_dsc, &tick_point_b, &label_coords);
                lv_draw_label(layer, &label_dsc, &label_coords);
            }

            /* Store initial and last tick widths to be used in the main line drawing */
            if(total_tick_count == tick_idx) {
                if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) || (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)) {
                    scale->last_tick_width = line_dsc.width;
                }
                else {
                    scale->first_tick_width = line_dsc.width;
                }
            }
            else if(0U == tick_idx) {
                if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) || (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)) {
                    scale->first_tick_width = line_dsc.width;
                }
                else {
                    scale->last_tick_width = line_dsc.width;
                }
            }
            else { /* Nothing to do */ }

            LV_LOG_USER("Tick %d at x %d,%d, tick_width: %d", tick_idx, tick_point_a.x, tick_point_b.y, line_dsc.width);

            /* Used to calculate position of main line section */
            _LV_LL_READ_BACK(&scale->section_ll, section) {
                if(section->minor_range <= tick_value && section->major_range >= tick_value) {

                    scale_set_indicator_label_properties(obj, &label_dsc, section->indicator_style);
                    scale_set_indicator_line_properties(obj, &line_dsc, section->indicator_style);
                }

                /* Store the first and last section tick vertical/horizontal position
                 * TODO: Move this tick position calculation into an API that can be used in scale_draw_main */
                if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
                    if(tick_idx == section->first_tick_idx_in_section && section->first_tick_idx_is_major) {
                        section->first_tick_in_section.y = tick_point_a.y;
                    }
                    else if(tick_idx == section->last_tick_idx_in_section && section->last_tick_idx_is_major) {
                        /* NOTE: Add the custom line width compensation */
                        section->last_tick_in_section.y = tick_point_a.y - line_dsc.width / 2U;
                    }
                }
                else {
                    if(tick_idx == section->first_tick_idx_in_section && section->first_tick_idx_is_major) {
                        section->first_tick_in_section.x = tick_point_a.x;
                    }
                    else if(tick_idx == section->last_tick_idx_in_section && section->last_tick_idx_is_major) {
                        section->last_tick_in_section.x = tick_point_a.x + line_dsc.width / 2U;
                    }
                }
            }

            line_dsc.p1 = tick_point_a;
            line_dsc.p2 = tick_point_b;
            lv_draw_line(layer, &line_dsc);
        }
    }
    else if(LV_SCALE_MODE_ROUND_OUTTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        lv_area_t scale_area;
        lv_obj_get_content_coords(obj, &scale_area);

        /* Find the center of the scale */
        lv_point_t center_point;
        lv_coord_t radius_edge = LV_MIN(lv_area_get_width(&scale_area) / 2U, lv_area_get_height(&scale_area) / 2U);
        center_point.x = scale_area.x1 + radius_edge;
        center_point.y = scale_area.y1 + radius_edge;

        /* Major tick */
        line_dsc.raw_end = 0;

        lv_coord_t radius_out = radius_edge;
        lv_coord_t radius_in_major;
        if(LV_SCALE_MODE_ROUND_INNER == scale->mode) {
            radius_in_major = radius_out - scale->major_len;
        }
        else {
            radius_in_major = radius_out + scale->major_len;
        }

        uint32_t angular_range = scale->angle_range;
        uint32_t rotation = scale->rotation;
        uint16_t label_gap = 15U; /* TODO: Consider lv_style_set_text_letter_space on sections */
        uint8_t tick_idx = 0;
        uint16_t major_tick_idx = 0;
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

            /* Also take into consideration the letter space of the style */
            uint32_t radius_text;
            if(LV_SCALE_MODE_ROUND_INNER == scale->mode) {
                radius_text = radius_in_major - (label_gap + label_dsc.letter_space);
            }
            else {
                radius_text = radius_in_major + (label_gap + label_dsc.letter_space);
            }

            lv_point_t point;
            point.x = center_point.x + radius_text;
            point.y = center_point.y;
            lv_point_transform(&point, angle_upscale, 256, &center_point);

            char buf[LV_SCALE_LABEL_TXT_LEN] = {0U};
            const int32_t min_out = scale->range_min;
            const int32_t max_out = scale->range_max;
            const int32_t tick_value = lv_map(tick_idx, 0U, scale->total_tick_count, min_out, max_out);

            /* Check if the custom text array has element for this major tick index */
            if(scale->txt_src) {
                if(scale->txt_src[major_tick_idx]) {
                    label_dsc.text = scale->txt_src[major_tick_idx];
                    /* Increment major tick counter only when we haven't reached the NULL sentinel */
                    major_tick_idx++;
                }
                else {
                    label_dsc.text = NULL;
                    /* TODO: Out of custom labels */
                }
            }
            else { /* Add label with mapped values */
                lv_snprintf(buf, sizeof(buf), "%" LV_PRId32, tick_value);
                label_dsc.text = buf;
            }

            /* Overwrite label properties if tick value is within section range */
            lv_scale_section_t * section;
            _LV_LL_READ_BACK(&scale->section_ll, section) {
                if(section->minor_range <= tick_value && section->major_range >= tick_value) {

                    scale_set_indicator_label_properties(obj, &label_dsc, section->indicator_style);

                    if(section->indicator_style) {
                        lv_style_value_t value;
                        lv_res_t res;

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

            if(label_dsc.text && scale->label_enabled) {
                /* Reserve appropriate size for the tick label */
                lv_point_t label_size;
                lv_txt_get_size(&label_size, label_dsc.text,
                                label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

                /* Set the label draw area at some distance of the major tick */
                lv_area_t label_coord;
                label_coord.x1 = point.x - label_size.x / 2;
                label_coord.y1 = point.y - label_size.y / 2;
                label_coord.x2 = label_coord.x1 + label_size.x;
                label_coord.y2 = label_coord.y1 + label_size.y;

                lv_draw_label(layer, &label_dsc, &label_coord);
            }

            line_dsc.p1 = p_outer;
            line_dsc.p2 = p_inner;
            lv_draw_line(layer, &line_dsc);
        }
    }
    else { /* Nothing to do */ }
}

static void scale_draw_main(lv_obj_t * obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    if(scale->total_tick_count <= 1) return;

    if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
       || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {

        /* Configure both line and label draw descriptors for the tick and label drawings */
        lv_draw_line_dsc_t line_dsc;
        lv_draw_line_dsc_init(&line_dsc);
        lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

        /* Get style properties so they can be used in the main line drawing */
        const lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
        const lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj,
                                                                                                               LV_PART_MAIN);
        const lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj,
                                                                                                                 LV_PART_MAIN);
        const lv_coord_t scroll_top = lv_obj_get_scroll_top(obj);
        const lv_coord_t scroll_left = lv_obj_get_scroll_left(obj);

        lv_coord_t x_ofs = 0U;
        lv_coord_t y_ofs = 0U;

        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            x_ofs = obj->coords.x2 + (line_dsc.width / 2U);
            y_ofs = obj->coords.y1 + pad_top + border_width - scroll_top;
        }
        else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            x_ofs = obj->coords.x1 + (line_dsc.width / 2U);
            y_ofs = obj->coords.y1 + pad_top + border_width - scroll_top;
        }
        if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
            x_ofs = obj->coords.x1 + pad_left - scroll_left;
            y_ofs = obj->coords.y1 + (line_dsc.width / 2U);
        }
        else if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
            x_ofs = obj->coords.x1 + pad_left - scroll_left;
            y_ofs = obj->coords.y2 + (line_dsc.width / 2U);
        }
        else { /* Nothing to do */ }

        lv_point_t main_line_point_a;
        lv_point_t main_line_point_b;

        /* Setup the tick points */
        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            main_line_point_a.x = x_ofs - 1U;
            main_line_point_a.y = y_ofs - 1U;
            main_line_point_b.x = x_ofs - 1U;
            main_line_point_b.y = obj->coords.y2 + pad_left - scroll_top;

            /* Adjust main line with initial and last tick width */
            main_line_point_a.y -= scale->last_tick_width / 2U;
            main_line_point_b.y += scale->first_tick_width / 2U;
        }
        else {
            main_line_point_a.x = x_ofs;
            main_line_point_a.y = y_ofs;
            main_line_point_b.x = obj->coords.x2 + pad_left - scroll_left;
            main_line_point_b.y = y_ofs;

            /* Adjust main line with initial and last tick width */
            main_line_point_a.x -= scale->last_tick_width / 2U;
            main_line_point_b.x += scale->first_tick_width / 2U;
        }

        line_dsc.p1 = main_line_point_a;
        line_dsc.p2 = main_line_point_b;
        lv_draw_line(layer, &line_dsc);

        lv_scale_section_t * section;
        _LV_LL_READ_BACK(&scale->section_ll, section) {
            lv_draw_line_dsc_t main_line_section_dsc;
            lv_draw_line_dsc_init(&main_line_section_dsc);
            lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &main_line_section_dsc);

            /* Calculate the points of the section line */
            lv_point_t main_point_a;
            lv_point_t main_point_b;
            lv_point_t dummy; /* Dummy point used when we calculate the position of a minor tick position */

            /* Calculate the position of the section based on the ticks (first and last) index */
            if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
                /* Calculate position of the first tick in the section */
                if(section->first_tick_idx_is_major) {
                    main_point_a.x = main_line_point_a.x;
                    main_point_a.y = section->first_tick_in_section.y;
                }
                else {
                    scale_get_tick_points(obj, section->first_tick_idx_in_section, section->first_tick_idx_is_major, &main_point_a, &dummy);
                }

                /* Calculate position of the last tick in the section */
                if(section->last_tick_idx_is_major) {
                    main_point_b.x = main_line_point_a.x;
                    main_point_b.y = section->last_tick_in_section.y;
                }
                else {
                    scale_get_tick_points(obj, section->last_tick_idx_in_section, section->last_tick_idx_is_major, &main_point_b, &dummy);
                }
            }
            else {
                /* Calculate position of the first tick in the section */
                if(section->first_tick_idx_is_major) {
                    main_point_a.x = section->first_tick_in_section.x;
                    main_point_a.y = main_line_point_a.y;
                }
                else {
                    scale_get_tick_points(obj, section->first_tick_idx_in_section, section->first_tick_idx_is_major, &main_point_a, &dummy);
                }

                /* Calculate position of the last tick in the section */
                if(section->last_tick_idx_is_major) {
                    main_point_b.x = section->last_tick_in_section.x;
                    main_point_b.y = main_line_point_a.y;
                }
                else {
                    scale_get_tick_points(obj, section->last_tick_idx_in_section, section->last_tick_idx_is_major, &main_point_b, &dummy);
                }
            }

            if(section->main_style) {
                lv_style_value_t value;
                lv_res_t res;

                /* Tick width */
                res = lv_style_get_prop(section->main_style, LV_STYLE_LINE_WIDTH, &value);
                if(res == LV_RES_OK) {
                    main_line_section_dsc.width = (lv_coord_t)value.num;
                }
                else {
                    main_line_section_dsc.width = lv_obj_get_style_line_width(obj, LV_PART_MAIN);
                }

                /* Tick color */
                res = lv_style_get_prop(section->main_style, LV_STYLE_LINE_COLOR, &value);
                if(res == LV_RES_OK) {
                    main_line_section_dsc.color = value.color;
                }
                else {
                    main_line_section_dsc.color = lv_obj_get_style_line_color(obj, LV_PART_MAIN);
                }

                /* Tick opa */
                res = lv_style_get_prop(section->main_style, LV_STYLE_LINE_OPA, &value);
                if(res == LV_RES_OK) {
                    main_line_section_dsc.opa = (lv_opa_t)value.num;
                }
                else {
                    main_line_section_dsc.opa = lv_obj_get_style_line_opa(obj, LV_PART_MAIN);
                }

                /* Tick gap */
                main_line_section_dsc.p1 = main_point_a;
                main_line_section_dsc.p2 = main_point_b;
                lv_draw_line(layer, &main_line_section_dsc);
            }
            else { /* Nothing to do */ }
        }
    }
    else if(LV_SCALE_MODE_ROUND_OUTTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        /* Configure arc draw descriptors for the main part */
        lv_draw_arc_dsc_t arc_dsc;
        lv_draw_arc_dsc_init(&arc_dsc);
        lv_obj_init_draw_arc_dsc(obj, LV_PART_MAIN, &arc_dsc);

        lv_point_t arc_center;
        lv_coord_t arc_radius;
        scale_get_center(obj, &arc_center, &arc_radius);

        const int32_t start_angle = lv_map(scale->range_min, scale->range_min, scale->range_max, scale->rotation,
                                           scale->rotation + scale->angle_range);
        const int32_t end_angle = lv_map(scale->range_max, scale->range_min, scale->range_max, scale->rotation,
                                         scale->rotation + scale->angle_range);

        arc_dsc.center = arc_center;
        arc_dsc.radius = arc_radius;
        arc_dsc.start_angle = start_angle;
        arc_dsc.end_angle = end_angle;

        lv_draw_arc(layer, &arc_dsc);
    }
    else { /* Nothing to do */ }
}

/**
 * Get center point and radius of scale arc
 * @param obj       pointer to a scale object
 * @param center    pointer to center
 * @param arc_r     pointer to arc radius
 */
static void scale_get_center(const lv_obj_t * obj, lv_point_t * center, lv_coord_t * arc_r)
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

/**
 * Get points for ticks
 *
 * In order to draw ticks we need two points, this interface returns both points for all scale modes.
 *
 * @param obj       pointer to a scale object
 * @param tick_idx  index of the current tick
 * @param is_major_tick true if tick_idx is a major tick
 * @param tick_point_a  pointer to point 'a' of the tick
 * @param tick_point_b  pointer to point 'b' of the tick
 */
static void scale_get_tick_points(lv_obj_t * obj, const uint16_t tick_idx, bool is_major_tick,
                                        lv_point_t * tick_point_a, lv_point_t * tick_point_b)
{
    lv_scale_t * scale = (lv_scale_t *)obj;

    lv_coord_t minor_len = 0;
    lv_coord_t major_len = 0;

    if (is_major_tick) {
    	major_len = scale->major_len;
    } else {
    	minor_len = scale->minor_len;
	}

    if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
       || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {

        /* Main line style */
        lv_draw_line_dsc_t main_line_dsc;
        lv_draw_line_dsc_init(&main_line_dsc);
        lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &main_line_dsc);

        /* Get style properties so they can be used in the tick and label drawing */
        const lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
        const lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj,
                                                                                                               LV_PART_MAIN);
        const lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj,
                                                                                                                 LV_PART_MAIN);
        lv_coord_t x_ofs = 0U;
        lv_coord_t y_ofs = 0U;

        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            x_ofs = obj->coords.x2 + (main_line_dsc.width / 2U);
            y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
        }
        else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            x_ofs = obj->coords.x1 + (main_line_dsc.width / 2U);
            y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
        }
        else if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
            x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
            y_ofs = obj->coords.y1 + (main_line_dsc.width / 2U);
        }
        else if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
            x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
            y_ofs = obj->coords.y2 + (main_line_dsc.width / 2U);
        }
        else { /* Nothing to do */ }


        if(is_major_tick && ((LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) || (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode))) {
        	major_len *= -1;
        }
        /* Draw tick lines to the right or top */
        else if(!is_major_tick && (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {
            minor_len *= -1;
        }
        else { /* Nothing to do */ }

        const lv_coord_t tick_length = is_major_tick ? major_len : minor_len;

        /* Setup the tick points */
        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            /* Vertical position starts at the bottom side of the main line */
            lv_coord_t vertical_position = obj->coords.y2;
            /* Increment the tick offset depending of its index */
            if(0 != tick_idx) {
                vertical_position -= (lv_obj_get_height(obj) / scale->total_tick_count) * tick_idx;
            }

            tick_point_a->x = x_ofs - 1U; /* Move extra pixel out of scale boundary */
            tick_point_a->y = vertical_position;
            tick_point_b->x = tick_point_a->x - tick_length;
            tick_point_b->y = vertical_position;
        }
        else {
            /* Horizontal position starts at the right most side of the main line */
            lv_coord_t horizontal_position = obj->coords.x1;
            /* Increment the tick offset depending of its index */
            if(0 != tick_idx) {
                horizontal_position += (lv_obj_get_width(obj) / scale->total_tick_count) * tick_idx;
            }

            tick_point_a->x = horizontal_position;
            tick_point_a->y = y_ofs;
            tick_point_b->x = horizontal_position;
            tick_point_b->y = tick_point_a->y + tick_length;
        }
    }
    else if(LV_SCALE_MODE_ROUND_OUTTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        lv_area_t scale_area;
        lv_obj_get_content_coords(obj, &scale_area);

        /* Find the center of the scale */
        lv_point_t center_point;
        const lv_coord_t radius_edge = LV_MIN(lv_area_get_width(&scale_area) / 2U, lv_area_get_height(&scale_area) / 2U);
        center_point.x = scale_area.x1 + radius_edge;
        center_point.y = scale_area.y1 + radius_edge;

        lv_coord_t radius_in_minor;
        if(LV_SCALE_MODE_ROUND_INNER == scale->mode) {
            radius_in_minor = radius_edge - minor_len;
        }
        else {
            radius_in_minor = radius_edge + minor_len;
        }

        int32_t angle_upscale = ((tick_idx * scale->angle_range) * 10U) / (scale->total_tick_count - 1U);
        angle_upscale += scale->rotation * 10U;

        /*Draw a little bit longer lines to be sure the mask will clip them correctly
         *and to get a better precision*/
        tick_point_a->x = center_point.x + radius_edge;
        tick_point_a->y = center_point.y;
        lv_point_transform(tick_point_a, angle_upscale, 256, &center_point);

        tick_point_b->x = center_point.x + radius_in_minor;
        tick_point_b->y = center_point.y;
        lv_point_transform(tick_point_b, angle_upscale, 256, &center_point);
    }
    else { /* Nothing to do */ }
}

/**
 * Get coordinates for label
 *
 * @param obj       pointer to a scale object
 * @param label_dsc	pointer to label descriptor
 * @param tick_point	pointer to reference tick
 * @param label_coords	pointer to label coordinates output
 */
static void scale_get_label_coords(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc, lv_point_t * tick_point, lv_area_t * label_coords)
{
	lv_scale_t * scale = (lv_scale_t *)obj;

    /* Reserve appropriate size for the tick label */
    lv_point_t label_size;
    lv_txt_get_size(&label_size, label_dsc->text,
                    label_dsc->font, label_dsc->letter_space, label_dsc->line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

    /* Set the label draw area at some distance of the major tick */
    if((LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) || (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {
        label_coords->x1 = (tick_point->x - label_size.x / 2U);
        label_coords->x2 = (tick_point->x + label_size.x / 2U);

        if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
            label_coords->y1 = tick_point->y + lv_obj_get_style_pad_bottom(obj, LV_PART_INDICATOR);
            label_coords->y2 = label_coords->y1 + label_size.y;
        }
        else {
            label_coords->y2 = tick_point->y - lv_obj_get_style_pad_top(obj, LV_PART_INDICATOR);
            label_coords->y1 = label_coords->y2 - label_size.y;
        }
    }
    else if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) || (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)) {
        label_coords->y1 = (tick_point->y - label_size.y / 2);
        label_coords->y2 = (tick_point->y + label_size.y / 2);

        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            label_coords->x1 = tick_point->x - label_size.x - lv_obj_get_style_pad_left(obj, LV_PART_INDICATOR);
            label_coords->x2 = tick_point->x - lv_obj_get_style_pad_left(obj, LV_PART_INDICATOR);
        }
        else {
            label_coords->x1 = tick_point->x + lv_obj_get_style_pad_right(obj, LV_PART_INDICATOR);
            label_coords->x2 = tick_point->x + label_size.x + lv_obj_get_style_pad_right(obj, LV_PART_INDICATOR);
        }
    }
    else { /* TODO: Add support for round modes */ }
}

/**
 * Set item (minor tick) line properties
 *
 * Checks if the item has a custom section configuration or not and sets the properties accordingly.
 *
 * @param obj       pointer to a scale object
 * @param line_dsc  pointer to line descriptor
 * @param items_section_style  pointer to items section style
 */
static void scale_set_items_properties(lv_obj_t * obj, lv_draw_line_dsc_t * line_dsc, lv_style_t * items_section_style)
{
    if(items_section_style) {
        lv_style_value_t value;
        lv_res_t res;

        /* Tick width */
        res = lv_style_get_prop(items_section_style, LV_STYLE_LINE_WIDTH, &value);
        if(res == LV_RES_OK) {
            line_dsc->width = (lv_coord_t)value.num;
        }
        else {
            line_dsc->width = lv_obj_get_style_line_width(obj, LV_PART_ITEMS);
        }

        /* Tick color */
        res = lv_style_get_prop(items_section_style, LV_STYLE_LINE_COLOR, &value);
        if(res == LV_RES_OK) {
            line_dsc->color = value.color;
        }
        else {
            line_dsc->color = lv_obj_get_style_line_color(obj, LV_PART_ITEMS);
        }

        /* Tick opa */
        res = lv_style_get_prop(items_section_style, LV_STYLE_LINE_OPA, &value);
        if(res == LV_RES_OK) {
            line_dsc->opa = (lv_opa_t)value.num;
        }
        else {
            line_dsc->opa = lv_obj_get_style_line_opa(obj, LV_PART_ITEMS);
        }

        /* Tick gap */

    }
    else {
        line_dsc->color = lv_obj_get_style_line_color(obj, LV_PART_ITEMS);
        line_dsc->opa = lv_obj_get_style_line_opa(obj, LV_PART_ITEMS);
        line_dsc->width = lv_obj_get_style_line_width(obj, LV_PART_ITEMS);
    }
}

/**
 * Set indicator label properties
 *
 * Checks if the indicator has a custom section configuration or not and sets the properties accordingly.
 *
 * @param obj       pointer to a scale object
 * @param label_dsc  pointer to label descriptor
 * @param items_section_style  pointer to indicator section style
 */
static void scale_set_indicator_label_properties(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc,
                                                 lv_style_t * indicator_section_style)
{
    if(indicator_section_style) {
        lv_style_value_t value;
        lv_res_t res;

        /* Text color */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_COLOR, &value);
        if(res == LV_RES_OK) {
            label_dsc->color = value.color;
        }
        else {
            label_dsc->color = lv_obj_get_style_text_color(obj, LV_PART_INDICATOR);
        }

        /* Text opa */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_OPA, &value);
        if(res == LV_RES_OK) {
            label_dsc->opa = (lv_opa_t)value.num;
        }
        else {
            label_dsc->opa = lv_obj_get_style_text_opa(obj, LV_PART_INDICATOR);
        }

        /* Text letter space */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_LETTER_SPACE, &value);
        if(res == LV_RES_OK) {
            label_dsc->letter_space = (lv_coord_t)value.num;
        }
        else {
            label_dsc->letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_INDICATOR);
        }

        /* Text font */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_FONT, &value);
        if(res == LV_RES_OK) {
            label_dsc->font = (const lv_font_t *)value.ptr;
        }
        else {
            label_dsc->font = lv_obj_get_style_text_font(obj, LV_PART_INDICATOR);
        }
    }
    else {
        /* If label is not within a range then get the indicator style */
        label_dsc->color = lv_obj_get_style_text_color(obj, LV_PART_INDICATOR);
        label_dsc->opa = lv_obj_get_style_text_opa(obj, LV_PART_INDICATOR);
        label_dsc->letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_INDICATOR);
        label_dsc->font = lv_obj_get_style_text_font(obj, LV_PART_INDICATOR);
    }
}

/**
 * Set indicator line properties
 *
 * Checks if the indicator has a custom section configuration or not and sets the properties accordingly.
 *
 * @param obj       pointer to a scale object
 * @param line_dsc  pointer to line descriptor
 * @param items_section_style  pointer to indicator section style
 */
static void scale_set_indicator_line_properties(lv_obj_t * obj, lv_draw_line_dsc_t * line_dsc,
                                                lv_style_t * indicator_section_style)
{
    if(indicator_section_style) {
        lv_style_value_t value;
        lv_res_t res;

        /* Tick width */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_LINE_WIDTH, &value);
        if(res == LV_RES_OK) {
            line_dsc->width = (lv_coord_t)value.num;
        }
        else {
            line_dsc->width = lv_obj_get_style_line_width(obj, LV_PART_INDICATOR);
        }

        /* Tick color */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_LINE_COLOR, &value);
        if(res == LV_RES_OK) {
            line_dsc->color = value.color;
        }
        else {
            line_dsc->color = lv_obj_get_style_line_color(obj, LV_PART_INDICATOR);
        }

        /* Tick opa */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_LINE_OPA, &value);
        if(res == LV_RES_OK) {
            line_dsc->opa = (lv_opa_t)value.num;
        }
        else {
            line_dsc->opa = lv_obj_get_style_line_opa(obj, LV_PART_INDICATOR);
        }
    }
    else {
        line_dsc->color = lv_obj_get_style_line_color(obj, LV_PART_INDICATOR);
        line_dsc->opa = lv_obj_get_style_line_opa(obj, LV_PART_INDICATOR);
        line_dsc->width = lv_obj_get_style_line_width(obj, LV_PART_INDICATOR);
    }
}

static void scale_find_section_tick_idx(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;

    const int32_t min_out = scale->range_min;
    const int32_t max_out = scale->range_max;
    const uint16_t total_tick_count = scale->total_tick_count;

    /* Section handling */
    uint8_t tick_idx = 0;
    for(tick_idx = 0; tick_idx <= total_tick_count; tick_idx++) {
        bool is_major_tick = false;
        if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;

        const int32_t tick_value = lv_map(tick_idx, 0U, total_tick_count, min_out, max_out);

        lv_scale_section_t * section;
        _LV_LL_READ_BACK(&scale->section_ll, section) {
            if(section->minor_range <= tick_value && section->major_range >= tick_value) {
                if(section->first_tick_idx_in_section == 255) {
                    section->first_tick_idx_in_section = tick_idx;
                    section->first_tick_idx_is_major = is_major_tick;
                }
                if(section->first_tick_idx_in_section != tick_idx) {
                    section->last_tick_idx_in_section = tick_idx;
                    section->last_tick_idx_is_major = is_major_tick;
                }
            }
            else { /* Nothing to do */ }
        }
    }
}

#endif
