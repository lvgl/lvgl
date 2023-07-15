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

static void scale_draw_indicator_round(lv_obj_t * obj, lv_event_t * event);

static void get_center(const lv_obj_t * obj, lv_point_t * center, lv_coord_t * arc_r);

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

void lv_scale_set_round_props(lv_obj_t * obj, uint16_t angle_range, int16_t rotation)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->angle_range = angle_range;
    scale->rotation = rotation;

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
    scale->mode = LV_SCALE_MODE_ROUND_INNER;
    scale->label_enabled = LV_SCALE_LABEL_ENABLED_DEFAULT;
    scale->angle_range = 270U; /* TODO: Replace with symbol? */
    scale->rotation = 90 + (360 - scale->angle_range) / 2; /* TODO: Replace magic numbers */

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

    	scale_draw_main(obj, event);
    	scale_draw_items(obj, event);

        if(((LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode))
           || ((LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) || (LV_SCALE_MODE_VERTICAL_LEFT == scale->mode))) {
            scale_draw_indicator(obj, event);
        }
        else if((LV_SCALE_MODE_ROUND_INNER == scale->mode) || (LV_SCALE_MODE_ROUND_OUTTER == scale->mode)) {
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

static void scale_get_minor_tick_points(lv_obj_t * obj, lv_draw_line_dsc_t * line_dsc, const uint16_t tick_idx, lv_point_t * tick_point_a, lv_point_t * tick_point_b)
{
	lv_scale_t * scale = (lv_scale_t *)obj;

    if ((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
		|| (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {
		/* Get style properties so they can be used in the tick and label drawing */
		const lv_coord_t height = (lv_coord_t) lv_obj_get_content_height(obj);
		const lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
		const lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
		const lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
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

		lv_coord_t minor_len = lv_obj_get_style_width(obj, LV_PART_ITEMS);

		/* Draw tick lines to the right */
		if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
			minor_len *= -1;
		}
		/* Draw tick lines to the top */
		else if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
			minor_len *= -1;
		}
		else { /* Nothing to do */ }

    	lv_coord_t tick_length = minor_len;

		/* Setup the tick points */
		if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
			lv_coord_t vertical_position = y_ofs + (int32_t)((int32_t)(height - line_dsc->width) *
															 (scale->total_tick_count - tick_idx)) / scale->total_tick_count;

			tick_point_a->x = x_ofs - 1U; /* Move extra pixel out of scale boundary */
			tick_point_a->y = vertical_position;
			tick_point_b->x = tick_point_a->x - tick_length;
			tick_point_b->y = vertical_position;
		}
		else {
			lv_coord_t horizontal_position = x_ofs + (int32_t)((int32_t)(height - line_dsc->width) * tick_idx) / scale->total_tick_count;

			tick_point_a->x = horizontal_position;
			tick_point_a->y = y_ofs;
			tick_point_b->x = horizontal_position;
			tick_point_b->y = tick_point_a->y + tick_length;
		}
    }
    else if (LV_SCALE_MODE_ROUND_OUTTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        lv_area_t scale_area;
    	lv_obj_get_content_coords(obj, &scale_area);

    	/* Find the center of the scale */
    	lv_point_t center_point;
    	const lv_coord_t radius_edge = LV_MIN(lv_area_get_width(&scale_area) / 2U, lv_area_get_height(&scale_area) / 2U);
    	center_point.x = scale_area.x1 + radius_edge;
    	center_point.y = scale_area.y1 + radius_edge;

    	lv_coord_t radius_in_minor = radius_edge - lv_obj_get_style_width(obj, LV_PART_ITEMS);

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

static void scale_draw_items(lv_obj_t * obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    if(scale->total_tick_count <= 1) return;

    /* Configure line draw descriptor for the minor tick drawing */
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &line_dsc);

	const int32_t min_out = scale->range_min;
	const int32_t max_out = scale->range_max;
    const uint16_t total_tick_count = scale->total_tick_count;

    uint8_t tick_idx = 0;
    for(tick_idx = 0; tick_idx < total_tick_count; tick_idx++) {
        /* A major tick is the one which has a label in it */
        bool is_major_tick = false;
        if(tick_idx % scale->major_tick_every == 0) is_major_tick = true;
        if(is_major_tick) continue;

        const int32_t tick_value = lv_map(tick_idx, 0U, total_tick_count, min_out, max_out);

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

        lv_point_t tick_point_a;
        lv_point_t tick_point_b;
        scale_get_minor_tick_points(obj, &line_dsc, tick_idx, &tick_point_a, &tick_point_b);

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
        	lv_snprintf(text_buffer, sizeof(text_buffer), "%" LV_PRId32, tick_value);
            label_dsc.text = text_buffer;
        }

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

        if (label_dsc.text) {
            /* Reserve appropriate size for the tick label */
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

            lv_draw_label(layer, &label_dsc, &label_coords);
        }

        line_dsc.p1 = tick_point_a;
        line_dsc.p2 = tick_point_b;
        lv_draw_line(layer, &line_dsc);
    }
}

static void scale_draw_main(lv_obj_t * obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    if(scale->total_tick_count <= 1) return;

    if ((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
		|| (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {
		/* Get style properties so they can be used in the main line drawing */
		const lv_coord_t height = (lv_coord_t) lv_obj_get_content_height(obj);
		const lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
		const lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
		const lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + lv_obj_get_style_border_width(obj, LV_PART_MAIN);
		const lv_coord_t scroll_top = lv_obj_get_scroll_top(obj);
		const lv_coord_t scroll_left = lv_obj_get_scroll_left(obj);

		lv_coord_t x_ofs = 0U;
		lv_coord_t y_ofs = 0U;

		if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
			x_ofs = obj->coords.x1;
			y_ofs = obj->coords.y1 + pad_top + border_width - scroll_top;
		}
		else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
			x_ofs = obj->coords.x2;
			y_ofs = obj->coords.y1 + pad_top + border_width - scroll_top;
		}
		if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
			x_ofs = obj->coords.x1 + pad_left - scroll_left;
			y_ofs = obj->coords.y2;
		}
		else if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
			x_ofs = obj->coords.x1 + pad_left - scroll_left;
			y_ofs = obj->coords.y1;
		}
		else { /* Nothing to do */ }

		/* Configure both line and label draw descriptors for the tick and label drawings */
		lv_draw_line_dsc_t line_dsc;
		lv_draw_line_dsc_init(&line_dsc);
		lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

		lv_point_t main_line_point_a;
		lv_point_t main_line_point_b;

		/* Setup the tick points */
		if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
			lv_coord_t vertical_position_a = y_ofs + + (((int32_t)((int32_t)(height - line_dsc.width))) /
					scale->total_tick_count);
			lv_coord_t vertical_position_b = y_ofs + (((int32_t)((int32_t)(height - line_dsc.width) * (scale->total_tick_count))) /
										   scale->total_tick_count);

			main_line_point_a.x = x_ofs - 1U;
			main_line_point_a.y = vertical_position_a;
			main_line_point_b.x = x_ofs - 1U;
			main_line_point_b.y = vertical_position_b;
		}
		else {
			lv_coord_t horizontal_position_a = x_ofs;
			lv_coord_t horizontal_position_b = x_ofs + ((int32_t)((int32_t)(height - line_dsc.width) * (scale->total_tick_count - 1U)) /
											 scale->total_tick_count);
			main_line_point_a.x = horizontal_position_a;
			main_line_point_a.y = y_ofs;
			main_line_point_b.x = horizontal_position_b;
			main_line_point_b.y = y_ofs;
		}

		/* Draw vertical line that covers all the ticks */
		line_dsc.p1 = main_line_point_a;
		line_dsc.p2 = main_line_point_b;
		lv_draw_line(layer, &line_dsc);
    }
    else if (LV_SCALE_MODE_ROUND_OUTTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        /* Configure arc draw descriptors for the main part */
        lv_draw_arc_dsc_t arc_dsc;
        lv_draw_arc_dsc_init(&arc_dsc);
        lv_obj_init_draw_arc_dsc(obj, LV_PART_MAIN, &arc_dsc);

        lv_point_t arc_center;
        lv_coord_t arc_radius;
        get_center(obj, &arc_center, &arc_radius);

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

		uint32_t radius_text = radius_in_major - label_gap;

		lv_point_t point;
		point.x = center_point.x + radius_text;
		point.y = center_point.y;
		lv_point_transform(&point, angle_upscale, 256, &center_point);

		char buf[16] = {0U};
        int32_t tick_value = 0U;
        int32_t min_out = scale->range_min;
        int32_t max_out = scale->range_max;
        tick_value = lv_map(tick_idx, 0U, scale->total_tick_count, min_out, max_out);

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

        if (label_dsc.text) {
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

#endif
