/**
 * @file lv_sw.h
 *
 */

#ifndef LV_SWITCH_H
#define LV_SWITCH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_SWITCH != 0

/*Testing of dependencies*/
#if LV_USE_SLIDER == 0
#error "lv_switch: lv_slider is required. Enable it in lv_conf.h (LV_USE_SLIDER  1)"
#endif

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


LV_CLASS_DECLARE_START(lv_switch, lv_obj);

#define _lv_switch_constructor   void (*constructor)(struct _lv_obj_t * obj, struct _lv_obj_t * parent, const struct _lv_obj_t * copy)

#define _lv_switch_data             \
  _lv_obj_data                  \
  lv_style_list_t style_indic;  \
  lv_style_list_t style_knob;

#define _lv_switch_class_dsc        \
  _lv_obj_class_dsc              \

LV_CLASS_DECLARE_END(lv_switch, lv_obj);

extern lv_switch_class_t lv_switch;

/**
 * Switch parts.
 */
enum {
    LV_SWITCH_PART_MAIN = LV_OBJ_PART_MAIN,                 /**< Switch background. */
    LV_SWITCH_PART_INDIC,           /**< Switch fill area. */
    LV_SWITCH_PART_KNOB,    /**< Switch knob. */
    _LV_SWITCH_PART_VIRTUAL_LAST
};

typedef uint8_t lv_switch_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a switch objects
 * @param parent pointer to an object, it will be the parent of the new switch
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other switch to copy.
 * @return pointer to the created switch
 */
lv_obj_t * lv_switch_create(lv_obj_t * parent, const lv_obj_t * copy);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SWITCH*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_SWITCH_H*/
