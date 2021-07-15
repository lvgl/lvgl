/**
 * @file lv_spinner.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_spinner.h"
#if LV_USE_SPINNER

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_spinner_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void arc_anim_start_angle(void * obj, int32_t v);
static void arc_anim_end_angle(void * obj, int32_t v);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_spinner_class = {
        .base_class = &lv_arc_class,
        .constructor_cb = lv_spinner_constructor
};

static uint32_t time_param;
static uint32_t arc_length_param;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a spinner object
 * @param par pointer to an object, it will be the parent of the new spinner
 * @return pointer to the created spinner
 */
lv_obj_t * lv_spinner_create(lv_obj_t * parent, uint32_t time, uint32_t arc_length)
{
    time_param = time;
    arc_length_param = arc_length;

    lv_obj_t * obj = lv_obj_class_create_obj(&lv_spinner_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_spinner_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_TRACE_OBJ_CREATE("begin");

    LV_UNUSED(class_p);

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, arc_anim_end_angle);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_time(&a, time_param);
    lv_anim_set_values(&a, arc_length_param, 360 + arc_length_param);
    lv_anim_start(&a);

    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_values(&a, 0, 360);
    lv_anim_set_exec_cb(&a, arc_anim_start_angle);
    lv_anim_start(&a);

    lv_arc_set_bg_angles(obj, 0, 360);
    lv_arc_set_rotation(obj, 270);
}


static void arc_anim_start_angle(void * obj, int32_t v)
{
    lv_arc_set_start_angle(obj, (uint16_t) v);
}


static void arc_anim_end_angle(void * obj, int32_t v)
{
    lv_arc_set_end_angle(obj, (uint16_t) v);
}

#endif /*LV_USE_SPINNER*/
