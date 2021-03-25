/**
 * @file lv_btn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_btn.h"
#if LV_USE_BTN != 0

#include "../extra/layouts/flex/lv_flex.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_btn_constructor(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_btn_class  = {
    .constructor_cb = lv_btn_constructor,
    .instance_size = sizeof(lv_btn_t),
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_btn_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin")
     return lv_obj_create_from_class(&lv_btn_class, parent);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_btn_constructor(lv_obj_t * obj)
{
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_set_size(obj, LV_DPI_DEF, LV_DPI_DEF / 3);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    LV_TRACE_OBJ_CREATE("finished");
}

#endif
