/**
 * @file lv_slider.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_slider.h"
#if LV_USE_SLIDER != 0

#include "../misc/lv_assert.h"
#include "../core/lv_group.h"
#include "../core/lv_indev.h"
#include "../draw/lv_draw.h"
#include "../misc/lv_math.h"
#include "../core/lv_disp.h"
#include "lv_img.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_slider_class

#define LV_SLIDER_KNOB_COORD(hor, is_rtl, area) (hor ? (is_rtl ? area.x1 : area.x2) : (is_rtl ? area.y1 : area.y2))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_slider_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_slider_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void position_knob(lv_obj_t * obj, lv_area_t * knob_area, lv_coord_t knob_size, bool hor);
static void draw_knob(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_slider_class = {
    .constructor_cb = lv_slider_constructor,
    .event_cb = lv_slider_event,
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .instance_size = sizeof(lv_slider_t),
    .base_class = &lv_bar_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_slider_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin")
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

bool lv_slider_is_dragged(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_slider_t * slider = (lv_slider_t *)obj;

    return slider->dragging ? true : false;
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
    slider->dragging = 0;
    slider->left_knob_focus = 0;

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_ext_click_area(obj, LV_DPX(8));
}

static void lv_slider_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_slider_t * slider = (lv_slider_t *)obj;
    lv_slider_mode_t type = lv_slider_get_mode(obj);

    /*Advanced hit testing: react only on dragging the knob(s)*/
    if(code == LV_EVENT_HIT_TEST) {
        lv_hit_test_info_t * info = lv_event_get_param(e);

        /*Ordinary slider: was the knob area hit?*/
        info->res = _lv_area_is_point_on(&slider->right_knob_area, info->point, 0);

        /*There's still a change we have a hit, if we have another knob*/
        if((info->res == false) && (type == LV_SLIDER_MODE_RANGE)) {
            info->res = _lv_area_is_point_on(&slider->left_knob_area, info->point, 0);
        }
    }
    else if(code == LV_EVENT_PRESSED) {
        lv_obj_invalidate(obj);

        lv_point_t p;
        slider->dragging = true;
        if(type == LV_SLIDER_MODE_NORMAL || type == LV_SLIDER_MODE_SYMMETRICAL) {
            slider->value_to_set = &slider->bar.cur_value;
        }
        else if(type == LV_SLIDER_MODE_RANGE) {
            lv_indev_get_point(lv_indev_get_act(), &p);
            bool hor = lv_obj_get_width(obj) >= lv_obj_get_height(obj);
            lv_base_dir_t base_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);

            lv_coord_t dist_left, dist_right;
            if(hor) {
                if((base_dir != LV_BASE_DIR_RTL && p.x > slider->right_knob_area.x2) || (base_dir == LV_BASE_DIR_RTL &&
                                                                                      p.x < slider->right_knob_area.x1)) {
                    slider->value_to_set = &slider->bar.cur_value;
                }
                else if((base_dir != LV_BASE_DIR_RTL && p.x < slider->left_knob_area.x1) || (base_dir == LV_BASE_DIR_RTL &&
                                                                                          p.x > slider->left_knob_area.x2)) {
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
                if(p.y < slider->right_knob_area.y1) {
                    slider->value_to_set = &slider->bar.cur_value;
                }
                else if(p.y > slider->left_knob_area.y2) {
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
    else if(code == LV_EVENT_PRESSING && slider->value_to_set != NULL) {
        lv_indev_t * indev = lv_indev_get_act();
        if(lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER) return;

        lv_point_t p;
        lv_indev_get_point(indev, &p);
        lv_base_dir_t base_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);

        lv_coord_t w = lv_obj_get_width(obj);
        lv_coord_t h = lv_obj_get_height(obj);

        lv_coord_t bg_left = lv_obj_get_style_pad_left(obj,   LV_PART_MAIN);
        lv_coord_t bg_right = lv_obj_get_style_pad_right(obj,  LV_PART_MAIN);
        lv_coord_t bg_top = lv_obj_get_style_pad_top(obj,    LV_PART_MAIN);
        lv_coord_t bg_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

        int32_t range = slider->bar.max_value - slider->bar.min_value;
        int32_t new_value = 0;
        int32_t real_max_value = slider->bar.max_value;
        int32_t real_min_value = slider->bar.min_value;

        if(w >= h) {
            lv_coord_t indic_w = w - bg_left - bg_right;
            if(base_dir == LV_BASE_DIR_RTL) {
                new_value = (obj->coords.x2 - bg_right) - p.x; /*Make the point relative to the indicator*/
            }
            else {
                new_value = p.x - (obj->coords.x1 + bg_left); /*Make the point relative to the indicator*/
            }
            new_value = (new_value * range) / indic_w;
            new_value += slider->bar.min_value;
        }
        else {
            lv_coord_t indic_h = h - bg_bottom - bg_top;
            new_value = p.y - (obj->coords.y2 + bg_bottom); /*Make the point relative to the indicator*/
            new_value = (-new_value * range) / indic_h;
            new_value += slider->bar.min_value;

        }

        /*Figure out the min. and max. for this mode*/
        if(slider->value_to_set == &slider->bar.start_value) {
            real_max_value = slider->bar.cur_value;
        }
        else {
            real_min_value = slider->bar.start_value;
        }

        if(new_value < real_min_value) new_value = real_min_value;
        else if(new_value > real_max_value) new_value = real_max_value;
        if(*slider->value_to_set != new_value) {
            *slider->value_to_set = new_value;
            lv_obj_invalidate(obj);
            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }

    }
    else if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        slider->dragging = false;
        slider->value_to_set = NULL;

        lv_obj_invalidate(obj);

        /*Leave edit mode if released. (No need to wait for LONG_PRESS)*/
        lv_group_t * g   = lv_obj_get_group(obj);
        bool editing     = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
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

    }
    else if(code == LV_EVENT_FOCUSED) {
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        if(indev_type == LV_INDEV_TYPE_ENCODER || indev_type == LV_INDEV_TYPE_KEYPAD) {
            slider->left_knob_focus = 0;
        }
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        lv_obj_refresh_ext_draw_size(obj);
    }
    else if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t knob_left = lv_obj_get_style_pad_left(obj, LV_PART_KNOB);
        lv_coord_t knob_right = lv_obj_get_style_pad_right(obj,LV_PART_KNOB);
        lv_coord_t knob_top = lv_obj_get_style_pad_top(obj, LV_PART_KNOB);
        lv_coord_t knob_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

        /*The smaller size is the knob diameter*/
        lv_coord_t zoom = lv_obj_get_style_transform_zoom(obj, LV_PART_KNOB);
        lv_coord_t trans_w = lv_obj_get_style_transform_width(obj, LV_PART_KNOB);
        lv_coord_t trans_h = lv_obj_get_style_transform_height(obj, LV_PART_KNOB);
        lv_coord_t knob_size = LV_MIN(lv_obj_get_width(obj) + 2 * trans_w, lv_obj_get_height(obj) + 2 * trans_h) >> 1;
        knob_size = (knob_size * zoom) >> 8;
        knob_size += LV_MAX(LV_MAX(knob_left, knob_right), LV_MAX(knob_bottom, knob_top));
        knob_size += 2;         /*For rounding error*/
        knob_size += lv_obj_calculate_ext_draw_size(obj, LV_PART_KNOB);

        /*Indic. size is handled by bar*/
        lv_coord_t * s = lv_event_get_param(e);
        *s  = LV_MAX(*s, knob_size);

    }
    else if(code == LV_EVENT_KEY) {
        char c = *((char *)lv_event_get_param(e));

        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            if(!slider->left_knob_focus) lv_slider_set_value(obj, lv_slider_get_value(obj) + 1, LV_ANIM_ON);
            else lv_slider_set_left_value(obj, lv_slider_get_left_value(obj) + 1, LV_ANIM_ON);

            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            if(!slider->left_knob_focus) lv_slider_set_value(obj, lv_slider_get_value(obj) - 1, LV_ANIM_ON);
            else lv_slider_set_left_value(obj, lv_slider_get_left_value(obj) - 1, LV_ANIM_ON);

            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }
    } else if(code == LV_EVENT_DRAW_MAIN) {
        draw_knob(e);
    }
}

static void draw_knob(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_slider_t * slider = (lv_slider_t *)obj;
    const lv_area_t * clip_area = lv_event_get_param(e);
    lv_base_dir_t base_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);

    lv_coord_t objw = lv_obj_get_width(obj);
    lv_coord_t objh = lv_obj_get_height(obj);
    bool hor = objw >= objh ? true : false;
    lv_coord_t knob_size = hor ? objh : objw;
    bool sym = false;
    if(slider->bar.mode == LV_BAR_MODE_SYMMETRICAL && slider->bar.min_value < 0 && slider->bar.max_value > 0) sym = true;

    lv_area_t knob_area;

    /*Horizontal*/
    if(hor) {
        if(!sym) {
            knob_area.x1 = LV_SLIDER_KNOB_COORD(hor, base_dir == LV_BASE_DIR_RTL, slider->bar.indic_area);
        }
        else {
            if(slider->bar.cur_value >= 0) {
                knob_area.x1 = LV_SLIDER_KNOB_COORD(hor, base_dir == LV_BASE_DIR_RTL, slider->bar.indic_area);
            }
            else {
                knob_area.x1 = LV_SLIDER_KNOB_COORD(hor, base_dir != LV_BASE_DIR_RTL, slider->bar.indic_area);
            }
        }
    }
    /*Vertical*/
    else {
        if(!sym) {
            knob_area.y1 = slider->bar.indic_area.y1;
        }
        else {
            if(slider->bar.cur_value >= 0) {
                knob_area.y1 = slider->bar.indic_area.y1;
            }
            else {
                knob_area.y1 = slider->bar.indic_area.y2;
            }
        }
    }

    lv_draw_rect_dsc_t knob_rect_dsc;
    lv_draw_rect_dsc_init(&knob_rect_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_KNOB, &knob_rect_dsc);

    position_knob(obj, &knob_area, knob_size, hor);
    lv_area_copy(&slider->right_knob_area, &knob_area);

    lv_obj_draw_part_dsc_t dsc;
    lv_obj_draw_dsc_init(&dsc, clip_area);
    dsc.part = LV_PART_KNOB;
    dsc.id = 0;
    dsc.draw_area = &slider->right_knob_area;
    dsc.rect_dsc = &knob_rect_dsc;

    if(lv_slider_get_mode(obj) != LV_SLIDER_MODE_RANGE) {
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &dsc);
        lv_draw_rect(&slider->right_knob_area, clip_area, &knob_rect_dsc);
        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &dsc);
    } else {
        /*Save the draw dsc. because it can be modified in the event*/
        lv_draw_rect_dsc_t knob_rect_dsc_tmp;
        lv_memcpy(&knob_rect_dsc_tmp, &knob_rect_dsc, sizeof(lv_draw_rect_dsc_t));

        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &dsc);
        lv_draw_rect(&slider->right_knob_area, clip_area, &knob_rect_dsc);
        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &dsc);

        /*Draw a second knob for the start_value side*/
        if(hor) {
            knob_area.x1 = LV_SLIDER_KNOB_COORD(hor, base_dir != LV_BASE_DIR_RTL, slider->bar.indic_area);
        }
        else {
            knob_area.y1 = slider->bar.indic_area.y2;
        }
        position_knob(obj, &knob_area, knob_size, hor);
        lv_area_copy(&slider->left_knob_area, &knob_area);

        lv_memcpy(&knob_rect_dsc, &knob_rect_dsc_tmp, sizeof(lv_draw_rect_dsc_t));
        dsc.draw_area = &slider->left_knob_area;
        dsc.rect_dsc = &knob_rect_dsc;
        dsc.id = 1;

        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &dsc);
        lv_draw_rect(&slider->left_knob_area, clip_area, &knob_rect_dsc);
        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &dsc);
    }
}

static void position_knob(lv_obj_t * obj, lv_area_t * knob_area, lv_coord_t knob_size, bool hor)
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

    lv_coord_t knob_left = lv_obj_get_style_pad_left(obj,   LV_PART_KNOB);
    lv_coord_t knob_right = lv_obj_get_style_pad_right(obj,  LV_PART_KNOB);
    lv_coord_t knob_top = lv_obj_get_style_pad_top(obj,    LV_PART_KNOB);
    lv_coord_t knob_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

    lv_coord_t transf_w = lv_obj_get_style_transform_width(obj, LV_PART_KNOB);
    lv_coord_t transf_h = lv_obj_get_style_transform_height(obj, LV_PART_KNOB);

    /*Apply the paddings on the knob area*/
    knob_area->x1 -= knob_left + transf_w;
    knob_area->x2 += knob_right + transf_w;
    knob_area->y1 -= knob_top + transf_h;
    knob_area->y2 += knob_bottom + transf_h;
}

#endif
