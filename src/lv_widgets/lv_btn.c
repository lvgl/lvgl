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
#define LV_OBJX_NAME "lv_btn"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_btn_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_btn_destructor(lv_obj_t * obj);
static lv_draw_res_t lv_btn_draw(lv_obj_t * bar, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_btn_signal(lv_obj_t * bar, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_btn  = {
    .constructor = lv_btn_constructor,
    .destructor = lv_btn_destructor,
    .signal_cb = lv_btn_signal,
    .draw_cb = lv_btn_draw,
    .instance_size = sizeof(lv_btn_t),
    .base_class = &lv_obj
};


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a button object
 * @param parent pointer to an object, it will be the parent of the new button
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other button to copy.
 * @return pointer to the created button
 */
lv_obj_t * lv_btn_create(lv_obj_t * parent, const lv_obj_t * copy)
{
     LV_LOG_TRACE("button create started");
     return lv_obj_create_from_class(&lv_btn, parent, copy);
}

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_btn_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("lv_btn create started");

    lv_obj_construct_base(obj, parent, copy);

    lv_obj_set_size(obj, LV_DPI, LV_DPI / 3);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    LV_LOG_INFO("btn created");
}

static void lv_btn_destructor(lv_obj_t * obj)
{
//    lv_bar_t * bar = obj;
//
//    _lv_obj_reset_style_list_no_refr(obj, LV_BAR_PART_INDIC);
//#if LV_USE_ANIMATION
//    lv_anim_del(&bar->cur_value_anim, NULL);
//    lv_anim_del(&bar->start_value_anim, NULL);
//#endif

//    lv_btn.base_p->destructor(obj);
}
static lv_draw_res_t lv_btn_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    return lv_obj.draw_cb(obj, clip_area, mode);
}

static lv_res_t lv_btn_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    return lv_obj.signal_cb(obj, sign, param);
}
#endif
