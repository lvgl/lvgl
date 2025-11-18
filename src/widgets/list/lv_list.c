/**
 * @file lv_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj_class_private.h"
#include "lv_list.h"
#include "../../layouts/flex/lv_flex.h"
#include "../../display/lv_display.h"
#include "../label/lv_label.h"
#include "../image/lv_image.h"
#include "../button/lv_button.h"

#if LV_USE_LIST

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS        (&lv_list_class)
#define MY_CLASS_BUTTON (&lv_list_button_class)
#define MY_CLASS_TEXT   (&lv_list_text_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
const lv_obj_class_t lv_list_class = {
    .base_class = &lv_obj_class,
    .width_def = (LV_DPI_DEF * 3) / 2,
    .height_def = LV_DPI_DEF * 2,
    .name = "lv_list",
};

const lv_obj_class_t lv_list_button_class = {
    .base_class = &lv_button_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .name = "lv_list_button",
};

const lv_obj_class_t lv_list_text_class = {
    .base_class = &lv_label_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .name = "lv_list_text",
};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_list_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    return obj;
}

lv_obj_t * lv_list_add_text(lv_obj_t * list, const char * txt)
{
    LV_LOG_INFO("begin");

    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_TEXT, list);
    lv_obj_class_init_obj(obj);
    lv_label_set_text(obj, txt);
    return obj;
}

lv_obj_t * lv_list_add_button(lv_obj_t * list, const void * icon, const char * txt)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_BUTTON, list);
    lv_obj_class_init_obj(obj);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);

#if LV_USE_IMAGE == 1
    if(icon) {
        lv_obj_t * img = lv_image_create(obj);
        lv_image_set_src(img, icon);
    }
#endif

    if(txt) {
        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    return obj;
}

const char * lv_list_get_button_text(lv_obj_t * list, lv_obj_t * btn)
{
    LV_UNUSED(list);
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_count(btn); i++) {
        lv_obj_t * child = lv_obj_get_child(btn, i);
        if(lv_obj_check_type(child, &lv_label_class)) {
            return lv_label_get_text(child);
        }

    }

    return "";
}

void lv_list_set_button_text(lv_obj_t * list, lv_obj_t * btn, const char * txt)
{
    LV_UNUSED(list);
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_count(btn); i++) {
        lv_obj_t * child = lv_obj_get_child(btn, i);
        if(lv_obj_check_type(child, &lv_label_class)) {
            lv_label_set_text(child, txt);
            return;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_LIST*/
