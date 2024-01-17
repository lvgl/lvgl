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

#define LV_SCALE_LABEL_TXT_LEN          (20U)
#define LV_SCALE_DEFAULT_ANGLE_RANGE    ((uint32_t) 270U)
#define LV_SCALE_DEFAULT_ROTATION       ((int32_t) 135U)
#define LV_SCALE_TICK_IDX_DEFAULT_ID    ((uint32_t) 255U)
#define LV_SCALE_DEFAULT_LABEL_GAP      ((uint32_t) 15U)

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
static void scale_draw_indicator(lv_obj_t * obj, lv_event_t * event);

static void scale_get_center(const lv_obj_t * obj, lv_point_t * center, int32_t * arc_r);
static void scale_get_tick_points(lv_obj_t * obj, const uint32_t tick_idx, bool is_major_tick,
                                  lv_point_t * tick_point_a, lv_point_t * tick_point_b);
static void scale_get_label_coords(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc, lv_point_t * tick_point,
                                   lv_area_t * label_coords);
static void scale_set_indicator_label_properties(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc,
                                                 lv_style_t * indicator_section_style);
static void scale_set_line_properties(lv_obj_t * obj, lv_draw_line_dsc_t * line_dsc, lv_style_t * section_style,
                                      uint32_t part);
static void scale_set_arc_properties(lv_obj_t * obj, lv_draw_arc_dsc_t * arc_dsc, lv_style_t * section_style);
/* Helpers */
static void scale_find_section_tick_idx(lv_obj_t * obj);
static void scale_store_main_line_tick_width_compensation(lv_obj_t * obj, const uint32_t tick_idx,
                                                          const bool is_major_tick, const int32_t major_tick_width, const int32_t minor_tick_width);
static void scale_store_section_line_tick_width_compensation(lv_obj_t * obj, const bool is_major_tick,
                                                             lv_draw_label_dsc_t * label_dsc, lv_draw_line_dsc_t * major_tick_dsc, lv_draw_line_dsc_t * minor_tick_dsc,
                                                             const int32_t tick_value, const uint8_t tick_idx, lv_point_t * tick_point_a);
static void scale_build_custom_label_text(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc,
                                          const uint16_t major_tick_idx);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_scale_class  = {
    .constructor_cb = lv_scale_constructor,
    .destructor_cb = lv_scale_destructor,
    .event_cb = lv_scale_event,
    .instance_size = sizeof(lv_scale_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .base_class = &lv_obj_class,
    .name = "scale",
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

void lv_scale_set_total_tick_count(lv_obj_t * obj, uint32_t total_tick_count)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->total_tick_count = total_tick_count;

    lv_obj_invalidate(obj);
}

void lv_scale_set_major_tick_every(lv_obj_t * obj, uint32_t major_tick_every)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->major_tick_every = major_tick_every;

    lv_obj_invalidate(obj);
}

void lv_scale_set_label_show(lv_obj_t * obj, bool show_label)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->label_enabled = show_label;

    lv_obj_invalidate(obj);
}

void lv_scale_set_range(lv_obj_t * obj, int32_t min, int32_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->range_min = min;
    scale->range_max = max;

    lv_obj_invalidate(obj);
}

void lv_scale_set_angle_range(lv_obj_t * obj, uint32_t angle_range)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->angle_range = angle_range;

    lv_obj_invalidate(obj);
}

void lv_scale_set_rotation(lv_obj_t * obj, int32_t rotation)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->rotation = rotation;

    lv_obj_invalidate(obj);
}

void lv_scale_set_line_needle_value(lv_obj_t * obj, lv_obj_t * needle_line, int32_t needle_length,
                                    int32_t value)
{
    int32_t angle;
    int32_t scale_width, scale_height;
    int32_t actual_needle_length;
    int32_t needle_length_x, needle_length_y;
    static lv_point_precise_t needle_line_points[2];

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;
    if((scale->mode != LV_SCALE_MODE_ROUND_INNER) &&
       (scale->mode != LV_SCALE_MODE_ROUND_OUTER)) {
        return;
    }

    lv_obj_align(needle_line, LV_ALIGN_TOP_LEFT, 0, 0);

    scale_width = lv_obj_get_style_width(obj, LV_PART_MAIN);
    scale_height = lv_obj_get_style_height(obj, LV_PART_MAIN);

    if(scale_width != scale_height) {
        return;
    }

    if(needle_length >= scale_width / 2) {
        actual_needle_length = scale_width / 2;
    }
    else if(needle_length >= 0) {
        actual_needle_length = needle_length;
    }
    else if(needle_length + scale_width / 2 < 0) {
        actual_needle_length = 0;
    }
    else {
        actual_needle_length = scale_width / 2 + needle_length;
    }

    if(value < scale->range_min) {
        angle = 0;
    }
    else if(value > scale->range_max) {
        angle = scale->angle_range;
    }
    else {
        angle = scale->angle_range * (value - scale->range_min) / (scale->range_max - scale->range_min);
    }

    needle_length_x = (actual_needle_length * lv_trigo_cos(scale->rotation + angle)) >> LV_TRIGO_SHIFT;
    needle_length_y = (actual_needle_length * lv_trigo_sin(scale->rotation + angle)) >> LV_TRIGO_SHIFT;

    needle_line_points[0].x = scale_width / 2;
    needle_line_points[0].y = scale_height / 2;
    needle_line_points[1].x = scale_width / 2 + needle_length_x;
    needle_line_points[1].y = scale_height / 2 + needle_length_y;

    lv_line_set_points(needle_line, needle_line_points, 2);
}

void lv_scale_set_image_needle_value(lv_obj_t * obj, lv_obj_t * needle_img, int32_t value)
{
    int32_t angle;
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;
    if((scale->mode != LV_SCALE_MODE_ROUND_INNER) &&
       (scale->mode != LV_SCALE_MODE_ROUND_OUTER)) {
        return;
    }

    if(value < scale->range_min) {
        angle = 0;
    }
    else if(value > scale->range_max) {
        angle = scale->angle_range;
    }
    else {
        angle = scale->angle_range * (value - scale->range_min) / (scale->range_max - scale->range_min);
    }

    lv_image_set_rotation(needle_img, (scale->rotation + angle) * 10);
}

void lv_scale_set_text_src(lv_obj_t * obj, const char * txt_src[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->txt_src = txt_src;
    scale->custom_label_cnt = 0;
    if(scale->txt_src) {
        int32_t idx;
        for(idx = 0; txt_src[idx]; ++idx) {
            scale->custom_label_cnt++;
        }
    }

    lv_obj_invalidate(obj);
}

void lv_scale_set_post_draw(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->post_draw = en;

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
    section->first_tick_idx_in_section = LV_SCALE_TICK_IDX_DEFAULT_ID;
    section->last_tick_idx_in_section = LV_SCALE_TICK_IDX_DEFAULT_ID;
    section->first_tick_idx_is_major = 0U;
    section->last_tick_idx_is_major = 0U;
    section->first_tick_in_section_width = 0U;
    section->last_tick_in_section_width = 0U;

    return section;
}

void lv_scale_section_set_range(lv_scale_section_t * section, int32_t minor_range, int32_t major_range)
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

lv_scale_mode_t lv_scale_get_mode(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->mode;
}

int32_t lv_scale_get_total_tick_count(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->total_tick_count;
}

int32_t lv_scale_get_major_tick_every(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->major_tick_every;
}

bool lv_scale_get_label_show(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->label_enabled;
}

uint32_t lv_scale_get_angle_range(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->angle_range;
}

int32_t lv_scale_get_range_min_value(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->range_min;
}

int32_t lv_scale_get_range_max_value(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->range_max;
}

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
    scale->angle_range = LV_SCALE_DEFAULT_ANGLE_RANGE;
    scale->rotation = LV_SCALE_DEFAULT_ROTATION;
    scale->range_min = 0U;
    scale->range_max = 100U;
    scale->last_tick_width = 0U;
    scale->first_tick_width = 0U;
    scale->post_draw = false;
    scale->custom_label_cnt = 0U;
    scale->txt_src = NULL;

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
    lv_result_t res = lv_obj_event_base(MY_CLASS, event);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t event_code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_current_target(event);
    lv_scale_t * scale = (lv_scale_t *) obj;
    LV_UNUSED(scale);

    if(event_code == LV_EVENT_DRAW_MAIN) {
        if(scale->post_draw == false) {
            scale_find_section_tick_idx(obj);
            scale_draw_indicator(obj, event);
            scale_draw_main(obj, event);
        }
    }
    if(event_code == LV_EVENT_DRAW_POST) {
        if(scale->post_draw == true) {
            scale_find_section_tick_idx(obj);
            scale_draw_indicator(obj, event);
            scale_draw_main(obj, event);
        }
    }
    else if(event_code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        /* NOTE: Extend scale draw size so the first tick label can be shown */
        lv_event_set_ext_draw_size(event, 100);
    }
    else {
        /* Nothing to do. Invalid event */
    }
}

static void scale_draw_indicator(lv_obj_t * obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    if(scale->total_tick_count <= 1) return;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    /* Formatting the labels with the configured style for LV_PART_INDICATOR */
    lv_obj_init_draw_label_dsc(obj, LV_PART_INDICATOR, &label_dsc);

    /* Major tick style */
    lv_draw_line_dsc_t major_tick_dsc;
    lv_draw_line_dsc_init(&major_tick_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &major_tick_dsc);

    /* Configure line draw descriptor for the minor tick drawing */
    lv_draw_line_dsc_t minor_tick_dsc;
    lv_draw_line_dsc_init(&minor_tick_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &minor_tick_dsc);

    /* Main line style */
    lv_draw_line_dsc_t main_line_dsc;
    lv_draw_line_dsc_init(&main_line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &main_line_dsc);

    const int32_t major_len = lv_obj_get_style_length(obj, LV_PART_INDICATOR);

    if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
       || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {

        uint32_t total_tick_count = scale->total_tick_count;
        uint32_t tick_idx = 0;
        uint32_t major_tick_idx = 0;
        for(tick_idx = 0; tick_idx < total_tick_count; tick_idx++) {
            /* A major tick is the one which has a label in it */
            bool is_major_tick = false;
            if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;
            if(is_major_tick) major_tick_idx++;

            const int32_t tick_value = lv_map(tick_idx, 0U, total_tick_count - 1, scale->range_min, scale->range_max);

            /* Overwrite label and tick properties if tick value is within section range */
            lv_scale_section_t * section;
            _LV_LL_READ_BACK(&scale->section_ll, section) {
                if(section->minor_range <= tick_value && section->major_range >= tick_value) {
                    if(is_major_tick) {
                        scale_set_indicator_label_properties(obj, &label_dsc, section->indicator_style);
                        scale_set_line_properties(obj, &major_tick_dsc, section->indicator_style, LV_PART_INDICATOR);
                    }
                    else {
                        scale_set_line_properties(obj, &minor_tick_dsc, section->items_style, LV_PART_ITEMS);
                    }
                    break;
                }
                else {
                    /* Tick is not in section, get the proper styles */
                    lv_obj_init_draw_label_dsc(obj, LV_PART_INDICATOR, &label_dsc);
                    lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &major_tick_dsc);
                    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &minor_tick_dsc);
                }
            }

            /* The tick is represented by a line. We need two points to draw it */
            lv_point_t tick_point_a;
            lv_point_t tick_point_b;
            scale_get_tick_points(obj, tick_idx, is_major_tick, &tick_point_a, &tick_point_b);

            /* Setup a label if they're enabled and we're drawing a major tick */
            if(scale->label_enabled && is_major_tick) {
                /* Label text setup */
                char text_buffer[LV_SCALE_LABEL_TXT_LEN] = {0};
                lv_area_t label_coords;

                /* Check if the custom text array has element for this major tick index */
                if(scale->txt_src) {
                    scale_build_custom_label_text(obj, &label_dsc, major_tick_idx);
                }
                else { /* Add label with mapped values */
                    lv_snprintf(text_buffer, sizeof(text_buffer), "%" LV_PRId32, tick_value);
                    label_dsc.text = text_buffer;
                    label_dsc.text_local = 1;
                }

                scale_get_label_coords(obj, &label_dsc, &tick_point_b, &label_coords);
                lv_draw_label(layer, &label_dsc, &label_coords);
            }

            /* Store initial and last tick widths to be used in the main line drawing */
            scale_store_main_line_tick_width_compensation(obj, tick_idx, is_major_tick, major_tick_dsc.width, minor_tick_dsc.width);

            /* Store the first and last section tick vertical/horizontal position */
            scale_store_section_line_tick_width_compensation(obj, is_major_tick, &label_dsc, &major_tick_dsc, &minor_tick_dsc,
                                                             tick_value, tick_idx, &tick_point_a);

            if(is_major_tick) {
                major_tick_dsc.p1 = lv_point_to_precise(&tick_point_a);
                major_tick_dsc.p2 = lv_point_to_precise(&tick_point_b);
                lv_draw_line(layer, &major_tick_dsc);
            }
            else {
                minor_tick_dsc.p1 = lv_point_to_precise(&tick_point_a);
                minor_tick_dsc.p2 = lv_point_to_precise(&tick_point_b);
                lv_draw_line(layer, &minor_tick_dsc);
            }
        }
    }
    else if(LV_SCALE_MODE_ROUND_OUTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        lv_area_t scale_area;
        lv_obj_get_content_coords(obj, &scale_area);

        /* Find the center of the scale */
        lv_point_t center_point;
        int32_t radius_edge = LV_MIN(lv_area_get_width(&scale_area) / 2U, lv_area_get_height(&scale_area) / 2U);
        center_point.x = scale_area.x1 + radius_edge;
        center_point.y = scale_area.y1 + radius_edge;

        /* Major tick */
        major_tick_dsc.raw_end = 0;

        uint32_t label_gap = LV_SCALE_DEFAULT_LABEL_GAP; /* TODO: Add to style properties */
        uint32_t tick_idx = 0;
        uint32_t major_tick_idx = 0;
        for(tick_idx = 0; tick_idx < scale->total_tick_count; tick_idx++) {
            /* A major tick is the one which has a label in it */
            bool is_major_tick = false;
            if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;
            if(is_major_tick) major_tick_idx++;

            const int32_t tick_value = lv_map(tick_idx, 0U, scale->total_tick_count - 1, scale->range_min, scale->range_max);

            /* Overwrite label and tick properties if tick value is within section range */
            lv_scale_section_t * section;
            _LV_LL_READ_BACK(&scale->section_ll, section) {
                if(section->minor_range <= tick_value && section->major_range >= tick_value) {
                    if(is_major_tick) {
                        scale_set_indicator_label_properties(obj, &label_dsc, section->indicator_style);
                        scale_set_line_properties(obj, &major_tick_dsc, section->indicator_style, LV_PART_INDICATOR);
                    }
                    else {
                        scale_set_line_properties(obj, &minor_tick_dsc, section->items_style, LV_PART_ITEMS);
                    }
                    break;
                }
                else {
                    /* Tick is not in section, get the proper styles */
                    lv_obj_init_draw_label_dsc(obj, LV_PART_INDICATOR, &label_dsc);
                    lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &major_tick_dsc);
                    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &minor_tick_dsc);
                }
            }

            /* The tick is represented by a line. We need two points to draw it */
            lv_point_t tick_point_a;
            lv_point_t tick_point_b;
            scale_get_tick_points(obj, tick_idx, is_major_tick, &tick_point_a, &tick_point_b);

            /* Setup a label if they're enabled and we're drawing a major tick */
            if(scale->label_enabled && is_major_tick) {
                /* Label text setup */
                char text_buffer[LV_SCALE_LABEL_TXT_LEN] = {0};
                lv_area_t label_coords;

                /* Check if the custom text array has element for this major tick index */
                if(scale->txt_src) {
                    scale_build_custom_label_text(obj, &label_dsc, major_tick_idx);
                }
                else { /* Add label with mapped values */
                    lv_snprintf(text_buffer, sizeof(text_buffer), "%" LV_PRId32, tick_value);
                    label_dsc.text = text_buffer;
                    label_dsc.text_local = 1;
                }

                /* Also take into consideration the letter space of the style */
                int32_t angle_upscale = ((tick_idx * scale->angle_range) * 10U) / (scale->total_tick_count - 1);
                angle_upscale += scale->rotation * 10U;

                uint32_t radius_text = 0;
                if(LV_SCALE_MODE_ROUND_INNER == scale->mode) {
                    radius_text = (radius_edge - major_len) - (label_gap + label_dsc.letter_space);
                }
                else if(LV_SCALE_MODE_ROUND_OUTER == scale->mode) {
                    radius_text = (radius_edge + major_len) + (label_gap + label_dsc.letter_space);
                }
                else { /* Nothing to do */ }

                lv_point_t point;
                point.x = center_point.x + radius_text;
                point.y = center_point.y;
                lv_point_transform(&point, angle_upscale, LV_SCALE_NONE, LV_SCALE_NONE, &center_point, false);
                scale_get_label_coords(obj, &label_dsc, &point, &label_coords);

                lv_draw_label(layer, &label_dsc, &label_coords);
            }

            /* Store initial and last tick widths to be used in the main line drawing */
            scale_store_main_line_tick_width_compensation(obj, tick_idx, is_major_tick, major_tick_dsc.width, minor_tick_dsc.width);

            if(is_major_tick) {
                major_tick_dsc.p1 = lv_point_to_precise(&tick_point_a);
                major_tick_dsc.p2 = lv_point_to_precise(&tick_point_b);
                lv_draw_line(layer, &major_tick_dsc);
            }
            else {
                minor_tick_dsc.p1 = lv_point_to_precise(&tick_point_a);
                minor_tick_dsc.p2 = lv_point_to_precise(&tick_point_b);
                lv_draw_line(layer, &minor_tick_dsc);
            }
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
        const int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
        const int32_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN) + border_width;

        int32_t x_ofs = 0U;
        int32_t y_ofs = 0U;

        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            x_ofs = obj->coords.x2 + (line_dsc.width / 2U) - pad_right;
            y_ofs = obj->coords.y1 + pad_top;
        }
        else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            x_ofs = obj->coords.x1 + (line_dsc.width / 2U) + pad_left;
            y_ofs = obj->coords.y1 + pad_top;
        }
        if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
            x_ofs = obj->coords.x1 + pad_right;
            y_ofs = obj->coords.y1 + (line_dsc.width / 2U) + pad_top;
        }
        else if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
            x_ofs = obj->coords.x1 + pad_left;
            y_ofs = obj->coords.y2 + (line_dsc.width / 2U) - pad_bottom;
        }
        else { /* Nothing to do */ }

        lv_point_t main_line_point_a;
        lv_point_t main_line_point_b;

        /* Setup the tick points */
        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            main_line_point_a.x = x_ofs - 1U;
            main_line_point_a.y = y_ofs;
            main_line_point_b.x = x_ofs - 1U;
            main_line_point_b.y = obj->coords.y2 - pad_bottom;

            /* Adjust main line with initial and last tick width */
            main_line_point_a.y -= scale->last_tick_width / 2U;
            main_line_point_b.y += scale->first_tick_width / 2U;
        }
        else {
            main_line_point_a.x = x_ofs;
            main_line_point_a.y = y_ofs;
            /* X of second point starts at the edge of the object minus the left pad */
            main_line_point_b.x = obj->coords.x2 - (pad_left);
            main_line_point_b.y = y_ofs;

            /* Adjust main line with initial and last tick width */
            main_line_point_a.x -= scale->last_tick_width / 2U;
            main_line_point_b.x += scale->first_tick_width / 2U;
        }

        line_dsc.p1 = lv_point_to_precise(&main_line_point_a);
        line_dsc.p2 = lv_point_to_precise(&main_line_point_b);
        lv_draw_line(layer, &line_dsc);

        lv_scale_section_t * section;
        _LV_LL_READ_BACK(&scale->section_ll, section) {
            lv_draw_line_dsc_t main_line_section_dsc;
            lv_draw_line_dsc_init(&main_line_section_dsc);
            lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &main_line_section_dsc);

            /* Calculate the points of the section line */
            lv_point_t main_point_a;
            lv_point_t main_point_b;

            /* Calculate the position of the section based on the ticks (first and last) index */
            if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
                /* Calculate position of the first tick in the section */
                main_point_a.x = main_line_point_a.x;
                int32_t tmp = (int32_t)(section->first_tick_in_section_width / 2U);
                main_point_a.y = section->first_tick_in_section.y + tmp;

                /* Calculate position of the last tick in the section */
                main_point_b.x = main_line_point_a.x;
                tmp = (int32_t)(section->last_tick_in_section_width / 2U);
                main_point_b.y = section->last_tick_in_section.y - tmp;
            }
            else {
                /* Calculate position of the first tick in the section */
                int32_t tmp = (int32_t)(section->first_tick_in_section_width / 2U);
                main_point_a.x = section->first_tick_in_section.x - tmp;
                main_point_a.y = main_line_point_a.y;

                /* Calculate position of the last tick in the section */
                tmp = (int32_t)(section->last_tick_in_section_width / 2U);
                main_point_b.x = section->last_tick_in_section.x + tmp;
                main_point_b.y = main_line_point_a.y;
            }

            scale_set_line_properties(obj, &main_line_section_dsc, section->main_style, LV_PART_MAIN);

            main_line_section_dsc.p1.x = main_point_a.x;
            main_line_section_dsc.p1.y = main_point_a.y;
            main_line_section_dsc.p2.x = main_point_b.x;
            main_line_section_dsc.p2.y = main_point_b.y;
            lv_draw_line(layer, &main_line_section_dsc);
        }
    }
    else if(LV_SCALE_MODE_ROUND_OUTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        /* Configure arc draw descriptors for the main part */
        lv_draw_arc_dsc_t arc_dsc;
        lv_draw_arc_dsc_init(&arc_dsc);
        lv_obj_init_draw_arc_dsc(obj, LV_PART_MAIN, &arc_dsc);

        lv_point_t arc_center;
        int32_t arc_radius;
        scale_get_center(obj, &arc_center, &arc_radius);

        /* TODO: Add compensation for the width of the first and last tick over the arc */
        const int32_t start_angle = lv_map(scale->range_min, scale->range_min, scale->range_max, scale->rotation,
                                           scale->rotation + scale->angle_range);
        const int32_t end_angle = lv_map(scale->range_max, scale->range_min, scale->range_max, scale->rotation,
                                         scale->rotation + scale->angle_range);

        arc_dsc.center = arc_center;
        arc_dsc.radius = arc_radius;
        arc_dsc.start_angle = start_angle;
        arc_dsc.end_angle = end_angle;

        lv_draw_arc(layer, &arc_dsc);

        lv_scale_section_t * section;
        _LV_LL_READ_BACK(&scale->section_ll, section) {
            lv_draw_arc_dsc_t main_arc_section_dsc;
            lv_draw_arc_dsc_init(&main_arc_section_dsc);
            lv_obj_init_draw_arc_dsc(obj, LV_PART_MAIN, &main_arc_section_dsc);

            lv_point_t section_arc_center;
            int32_t section_arc_radius;
            scale_get_center(obj, &section_arc_center, &section_arc_radius);

            /* TODO: Add compensation for the width of the first and last tick over the arc */
            const int32_t section_start_angle = lv_map(section->minor_range, scale->range_min, scale->range_max, scale->rotation,
                                                       scale->rotation + scale->angle_range);
            const int32_t section_end_angle = lv_map(section->major_range, scale->range_min, scale->range_max, scale->rotation,
                                                     scale->rotation + scale->angle_range);

            scale_set_arc_properties(obj, &main_arc_section_dsc, section->main_style);

            main_arc_section_dsc.center = section_arc_center;
            main_arc_section_dsc.radius = section_arc_radius;
            main_arc_section_dsc.start_angle = section_start_angle;
            main_arc_section_dsc.end_angle = section_end_angle;

            lv_draw_arc(layer, &main_arc_section_dsc);
        }
    }
    else { /* Nothing to do */ }
}

/**
 * Get center point and radius of scale arc
 * @param obj       pointer to a scale object
 * @param center    pointer to center
 * @param arc_r     pointer to arc radius
 */
static void scale_get_center(const lv_obj_t * obj, lv_point_t * center, int32_t * arc_r)
{
    int32_t left_bg = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    int32_t right_bg = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    int32_t top_bg = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    int32_t bottom_bg = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

    int32_t r = (LV_MIN(lv_obj_get_width(obj) - left_bg - right_bg, lv_obj_get_height(obj) - top_bg - bottom_bg)) / 2U;

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
static void scale_get_tick_points(lv_obj_t * obj, const uint32_t tick_idx, bool is_major_tick,
                                  lv_point_t * tick_point_a, lv_point_t * tick_point_b)
{
    lv_scale_t * scale = (lv_scale_t *)obj;

    /* Main line style */
    lv_draw_line_dsc_t main_line_dsc;
    lv_draw_line_dsc_init(&main_line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &main_line_dsc);

    int32_t minor_len = 0;
    int32_t major_len = 0;

    if(is_major_tick) {
        major_len = lv_obj_get_style_length(obj, LV_PART_INDICATOR);
    }
    else {
        minor_len = lv_obj_get_style_length(obj, LV_PART_ITEMS);
    }

    if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
       || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {

        /* Get style properties so they can be used in the tick and label drawing */
        const int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
        const int32_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
        const int32_t tick_pad_right = lv_obj_get_style_pad_right(obj, LV_PART_ITEMS);
        const int32_t tick_pad_left = lv_obj_get_style_pad_left(obj, LV_PART_ITEMS);
        const int32_t tick_pad_top = lv_obj_get_style_pad_top(obj, LV_PART_ITEMS);
        const int32_t tick_pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_ITEMS);

        int32_t x_ofs = 0U;
        int32_t y_ofs = 0U;

        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            x_ofs = obj->coords.x2 + (main_line_dsc.width / 2U) - pad_right;
            y_ofs = obj->coords.y1 + (pad_top + tick_pad_top);
        }
        else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            x_ofs = obj->coords.x1 + (main_line_dsc.width / 2U) + pad_left;
            y_ofs = obj->coords.y1 + (pad_top + tick_pad_top);
        }
        else if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
            x_ofs = obj->coords.x1 + (pad_right + tick_pad_right);
            y_ofs = obj->coords.y1 + (main_line_dsc.width / 2U) + pad_top;
        }
        /* LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode */
        else {
            x_ofs = obj->coords.x1 + (pad_left + tick_pad_left);
            y_ofs = obj->coords.y2 + (main_line_dsc.width / 2U) - pad_bottom;
        }

        /* Adjust length when tick will be drawn on horizontal top or vertical right scales */
        if((LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) || (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)) {
            if(is_major_tick) {
                major_len *= -1;
            }
            else {
                minor_len *= -1;
            }
        }
        else { /* Nothing to do */ }

        const int32_t tick_length = is_major_tick ? major_len : minor_len;
        /* NOTE
         * Minus 1 because tick count starts at 0
         * TODO
         * What if total_tick_count is 1? This will lead to an division by 0 further down */
        const uint32_t tmp_tick_count = scale->total_tick_count - 1U;

        /* Calculate the position of the tick points based on the mode and tick index */
        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            /* Vertical position starts at y2 of the scale main line, we start at y2 because the ticks are drawn from bottom to top */
            int32_t vertical_position = obj->coords.y2 - (pad_bottom + tick_pad_bottom);

            /* Position the last tick */
            if(tmp_tick_count == tick_idx) {
                vertical_position = y_ofs;
            }
            /* Otherwise adjust the tick position depending of its index and number of ticks on the scale */
            else if(0 != tick_idx) {
                const int32_t scale_total_height = lv_obj_get_height(obj) - (pad_top + pad_bottom + tick_pad_top + tick_pad_bottom);
                const int32_t offset = ((int32_t) tick_idx * (int32_t) scale_total_height) / (int32_t)(tmp_tick_count);
                vertical_position -= offset;
            }
            else { /* Nothing to do */ }

            tick_point_a->x = x_ofs - 1U; /* Move extra pixel out of scale boundary */
            tick_point_a->y = vertical_position;
            tick_point_b->x = tick_point_a->x - tick_length;
            tick_point_b->y = vertical_position;
        }
        else {
            /* Horizontal position starts at x1 of the scale main line */
            int32_t horizontal_position = x_ofs;

            /* Position the last tick */
            if(tmp_tick_count == tick_idx) {
                horizontal_position = obj->coords.x2 - (pad_left + tick_pad_left);
            }
            /* Otherwise adjust the tick position depending of its index and number of ticks on the scale */
            else if(0U != tick_idx) {
                const int32_t scale_total_width = lv_obj_get_width(obj) - (pad_right + pad_left + tick_pad_right + tick_pad_left);
                const int32_t offset = ((int32_t) tick_idx * (int32_t) scale_total_width) / (int32_t)(tmp_tick_count);
                horizontal_position += offset;
            }
            else { /* Nothing to do */ }

            tick_point_a->x = horizontal_position;
            tick_point_a->y = y_ofs;
            tick_point_b->x = horizontal_position;
            tick_point_b->y = tick_point_a->y + tick_length;
        }
    }
    else if(LV_SCALE_MODE_ROUND_OUTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        lv_area_t scale_area;
        lv_obj_get_content_coords(obj, &scale_area);

        /* Find the center of the scale */
        lv_point_t center_point;
        const int32_t radius_edge = LV_MIN(lv_area_get_width(&scale_area) / 2U, lv_area_get_height(&scale_area) / 2U);
        center_point.x = scale_area.x1 + radius_edge;
        center_point.y = scale_area.y1 + radius_edge;

        int32_t angle_upscale = ((tick_idx * scale->angle_range) * 10U) / (scale->total_tick_count - 1);
        angle_upscale += scale->rotation * 10U;

        /* Draw a little bit longer lines to be sure the mask will clip them correctly
         * and to get a better precision. Adding the main line width to the calculation so we don't have gaps
         * between the arc and the ticks */
        int32_t point_closer_to_arc = 0;
        int32_t adjusted_radio_with_tick_len = 0;
        if(LV_SCALE_MODE_ROUND_INNER == scale->mode) {
            point_closer_to_arc = radius_edge - main_line_dsc.width;
            adjusted_radio_with_tick_len = point_closer_to_arc - (is_major_tick ? major_len : minor_len);
        }
        /* LV_SCALE_MODE_ROUND_OUTER == scale->mode */
        else {
            point_closer_to_arc = radius_edge - main_line_dsc.width;
            adjusted_radio_with_tick_len = point_closer_to_arc + (is_major_tick ? major_len : minor_len);
        }

        tick_point_a->x = center_point.x + point_closer_to_arc;
        tick_point_a->y = center_point.y;
        lv_point_transform(tick_point_a, angle_upscale, LV_SCALE_NONE, LV_SCALE_NONE, &center_point, false);

        tick_point_b->x = center_point.x + adjusted_radio_with_tick_len;
        tick_point_b->y = center_point.y;
        lv_point_transform(tick_point_b, angle_upscale, LV_SCALE_NONE, LV_SCALE_NONE, &center_point, false);
    }
    else { /* Nothing to do */ }
}

/**
 * Get coordinates for label
 *
 * @param obj       pointer to a scale object
 * @param label_dsc pointer to label descriptor
 * @param tick_point    pointer to reference tick
 * @param label_coords  pointer to label coordinates output
 */
static void scale_get_label_coords(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc, lv_point_t * tick_point,
                                   lv_area_t * label_coords)
{
    lv_scale_t * scale = (lv_scale_t *)obj;

    /* Reserve appropriate size for the tick label */
    lv_point_t label_size;
    lv_text_get_size(&label_size, label_dsc->text,
                     label_dsc->font, label_dsc->letter_space, label_dsc->line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

    /* Set the label draw area at some distance of the major tick */
    if((LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) || (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {
        label_coords->x1 = tick_point->x - (label_size.x / 2U);
        label_coords->x2 = tick_point->x + (label_size.x / 2U);

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
        label_coords->y1 = tick_point->y - (label_size.y / 2U);
        label_coords->y2 = tick_point->y + (label_size.y / 2U);

        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            label_coords->x1 = tick_point->x - label_size.x - lv_obj_get_style_pad_left(obj, LV_PART_INDICATOR);
            label_coords->x2 = tick_point->x - lv_obj_get_style_pad_left(obj, LV_PART_INDICATOR);
        }
        else {
            label_coords->x1 = tick_point->x + lv_obj_get_style_pad_right(obj, LV_PART_INDICATOR);
            label_coords->x2 = tick_point->x + label_size.x + lv_obj_get_style_pad_right(obj, LV_PART_INDICATOR);
        }
    }
    else if(LV_SCALE_MODE_ROUND_OUTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        label_coords->x1 = tick_point->x - (label_size.x / 2U);
        label_coords->y1 = tick_point->y - (label_size.y / 2U);
        label_coords->x2 = label_coords->x1 + label_size.x;
        label_coords->y2 = label_coords->y1 + label_size.y;
    }
    else { /* Nothing to do */ }
}

/**
 * Set line properties
 *
 * Checks if the line has a custom section configuration or not and sets the properties accordingly.
 *
 * @param obj       pointer to a scale object
 * @param line_dsc  pointer to line descriptor
 * @param items_section_style  pointer to indicator section style
 * @param part      line part, example: LV_PART_INDICATOR, LV_PART_ITEMS, LV_PART_MAIN
 */
static void scale_set_line_properties(lv_obj_t * obj, lv_draw_line_dsc_t * line_dsc, lv_style_t * section_style,
                                      uint32_t part)
{
    if(section_style) {
        lv_style_value_t value;
        lv_result_t res;

        /* Line width */
        res = lv_style_get_prop(section_style, LV_STYLE_LINE_WIDTH, &value);
        if(res == LV_RESULT_OK) {
            line_dsc->width = (int32_t)value.num;
        }
        else {
            line_dsc->width = lv_obj_get_style_line_width(obj, part);
        }

        /* Line color */
        res = lv_style_get_prop(section_style, LV_STYLE_LINE_COLOR, &value);
        if(res == LV_RESULT_OK) {
            line_dsc->color = value.color;
        }
        else {
            line_dsc->color = lv_obj_get_style_line_color(obj, part);
        }

        /* Line opa */
        res = lv_style_get_prop(section_style, LV_STYLE_LINE_OPA, &value);
        if(res == LV_RESULT_OK) {
            line_dsc->opa = (lv_opa_t)value.num;
        }
        else {
            line_dsc->opa = lv_obj_get_style_line_opa(obj, part);
        }
    }
    else {
        line_dsc->color = lv_obj_get_style_line_color(obj, part);
        line_dsc->opa = lv_obj_get_style_line_opa(obj, part);
        line_dsc->width = lv_obj_get_style_line_width(obj, part);
    }
}

/**
 * Set arc properties
 *
 * Checks if the arc has a custom section configuration or not and sets the properties accordingly.
 *
 * @param obj       pointer to a scale object
 * @param line_dsc  pointer to arc descriptor
 * @param items_section_style  pointer to indicator section style
 */
static void scale_set_arc_properties(lv_obj_t * obj, lv_draw_arc_dsc_t * arc_dsc, lv_style_t * section_style)
{
    if(section_style) {
        lv_style_value_t value;
        lv_result_t res;

        /* Line width */
        res = lv_style_get_prop(section_style, LV_STYLE_ARC_WIDTH, &value);
        if(res == LV_RESULT_OK) {
            arc_dsc->width = (int32_t)value.num;
        }
        else {
            arc_dsc->width = lv_obj_get_style_line_width(obj, LV_PART_MAIN);
        }

        /* Line color */
        res = lv_style_get_prop(section_style, LV_STYLE_ARC_COLOR, &value);
        if(res == LV_RESULT_OK) {
            arc_dsc->color = value.color;
        }
        else {
            arc_dsc->color = lv_obj_get_style_line_color(obj, LV_PART_MAIN);
        }

        /* Line opa */
        res = lv_style_get_prop(section_style, LV_STYLE_ARC_OPA, &value);
        if(res == LV_RESULT_OK) {
            arc_dsc->opa = (lv_opa_t)value.num;
        }
        else {
            arc_dsc->opa = lv_obj_get_style_line_opa(obj, LV_PART_MAIN);
        }
    }
    else {
        arc_dsc->color = lv_obj_get_style_line_color(obj, LV_PART_MAIN);
        arc_dsc->opa = lv_obj_get_style_line_opa(obj, LV_PART_MAIN);
        arc_dsc->width = lv_obj_get_style_line_width(obj, LV_PART_MAIN);
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
        lv_result_t res;

        /* Text color */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_COLOR, &value);
        if(res == LV_RESULT_OK) {
            label_dsc->color = value.color;
        }
        else {
            label_dsc->color = lv_obj_get_style_text_color(obj, LV_PART_INDICATOR);
        }

        /* Text opa */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_OPA, &value);
        if(res == LV_RESULT_OK) {
            label_dsc->opa = (lv_opa_t)value.num;
        }
        else {
            label_dsc->opa = lv_obj_get_style_text_opa(obj, LV_PART_INDICATOR);
        }

        /* Text letter space */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_LETTER_SPACE, &value);
        if(res == LV_RESULT_OK) {
            label_dsc->letter_space = (int32_t)value.num;
        }
        else {
            label_dsc->letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_INDICATOR);
        }

        /* Text font */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_FONT, &value);
        if(res == LV_RESULT_OK) {
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

static void scale_find_section_tick_idx(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;

    const int32_t min_out = scale->range_min;
    const int32_t max_out = scale->range_max;
    const uint32_t total_tick_count = scale->total_tick_count;

    /* Section handling */
    uint32_t tick_idx = 0;
    for(tick_idx = 0; tick_idx < total_tick_count; tick_idx++) {
        bool is_major_tick = false;
        if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;

        const int32_t tick_value = lv_map(tick_idx, 0U, total_tick_count - 1, min_out, max_out);

        lv_scale_section_t * section;
        _LV_LL_READ_BACK(&scale->section_ll, section) {
            if(section->minor_range <= tick_value && section->major_range >= tick_value) {
                if(LV_SCALE_TICK_IDX_DEFAULT_ID == section->first_tick_idx_in_section) {
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

/**
 * Stores the width of the initial and last tick of the main line
 *
 * This width is used to compensate the main line drawing taking into consideration the widths of both ticks
 *
 * @param obj       pointer to a scale object
 * @param tick_idx  index of the current tick
 * @param is_major_tick true if tick_idx is a major tick
 * @param major_tick_width width of the major tick
 * @param minor_tick_width width of the minor tick
 */
static void scale_store_main_line_tick_width_compensation(lv_obj_t * obj, const uint32_t tick_idx,
                                                          const bool is_major_tick, const int32_t major_tick_width, const int32_t minor_tick_width)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    const bool is_first_tick = 0U == tick_idx;
    const bool is_last_tick = scale->total_tick_count == tick_idx;
    const int32_t tick_width = is_major_tick ? major_tick_width : minor_tick_width;

    /* Exit early if tick_idx is not the first nor last tick on the main line */
    if(((!is_last_tick) && (!is_first_tick))
       /* Exit early if scale mode is round. It doesn't support main line compensation */
       || ((LV_SCALE_MODE_ROUND_INNER == scale->mode) || (LV_SCALE_MODE_ROUND_OUTER == scale->mode))) {
        return;
    }

    if(is_last_tick) {
        /* Mode is vertical */
        if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) || (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)) {
            scale->last_tick_width = tick_width;
        }
        /* Mode is horizontal */
        else {
            scale->first_tick_width = tick_width;
        }
    }
    /* is_first_tick */
    else {
        /* Mode is vertical */
        if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) || (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)) {
            scale->first_tick_width = tick_width;
        }
        /* Mode is horizontal */
        else {
            scale->last_tick_width = tick_width;
        }
    }
}

/**
 * Sets the text of the tick label descriptor when using custom labels
 *
 * Sets the text pointer when valid custom label is available, otherwise set it to NULL.
 *
 * @param obj       pointer to a scale object
 * @param label_dsc pointer to the label descriptor
 * @param major_tick_idx  index of the current major tick
 */
static void scale_build_custom_label_text(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc,
                                          const uint16_t major_tick_idx)
{
    lv_scale_t * scale = (lv_scale_t *) obj;

    /* Check if the scale has valid custom labels available,
     * this avoids reading past txt_src array when the scale requires more tick labels than available */
    if(major_tick_idx <= scale->custom_label_cnt) {
        if(scale->txt_src[major_tick_idx - 1U]) {
            label_dsc->text = scale->txt_src[major_tick_idx - 1U];
            label_dsc->text_local = 0;
        }
        else {
            label_dsc->text = NULL;
        }
    }
    else {
        label_dsc->text = NULL;
    }
}

/**
 * Stores tick width compensation information for main line sections
 *
 * @param obj       pointer to a scale object
 * @param is_major_tick Indicates if tick is major or not
 * @param label_dsc pointer to the label descriptor
 * @param major_tick_dsc pointer to the major_tick_dsc
 * @param minor_tick_dsc pointer to the minor_tick_dsc
 * @param tick_value Current tick value, used to know if tick_idx belongs to a section or not
 * @param tick_idx Current tick index
 * @param tick_point_a Pointer to tick point a
 */
static void scale_store_section_line_tick_width_compensation(lv_obj_t * obj, const bool is_major_tick,
                                                             lv_draw_label_dsc_t * label_dsc, lv_draw_line_dsc_t * major_tick_dsc, lv_draw_line_dsc_t * minor_tick_dsc,
                                                             const int32_t tick_value, const uint8_t tick_idx, lv_point_t * tick_point_a)
{
    lv_scale_t * scale = (lv_scale_t *) obj;
    lv_scale_section_t * section;

    _LV_LL_READ_BACK(&scale->section_ll, section) {
        if(section->minor_range <= tick_value && section->major_range >= tick_value) {
            if(is_major_tick) {
                scale_set_indicator_label_properties(obj, label_dsc, section->indicator_style);
                scale_set_line_properties(obj, major_tick_dsc, section->indicator_style, LV_PART_INDICATOR);
            }
            else {
                scale_set_line_properties(obj, minor_tick_dsc, section->items_style, LV_PART_ITEMS);
            }
        }

        int32_t tmp_width = 0;

        if(tick_idx == section->first_tick_idx_in_section) {
            if(section->first_tick_idx_is_major) {
                tmp_width = major_tick_dsc->width;
            }
            else {
                tmp_width = minor_tick_dsc->width;
            }

            section->first_tick_in_section.y = tick_point_a->y;
            /* Add 1px as adjustment if tmp_width is odd */
            if(tmp_width & 0x01U) {
                if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
                    tmp_width += 1U;
                }
                else {
                    tmp_width -= 1U;
                }
            }
            section->first_tick_in_section_width = tmp_width;
        }
        else if(tick_idx == section->last_tick_idx_in_section) {
            if(section->last_tick_idx_is_major) {
                tmp_width = major_tick_dsc->width;
            }
            else {
                tmp_width = minor_tick_dsc->width;
            }

            section->last_tick_in_section.y = tick_point_a->y;
            /* Add 1px as adjustment if tmp_width is odd */
            if(tmp_width & 0x01U) {
                if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
                    tmp_width -= 1U;
                }
                else {
                    tmp_width += 1U;
                }
            }
            section->last_tick_in_section_width = tmp_width;
        }
        else { /* Nothing to do */ }
    }
}

#endif
