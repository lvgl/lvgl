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
#include "../core/lv_obj.h"

#if LV_USE_CHECKBOX != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
    const char * txt;
    uint32_t static_txt : 1;
} lv_checkbox_t;

extern const lv_obj_class_t lv_checkbox_class;

/**
 * `type` field in `lv_obj_draw_part_dsc_t` if `class_p = lv_checkbox_class`
 * Used in `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END`
 */
typedef enum {
    LV_CHECKBOX_DRAW_PART_BOX,    /**< The tick box*/
} lv_checkbox_draw_part_type_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a check box object
 * @param parent    pointer to an object, it will be the parent of the new button
 * @return          pointer to the created check box
 */
lv_obj_t * lv_checkbox_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a check box. `txt` will be copied and may be deallocated
 * after this function returns.
 * @param cb    pointer to a check box
 * @param txt   the text of the check box. NULL to refresh with the current text.
 */
void lv_checkbox_set_text(lv_obj_t * obj, const char * txt);

/**
 * Set the text of a check box. `txt` must not be deallocated during the life
 * of this checkbox.
 * @param cb    pointer to a check box
 * @param txt   the text of the check box.
 */
void lv_checkbox_set_text_static(lv_obj_t * obj, const char * txt);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param cb    pointer to check box object
 * @return      pointer to the text of the check box
 */
const char * lv_checkbox_get_text(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CHECKBOX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CHECKBOX_H*/
