/**
 * @file lv_radiobtn.h
 *
 */

#ifndef LV_RADIOBTN_H
#define LV_RADIOBTN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"
#include "../../layouts/flex/lv_flex.h"
#include "../../lv_conf_internal.h"

#if LV_USE_RADIOBTN

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t obj;
    char * checked_txt;
    uint32_t active_index;
} lv_radiobtn_t;

typedef struct {
    lv_obj_t obj;
    char * txt;
    uint32_t static_txt : 1;
} lv_radiobtn_item_t;

extern const lv_obj_class_t lv_radiobtn_class;
extern const lv_obj_class_t lv_radiobtn_item_class;

/**
 * `type` field in `lv_obj_draw_part_dsc_t` if `class_p = lv_radiobtn_item_class`
 * Used in `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END`
 */
typedef enum {
    LV_RADIOBTN_DRAW_PART_BOX,    /**< The tick box*/
    LV_RADIOBTN_DRAW_PART_BOX_INNER, /**< The inner box of tich box*/
} lv_radiobtn_draw_part_type_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a radio button object
 * @param parent    pointer to an object, it will be the parent of radio button
 * @return          pointer to the created radio button object
 */
lv_obj_t * lv_radiobtn_create(lv_obj_t * parent);

/**
 * Create a radiobtn item object like check box
 * @param parent    pointer to an object, it will be the parent of the new button
 * @return          pointer to the created radiobtn item
 */
lv_obj_t * lv_radiobtn_create_item(lv_obj_t * parent);

/**
 * Add new radiobtn item into radio button with text
 * @param radiobtn  pointer to an object, it will be the parent of new radiobtn item
 * @param txt       the text of new radiobtn item
 * @return          pointer to created new radiobtn item
 */
lv_obj_t * lv_radiobtn_add_item(lv_obj_t * radiobtn, const char * txt);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set new text of a radiobtn item
 * @param obj       pointer to an object, it is a radiobtn item to set text
 * @param txt       the new text that you want to set for radiobtn item
 * @return          pointer to chenged radiobtn item
 */
void lv_radiobtn_set_item_text(lv_obj_t * obj, const char * txt);

/**
 * Set the text of a radiobtn item. `txt` must not be deallocated during the life
 * of this radiobtn.
 * @param obj       pointer to an object, it is a radiobtn item to set static text
 * @param txt       the static text of the radiobtn item.
 */
void lv_radiobtn_set_item_text_static(lv_obj_t * obj, const char * txt);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get radiobtn item from radio bytton by index
 * @param radiobtn  pointer to an object, it's parent of the wanted radiobtn item
 * @param index     the index of radiobtn item in radio button
 * @return          pointer to the wanted radiobtn item
 */
lv_obj_t * lv_radiobtn_get_item(lv_obj_t * radiobtn, uint32_t index);

/**
 * Get the text of a radiobtn item
 * @param radiobtn  pointer to an object, it is the parent of radiobtn item
 * @param obj      pointer to an object, it is a radiobtn item to get text
 * @return          the text of specified radiobtn item
 */
const char * lv_radiobtn_get_item_text(lv_obj_t * radiobtn, lv_obj_t * obj);

/**
 * Get the item number of a radio button
 * @param radiobtn  pointer to an object, it is a radio button to get item number
 * @return          the item number of radio button
 */
uint32_t lv_radiobtn_get_item_num(lv_obj_t * radiobtn);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RADIOBTN*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_RADIOBTN_H*/
