
/**
 * @file lv_slider.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_slider.h"
#if LV_USE_SLIDER != 0

#include "../lv_core/lv_debug.h"
#include "../lv_core/lv_group.h"
#include "../lv_core/lv_indev.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_math.h"
#include "lv_img.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_slider"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_slider_design(lv_obj_t * slider, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_slider_signal(lv_obj_t * slider, lv_signal_t sign, void * param);
static lv_style_dsc_t * lv_slider_get_style(lv_obj_t * slider, uint8_t part);
static void lv_slider_position_knob(lv_obj_t * slider, lv_area_t * knob_area, lv_coord_t knob_size, bool hor);
static void lv_slider_draw_knob(lv_obj_t * slider, const lv_area_t * knob_area, const lv_area_t * clip_area);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_cb_t ancestor_design_f;
static lv_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a slider objects
 * @param par pointer to an object, it will be the parent of the new slider
 * @param copy pointer to a slider object, if not NULL then the new object will be copied from it
 * @return pointer to the created slider
 */
lv_obj_t * lv_slider_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("slider create started");

    /*Create the ancestor slider*/
    lv_obj_t * new_slider = lv_bar_create(par, copy);
    LV_ASSERT_MEM(new_slider);
    if(new_slider == NULL) return NULL;

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_cb(new_slider);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_slider);

    /*Allocate the slider type specific extended data*/
    lv_slider_ext_t * ext = lv_obj_allocate_ext_attr(new_slider, sizeof(lv_slider_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(new_slider);
        return NULL;
    }

    /*Initialize the allocated 'ext' */
    lv_style_dsc_init(&ext->style_knob);
    ext->value_to_set = NULL;
    ext->dragging = false;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_slider, lv_slider_signal);
    lv_obj_set_design_cb(new_slider, lv_slider_design);

    /*Init the new slider slider*/
    if(copy == NULL) {
        lv_obj_set_click(new_slider, true);
        lv_obj_set_protect(new_slider, LV_PROTECT_PRESS_LOST);

        _ot(new_slider, LV_SLIDER_PART_KNOB, SLIDER_KNOB);
    }
    /*Copy an existing slider*/
    else {
//        lv_slider_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    }

    LV_LOG_INFO("slider created");

    return new_slider;
}

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a slider
 * @param slider pointer to a slider object
 * @return the value of the slider
 */
int16_t lv_slider_get_value(const lv_obj_t * slider)
{
    LV_ASSERT_OBJ(slider, LV_OBJX_NAME);
    return lv_bar_get_value(slider);
}

/**
 * Give the slider is being dragged or not
 * @param slider pointer to a slider object
 * @return true: drag in progress false: not dragged
 */
bool lv_slider_is_dragged(const lv_obj_t * slider)
{
    LV_ASSERT_OBJ(slider, LV_OBJX_NAME);

    lv_slider_ext_t * ext = lv_obj_get_ext_attr(slider);
    return ext->dragging ? true : false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the sliders
 * @param slider pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_slider_design(lv_obj_t * slider, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return LV_DESIGN_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {

        /*The ancestor design function will draw the background and the indicator.
         * It also sets ext->bar.indic_area*/
        ancestor_design_f(slider, clip_area, mode);

        lv_slider_ext_t * ext = lv_obj_get_ext_attr(slider);

        lv_coord_t objw = lv_obj_get_width(slider);
        lv_coord_t objh = lv_obj_get_height(slider);
        bool hor = objw >= objh ? true : false;
        lv_coord_t knob_size = hor ? objh : objw;
        bool sym = false;
        if(ext->bar.type == LV_BAR_TYPE_SYM && ext->bar.min_value < 0 && ext->bar.max_value > 0) sym = true;

        lv_area_t knob_area;

        /*Horizontal*/
        if(hor) {
            if(!sym) {
                knob_area.x1 = ext->bar.indic_area.x2;
            } else {
                if(ext->bar.cur_value >= 0) {
                    knob_area.x1 = ext->bar.indic_area.x2;
                } else {
                    knob_area.x1 = ext->bar.indic_area.x1;
                }
            }
        }
        /*Vertical*/
        else {
           if(!sym) {
               knob_area.y1 = ext->bar.indic_area.y1;
           } else {
               if(ext->bar.cur_value >= 0) {
                   knob_area.y1 = ext->bar.indic_area.y1;
               } else {
                   knob_area.y1 = ext->bar.indic_area.y2;
               }
           }
        }
		lv_slider_position_knob(slider, &knob_area, knob_size, hor);

		lv_area_copy(&ext->left_knob_area, &knob_area);
        lv_slider_draw_knob(slider, &knob_area, clip_area);

        if(lv_slider_get_type(slider) == LV_SLIDER_TYPE_RANGE) {
			/* Draw a second knob for the start_value side */
			if(hor) {
                knob_area.x1 = ext->bar.indic_area.x1;
			} else {
				knob_area.y1 = ext->bar.indic_area.y1;
			}
			lv_slider_position_knob(slider, &knob_area, knob_size, hor);

			lv_area_copy(&ext->right_knob_area, &knob_area);
			lv_slider_draw_knob(slider, &knob_area, clip_area);
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        return ancestor_design_f(slider, clip_area, mode);
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the slider
 * @param slider pointer to a slider object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_slider_signal(lv_obj_t * slider, lv_signal_t sign, void * param)
{
    lv_res_t res;

    if(sign == LV_SIGNAL_GET_STYLE) {
        uint8_t ** type_p = param;
        lv_style_dsc_t ** style_dsc_p = param;
        *style_dsc_p = lv_slider_get_style(slider, **type_p);
        return LV_RES_OK;
    }

    /* Include the ancient signal function */
    res = ancestor_signal(slider, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_slider_ext_t * ext = lv_obj_get_ext_attr(slider);
    lv_point_t p;

    if(sign == LV_SIGNAL_PRESSED) {
        ext->dragging = true;
		if(lv_slider_get_type(slider) == LV_SLIDER_TYPE_RANGE) {
			lv_indev_get_point(param, &p);
			bool hor = lv_obj_get_width(slider) >= lv_obj_get_height(slider);

			/* Calculate the distance from each knob */
			lv_coord_t dist_left, dist_right;
			if(hor) {
				dist_left = LV_MATH_ABS((ext->left_knob_area.x1+(ext->left_knob_area.x2 - ext->left_knob_area.x1)/2) - p.x);
				dist_right = LV_MATH_ABS((ext->right_knob_area.x1+(ext->right_knob_area.x2 - ext->right_knob_area.x1)/2) - p.x);
			} else {
				dist_left = LV_MATH_ABS((ext->left_knob_area.y1+(ext->left_knob_area.y2 - ext->left_knob_area.y1)/2) - p.y);
				dist_right = LV_MATH_ABS((ext->right_knob_area.y1+(ext->right_knob_area.y2 - ext->right_knob_area.y1)/2) - p.y);
			}

			/* Use whichever one is closer */
			if(dist_right > dist_left)
				ext->value_to_set = &ext->bar.cur_value;
			else
				ext->value_to_set = &ext->bar.start_value;
		} else
			ext->value_to_set = &ext->bar.cur_value;
    } else if(sign == LV_SIGNAL_PRESSING && ext->value_to_set != NULL) {
        lv_indev_get_point(param, &p);

        lv_coord_t w = lv_obj_get_width(slider);
        lv_coord_t h = lv_obj_get_height(slider);

        lv_style_int_t bg_left = lv_obj_get_style_int(slider,   LV_SLIDER_PART_BG, LV_STYLE_PAD_LEFT);
        lv_style_int_t bg_right = lv_obj_get_style_int(slider,  LV_SLIDER_PART_BG, LV_STYLE_PAD_RIGHT);
        lv_style_int_t bg_top = lv_obj_get_style_int(slider,    LV_SLIDER_PART_BG, LV_STYLE_PAD_TOP);
        lv_style_int_t bg_bottom = lv_obj_get_style_int(slider, LV_SLIDER_PART_BG, LV_STYLE_PAD_BOTTOM);

        int32_t range = ext->bar.max_value - ext->bar.min_value;
        int16_t new_value = 0, real_max_value = ext->bar.max_value, real_min_value = ext->bar.min_value;

	    if(w >= h) {
	        lv_coord_t indic_w = w - bg_left - bg_right;
	        new_value = p.x - (slider->coords.x1 + bg_left); /*Make the point relative to the indicator*/
	        new_value = (new_value * range) / indic_w;
	        new_value += ext->bar.min_value;
	    } else {
	        lv_coord_t indic_h = h - bg_bottom - bg_top;
	        new_value = p.y - (slider->coords.y2 + bg_bottom); /*Make the point relative to the indicator*/
	        new_value = (-new_value * range) / indic_h;
	        new_value += ext->bar.min_value;

	    }

		/* Figure out the min. and max. for this mode */
		if(ext->value_to_set == &ext->bar.start_value) {
			real_max_value = ext->bar.cur_value;
		} else
			real_min_value = ext->bar.start_value;

		if(new_value < real_min_value) new_value = real_min_value;
	    else if(new_value > real_max_value) new_value = real_max_value;

	    if(new_value != ext->bar.cur_value) {
			*ext->value_to_set = new_value;
			lv_obj_invalidate(slider);
			res = lv_event_send(slider, LV_EVENT_VALUE_CHANGED, NULL);
	    	if(res != LV_RES_OK) return res;
	    }
        
    } else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        ext->dragging = false;
		ext->value_to_set = NULL;

#if LV_USE_GROUP
        /*Leave edit mode if released. (No need to wait for LONG_PRESS) */
        lv_group_t * g             = lv_obj_get_group(slider);
        bool editing               = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            if(editing) lv_group_set_editing(g, false);
        }
#endif

    } else if(sign == LV_SIGNAL_COORD_CHG) {
        /* The knob size depends on slider size.
         * During the drawing method the ext. size is used by the knob so refresh the ext. size.*/
        if(lv_obj_get_width(slider) != lv_area_get_width(param) ||
           lv_obj_get_height(slider) != lv_area_get_height(param)) {
            slider->signal_cb(slider, LV_SIGNAL_REFR_EXT_DRAW_PAD, NULL);
        }
    } else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        lv_style_int_t knob_left = lv_obj_get_style_int(slider,   LV_SLIDER_PART_KNOB, LV_STYLE_PAD_LEFT);
        lv_style_int_t knob_right = lv_obj_get_style_int(slider,  LV_SLIDER_PART_KNOB, LV_STYLE_PAD_RIGHT);
        lv_style_int_t knob_top = lv_obj_get_style_int(slider,    LV_SLIDER_PART_KNOB, LV_STYLE_PAD_TOP);
        lv_style_int_t knob_bottom = lv_obj_get_style_int(slider, LV_SLIDER_PART_KNOB, LV_STYLE_PAD_BOTTOM);

        /* The smaller size is the knob diameter*/
        lv_coord_t knob_size = LV_MATH_MIN(lv_obj_get_width(slider), lv_obj_get_height(slider)) >> 1;
        knob_size += LV_MATH_MAX(LV_MATH_MAX(knob_left, knob_right), LV_MATH_MAX(knob_bottom,knob_top));

        lv_style_int_t knob_sh_width = lv_obj_get_style_int(slider, LV_SLIDER_PART_KNOB, LV_STYLE_SHADOW_WIDTH);
        lv_style_int_t knob_sh_spread = lv_obj_get_style_int(slider, LV_SLIDER_PART_KNOB, LV_STYLE_SHADOW_SPREAD);
        lv_style_int_t knob_sh_ox = lv_obj_get_style_int(slider, LV_SLIDER_PART_KNOB, LV_STYLE_SHADOW_OFFSET_X);
        lv_style_int_t knob_sh_oy = lv_obj_get_style_int(slider, LV_SLIDER_PART_KNOB, LV_STYLE_SHADOW_OFFSET_Y);

        knob_size += knob_sh_width + knob_sh_spread;
        knob_size += LV_MATH_MAX(LV_MATH_ABS(knob_sh_ox), LV_MATH_ABS(knob_sh_oy));

        lv_style_int_t bg_sh_width = lv_obj_get_style_int(slider, LV_SLIDER_PART_BG, LV_STYLE_SHADOW_WIDTH);
        lv_style_int_t bg_sh_spread = lv_obj_get_style_int(slider, LV_SLIDER_PART_BG, LV_STYLE_SHADOW_SPREAD);
        lv_style_int_t bg_sh_ox = lv_obj_get_style_int(slider, LV_SLIDER_PART_BG, LV_STYLE_SHADOW_OFFSET_X);
        lv_style_int_t bg_sh_oy = lv_obj_get_style_int(slider, LV_SLIDER_PART_BG, LV_STYLE_SHADOW_OFFSET_Y);

        lv_coord_t bg_size = bg_sh_width + bg_sh_spread;
        bg_size += LV_MATH_MAX(LV_MATH_ABS(bg_sh_ox), LV_MATH_ABS(bg_sh_oy));

        lv_style_int_t indic_sh_width = lv_obj_get_style_int(slider, LV_SLIDER_PART_INDIC, LV_STYLE_SHADOW_WIDTH);
        lv_style_int_t indic_sh_spread = lv_obj_get_style_int(slider, LV_SLIDER_PART_INDIC, LV_STYLE_SHADOW_SPREAD);
        lv_style_int_t indic_sh_ox = lv_obj_get_style_int(slider, LV_SLIDER_PART_INDIC, LV_STYLE_SHADOW_OFFSET_X);
        lv_style_int_t indic_sh_oy = lv_obj_get_style_int(slider, LV_SLIDER_PART_INDIC, LV_STYLE_SHADOW_OFFSET_Y);

        lv_coord_t indic_size = indic_sh_width + indic_sh_spread;
        indic_size += LV_MATH_MAX(LV_MATH_ABS(indic_sh_ox), LV_MATH_ABS(indic_sh_oy));

        slider->ext_draw_pad = LV_MATH_MAX(slider->ext_draw_pad, knob_size);
        slider->ext_draw_pad = LV_MATH_MAX(slider->ext_draw_pad, indic_size);
        slider->ext_draw_pad = LV_MATH_MAX(slider->ext_draw_pad, bg_size);

    } else if(sign == LV_SIGNAL_CONTROL) {
        char c = *((char *)param);

        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            lv_slider_set_value(slider, lv_slider_get_value(slider) + 1, true);
            res = lv_event_send(slider, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        } else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            lv_slider_set_value(slider, lv_slider_get_value(slider) - 1, true);
            res = lv_event_send(slider, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    } else if(sign == LV_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = true;
    }

    return res;
}


static lv_style_dsc_t * lv_slider_get_style(lv_obj_t * slider, uint8_t part)
{
    LV_ASSERT_OBJ(slider, LV_OBJX_NAME);

    lv_slider_ext_t * ext = lv_obj_get_ext_attr(slider);
    lv_style_dsc_t * style_dsc_p;

    switch(part) {
    case LV_SLIDER_PART_BG:
        style_dsc_p = &slider->style_dsc;
        break;
    case LV_SLIDER_PART_INDIC:
        style_dsc_p = &ext->bar.style_indic;
        break;
    case LV_SLIDER_PART_KNOB:
        style_dsc_p = &ext->style_knob;
        break;
    default:
        style_dsc_p = NULL;
    }

    return style_dsc_p;
}

static void lv_slider_position_knob(lv_obj_t * slider, lv_area_t * knob_area, lv_coord_t knob_size, bool hor)
{

	if(hor) {
		knob_area->x1 -= (knob_size >> 1);
        knob_area->x2 = knob_area->x1 + knob_size;
        knob_area->y1 = slider->coords.y1;
        knob_area->y2 = slider->coords.y2;
	} else {
		knob_area->y1 -= (knob_size >> 1);
		knob_area->y2 = knob_area->y1 + knob_size;
		knob_area->x1 = slider->coords.x1;
		knob_area->x2 = slider->coords.x2;
	}

    lv_style_int_t knob_left = lv_obj_get_style_int(slider,   LV_SLIDER_PART_KNOB, LV_STYLE_PAD_LEFT);
    lv_style_int_t knob_right = lv_obj_get_style_int(slider,  LV_SLIDER_PART_KNOB, LV_STYLE_PAD_RIGHT);
    lv_style_int_t knob_top = lv_obj_get_style_int(slider,    LV_SLIDER_PART_KNOB, LV_STYLE_PAD_TOP);
    lv_style_int_t knob_bottom = lv_obj_get_style_int(slider, LV_SLIDER_PART_KNOB, LV_STYLE_PAD_BOTTOM);

	/*Apply the paddings on the knob area*/
    knob_area->x1 -= knob_left;
    knob_area->x2 += knob_right;
    knob_area->y1 -= knob_top;
    knob_area->y2 += knob_bottom;
}

static void lv_slider_draw_knob(lv_obj_t * slider, const lv_area_t * knob_area, const lv_area_t * clip_area)
{
    lv_slider_ext_t * ext = lv_obj_get_ext_attr(slider);

    lv_draw_rect_dsc_t knob_rect_dsc;
    lv_draw_rect_dsc_init(&knob_rect_dsc);
    lv_obj_init_draw_rect_dsc(slider, LV_SLIDER_PART_KNOB, &knob_rect_dsc);

    lv_draw_rect(knob_area, clip_area, &knob_rect_dsc);
}
#endif
