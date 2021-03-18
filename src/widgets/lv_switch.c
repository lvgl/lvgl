/**
 * @file lv_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_switch.h"

#if LV_USE_SWITCH != 0

/*Testing of dependencies*/
#if LV_USE_SLIDER == 0
    #error "lv_sw: lv_slider is required. Enable it in lv_conf.h (LV_USE_SLIDER 1)"
#endif

#include "../misc/lv_assert.h"
#include "../misc/lv_math.h"
#include "../core/lv_indev.h"
#include "../core/lv_disp.h"
#include "lv_img.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_switch_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_switch_constructor(lv_obj_t * obj, const lv_obj_t * copy);
static void lv_switch_event(lv_obj_t * obj, lv_event_t e);
static void draw_main(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_switch_class = {
    .constructor_cb = lv_switch_constructor,
    .event_cb = lv_switch_event,
    .instance_size = sizeof(lv_switch_t),
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a switch objects
 * @param parent pointer to an object, it will be the parent of the new switch
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other switch to copy.
 * @return pointer to the created switch
 */
lv_obj_t * lv_switch_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_INFO("begin")
    return lv_obj_create_from_class(&lv_switch_class, parent, copy);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_switch_constructor(lv_obj_t * obj, const lv_obj_t * copy)
{
    LV_TRACE_OBJ_CREATE("begin");

   if(copy == NULL) {
       lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
       lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
       lv_obj_set_size(obj, LV_DPX(60), LV_DPX(35));
   }

   LV_TRACE_OBJ_CREATE("finished");
}


static void lv_switch_event(lv_obj_t * obj, lv_event_t e)
{
    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, obj, e);
    if(res != LV_RES_OK) return;

    if(e == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t knob_left = lv_obj_get_style_pad_left(obj,   LV_PART_KNOB);
        lv_coord_t knob_right = lv_obj_get_style_pad_right(obj,  LV_PART_KNOB);
        lv_coord_t knob_top = lv_obj_get_style_pad_top(obj,    LV_PART_KNOB);
        lv_coord_t knob_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

        /*The smaller size is the knob diameter*/
        lv_coord_t knob_size = LV_MIN(lv_obj_get_width(obj), lv_obj_get_height(obj)) >> 1;
        knob_size += LV_MAX(LV_MAX(knob_left, knob_right), LV_MAX(knob_bottom, knob_top));
        knob_size += 2;         /*For rounding error*/

        knob_size += lv_obj_calculate_ext_draw_size(obj, LV_PART_KNOB);

        knob_size = (knob_size - lv_obj_get_height(obj)) / 2;

        lv_coord_t * s = lv_event_get_param();
        *s = LV_MAX(*s, knob_size);
        *s = LV_MAX(*s, lv_obj_calculate_ext_draw_size(obj, LV_PART_INDICATOR));
    }
    else if(e == LV_EVENT_CLICKED) {
        uint32_t v = lv_obj_get_state(obj) & LV_STATE_CHECKED ? 1 : 0;
        res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &v);
        if(res != LV_RES_OK) return;

        lv_obj_invalidate(obj);
    }
    else if(e == LV_EVENT_DRAW_MAIN) {
        draw_main(obj);
    }
}

static void draw_main(lv_obj_t * obj)
{
    const lv_area_t * clip_area = lv_event_get_param();
    lv_bidi_dir_t base_dir = lv_obj_get_base_dir(obj);

    /*Calculate the indicator area*/
    lv_coord_t bg_left = lv_obj_get_style_pad_left(obj,     LV_PART_MAIN);
    lv_coord_t bg_right = lv_obj_get_style_pad_right(obj,   LV_PART_MAIN);
    lv_coord_t bg_top = lv_obj_get_style_pad_top(obj,       LV_PART_MAIN);
    lv_coord_t bg_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

    bool chk = lv_obj_get_state(obj) & LV_STATE_CHECKED;
    /*Draw the indicator in checked state*/
    if(chk) {
        /*Respect the background's padding*/
        lv_area_t indic_area;
        lv_area_copy(&indic_area, &obj->coords);
        indic_area.x1 += bg_left;
        indic_area.x2 -= bg_right;
        indic_area.y1 += bg_top;
        indic_area.y2 -= bg_bottom;

        lv_draw_rect_dsc_t draw_indic_dsc;
        lv_draw_rect_dsc_init(&draw_indic_dsc);
        lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &draw_indic_dsc);
        lv_draw_rect(&indic_area, clip_area, &draw_indic_dsc);
    }

    /*Draw the knob*/
    lv_coord_t objh = lv_obj_get_height(obj);
    lv_coord_t knob_size = objh;
    lv_area_t knob_area;

    /*Left*/
    if((base_dir != LV_BIDI_DIR_RTL && !chk) || (base_dir == LV_BIDI_DIR_RTL && chk)) {
        knob_area.x1 = obj->coords.x1 + bg_left;
        knob_area.x2 = knob_area.x1 + knob_size;
    }
    else {
        knob_area.x2 = obj->coords.x2 - bg_right;
        knob_area.x1 = knob_area.x2 - knob_size;
    }

    knob_area.y1 = obj->coords.y1 + bg_top;
    knob_area.y2 = obj->coords.y2 - bg_bottom;

    lv_coord_t knob_left = lv_obj_get_style_pad_left(obj, LV_PART_KNOB);
    lv_coord_t knob_right = lv_obj_get_style_pad_right(obj, LV_PART_KNOB);
    lv_coord_t knob_top = lv_obj_get_style_pad_top(obj, LV_PART_KNOB);
    lv_coord_t knob_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

    /*Apply the paddings on the knob area*/
    knob_area.x1 -= knob_left;
    knob_area.x2 += knob_right;
    knob_area.y1 -= knob_top;
    knob_area.y2 += knob_bottom;

    lv_draw_rect_dsc_t knob_rect_dsc;
    lv_draw_rect_dsc_init(&knob_rect_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_KNOB, &knob_rect_dsc);

    lv_draw_rect(&knob_area, clip_area, &knob_rect_dsc);

}

#endif
