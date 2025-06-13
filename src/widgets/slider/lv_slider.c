/**
 * @file lv_slider.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_slider_private.h"
#include "../../misc/lv_area_private.h"
#include "../../core/lv_obj_private.h"
#include "../../core/lv_obj_event_private.h"
#include "../../core/lv_obj_class_private.h"
#if LV_USE_SLIDER != 0

#include "../../misc/lv_assert.h"
#include "../../core/lv_group.h"
#include "../../indev/lv_indev.h"
#include "../../indev/lv_indev_private.h"
#include "../../display/lv_display.h"
#include "../../draw/lv_draw.h"
#include "../../stdlib/lv_string.h"
#include "../../misc/lv_math.h"
#include "../image/lv_image.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_slider_class)

#define LV_SLIDER_KNOB_COORD(is_reversed, area) (is_reversed ? area.x1 : area.x2)
#define LV_SLIDER_KNOB_COORD_VERTICAL(is_reversed, area) (is_reversed ? area.y2 : area.y1)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_slider_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_slider_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void position_knob(lv_obj_t * obj, lv_area_t * knob_area, const int32_t knob_size, const bool hor);
static void draw_knob(lv_event_t * e);
static bool is_slider_horizontal(lv_obj_t * obj);
static void drag_start(lv_obj_t * obj);
static void update_knob_pos(lv_obj_t * obj, bool check_drag);

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_OBJ_PROPERTY
static const lv_property_ops_t properties[] = {
    {
        .id = LV_PROPERTY_SLIDER_VALUE,
        .setter = lv_slider_set_value,
        .getter = lv_slider_get_value,
    },
    {
        .id = LV_PROPERTY_SLIDER_LEFT_VALUE,
        .setter = lv_slider_set_start_value,
        .getter = lv_slider_get_left_value,
    },
    {
        .id = LV_PROPERTY_SLIDER_RANGE,
        .setter = lv_slider_set_range,
        .getter = NULL,
    },
    {
        .id = LV_PROPERTY_SLIDER_MIN_VALUE,
        .setter = NULL,
        .getter = lv_slider_get_min_value,
    },
    {
        .id = LV_PROPERTY_SLIDER_MAX_VALUE,
        .setter = NULL,
        .getter = lv_slider_get_max_value,
    },
    {
        .id = LV_PROPERTY_SLIDER_MODE,
        .setter = lv_slider_set_mode,
        .getter = lv_slider_get_mode,
    },
    {
        .id = LV_PROPERTY_SLIDER_IS_DRAGGED,
        .setter = NULL,
        .getter = lv_slider_is_dragged,
    },
    {
        .id = LV_PROPERTY_SLIDER_IS_SYMMETRICAL,
        .setter = NULL,
        .getter = lv_slider_is_symmetrical,
    },
};
#endif

const lv_obj_class_t lv_slider_class = {
    .constructor_cb = lv_slider_constructor,
    .event_cb = lv_slider_event,
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .instance_size = sizeof(lv_slider_t),
    .base_class = &lv_bar_class,
    .name = "lv_slider",
#if LV_USE_OBJ_PROPERTY
    .prop_index_start = LV_PROPERTY_SLIDER_START,
    .prop_index_end = LV_PROPERTY_SLIDER_END,
    .properties = properties,
    .properties_count = sizeof(properties) / sizeof(properties[0]),

#if LV_USE_OBJ_PROPERTY_NAME
    .property_names = lv_slider_property_names,
    .names_count = sizeof(lv_slider_property_names) / sizeof(lv_property_name_t),
#endif
#endif
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_slider_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

bool lv_slider_is_dragged(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_slider_t * slider = (lv_slider_t *)obj;

    return slider->dragging;
}

void lv_slider_set_value(lv_obj_t * obj, int32_t value, lv_anim_enable_t anim)
{
    lv_bar_set_value(obj, value, anim);
}

void lv_slider_set_start_value(lv_obj_t * obj, int32_t value, lv_anim_enable_t anim)
{
    lv_bar_set_start_value(obj, value, anim);
}

void lv_slider_set_range(lv_obj_t * obj, int32_t min, int32_t max)
{
    lv_bar_set_range(obj, min, max);
}

void lv_slider_set_min_value(lv_obj_t * obj, int32_t min)
{
    lv_bar_set_min_value(obj, min);
}

void lv_slider_set_max_value(lv_obj_t * obj, int32_t min)
{
    lv_bar_set_max_value(obj, min);
}

void lv_slider_set_mode(lv_obj_t * obj, lv_slider_mode_t mode)
{
    lv_bar_set_mode(obj, (lv_bar_mode_t)mode);
}

void lv_slider_set_orientation(lv_obj_t * obj, lv_slider_orientation_t orientation)
{
    lv_bar_set_orientation(obj, (lv_bar_orientation_t)orientation);
}

int32_t lv_slider_get_value(const lv_obj_t * obj)
{
    return lv_bar_get_value(obj);
}

int32_t lv_slider_get_left_value(const lv_obj_t * obj)
{
    return lv_bar_get_start_value(obj);
}

int32_t lv_slider_get_min_value(const lv_obj_t * obj)
{
    return lv_bar_get_min_value(obj);
}

int32_t lv_slider_get_max_value(const lv_obj_t * obj)
{
    return lv_bar_get_max_value(obj);
}

lv_slider_mode_t lv_slider_get_mode(lv_obj_t * slider)
{
    lv_bar_mode_t mode = lv_bar_get_mode(slider);
    if(mode == LV_BAR_MODE_SYMMETRICAL) return LV_SLIDER_MODE_SYMMETRICAL;
    else if(mode == LV_BAR_MODE_RANGE) return LV_SLIDER_MODE_RANGE;
    else return LV_SLIDER_MODE_NORMAL;
}

lv_slider_orientation_t lv_slider_get_orientation(lv_obj_t * slider)
{
    lv_bar_orientation_t ori = lv_bar_get_orientation(slider);
    if(ori == LV_BAR_ORIENTATION_HORIZONTAL) return LV_SLIDER_ORIENTATION_HORIZONTAL;
    else if(ori == LV_BAR_ORIENTATION_VERTICAL) return LV_SLIDER_ORIENTATION_VERTICAL;
    else return LV_SLIDER_ORIENTATION_AUTO;
}

bool lv_slider_is_symmetrical(lv_obj_t * obj)
{
    return lv_bar_is_symmetrical(obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_slider_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_slider_t * slider = (lv_slider_t *)obj;

    /*Initialize the allocated 'slider'*/
    slider->value_to_set = NULL;
    slider->dragging = 0U;
    slider->left_knob_focus = 0U;

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_ext_click_area(obj, LV_DPX(8));
}

static void lv_slider_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_slider_t * slider = (lv_slider_t *)obj;
    lv_slider_mode_t type = lv_slider_get_mode(obj);

    /*Advanced hit testing: react only on dragging the knob(s)*/
    if(code == LV_EVENT_HIT_TEST) {
        lv_hit_test_info_t * info = lv_event_get_param(e);
        int32_t ext_click_area = obj->spec_attr ? obj->spec_attr->ext_click_pad : 0;

        /*Ordinary slider: was the knob area hit?*/
        lv_area_t a;
        lv_area_copy(&a, &slider->right_knob_area);
        lv_area_increase(&a, ext_click_area, ext_click_area);
        info->res = lv_area_is_point_on(&a, info->point, 0);

        /*There's still a chance that there is a hit if there is another knob*/
        if((info->res == false) && (type == LV_SLIDER_MODE_RANGE)) {
            lv_area_copy(&a, &slider->left_knob_area);
            lv_area_increase(&a, ext_click_area, ext_click_area);
            info->res = lv_area_is_point_on(&a, info->point, 0);
        }
    }
    else if(code == LV_EVENT_PRESSED) {
        /*Save the pressed coordinates*/
        lv_indev_get_point(lv_indev_active(), &slider->pressed_point);
        lv_obj_transform_point(obj, &slider->pressed_point, LV_OBJ_POINT_TRANSFORM_FLAG_INVERSE_RECURSIVE);
    }
    else if(code == LV_EVENT_PRESSING) {
        update_knob_pos(obj, true);
    }
    else if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        update_knob_pos(obj, false);
        slider->dragging = false;
        slider->value_to_set = NULL;

        lv_obj_invalidate(obj);

        /*Leave edit mode if released. (No need to wait for LONG_PRESS)*/
        lv_group_t * g   = lv_obj_get_group(obj);
        bool editing     = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_active());
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            if(editing) {
                if(lv_slider_get_mode(obj) == LV_SLIDER_MODE_RANGE) {
                    if(slider->left_knob_focus == 0) slider->left_knob_focus = 1;
                    else {
                        slider->left_knob_focus = 0;
                        lv_group_set_editing(g, false);
                    }
                }
                else {
                    lv_group_set_editing(g, false);
                }
            }
        }
        else if(indev_type == LV_INDEV_TYPE_POINTER) {
            if(is_slider_horizontal(obj)) lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_VER);
            else  lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
        }
    }
    else if(code == LV_EVENT_FOCUSED) {
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_active());
        if(indev_type == LV_INDEV_TYPE_ENCODER || indev_type == LV_INDEV_TYPE_KEYPAD) {
            slider->left_knob_focus = 0;
        }
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        if(is_slider_horizontal(obj)) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_VER);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
        }
        else {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_VER);
        }
        lv_obj_refresh_ext_draw_size(obj);
    }
    else if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        int32_t knob_left = lv_obj_get_style_pad_left(obj, LV_PART_KNOB);
        int32_t knob_right = lv_obj_get_style_pad_right(obj, LV_PART_KNOB);
        int32_t knob_top = lv_obj_get_style_pad_top(obj, LV_PART_KNOB);
        int32_t knob_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

        /*The smaller size is the knob diameter*/
        int32_t trans_w = lv_obj_get_style_transform_width(obj, LV_PART_KNOB);
        int32_t trans_h = lv_obj_get_style_transform_height(obj, LV_PART_KNOB);
        int32_t knob_size = LV_MIN(lv_obj_get_width(obj) + 2 * trans_w, lv_obj_get_height(obj) + 2 * trans_h) >> 1;
        knob_size += LV_MAX(LV_MAX(knob_left, knob_right), LV_MAX(knob_bottom, knob_top));
        knob_size += 2;         /*For rounding error*/
        knob_size += lv_obj_calculate_ext_draw_size(obj, LV_PART_KNOB);

        /*Indic. size is handled by bar*/
        int32_t * s = lv_event_get_param(e);
        *s  = LV_MAX(*s, knob_size);

    }
    else if(code == LV_EVENT_KEY) {
        uint32_t c = lv_event_get_key(e);

        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            if(!slider->left_knob_focus) lv_slider_set_value(obj, lv_slider_get_value(obj) + 1, LV_ANIM_ON);
            else lv_slider_set_start_value(obj, lv_slider_get_left_value(obj) + 1, LV_ANIM_ON);
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            if(!slider->left_knob_focus) lv_slider_set_value(obj, lv_slider_get_value(obj) - 1, LV_ANIM_ON);
            else lv_slider_set_start_value(obj, lv_slider_get_left_value(obj) - 1, LV_ANIM_ON);
        }
        else {
            return;
        }

        res = lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
        if(res != LV_RESULT_OK) return;
    }
    else if(code == LV_EVENT_ROTARY) {
        int32_t r = lv_event_get_rotary_diff(e);

        if(!slider->left_knob_focus) lv_slider_set_value(obj, lv_slider_get_value(obj) + r, LV_ANIM_ON);
        else lv_slider_set_start_value(obj, lv_slider_get_left_value(obj) + 1, LV_ANIM_ON);

        res = lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
        if(res != LV_RESULT_OK) return;
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_knob(e);
    }
}

static void draw_knob(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_slider_t * slider = (lv_slider_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(e);

    const bool is_rtl = LV_BASE_DIR_RTL == lv_obj_get_style_base_dir(obj, LV_PART_MAIN);
    const bool is_horizontal = is_slider_horizontal(obj);
    const bool is_reversed = slider->bar.val_reversed ^ (is_rtl && is_horizontal);

    lv_area_t knob_area;
    int32_t knob_size;
    bool is_symmetrical = lv_slider_is_symmetrical(obj);

    if(is_horizontal) {
        knob_size = lv_obj_get_height(obj);
        if(is_symmetrical &&
           slider->bar.cur_value < 0) knob_area.x1 = LV_SLIDER_KNOB_COORD(!is_reversed, slider->bar.indic_area);
        else knob_area.x1 = LV_SLIDER_KNOB_COORD(is_reversed, slider->bar.indic_area);
    }
    else {
        knob_size = lv_obj_get_width(obj);
        if(is_symmetrical &&
           slider->bar.cur_value < 0) knob_area.y1 =  LV_SLIDER_KNOB_COORD_VERTICAL(!is_reversed, slider->bar.indic_area);
        else knob_area.y1 = LV_SLIDER_KNOB_COORD_VERTICAL(is_reversed, slider->bar.indic_area);
    }
    lv_draw_rect_dsc_t knob_rect_dsc;
    lv_draw_rect_dsc_init(&knob_rect_dsc);
    knob_rect_dsc.base.layer = layer;
    lv_obj_init_draw_rect_dsc(obj, LV_PART_KNOB, &knob_rect_dsc);
    /* Update knob area with knob style */
    position_knob(obj, &knob_area, knob_size, is_horizontal);
    /* Update right knob area with calculated knob area */
    lv_area_copy(&slider->right_knob_area, &knob_area);

    if(lv_slider_get_mode(obj) != LV_SLIDER_MODE_RANGE) {
        lv_draw_rect(layer, &knob_rect_dsc, &slider->right_knob_area);
    }
    else {
        /*Save the draw part_draw_dsc. because it can be modified in the event*/
        lv_draw_rect_dsc_t knob_rect_dsc_tmp;
        lv_memcpy(&knob_rect_dsc_tmp, &knob_rect_dsc, sizeof(lv_draw_rect_dsc_t));
        /* Draw the right knob */
        lv_draw_rect(layer, &knob_rect_dsc, &slider->right_knob_area);

        /*Calculate the second knob area*/
        if(is_horizontal) {
            /*use !is_reversed to get the other knob*/
            knob_area.x1 = LV_SLIDER_KNOB_COORD(!is_reversed, slider->bar.indic_area);
        }
        else {
            knob_area.y1 = LV_SLIDER_KNOB_COORD_VERTICAL(!is_reversed, slider->bar.indic_area);
        }
        position_knob(obj, &knob_area, knob_size, is_horizontal);
        lv_area_copy(&slider->left_knob_area, &knob_area);

        lv_memcpy(&knob_rect_dsc, &knob_rect_dsc_tmp, sizeof(lv_draw_rect_dsc_t));

        lv_draw_rect(layer, &knob_rect_dsc, &slider->left_knob_area);
    }
}

static void position_knob(lv_obj_t * obj, lv_area_t * knob_area, const int32_t knob_size, const bool hor)
{
    if(hor) {
        knob_area->x1 -= (knob_size >> 1);
        knob_area->x2 = knob_area->x1 + knob_size - 1;
        knob_area->y1 = obj->coords.y1;
        knob_area->y2 = obj->coords.y2;
    }
    else {
        knob_area->y1 -= (knob_size >> 1);
        knob_area->y2 = knob_area->y1 + knob_size - 1;
        knob_area->x1 = obj->coords.x1;
        knob_area->x2 = obj->coords.x2;
    }

    int32_t knob_left = lv_obj_get_style_pad_left(obj, LV_PART_KNOB);
    int32_t knob_right = lv_obj_get_style_pad_right(obj, LV_PART_KNOB);
    int32_t knob_top = lv_obj_get_style_pad_top(obj, LV_PART_KNOB);
    int32_t knob_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

    int32_t transf_w = lv_obj_get_style_transform_width(obj, LV_PART_KNOB);
    int32_t transf_h = lv_obj_get_style_transform_height(obj, LV_PART_KNOB);

    /*Apply the paddings on the knob area*/
    knob_area->x1 -= knob_left + transf_w;
    knob_area->x2 += knob_right + transf_w;
    knob_area->y1 -= knob_top + transf_h;
    knob_area->y2 += knob_bottom + transf_h;
}

static bool is_slider_horizontal(lv_obj_t * obj)
{
    lv_slider_t * slider = (lv_slider_t *)obj;
    if(slider->bar.orientation == LV_BAR_ORIENTATION_AUTO) return lv_obj_get_width(obj) >= lv_obj_get_height(obj);
    else if(slider->bar.orientation == LV_BAR_ORIENTATION_HORIZONTAL) return true;
    else return false;
}

static void drag_start(lv_obj_t * obj)
{
    lv_slider_t * slider = (lv_slider_t *)obj;
    lv_slider_mode_t mode = lv_slider_get_mode(obj);
    lv_point_t p;
    slider->dragging = true;
    if(mode == LV_SLIDER_MODE_NORMAL || mode == LV_SLIDER_MODE_SYMMETRICAL) {
        slider->value_to_set = &slider->bar.cur_value;
    }
    else if(mode == LV_SLIDER_MODE_RANGE) {
        lv_indev_get_point(lv_indev_active(), &p);
        lv_obj_transform_point(obj, &p, LV_OBJ_POINT_TRANSFORM_FLAG_INVERSE_RECURSIVE);
        const bool is_rtl = LV_BASE_DIR_RTL == lv_obj_get_style_base_dir(obj, LV_PART_MAIN);
        const bool is_horizontal = is_slider_horizontal(obj);
        const bool is_reversed = slider->bar.val_reversed ^ (is_rtl && is_horizontal);
        int32_t dist_left, dist_right;
        if(is_horizontal) {
            if((!is_reversed && p.x > slider->right_knob_area.x2) || (is_reversed && p.x < slider->right_knob_area.x1)) {
                slider->value_to_set = &slider->bar.cur_value;
            }
            else if((!is_reversed && p.x < slider->left_knob_area.x1) || (is_reversed && p.x > slider->left_knob_area.x2)) {
                slider->value_to_set = &slider->bar.start_value;
            }
            else {
                /*Calculate the distance from each knob*/
                dist_left = LV_ABS((slider->left_knob_area.x1 + (slider->left_knob_area.x2 - slider->left_knob_area.x1) / 2) - p.x);
                dist_right = LV_ABS((slider->right_knob_area.x1 + (slider->right_knob_area.x2 - slider->right_knob_area.x1) / 2) - p.x);
                /*Use whichever one is closer*/
                if(dist_right < dist_left) {
                    slider->value_to_set = &slider->bar.cur_value;
                    slider->left_knob_focus = 0;
                }
                else {
                    slider->value_to_set = &slider->bar.start_value;
                    slider->left_knob_focus = 1;
                }
            }
        }
        else {
            if((!is_reversed && p.y < slider->right_knob_area.y1) || (is_reversed && p.y > slider->right_knob_area.y2)) {
                slider->value_to_set = &slider->bar.cur_value;
            }
            else if((!is_reversed && p.y > slider->left_knob_area.y2) || (is_reversed && p.y < slider->left_knob_area.y1)) {
                slider->value_to_set = &slider->bar.start_value;
            }
            else {
                /*Calculate the distance from each knob*/
                dist_left = LV_ABS((slider->left_knob_area.y1 + (slider->left_knob_area.y2 - slider->left_knob_area.y1) / 2) - p.y);
                dist_right = LV_ABS((slider->right_knob_area.y1 + (slider->right_knob_area.y2 - slider->right_knob_area.y1) / 2) - p.y);

                /*Use whichever one is closer*/
                if(dist_right < dist_left) {
                    slider->value_to_set = &slider->bar.cur_value;
                    slider->left_knob_focus = 0;
                }
                else {
                    slider->value_to_set = &slider->bar.start_value;
                    slider->left_knob_focus = 1;
                }
            }
        }
    }
}

static void update_knob_pos(lv_obj_t * obj, bool check_drag)
{
    lv_slider_t * slider = (lv_slider_t *)obj;
    lv_indev_t * indev = lv_indev_active();
    if(lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER)
        return;
    if(lv_indev_get_scroll_obj(indev) != NULL)
        return;

    lv_point_t p;
    lv_indev_get_point(indev, &p);
    lv_obj_transform_point(obj, &p, LV_OBJ_POINT_TRANSFORM_FLAG_INVERSE_RECURSIVE);

    bool is_hor = is_slider_horizontal(obj);

    if(check_drag && !slider->dragging) {
        int32_t ofs = is_hor ? (p.x - slider->pressed_point.x) : (p.y - slider->pressed_point.y);

        /*Stop processing when offset is below scroll_limit*/
        if(LV_ABS(ofs) < indev->scroll_limit) {
            return;
        }
    }

    if(!slider->value_to_set) {
        /*Ready to start drag*/
        drag_start(obj);
    }

    int32_t new_value = 0;
    const int32_t range = slider->bar.max_value - slider->bar.min_value;
    const bool is_rtl = LV_BASE_DIR_RTL == lv_obj_get_style_base_dir(obj, LV_PART_MAIN);
    const bool is_horizontal = is_slider_horizontal(obj);
    const bool is_reversed = slider->bar.val_reversed ^ (is_rtl && is_horizontal);

    if(is_hor) {
        const int32_t bg_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        const int32_t bg_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
        const int32_t w = lv_obj_get_width(obj);
        const int32_t indic_w = w - bg_left - bg_right;

        if(is_reversed) {
            /*Make the point relative to the indicator*/
            new_value = (obj->coords.x2 - bg_right) - p.x;
        }
        else {
            /*Make the point relative to the indicator*/
            new_value = p.x - (obj->coords.x1 + bg_left);
        }
        if(indic_w) {
            new_value = (new_value * range + indic_w / 2) / indic_w;
            new_value += slider->bar.min_value;
        }
    }
    else {
        const int32_t bg_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
        const int32_t bg_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
        const int32_t h = lv_obj_get_height(obj);
        const int32_t indic_h = h - bg_bottom - bg_top;

        if(is_reversed) {
            /*Make the point relative to the indicator*/
            new_value = p.y - (obj->coords.y1 + bg_top);
        }
        else {
            /*Make the point relative to the indicator*/
            new_value = p.y - (obj->coords.y2 + bg_bottom);
            new_value = -new_value;
        }
        new_value = (new_value * range + indic_h / 2) / indic_h;
        new_value += slider->bar.min_value;
    }

    int32_t real_max_value = slider->bar.max_value;
    int32_t real_min_value = slider->bar.min_value;
    /*Figure out the min. and max. for this mode*/
    if(slider->value_to_set == &slider->bar.start_value) {
        real_max_value = slider->bar.cur_value;
    }
    else {
        real_min_value = slider->bar.start_value;
    }

    new_value = LV_CLAMP(real_min_value, new_value, real_max_value);
    if(*slider->value_to_set != new_value) {
        *slider->value_to_set = new_value;
        if(is_hor)
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_VER);
        else
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);

        lv_obj_invalidate(obj);
        lv_result_t res = lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
        if(res != LV_RESULT_OK)
            return;
    }
}

#endif
