/**
 * @file lv_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_list.h"
#include "../../layouts/flex/lv_flex.h"
#include "../../core/lv_disp.h"
#include "../label/lv_label.h"
#include "../img/lv_img.h"
#include "../btn/lv_btn.h"

#if LV_USE_LIST

/*********************
 *      DEFINES
 *********************/
#define MV_CLASS &lv_list

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

const lv_obj_class_t lv_list_class = {
    .base_class = &lv_obj_class,
    .width_def = (LV_DPI_DEF * 3) / 2,
    .height_def = LV_DPI_DEF * 2
};

const lv_obj_class_t lv_list_btn_class = {
    .base_class = &lv_btn_class,
};

const lv_obj_class_t lv_list_text_class = {
    .base_class = &lv_label_class,
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
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_list_class, parent);
    lv_obj_class_init_obj(obj);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    return obj;
}

lv_obj_t * lv_list_add_text(lv_obj_t * list, const char * txt)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_list_text_class, list);
    lv_obj_class_init_obj(obj);
    lv_label_set_text(obj, txt);
    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(obj, LV_PCT(100));
    return obj;
}

lv_obj_t * lv_list_add_btn(lv_obj_t * list, const void * icon, const char * txt)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_list_btn_class, list);
    lv_obj_class_init_obj(obj);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);

#if LV_USE_IMG == 1
    if(icon) {
        lv_obj_t * img = lv_img_create(obj);
        lv_img_set_src(img, icon);
    }
#endif

    if(txt) {
        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    return obj;
}

const char * lv_list_get_btn_text(lv_obj_t * list, lv_obj_t * btn)
{
    LV_UNUSED(list);
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(btn); i++) {
        lv_obj_t * child = lv_obj_get_child(btn, i);
        if(lv_obj_check_type(child, &lv_label_class)) {
            return lv_label_get_text(child);
        }

    }

    return "";
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_LIST*/
