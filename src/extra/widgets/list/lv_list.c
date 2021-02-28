/**
 * @file lv_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_list.h"
#include "../../../lv_core/lv_disp.h"
#include "../../../lv_widgets/lv_label.h"
#include "../../../lv_widgets/lv_img.h"
#include "../../../lv_widgets/lv_btn.h"

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
    lv_obj_t * list = lv_obj_create_from_class(&lv_list_class, parent, NULL);
    lv_obj_set_size(list, LV_DPX(200), LV_DPX(300));
    lv_obj_set_layout(list, &lv_flex_stacked);

    return list;
}

lv_obj_t * lv_list_add_text(lv_obj_t * list, const char * txt)
{
    lv_obj_t * label = lv_obj_create_from_class(&lv_list_text_class, list, NULL);
    lv_label_set_text(label, txt);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(label, LV_SIZE_PCT(100));
    return label;
}

lv_obj_t * lv_list_add_btn(lv_obj_t * list, const char * icon, const char * txt, lv_event_cb_t event_cb)
{
    lv_obj_t * btn = lv_obj_create_from_class(&lv_list_btn_class, list, NULL);
    lv_obj_set_width(btn, LV_SIZE_PCT(100));
    lv_obj_add_event_cb(btn, event_cb, NULL);
    lv_obj_set_layout(btn, &lv_flex_inline);

    if(icon) {
        lv_obj_t * img = lv_img_create(btn, NULL);
        lv_img_set_src(img, icon);
    }

    if(txt) {
        lv_obj_t * label = lv_label_create(btn, NULL);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    return btn;
}

const char * lv_list_get_btn_text(lv_obj_t * list, lv_obj_t * btn)
{
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
