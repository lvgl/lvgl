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

#include "../lv_misc/lv_debug.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_math.h"
#include "../lv_core/lv_indev.h"
#include "../lv_core/lv_disp.h"
#include "lv_img.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_switch"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_switch_signal(lv_obj_t * sw, lv_signal_t sign, void * param);
static lv_design_res_t lv_switch_design(lv_obj_t * sw, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_style_list_t * lv_switch_get_style(lv_obj_t * sw, uint8_t part);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a switch objects
 * @param par pointer to an object, it will be the parent of the new switch
 * @param copy pointer to a switch object, if not NULL then the new object will be copied from it
 * @return pointer to the created switch
 */
lv_obj_t * lv_switch_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("switch create started");

    /*Create the ancestor of switch*/
    lv_obj_t * sw = lv_bar_create(par, copy);
    LV_ASSERT_MEM(sw);

    if(sw == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(sw);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(sw);

    /*Allocate the switch type specific extended data*/
    lv_switch_ext_t * ext = lv_obj_allocate_ext_attr(sw, sizeof(lv_switch_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(sw);
        return NULL;
    }

    lv_style_list_init(&ext->style_knob);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(sw, lv_switch_signal);
    lv_obj_set_design_cb(sw, lv_switch_design);

    /*Init the new switch switch*/
    if(copy == NULL) {
        lv_obj_set_click(sw, true);
        lv_obj_add_protect(sw, LV_PROTECT_PRESS_LOST);
        lv_obj_set_size(sw, LV_DPX(60), LV_DPX(35));
        lv_bar_set_range(sw, 0, 1);

        lv_theme_apply(sw, LV_THEME_SWITCH);
    }
    /*Copy an existing switch*/
    else {
        lv_switch_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        lv_style_list_copy(&ext->style_knob, &copy_ext->style_knob);
        lv_obj_refresh_style(sw, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    /*Refresh the style with new signal function*/

    LV_LOG_INFO("switch created");

    return sw;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Turn ON the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_switch_on(lv_obj_t * sw, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif
    if(lv_bar_get_value(sw) == 1)
        return;
    lv_bar_set_value(sw, 1, anim);
    lv_obj_add_state(sw, LV_STATE_CHECKED);
}

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_switch_off(lv_obj_t * sw, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif
    if(lv_bar_get_value(sw) == 0)
        return;
    lv_bar_set_value(sw, 0, anim);
    lv_obj_clear_state(sw, LV_STATE_CHECKED);
}

/**
 * Toggle the position of the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 * @return resulting state of the switch.
 */
bool lv_switch_toggle(lv_obj_t * sw, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif

    bool state = lv_switch_get_state(sw);
    if(state)
        lv_switch_off(sw, anim);
    else
        lv_switch_on(sw, anim);

    return !state;
}

/*=====================
 * Getter functions
 *====================*/

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
static lv_design_res_t lv_switch_design(lv_obj_t * sw, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return LV_DESIGN_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_bidi_dir_t base_dir = lv_obj_get_base_dir(sw);

        /*The ancestor design function will draw the background and the indicator.
         * It also sets ext->bar.indic_area*/
        ancestor_design(sw, clip_area, mode);

        lv_switch_ext_t * ext = lv_obj_get_ext_attr(sw);

        lv_coord_t objw = lv_obj_get_width(sw);
        lv_coord_t objh = lv_obj_get_height(sw);
        lv_coord_t knob_size = objh;
        lv_area_t knob_area;

        lv_style_int_t bg_left = lv_obj_get_style_pad_left(sw,   LV_SWITCH_PART_BG);
        lv_style_int_t bg_right = lv_obj_get_style_pad_right(sw,  LV_SWITCH_PART_BG);

        lv_coord_t max_indic_w = objw - bg_left - bg_right;
        lv_coord_t act_indic_w = lv_area_get_width(&ext->bar.indic_area);
        if(base_dir != LV_BIDI_DIR_RTL) {
            knob_area.x1 = ext->bar.indic_area.x2 - ((act_indic_w * knob_size) / max_indic_w);
            knob_area.x2 = knob_area.x1 + knob_size;
        }
        else {
            knob_area.x2 = ext->bar.indic_area.x1 + ((act_indic_w * knob_size) / max_indic_w);
            knob_area.x1 = knob_area.x2 - knob_size;
        }

        knob_area.y1 = sw->coords.y1;
        knob_area.y2 = sw->coords.y2;

        lv_style_int_t knob_left = lv_obj_get_style_pad_left(sw,   LV_SWITCH_PART_KNOB);
        lv_style_int_t knob_right = lv_obj_get_style_pad_right(sw,  LV_SWITCH_PART_KNOB);
        lv_style_int_t knob_top = lv_obj_get_style_pad_top(sw,    LV_SWITCH_PART_KNOB);
        lv_style_int_t knob_bottom = lv_obj_get_style_pad_bottom(sw, LV_SWITCH_PART_KNOB);

        /*Apply the paddings on the knob area*/
        knob_area.x1 -= knob_left;
        knob_area.x2 += knob_right;
        knob_area.y1 -= knob_top;
        knob_area.y2 += knob_bottom;

        lv_draw_rect_dsc_t knob_rect_dsc;
        lv_draw_rect_dsc_init(&knob_rect_dsc);
        lv_obj_init_draw_rect_dsc(sw, LV_SWITCH_PART_KNOB, &knob_rect_dsc);

        lv_draw_rect(&knob_area, clip_area, &knob_rect_dsc);

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        return ancestor_design(sw, clip_area, mode);
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the switch
 * @param sw pointer to a switch object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_switch_signal(lv_obj_t * sw, lv_signal_t sign, void * param)
{
    lv_res_t res;

    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_switch_get_style(sw, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(sw, sign, param);
    }

    if(sign == LV_SIGNAL_GET_TYPE) {
        res = ancestor_signal(sw, sign, param);
        if(res != LV_RES_OK) return res;
        return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);
    }

    /* Include the ancient signal function */
    res = ancestor_signal(sw, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_CLEANUP) {
        lv_obj_clean_style_list(sw, LV_SWITCH_PART_KNOB);
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        if(lv_switch_get_state(sw)) lv_switch_off(sw, LV_ANIM_ON);
        else lv_switch_on(sw, LV_ANIM_ON);

        res = lv_event_send(sw, LV_EVENT_VALUE_CHANGED, NULL);
        if(res != LV_RES_OK) return res;

    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) lv_switch_on(sw, LV_ANIM_ON);
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) lv_switch_off(sw, LV_ANIM_ON);

        res   = lv_event_send(sw, LV_EVENT_VALUE_CHANGED, NULL);
        if(res != LV_RES_OK) return res;
#endif
    }
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        lv_style_int_t knob_left = lv_obj_get_style_pad_left(sw,   LV_SWITCH_PART_KNOB);
        lv_style_int_t knob_right = lv_obj_get_style_pad_right(sw,  LV_SWITCH_PART_KNOB);
        lv_style_int_t knob_top = lv_obj_get_style_pad_top(sw,    LV_SWITCH_PART_KNOB);
        lv_style_int_t knob_bottom = lv_obj_get_style_pad_bottom(sw, LV_SWITCH_PART_KNOB);

        /* The smaller size is the knob diameter*/
        lv_coord_t knob_size = LV_MATH_MIN(lv_obj_get_width(sw), lv_obj_get_height(sw)) >> 1;
        knob_size += LV_MATH_MAX(LV_MATH_MAX(knob_left, knob_right), LV_MATH_MAX(knob_bottom, knob_top));
        knob_size += 2;         /*For rounding error*/

        knob_size += lv_obj_get_draw_rect_ext_pad_size(sw, LV_SWITCH_PART_KNOB);

        /*Indic. size is handled by bar*/
        sw->ext_draw_pad = LV_MATH_MAX(sw->ext_draw_pad, knob_size);
    }
    else if(sign == LV_SIGNAL_GET_EDITABLE) {
#if LV_USE_GROUP
        bool * editable = (bool *)param;
        *editable       = false; /*The ancestor slider is editable the switch is not*/
#endif
    }

    return res;
}

static lv_style_list_t * lv_switch_get_style(lv_obj_t * sw, uint8_t part)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

    lv_switch_ext_t * ext = lv_obj_get_ext_attr(sw);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_SWITCH_PART_BG:
            style_dsc_p = &sw->style_list;
            break;
        case LV_SWITCH_PART_INDIC:
            style_dsc_p = &ext->bar.style_indic;
            break;
        case LV_SWITCH_PART_KNOB:
            style_dsc_p = &ext->style_knob;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}
#endif
