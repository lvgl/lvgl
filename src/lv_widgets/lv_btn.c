/**
 * @file lv_btn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_btn.h"
#if LV_USE_BTN != 0

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_btn

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_btn_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_btn  = {
    .constructor_cb = lv_btn_constructor,
    .instance_size = sizeof(lv_btn_t),
    .base_class = &lv_obj
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_btn_create(lv_obj_t * parent, const lv_obj_t * copy)
{
     return lv_obj_create_from_class(&lv_btn, parent, copy);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_btn_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("lv_btn create started");

    lv_obj_set_size(obj, LV_DPI_DEF, LV_DPI_DEF / 3);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    LV_LOG_INFO("btn created");
}

#endif
