
/**
 * @file lv_slider.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_slider.h"
#if LV_USE_SLIDER != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_core/lv_group.h"
#include "../lv_core/lv_indev.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_math.h"
#include "../lv_core/lv_disp.h"
#include "lv_img.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_slider"

#define LV_SLIDER_KNOB_COORD(hor, is_rtl, area) (hor ? (is_rtl ? area.x1 : area.x2) : (is_rtl ? area.y1 : area.y2))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_slider_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_slider_destructor(lv_obj_t * obj);
static lv_draw_res_t lv_slider_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_slider_signal(lv_obj_t * obj, lv_signal_t sign, void * param);
static void position_knob(lv_obj_t * obj, lv_area_t * knob_area, lv_coord_t knob_size, bool hor);
static void draw_knob(lv_obj_t * obj, const lv_area_t * clip_area);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_slider = {
    .constructor = lv_slider_constructor,
    .destructor = lv_slider_destructor,
    .signal_cb = lv_slider_signal,
    .draw_cb = lv_slider_draw,
    .editable = 1,
    .instance_size = sizeof(lv_slider_t),
    .base_class = &lv_bar
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a slider objects
 * @param parent pointer to an object, it will be the parent of the new slider
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other slider to copy.
 * @return pointer to the created slider
 */
lv_obj_t * lv_slider_create(lv_obj_t * parent, const lv_obj_t * copy)
{

    return lv_obj_create_from_class(&lv_slider, parent, copy);
}

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/**
 * Give the slider is being dragged or not
 * @param slider pointer to a slider object
 * @return true: drag in progress false: not dragged
 */
bool lv_slider_is_dragged(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_slider_t * slider = (lv_slider_t *)obj;

    return slider->dragging ? true : false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_slider_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    lv_obj_construct_base(obj, parent, copy);
    lv_slider_t * slider = (lv_slider_t *)obj;

    /*Initialize the allocated 'slider' */
    slider->value_to_set = NULL;
    slider->dragging = 0;
    slider->left_knob_focus = 0;

    /*Init the new slider slider*/
    if(copy == NULL) {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_ext_click_area(obj, lv_dpx(5), lv_dpx(5), lv_dpx(5), lv_dpx(5));
        lv_obj_set_height(obj, LV_DPI / 15);
    } else {
        lv_slider_t * copy_slider = (lv_slider_t * ) copy;
        lv_area_copy(&slider->left_knob_area, &copy_slider->left_knob_area);
        lv_area_copy(&slider->right_knob_area, &copy_slider->right_knob_area);
    }

}

static void lv_slider_destructor(lv_obj_t * obj)
{
//    lv_slider_t * slider = obj;
//
//    _lv_obj_reset_style_list_no_refr(obj, LV_PART_KNOB);
//
//    slider->class_p->base_p->destructor(obj);
}

/**
 * Handle the drawing related tasks of the sliders
 * @param slider pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DRAW_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DRAW_DRAW: draw the object (always return 'true')
 *             LV_DRAW_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_draw_res_t`
 */
static lv_draw_res_t lv_slider_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DRAW_MODE_COVER_CHECK) {
        return LV_DRAW_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {
        /* The ancestor draw function will draw the background and the indicator.
         * It also sets slider->bar.indic_area*/
        lv_bar.draw_cb(obj, clip_area, mode);

        draw_knob(obj, clip_area);

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DRAW_MODE_POST_DRAW) {
        return lv_bar.draw_cb(obj, clip_area, mode);
    }

    return LV_DRAW_RES_OK;
}

/**
 * Signal function of the slider
 * @param slider pointer to a slider object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_slider_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = lv_bar.signal_cb(obj, sign, param);
    if(res != LV_RES_OK) return res;

    lv_slider_t * slider = (lv_slider_t *)obj;
    lv_slider_type_t type = lv_slider_get_type(obj);

    /* Advanced hit testing: react only on dragging the knob(s) */
    if(sign == LV_SIGNAL_HIT_TEST) {
        lv_hit_test_info_t * info = param;

        /* Ordinary slider: was the knob area hit? */
        info->result = _lv_area_is_point_on(&slider->right_knob_area, info->point, 0);

        /* There's still a change we have a hit, if we have another knob */
        if((info->result == false) && (type == LV_SLIDER_TYPE_RANGE)) {
            info->result = _lv_area_is_point_on(&slider->left_knob_area, info->point, 0);
        }
    }

    lv_point_t p;

    if(sign == LV_SIGNAL_PRESSED) {
        slider->dragging = true;
        if(type == LV_SLIDER_TYPE_NORMAL || type == LV_SLIDER_TYPE_SYMMETRICAL) {
            slider->value_to_set = &slider->bar.cur_value;
        }
        else if(type == LV_SLIDER_TYPE_RANGE) {
            lv_indev_get_point(param, &p);
            bool hor = lv_obj_get_width(obj) >= lv_obj_get_height(obj);
            lv_bidi_dir_t base_dir = lv_obj_get_base_dir(obj);

            lv_coord_t dist_left, dist_right;
            if(hor) {
                if((base_dir != LV_BIDI_DIR_RTL && p.x > slider->right_knob_area.x2) || (base_dir == LV_BIDI_DIR_RTL &&
                                                                                      p.x < slider->right_knob_area.x1)) {
                    slider->value_to_set = &slider->bar.cur_value;
                }
                else if((base_dir != LV_BIDI_DIR_RTL && p.x < slider->left_knob_area.x1) || (base_dir == LV_BIDI_DIR_RTL &&
                                                                                          p.x > slider->left_knob_area.x2)) {
                    slider->value_to_set = &slider->bar.start_value;
                }
                else {
                    /* Calculate the distance from each knob */
                    dist_left = LV_ABS((slider->left_knob_area.x1 + (slider->left_knob_area.x2 - slider->left_knob_area.x1) / 2) - p.x);
                    dist_right = LV_ABS((slider->right_knob_area.x1 + (slider->right_knob_area.x2 - slider->right_knob_area.x1) / 2) - p.x);

                    /* Use whichever one is closer */
                    if(dist_right < dist_left)slider->value_to_set = &slider->bar.cur_value;
                    else slider->value_to_set = &slider->bar.start_value;
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
                    /* Calculate the distance from each knob */
                    dist_left = LV_ABS((slider->left_knob_area.y1 + (slider->left_knob_area.y2 - slider->left_knob_area.y1) / 2) - p.y);
                    dist_right = LV_ABS((slider->right_knob_area.y1 + (slider->right_knob_area.y2 - slider->right_knob_area.y1) / 2) - p.y);

                    /* Use whichever one is closer */
                    if(dist_right < dist_left)slider->value_to_set = &slider->bar.cur_value;
                    else slider->value_to_set = &slider->bar.start_value;
                }
            }
        }
    }
    else if(sign == LV_SIGNAL_PRESSING && slider->value_to_set != NULL) {
        if(lv_indev_get_type(param) != LV_INDEV_TYPE_POINTER) return res;

        lv_indev_get_point(param, &p);
        lv_bidi_dir_t base_dir = lv_obj_get_base_dir(obj);

        lv_coord_t w = lv_obj_get_width(obj);
        lv_coord_t h = lv_obj_get_height(obj);

        lv_coord_t bg_left = lv_obj_get_style_pad_left(obj,   LV_PART_MAIN);
        lv_coord_t bg_right = lv_obj_get_style_pad_right(obj,  LV_PART_MAIN);
        lv_coord_t bg_top = lv_obj_get_style_pad_top(obj,    LV_PART_MAIN);
        lv_coord_t bg_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

        int32_t range = slider->bar.max_value - slider->bar.min_value;
        int16_t new_value = 0;
        int16_t real_max_value = slider->bar.max_value;
        int16_t real_min_value = slider->bar.min_value;

        if(w >= h) {
            lv_coord_t indic_w = w - bg_left - bg_right;
            if(base_dir == LV_BIDI_DIR_RTL) {
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

        /* Figure out the min. and max. for this mode */
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
            if(res != LV_RES_OK) return res;
        }

    }
    else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        slider->dragging = false;
        slider->value_to_set = NULL;

#if LV_USE_GROUP
        /*Leave edit mode if released. (No need to wait for LONG_PRESS) */
        lv_group_t * g             = lv_obj_get_group(slider);
        bool editing               = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            if(editing) {
                if(lv_slider_get_type(slider) == LV_SLIDER_TYPE_RANGE) {
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
#endif

    }
    else if(sign == LV_SIGNAL_FOCUS) {
        slider->left_knob_focus = 0;
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {
        /* The knob size depends on slider size.
         * During the drawing method the obj. size is used by the knob so refresh the obj. size.*/
        if(lv_obj_get_width(obj) != lv_area_get_width(param) ||
           lv_obj_get_height(obj) != lv_area_get_height(param)) {
            lv_obj_refresh_ext_draw_size(obj);
        }
    }
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_SIZE) {
        lv_coord_t knob_left = lv_obj_get_style_pad_left(obj, LV_PART_KNOB);
        lv_coord_t knob_right = lv_obj_get_style_pad_right(obj,LV_PART_KNOB);
        lv_coord_t knob_top = lv_obj_get_style_pad_top(obj, LV_PART_KNOB);
        lv_coord_t knob_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

        /* The smaller size is the knob diameter*/
        lv_coord_t trans_w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
        lv_coord_t trans_h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
        lv_coord_t knob_size = LV_MIN(lv_obj_get_width(obj) + 2 * trans_w, lv_obj_get_height(obj) + 2 * trans_h) >> 1;
        knob_size += LV_MAX(LV_MAX(knob_left, knob_right), LV_MAX(knob_bottom, knob_top));
        knob_size += 2;         /*For rounding error*/

        knob_size += lv_obj_calculate_ext_draw_size(obj, LV_PART_KNOB);

        /*Indic. size is handled by bar*/
        lv_coord_t * s = param;
        *s  = LV_MAX(*s, knob_size);

    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        char c = *((char *)param);

        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            if(!slider->left_knob_focus) lv_slider_set_value(slider, lv_slider_get_value(slider) + 1, LV_ANIM_ON);
            else lv_slider_set_left_value(slider, lv_slider_get_left_value(slider) + 1, LV_ANIM_ON);

            res = lv_event_send(slider, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            if(!slider->left_knob_focus) lv_slider_set_value(slider, lv_slider_get_value(slider) - 1, LV_ANIM_ON);
            else lv_slider_set_left_value(slider, lv_slider_get_left_value(slider) - 1, LV_ANIM_ON);

            res = lv_event_send(slider, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
#endif
    }

    return res;
}

void draw_knob(lv_obj_t * obj, const lv_area_t * clip_area)
{
    lv_slider_t * slider = (lv_slider_t *)obj;
    lv_bidi_dir_t base_dir = lv_obj_get_base_dir(obj);

    lv_coord_t objw = lv_obj_get_width(obj);
    lv_coord_t objh = lv_obj_get_height(obj);
    bool hor = objw >= objh ? true : false;
    lv_coord_t knob_size = hor ? objh : objw;
    bool sym = false;
    if(slider->bar.type == LV_BAR_TYPE_SYMMETRICAL && slider->bar.min_value < 0 && slider->bar.max_value > 0) sym = true;

    lv_area_t knob_area;

    /*Horizontal*/
    if(hor) {
        if(!sym) {
            knob_area.x1 = LV_SLIDER_KNOB_COORD(hor, base_dir == LV_BIDI_DIR_RTL, slider->bar.indic_area);
        }
        else {
            if(slider->bar.cur_value >= 0) {
                knob_area.x1 = LV_SLIDER_KNOB_COORD(hor, base_dir == LV_BIDI_DIR_RTL, slider->bar.indic_area);
            }
            else {
                knob_area.x1 = LV_SLIDER_KNOB_COORD(hor, base_dir != LV_BIDI_DIR_RTL, slider->bar.indic_area);
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

    /*Handle custom drawer*/
    lv_draw_rect(&slider->right_knob_area, clip_area, &knob_rect_dsc);

    if(lv_slider_get_type(obj) == LV_SLIDER_TYPE_RANGE) {
        /* Draw a second knob for the start_value side */
        if(hor) {
            knob_area.x1 = LV_SLIDER_KNOB_COORD(hor, base_dir != LV_BIDI_DIR_RTL, slider->bar.indic_area);
        }
        else {
            knob_area.y1 = slider->bar.indic_area.y2;
        }
        position_knob(obj, &knob_area, knob_size, hor);

        lv_area_copy(&slider->left_knob_area, &knob_area);

        /*Draw the knob if the custom drawer allows it*/
        lv_draw_rect(&slider->left_knob_area, clip_area, &knob_rect_dsc);
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
