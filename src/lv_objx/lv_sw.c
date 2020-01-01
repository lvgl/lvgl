/**
 * @file lv_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_sw.h"

#if LV_USE_SW != 0

/*Testing of dependencies*/
#if LV_USE_SLIDER == 0
#error "lv_sw: lv_slider is required. Enable it in lv_conf.h (LV_USE_SLIDER  1) "
#endif

#include "../lv_core/lv_debug.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_math.h"
#include "../lv_core/lv_indev.h"
#include "lv_img.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_sw"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_sw_signal(lv_obj_t * sw, lv_signal_t sign, void * param);
static lv_style_dsc_t * lv_sw_get_style(lv_obj_t * sw, uint8_t part);
static lv_style_dsc_t * lv_sw_get_style(lv_obj_t * sw, uint8_t part);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;

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
lv_obj_t * lv_sw_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("switch create started");

    /*Create the ancestor of switch*/
    lv_obj_t * new_sw = lv_slider_create(par, copy);
    LV_ASSERT_MEM(new_sw);

    if(new_sw == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_sw);

    /*Allocate the switch type specific extended data*/
    lv_sw_ext_t * ext = lv_obj_allocate_ext_attr(new_sw, sizeof(lv_sw_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(new_sw);
        return NULL;
    }

    /*Initialize the allocated 'ext' */
    ext->changed = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_sw, lv_sw_signal);

    /*Init the new switch switch*/
    if(copy == NULL) {
        lv_obj_set_click(new_sw, true);
        lv_obj_set_protect(new_sw, LV_PROTECT_PRESS_LOST);
        lv_obj_set_size(new_sw, 2 * LV_DPI / 3, LV_DPI / 3);
        lv_slider_set_range(new_sw, 0, 1);

        _ot(new_sw, LV_SW_PART_KNOB, SW_KNOB);

    }
    /*Copy an existing switch*/
    else {
//        lv_sw_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
//        ext->style_knob_off    = copy_ext->style_knob_off;
//        ext->style_knob_on     = copy_ext->style_knob_on;
    }

    /*Refresh the style with new signal function*/

    LV_LOG_INFO("switch created");

    return new_sw;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Turn ON the switch
 * @param sw pointer to a switch objec
 * @param anim LV_ANOM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_sw_on(lv_obj_t * sw, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);
    ext->state = 1;
    lv_slider_set_value(sw, 1, anim);
    lv_obj_set_state(sw, LV_OBJ_STATE_CHECKED);
}

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_sw_off(lv_obj_t * sw, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif
    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);
    ext->state = 0;
    lv_bar_set_value(sw, 0, anim);
    lv_obj_clear_state(sw, LV_OBJ_STATE_CHECKED);
}

/**
 * Toggle the position of the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 * @return resulting state of the switch.
 */
bool lv_sw_toggle(lv_obj_t * sw, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif

    bool state = lv_sw_get_state(sw);
    if(state)
        lv_sw_off(sw, anim);
    else
        lv_sw_on(sw, anim);

    return !state;
}

/*=====================
 * Getter functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the switch
 * @param sw pointer to a switch object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_sw_signal(lv_obj_t * sw, lv_signal_t sign, void * param)
{
    lv_res_t res;

    if(sign == LV_SIGNAL_GET_STYLE) {
        uint8_t ** type_p = param;
        lv_style_dsc_t ** style_dsc_p = param;
        *style_dsc_p = lv_sw_get_style(sw, **type_p);
        return LV_RES_OK;
    }

    if(sign == LV_SIGNAL_GET_TYPE) {
        res = ancestor_signal(sw, sign, param);
        if(res != LV_RES_OK) return res;
        return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);
    }

    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);

    /* Include the ancient signal function */
    res = ancestor_signal(sw, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_PRESSED) {
        ext->changed = 0;
    } else if(sign == LV_SIGNAL_PRESSING) {
        if(ext->state != ext->slider.bar.cur_value) ext->changed = 1;
    } else if(sign == LV_SIGNAL_PRESS_LOST) {
        if(lv_sw_get_state(sw)) lv_sw_on(sw, LV_ANIM_ON);
        else lv_sw_off(sw, LV_ANIM_ON);
    } else if(sign == LV_SIGNAL_RELEASED) {
        /*If not dragged then toggle the switch*/
        if(ext->changed == 0) {
            if(lv_sw_get_state(sw)) {
                lv_sw_off(sw, LV_ANIM_ON);
            } else {
                lv_sw_on(sw, LV_ANIM_ON);
            }
        }
        /*If the switch was dragged then calculate the new state based on the current position*/
        else {
            if(ext->slider.bar.cur_value != 0) lv_sw_on(sw, LV_ANIM_ON);
            else lv_sw_off(sw, LV_ANIM_ON);
        }

        res = lv_event_send(sw, LV_EVENT_VALUE_CHANGED, NULL);
        if(res != LV_RES_OK) return res;

    } else if(sign == LV_SIGNAL_CONTROL) {
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) lv_sw_on(sw, LV_ANIM_ON);
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) lv_sw_off(sw, LV_ANIM_ON);

        res   = lv_event_send(sw, LV_EVENT_VALUE_CHANGED, NULL);
        if(res != LV_RES_OK) return res;
    }
    else if(sign == LV_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = false; /*The ancestor slider is editable the switch is not*/
    }

    return res;
}

static lv_style_dsc_t * lv_sw_get_style(lv_obj_t * sw, uint8_t part)
{
    LV_ASSERT_OBJ(sw, LV_OBJX_NAME);

    lv_sw_ext_t * ext = lv_obj_get_ext_attr(sw);
    lv_style_dsc_t * style_dsc_p;

    switch(part) {
    case LV_SW_PART_BG:
        style_dsc_p = &sw->style_dsc;
        break;
    case LV_SW_PART_INDIC:
        style_dsc_p = &ext->slider.bar.style_indic;
        break;
    case LV_SW_PART_KNOB:
        style_dsc_p = &ext->slider.style_knob;
        break;
    default:
        style_dsc_p = NULL;
    }

    return style_dsc_p;
}
#endif
