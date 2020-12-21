/**
 * @file lv_cb.h
 *
 */

#ifndef LV_CHECKBOX_H
#define LV_CHECKBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "../lv_core/lv_obj.h"

#if LV_USE_CHECKBOX != 0

/*Testing of dependencies*/
#if LV_USE_LABEL == 0
#error "lv_cb: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


LV_CLASS_DECLARE_START(lv_checkbox, lv_obj);

#define _lv_checkbox_constructor   void (*constructor)(struct _lv_obj_t * obj, struct _lv_obj_t * parent, const struct _lv_obj_t * copy)

#define _lv_checkbox_data             \
  _lv_obj_data                        \
    char * txt;                       \
    uint32_t static_txt :1;

#define _lv_checkbox_class_dsc        \
  _lv_obj_class_dsc

LV_CLASS_DECLARE_END(lv_checkbox, lv_obj);

extern lv_checkbox_class_t lv_checkbox;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a check box objects
 * @param par pointer to an object, it will be the parent of the new check box
 * @param copy pointer to a check box object, if not NULL then the new object will be copied from it
 * @return pointer to the created check box
 */
lv_obj_t * lv_checkbox_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a check box. `txt` will be copied and may be deallocated
 * after this function returns.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void lv_checkbox_set_text(lv_obj_t * cb, const char * txt);

/**
 * Set the text of a check box. `txt` must not be deallocated during the life
 * of this checkbox.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void lv_checkbox_set_text_static(lv_obj_t * cb, const char * txt);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param cb pointer to check box object
 * @return pointer to the text of the check box
 */
const char * lv_checkbox_get_text(const lv_obj_t * cb);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CHECKBOX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CHECKBOX_H*/
